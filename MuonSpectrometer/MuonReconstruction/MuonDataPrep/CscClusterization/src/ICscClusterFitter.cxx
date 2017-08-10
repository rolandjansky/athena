/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ICscClusterFitter.cxx
#include "CscClusterization/ICscClusterFitter.h"

typedef ICscClusterFitter::DataNames DataNames;

//const InterfaceID& ICscClusterFitter::interfaceID() {
//  return IID_ICscClusterFitter;
//}

const DataNames& ICscClusterFitter::dataNames() const {
  static DataNames dnames;
  return dnames;
}

// This function is for old-release 12.0.X and old CscClusterValidation package.
// It doesn't matter that we return the first result only.
// 12/11/07 fstrip and lstrip are added in Result class but they don't have to be included.
/*
int ICscClusterFitter::
fit(const StripList&, const StripFitList& sfits,
    double& pos, double& dpos, Muon::CscClusterStatus& clustatus,
    unsigned int& istrip, double& charge, double& time, DataMap* pdmap) const {
  Results res = fit(sfits);
  if ( res[0].fitStatus ) return res[0].fitStatus;
  pos = res[0].position;
  dpos = res[0].dposition;
  clustatus = res[0].clusterStatus;
  istrip = res[0].strip;
  charge = res[0].charge;
  time = res[0].time;
  if ( pdmap ) *pdmap = res[0].dataMap;
  return 0;
}
*/

