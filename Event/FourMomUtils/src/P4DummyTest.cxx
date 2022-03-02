/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AthAnalysisBase/ManaCore doesn't currently include the Trigger Service
#ifndef XAOD_ANALYSIS

// this file is only useful to check
// all virtual methods have been implemented
#include "FourMomUtils/P4DummyTest.h"
#include "FourMom/P4EEtaPhiM.h"
#include "FourMom/P4PtEtaPhiM.h"
#include "FourMom/P4PxPyPzE.h"
#include "FourMom/P4IPtCotThPhiM.h"
#include "FourMomUtils/P4Helpers.h"
#include "CLHEP/Vector/LorentzVector.h"


P4DummyTest::P4DummyTest()
{
  P4EEtaPhiM theP4EEtaPhiM(10.,1.,2.,0.);
  P4EEtaPhiM twoP4EEtaPhiM(10.,1.3,2.4,0.);
  P4PxPyPzE theP4PxPyPzE(0.,3.,4.,5.);
  P4IPtCotThPhiM theP4IPtCotThPhiM(0.5,1.,2.,0.);
  P4EEtaPhiM threeP4EEtaPhiM(theP4IPtCotThPhiM);
  P4PtEtaPhiM fourP4PtEtaPhiM(theP4IPtCotThPhiM);


  double aux [[maybe_unused]] = theP4EEtaPhiM.px();
  aux+=theP4PxPyPzE.px();
  aux+=theP4IPtCotThPhiM.px();
  double theDeltaR1= P4Helpers::deltaR(theP4EEtaPhiM,theP4IPtCotThPhiM);
  std::cout << "P4DummyTest: theDeltaR1= " << theDeltaR1 << std::endl ;
  double theDeltaR2= P4Helpers::deltaR(theP4EEtaPhiM,threeP4EEtaPhiM);
  std::cout << "P4DummyTesters: theDeltaR2= " << theDeltaR2 << std::endl ;
  double theDeltaR3= P4Helpers::deltaR(theP4EEtaPhiM,twoP4EEtaPhiM);
  std::cout << "P4DummyTest: theDeltaR3= " << theDeltaR3 << std::endl ;
  CLHEP::HepLorentzVector theHlv=twoP4EEtaPhiM.hlv() ;
  double theDeltaR4= P4Helpers::deltaR(P4PxPyPzE(theHlv),theP4EEtaPhiM);
  std::cout << "P4DummyTest: theDeltaR4= " << theDeltaR4 << std::endl ;


  double theDeltaR5= P4Helpers::deltaR(theP4EEtaPhiM,fourP4PtEtaPhiM);
  std::cout << "P4DummyTest: theDeltaR5= " << theDeltaR5 << " [=R2]"<< std::endl ;


  //> Test of deltaPhi
  double dPhi1 = P4Helpers::deltaPhi( theP4EEtaPhiM, theP4IPtCotThPhiM );
  std::cout << "P4DummyTest: dPhi1= " << dPhi1 << std::endl;

  double dPhi2 = P4Helpers::deltaPhi( theP4EEtaPhiM, threeP4EEtaPhiM );
  std::cout << "P4DummyTest: dPhi2= " << dPhi2 << std::endl;

  double dPhi3 = P4Helpers::deltaPhi( theP4EEtaPhiM, twoP4EEtaPhiM );
  std::cout << "P4DummyTest: dPhi3= " << dPhi3 << std::endl;

  double dPhi4 = P4Helpers::deltaPhi( P4PxPyPzE(theHlv), theP4EEtaPhiM );
  std::cout << "P4DummyTest: dPhi4= " << dPhi4 << std::endl;

  double dPhi5 = P4Helpers::deltaPhi( theP4EEtaPhiM, fourP4PtEtaPhiM );
  std::cout << "P4DummyTest: dPhi5= " << dPhi5 << " [=dPhi2]"<< std::endl;



  //> Test of invMass -- 2Body
  std::cout << "P4DummyTest: --- Test 2-body invariant mass ---" << std::endl;
  double inv2Mass1 = P4Helpers::invMass( theP4EEtaPhiM, theP4IPtCotThPhiM );
  std::cout << "P4DummyTest: inv2Mass1= " << inv2Mass1 << std::endl;

  double inv2Mass2 = P4Helpers::invMass( theP4EEtaPhiM, threeP4EEtaPhiM );
  std::cout << "P4DummyTest: inv2Mass2= " << inv2Mass2 << std::endl;

  double inv2Mass3 = P4Helpers::invMass( theP4EEtaPhiM, twoP4EEtaPhiM );
  std::cout << "P4DummyTest: inv2Mass3= " << inv2Mass3 << std::endl;

  double inv2Mass4 = P4Helpers::invMass( P4PxPyPzE(theHlv), theP4EEtaPhiM );
  std::cout << "P4DummyTest: inv2Mass4= " << inv2Mass4 << std::endl;

  double inv2Mass5 = P4Helpers::invMass( theP4EEtaPhiM, fourP4PtEtaPhiM );
  std::cout << "P4DummyTest: inv2Mass5= " << inv2Mass5 << " [=mass2]"<< std::endl;

  //> Test of invMass -- 4-Body
  std::cout << "P4DummyTest: --- Test 4-body invariant mass ---" << std::endl;
  double inv4Mass1 = P4Helpers::invMass( theP4EEtaPhiM, theP4IPtCotThPhiM,
              theP4EEtaPhiM, theP4IPtCotThPhiM );
  std::cout << "P4DummyTest: inv4Mass1= " << inv4Mass1 << std::endl;

  double inv4Mass2 = P4Helpers::invMass( theP4EEtaPhiM, threeP4EEtaPhiM,
              theP4EEtaPhiM, threeP4EEtaPhiM );
  std::cout << "P4DummyTest: inv4Mass2= " << inv4Mass2 << std::endl;

  double inv4Mass3 = P4Helpers::invMass( theP4EEtaPhiM, twoP4EEtaPhiM,
              theP4EEtaPhiM, twoP4EEtaPhiM );
  std::cout << "P4DummyTesters: inv4Mass3= " << inv4Mass3 << std::endl;

  double inv4Mass4 = P4Helpers::invMass( P4PxPyPzE(theHlv), theP4EEtaPhiM,
              P4PxPyPzE(theHlv), theP4EEtaPhiM );
  std::cout << "P4DummyTest: inv4Mass4= " << inv4Mass4 << std::endl;

  double inv4Mass5 = P4Helpers::invMass( theP4EEtaPhiM, fourP4PtEtaPhiM,
              theP4EEtaPhiM, fourP4PtEtaPhiM );
  std::cout << "P4DummyTest: inv4Mass5= " << inv4Mass5 << " [=Mass2]"<< std::endl;



}

#endif
