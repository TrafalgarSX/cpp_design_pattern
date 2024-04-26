#include <iostream>
#include <string>
#include <memory>

// #define USE_FUNC

#ifndef USE_FUNC
// #include <Observer_weak_ptr.h>
#include <Observer_raii.h>
#else
#include <Observer_function.h>
#endif

#ifdef USE_FUNC

void func1(int x, const std::string &str)
{
    std::cout << "func1: " << x << ", " << str << std::endl;
}

void func2(int x, const std::string &str)
{
    std::cout << "func2: " << x << ", " << str << std::endl;
}

void test_observer()
{
    Subject<int, const std::string&> subject;
    subject.notify(0, "message zero");
    {
        auto observer1 = subject.makeObserver(func1);
        subject.notify(1, "message one");
        {
            auto observer2 = subject.makeObserver(func2);
            subject.notify(2, "message two");
        }
        subject.notify(3, "message three");

        // equal to unsubscribe
        observer1.reset();
        subject.notify(4, "message four");
    }
    subject.notify(5, "message five");
}
#else

#if 0
// 这里的观察者模式使用了弱指针，当观察者被析构时，会自动从观察者列表中删除
// observer 不保存 Subject的引用， 由subject 主动进行 attach 和 detach
void test_observer_weak_ptr()
{
    Subject subject;
    auto observer1 = std::make_shared<Observer>();
    auto observer2 = std::make_shared<Observer>();
    auto observer3 = std::make_shared<Observer>();

    subject.attach(observer1);
    subject.attach(observer2);
    subject.attach(observer3);
    {
        auto observer4 = std::make_shared<Observer>();
        subject.attach(observer4);
        subject.notify();
        std::cout << "------------------------" << std::endl;
    }

    subject.notify();

    std::cout << "------------------------" << std::endl;

    subject.detach(observer2);

    subject.createMessage("test message");

    // subject.notify();
}
#endif

void test_observer_raii()
{
    auto subject = std::make_shared<Subject>();
    Observer observer1(subject);
    Observer observer2(subject);
    Observer observer3(subject);

    subject->notify();
    std::cout << "------------------------" << std::endl;

    {
        Observer observer4(subject);
        subject->notify();
        std::cout << "------------------------" << std::endl;
    }

    subject->notify();

    std::cout << "------------------------" << std::endl;

    {
        Observer observer4(subject);
        Observer observer5(subject);
        subject->createMessage("new test message");
        std::cout << "------------------------" << std::endl;
    }

}

#endif

int main() 
{
#ifdef USE_FUNC
    test_observer();
#endif

    // test_observer_weak_ptr();
    test_observer_raii();
    return 0;
}
