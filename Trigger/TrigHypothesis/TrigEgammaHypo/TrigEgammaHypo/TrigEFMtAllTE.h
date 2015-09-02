/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFMtAllTE.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigEgammaHypo
 *
 * AUTHOR:   Arantxa Ruiz Martinez
 * CREATED:  2 June 2015
 *
 ********************************************************************/

#ifndef TRIGEFMTALLTE_H
#define TRIGEFMTALLTE_H

#include <string>
#include <vector>

#include "TrigInterfaces/AllTEAlgo.h"

class TrigEFMtAllTE : public HLT::AllTEAlgo {

public:

  TrigEFMtAllTE(const std::string& name, ISvcLocator* svcloc);
  ~TrigEFMtAllTE();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(std::vector<HLT::TEVec>& inputs, unsigned int type_out);
  HLT::ErrorCode hltFinalize();

  HLT::TriggerElement* makeOutputTE(std::vector<HLT::TEVec>& inputs, unsigned int output);

private:

  float m_MinMtCut;
  int m_MaxNbElectrons;
  float m_MinElectronEt;

  float m_mt_electron1;
  float m_mt_electron1_pass;

};

#endif
