/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauEtaPhiPtTool.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2Tau
// 
// AUTHOR:   M.P. Casado
// 
// ********************************************************************
 
#ifndef TRIGT2TAU_T2TAUETAPHIPTTOOL_H 
#define TRIGT2TAU_T2TAUETAPHIPTTOOL_H
 
#include "TrigT2Tau/IAlgToolTau.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigTauCluster;
class TrigTauTracksInfo;
class TrigTau;

/** Tool which sets Eta, Phi, Pt  of TrigTau candidate */
class T2TauEtaPhiPtTool: public IAlgToolTau, public AthAlgTool{
  public:
  /** constructor */
    T2TauEtaPhiPtTool(const std::string & type, const std::string & name, 
               const IInterface* parent);
  /** destructor */
    virtual ~T2TauEtaPhiPtTool();
  /** initialize */
    StatusCode initialize();
  /** finalize */
    StatusCode finalize();
  /**  sets Eta, Phi, Pt  of TrigTau candidate */
    StatusCode execute(const TrigTauCluster *pTrigTauCluster,
		       const TrigTauTracksInfo* pTracks,
		       TrigTau& pTrigTau);
  private:
};
#endif
