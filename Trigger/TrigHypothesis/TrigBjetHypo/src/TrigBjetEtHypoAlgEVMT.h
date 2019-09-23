/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGBJETHYPO_TRIGBJETETHYPOALG_EV_MT_H
#define TRIGBJETHYPO_TRIGBJETETHYPOALG_EV_MT_H 1

#include "TrigBjetEtHypoAlgMT.h"
#include "AthViews/ViewHelper.h"

class TrigBjetEtHypoAlgEVMT : public TrigBjetEtHypoAlgMT {
 public: 
  TrigBjetEtHypoAlgEVMT( const std::string& name, ISvcLocator* pSvLocator );
  virtual ~TrigBjetEtHypoAlgEVMT();

  virtual StatusCode initialize();

 protected:
  TrigBjetEtHypoAlgEVMT();

  virtual StatusCode retrieveJets( const EventContext&,
				   ElementLinkVector< xAOD::JetContainer >&,
                                   const SG::ReadHandleKey< xAOD::JetContainer >&,
                                   const TrigCompositeUtils::DecisionContainer* ) const;

  virtual StatusCode retrieveRoIs( const EventContext&,
                                   const TrigRoiDescriptorCollection*&,
                                   const SG::ReadHandleKey< TrigRoiDescriptorCollection >& ) const;

  virtual StatusCode setJetLink( const EventContext&,
                                 const SG::ReadHandleKey< xAOD::JetContainer >&,
                                 const unsigned int,
                                 const TrigCompositeUtils::DecisionContainer*&,
                                 std::vector< TrigCompositeUtils::Decision* >& ) const;

  virtual StatusCode setTracksLink( const EventContext&,
				    const SG::ReadHandleKey< xAOD::TrackParticleContainer >&,
				    const unsigned int,
				    const TrigCompositeUtils::DecisionContainer*&,
				    std::vector< TrigCompositeUtils::Decision* >& ) const;

  virtual const TrigCompositeUtils::Decision* getPreviousDecision( const TrigCompositeUtils::DecisionContainer*,unsigned int ) const;

  virtual StatusCode attachLinkToDecisions( const EventContext&,
                                            const TrigCompositeUtils::DecisionContainer*,
                                            std::vector< TrigCompositeUtils::Decision* >& ) const;

 private:
  Gaudi::Property< std::string > m_trackLink {this,"TracksLink","Undefined","Link for track particle container in the output decision"};
  SG::ReadHandleKey< xAOD::TrackParticleContainer > m_inputTracksKey {this,"Tracks","Undefined","Track particle key to be linked to the output decision"};
}; 

#endif //> !TRIGBJETHYPO_TRIGBJETETHYPOALG_EV__H
