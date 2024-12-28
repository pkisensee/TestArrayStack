///////////////////////////////////////////////////////////////////////////////
//
//  Test_array_stack.cpp
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

// #define PK_ENABLE_EXCEPTIONS 1

#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

#include "array_stack.h"
#include "Util.h"

using namespace PKIsensee;

#ifdef _DEBUG
#define test(e) assert(e)
#else
#define test(e) static_cast<void>( (e) || ( Util::DebugBreak(), 0 ) )
#endif

struct Point // for testing SynthThreeWay
{
  int x = 0;
  int y = 0;

  // minimal requirement is support of < and >
  constexpr bool operator<( const Point& lhs ) const { return x < lhs.x; }
  constexpr bool operator>( const Point& lhs ) const { return x > lhs.x; }

};

#define testex(e, msg) TryCatch( ( [&]() { (e); } ), msg );

/*
#define testex(e, msg) \
try {                   \
  (e);                  \
}                         \
catch( std::out_of_range& ex ) {            \
  test( std::string( ex.what() ) == msg );  \
}                                           \
catch( ... ) {                              \
  test( false );                            \
}
*/

template <typename TryLambda>
void TryCatch( TryLambda&& tryLambda, std::string_view exceptionMsg )
{
  try
  {
    tryLambda();
  }
  catch( std::out_of_range& ex )
  {
    test( ex.what() == exceptionMsg );
  }
  catch( ... )
  {
    test( false );
  }
}

void TestConstMembers( const array_stack<int, 3> as )
{
  test( as[0] == 42 );
  test( as.top() == 42 );
  // test( as.push( 1 ) ); compiler error
}

int __cdecl main()
{
  // array_stack<void, 1> fail;

  // size and capacity
  array_stack<int, 3> a;
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
  TestConstMembers( a );

  // Exception handling
  array_stack<int, 2> err;

#if defined(PK_ENABLE_EXCEPTIONS)

  testex( err.top(), "empty stack" );
  testex( err.pop(), "empty stack" );

  err.push( 1 );
  err.push( 1 );
  testex( err.push( 1 ), "stack overflow" );

  int arrErr[3] = { 0, 1, 2 };
  err.clear();
  testex( err.push_range( arrErr ), "stack overflow" );

  err.clear();
  err.emplace( 1 );
  err.emplace( 1 );
  testex( err.emplace( 1 ), "stack overflow" );

#else

  // err.top(); // assertion
  // err.pop(); // assertion

  err.push( 1 );
  err.push( 1 );
  // err.push( 1 ); // assertion

  int arrErr[3] = { 0, 1, 2 };
  err.clear();
  // err.push_range(arrErr); // assertion

  err.clear();
  err.emplace( 1 );
  err.emplace( 1 );
  // err.emplace( 1 ); // assertion

#endif // PK_ENABLE_EXCEPTIONS

  // == and != comparison
  std::vector<int> v = {42};
  array_stack<int, 3> b( std::begin(v), std::end(v) );
  test( a == b );
  test( b.top() == 42 );
  b.push( 1 );
  test( a != b ); // different sizes
  b.pop();
  b.pop();
  b.push( 41 );
  test( a != b ); // same size, different values

  // <,>,<=,>= comparisons with same size stacks
  test( a > b );
  test( b < a );
  test( a >= b );
  test( b <= a );

  // <,>,<=,>= comparisons with different sized stacks
  b.push( 0 );
  test( a != b );
  test( a > b );
  test( b < a );
  test( a >= b );
  test( b <= a );

  array_stack<int, 3> c( std::array<int, 3>{{ 4, 5, 6 }} );
  test( c.top() == 6 );

  v.push_back( 24 );
  array_stack<int, 3> d( std::from_range, v );
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

  // push lvalues and rvalues
  std::string value = "Here's a long string that's stored on the heap";
  array_stack<std::string, 3> ss;
  ss.push( "foo" ); // rvalue
  ss.push( std::string("bar") ); // rvalue
  ss.push( value ); // lvalue
  test( ss.top() == value );
  ss.pop();
  test( ss.top() == "bar" );

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

  // emplace
  array_stack<std::pair<int, double>, 4> pairStack;
  pairStack.emplace( 1, 1.0 );
  pairStack.emplace( 2, 2.0 );
  auto [ii, dd] = pairStack.top();
  test( ii == 2 );
  test( dd == 2.0 );
  pairStack.pop();
  test( pairStack.top().first == 1 );
  test( pairStack.top().second == 1.0 );

  // free swap
  // swap( pairStack, d ); won't compile
  auto aTop = a.top();
  auto bTop = b.top();
  swap( a, b );
  test( a.top() == bTop );
  test( b.top() == aTop );

  // SynthThreeWay alternate path
  array_stack<Point, 10> ps1;
  array_stack<Point, 10> ps2;
  ps1.push( Point{} );
  ps1[0].x = 1;
  ps2.push( Point{} );
  test( ps1 > ps2 );

}

///////////////////////////////////////////////////////////////////////////////
