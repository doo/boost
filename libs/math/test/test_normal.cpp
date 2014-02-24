// Copyright Paul A. Bristow 2010.
// Copyright John Maddock 2007.

// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

// test_normal.cpp

// http://en.wikipedia.org/wiki/Normal_distribution
// http://www.itl.nist.gov/div898/handbook/eda/section3/eda3661.htm
// Also:
// Weisstein, Eric W. "Normal Distribution."
// From MathWorld--A Wolfram Web Resource.
// http://mathworld.wolfram.com/NormalDistribution.html

#include <pch.hpp> // include directory /libs/math/src/tr1/ is needed.

#ifdef _MSC_VER
#  pragma warning (disable: 4127) // conditional expression is constant
// caused by using   if(std::numeric_limits<RealType>::has_infinity)
// and   if (std::numeric_limits<RealType>::has_quiet_NaN)
#endif

#include <boost/math/concepts/real_concept.hpp> // for real_concept
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp> // Boost.Test
#include <boost/test/floating_point_comparison.hpp>

#include <boost/math/distributions/normal.hpp>
    using boost::math::normal_distribution;
#include <boost/math/tools/test.hpp>
#include "test_out_of_range.hpp"

#include <iostream>
   using std::cout;
   using std::endl;
   using std::setprecision;
#include <limits>
  using std::numeric_limits;

template <class RealType>
RealType NaivePDF(RealType mean, RealType sd, RealType x)
{
   // Deliberately naive PDF calculator again which
   // we'll compare our pdf function.  However some
   // published values to compare against would be better....
   using namespace std;
   return exp(-(x-mean)*(x-mean)/(2*sd*sd))/(sd * sqrt(2*boost::math::constants::pi<RealType>()));
}

template <class RealType>
void check_normal(RealType mean, RealType sd, RealType x, RealType p, RealType q, RealType tol)
{
   BOOST_CHECK_CLOSE(
      ::boost::math::cdf(
         normal_distribution<RealType>(mean, sd),       // distribution.
         x),                                            // random variable.
         p,                                             // probability.
         tol);                                          // %tolerance.
   BOOST_CHECK_CLOSE(
      ::boost::math::cdf(
         complement(
            normal_distribution<RealType>(mean, sd),    // distribution.
            x)),                                        // random variable.
         q,                                             // probability complement.
         tol);                                          // %tolerance.
   BOOST_CHECK_CLOSE(
      ::boost::math::quantile(
         normal_distribution<RealType>(mean, sd),       // distribution.
         p),                                            // probability.
         x,                                             // random variable.
         tol);                                          // %tolerance.
   BOOST_CHECK_CLOSE(
      ::boost::math::quantile(
         complement(
            normal_distribution<RealType>(mean, sd),    // distribution.
            q)),                                        // probability complement.
         x,                                             // random variable.
         tol);                                          // %tolerance.
}

