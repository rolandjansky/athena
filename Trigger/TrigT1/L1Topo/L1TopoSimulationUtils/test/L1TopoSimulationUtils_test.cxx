/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Unit test for L1TopoSimulationUtils
 *
 * We should collect here a set of basic unit tests for the functions
 * implemented in this library.
 *
 * davide.gerbaudo@gmail.com
 * May 2017
 */

#undef NDEBUG
#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoSimulationUtils/KFLUT.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Kinematics.h"

#include <boost/numeric/conversion/cast.hpp>

#include <iostream>
#include <cassert>
#include <cstdint>
#include <exception>

using std::cout;
using std::endl;
using TCS::ClusterTOB;

// Check cosh for intermediate delta-eta
void test1()
{
    cout << "** test1: L1TopoSimulationUtils cosh at different delta-eta values**\n";
    ClusterTOB cluster_fix0; // one cluster fixed at eta=0
    ClusterTOB cluster_var; // one cluster scanning eta values

    for(double ieta : {23, int(23.5), 24}) {
        cluster_var.setEta(ieta);
        auto cosh_val = TSU::Hyperbolic::Cosh.at(abs(cluster_fix0.eta() - cluster_var.eta()));
        cout<<"delta eta: "
            <<" abs("<<cluster_fix0.eta()<<" - "<<cluster_var.eta()<<")"
            <<" = "<<abs(cluster_fix0.eta() - cluster_var.eta())
            <<", cosh "<<cosh_val
            <<endl;
    }
}

// Check KF correction values
void test2()
{
    cout << "** test2: L1TopoSimulationUtils KFLUT correction vs. et and eta**\n";
    TCS::KFLUT  kflut;

    // use roughly the center of the bins defined in KFLUT.cxx + one outsider
    for(const double eta : {0.05, 0.2, 0.41, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6, 2.9, 3.1, 3.5}) {
        for(const unsigned int et : {4, 12, 24, 48, 96, 360}) {
            int iet = kflut.getetbin(et);
            int ieta = kflut.getetabin(abs(eta));
            double corrfactor = kflut.getcorrKF(iet, ieta);
            const bool large = corrfactor>+0.5;
            const bool small = corrfactor<-0.5;
            if(small or large) {
                cout<<" et "<<et<<" ["<<iet<<"], "
                    <<" eta "<<eta<<" ["<<ieta<<"] : "
                    <<corrfactor<<(large ? " >>>" :
                                   small ? " <<<" :
                                   "")
                    <<endl;
            }
        } // for(et)
    } // for(eta)
}

// Check KF saturation
void test3()
{
    cout << "** test3: L1TopoSimulationUtils KFLUT saturation**\n";
    TCS::KFLUT  kflut;
    try {
        const int et = 1024+1; // 2**10 + 1 : JetTOB has 10bits for E8x8
        const double eta = 0.5;
        int iet = kflut.getetbin(et);
        int ieta = kflut.getetabin(abs(eta));
        double corrfactor = kflut.getcorrKF(iet, ieta);
        cout<<" et "<<et<<" ["<<iet<<"], "
            <<" eta "<<eta<<" ["<<ieta<<"] : "
            <<corrfactor<<" THIS SHOULD NOT APPEAR HERE"
            <<endl;
    } catch(const std::out_of_range &e) {
        cout<<"This out_of_range is expected for ET values beyond 1024 : "
            <<e.what()<<endl;
    }
}

// test4: Check quadraticSumBW: compare against a "traditional" (binary search) int sqrt implementation
/**
   @brief Returns floor of square root of x

   from http://www.geeksforgeeks.org/square-root-of-an-integer/
*/
int floorSqrt(unsigned int x)
{
    // Base cases
    if (x == 0 || x == 1)
        return x;
    // Do Binary Search for floor(sqrt(x))
    unsigned long int start = 1, end = x, ans;
    unsigned long int ux = x;
    while (start <= end) {
        unsigned long int mid = (start + end) / 2;
        // If x is a perfect square
        if (mid*mid == ux)
            return mid;
        // Since we need floor, we update answer when mid*mid is
        // smaller than x, and move closer to sqrt(x)
        if (mid*mid < ux) {
            start = mid + 1;
            ans = mid;
        } else // If mid*mid is greater than x
            end = mid - 1;
    }
    return boost::numeric_cast<int>(ans);
}
int test4_compare(int u, int v){
    int bw_result = TSU::Kinematics::quadraticSumBW(u,v);
    unsigned int uu = u*u;
    unsigned int uv = v*v;
    int fl_result = floorSqrt(uu+uv);
    cout<<" sum2("<<u<<", "<<v<<") :"
        <<" std = "<<fl_result
        <<" bw = "<<bw_result
        <<endl;
    if(bw_result < u && bw_result < v) return 1; //overflow occurred
    else return bw_result - fl_result; //possibility mismatch
}
int test4()
{
    cout << "** test4: L1TopoSimulationUtils quadraticSumBW bitshift**\n";
    const std::vector<int> values = {/*0b0,*/ // 0 through 32767=2^(15) (16 bits)
                                     0b1,
                                     0b11,
                                     0b111,
                                     0b1111,
                                     0b11111,
                                     0b111111,
                                     0b1111111,
                                     0b11111111,
                                     0b111111111,
                                     0b1111111111,
                                     0b11111111111,
                                     0b111111111111,
                                     0b1111111111111,
                                     0b11111111111111,
                                     0b111111111111111};
    for(const int v : values){
      if(test4_compare(v,v) != 0) return 1;
      if(test4_compare(-v,-v) != 0) return 1;
    }

    if(test4_compare(0,65535) != 0) return 1;
    if(test4_compare(362,65535) != 0) return 1;
    if(test4_compare(46340,46340) != 0) return 1;

    return 0;
}



int main()
{
  test1();
  test2();
  test3();
  int result = test4();

  return result;
}
