/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauTrackSlimmer.cxx
// package:     Reconstruction/tauRec
// authors:     Anna Kaczmarska, Lukasz Janyst
// 
// This class builds Slim Track objects for taus in the AOD 
//             
// date:        2008-01-17
// 15/04/2008 - (AK) fixing compilation warning bug #35463 
// 25/02/2009 - (AK) adding  declareProperty thinSvc
// 22/01/2010 - (AK) adding protection against size 0 track collection 
//-----------------------------------------------------------------------------

#include "tauRec/TauTrackSlimmer.h"

#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauCommonDetails.h"

#include "GaudiKernel/ListItem.h"
#include <cmath>

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TauTrackSlimmer::TauTrackSlimmer( const std::string &name,
        ISvcLocator * pSvcLocator ) :
    AthAlgorithm( name, pSvcLocator ),
    m_thinningSvc( "ThinningSvc",  name ),
    m_filterTaus(false),
    m_maxNTrack(4),
    m_maxCharge(2),
    m_maxEmRadius(0.2),
    m_maxIsoFrac(0.5)
{
    // Name of the thinningSvc
    declareProperty( "thinSvc", m_thinningSvc, "Name of the thinningSvc" ); 
    declareProperty( "TauContainer", m_tauContainerName );
    declareProperty( "FilterTaus", m_filterTaus );
    declareProperty( "maxNTrack", m_maxNTrack );
    declareProperty( "maxCharge", m_maxCharge );
    declareProperty( "maxEmRadius", m_maxEmRadius );
    declareProperty( "maxIsoFrac", m_maxIsoFrac );
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TauTrackSlimmer::~TauTrackSlimmer()
{
}

//-----------------------------------------------------------------------------
// Initialiezer
//-----------------------------------------------------------------------------
StatusCode TauTrackSlimmer::initialize()
{

    ATH_MSG_VERBOSE( "TauTrackSlimmer :: initialize()" );

    /*
       sc = service( "StoreGateSvc", evtStore() );
       if( sc.isFailure() )
       {
       log << MSG :: ERROR;
       log << "Unable to retrieve pointer to StoreGateSvc";
       log );
       return StatusCode :: FAILURE;
       }
     */
    if ( m_thinningSvc.retrieve().isFailure() )
    {
        ATH_MSG_ERROR( "Unable to retrieve pointer to IThinningSvc" );
        return StatusCode :: FAILURE;
    }
    return StatusCode :: SUCCESS;
}

//-----------------------------------------------------------------------------
// Finalizer
//-----------------------------------------------------------------------------
StatusCode TauTrackSlimmer :: finalize()
{
    return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
// Execution
//-----------------------------------------------------------------------------
StatusCode TauTrackSlimmer::execute()
{
    using namespace Analysis;

    StatusCode sc;

    //---------------------------------------------------------------------------
    // Retrieve tau jet container
    //---------------------------------------------------------------------------
    const Analysis::TauJetContainer  *tauContainer = nullptr;
    sc = evtStore()->retrieve( tauContainer, m_tauContainerName );

    if( sc.isFailure() || ! tauContainer )
    {
        ATH_MSG_DEBUG( "No tau container found!" );
        return StatusCode :: SUCCESS;
    }

    //---------------------------------------------------------------------------
    // Retrieve track container
    //---------------------------------------------------------------------------
    const TrackCollection* tracks = 0;
    sc = evtStore()->retrieve( tracks, "Tracks" );

    if( sc.isFailure() || !tracks ){
        ATH_MSG_WARNING( "No track container found in TDS" );
        return StatusCode :: SUCCESS;
    }

    if(tracks->size() == 0){
        ATH_MSG_DEBUG( "empty track container found in TDS" );
        return StatusCode :: SUCCESS;
    }

    //---------------------------------------------------------------------------
    // Initialize vector of bools
    //---------------------------------------------------------------------------
    std::vector<bool> selected;
    selected.resize( tracks->size(), false );

    //---------------------------------------------------------------------------
    // Initialize iterators
    //---------------------------------------------------------------------------
    Analysis::TauJetContainer :: const_iterator ftau = tauContainer->begin();
    Analysis::TauJetContainer :: const_iterator etau = tauContainer->end();

    //---------------------------------------------------------------------------
    // Loop over taus
    //---------------------------------------------------------------------------

    for(; ftau != etau; ++ftau )
    {

        const Analysis::TauCommonDetails*  p_taudetails = (*ftau)->details<const Analysis::TauCommonDetails>();

        if (m_filterTaus) {

            if ((*ftau)->numTrack() > m_maxNTrack) continue;
            if (std::abs((*ftau)->charge()) > m_maxCharge) continue;
            if ( p_taudetails) {
                if (p_taudetails->seedCalo_EMRadius() > m_maxEmRadius) continue;
                if (p_taudetails->seedCalo_isolFrac() > m_maxIsoFrac) continue;
            }

        }

        for( unsigned itr = 0; itr < (*ftau)->numTrack(); ++itr )
            selected[(*ftau)->track(itr)->trackElementLink()->index()] = true;
    }



    //---------------------------------------------------------------------------
    // Invoke the thinning service
    //---------------------------------------------------------------------------
    sc = m_thinningSvc->filter( *tracks, selected, IThinningSvc :: Operator :: Or );
    if( sc.isFailure() )
    {
        ATH_MSG_ERROR( "Failed to thin Tracks associated to taus" );
        return StatusCode :: SUCCESS;
    }

    return StatusCode :: SUCCESS;
}
