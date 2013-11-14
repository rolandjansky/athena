/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"

/////////////////////////////////////////////////////////
#include "MboyAthTool/MuonDynamicLayerCreator.h"


// constructor
Trk::MuonDynamicLayerCreator::MuonDynamicLayerCreator(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  p_INavigator("Trk::Navigator/AtlasNavigator")                   ,
  p_IPropagator("Trk::RungeKuttaPropagator/RungeKuttaPropagator") ,                                       
  p_MboyCollectMatterTool("MboyCollectMatterTool")                                              
{
  declareInterface<IDynamicLayerCreator>(this);

  declareProperty("NoReducs", m_noreducs=false);

  declareProperty("INavigator" , p_INavigator ) ;
  declareProperty("IPropagator" , p_IPropagator ) ;
  declareProperty("MboyCollectMatterTool" , p_MboyCollectMatterTool ) ;

}

// destructor
Trk::MuonDynamicLayerCreator::~MuonDynamicLayerCreator(){}

// Athena standard methods
// initialize
StatusCode Trk::MuonDynamicLayerCreator::initialize()
{

    ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION ) ;
 
    StatusCode sc = AlgTool::initialize();
    if ( sc.isFailure() ) {
      ATH_MSG_WARNING( " AlgTool::initialize() failed" ) ;
      return( StatusCode::FAILURE );
    }

    ATH_MSG_INFO( "================================" ) ;
    ATH_MSG_INFO( "=Proprieties are     " ) ;
    ATH_MSG_INFO( "= NoReducs  " << m_noreducs << " IS OBSOLETE " ) ;
    ATH_MSG_INFO( "================================" ) ;

//  Retrieve p_MboyCollectMatterTool
    if ( p_MboyCollectMatterTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL( "Failed to retrieve tool " << p_MboyCollectMatterTool ) ;
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << p_MboyCollectMatterTool ) ;

    ATH_MSG_DEBUG( name() <<" initialize() successful" ) ;

    return sc;
}

// finalize
StatusCode Trk::MuonDynamicLayerCreator::finalize(){return StatusCode::SUCCESS;}

const std::vector< std::pair<const Trk::TrackParameters*,const Trk::Layer* > >* 
Trk::MuonDynamicLayerCreator::materialLayers(const Trk::TrackParameters& parm,
									    const Trk::Surface& /*sf*/ ,
									    const Trk::IPropagator* )  const
{

  const Trk::TrackParameters* parm_on_entry = &parm;

  std::vector< std::pair<const Trk::TrackParameters*, const Trk::Layer* > >* matLayers =
       new std::vector< std::pair<const Trk::TrackParameters*,const Trk::Layer* > >;
  
  StatusCode sc =p_MboyCollectMatterTool->DoCollectMatter(parm_on_entry,matLayers);
  
  if (sc==StatusCode::FAILURE) ATH_MSG_FATAL( "Could not collect material!!" ) ;

  return matLayers;

}

