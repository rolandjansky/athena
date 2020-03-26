/*
  General-purpose view creation algorithm <bwynne@cern.ch>

  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_EventViewCreatorAlgorithm_h
#define ViewAlgs_EventViewCreatorAlgorithm_h

#include <vector>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthContainers/ConstDataVector.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/InputMakerBase.h"

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "GaudiKernel/IAlgResourcePool.h"
#include "GaudiKernel/IScheduler.h"
#include "AthViews/View.h"

#include "ViewAlgs/IViewCreatorROITool.h"

/**
  * @class EventViewCreatorAlgorithm
  * @brief Used at the start of a sequence to create the EventViews: retrieves filtered collections via menu decision from previous 
  * step, merges them and writes out a merged collection to be consumed by HypoAlg(s).
  * Spawns EventViews for each unique ROI associated to each Decision object in the merged output.
  * Links the EventView and the ROI to each Decision object in the merged output.
  **/


class EventViewCreatorAlgorithm : public ::InputMakerBase
{
 public:
    EventViewCreatorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~EventViewCreatorAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext&) const override;
    virtual StatusCode finalize() override { return StatusCode::SUCCESS; }
 protected:

    EventViewCreatorAlgorithm();

    SG::WriteHandleKey< ViewContainer > m_viewsKey{ this, "Views", "Unspecified",
      "The key of views collection produced" };

    SG::WriteHandleKey< ConstDataVector<TrigRoiDescriptorCollection> > m_inViewRoIs{ this, "InViewRoIs", "Unspecified",
      "Name of the collection which should be inserted into the just-spawned views to seed them with their ROI" };

    // needs for views
    Gaudi::Property< std::string > m_schedulerName { this, "SchedulerName", "AvalancheSchedulerSvc", "Name of the scheduler" };
    Gaudi::Property< std::string > m_viewNodeName{ this, "ViewNodeName", "", "Name of the ControlFlow node to attach new views to" };

    Gaudi::Property< bool > m_viewFallThrough { this, "ViewFallThrough", false,
      "Set whether views may access StoreGate directly to retrieve data" };
    Gaudi::Property< std::vector< std::string > > m_viewFallFilter { this, "FallThroughFilter", {}, 
      "An optional list of SG keys (or parts of them) that can come from StoreGate via FallThrough" };

    Gaudi::Property< bool > m_requireParentView { this, "RequireParentView", false,
      "For each new view, locate and link a parent view (previous step). Fail if the parent view can not be found." };

    Gaudi::Property< bool > m_reverseViews { this, "ReverseViewsDebug", false, 
      "Reverse order of views, as a debugging option" };

    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_cachedViewsKey { this, "InputCachedViews", "",
      "Optional ReadHandle on the output (InputMakerOutputDecisions) of an EVCA in a previous Step, whose Views can be re-used" };

    ToolHandle<IViewCreatorROITool> m_roiTool{this, "RoITool", "",
      "Tool used to supply per-Decision Object the RoI on which the Decision Object's view is to be spawned"};

    /**
     * @brief Makes sure the views are linked, if configuration requireParentView is set. Failure to set the parent is an error
     **/
    StatusCode linkViewToParent( const TrigCompositeUtils::Decision* outputDecision, SG::View* newView ) const;

    /**
     * @brief Seeds a newly created view with an ROI collection containing the single seeding ROI
     **/
    StatusCode placeRoIInView( const ElementLink<TrigRoiDescriptorCollection>& roi, SG::View* view, const EventContext& context ) const;

    /**
     * @brief Obtain smart pointer to scheduler in order to schedule newly spawned views
     **/
    SmartIF<IScheduler> getScheduler() const;

    /**
     * @brief Allow for the re-use of EventViews run in a previous Step in another EVCA instance configured
     * to spawn EventViews using the same reconstruction sequence (i.e. both algs should share a common ViewNodeName).
     **/
    bool checkCache(const TrigCompositeUtils::DecisionContainer* cachedViews, 
      const TrigCompositeUtils::Decision* outputDecision, 
      size_t& cachedIndex) const;

};

#include "EventViewCreatorAlgorithm.icc"

#endif

