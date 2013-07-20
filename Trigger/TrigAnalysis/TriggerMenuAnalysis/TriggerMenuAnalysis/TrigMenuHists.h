/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigMenuHists_h__
#define __TrigMenuHists_h__
/*
  TrigMenuHists.h
*/
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"
#include <vector>
#include "TH1.h"

class TrigMenuHists : public AlgTool {
public:
  static const InterfaceID& interfaceID();

public:
  TrigMenuHists(const std::string& type, const std::string& name, 
		const IInterface* p);
  ~TrigMenuHists();

  StatusCode initialize();
  StatusCode finalize();

  bool fillMuonObjects(const std::vector<HLTObjectsInRoI*>& objs);

private:
  ServiceHandle<ITHistSvc> mTHistSvc;
  TH1* mHistMuFastPt;
  TH1* mHistMuInDetPt;
  TH1* mHistMuCombPt;
};

#endif // __TrigMenuHists_h__
