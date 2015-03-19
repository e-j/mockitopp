

# Basics #
The following example shows howto construct a simple mock of an interface, stubbing a _zero_ argument _void_ function.

**Note:** Be sure to always include the .when() and .thenReturn() clauses to provide a complete stubbing. Failure to do so may result in an unexpected crash and/or undesirable behavior.

```
#include <mockitopp/mockitopp.hpp>

// interface
struct Foo {
   virtual void bar() = 0;
};

// test
TEST(BasicExample) {
   mockitopp::mock_object<Foo> mock;
   mock(&Foo::bar).when().thenReturn();
   Foo& fooImpl = mock.getInstance();

   ASSERT_NO_THROW(fooImpl.bar());
}
```

# Returnable Behavior #
This example shows how you can define simple returnable behavior for an object.  You will notice that we chained a bunch of arguments and their return values together in a nice readable format. We then have our associated gtest `ASSERT_?` macros verify the expected behavior. As you can see we make a call to `adderImpl.add(4, 5)`, but we didn't define any behavior for it. What happens in this case is the stub implementation will try to return the default behavior for function's signature.  This means for POD (i.e. `int`, `bool`, etc.) it will return 0, false, 0.0, etc.  For objects it will try to return the object using the default constructor (i.e. `std::string()`, `std::vector()`, etc).  For pointers it will return `NULL`. For reference types it will do a very dangerous cast from a pointer to a primitive to a reference of the return type (which may or may not be desired, SO BE CAREFUL).
```
#include <mockitopp/mockitopp.hpp>

// interface
struct Adder {
   virtual int add(int a, int b) = 0;
};

// test
TEST(ReturnableTest)
{
   mockitopp::mock_object<Adder> mock;
   mock(&Adder::add).when(0, 0).thenReturn(0);
   mock(&Adder::add).when(1, 1).thenReturn(2)
   mock(&Adder::add).when(2, 2).thenReturn(5); // most certainly 2 + 2 = 5!
   Adder& adderImpl = mock.getInstance();

   ASSERT_EQUAL(adderImpl.add(0, 0), 0);
   ASSERT_EQUAL(adderImpl.add(1, 1), 2);
   ASSERT_EQUAL(adderImpl.add(2, 2), 5);
   // What does 4 + 5 return?
   // We didn't define any behavior!!!
   ASSERT_EQUAL(adderImpl.add(4, 5), 0);
}
```

# Throwable Behavior #
This example shows how you can define simple throwable behavior for an object.  Similarly to the returnable action behavior you can instead decide to use `thenThrow(...)` to define a throwable action. AS you may have noticed you can chain together multiple throwable and returnable actions.  This is useful for cases where your mock object is only expected to handle certain argument types.
```
#include <mockitopp/mockitopp.hpp>

// interface
struct Adder {
   virtual int add(int a, int b) = 0;
};

// test
TEST(ThrowableTest) {
   mockitopp::mock_object<Adder> mock;
   mock(&Adder::add).when(0, 0).thenReturn(0)
   mock(&Adder::add).when(1, 1).thenReturn(2)
   mock(&Adder::add).when(2, 2).thenThrow(std::string("I don't support this!")); // Don't believe me? I dare you to try it!
   Adder& adderImpl = mock.getInstance();

   ASSERT_EQUAL(adderImpl.add(0, 0), 0);
   ASSERT_EQUAL(adderImpl.add(1, 1), 2);
   ASSERT_THROW(int result = adderImpl.add(2, 2), std::string);
}
```

# Void Functions #
You can stub functions that return `void` similarly to the way it is done for functions with return values. The different is that the `thenReturn` method takes no arguments, while the `thenThrow` method still operates as expected.

