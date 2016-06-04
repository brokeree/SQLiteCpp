/**
 * @file    VariadicBind.h
 * @ingroup SQLiteCpp
 * @brief   Convenience function for Statement::bind(...)
 *
 * Copyright (c) 2016 Paul Dreik (github@pauldreik.se)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <SQLiteCpp/Statement.h>

namespace SQLite
{


//this requires c++14. seems like visual studio 2015 should work (yet untested).
#if ( __cplusplus>= 201402L) || ( defined(_MSC_VER) && (_MSC_VER >= 1900) )
/// @cond
#include <utility>
#include <initializer_list>

/// implementation detail for variadic bind.
namespace detail {
template<class F,class ...Args, std::size_t ... I>
inline void invoke_with_index(F&& f, std::integer_sequence<std::size_t, I...>,
    const Args& ...args) {
  std::initializer_list<int> { (f(I+1,args),0)... };
}

/// implementation detail for variadic bind.
template<class F,class ...Args>
inline void invoke_with_index(F&&f, const Args& ... args) {
  invoke_with_index(std::forward<F>(f),std::index_sequence_for<Args...>(), args...);
}

} //namespace detail
/// @endcond

/**
 * \brief Convenience function for calling Statement::bind(...) once for each argument given.
 *
 * This takes care of incrementing the index between each calls to bind.
 *
 * This feature requires a c++14 capable compiler.
 *
 * \code{.cpp}
 * SQLite::Statement stm("SELECT * FROM MyTable WHERE colA>? && colB=? && colC<?");
 * bind(stm,a,b,c);
 * //...is equivalent to
 * stm.bind(1,a);
 * stm.bind(2,b);
 * stm.bind(3,c);
 * \endcode
 * @param s statement
 * @param args one or more args to bind.
 */
template<class ...Args>
void bind(SQLite::Statement& s,const Args& ... args) {

  static_assert(sizeof...(args)>0,"please invoke bind with one or more args");

  auto f=[&s](std::size_t index, const auto& value) {
    s.bind(index,value);
  };
  detail::invoke_with_index(f, args...);
}
#else
//not supported in older c++. provide a fallback?
#endif // c++14

}  // namespace SQLite
