/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1JEMJetTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1JEMJETTOOLS_H
#define ILVL1L1JEMJETTOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "AthContainers/DataVector.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/JEMTobRoIContainer.h"


namespace LVL1 
{
class JEMJetAlgorithm;
class JetInput;

// amazurov: for deprecated method
class JEMTobRoI;

/**
Interface definition for L1JEMJetTools
*/

  static const InterfaceID IID_IL1JEMJetTools("LVL1::IL1JEMJetTools", 1, 0);

  class IL1JEMJetTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual void findRoIs(const std::map<int, JetInput*>* elements, xAOD::JEMTobRoIContainer* rois) const = 0;
    virtual void findRoIs(const xAOD::JetElementContainer* jes, xAOD::JEMTobRoIContainer* rois, int slice = -1) const = 0;
    virtual void findRoIs(const std::map<int, JetInput*>* elements, DataVector<JEMJetAlgorithm>* rois) const = 0;
    
    virtual void findRoIs(const xAOD::JetElementContainer* jes, DataVector<JEMJetAlgorithm>* rois, int slice = -1) const = 0;
    
    virtual void findJEMResults(const std::map<int, JetInput*>* inputs, int crate, int module,
                                xAOD::JEMTobRoIContainer* rois, std::vector<unsigned int>& jetCMXData) const = 0;

    // DEPRECATED(amazurov): use findJEMResults with AOD::JEMTobRoIContainer
    virtual void findJEMResults(const std::map<int, JetInput*>* inputs, int crate, int module,
                                DataVector<JEMTobRoI>* rois, std::vector<unsigned int>& jetCMXData) const = 0;

    
    virtual void mapJetInputs(const xAOD::JetElementContainer* jes, std::map<int, JetInput*>* elements, int slice = -1) const = 0;
    virtual JEMJetAlgorithm findRoI(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements) const = 0;
    virtual JEMJetAlgorithm formSums(double RoIeta, double RoIphi, const std::map<int, JetInput*>* elements) const = 0;
    virtual JEMJetAlgorithm formSums(uint32_t roiWord, const std::map<int, JetInput*>* elements) const = 0;
    
  };

  inline const InterfaceID& LVL1::IL1JEMJetTools::interfaceID()
    { 
      return IID_IL1JEMJetTools;
    }

} // end of namespace

#endif 
