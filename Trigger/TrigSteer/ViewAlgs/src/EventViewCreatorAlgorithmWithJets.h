/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_EventViewCreatorAlgorithmWithJets_h
#define ViewAlgs_EventViewCreatorAlgorithmWithJets_h

#include "EventViewCreatorAlgorithm.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

 /**
   * @class EventViewCreatorAlgorithmWithJets
   * @brief Used at the start of a sequence to create the EventViews: retrieves filtered collection via menu decision from previous step and writes it out directly so it can be used as input by the reco alg that follows in sequence.
   **/

class EventViewCreatorAlgorithmWithJets : public EventViewCreatorAlgorithm {
 public:
    EventViewCreatorAlgorithmWithJets( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~EventViewCreatorAlgorithmWithJets();

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext&) const override;

 private:

    StatusCode placeJetInView( const xAOD::Jet* theObject,
			       SG::View* view,
			       const EventContext& context ) const;

    EventViewCreatorAlgorithmWithJets();

    SG::WriteHandleKey< ConstDataVector<xAOD::JetContainer> > m_inViewJets {this,"InViewJets","Unspecified","Name with which the Jets should be inserted into the views"};
};

#endif

