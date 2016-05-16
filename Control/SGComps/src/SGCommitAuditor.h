/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SGTOOLS_SGCOMMITAUDITOR_H
#define SGTOOLS_SGCOMMITAUDITOR_H

#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/IAuditor.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IHiveStoreMgr.h"

/////////////////////////////////////////////////////////////////////////
//
// SGCommitAuditor
//
// Auditor for use with GaudiHive, causes DataObjects recorded during
// the preceeding Algorithm's execute method to be made known to the
// WhiteBoard
//
// Author: C. Leggett
// Date: 2015-02-03
/////////////////////////////////////////////////////////////////////////

class SGCommitAuditor: virtual public Auditor {

public:
  SGCommitAuditor(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SGCommitAuditor();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual void afterExecute(INamedInterface* alg, const StatusCode&);

private:

  ServiceHandle<IHiveStoreMgr> p_sg;

};

#endif
