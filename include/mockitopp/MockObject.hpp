#ifndef __MOCKITOPP_MOCK_OBJECT_HPP__
#define __MOCKITOPP_MOCK_OBJECT_HPP__

#include <mockitopp/detail/stubbing/dynamic_object.hpp>
#include <mockitopp/matchers/base/any.hpp>
#include <mockitopp/matchers/base/equal.hpp>
#include <mockitopp/matchers/base/is_not.hpp>
#include <mockitopp/matchers/base/null.hpp>
#include <mockitopp/matchers/type/string_literal.hpp>

namespace mockitopp
{
   /**
    * provides a base implementation of a mock object
    *
    * @author Trevor Pounds
    */
   template <typename T>
   struct MockObject : protected detail::dynamic_object
   {
      /**
       * returns a mock object of the given abstract base class/interface
       *
       * @return mock object
       */
      T& getInstance()
        { return reinterpret_cast<T&>(*this); }

      /**
       * stub functionality for a given method
       *
       * @param ptr2member method to be stubbed
       */
      template <typename M>
      detail::dynamic_vfunction<M>& operator() (M ptr2member)
         { return define_function(ptr2member); }
   };
} // namespace mockitopp

#endif //__MOCKITOPP_MOCK_OBJECT_HPP__
