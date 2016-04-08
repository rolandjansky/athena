/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISEXAMPLES_MYCUTCLASS_H
#define PYANALYSISEXAMPLES_MYCUTCLASS_H

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "egammaEvent/Electron.h"

class MyCutClass
{
public:
  MyCutClass (double thr) : m_Thr(thr) {}
  virtual ~MyCutClass () {}  
  
  bool isAccepted (const Analysis::Electron &e)
  {
    MsgStream log(Athena::getMessageSvc(), "MyCutClass");
    log << MSG::DEBUG << e.pt() << endreq;
    if (e.pt() > m_Thr) return true;
    return false;
  }
  
private:
  double m_Thr;
};

#endif
