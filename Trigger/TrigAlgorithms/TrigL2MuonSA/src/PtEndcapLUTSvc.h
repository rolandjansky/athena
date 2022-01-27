/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTENDCAPLUTSVC_H
#define TRIGL2MUONSA_PTENDCAPLUTSVC_H

#include "AthenaBaseComps/AthService.h"

#include "GaudiKernel/ToolHandle.h"

#include "PtEndcapLUT.h"

#include <map>
#include <cstring>

namespace TrigL2MuonSA {

class PtEndcapLUTSvc : public AthService
{
  public:
    // import IService as this service does not define its own interface:
    using IService::interfaceID;

    PtEndcapLUTSvc(const std::string& name, ISvcLocator* sl);

    virtual StatusCode initialize() override;

  private:
    Gaudi::Property< std::string >    m_lut_fileNameRun3 {
	this, "FileNameRun3", "pt_endcap_run3.lut", ""};
    Gaudi::Property< std::string >    m_lut_fileNameRun2 {
	this, "FileNameRun2", "pt_endcap_run2.lut", ""};
    Gaudi::Property< std::string >    m_lut_fileName {
	this, "FileName", "pt_endcap.lut", ""};
    Gaudi::Property< std::string >    m_lut_mean {
	this, "EMeanLUT", "pt_comb_mean.lut", ""};
    Gaudi::Property< std::string >    m_lut_sigma {
	this, "ESigmaLUT", "pt_comb_sigma.lut", ""};
    Gaudi::Property<bool>             m_useRun3LUT{
	this,"UseRun3LUT",true,"Select LUT at geometry"};

    ToolHandle<PtEndcapLUT> m_ptEndcapLUT{this, "PtEndcapLUT", "TrigL2MuonSA::PtEndcapLUT"};

  public:
    const ToolHandle<PtEndcapLUT>* ptEndcapLUT(void) const
    {return &m_ptEndcapLUT;};

};

}

#endif
