/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASSOCIATIONUTILS_ORTOOLBOXTESTALG_H
#define ASSOCIATIONUTILS_ORTOOLBOXTESTALG_H

// Framework includes
#include "AthenaBaseComps/AthAlgorithm.h"

// Local includes
#include "AssociationUtils/ToolBox.h"
#include "AssociationUtils/OverlapRemovalInit.h"


/// @class ORToolBoxTestAlg
/// @brief A testing algorithm for the dual-use overlap removal tool in Athena.
///
/// This algorithm actually tests the ToolBox-based C++ configuration mechanism
/// in Athena. For a more standard Athena example usage, please see the
/// ORToolBoxTestAlg instead.
///
/// @author Steve Farrell <Steven.Farrell@cern.ch>
///
class ORToolBoxTestAlg : public AthAlgorithm
{

  public:

    /// Standard algorithm constructor
    ORToolBoxTestAlg(const std::string& name, ISvcLocator* svcLoc);

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

    /// The overlap removal tools' toolbox
    ORUtils::ToolBox m_toolBox;

    /// The overlap removal tools' configuration flags
    ORUtils::ORFlags m_flags;

};

#endif
