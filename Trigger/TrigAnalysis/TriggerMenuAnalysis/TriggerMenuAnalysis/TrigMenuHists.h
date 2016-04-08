/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_TRIGMENUHISTS_H
#define TRIGGERMENUANALYSIS_TRIGMENUHISTS_H
/*
  TrigMenuHists.h
*/
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"
#include <vector>
#include "TH1.h"

class TrigMenuHists : public AthAlgTool {
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
  ServiceHandle<ITHistSvc> m_THistSvc;
  TH1* m_histMuFastPt;
  TH1* m_histMuInDetPt;
  TH1* m_histMuCombPt;
};

#endif // TRIGGERMENUANALYSIS_TRIGMENUHISTS_H
