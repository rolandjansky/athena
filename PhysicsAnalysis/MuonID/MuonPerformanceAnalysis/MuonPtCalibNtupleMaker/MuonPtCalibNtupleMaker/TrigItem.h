/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigItem_h__
#define __TrigItem_h__

//::: STL includes
#include <string>

//::: ROOT includes
#include <TTree.h>

class TrigItem {

private:
  std::string m_Name;
  Int_t       m_Decision;
  Int_t       m_Matching;
  Int_t       m_NumberOfMuons;

public:
  TrigItem( std::string name, Int_t n_muons );
  ~TrigItem() {}

  void Reset();
  void Register( TTree* t );
  void SetDecision( Int_t dec ) { m_Decision = dec; }
  void SetMatching( Int_t dec ) { m_Matching = dec; }

  Int_t GetNumberOfMuons() { return m_NumberOfMuons; }
  
};

#endif
