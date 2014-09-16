/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_PTENDCAPLUTSVC_H
#define TRIGL2MUONSA_PTENDCAPLUTSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

#include "TrigL2MuonSA/PtEndcapLUT.h"

#include <map>
#include <cstring>

namespace TrigL2MuonSA {

  class PtEndcapLUTSvc : public Service, virtual public IInterface
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

    MsgStream*        m_msg;
    inline MsgStream& msg() const { return *m_msg; }

  private:
    StringProperty    m_lut_fileName;

    PtEndcapLUT*      m_ptEndcapLUT;

  public:
    virtual const PtEndcapLUT* ptEndcapLUT(void) const;
  };

}

#endif
