/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTENDCAPLUTSVC_H
#define TRIGL2MUONSA_PTENDCAPLUTSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "PtEndcapLUT.h"

#include <map>
#include <cstring>

namespace TrigL2MuonSA {

class PtEndcapLUTSvc : public AthService, virtual public IInterface
{

  public:
    static const InterfaceID& interfaceID() { 
      static const InterfaceID IID(11497, 0 , 0);
      return IID;
    }

  public:
    PtEndcapLUTSvc(const std::string& name, ISvcLocator* sl);
    virtual ~PtEndcapLUTSvc() {}
    
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF) override;

    virtual StatusCode initialize() override;

  private:
    Gaudi::Property< std::string >    m_lut_fileNameRun2 {
	this, "FileNameRun2", "pt_endcap_run2.lut", ""};
    Gaudi::Property< std::string >    m_lut_fileName {
	this, "FileName", "pt_endcap.lut", ""};
    Gaudi::Property< std::string >    m_lut_mean {
	this, "EMeanLUT", "pt_comb_mean.lut", ""};
    Gaudi::Property< std::string >    m_lut_sigma {
	this, "ESigmaLUT", "pt_comb_sigma.lut", ""};

    ToolHandle<PtEndcapLUT> m_ptEndcapLUT{this, "PtEndcapLUT", "TrigL2MuonSA::PtEndcapLUT"};

  public:
    const ToolHandle<PtEndcapLUT>* ptEndcapLUT(void) const
    {return &m_ptEndcapLUT;};

};

}

#endif
