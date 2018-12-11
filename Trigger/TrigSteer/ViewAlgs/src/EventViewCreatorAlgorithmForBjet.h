/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_EventViewCreatorAlgorithmForBjet_h
#define ViewAlgs_EventViewCreatorAlgorithmForBjet_h

#include "EventViewCreatorAlgorithm.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

 /**
   * @class EventViewCreatorAlgorithmForBjet
   * @brief Used at the start of a sequence to create the EventViews: retrieves filtered collection via menu decision from previous step and writes it out directly so it can be used as input by the reco alg that follows in sequence.
   **/

class EventViewCreatorAlgorithmForBjet : public EventViewCreatorAlgorithm {
 public:
    EventViewCreatorAlgorithmForBjet( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~EventViewCreatorAlgorithmForBjet();

    virtual StatusCode initialize() override;
    virtual StatusCode execute_r(const EventContext&) const override;

 private:

    StatusCode placeJetInView( const xAOD::Jet* theObject,
			       SG::View* view,
			       const EventContext& context ) const;

    EventViewCreatorAlgorithmForBjet();

    SG::WriteHandleKey< ConstDataVector<xAOD::JetContainer> > m_inViewJets {this,"InViewJets","Unspecified","Name with which the Jets should be inserted into the views"};

    Gaudi::Property< std::string > m_jetsLink {this,"JetsLink","Unspecified","Name of EL to Jet object linked to the decision"};
};

#endif

