/*
  General-purpose view creation algorithm <bwynne@cern.ch>

  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_EventViewCreatorAlgorithmForBjet_h
#define ViewAlgs_EventViewCreatorAlgorithmForBjet_h

#include <vector>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthContainers/ConstDataVector.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/InputMakerBase.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "GaudiKernel/IAlgResourcePool.h"
#include "GaudiKernel/IScheduler.h"
#include "AthViews/View.h"

 /**
   * @class EventViewCreatorAlgorithmForBjet
   * @brief Used at the start of a sequence to create the EventViews: retrieves filtered collection via menu decision from previous step and writes it out directly so it can be used as input by the reco alg that follows in sequence.
   **/

class EventViewCreatorAlgorithmForBjet : public ::InputMakerBase {
 public:
    EventViewCreatorAlgorithmForBjet( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~EventViewCreatorAlgorithmForBjet();
    virtual StatusCode initialize() override;
    virtual StatusCode execute_r(const EventContext&) const override;
    virtual StatusCode finalize() override { return StatusCode::SUCCESS; }
  private:

    EventViewCreatorAlgorithmForBjet();
       
    //Output views for merging
    SG::WriteHandleKey< ViewContainer > m_viewsKey{ this, "Views", "Unspecified", "The key of views collection produced" };

    // auxiliary handles
    SG::WriteHandleKey< ConstDataVector<TrigRoiDescriptorCollection> > m_inViewRoIs{ this, "InViewRoIs", "Unspecified", "Name with which the RoIs shoudl be inserted into the views" };
    // **** b-jet Specific stuff
    SG::WriteHandleKey< ConstDataVector<xAOD::JetContainer> > m_inViewJets {this,"InViewJets","Unspecified","Name with which the Jets should be inserted into the views"};
    SG::WriteHandleKey< ConstDataVector<xAOD::VertexContainer> > m_inViewPrimaryVertex {this,"InViewPrimaryVertex","Unspecified","Name with which the Prmary Vertex should be inserted into the views"};

    Gaudi::Property< std::string > m_jetsLink {this,"JetsLink","Unspecified","Name of EL to Jet object linked to the decision"};
    Gaudi::Property< std::string > m_primaryVertexLink {this,"PrimaryVertexLink","Unspecified","Name of EL to primary vertex linked to the decision"};
    // ****

    ServiceHandle< IScheduler > m_scheduler{ this, "Scheduler", "AvalancheSchedulerSvc", "The Athena scheduler" };
    Gaudi::Property<bool> m_viewPerRoI{ this, "ViewPerRoI", false, "Create one View per RoI as opposed to one View per Decision object, needs to be true when multiple decisions per RoI exists" };
    Gaudi::Property< std::string > m_viewNodeName{ this, "ViewNodeName", "", "Name of the CF node to attach a view to" };
    Gaudi::Property< std::string > m_roisLink{ this, "RoIsLink", "Unspecified", "Name of EL to RoI object lined to the decision" };
    Gaudi::Property< bool > m_viewFallThrough { this, "ViewFallThrough", false, "Set whether views may accesas StoreGate directly to retrieve data" };
    Gaudi::Property< bool > m_requireParentView { this, "RequireParentView", false, "Fail if the parent view can not be found" };

    size_t countInputHandles( const EventContext& context ) const;
    void   printDecisions( const std::vector<SG::WriteHandle<TrigCompositeUtils::DecisionContainer>>& outputHandles ) const;
    void insertDecisions( const TrigCompositeUtils::Decision* src, TrigCompositeUtils::Decision* dest ) const;
  /**
   * @brief makes sure the views are linked, if configuration requireParentView is set, failure to set the parent is an error
   **/
    StatusCode linkViewToParent( const TrigCompositeUtils::Decision* inputDecsion, SG::View* newView ) const;
    StatusCode placeRoIInView( const TrigRoiDescriptor* roi, SG::View* view, const EventContext& context ) const;
    StatusCode placeJetInView( const xAOD::Jet* theObject,
			       SG::View* view,
			       const EventContext& context ) const;
    StatusCode placeVertexInView( const xAOD::Vertex* theObject,
				  SG::View* view,
				  const EventContext& context ) const;
};

#endif

