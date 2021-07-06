/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTBARRELLUTSVC_H
#define TRIGL2MUONSA_PTBARRELLUTSVC_H

#include <string>

#include "AthenaBaseComps/AthService.h"

#include "GaudiKernel/ToolHandle.h"

#include "PtBarrelLUT.h"

namespace TrigL2MuonSA {

  class PtBarrelLUTSvc : public AthService
  {
  public:
    // import IService as this service does not define its own interface:
    using IService::interfaceID;

    PtBarrelLUTSvc(const std::string& name,ISvcLocator* sl);

    virtual StatusCode initialize(void) override;

  private:
    Gaudi::Property< std::string > m_lut_fileName {
	this, "LUTfile", "pt_barrel.lut", ""};
    Gaudi::Property< std::string > m_lutSP_fileName {
	this, "SP_LUTfile", "pt_barrelSP_new.lut", ""};

    ToolHandle<PtBarrelLUT>  m_ptBarrelLUT {
      this, "PtBarrelLUT", "TrigL2MuonSA::PtBarrelLUT"};

  public:
    const ToolHandle<PtBarrelLUT>* ptBarrelLUT(void) const
    {return &m_ptBarrelLUT;};
    
  };

}

#endif
