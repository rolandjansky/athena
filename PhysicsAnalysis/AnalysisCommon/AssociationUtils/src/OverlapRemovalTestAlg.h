/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_OVERLAPREMOVALTESTALG_H
#define ASSOCIATIONUTILS_OVERLAPREMOVALTESTALG_H

// Framework includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// EDM includes
#include "xAODBase/IParticleContainer.h"

// Local includes
#include "AssociationUtils/IOverlapRemovalTool.h"

/// A testing algorithm for the dual-use overlap removal tool in Athena
///
/// @author Steve Farrell <Steven.Farrell@cern.ch>
///
class OverlapRemovalTestAlg : public AthAlgorithm
{

  public:

    /// Standard algorithm constructor
    OverlapRemovalTestAlg(const std::string& name, ISvcLocator* svcLoc);

    /// Initialize the algorithm
    virtual StatusCode initialize();

    /// Execute the algorithm
    virtual StatusCode execute();

  private:

    /// Simple object selection
    template<class ContainerType>
    void applySelection(const ContainerType& container);

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
