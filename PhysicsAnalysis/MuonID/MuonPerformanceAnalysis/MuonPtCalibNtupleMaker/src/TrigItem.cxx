/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define __TrigItem_cxx__
#include "MuonPtCalibNtupleMaker/TrigItem.h"

TrigItem::TrigItem( std::string name, Int_t n_muons ) {
  
  m_Name = name;
  m_NumberOfMuons = n_muons;
  Reset();

}

void TrigItem::Reset() {

  m_Decision = 0;
  m_Matching = 0;

}

void TrigItem::Register( TTree* t ) {

  
  t->Branch( ( "Trig_" + m_Name ).c_str(), &m_Decision );
  t->Branch( ( "Match_" + m_Name ).c_str(), &m_Matching );

}
