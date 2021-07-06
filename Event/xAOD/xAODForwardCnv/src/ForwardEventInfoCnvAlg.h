// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef XAODFORWARDCNV_FORWARDEVENTINFOCNVALG_H
#define XAODFORWARDCNV_FORWARDEVENTINFOCNVALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TileEvent/MBTSCollisionTime.h"
#include "xAODForward/ForwardEventInfoContainer.h"
#include "xAODForwardCnv/IForwardEventInfoCnvTool.h"

namespace xAODMaker
{

  /**
    *  @short Algorithm creating xAOD::ForwardEventInfos from MBTSCollisionTime object
    */
  class ForwardEventInfoCnvAlg : public AthReentrantAlgorithm
  {

  public:
    ///@brief Regular algorithm constructor
    ForwardEventInfoCnvAlg(const std::string &name, ISvcLocator *svcLoc);

    ///@brief Function initialising the algorithm
    virtual StatusCode initialize() override;
    ///@brief Function executing the algorithm
    virtual StatusCode execute(const EventContext &ctx) const override;

  private:
    SG::ReadHandleKey<MBTSCollisionTime> m_inputKey{this, "MBTSCollisionsTimeKey", "MBTSCollisionTime"};

    SG::WriteHandleKey<xAOD::ForwardEventInfoContainer> m_outputKey{this, "ForwardEventInfoKey", "MBTSForwardEventInfo"};

    ToolHandle<IForwardEventInfoCnvTool> m_cnvTool{this, "CnvTool", "xAODMaker::ForwardEventInfoCnvTool/ForwardEventInfoCnvTool"};

  }; // class ForwardEventInfoCnvAlg

} // namespace xAODMaker

#endif // XAODFORWARDCNV_FORWARDEVENTINFOCNVALG_H
