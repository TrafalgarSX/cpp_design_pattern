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
    virtual void attach(IObserver* observer) = 0;
    virtual void detach(IObserver* observer) = 0;
    virtual void notify() = 0;
};

class IObserver
{
public:
    virtual ~IObserver(){};
    virtual void onNotify(const std::string &message_from_subject) = 0;
};

class Subject : public ISubject
{
public:
    virtual ~Subject(){
        std::cout << "Goodbye, I was the Subject.\n";
    };

    void attach(IObserver* observer) override
    {
        list_observer.emplace_front(observer);
    }

    void detach(IObserver* observer) override
    {
        list_observer.remove_if([observer](const IObserver* weak_observer) {
            return weak_observer == observer;
        });
    }

    void notify() override
    {
        for (const auto &observer : list_observer)
        {
            // raii 版本实际上不需要， 因为expired的observer会在析构的detach中被删除
            observer->onNotify(message);
        }
    }

    void createMessage(std::string message = "Empty")
    {
        this->message = message;
        notify();
    }

private:
    std::forward_list<IObserver*> list_observer;
    std::string message{"Hello from the Subject! raii_version"};
};

class Observer : public IObserver
{
public:

    Observer(const std::shared_ptr<ISubject> &subject) : subject(subject) 
    {
        subject->attach(this);
    }

    virtual ~Observer()
    {
        subject->detach(this);
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
    // ISubject &subject;
    // 可以保证subject不会先于observer析构， 尤其是Observer会在析构中调用subject的detach
    std::shared_ptr<ISubject> subject;
    std::string message_from_subject;
};
