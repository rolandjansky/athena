// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RpcPadFillerTool.h 439443 2011-05-26 01:35:49Z kanno $
#ifndef TRIGMUOND3PDMAKER_RPCPADFILLERTOOL_H
#define TRIGMUOND3PDMAKER_RPCPADFILLERTOOL_H

// EDM include(s):
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"

//Local includes
#include "RpcPadContainerAssociationTool.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

#include <vector>

// Forward declaration(s):
namespace MuonGM {
   class MuonDetectorManager;
}
class RpcIdHelper;

namespace D3PD {

   /**
    *  @short Tool filling information about RpcPad objects into the D3PD
    *
    * @author Lorenzo Bellagamba
    *
    * $Revision: 439443 $
    * $Date: 2011-05-26 03:35:49 +0200 (Thu, 26 May 2011) $
    */
   class RpcPadFillerTool : public BlockFillerTool< RpcTree > {

   public:
      /// Regular AlgTool constructor
      RpcPadFillerTool( const std::string& type, const std::string& name,
                             const IInterface* parent );

      /// Algorithm initializer
      virtual StatusCode initialize();
      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const RpcTree& rpcelem );

   private:
      const MuonGM::MuonDetectorManager* m_muonManager;
      const RpcIdHelper* m_rpcIdHelper;

      int* m_sector;
      int* m_padId;
      int* m_status;
      int* m_ercode;
      int* m_cmaId;
      int* m_fel1Id;
      int* m_febcId;
      int* m_crc;
      int* m_bcId;
      int* m_ticks;
      int* m_ijk;
      int* m_cmachan;
      int* m_overlap;
      int* m_threshold;
   }; // class RpcPadFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_RPCPADFILLERTOOL_H
