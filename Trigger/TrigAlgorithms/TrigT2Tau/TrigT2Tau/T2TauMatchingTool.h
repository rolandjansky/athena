/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     T2TauMatchingTool.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigT2Tau
// 
// AUTHOR:   M.P. Casado
// 
// ********************************************************************
 
#ifndef TRIGT2TAU_T2TAUMATCHINGTOOL_H 
#define TRIGT2TAU_T2TAUMATCHINGTOOL_H
 
#include "TrigT2Tau/IAlgToolTau.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TrigTauCluster;
class TrigTauTracksInfo;
class TrigTau;

/** Tool which propogates tracks to the calorimeneter surface and calculates 
number of tracks matched to the cluster*/
class T2TauMatchingTool: public IAlgToolTau, public AthAlgTool{
  public:
  /** constructor */
    T2TauMatchingTool(const std::string & type, const std::string & name, 
               const IInterface* parent);
  /** destructor */
    virtual ~T2TauMatchingTool();
  /** initialize */
    StatusCode initialize();
  /** finalize */
    StatusCode finalize();
  /** propogates tracks to the calorimeneter surface and calculates 
      number of tracks matched to the cluster*/
    StatusCode execute(const TrigTauCluster *pTrigTauCluster,
		       const TrigTauTracksInfo* pTracks,
		       TrigTau& pTrigTau);
  private:
  /** only tracks with pt > m_pTCut are counted for matching */
    double m_pTCut;
  /** only tracks within m_coneCut radius with respect to the cluster center
      are counted for matching */
    double m_coneCut;
};
#endif
