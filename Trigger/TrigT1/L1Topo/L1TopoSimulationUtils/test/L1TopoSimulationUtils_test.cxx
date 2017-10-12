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

// test4: Check quadraticSumBW: compare against int sqrt (binary search implementation)
///! Returns floor of square root of x
///! from http://www.geeksforgeeks.org/square-root-of-an-integer/
int floorSqrt(int x)
{
    // Base cases
    if (x == 0 || x == 1)
        return x;
    // Do Binary Search for floor(sqrt(x))
    int start = 1, end = x, ans;
    while (start <= end) {
        int mid = (start + end) / 2;
        // If x is a perfect square
        if (mid*mid == x)
            return mid;
        // Since we need floor, we update answer when mid*mid is
        // smaller than x, and move closer to sqrt(x)
        if (mid*mid < x) {
            start = mid + 1;
            ans = mid;
        } else // If mid*mid is greater than x
            end = mid - 1;
    }
    return ans;
}
void test4()
{
    cout << "** test3: L1TopoSimulationUtils quadraticSumBW bitshift**\n";

    const std::vector<int> values = {0b0, // 0 through 16 bits
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
                                     0b111111111111111,
                                     0b1111111111111111};

    for(const int v : values){
        cout<<"sum2("<<v<<", "<<v<<") :"
            <<" std = "<<floorSqrt(v*v+v*v)
            <<" bw = "<<TSU::Kinematics::quadraticSumBW(v, v)
            <<endl;
    }
}



int main()
{
  test1();
  test2();
  test3();
  test4();
  return 0;
}
