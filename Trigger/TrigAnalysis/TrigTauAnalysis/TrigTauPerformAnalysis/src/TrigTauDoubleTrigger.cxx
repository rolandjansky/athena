/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauAnalyzer.cxx
/// Package : TrigTauPerformAnalysis
/// Author  : Olga Igonkina (University of Oregon)
/// Created : April 2007
///
/// DESCRIPTION:
///           Class to manage the double triggers
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrigTauPerformAnalysis/TrigTauDoubleTrigger.h"

#include "TrigTauPerformNtuple/TrigTauEvent.h"

#include <iostream>
using namespace std;

ClassImp(TrigTauDoubleTrigger)  

TrigTauDoubleTrigger::~TrigTauDoubleTrigger(){}

// Menu2 should either be E10, Mu10, or tau
TrigTauDoubleTrigger::TrigTauDoubleTrigger(TrigTauFlags::TRIGGERMENU item1, 
					   TrigTauFlags::TRIGGERMENU item2)
  : analyzer(), trigMan1(), trigMan2()
{
  bool tau1 = (item1 == TrigTauFlags::TAU12  || 
	       item1 == TrigTauFlags::TAU16 || item1 == TrigTauFlags::TAU20 ||
	       item1 == TrigTauFlags::TAU29 || item1 == TrigTauFlags::TAUNOCUT );

  bool tau2 = (
	       item2 == TrigTauFlags::TAU12  || 
	       item2 == TrigTauFlags::TAU16 || item2 == TrigTauFlags::TAU20 ||
	       item2 == TrigTauFlags::TAU29 || item2 == TrigTauFlags::TAUNOCUT );

  menuWithTau = (tau1 || tau2);
  bothMenuWithTau = tau1 && tau2;
  if(  !tau1 && tau2 ){ menu1 = item2; menu2= item1; }
  else                { menu1 = item1; menu2 = item2;}

//   cout << " menuWithTau " << menuWithTau << endl;
//   cout << " bothMenuWithTau " << bothMenuWithTau << endl;
//   cout << " menu1  " << menu1 << endl;
//   cout << "  menu2 " << menu2 << endl;


  float met = 0;
  menuWithMET = false;
  if( item1 == TrigTauFlags::MET15 ||  item2 == TrigTauFlags::MET15 )
    {met = 15; menuWithMET = true;
      if(!tau1 && item2 == TrigTauFlags::MET15 ){menu1=item2; menu2=item1;}
    }
  if( item1 == TrigTauFlags::MET20 ||  item2 == TrigTauFlags::MET20 )
    {met = 20; menuWithMET = true;
      if(!tau1 && item2 == TrigTauFlags::MET20 ){menu1=item2; menu2=item1;}
    }
  if( item1 == TrigTauFlags::MET25 ||  item2 == TrigTauFlags::MET25 )
    {met = 25; menuWithMET = true;
      if(!tau1 && item2 == TrigTauFlags::MET25 ){menu1=item2; menu2=item1;}
    }
  if( item1 == TrigTauFlags::MET30 ||  item2 == TrigTauFlags::MET30 )
    {met = 30; menuWithMET = true;
      if(!tau1 && item2 == TrigTauFlags::MET30 ){menu1=item2; menu2=item1;}
    }
  if( item1 == TrigTauFlags::MET40 ||  item2 == TrigTauFlags::MET40 )
    {met = 40; menuWithMET = true;
      if(!tau1 && item2 == TrigTauFlags::MET40 ){menu1=item2; menu2=item1;}
    }
  if( item1 == TrigTauFlags::MET50 ||  item2 == TrigTauFlags::MET50 )
    {met = 50; menuWithMET = true;
      if(!tau1 && item2 == TrigTauFlags::MET50 ){menu1=item2; menu2=item1;}
    }
//   if( item1 == TrigTauFlags::MET70 ||  item2 == TrigTauFlags::MET70 )
//     {met = 70; menuWithMET = true;
//       if(!tau1 && item2 == TrigTauFlags::MET70 ){menu1=item2; menu2=item1;}
//     }
//   if( item1 == TrigTauFlags::MET80 ||  item2 == TrigTauFlags::MET80 )
//     {met = 80; menuWithMET = true;
//       if(!tau1 && item2 == TrigTauFlags::MET80 ){menu1=item2; menu2=item1;}
//    }


  // Also set an L1 MET cut if desired
  analyzer.trigMan.setL1MET(met*1000.);

  // Set up our first selection
  if( tau1 ) trigMan1.setTauMenu(menu1);

  // Now, look at our second selection
  if( tau2 ) trigMan2.setTauMenu(menu2);

//   cout << " menu1 " << menu1 << endl;
//   trigMan1.print();
//   cout << " menu2 " << menu2 << endl;
//   trigMan2.print();

}

