/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef STANDALONEL1TOPOHISTSVC_H
#define STANDALONEL1TOPOHISTSVC_H

#include <memory>

#include "L1TopoInterfaces/IL1TopoHistSvc.h"

class StandaloneL1TopoHistSvc : public IL1TopoHistSvc
{
public:
   StandaloneL1TopoHistSvc();
   virtual ~StandaloneL1TopoHistSvc();

   virtual void registerHist(TH1 * h) override;

   virtual void registerHist(TH2 * h) override;

   virtual TH1 * findHist(const std::string & histName) override;

   virtual void fillHist1D(const std::string & histName, double x) override;

   virtual void fillHist2D(const std::string & histName, double x, double y) override;

   virtual void setBaseDir(const std::string & baseDir) override;

   virtual void save() override;

private:

   class StandaloneL1TopoHistSvcImpl;
   std::unique_ptr<StandaloneL1TopoHistSvcImpl> m_impl;

};

#endif
