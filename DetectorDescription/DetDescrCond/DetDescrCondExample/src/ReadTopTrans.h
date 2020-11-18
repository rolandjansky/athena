/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDEXAMPLE_READTOPTRANS_H
#define DETDESCRCONDEXAMPLE_READTOPTRANS_H
// ReadTopTrans - simple algorithm to demonstrate reading of DetCondKeyTrans
// object TOPTRANS for global geometry
// Richard Hawkings, started 24/6/05

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/IOVSvcDefs.h"


class ReadTopTrans: public AthAlgorithm, virtual public IIncidentListener
{
 public:
  ReadTopTrans(const std::string& name, ISvcLocator* pSvcLocator);
  ~ReadTopTrans(void);
  StatusCode initialize(void);
  StatusCode execute(void);
  StatusCode finalize(void);
  void handle(const Incident& incident);

  StatusCode testCallBack( IOVSVC_CALLBACK_ARGS );

};

#endif // DETDESCRCONDEXAMPLE_READTOPTRANS_H
