/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_OVERLAPREMOVALECALG_H
#define ASSOCIATIONUTILS_OVERLAPREMOVALECALG_H

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
class OverlapRemovalECAlg : public AthAlgorithm
{

  public:

    /// Standard algorithm constructor
    OverlapRemovalECAlg(const std::string& name, ISvcLocator* svcLoc);

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
    void resetDecorations(const ContainerType& container);
    
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

};

#endif
