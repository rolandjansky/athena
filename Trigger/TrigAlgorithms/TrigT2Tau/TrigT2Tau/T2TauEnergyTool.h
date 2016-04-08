/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauEnergyTool.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2Tau
// 
// AUTHOR:   M.P. Casado
// 
// ********************************************************************
 
#ifndef TRIGT2TAU_T2TAUENERGYTOOL_H 
#define TRIGT2TAU_T2TAUENERGYTOOL_H
 
#include "TrigT2Tau/IAlgToolTau.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigTauCluster;
class TrigTauTracksInfo;
class TrigTau;

/** Tool which sets EtCalibCluster of TrigTau candidate */
class T2TauEnergyTool: public IAlgToolTau, public AthAlgTool{
  public:
  /** constructor */
    T2TauEnergyTool(const std::string & type, const std::string & name, 
               const IInterface* parent);
  /** destructor */
    virtual ~T2TauEnergyTool();
  /** initialization */
    StatusCode initialize();
  /** finalization */
    StatusCode finalize();
  /** sets EtCalibCluster of TrigTau candidate */
    StatusCode execute(const TrigTauCluster *pTrigTauCluster,
		       const TrigTauTracksInfo* pTracks,
		       TrigTau& pTrigTau);
  private:
};
#endif
