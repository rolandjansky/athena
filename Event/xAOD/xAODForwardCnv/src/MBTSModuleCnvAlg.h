// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef XAODFORWARDCNV_MBTSMODULECNVALG_H
#define XAODFORWARDCNV_MBTSMODULECNVALG_H

// System include(s):
#include <string>

// Athena/Gaudi include(s):
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODForwardCnv/IMBTSModuleCnvTool.h"
#include "xAODForward/MBTSModuleContainer.h"

namespace xAODMaker
{

  /**
    *  @short Algorithm creating xAOD::MBTSModules from TileCellContainer
    *
    *         This algorithm can be used to translate the TileCells coming
    *         from an AOD, and create xAOD::MBTSModule objects out of them
    *         for an output xAOD. Technically all it does is to call the converting tool.
    */
  class MBTSModuleCnvAlg : public AthReentrantAlgorithm
  {

  public:
    MBTSModuleCnvAlg(const std::string &name, ISvcLocator *svcLoc);

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &ctx) const override;

  private:
    SG::ReadHandleKey<TileCellContainer> m_inputKey{this, "MBTSContainerKey", "MBTSContainer"};

    SG::WriteHandleKey<xAOD::MBTSModuleContainer> m_outputKey{this, "AODContainerKey", "MBTSModules"};

    /** @brief Handle to the converter tool */
    ToolHandle<IMBTSModuleCnvTool> m_cnvTool{this, "CnvTool", "xAODMaker::MBTSModuleCnvTool/MBTSModuleCnvTool", "The converter tool for MBTSModules"};

  }; // class MBTSModuleCnvAlg

} // namespace xAODMaker

#endif // XAODFORWARDCNV_MBTSMODULECNVALG_H
