/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTBARRELLUTSVC_H
#define TRIGL2MUONSA_PTBARRELLUTSVC_H

#include <string>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigL2MuonSA/PtBarrelLUT.h"

namespace TrigL2MuonSA {
  
  class PtBarrelLUTSvc : public AthService, virtual public IInterface
  {
  public:
    static const InterfaceID& interfaceID() { 
      static const InterfaceID _IID(11496, 0 , 0);
      return _IID;
    }

  public:
    PtBarrelLUTSvc(const std::string& name,ISvcLocator* sl);
    virtual ~PtBarrelLUTSvc() {}
    
    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);
    
    virtual StatusCode initialize(void);
    virtual StatusCode finalize(void);

  private:
    StringProperty m_lut_fileName;
    StringProperty m_lutSP_fileName;

    ToolHandle<PtBarrelLUT>  m_ptBarrelLUT;

  public:
    const ToolHandle<PtBarrelLUT>* ptBarrelLUT(void) const
    {return &m_ptBarrelLUT;};
    
  };

}
  
#endif 
