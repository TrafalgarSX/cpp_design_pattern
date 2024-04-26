#pragma once

/**
 * Observer Design Pattern
 *
 * Intent: Lets you define a subscription mechanism to notify multiple objects
 * about any events that happen to the object they're observing.
 *
 * Note that there's a lot of different terms with similar meaning associated
 * with this pattern. Just remember that the Subject is also called the
 * Publisher and the Observer is often called the Subscriber and vice versa.
 * Also the verbs "observe", "listen" or "track" usually mean the same thing.
 */

#include <iostream>
#include <string>
#include <memory>
#include <forward_list>

class IObserver;
class ISubject
{
public:
    virtual ~ISubject(){};
    virtual void attach(const std::shared_ptr<IObserver>& observer) = 0;
    virtual void detach(const std::shared_ptr<IObserver>& observer) = 0;
    virtual void notify() = 0;
};


class IObserver
{
public:
    virtual ~IObserver(){};
    virtual void onNotify(const std::string &message_from_subject) = 0;
};

// https://refactoring.guru/design-patterns/observer/cpp/example
class Subject : public ISubject
{
public:
    virtual ~Subject(){
        std::cout << "Goodbye, I was the Subject.\n";
    };

    void attach(const std::shared_ptr<IObserver> &observer) override
    {
        list_observer.emplace_front(observer);
    }

    void detach(const std::shared_ptr<IObserver> &observer) override
    {
        // test weak_ptr
        list_observer.remove_if([observer](const std::weak_ptr<IObserver> &weak_observer) {
            return weak_observer.expired() || weak_observer.lock() == observer;
        });
    }

    void notify() override
    {
        bool cleanup = false;
        for (const auto &observer : list_observer)
        {
        // if c++17
        #if __cplusplus >= 201703L
        {
            if (auto shared_observer = observer.lock())
            {
                shared_observer->onNotify("Hello from the Subject!");
            }else
            {
                cleanup = true;
            }
        }
        #else
        {
            if (observer.expired())
            {
                // need to remove expired weak_ptr?
                cleanup = true;
                continue;
            }
            auto shared_observer = observer.lock();
            shared_observer->onNotify(message);
        }
        #endif
            if(cleanup)
            {
                list_observer.remove_if([](const std::weak_ptr<IObserver> &weak_observer) {
                    return weak_observer.expired();
                });
            }
        }
    }

    void createMessage(std::string message = "Empty")
    {
        this->message = message;
        notify();
    }

private:
    std::forward_list<std::weak_ptr<IObserver>> list_observer;
    std::string message{"Hello from the Subject!"};
};

class Observer : public IObserver, public std::enable_shared_from_this<Observer>
{
public:
    Observer() = default;

    virtual ~Observer()
    {
        std::cout << "Goodbye, I was the Observer.\n";
    }

    void onNotify(const std::string &message) override
    {
        message_from_subject = message;
        std::cout << "Observer: Reacted to the message: " << message_from_subject << "\n";
    }

    // 似乎没有必要，detach 后 Observer没有必要存在， 所以没有主动调用该函数的必要
    void removeMeFromSubject()
    {
    }


private:
    std::string message_from_subject;
};
