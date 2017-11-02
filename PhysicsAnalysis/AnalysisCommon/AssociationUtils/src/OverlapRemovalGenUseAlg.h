/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_OVERLAPREMOVALGenUseALG_H
#define ASSOCIATIONUTILS_OVERLAPREMOVALGenUseALG_H

// Framework includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// EDM includes
#include "xAODBase/IParticleContainer.h"

// Local includes
#include "AssociationUtils/IOverlapRemovalTool.h"

/// Algorithm to implement basic OR procedure as a prerequisite for event cleaning
///
/// @author Julia Gonski <j.gonski@cern.ch>
///
class OverlapRemovalGenUseAlg : public AthAlgorithm
{

  public:

    /// Standard algorithm constructor
    OverlapRemovalGenUseAlg(const std::string& name, ISvcLocator* svcLoc);

    /// Initialize the algorithm
    virtual StatusCode initialize();

    /// Execute the algorithm
    virtual StatusCode execute();

  private:

    /// Simple object selection
    template<class ContainerType>
    void applySelection(const ContainerType& container);

    // Reset decorations to failing
    template<class ContainerType>
    void setDefaultDecorations(const ContainerType& container);
    
    /// Simple object selection
    template<class ObjType>
    bool selectObject(const ObjType& obj);

    /// Print object info
    void printObjects(const xAOD::IParticleContainer& container,
                      const std::string& type);
    
    /// Handle to the tool
    ToolHandle<ORUtils::IOverlapRemovalTool> m_orTool;

    /// Configuration
    std::string m_selectionLabel;
    std::string m_overlapLabel;
    std::string m_bJetLabel;
    std::string m_electronLabel;
    std::string m_photonLabel;
    std::string m_muonLabel;
    std::string m_tauLabel;
    double m_ptCut;
    double m_etaCut;

};

#endif
