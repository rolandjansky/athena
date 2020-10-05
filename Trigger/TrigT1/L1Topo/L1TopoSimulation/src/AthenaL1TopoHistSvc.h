/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAL1TOPOHISTSVC_H
#define ATHENAL1TOPOHISTSVC_H

#include "GaudiKernel/ServiceHandle.h"
#include "L1TopoInterfaces/IL1TopoHistSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "GaudiKernel/LockedHandle.h"

#include <memory>

class AthenaL1TopoHistSvc : public IL1TopoHistSvc {
public:
   AthenaL1TopoHistSvc(ServiceHandle<ITHistSvc> histSvc);
   virtual ~AthenaL1TopoHistSvc();
   
   virtual void registerHist(TH1 * h) override;

   virtual void registerHist(TH2 * h) override;

   virtual TH1 * findHist(const std::string & histName) override;

   virtual void fillHist1D(const std::string & histName, double x) override;

   virtual void fillHist2D(const std::string & histName, double x, double y) override;

   virtual void setBaseDir(const std::string & baseDir) override;

   virtual void save() override;

private:

   class AthenaL1TopoHistSvcImpl;
   std::unique_ptr<AthenaL1TopoHistSvcImpl> m_impl;

};

#endif