template <class RealType>
void test_spots(RealType)
{
   // Basic sanity checks
   RealType tolerance = 1e-2f; // 1e-4 (as %)
   // Some tests only pass at 1e-4 because values generated by
   // http://faculty.vassar.edu/lowry/VassarStats.html
   // give only 5 or 6 *fixed* places, so small values have fewer digits.

  // Check some bad parameters to the distribution,
   BOOST_CHECK_THROW(boost::math::normal_distribution<RealType> nbad1(0, 0), std::domain_error); // zero sd
   BOOST_CHECK_THROW(boost::math::normal_distribution<RealType> nbad1(0, -1), std::domain_error); // negative sd

  // Tests on extreme values of random variate x, if has numeric_limit infinity etc.
    normal_distribution<RealType> N01;
  if(std::numeric_limits<RealType>::has_infinity)
  {
    BOOST_CHECK_EQUAL(pdf(N01, +std::numeric_limits<RealType>::infinity()), 0); // x = + infinity, pdf = 0
    BOOST_CHECK_EQUAL(pdf(N01, -std::numeric_limits<RealType>::infinity()), 0); // x = - infinity, pdf = 0
    BOOST_CHECK_EQUAL(cdf(N01, +std::numeric_limits<RealType>::infinity()), 1); // x = + infinity, cdf = 1
    BOOST_CHECK_EQUAL(cdf(N01, -std::numeric_limits<RealType>::infinity()), 0); // x = - infinity, cdf = 0
    BOOST_CHECK_EQUAL(cdf(complement(N01, +std::numeric_limits<RealType>::infinity())), 0); // x = + infinity, c cdf = 0
    BOOST_CHECK_EQUAL(cdf(complement(N01, -std::numeric_limits<RealType>::infinity())), 1); // x = - infinity, c cdf = 1
    BOOST_CHECK_THROW(boost::math::normal_distribution<RealType> nbad1(std::numeric_limits<RealType>::infinity(), static_cast<RealType>(1)), std::domain_error); // +infinite mean
     BOOST_CHECK_THROW(boost::math::normal_distribution<RealType> nbad1(-std::numeric_limits<RealType>::infinity(),  static_cast<RealType>(1)), std::domain_error); // -infinite mean
     BOOST_CHECK_THROW(boost::math::normal_distribution<RealType> nbad1(static_cast<RealType>(0), std::numeric_limits<RealType>::infinity()), std::domain_error); // infinite sd
  }

  if (std::numeric_limits<RealType>::has_quiet_NaN)
  {
    // No longer allow x to be NaN, then these tests should throw.
    BOOST_CHECK_THROW(pdf(N01, +std::numeric_limits<RealType>::quiet_NaN()), std::domain_error); // x = NaN
    BOOST_CHECK_THROW(cdf(N01, +std::numeric_limits<RealType>::quiet_NaN()), std::domain_error); // x = NaN
    BOOST_CHECK_THROW(cdf(complement(N01, +std::numeric_limits<RealType>::quiet_NaN())), std::domain_error); // x = + infinity
    BOOST_CHECK_THROW(quantile(N01, +std::numeric_limits<RealType>::quiet_NaN()), std::domain_error); // p = + infinity
    BOOST_CHECK_THROW(quantile(complement(N01, +std::numeric_limits<RealType>::quiet_NaN())), std::domain_error); // p = + infinity
  }

   cout << "Tolerance for type " << typeid(RealType).name()  << " is " << tolerance << " %" << endl;

   check_normal(
      static_cast<RealType>(5),
      static_cast<RealType>(2),
      static_cast<RealType>(4.8),
      static_cast<RealType>(0.46017),
      static_cast<RealType>(1 - 0.46017),
      tolerance);

   check_normal(
      static_cast<RealType>(5),
      static_cast<RealType>(2),
      static_cast<RealType>(5.2),
      static_cast<RealType>(1 - 0.46017),
      static_cast<RealType>(0.46017),
      tolerance);

   check_normal(
      static_cast<RealType>(5),
      static_cast<RealType>(2),
      static_cast<RealType>(2.2),
      static_cast<RealType>(0.08076),
      static_cast<RealType>(1 - 0.08076),
      tolerance);

   check_normal(
      static_cast<RealType>(5),
      static_cast<RealType>(2),
      static_cast<RealType>(7.8),
      static_cast<RealType>(1 - 0.08076),
      static_cast<RealType>(0.08076),
      tolerance);

   check_normal(
      static_cast<RealType>(-3),
      static_cast<RealType>(5),
      static_cast<RealType>(-4.5),
      static_cast<RealType>(0.38209),
      static_cast<RealType>(1 - 0.38209),
      tolerance);

   check_normal(
      static_cast<RealType>(-3),
      static_cast<RealType>(5),
      static_cast<RealType>(-1.5),
      static_cast<RealType>(1 - 0.38209),
      static_cast<RealType>(0.38209),
      tolerance);

   check_normal(
      static_cast<RealType>(-3),
      static_cast<RealType>(5),
      static_cast<RealType>(-8.5),
      static_cast<RealType>(0.13567),
      static_cast<RealType>(1 - 0.13567),
      tolerance);

   check_normal(
      static_cast<RealType>(-3),
      static_cast<RealType>(5),
      static_cast<RealType>(2.5),
      static_cast<RealType>(1 - 0.13567),
      static_cast<RealType>(0.13567),
      tolerance);

   //
   // Tests for PDF: we know that the peak value is at 1/sqrt(2*pi)
   //
   tolerance = boost::math::tools::epsilon<RealType>() * 5 * 100; // 5 eps as a percentage
   BOOST_CHECK_CLOSE(
      pdf(normal_distribution<RealType>(), static_cast<RealType>(0)),
      static_cast<RealType>(0.3989422804014326779399460599343818684759L), // 1/sqrt(2*pi)
      tolerance);
   BOOST_CHECK_CLOSE(
      pdf(normal_distribution<RealType>(3), static_cast<RealType>(3)),
      static_cast<RealType>(0.3989422804014326779399460599343818684759L),
      tolerance);
   BOOST_CHECK_CLOSE(
      pdf(normal_distribution<RealType>(3, 5), static_cast<RealType>(3)),
      static_cast<RealType>(0.3989422804014326779399460599343818684759L / 5),
      tolerance);

   //
   // Spot checks for mean = -5, sd = 6:
   //
   for(RealType x = -15; x < 5; x += 0.125)
   {
      BOOST_CHECK_CLOSE(
         pdf(normal_distribution<RealType>(-5, 6), x),
         NaivePDF(RealType(-5), RealType(6), x),
         tolerance);
   }

    RealType tol2 = boost::math::tools::epsilon<RealType>() * 5;
    normal_distribution<RealType> dist(8, 3);
    RealType x = static_cast<RealType>(0.125);

    BOOST_MATH_STD_USING // ADL of std math lib names

    // mean:
    BOOST_CHECK_CLOSE(
       mean(dist)
       , static_cast<RealType>(8), tol2);
    // variance:
    BOOST_CHECK_CLOSE(
       variance(dist)
       , static_cast<RealType>(9), tol2);
    // std deviation:
    BOOST_CHECK_CLOSE(
       standard_deviation(dist)
       , static_cast<RealType>(3), tol2);
    // hazard:
    BOOST_CHECK_CLOSE(
       hazard(dist, x)
       , pdf(dist, x) / cdf(complement(dist, x)), tol2);
    // cumulative hazard:
    BOOST_CHECK_CLOSE(
       chf(dist, x)
       , -log(cdf(complement(dist, x))), tol2);
    // coefficient_of_variation:
    BOOST_CHECK_CLOSE(
       coefficient_of_variation(dist)
       , standard_deviation(dist) / mean(dist), tol2);
    // mode:
    BOOST_CHECK_CLOSE(
       mode(dist)
       , static_cast<RealType>(8), tol2);

    BOOST_CHECK_CLOSE(
       median(dist)
       , static_cast<RealType>(8), tol2);

    // skewness:
    BOOST_CHECK_CLOSE(
       skewness(dist)
       , static_cast<RealType>(0), tol2);
    // kertosis:
    BOOST_CHECK_CLOSE(
       kurtosis(dist)
       , static_cast<RealType>(3), tol2);
    // kertosis excess:
    BOOST_CHECK_CLOSE(
       kurtosis_excess(dist)
       , static_cast<RealType>(0), tol2);

    normal_distribution<RealType> norm01(0, 1); // Test default (0, 1)
    BOOST_CHECK_CLOSE(
       mean(norm01),
       static_cast<RealType>(0), 0); // Mean == zero

    normal_distribution<RealType> defsd_norm01(0); // Test default (0, sd = 1)
    BOOST_CHECK_CLOSE(
       mean(defsd_norm01),
       static_cast<RealType>(0), 0); // Mean == zero

    normal_distribution<RealType> def_norm01; // Test default (0, sd = 1)
    BOOST_CHECK_CLOSE(
       mean(def_norm01),
       static_cast<RealType>(0), 0); // Mean == zero

    BOOST_CHECK_CLOSE(
       standard_deviation(def_norm01),
       static_cast<RealType>(1), 0); // Mean == zero

    // Error tests:
    check_out_of_range<boost::math::normal_distribution<RealType> >(0, 1); // (All) valid constructor parameter values.
    
    BOOST_CHECK_THROW(pdf(normal_distribution<RealType>(0, 0), 0), std::domain_error);
    BOOST_CHECK_THROW(pdf(normal_distribution<RealType>(0, -1), 0), std::domain_error);
    BOOST_CHECK_THROW(quantile(normal_distribution<RealType>(0, 1), -1), std::domain_error);
    BOOST_CHECK_THROW(quantile(normal_distribution<RealType>(0, 1), 2), std::domain_error);
} // template <class RealType>void test_spots(RealType)

