/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodness_Loose_20100408.h"
#include "MissingETGoodness/TSelectionSetArchive.h"
#include "CxxUtils/unused.h"

ClassImp(MET::EtmissGoodness_Loose_20100408)


MET::EtmissGoodness_Loose_20100408::EtmissGoodness_Loose_20100408()
 : TSelectionSet(MET::Loose,20100408)
{
}


Bool_t
MET::EtmissGoodness_Loose_20100408::CreateSelection() 
{
  Bool_t status(kTRUE);

  // Add selections here.
  // ...

  MET::TSelection sel;

  sel.AddCut("N90Cells_Jet0<-998 || N90Cells_Jet0>2");
  sel.AddCut("N90Cells_Jet1<-998 || N90Cells_Jet1>2");
//  sel.AddCut("DeltaPhi_MET_Jet0<-998 || DeltaPhi_MET_Jet0<2.8"); // rejects a  lot of W MC...
  sel.AddCut("EventEMFraction<-998  || (EventEMFraction>0.1 && EventEMFraction<0.1.4)");
  sel.AddCut("EMFraction_Jet0<-998 || EMFraction_Jet0>0.1");
  sel.AddCut("EMFraction_Jet1<-998 || EMFraction_Jet1>0.1");
  sel.AddCut("NConstituents_Jet0<0 || NConstituents_Jet0>4");
  sel.AddCut("NConstituents_Jet1<0 || NConstituents_Jet1>4");
  sel.AddCut("NumTracks_Jet0<0 || NumTracks_Jet0>1");
  sel.AddCut("NumTracks_Jet1<0 || NumTracks_Jet1>1");
  sel.AddCut("ChargeFraction_Jet0<-998 || ChargeFraction_Jet0>0.02");
  sel.AddCut("ChargeFraction_Jet1<-998 || ChargeFraction_Jet1>0.02");
// sel.AddCut("fcor_Jet0<0.2");
// sel.AddCut("fcor_Jet1<0.2");
  sel.AddCut("SamplingMaxID_Jet0!=17");   
  sel.AddCut("SamplingMaxID_Jet1!=17");  
  sel.AddCut("CalClus_eventTime<-998 || ( CalClus_eventTime>= -4 && CalClus_eventTime<=4)");
  sel.AddCut("CalClusForwBackw_timeDiff<-998 || ( CalClusForwBackw_timeDiff>=-15 && CalClusForwBackw_timeDiff<=15)");
  sel.AddCut("CalClus_inTimeFraction<-998 || ( CalClus_inTimeFraction>=0.2 &&  CalClus_inTimeFraction<=1.2)");

  // harder version of the same cut: rejects <<1e-5 of any MC sample, but
  // rejects 70% of scrapes and 10% of min bias stream. ( May not be safe for min-bias analysis? )
//  sel.AddCut("CalClus_inTimeFraction<-998 || ( CalClus_inTimeFraction>=0.75 &&  CalClus_inTimeFraction<=1.1)");
  sel.AddCut("CalClus_eventTime<-998 || ( CalClus_eventTime>=-5 && CalClus_eventTime<=5)");
  sel.AddCut("SamplingFracTile_Jet0 <0.95");
  sel.AddCut("SamplingFracTile10_Jet0<0.95");
  sel.AddCut("SamplingFracTile2_Jet0 <0.6");
// dublicated.  sel.AddCut("CalClusForwBackw_timeDiff<-998 || (CalClusForwBackw_timeDiff>=-15 && CalClusForwBackw_timeDiff<=15)");


/*
  // Example:

  MET::TSelection foo(100,1000); // runrange
  foo.AddCut("runNr>200");
  foo.AddCut("bar<20");

  MET::TSelection bar(1100,11000);
  bar.AddCut("runNr>2000");
  bar.AddCut("bar<200");
*/

  this->AddSelection(sel);
  
  this->SetMETKey("MET_Topo");
  this->SetJetKey("AntiKt4H1TopoJets");

  return status;
}


/*-------------------------------------------------------------------------
 *  Register the selection, automatically done when loading the library
 *-------------------------------------------------------------------------*/
namespace
{
  const bool UNUSED(dummy) =
    MET::TSelectionSetArchive::instance().RegisterSelectionSetClass(MET::Loose,20100408,"MET::EtmissGoodness_Loose_20100408");
}

