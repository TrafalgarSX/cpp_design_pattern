#pragma once

#include <iostream>
#include <algorithm>
#include <forward_list>
#include <functional>

/**
 * Simple observer pattern in modern C++ without pointers.
 * Register functions with make_observer() and call notify() to call them.
 * Note: Functions are called from the thread that calls notify().
 */
template <typename... Args>
class Subject
{
public:
    typedef std::shared_ptr<std::function<void(Args...)>> Observer;

    Subject() {}

    void notify(const Args &...args)
    {
        bool cleanup = false;
        for (const auto &observer_weak : m_observers)
        {
            if (const auto &observer_function = observer_weak.lock())
            {
                (*observer_function)(args...);
            }
            else
            {
                // weak pointer expired, do a cleanup round
                cleanup = true;
            }
        }

        if (cleanup)
        {
            m_observers.remove_if([](auto observer) { return observer.expired(); });
        }
    }

    /**
     * Register a function as observer. Keep the returned shared_ptr around as long as you want
     * the function to be called.
     */
    Observer makeObserver(std::function<void(Args...)> observerFunction)
    {
        auto observer = std::make_shared<std::function<void(Args...)>>(observerFunction);
        m_observers.emplace_front(observer);
        return observer;
    }

private:
    std::forward_list<std::weak_ptr<std::function<void(Args...)>>> m_observers;
};