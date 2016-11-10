/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/EtmissGoodness_Loose_20100217.h"
#include "MissingETGoodness/TSelectionSetArchive.h"
#include "CxxUtils/unused.h"

ClassImp(MET::EtmissGoodness_Loose_20100217)


MET::EtmissGoodness_Loose_20100217::EtmissGoodness_Loose_20100217()
 : TSelectionSet(MET::Loose,20100217)
{
}


Bool_t
MET::EtmissGoodness_Loose_20100217::CreateSelection() 
{
  Bool_t status(kTRUE);

  // Add selections here.
  // ...

  MET::TSelection sel;
  sel.AddCut("N90Cells_Jet0<0 || N90Cells_Jet0>2");
  sel.AddCut("N90Cells_Jet1<0 || N90Cells_Jet1>2");
  sel.AddCut("DeltaPhi_MET_Jet0<2.8");
  sel.AddCut("EventEMFraction<0 || EventEMFraction>0.1");
  sel.AddCut("EMFraction_Jet0<0 || EMFraction_Jet0>0.1");
  sel.AddCut("EMFraction_Jet1<0 || EMFraction_Jet1>0.1");
  sel.AddCut("NConstituents_Jet0<0 || NConstituents_Jet0>4");
  sel.AddCut("NConstituents_Jet1<0 || NConstituents_Jet1>4");
  sel.AddCut("NumTracks_Jet0<0 || NumTracks_Jet0>1");
  sel.AddCut("NumTracks_Jet1<0 || NumTracks_Jet1>1");
  sel.AddCut("ChargeFraction_Jet0<0 || ChargeFraction_Jet0>0.02");
  sel.AddCut("ChargeFraction_Jet1<0 || ChargeFraction_Jet1>0.02");
  sel.AddCut("fcor_Jet0<0.2");
  sel.AddCut("fcor_Jet1<0.2");
  sel.AddCut("SamplingMaxID_Jet0!=17");
  sel.AddCut("SamplingMaxID_Jet1!=17");

  this->AddSelection(sel);

/*
  // Example:

  MET::TSelection foo(100,1000); // runrange
  foo.AddCut("runNr>200");
  foo.AddCut("bar<20");

  MET::TSelection bar(1100,11000);
  bar.AddCut("runNr>2000");
  bar.AddCut("bar<200");
*/

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
    MET::TSelectionSetArchive::instance().RegisterSelectionSetClass(MET::Loose,20100217,"MET::EtmissGoodness_Loose_20100217");
}

