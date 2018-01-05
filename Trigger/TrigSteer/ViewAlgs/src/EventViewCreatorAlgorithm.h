/*
  General-purpose view creation algorithm <bwynne@cern.ch>

  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_EventViewCreatorAlgorithm_h
#define ViewAlgs_EventViewCreatorAlgorithm_h

#include <vector>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthContainers/ConstDataVector.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "GaudiKernel/IAlgResourcePool.h"
#include "GaudiKernel/IScheduler.h"
#include "AthViews/View.h"

class EventViewCreatorAlgorithm : public AthAlgorithm
{
  public:
    EventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    StatusCode initialize();
    StatusCode execute();

  private:
    ServiceHandle< IScheduler > m_scheduler{ this, "Scheduler", "AvalancheSchedulerSvc", "The Athena scheduler" };

    //Input trig composite collection to split into views
    SG::ReadHandleKey< TrigCompositeUtils::DecisionContainer > m_inputDecisionsKey{ this, "Decisions", "Unspecified", "The name of decision container to use in making views" };

    //Output views for merging
    SG::WriteHandleKey< std::vector< SG::View* > > m_viewsKey{ this, "Views", "Unspecified", "The key of views collection produced" };

    // auxiliary handles
    SG::WriteHandleKey< ConstDataVector<TrigRoiDescriptorCollection> > m_inViewRoIs{ this, "InViewRoIs", "Unspecified", "Name with which the RoIs shoudl be inserted into the views" };
    SG::WriteHandleKey< ConstDataVector<TrigCompositeUtils::DecisionContainer> > m_inViewDecisions{ this, "InViewDecisions", "Unspecified", "The name of decision container placed in the view" };

    Gaudi::Property<bool> m_viewPerRoI{ this, "ViewPerRoI", false, "Create one View per RoI as opposed to one View per Decision object, needs to be true when multiple decisions per RoI exists" };
    Gaudi::Property< std::string > m_viewNodeName{ this, "ViewNodeName", "", "Name of the CF node to attach a view to" };
    Gaudi::Property< std::string > m_roisLink{ this, "RoIsLink", "Unspecified", "Name of EL to RoI object lined to the decision" };
    Gaudi::Property< bool > m_viewFallThrough { this, "ViewFallThrough", false, "Set whether views may accesas StoreGate directly to retrieve data" };
};

#endif

