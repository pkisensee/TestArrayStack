///////////////////////////////////////////////////////////////////////////////
//
//  TestArrayStack.cpp
//
//  Copyright © Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.
//
///////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include <utility>

#include "ArrayStack.h"
#include "Util.h"

using namespace PKIsensee;

#ifdef _DEBUG
#define test(e) assert(e)
#else
#define test(e) static_cast<void>( (e) || ( Util::DebugBreak(), 0 ) )
#endif

int __cdecl main()
{
  // ArrayStack<void, 1> fail;

  // size and capacity
  ArrayStack<int, 3> a;
  test( a.size() == 0 );
  test( a.capacity() == 3 );

  // push/pop, size, top, empty, full
  a.push( 0 );
  a.push( 1 );
  a.push( 2 );
  test( a.size() == 3 );
  test( a.top() == 2 );
  test( a.full() );
  a.pop();
  test( a.size() == 2 );
  test( a.top() == 1 );
  a.pop();
  test( a.size() == 1 );
  test( a.top() == 0 );
  a.pop();
  test( a.size() == 0 );
  test( a.empty() );
  a.push( 42 );
  test( a.size() == 1 );

  // == and != comparison
  std::vector<int> v = {42};
  ArrayStack<int, 3> b( std::begin(v), std::end(v) );
  test( a == b );
  test( b.top() == 42 );
  b.push( 1 );
  test( a != b ); // different sizes
  b.pop();
  b.pop();
  b.push( 41 );
  test( a != b ); // same size, different values

  // <,>,<=,>= comparisons
  test( a > b );
  test( b < a );
  test( a >= b );
  test( b <= a );

  ArrayStack<int, 3> c( std::array<int, 3>{{ 4, 5, 6 }} );
  test( c.top() == 6 );

  v.push_back( 24 );
  ArrayStack<int, 3> d( std::from_range, v );
  test( d.size() == 2 );
  test( d.top() == 24 );
  test( d[0] == 42 );
  test( d[1] == 24 );
  test( d[1] = -3 );
  test( d[1] == -3 );
  // test( d[2] == 0 ); // error

  d.top() = 1234;
  test( d.top() == 1234 );
  d.swap( c );
  test( d.top() == 6 );
  test( c.top() == 1234 );

  ArrayStack<std::string, 3> ss;
  ss.push( "foo" );
  ss.push( std::string("bar") );

  // push_range
  int arr[3] = { 0, 1, 2 };
  d.clear();
  d.push_range( arr );
  test( d.top() == 2 );
  d.pop();
  test( d.top() == 1);
  d.pop();
  test( d.top() == 0 );

  // emplace
  d.clear();
  d.emplace( 42 );
  d.emplace( 24 );
  test( d.top() == 24 );
  d.pop();
  test( d.top() == 42 );

  ArrayStack<std::pair<int, double>, 4> pairStack;
  pairStack.emplace( 1, 1.0 );
  pairStack.emplace( 2, 2.0 );
  auto [ii, dd] = pairStack.top();
  test( ii == 2 );
  test( dd == 2.0 );
  pairStack.pop();
  test( pairStack.top().first == 1 );
  test( pairStack.top().second == 1.0 );


}

///////////////////////////////////////////////////////////////////////////////
