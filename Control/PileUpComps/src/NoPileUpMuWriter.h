/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_NOPILEUPMUWRITER_H
#define PILEUPCOMPS_NOPILEUPMUWRITER_H

/**
 * @class NoPileUpMuWriter
 *
 * @brief Algorithm to write interactions per crossing (mu) into xAOD::EventInfo as decorations
 *
 **/

#include <AthenaBaseComps/AthReentrantAlgorithm.h>
#include <StoreGate/WriteDecorHandleKey.h>
#include <xAODEventInfo/EventInfo.h>

class NoPileUpMuWriter : public AthReentrantAlgorithm 
{
public:
  NoPileUpMuWriter(const std::string& name, ISvcLocator* pSvcLocator);
  
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;
  
private:
  SG::WriteDecorHandleKey<xAOD::EventInfo> m_actMuKey {this
      ,"actualInteractionsPerCrossingKey"
      ,"EventInfo.actualInteractionsPerCrossing"
      ,"Decoration for Actual Interaction Per Crossing"};

  SG::WriteDecorHandleKey<xAOD::EventInfo> m_aveMuKey {this
      ,"averageInteractionsPerCrossingKey"
      ,"EventInfo.averageInteractionsPerCrossing"
      ,"Decoration for Average Interaction Per Crossing"};
};

#endif
