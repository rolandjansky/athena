/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1JetCMXTools.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1JETCMXTOOLS_H
#define ILVL1L1JETCMXTOOLS_H

#include "GaudiKernel/IAlgTool.h"


namespace LVL1 
{

class CMXJetTob;
class CMXJetHits;
class JEMTobRoI;
class JetAlgorithm;
class JetROI;

/**
Interface definition for L1JetCMXTools
*/
  static const InterfaceID IID_IL1JetCMXTools("LVL1::IL1JetCMXTools", 1, 0);

  class IL1JetCMXTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void formJEMTobRoI(const DataVector<JetAlgorithm>* jetAlgorithmVec,
                                     DataVector<JEMTobRoI>*    jemRoiVec) const = 0;
    virtual void formJEMTobRoI(const DataVector<JetROI>*    jetRoiVec,
                                     DataVector<JEMTobRoI>* jemRoiVec) const = 0;
    virtual void formCMXJetTob(const DataVector<JEMTobRoI>* jemRoiVec,
                                     DataVector<CMXJetTob>* cmxTobVec) const = 0;
    virtual void formCMXJetTob(
                 const std::vector<const DataVector<JEMTobRoI>*>& jemRoiColls,
                 DataVector<CMXJetTob>* cmxTobVec, int peak) const = 0;
    virtual void formCMXJetHits(const DataVector<CMXJetTob>* cmxTobVec,
                                DataVector<CMXJetHits>* cmxHitsVec) const = 0;
    virtual void formCMXJetHitsCrate(const DataVector<CMXJetTob>* cmxTobVec,
                                DataVector<CMXJetHits>* cmxHitsCrate) const = 0;
    virtual void formCMXJetHitsSystem(
                                const DataVector<CMXJetHits>* cmxHitsCrate,
                                DataVector<CMXJetHits>* cmxHitsSys) const = 0;
    virtual void formCMXJetHitsTopo(const DataVector<CMXJetTob>* cmxTobVec,
                                DataVector<CMXJetHits>* cmxHitsTopo) const = 0;
      
  };

  inline const InterfaceID& LVL1::IL1JetCMXTools::interfaceID()
    { 
      return IID_IL1JetCMXTools;
    }

} // end of namespace

#endif 
