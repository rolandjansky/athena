/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauEtFlowTool.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2Tau
// 
// AUTHOR:   O.Igonkina (Nikhef)
// 
// ********************************************************************
 
#ifndef TRIGT2TAU_T2TAUETFLOWTOOL_H 
#define TRIGT2TAU_T2TAUETFLOWTOOL_H
 
#include "TrigT2Tau/IAlgToolTau.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigTauCluster;
class TrigTauTracksInfo;
class TrigTau;

/** Tool which sets EtFlow  of TrigTau candidate */
class T2TauEtFlowTool: public IAlgToolTau, public AthAlgTool{
  public:
  /** constructor */
    T2TauEtFlowTool(const std::string & type, const std::string & name, 
               const IInterface* parent);
  /** destructor */
    virtual ~T2TauEtFlowTool();
  /** initialize */
    StatusCode initialize();
  /** finalize */
    StatusCode finalize();
  /**  sets EtFlow  of TrigTau candidate */
    StatusCode execute(const TrigTauCluster *pTrigTauCluster,
		       const TrigTauTracksInfo* pTracks,
		       TrigTau& pTrigTau);
  private:
};
#endif
