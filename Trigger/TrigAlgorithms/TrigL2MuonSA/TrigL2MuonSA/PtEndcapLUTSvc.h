/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTENDCAPLUTSVC_H
#define TRIGL2MUONSA_PTENDCAPLUTSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigL2MuonSA/PtEndcapLUT.h"

#include <map>
#include <cstring>

namespace TrigL2MuonSA {

class PtEndcapLUTSvc : public AthService, virtual public IInterface
{

  public:
    static const InterfaceID& interfaceID() { 
      static const InterfaceID _IID(11497, 0 , 0);
      return _IID;
    }

  public:
    PtEndcapLUTSvc(const std::string& name, ISvcLocator* sl);
    virtual ~PtEndcapLUTSvc() {}
    
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

    virtual StatusCode initialize();
    virtual StatusCode finalize();

  private:
    StringProperty    m_lut_fileName;
    StringProperty    m_lut_mean;
    StringProperty    m_lut_sigma;

    ToolHandle<PtEndcapLUT>      m_ptEndcapLUT;

  public:
    const ToolHandle<PtEndcapLUT>* ptEndcapLUT(void) const
    {return &m_ptEndcapLUT;};

};

}

#endif
