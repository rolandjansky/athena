/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodness_Loose_20100415.h"
#include "MissingETGoodness/TSelectionSetArchive.h"
#include "CxxUtils/unused.h"

ClassImp(MET::EtmissGoodness_Loose_20100415)


MET::EtmissGoodness_Loose_20100415::EtmissGoodness_Loose_20100415()
 : TSelectionSet(MET::Loose,20100415)
{
}


Bool_t
MET::EtmissGoodness_Loose_20100415::CreateSelection() 
{
  Bool_t status(kTRUE);

  // Add selections here.
  MET::TSelection sel;

  // these are actually *selected events*, not rejected
  sel.AddCut("(N90Cells_Jet0<-1 || N90Cells_Jet0>5) || (EMFraction_Jet0<-1 || EMFraction_Jet0>=0.2)");
  sel.AddCut("(N90Cells_Jet1<-1 || N90Cells_Jet1>5) || (EMFraction_Jet1<-1 || EMFraction_Jet1>=0.2)");
  sel.AddCut("QualityFrac_Jet0<=0.8 || EMFraction_Jet0<=0.95");
  sel.AddCut("QualityFrac_Jet1<=0.8 || EMFraction_Jet1<=0.95");
  //sel.AddCut("SamplingMaxID_Jet0!=17");
  //sel.AddCut("SamplingMaxID_Jet1!=17");
  sel.AddCut("Timing_Jet0==-999 || abs(Timing_Jet0)<50");
  sel.AddCut("Timing_Jet1==-999 || abs(Timing_Jet1)<50");

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
    MET::TSelectionSetArchive::instance().RegisterSelectionSetClass(MET::Loose,20100415,"MET::EtmissGoodness_Loose_20100415");
}

