/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __XAOD2NTUPLUMISVC__
#define __XAOD2NTUPLUMISVC__

///Use this service to write out luminosity bookkeeping information
///to ntuples (managed by THistSvc) that can then be used with
///getLumi.exe to check luminosity covered
///
///Usage ... only needed when running on data:
/// theApp.CreateSvc += ['xAOD2NtupLumiSvc']
/// svcMgr.MetaDataSvc.MetaDataTools += [ "LumiBlockMetaDataTool" ]



#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"




class xAOD2NtupLumiSvc : public AthService,
                             virtual public IIncidentListener {

public:
  xAOD2NtupLumiSvc(const std::string& name, ISvcLocator *svcLoc);

  virtual void handle(const Incident& inc);

  virtual StatusCode initialize();

  StatusCode finish();


};


#endif

