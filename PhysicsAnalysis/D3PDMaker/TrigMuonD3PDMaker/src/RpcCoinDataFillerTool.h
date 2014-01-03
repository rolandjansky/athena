// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGMUOND3PDMAKER_RPCCOINDATAFILLERTOOL_H
#define TRIGMUOND3PDMAKER_RPCCOINDATAFILLERTOOL_H

// EDM include(s):
#include "MuonTrigCoinData/RpcCoinData.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

// Forward declaration(s):
namespace MuonGM {
   class MuonDetectorManager;
}
class RpcIdHelper;

namespace D3PD {

   /**
    *  @short Tool filling information about RpcCoinData objects into the D3PD
    *
    * @author Takayuki Kanno
    *
    * $Revision: 341143 $
    * $Date$
    */
   class RpcCoinDataFillerTool : public BlockFillerTool< Muon::RpcCoinData > {

   public:
      /// Regular AlgTool constructor
      RpcCoinDataFillerTool( const std::string& type, const std::string& name,
                             const IInterface* parent );

      /// Algorithm initializer
      virtual StatusCode initialize();
      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const Muon::RpcCoinData& rpcelem );

   private:
      const MuonGM::MuonDetectorManager* m_muonManager;
      const RpcIdHelper* m_rpcIdHelper;

      float* m_time;
      int* m_threshold;
      int* m_isLowPtCoin;
      int* m_isHighPtCoin;
      int* m_isLowPtToHighPtCm;
      int* m_overlap;
      int* m_ijk;
      int* m_parentCmId;
      int* m_parentPadId;
      int* m_parentSectorId;
      //Values from RpcIdHelper
      int* m_station;
      int* m_eta;
      int* m_phi;
      int* m_doubletR;
      int* m_doubletZ;
      int* m_doubletPhi;
      int* m_measuresPhi;
      int* m_strip;
      //Values from MuonManager
      float* m_stripX;
      float* m_stripY;
      float* m_stripZ;
      float* m_stripEta;
      float* m_stripPhi;
 
   }; // class RpcCoinDataFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_RPCCOINDATAFILLERTOOL_H
