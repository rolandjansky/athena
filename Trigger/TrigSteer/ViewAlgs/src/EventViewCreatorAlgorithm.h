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

 /**
   * @class EventViewCreatorAlgorithm
   * @brief Used at the start of a sequence to create the EventViews: retrieves filtered collection via menu decision from previous step and writes it out directly so it can be used as input by the reco alg that follows in sequence.
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

    //Output views for merging
    SG::WriteHandleKey< ViewContainer > m_viewsKey{ this, "Views", "Unspecified", "The key of views collection produced" };

    // same handles as inputMakerForRoI
    SG::WriteHandleKey< ConstDataVector<TrigRoiDescriptorCollection> > m_inViewRoIs{ this, "InViewRoIs", "Unspecified", "Name with which the RoIs shoudl be inserted into the views" };

    // needs for views
    Gaudi::Property< std::string > m_schedulerName { this, "SchedulerName", "AvalancheSchedulerSvc", "Name of the scheduler" };
    Gaudi::Property< std::string > m_viewNodeName{ this, "ViewNodeName", "", "Name of the CF node to attach a view to" };

    Gaudi::Property< bool > m_viewFallThrough { this, "ViewFallThrough", false, "Set whether views may accesas StoreGate directly to retrieve data" };
    Gaudi::Property< bool > m_requireParentView { this, "RequireParentView", false, "Fail if the parent view can not be found" };



    // methods
  /**
   * @brief makes sure the views are linked, if configuration requireParentView is set, failure to set the parent is an error
   **/
    StatusCode linkViewToParent( const TrigCompositeUtils::Decision* inputDecsion, SG::View* newView ) const;
    StatusCode placeRoIInView( const TrigRoiDescriptor* roi, SG::View* view, const EventContext& context ) const;
    inline SmartIF<IScheduler> getScheduler() const {return svcLoc()->service<IScheduler>(m_schedulerName,false);}
};

#endif

