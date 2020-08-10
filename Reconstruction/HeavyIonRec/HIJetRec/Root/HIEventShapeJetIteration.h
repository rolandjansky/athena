/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIJETREC_HIEVENTSHAPEJETITERATION_H
#define HIJETREC_HIEVENTSHAPEJETITERATION_H

#include "AsgTools/AsgTool.h"
#include <string>
#include "JetInterface/IJetExecuteTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEvent/HIEventShape.h"
#include "xAODJet/JetContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "HIJetRec/IHISubtractorTool.h"
#include "HIJetRec/IHIUEModulatorTool.h"
#include "HIEventUtils/HIEventShapeMapTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class HIEventShapeJetIteration : virtual public IJetExecuteTool,
  virtual public asg::AsgTool
{
  ASG_TOOL_CLASS(HIEventShapeJetIteration, IJetExecuteTool);

public:

  HIEventShapeJetIteration(std::string name);

  virtual StatusCode initialize() override;

  virtual int execute() const override;

  StatusCode makeClusterList(std::vector<const xAOD::CaloCluster*>& particleList, const xAOD::JetContainer* theJets,
			     std::set<unsigned int>& used_indices, std::set<unsigned int>& used_eta_bins) const;

  //refactored for use in xAOD analysis.
  //functionality decoupled from store access
  StatusCode makeClusterList(std::vector<const xAOD::CaloCluster*>& particleList, const xAOD::JetContainer* theJets) const;
  StatusCode makeClusterList(std::vector<const xAOD::CaloCluster*>& particleList, const std::vector<const xAOD::JetContainer*>& theJets_vector) const;
  void updateShape(xAOD::HIEventShapeContainer* output_shape, const std::vector<const xAOD::CaloCluster*>& assoc_clusters, const HIEventShapeIndex* es_index=nullptr ) const;
  StatusCode fillModulatorShape(xAOD::HIEventShape* ms, const xAOD::HIEventShapeContainer* output_shape, const std::set<unsigned int>& used_indices, unsigned int scheme) const;
  StatusCode remodulate(xAOD::HIEventShapeContainer* output_shape, const xAOD::HIEventShape* ms, const std::set<unsigned int>& used_indices) const;
  StatusCode getShapes(const xAOD::HIEventShapeContainer*& input_shape, xAOD::HIEventShapeContainer*& output_shape, bool record_aux=false) const;

private:

  ToolHandle<IHISubtractorTool> m_subtractorTool { this, "Subtractor", "HIJetClusterSubtractorTool", "Handle to HIJetClusterSubtractorTool" };
  ToolHandle<IHIUEModulatorTool> m_modulatorTool { this, "Modulator" , "HIUEModulatorTool", "Handle to HIUEModulatorTool" };
  ToolHandle<IHIEventShapeMapTool> m_eventShapeMapTool { this, "EventShapeMapTool", "HIEventShapeMapTool", "Handle to Event Shape Map Tool"};

  /// \brief Name of input HIEventShapeContainer
  SG::ReadHandleKey<xAOD::HIEventShapeContainer> m_inputEventShapeKey { this, "InputEventShapeKey", "HIEventShape", "HIEventShape"};
  /// \brief Name of output HIEventShapeContainer
  SG::WriteHandleKey<xAOD::HIEventShapeContainer> m_outputEventShapeKey { this, "OutputEventShapeKey", "HIEventShape_iter", "HIEventShape_iter"};

  /// \brief List of names of JetCollections, all jets in these collections are seeds
  SG::ReadHandleKey<xAOD::JetContainer> m_caloJetSeedKey { this, "CaloJetSeedContainerKey", "", "Names of seed collections"};
  SG::ReadHandleKey<xAOD::JetContainer> m_trackJetSeedKey { this, "TrackJetSeedContainerKey", "", "Names of seed collections"};

  /// \brief Name of jet attribute providing link between jets and clusters
  Gaudi::Property< std::string > m_associationKey { this, "AssociationKey", "", "Name of jet attribute providing link between jets and clusters" };
  /// \brief All clusters w/in this DR of jet are excluded from shape calc.
  Gaudi::Property< float > m_excludeDR { this, "ExclusionRadius", 0.4, "Exclude all calo regions w/in this DR to jet" };
  /// \brief If selected, the jet constituents define the associated clusters
  Gaudi::Property< bool > m_excludeConstituents { this, "ExcludeConstituents", false, "Only exclude constituents of jets" };

  Gaudi::Property< bool > m_doRemodulation { this, "RemodulateUE", false, "Correct UE for incomplete cancellation of flow harmonics when iterating" };

  Gaudi::Property< unsigned int > m_modulationScheme { this, "ModulationScheme", 0, "Scheme to build separate ES object for flow modulation" };

  SG::WriteHandleKey<xAOD::HIEventShapeContainer> m_modulationKey { this, "ModulationEventShapeKey", "HIEventShape_itr_mod", "Modulation Event Shape Key"};

  Gaudi::Property< bool > m_shallowCopy { this, "ShallowCopy", true, "Use shallow copy for iterated event shape" };

};

#endif
