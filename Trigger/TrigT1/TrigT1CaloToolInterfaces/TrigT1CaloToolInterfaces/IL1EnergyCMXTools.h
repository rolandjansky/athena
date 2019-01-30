/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1EnergyCMXTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1ENERGYCMXTOOLS_H
#define ILVL1L1ENERGYCMXTOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/CMXEtSumsContainer.h"


namespace LVL1
{
  //using namespace xAOD;

class CMXEtSums;
class JEMEtSums;
class JetElement;

/**
Interface definition for L1EnergyCMXTools
*/
  static const InterfaceID IID_IL1EnergyCMXTools("LVL1::IL1EnergyCMXTools", 1, 0);

  class IL1EnergyCMXTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void formJEMEtSums(const xAOD::JetElementContainer*  jetElementVec,
                               xAOD::JEMEtSumsContainer* jemEtSumsVec) const = 0;
    virtual void formCMXEtSums(const xAOD::JEMEtSumsContainer* jemEtSumsVec,
                               xAOD::CMXEtSumsContainer* cmxEtSumsVec) const = 0;
    virtual void formCMXEtSumsModule(const xAOD::JEMEtSumsContainer* jemEtSumsVec,
                               xAOD::CMXEtSumsContainer* cmxEtSumsMod) const = 0;
    virtual void formCMXEtSumsCrate(const xAOD::CMXEtSumsContainer* cmxEtSumsMod,
                               xAOD::CMXEtSumsContainer* cmxEtSumsCrate) const = 0;
    virtual void formCMXEtSumsSystem(
                               const xAOD::CMXEtSumsContainer* cmxEtSumsCrate,
                               xAOD::CMXEtSumsContainer* cmxEtSumsSys) const = 0;
    virtual void formCMXEtSumsEtMaps(const xAOD::CMXEtSumsContainer* cmxEtSumsSys,
                               xAOD::CMXEtSumsContainer* cmxEtSumsMaps) const = 0;
      
  };

  inline const InterfaceID& LVL1::IL1EnergyCMXTools::interfaceID()
    { 
      return IID_IL1EnergyCMXTools;
    }

} // end of namespace

#endif 
