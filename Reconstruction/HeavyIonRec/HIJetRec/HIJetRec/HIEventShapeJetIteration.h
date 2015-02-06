/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIEventShapeJetIteration.h

#ifndef __HIJETREC_HIEVENTSHAPEJETITERATION_H__
#define __HIJETREC_HIEVENTSHAPEJETITERATON_H__

////////////////////////////////////////////////////////////////////////////////
///
/// \class HIEventShapeJetIteration
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \brief Algorithm that produces a new HIEventShapeContainer from an
/// input HIEventShapeContainer and a set of "seed" jets
/// New event shape has contribution from clusters near seed jets removed
///
////////////////////////////////////////////////////////////////////////////////


#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include <set>
#include <xAODHIEvent/HIEventShapeContainer.h>
#include <xAODCaloEvent/CaloCluster.h>


class HIEventShapeJetIteration : public AthAlgorithm
{
public:

  HIEventShapeJetIteration(const std::string& name, ISvcLocator* pSvcLocator);
  ~HIEventShapeJetIteration() {};


  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  /// \brief Name of input HIEventShapeContainer
  std::string m_input_event_shape_key;

  /// \brief Name of output HIEventShapeContainer
  std::string m_output_event_shape_key;

  /// \brief List of names of JetCollections, all jets in these collections are seeds
  std::vector<std::string> m_jet_coll_keys;

  /// \brief Name of jet attribute providing link between jets and clusters
  std::string m_association_key;

  /// \brief All clusters w/in this DR of jet are excluded from shape calc.
  float m_exclude_DR;

  /// \brief If selected, the jet constituents define the associated clusters
  bool m_exclude_constituents;

  /// \brief Method to update the shape based on a given cluster
  /// two sets of indices are passed by reference and updated by the method
  /// sets are queried to see if the cluster has already been used, e.g. by another jet seed
  /// checking/updating sets prevents double counting
  /// eta-bin set used to determine eta-averaged flow
  void UpdateUsingCluster(xAOD::HIEventShapeContainer* shape, const xAOD::CaloCluster* cl, 
			  std::set<unsigned int>& used_indices, std::set<unsigned int>& used_eta_bins) const;

};

#endif