BOOST_AUTO_TEST_CASE( test_main )
{
    // Check that can generate normal distribution using the two convenience methods:
   boost::math::normal myf1(1., 2); // Using typedef
   normal_distribution<> myf2(1., 2); // Using default RealType double.
  boost::math::normal myn01; // Use default values.
  // Note NOT myn01() as the compiler will interpret as a function!

  // Check the synonyms, provided to allow generic use of find_location and find_scale.
  BOOST_CHECK_EQUAL(myn01.mean(), myn01.location());
  BOOST_CHECK_EQUAL(myn01.standard_deviation(), myn01.scale());

    // Basic sanity-check spot values.
   // (Parameter value, arbitrarily zero, only communicates the floating point type).
  test_spots(0.0F); // Test float. OK at decdigits = 0 tolerance = 0.0001 %
  test_spots(0.0); // Test double. OK at decdigits 7, tolerance = 1e07 %
#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
  test_spots(0.0L); // Test long double.
#if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x0582))
  test_spots(boost::math::concepts::real_concept(0.)); // Test real concept.
#endif
#else
   std::cout << "<note>The long double tests have been disabled on this platform "
      "either because the long double overloads of the usual math functions are "
      "not available at all, or because they are too inaccurate for these tests "
      "to pass.</note>" << std::cout;
#endif

   
} // BOOST_AUTO_TEST_CASE( test_main )

