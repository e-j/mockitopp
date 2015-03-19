## Verifying Invocations ##
There are several introspection methods which provide an observation into the use of the mock object during the xUnit test run. The mock object's `verify` methods allow test cases to assert if a given stubbed function has been invoke a certain number of times.  This is useful to ensure that no unexpected method invocations were encountered during the test run.  All of these examples will be mocking the simple interface below.

```
struct Foo
{
   virtual void bar() = 0;
};
```

### Verify Exactly ###
```
TEST(TestVerify, Exactly)
{
   mockitopp::mock_object<Foo> mock;
   mock(&Foo::bar);
   Foo& fooImpl = mock.getInstance();

   fooImpl.bar();
   fooImpl.bar();
   fooImpl.bar();

   ASSERT_TRUE(mock.verify(&Foo::bar).exactly(3)); // pass
}
```

### Verify at Least ###
```
TEST(TestVerify, AtLeast)
{
   mockitopp::mock_object<Foo> mock;
   mock(&Foo::bar);
   Foo& fooImpl = mock.getInstance();

   fooImpl.bar();
   fooImpl.bar();
   fooImpl.bar();

   ASSERT_TRUE(mock.verify(&Foo::bar).atLeast(1)); // pass
   ASSERT_TRUE(mock.verify(&Foo::bar).atLeast(2)); // pass
   ASSERT_TRUE(mock.verify(&Foo::bar).atLeast(3)); // pass
   ASSERT_TRUE(mock.verify(&Foo::bar).atLeast(4)); // fail
}
```

### Verify at Most ###
```
TEST(TestVerify, AtMost)
{
   mockitopp::mock_object<Foo> mock;
   mock(&Foo::bar);
   Foo& fooImpl = mock.getInstance();

   fooImpl.bar();
   fooImpl.bar();
   fooImpl.bar();

   ASSERT_TRUE(mock.verify(&Foo::bar).aMost(4)); // pass
   ASSERT_TRUE(mock.verify(&Foo::bar).atMost(3)); // pass
   ASSERT_TRUE(mock.verify(&Foo::bar).atMost(2)); // fail
}
```

### Verify Never ###
```
TEST(TestVerify, Never)
{
   mockitopp::mock_object<Foo> mock;
   mock(&Foo::bar);
   Foo& fooImpl = mock.getInstance();

   ASSERT_TRUE(mock.verify(&Foo::bar).never()); // pass
   fooImpl.bar();
   ASSERT_TRUE(mock.verify(&Foo::bar).never()); // fail
}
```

### Verify Range ###
```
TEST(TestVerify, Range)
{
   mockitopp::mock_object<Foo> mock;
   mock(&Foo::bar);
   Foo& fooImpl = mock.getInstance();

   fooImpl.bar();
   fooImpl.bar();
   ASSERT_TRUE(mock.verify(&Foo::bar).between(1, 3)); // pass
   fooImpl.bar();
   ASSERT_TRUE(mock.verify(&Foo::bar).between(1, 3)); // pass
   fooImpl.bar();
   ASSERT_TRUE(mock.verify(&Foo::bar).between(1, 3)); // fail
}
```