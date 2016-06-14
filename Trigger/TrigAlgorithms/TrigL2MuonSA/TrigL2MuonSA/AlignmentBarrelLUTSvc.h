/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_ALIGNMENTBARRELLUTSVC_H
#define TRIGL2MUONSA_ALIGNMENTBARRELLUTSVC_H

#include <string>
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrigL2MuonSA/AlignmentBarrelLUT.h"

namespace TrigL2MuonSA {
  
  class AlignmentBarrelLUTSvc : public AthService, virtual public IInterface
  {
  public:
    static const InterfaceID& interfaceID() { 
      static const InterfaceID _IID(11498, 0 , 0);
      return _IID;
    }

  public:
    AlignmentBarrelLUTSvc(const std::string& name,ISvcLocator* sl);
    virtual ~AlignmentBarrelLUTSvc() {}
    
    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);
    
    virtual StatusCode initialize(void);
    virtual StatusCode finalize(void);

  private:
    StringProperty m_lut_fileName;

    ToolHandle<AlignmentBarrelLUT>   m_alignmentBarrelLUT;
    
  public:
    const ToolHandle<AlignmentBarrelLUT>* alignmentBarrelLUT(void) const
    { return &m_alignmentBarrelLUT; };
  };

}
  
#endif 
