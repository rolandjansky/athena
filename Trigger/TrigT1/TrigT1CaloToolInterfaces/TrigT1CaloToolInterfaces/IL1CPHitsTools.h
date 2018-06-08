/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1CPHitsTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1CPHITSTOOLS_H
#define ILVL1L1CPHITSTOOLS_H

#include "GaudiKernel/IAlgTool.h"
//#include "TrigT1CaloUtils/CPAlgorithm.h"

namespace LVL1 
{

class CMMCPHits;
class CPAlgorithm;
class CPMHits;
class CPMRoI;
class EmTauROI;

/**
Interface definition for L1CPHitsTools
*/
  static const InterfaceID IID_IL1CPHitsTools("LVL1::IL1CPHitsTools", 1, 0);

  class IL1CPHitsTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void formCPMRoI(const DataVector<CPAlgorithm>* cpAlgorithmVec,
                                  DataVector<CPMRoI>*      cpmRoiVec) const = 0;
    virtual void formCPMRoI(const DataVector<EmTauROI>* emTauRoiVec,
                                  DataVector<CPMRoI>*   cpmRoiVec) const = 0;
    virtual void formCPMHits(const DataVector<CPMRoI>*  cpmRoiVec,
                                   DataVector<CPMHits>* cpmHitsVec) const = 0;
    virtual void formCPMHits(
                 const std::vector<const DataVector<CPMRoI>*>& cpmRoiColls,
                 DataVector<CPMHits>* cpmHitsVec, int peak) const = 0;
    virtual void formCMMCPHits(const DataVector<CPMHits>*   cpmHitsVec,
                               DataVector<CMMCPHits>* cmmHitsVec) const = 0;
    virtual void formCMMCPHitsModule(const DataVector<CPMHits>* cpmHitsVec,
                               DataVector<CMMCPHits>* cmmHitsMod) const = 0;
    virtual void formCMMCPHitsCrate(const DataVector<CMMCPHits>* cmmHitsMod,
                               DataVector<CMMCPHits>* cmmHitsCrate) const = 0;
    virtual void formCMMCPHitsSystem(const DataVector<CMMCPHits>* cmmHitsCrate,
                               DataVector<CMMCPHits>* cmmHitsSys) const = 0;
      
  };

  inline const InterfaceID& LVL1::IL1CPHitsTools::interfaceID()
    { 
      return IID_IL1CPHitsTools;
    }

} // end of namespace

#endif 
