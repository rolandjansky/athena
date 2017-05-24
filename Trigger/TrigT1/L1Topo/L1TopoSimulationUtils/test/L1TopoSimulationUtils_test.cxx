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
<<<<<<< HEAD
#include <exception>
=======
>>>>>>> 6d0bfb87f1... Initial skeleton of unit tests for L1TopoSimulationUtils

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

<<<<<<< HEAD
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


=======
>>>>>>> 6d0bfb87f1... Initial skeleton of unit tests for L1TopoSimulationUtils

int main()
{
  test1();
<<<<<<< HEAD
  test2();
  test3();
=======
>>>>>>> 6d0bfb87f1... Initial skeleton of unit tests for L1TopoSimulationUtils
  return 0;
}