/*

Output:

Autorun "i:\boost-06-05-03-1300\libs\math\test\Math_test\debug\test_normal.exe"
Running 1 test case...
Tolerance for type float is 0.01 %
Tolerance for type double is 0.01 %
Tolerance for type long double is 0.01 %
Tolerance for type class boost::math::concepts::real_concept is 0.01 %
*** No errors detected




------ Build started: Project: test_normal, Configuration: Release Win32 ------
  test_normal.cpp
  Generating code
  Finished generating code
  test_normal.vcxproj -> J:\Cpp\MathToolkit\test\Math_test\Release\test_normal.exe
  Running 1 test case...
  Tolerance for type float is 0.01 %
  Tolerance for type double is 0.01 %
  Tolerance for type long double is 0.01 %
  Tolerance for type class boost::math::concepts::real_concept is 0.01 %
  
  *** No errors detected
  Detected memory leaks!
  Dumping objects ->
  {2413} normal block at 0x00321190, 42 bytes long.
   Data: <class boost::mat> 63 6C 61 73 73 20 62 6F 6F 73 74 3A 3A 6D 61 74 
  {2412} normal block at 0x003231F0, 8 bytes long.
   Data: <  2  22 > 90 11 32 00 98 32 32 00 
  {1824} normal block at 0x00323180, 12 bytes long.
   Data: <long double > 6C 6F 6E 67 20 64 6F 75 62 6C 65 00 
  {1823} normal block at 0x00323298, 8 bytes long.
   Data: < 12 `22 > 80 31 32 00 60 32 32 00 
  {1227} normal block at 0x00323148, 7 bytes long.
   Data: <double > 64 6F 75 62 6C 65 00 
  {1226} normal block at 0x00323260, 8 bytes long.
   Data: <H12  02 > 48 31 32 00 A0 30 32 00 
  {633} normal block at 0x003230D8, 6 bytes long.
   Data: <float > 66 6C 6F 61 74 00 
  {632} normal block at 0x003230A0, 8 bytes long.
   Data: < 02     > D8 30 32 00 00 00 00 00 
  Object dump complete.
========== Build: 1 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========


*/


