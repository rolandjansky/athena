/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     T2CaloTileMon.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTileMon

 AUTHOR:   D.O. Damazion Jens Damaske

 PURPOSE:  Based on T2CaloBase works as main driver for the LVL2
      Calorimeter TileMon Algorithm. Receives and decodes LVL1
      information (RoI position) and just makes some monitoring
 *******************************************************************/

#ifndef TRIGT2CALOTILEMON_T2CALOTILEMON_H
#define TRIGT2CALOTILEMON_T2CALOTILEMON_H

#include <string>
#include "TrigT2CaloCommon/T2CaloBase.h"
#include "GaudiKernel/ToolHandle.h"

class IAlgToolCalo;
class T2CaloEgammaMon;
class MsgStream;
class TrigEMCluster;
class TrigT2TileJet;
class TrigT2Tower;

/** Main LVL2 Algorithm. Processes LVL1 information, call FEX
    IAlgToolCalos and produces the TrigEMCluster output. */
class T2CaloTileMon: public T2CaloBase {

   public:
      /** Constructor */
      T2CaloTileMon(const std::string & name, ISvcLocator* pSvcLocator);
      /** Destructor */
      ~T2CaloTileMon();

      /** hltExecute will call FEX IAlgToolCalo::execute() to process RoI.
      called by the Steering per EMRoI. */
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE,
               HLT::TriggerElement* outputTE);
      /** hltInitialize. Called by the Steering. */
      HLT::ErrorCode hltInitialize();
      /** hltFinalize. Called by the Steering. */
      HLT::ErrorCode hltFinalize();

   private:
      /** To support new monitoring. Values must be copied to
      this monitored Cluster for each RoI. */
      //const TrigEMCluster* m_monitoredCluster;
      /** To monitor rCore */
      //float m_rCore;
      /** To monitor eRatio */
      //float m_eRatio;
      /** Forced LVL1 eta */
      float m_l1eta;
      /** Forced LVL1 phi */
      float m_l1phi;

      //int m_index;

      // Properties:
      std::string m_trigEmClusterKey;
      /** Will monitoring be used or not */
      //bool m_mon;
      /** Coordinates to be used by the tools (in order by tool) */
      std::vector<float> m_etamin;
      std::vector<float> m_etamax;
      std::vector<float> m_phimin;
      std::vector<float> m_phimax;
};

#endif
