/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: interpolate_test.cxx,v 1.4 2008-12-23 03:40:05 ssnyder Exp $
/**
 * @file  interpolate_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Regression tests for table interpolation.
 */

#undef NDEBUG


#include "CaloClusterCorrection/interpolate.h"
#include "CaloConditions/Array.h"
#include "boost/io/ios_state.hpp"
#include <vector>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>


using CaloRec::Array;
using CaloRec::Arrayrep;
using CaloClusterCorr::interpolate;

#ifdef USE_DIVDIF
extern "C" {float divdif_(const float[],const float[],int*,float*,int* );}
#endif



const float data[] = {
     0.05,   6.264762e-01,
     0.15,   6.671484e-01,
     0.25,   7.134157e-01,
     0.35,   8.211740e-01,
     0.45,   9.047978e-01,
     0.55,   9.087084e-01,
     0.65,   8.923957e-01,
     0.75,   9.584507e-01,
     0.85,  -5.659607e-01,
     0.95,  -6.995252e-01,
     1.05,  -8.711259e-01,
     1.15,  -1.038724e+00,
     1.25,  -1.294152e+00,
     1.35,  -1.170352e+00,
     1.45,  -9.312256e-01,
};
const int shape[2] = {15, 2};


#define ARSZ(x) (sizeof(x)/sizeof((x)[0]))


// Test to see if two numbers are significantly different.
bool is_different (float a, float b)
{
  float den = std::abs (a) + std::abs (b);
  if (den == 0) return false;
  return std::abs ((a-b)/den) > 1e-6;
}


#ifdef USE_DIVDIF
void make_xytab (const Array<2>& table,
                 float x,
                 const Array<1>& regions,
                 std::vector<float> xytab[2],
                 int ycol)
{
  unsigned int ir;
  for (ir=0; ir < regions.size(); ir++)
    if (x <= regions[ir])
      break;

  unsigned int i=0;
  if (ir > 0) {
    while (i < table.size() && table[i][0] < regions[ir-1])
      ++i;
    if (i < table.size() && is_different (table[i][0], regions[ir-1])) {
      xytab[0].push_back (regions[ir-1]);
      xytab[1].push_back (table[i][ycol]);
    }
  }

  for (; i < table.size() &&
         !(ir < regions.size() && table[i][0] >= regions[ir]);
       ++i)
  {
    xytab[0].push_back (table[i][0]);
    xytab[1].push_back (table[i][ycol]);
  }

  if (i > 0 && ir < regions.size() &&
      is_different (table[i-1][0], regions[ir]))
  {
    xytab[0].push_back (regions[ir]);
    xytab[1].push_back (table[i-1][ycol]);
  }
}


void testit (const Array<2>& table,
             float x,
             int degree,
             const Array<1>& regions = Array<1>(),
             int ycol = 1)
{
  float y1 = interpolate (table, x, degree, ycol, regions);

  std::vector<float> xytab[2];
  make_xytab (table, x, regions, xytab, ycol);
  
  int n = xytab[0].size();
  float y2 = divdif_ (&*xytab[1].begin(),
                      &*xytab[0].begin(),
                      &n,
                      &x,
                      &degree);

  assert (!is_different (y1, y2));
  std::cout << std::setprecision (8);
  std::cout << x << " " << y2 << "\n";
}
#endif // USE_DIVDIF


void testit (float y2,
             const Array<2>& table,
             float x,
             int degree,
             const Array<1>& regions = Array<1>(),
             int ycol = 1)
{
  float y1 = interpolate (table, x, degree, ycol, regions);
  //assert (!is_different (y1, y2));
  if (is_different (y1, y2)) {
    boost::io::ios_base_all_saver iosaver (std::cout);  // Avoid coverity warning.
    std::cout << std::setprecision (8);
    std::cout << "diff " << x << " " << y1 << " " << y2 << "\n";
    std::abort();
  }
}


void test1()
{
  Arrayrep rep;
  rep.m_shape.assign (shape, shape + ARSZ(shape));
  rep.m_data.assign (data, data + ARSZ(data));
  rep.init_sizes();
  Array<2> table (rep);

  testit ( 0.912999,  table, 0.5,  3);
  testit ( 0.590771,  table, 0.0,  3);
  testit (-0.871126,  table, 1.05, 3);
  testit (-1.0011166, table, 1.13, 3);
  testit (-1.04871,   table, 1.4 , 3);
  testit (-9.34768,   table, 2.0 , 3);

  testit ( 0.91485268, table, 0.5,  4);
  testit ( 0.55207348, table, 0.0,  4);
  testit (-0.87112588, table, 1.05, 4);
  testit (-0.99805647, table, 1.13, 4);
  testit (-1.0201578,  table, 1.4 , 4);
  testit (-78.760239,  table, 2.0 , 4);

  Arrayrep rep1;
  rep1.m_shape.push_back (2);
  rep1.m_data.push_back (0.8);
  rep1.m_data.push_back (1.05);
  rep1.init_sizes();
  Array<1> regions (rep1);

  testit ( 0.65513462, table, 0.1,  4, regions);
  testit ( 0.92242599, table, 0.7,  4, regions);
  testit (-0.88620204, table, 1.1,  4, regions);
  testit (-0.73959452, table, 1.0,  4, regions);
  testit (-0.69952518, table, 1.05, 4, regions);
  testit ( 0.95845068, table, 0.8,  4, regions);
  testit (-1.2802936,  table, 1.3,  4, regions);

  testit ( 0.64960641, table, 0.1,  3, regions);
  testit ( 0.92791027, table, 0.7,  3, regions);
  testit (-0.91475517, table, 1.1,  3, regions);
  testit (-0.73959452, table, 1.0,  3, regions);
  testit (-0.69952518, table, 1.05, 3, regions);
  testit ( 0.95845068, table, 0.8,  3, regions);
  testit (-1.2631618,  table, 1.3,  3, regions);

  testit (0.1, table, 0.1,  3, regions, 0);

  Arrayrep rep2;
  rep2.m_shape.push_back (2);
  rep2.m_data.push_back (0.2);
  rep2.m_data.push_back (1.2);
  rep2.init_sizes();
  Array<1> regions2 (rep2);
  testit ( 0.65359104, table, 0.1,  4, regions2);
  testit (-1.2503984,  table, 1.3,  4, regions2);
}


int main()
{
  test1();
  return 0;
}
