/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_ALIGNMENTBARRELLUTSVC_H
#define TRIGL2MUONSA_ALIGNMENTBARRELLUTSVC_H

#include <string>
#include "AthenaBaseComps/AthService.h"

#include "GaudiKernel/ToolHandle.h"

#include "AlignmentBarrelLUT.h"

namespace TrigL2MuonSA {

  class AlignmentBarrelLUTSvc : public AthService
  {
  public:
    // import IService as this service does not define its own interface:
    using IService::interfaceID;

    AlignmentBarrelLUTSvc(const std::string& name,ISvcLocator* sl);

    virtual StatusCode initialize() override;

  private:
    Gaudi::Property< std::string > m_lut_fileName {
      this, "LUTfile", "dZ_barrel.lut", ""};

    ToolHandle<AlignmentBarrelLUT>   m_alignmentBarrelLUT {
      this, "AlignmentBarrelLUT", "TrigL2MuonSA::AlignmentBarrelLUT/AlignmentBarrelLUT"};

  public:
    const ToolHandle<AlignmentBarrelLUT>* alignmentBarrelLUT(void) const
    { return &m_alignmentBarrelLUT; };
  };

}

#endif
