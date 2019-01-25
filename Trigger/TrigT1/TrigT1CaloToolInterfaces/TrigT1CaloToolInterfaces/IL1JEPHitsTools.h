/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1JEPHitsTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1JEPHITSTOOLS_H
#define ILVL1L1JEPHITSTOOLS_H

#include "GaudiKernel/IAlgTool.h"


namespace LVL1 
{

class CMMJetHits;
class JEMHits;
class JEMRoI;
class JetAlgorithm;
class JetROI;

/**
Interface definition for L1JEPHitsTools
*/
  static const InterfaceID IID_IL1JEPHitsTools("LVL1::IL1JEPHitsTools", 1, 0);

  class IL1JEPHitsTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void formJEMRoI(const DataVector<JetAlgorithm>* jetAlgorithmVec,
                                  DataVector<JEMRoI>*   jemRoiVec) const = 0;
    virtual void formJEMRoI(const DataVector<JetROI>*   jetRoiVec,
                                  DataVector<JEMRoI>*   jemRoiVec) const = 0;
    virtual void formJEMHits(const DataVector<JEMRoI>*  jemRoiVec,
                                   DataVector<JEMHits>* jemHitsVec) const = 0;
    virtual void formJEMHits(
                 const std::vector<const DataVector<JEMRoI>*>& jemRoiColls,
                 DataVector<JEMHits>* jemHitsVec, int peak) const = 0;
    virtual void formCMMJetHits(const DataVector<JEMHits>* jemHitsVec,
                                DataVector<CMMJetHits>* cmmHitsVec) const = 0;
    virtual void formCMMJetHitsModule(const DataVector<JEMHits>* jemHitsVec,
                                DataVector<CMMJetHits>* cmmHitsMod) const = 0;
    virtual void formCMMJetHitsCrate(const DataVector<CMMJetHits>* cmmHitsMod,
                                DataVector<CMMJetHits>* cmmHitsCrate) const = 0;
    virtual void formCMMJetHitsSystem(
                                const DataVector<CMMJetHits>* cmmHitsCrate,
                                DataVector<CMMJetHits>* cmmHitsSys) const = 0;
    virtual void formCMMJetHitsEtMap(const DataVector<CMMJetHits>* cmmHitsSys,
                                DataVector<CMMJetHits>* cmmHitsMap) const = 0;
      
  };

  inline const InterfaceID& LVL1::IL1JEPHitsTools::interfaceID()
    { 
      return IID_IL1JEPHitsTools;
    }

} // end of namespace

#endif 