What happens when you don't stub behavior for a given input? In this case it operates as if you stubbed the argument using the `thenReturn` method.
```
#include <mockitopp/mockitopp.hpp>

// interface
struct Foo {
   virtual void bar(int x) = 0;
};

// test
TEST(VoidFunctionTest) {
   mockitopp::mock_object<Foo> mock;
   mock(&Foo::bar).when(0).thenReturn();
   mock(&Foo::bar).when(1).thenReturn();
   mock(&Foo::bar).when(2).thenThrow(std::string("Invalid argument!"));
   Foo& foo = mock.getInstance();

   ASSERT_NO_THROW(foo.bar(0)); // returns cause we stubbed it
   ASSERT_NO_THROW(foo.bar(1)); // returns cause we stubbed it
   ASSERT_NO_THROW(foo.bar(3)); // returns even though we didn't stub it for this argument
   ASSERT_THROW(foo.bar(2), std::string); // throws stubbed exception
}
```

# Iterator Style #
You can stub arguments to return different values on consecutive calls.  This is useful for modeling iterator like behavior for a given argument list.  It even works with a function that takes no arguments.  The configuration is similar to the previous uses except you chain together consecutive calls to the `thenReturn` and `thenThrow` methods from a single call to the `when` method.  Also, the last stubbed `thenReturn` or `thenThrow` becomes the default behavior on consecutive calls after the the last element is hit.
```
#include <mockitopp/mockitopp.hpp>

// interface
struct Enumeration {
   virtual int next() = 0;
};

// test
TEST(IteartorTest)
{
   mockitopp::mock_object<Enumeration> mock;
   mock(&Enumeration::next).when().thenReturn(0)
                                  .thenReturn(1)
                                  .thenThrow(std::string("Invalid argument!"))
                                  .thenReturn(2)
                                  .thenReturn(3);
   Enumeration& e = mock.getInstance();

   ASSERT_EQUAL(0, e.next()); // should return 0
   ASSERT_EQUAL(1, e.next()); // should return 1
   ASSERT_THROW(1 == e.next(), std::string); // should throw an exception
   ASSERT_EQUAL(2, e.next()); // should return 2
   ASSERT_EQUAL(3, e.next()); // should return 3
   ASSERT_EQUAL(3, e.next()); // remaining consecutive calls return the last stubbed value
}
```

# Overloaded Functions #
In some cases an interface may have several overloaded functions. The mocking behavior is similar to the other examples but requires an extra step for resolving the overloaded function. There are currently two ways in which you can resolve the member function pointer: `(1)` define an intermediate variable acting as a proxy for the lookup, or `(2)` use the static\_cast operator to provide the compiler lookup. Unfortunately, both ways are a little bit more verbose but are required for the compiler to resolve the functions properly.
```
#include <mockitopp/mockitopp.hpp>

// interface
struct OverloadedInterface {
   virtual void foo() = 0;
   virtual void foo(int) = 0;
   virtual void foo(std::string) = 0;
};


// Example 1: using variable proxy resolution
TEST(OverloadedFunctionsTest, ByVariableProxy)
{
   mockitopp::mock_object<OverloadedInterface> mock;
   // create overloaded lookups via variable proxy
   void (OverloadedInterface::*void_proxy_ptr)() = &OverloadedInterface::foo;
   void (OverloadedInterface::*int_proxy_ptr)(int) = &OverloadedInterface::foo;
   void (OverloadedInterface::*str_proxy_ptr)(std::string) = &OverloadedInterface::foo;
   // mock using the proxies in place of function address
   mock(void_proxy_ptr).when().thenReturn();
   mock(int_proxy_ptr).when(1234).thenReturn();
   mock(str_proxy_ptr).when("Hello World!").thenReturn();
   OverloadedInterface& obj = mock.getInstance();

   // do stuff with mock object as normal
}

// Example 2: using static_cast operator resolution
TEST(OverloadedFunctionsTest, ByStaticCast)
{
   mockitopp::mock_object<OverloadedInterface> mock;
   // mock using static_cast operator
   mock(static_cast<void (OverloadedInterface::*)()>(&overloaded_inteface::foo)).when().thenReturn();
   mock(static_cast<void (OverloadedInterface::*)(int)>(&overloaded_inteface::foo)).when(1234).thenReturn();
   mock(static_cast<void (OverloadedInterface::*)(std::string)>(&overloaded_inteface::foo)).when("Hellow World!").thenReturn();
   OverloadedInterface& obj = mock.getInstance();

   // do stuff with mock object as normal
}
```