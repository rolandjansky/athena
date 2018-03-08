/*
  General-purpose view creation algorithm <bwynne@cern.ch>

  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_TestEventViewCreatorAlgorithm_h
#define ViewAlgs_TestEventViewCreatorAlgorithm_h

#include <vector>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthContainers/ConstDataVector.h"
#include "DecisionHandling/TrigCompositeUtils.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "GaudiKernel/IAlgResourcePool.h"
#include "GaudiKernel/IScheduler.h"
#include "AthViews/View.h"

class TestEventViewCreatorAlgorithm : public AthAlgorithm
{
  public:
    TestEventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    StatusCode initialize();
    StatusCode execute();

  private:
    ServiceHandle< IScheduler > m_scheduler{ this, "Scheduler", "AvalancheSchedulerSvc", "The Athena scheduler" };

    //Input trig composite collection to split into views
    SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_inputDecisionsKey { this, "InputDecisions", {}, "The vector name of input decision container (implicit)" } ;

    // Ouput decisions
    SG::WriteHandleKey< TrigCompositeUtils::DecisionContainer > m_outputDecisionsKey{ this, "OutputDecisions", "Unspecified", "The name of output decision container" };

    //Output views for merging
    SG::WriteHandleKey< std::vector< SG::View* > > m_viewsKey{ this, "Views", "Unspecified", "The key of views collection produced" };

    // auxiliary handles
    SG::WriteHandleKey< ConstDataVector<TrigRoiDescriptorCollection> > m_inViewRoIs{ this, "InViewRoIs", "Unspecified", "Name with which the RoIs shoudl be inserted into the views" };

    Gaudi::Property<bool> m_viewPerRoI{ this, "ViewPerRoI", false, "Create one View per RoI as opposed to one View per Decision object, needs to be true when multiple decisions per RoI exists" };
    Gaudi::Property< std::string > m_viewNodeName{ this, "ViewNodeName", "", "Name of the CF node to attach a view to" };
    Gaudi::Property< std::string > m_roisLink{ this, "RoIsLink", "Unspecified", "Name of EL to RoI object lined to the decision" };
    Gaudi::Property< bool > m_viewFallThrough { this, "ViewFallThrough", false, "Set whether views may accesas StoreGate directly to retrieve data" };
};

#endif

