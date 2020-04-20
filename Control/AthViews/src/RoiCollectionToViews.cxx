/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RoiCollectionToViews.h"
#include "AthViews/ViewHelper.h"
#include "AthContainers/ConstDataVector.h"

namespace AthViews {

RoiCollectionToViews::RoiCollectionToViews( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm( name, pSvcLocator )
{
}

RoiCollectionToViews::~RoiCollectionToViews()
{
}

StatusCode RoiCollectionToViews::initialize()
{
  ATH_MSG_DEBUG ("Initializing " << name() << "...");

  CHECK( m_trigRoIs.initialize() );
  CHECK( m_viewRoIs.initialize() );
  CHECK( m_w_views.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode RoiCollectionToViews::execute()
{
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const EventContext& ctx = getContext();

  //Load the collection of RoI descriptors
  SG::ReadHandle< TrigRoiDescriptorCollection > inputRoIs( m_trigRoIs, ctx );
  ATH_CHECK( inputRoIs.isValid() );

  std::vector< ConstDataVector< TrigRoiDescriptorCollection> > outputRoICollectionVector;
  for ( auto roi: *inputRoIs )
  {
    ATH_MSG_DEBUG( "RoI Eta: " << roi->eta() << " Phi: " << roi->phi() << " RoIWord: " << roi->roiWord() );

    ConstDataVector<TrigRoiDescriptorCollection> oneRoIColl (SG::VIEW_ELEMENTS);
    oneRoIColl.push_back( roi );
    outputRoICollectionVector.push_back( std::move(oneRoIColl) );
  }

  //Create the views and populate them
  auto viewVector =  std::make_unique<ViewContainer>();
  CHECK( ViewHelper::makeAndPopulate( m_viewBaseName,            //Base name for all views to use
                                      viewVector.get(),          //Vector to store views
                                      m_viewRoIs,                //A writehandlekey to use to access the views
                                      ctx,                       //The context of this algorithm
                                      outputRoICollectionVector, //Data to initialise each view - one view will be made per entry
                                      m_viewFallThrough ) );     //Allow fall through from view to storegate

  //Run the algorithms in views
  CHECK( ViewHelper::scheduleViews( viewVector.get(),            //View vector
                                    m_viewNodeName,              //CF node to attach views to
                                    ctx,                         //Context to attach the views to
                                    svcLoc()->service<IScheduler>(m_schedulerName,false) ) ); //Scheduler

  //Store the collection of views
  SG::WriteHandle< ViewContainer > outputViewHandle( m_w_views, ctx );
  CHECK( outputViewHandle.record( std::move( viewVector ) ) );

  return StatusCode::SUCCESS;
}

} //> end namespace AthViews