void TrigTauDoubleTrigger::pass(const TrigTauEvent* event, 
				bool & passL1, bool &  passL2, bool & passEF)
{
  if( !menuWithTau ) return passNoTauSignature(event,  passL1, passL2, passEF);

  passL1 = false;
  passL2 = false;
  passEF = false;

  // this is for menus with taus
  analyzer.chainList.fillChainList(event);

  bool pass = false;
  //analyzer.chainList.printChainList();

  // Look for L1 pass
  unsigned int numCh = analyzer.chainList.nTrigChains();
  for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
    TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);

      if (!trigMan1.passedL1(ch1)) continue;

      if (bothMenuWithTau) {

	// Loop over chain list looking for another pass
	for (unsigned int iCh2=0; iCh2 < numCh; iCh2++) {
	  if (iCh1 == iCh2) continue;
	  TrigTauTrigChain ch2 = analyzer.chainList.trigChainEntry(iCh2);
	  if (!trigMan2.passedL1(ch2)) continue;
	  pass = true;
	}

	if (pass) break;

      } else if( !menuWithMET ) {

	// Loop over the extra ROI list
	unsigned int nROI = event->nOtherTrigRoI();
	for (unsigned int iROI = 0; iROI<nROI; iROI++) {
	  TrigTauRoI roi = event->otherTrigRoIEntry(iROI);

	  if (!roi.m_tags.passedL1(menu2)) continue;
	  if (roi.DeltaR(*(ch1.L1)) < 0.2) continue;
	  pass = true;
	}

	if (pass) break;

      }      
  } // end of L1 chain

  passL1 = pass;
  if( !passL1 ) return;

    // Look for L2 pass
  pass = false;
  for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
    TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);
    

    if (!trigMan1.passedL2(ch1)) continue;

    if (bothMenuWithTau) {
      
      // Loop over chain list looking for another pass
      for (unsigned int iCh2=0; iCh2 < numCh; iCh2++) {
	if (iCh1 == iCh2) continue;
	TrigTauTrigChain ch2 = analyzer.chainList.trigChainEntry(iCh2);
	if (!trigMan2.passedL2(ch2)) continue;
	pass = true;
	}
      
      if (pass) break;

    } else if(  !menuWithMET) {

	// Loop over the extra ROI list
	unsigned int nROI = event->nOtherTrigRoI();
	for (unsigned int iROI = 0; iROI<nROI; iROI++) {
	  TrigTauRoI roi = event->otherTrigRoIEntry(iROI);

	  if (!roi.m_tags.passedL2(menu2)) continue;
	  if (roi.DeltaR(*(ch1.L2Cal)) < 0.2) continue;
	  pass = true;
	}

	if (pass) break;

      }

    }

  passL2 = pass;
  if( !passL2 ) return;
 
    // Look for EF pass
    pass = false;
    for (unsigned int iCh1=0; iCh1 < numCh; iCh1++) {
      TrigTauTrigChain ch1 = analyzer.chainList.trigChainEntry(iCh1);

      if (!trigMan1.passedEF(ch1)) continue;

      if (bothMenuWithTau) {

	// Loop over chain list looking for another pass
	for (unsigned int iCh2=0; iCh2 < numCh; iCh2++) {
	  if (iCh1 == iCh2) continue;
	  TrigTauTrigChain ch2 = analyzer.chainList.trigChainEntry(iCh2);
	  if (!trigMan2.passedEF(ch2)) continue;
	  pass = true;
	}

	if (pass) break;

      } else if ( !menuWithMET ) {

	// Loop over the extra ROI list
	unsigned int nROI = event->nOtherTrigRoI();
	for (unsigned int iROI = 0; iROI<nROI; iROI++) {
	  TrigTauRoI roi = event->otherTrigRoIEntry(iROI);

	  if (!roi.m_tags.passedEF(menu2)) continue;
	  if (roi.DeltaR(*(ch1.EF)) < 0.2) continue;
	  pass = true;
	}

	if (pass) break;

      }

    }

    passEF = pass;
    return;
}

void TrigTauDoubleTrigger::passNoTauSignature(const TrigTauEvent* event, 
					      bool & passL1, bool &  passL2, bool & passEF)
{
  if( menuWithMET ){
    passL1 = event->decision.passedL1(menu1) &&  event->decision.passedL1(menu2);
    passL2 = event->decision.passedL1(menu1) &&  event->decision.passedL2(menu2);
    passEF = event->decision.passedL1(menu1) &&  event->decision.passedEF(menu2);
  }else{
    passL1 = event->decision.passedL1(menu1) &&  event->decision.passedL1(menu2);
    passL2 = event->decision.passedL2(menu1) &&  event->decision.passedL2(menu2);
    passEF = event->decision.passedEF(menu1) &&  event->decision.passedEF(menu2);
  }
  return;
}
