/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1EtTools.h
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1ETTOOLS_H
#define ILVL1L1ETTOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
//#include "TrigT1CaloUtils/ModuleEnergy.h"
//#include "TrigT1CaloUtils/CrateEnergy.h"
//#include "TrigT1CaloUtils/SystemEnergy.h"

namespace LVL1
{
class ModuleEnergy;
class CrateEnergy;
class SystemEnergy;
class EnergyCMXData;

/**
Interface definition for L1EtTools
*/

  static const InterfaceID IID_IL1EtTools("LVL1::IL1EtTools", 1, 0);

  class IL1EtTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void moduleSums(const xAOD::JetElementContainer* jetelements,
                            DataVector<ModuleEnergy>* modules, int slice = -1) const = 0;
    virtual void moduleSums(const xAOD::JetElementMap_t* jemap,
                            DataVector<ModuleEnergy>* modules, int slice = -1) const = 0;
    virtual void crateSums(const DataVector<ModuleEnergy>* modules,
                            DataVector<CrateEnergy>* crates, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) const = 0 ;
    virtual void crateSums(const DataVector<EnergyCMXData>* modules,
                            DataVector<CrateEnergy>* crates, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) const = 0 ;
    virtual void crateSums(const xAOD::JetElementContainer* jetelements,
                           DataVector<CrateEnergy>* crates, int slice = -1, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) const = 0;
    virtual void crateSums(const xAOD::JetElementMap_t* jemap,
                           DataVector<CrateEnergy>* crates, int slice = -1, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) const = 0;
    virtual SystemEnergy systemSums(const DataVector<CrateEnergy>* crates) const = 0;
    virtual SystemEnergy systemSums(const xAOD::JetElementContainer* jetelements, int slice = -1, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) const = 0;
    virtual SystemEnergy systemSums(const xAOD::JetElementMap_t* jemap, int slice = -1, uint32_t maskXE = 0xff, uint32_t maskTE = 0xff, bool restricted = false) const = 0;

  };

  inline const InterfaceID& LVL1::IL1EtTools::interfaceID()
    {
      return IID_IL1EtTools;
    }

} // end of namespace

#endif
