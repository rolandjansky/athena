/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  ExclusiveJets.h
//  TopoCore
//  Created by Carlos Moreno based on InvariantMassInclusive1 by Joerg Stelzer on 11/16/12.
#ifndef __TopoCore__ExclusiveJets__
#define __TopoCore__ExclusiveJets__
#include <iostream>
#include "L1TopoInterfaces/DecisionAlg.h"
namespace TCS {
   
   class ExclusiveJets : public DecisionAlg {
   public:
      ExclusiveJets(const std::string & name);
      virtual ~ExclusiveJets();
      virtual StatusCode initialize() override final;
      virtual StatusCode processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison ) override final;
      
      virtual StatusCode process( const std::vector<TCS::TOBArray const *> & input,
                                  const std::vector<TCS::TOBArray *> & output,
                                  Decision & decison ) override final;
      
   private:
      parType_t      p_NumberLeading1 = { 0 };
      parType_t      p_MinET1[6] = { 0 ,0,0,0,0,0};
      parType_t      p_XiMin[6] = { 0,0,0,0,0,0 };
      parType_t      p_XiMax[6] = { 0,0,0,0,0,0 };
      parType_t      p_ApplyEtaCut[6] = { 0,0,0,0,0,0 };
      parType_t      p_MinEta1[6] = { 0,0,0,0,0,0 };
      parType_t      p_MaxEta1[6] = { 0,0,0,0,0,0 };
      parType_t      p_MinEta2[6] = { 0,0,0,0,0,0 };
      parType_t      p_MaxEta2[6] = { 0,0,0,0,0,0 };
      TH2F * m_histAcceptExclusiveJets[6] = {};
      TH2F * m_histRejectExclusiveJets[6] = {};
      TH2F * m_histAcceptEta1Eta2[6] = {};
      TH2F * m_histRejectEta1Eta2[6] = {};
   };
   
}
#endif
