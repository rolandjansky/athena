/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1CPCMXTools.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1CPCMXTOOLS_H
#define ILVL1L1CPCMXTOOLS_H

#include "GaudiKernel/IAlgTool.h"
//#include "TrigT1CaloUtils/CPAlgorithm.h"

namespace LVL1 
{

class CMXCPHits;
class CPAlgorithm;
class CMXCPTob;
class CPMTobRoI;
class EmTauROI;

/**
Interface definition for L1CPCMXTools
*/
  static const InterfaceID IID_IL1CPCMXTools("LVL1::IL1CPCMXTools", 1, 0);

  class IL1CPCMXTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void formCPMTobRoI(const DataVector<CPAlgorithm>* cpAlgorithmVec,
                                  DataVector<CPMTobRoI>*      cpmRoiVec) const = 0;
    virtual void formCPMTobRoI(const DataVector<EmTauROI>* emTauRoiVec,
                                  DataVector<CPMTobRoI>*   cpmRoiVec) const = 0;
    virtual void formCMXCPTob(const DataVector<CPMTobRoI>*   cpmRoiVec,
                              DataVector<CMXCPTob>* cmxTobVec) const = 0;
    virtual void formCMXCPTob(
                 const std::vector<const DataVector<CPMTobRoI>*>& cpmRoiColls,
                 DataVector<CMXCPTob>* cmxTobVec, int peak) const = 0;
    virtual void formCMXCPHits(const DataVector<CMXCPTob>*  cmxTobVec,
                               DataVector<CMXCPHits>* cmxHitsVec) const = 0;
    virtual void formCMXCPHitsCrate(const DataVector<CMXCPTob>* cmxTobVec,
                               DataVector<CMXCPHits>* cmxHitsCrate) const = 0;
    virtual void formCMXCPHitsSystem(const DataVector<CMXCPHits>* cmxHitsCrate,
                               DataVector<CMXCPHits>* cmxHitsSys) const = 0;
    virtual void formCMXCPHitsTopo(const DataVector<CMXCPTob>* cmxTobVec,
                               DataVector<CMXCPHits>* cmxHitsTopo) const = 0;
      
  };

  inline const InterfaceID& LVL1::IL1CPCMXTools::interfaceID()
    { 
      return IID_IL1CPCMXTools;
    }

} // end of namespace

#endif 
