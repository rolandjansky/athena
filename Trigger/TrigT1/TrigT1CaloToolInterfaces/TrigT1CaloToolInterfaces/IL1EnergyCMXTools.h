/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1EnergyCMXTools.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1ENERGYCMXTOOLS_H
#define ILVL1L1ENERGYCMXTOOLS_H

#include "GaudiKernel/IAlgTool.h"


namespace LVL1 
{

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
    virtual void formJEMEtSums(const DataVector<JetElement>*  jetElementVec,
                               DataVector<JEMEtSums>* jemEtSumsVec) const = 0;
    virtual void formCMXEtSums(const DataVector<JEMEtSums>* jemEtSumsVec,
                               DataVector<CMXEtSums>* cmxEtSumsVec) const = 0;
    virtual void formCMXEtSumsModule(const DataVector<JEMEtSums>* jemEtSumsVec,
                               DataVector<CMXEtSums>* cmxEtSumsMod) const = 0;
    virtual void formCMXEtSumsCrate(const DataVector<CMXEtSums>* cmxEtSumsMod,
                               DataVector<CMXEtSums>* cmxEtSumsCrate) const = 0;
    virtual void formCMXEtSumsSystem(
                               const DataVector<CMXEtSums>* cmxEtSumsCrate,
                               DataVector<CMXEtSums>* cmxEtSumsSys) const = 0;
    virtual void formCMXEtSumsEtMaps(const DataVector<CMXEtSums>* cmxEtSumsSys,
                               DataVector<CMXEtSums>* cmxEtSumsMaps) const = 0;
      
  };

  inline const InterfaceID& LVL1::IL1EnergyCMXTools::interfaceID()
    { 
      return IID_IL1EnergyCMXTools;
    }

} // end of namespace

#endif 
