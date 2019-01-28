/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1JetCMXTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1JETCMXTOOLS_H
#define ILVL1L1JETCMXTOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTrigL1Calo/JEMTobRoIContainer.h"
#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"

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

    virtual void formCMXJetTob(const xAOD::JEMTobRoIContainer* jemRoiVec,
                                xAOD::CMXJetTobContainer* cmxTobVec) const = 0;

    virtual void formCMXJetTob(
                 const std::vector<const xAOD::JEMTobRoIContainer*>& jemRoiColls,
                 xAOD::CMXJetTobContainer* cmxTobVec, int peak) const = 0;

    virtual void formCMXJetHits(const xAOD::CMXJetTobContainer* cmxTobVec,
                                xAOD::CMXJetHitsContainer* cmxHitsVec) const = 0;

    virtual void formCMXJetHitsCrate(const xAOD::CMXJetTobContainer* cmxTobVec,
                                xAOD::CMXJetHitsContainer* cmxHitsCrate) const = 0;
    virtual void formCMXJetHitsSystem(
                                const xAOD::CMXJetHitsContainer* cmxHitsCrate,
                                xAOD::CMXJetHitsContainer* cmxHitsSys) const = 0;
    virtual void formCMXJetHitsTopo(const xAOD::CMXJetTobContainer* cmxTobVec,
                                xAOD::CMXJetHitsContainer* cmxHitsTopo) const = 0;


  };

  inline const InterfaceID& LVL1::IL1JetCMXTools::interfaceID()
    { 
      return IID_IL1JetCMXTools;
    }

} // end of namespace

#endif 
