  #define BOOST_TEST_MODULE shared_ptr_tests

#include <exception>
#include "shared_ptr.hpp"
#include <boost/test/unit_test.hpp>
#include <exception>
#include <cstdlib>
#include <new>

struct FailNew
{
    FailNew()
    {
        failNew = true;
    }
    ~FailNew()
    {
        failNew = false;
    }
    static bool failNew;
};

bool FailNew::failNew = false;

void* operator new(std::size_t sz)
{
    if (not FailNew::failNew)
    {
        if (sz == 0)
            ++sz; // avoid std::malloc(0) which may return nullptr on success

        if (void* ptr = std::malloc(sz))
            return ptr;
    }
    throw std::bad_alloc{};
}

class TestObject
{
public:
    TestObject(bool* b) : destroyed(b)
    {
        *destroyed = false;
    }
    ~TestObject()
    {
        *destroyed = true;
    }
    bool operator==(const TestObject& other) const
    {
        return destroyed == other.destroyed;
    }
    bool* destroyed;
};

BOOST_AUTO_TEST_SUITE( shared_ptr_tests )



BOOST_AUTO_TEST_CASE(SimpleTest)
{
    constexpr int defaultValue = 42;
    auto ptr1 = SharedPointer<int>(new int(defaultValue));
    BOOST_CHECK_EQUAL(*ptr1, defaultValue);
    BOOST_CHECK_EQUAL(ptr1.getCount(), 1);
}

BOOST_AUTO_TEST_CASE(EmptyObject)
{
    auto ptr1 = SharedPointer<int>();
    BOOST_CHECK_EQUAL(ptr1.get(), nullptr);
    BOOST_REQUIRE_NO_THROW(BOOST_CHECK_EQUAL(ptr1.getCount(), 0));
}

BOOST_AUTO_TEST_CASE(CopyConstructor)
{
    bool destroyed1 = false;
    SharedPointer<TestObject> ptr1(new TestObject(&destroyed1));
    {
        SharedPointer<TestObject> ptr2(ptr1);
        BOOST_CHECK_EQUAL(ptr1 == ptr2, true);
        BOOST_CHECK_EQUAL(ptr1.getCount(), 2);
    }
    BOOST_CHECK_EQUAL(destroyed1, false);
}

BOOST_AUTO_TEST_CASE(MoveConstructor)
{
    bool destroyed1 = false;
    SharedPointer<TestObject> ptr1(new TestObject(&destroyed1));
    SharedPointer<TestObject> ptr2 = std::move(ptr1);

    BOOST_CHECK_EQUAL((*ptr2).destroyed, &destroyed1);
    BOOST_CHECK_EQUAL(ptr1.get(), nullptr);
    BOOST_CHECK_EQUAL(ptr1.getCount(), 0);
    BOOST_CHECK_EQUAL(ptr2.getCount(), 1);
}

BOOST_AUTO_TEST_CASE(ReferenceCountDecremented)
{
    bool destroyed1 = false;
    SharedPointer<TestObject> ptr1(new TestObject(&destroyed1));
    {
        SharedPointer<TestObject> ptr2(ptr1);
        BOOST_CHECK_EQUAL(ptr1.getCount(), 2);
    }
    BOOST_CHECK_EQUAL(ptr1.getCount(), 1);
    BOOST_CHECK_EQUAL(destroyed1, false);
}

BOOST_AUTO_TEST_CASE(ReferenceCountIsZero)
{
    bool destroyed = false;
    {
        SharedPointer<TestObject> ptr2(new TestObject(&destroyed));
        BOOST_CHECK_EQUAL(destroyed, false);
    }
    BOOST_CHECK_EQUAL(destroyed, true);
}

BOOST_AUTO_TEST_CASE(CopyAssignment)
{
    bool destroyed1 = false;
    SharedPointer<TestObject> ptr1(new TestObject(&destroyed1));
    bool destroyed2 = false;
    SharedPointer<TestObject> ptr2(new TestObject(&destroyed2));
    ptr1 = ptr2;
    BOOST_CHECK_EQUAL(destroyed1, true);
    BOOST_CHECK_EQUAL(destroyed2, false);
    BOOST_CHECK_EQUAL(ptr1.getCount(), 2);
    BOOST_CHECK_EQUAL(ptr1 == ptr2, true);
}

BOOST_AUTO_TEST_CASE(CopyAssignmentWithSelf)
{
    bool destroyed1 = false;
    SharedPointer<TestObject> ptr1(new TestObject(&destroyed1));
    ptr1 = ptr1;
    BOOST_CHECK_EQUAL(destroyed1, false);
    BOOST_CHECK_EQUAL(ptr1.getCount(), 1);
}

BOOST_AUTO_TEST_CASE(MoveAssignmentWithSelf)
{
    bool destroyed1 = false;
    SharedPointer<TestObject> ptr1(new TestObject(&destroyed1));
    ptr1 = std::move(ptr1);
    BOOST_CHECK_EQUAL(destroyed1, false);
    BOOST_CHECK_EQUAL(ptr1.getCount(), 1);
}

BOOST_AUTO_TEST_CASE(MoveAssignment)
{
    bool destroyed1 = false;
    bool destroyed2 = false;
    SharedPointer<TestObject> ptr1(new TestObject(&destroyed1));
    ptr1 = SharedPointer<TestObject>(new TestObject(&destroyed2));

    BOOST_CHECK_EQUAL(destroyed1, true);
    BOOST_CHECK_EQUAL(destroyed2, false);

    BOOST_CHECK_EQUAL((*ptr1).destroyed == &destroyed2, true);
    BOOST_CHECK_EQUAL(ptr1.getCount(), 1);
}

BOOST_AUTO_TEST_CASE(ExceptionSafe)
{
    bool destroyed = false;
    auto* ptr = new TestObject(&destroyed);
    try
    {
        auto guard = FailNew();
        auto ptr1 = SharedPointer(ptr);
    }
    catch (...)
    {
        BOOST_CHECK_EQUAL(destroyed, true);
    }
    BOOST_CHECK_EQUAL(destroyed, true);
}

BOOST_AUTO_TEST_SUITE_END()

