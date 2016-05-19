/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/************************************************************************
                   CaloParticleIDTool.cxx - Copyright ATLAS Collaboration

author: Doug Schouten <dschoute at sfu dot ca>
author: Gennady Pospelov <pospelov at mppmu dot mpg dot de>

**************************************************************************/

///////////////////////////////////////////////////////////////////////////////
//
// Class to access calibration hit information, including 
// particle provenance, for (composite) calorimeter objects.
//
///////////////////////////////////////////////////////////////////////////////

#define CALOPARTICLEIDTOOL_CXX

#include "CaloParticleID/CaloParticleIDTool.h"

#include "Navigation/Navigable.h"

#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "CaloDetDescr/CaloDetDescrManager.h"

#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenInteracting.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <iterator>
#include <cmath>
#include <exception>
#include <limits>

using std::string;
using std::vector;

namespace {
  
  template <typename TYPE>
  struct cmp_less{
    bool operator()( const TYPE* l, const TYPE* r ) {
      if( l->cellID() != r->cellID() )
	return l->cellID() < r->cellID(); 
      else
	return l->particleID() < r->particleID();       
    }
  };
  
  template <typename TYPE>
  void set_intersection( std::vector<const TYPE*>& l,
			 std::vector<const TYPE*>& r,
			 std::vector<const TYPE*>& d )
  {
    std::sort( l.begin(), l.end(), cmp_less<TYPE>() );
    std::sort( r.begin(), r.end(), cmp_less<TYPE>() );
    std::set_intersection( l.begin(), l.end(), 
			   r.begin(), r.end(), 
			   std::back_inserter( d ) );
  }
  
  typedef std::vector<const CaloCalibrationHit*> hit_vector;
}

//
// Options:
//
// UseCellWeights: 
//   if the flag is set to false, this means that kinematic 
//   sharing of cells in composite calorimeter objects will not 
//   be properly handled in calculations. However, this may be 
//   approximately correct (or at least, the only option)
//   in the case where hadronic weights have been applied.
//
// MaskRegions :
//   a list of calorimeter samplings in which calibration
//   hits will not be loaded, i.e., these regions will be ignored in
//   all calculations
//

/** ============================================================================ */
CaloParticleIDTool::CaloParticleIDTool(const std::string& t,
				       const std::string& n,
				       const IInterface* i)
  : AthAlgTool(t, n, i),
    m_isConfigValid(false),
    m_caloDetDescrMgr(nullptr),
    m_detectorIDHelper(nullptr),
    m_truthParticles(nullptr)
{
  declareInterface<ICaloParticleIDTool>( this );
  
  declareProperty( "CalorimeterHits"    , m_calibrationCaloHitContainerNames, "" );
  
  declareProperty( "DeadMaterialHits"   , m_calibrationDMHitContainerNames  , "" );
  
  declareProperty( "TruthParticles"     , m_truthParticleCollectionName     , "" );
  
  declareProperty( "UseCellWeights"     , m_useCellWeights = true           , "" );
  
  declareProperty( "MaskRegions"        , m_maskRegions                     , "" );
}

/** ============================================================================ */
StatusCode CaloParticleIDTool::initialize( ) 
{
  ATH_CHECK( detStore()->retrieve( m_detectorIDHelper ) );
  m_caloDetDescrMgr = CaloDetDescrManager::instance(); 
  
  // register this tool with the IncidentSvc for setupEvent()
  ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
  ATH_CHECK( incidentSvc.retrieve() );
  incidentSvc->addListener( this, IncidentType::BeginEvent );    
  
  m_isConfigValid = false;
  
  return StatusCode::SUCCESS;
}

/** ============================================================================ */
StatusCode CaloParticleIDTool::finalize( ) 
{
  return StatusCode::SUCCESS;
}

/** ============================================================================ */
StatusCode CaloParticleIDTool::setupEvent( )
{
  ATH_CHECK(  evtStore()->retrieve( m_truthParticles, m_truthParticleCollectionName ) );
  
  //
  // load information for particles that have hits in the calorimeter,
  // and for cells which contain calibration hits
  //
  
  m_calorimeterCellInfo.clear();
  m_calorimeterParticleInfo.clear();
  m_deadMaterialParticleInfo.clear();
  
  m_allHits.clear();
  
  const DataHandle<CaloCalibrationHitContainer> handle;
  
  std::vector<const CaloCalibrationHitContainer*> hitContainers;
  
  std::vector<std::string>::iterator names_itr = m_calibrationCaloHitContainerNames.begin();
  std::vector<std::string>::iterator names_itr_end = m_calibrationCaloHitContainerNames.end();
  for ( ; names_itr != names_itr_end; ++names_itr )
  {
    ATH_CHECK( evtStore()->retrieve( handle, (*names_itr) ) );
    hitContainers.push_back( handle );
  }
  
  std::vector<const CaloCalibrationHitContainer*>::const_iterator hitcoll_itr = hitContainers.begin();
  std::vector<const CaloCalibrationHitContainer*>::const_iterator hitcoll_itr_end = hitContainers.end();
  
  CaloCalibrationHitContainer::const_iterator hit_itr;
  CaloCalibrationHitContainer::const_iterator hit_itr_end;
  
  for( ; hitcoll_itr != hitcoll_itr_end; ++hitcoll_itr ) // begin loop over hit containers
  {
    hit_itr     = (*hitcoll_itr)->begin();
    hit_itr_end = (*hitcoll_itr)->end();
    for( ; hit_itr != hit_itr_end; ++hit_itr ) // begin loop over calibration hits
    { 
      if( (*hit_itr)->particleID() == 0 )
      {
	ATH_MSG_WARNING( "Calibration hit has no particle ID!" );
	continue;
      }
      const CaloDetDescrElement* dde = m_caloDetDescrMgr->get_element( (*hit_itr)->cellID() );
      if( std::find( m_maskRegions.begin(), m_maskRegions.end(), (unsigned int)dde->getSampling() ) != m_maskRegions.end() )
      {
	ATH_MSG_DEBUG( "Skipping hit because it is in masked region: " 
		       << dde->getSampling() );
	continue;
      }
      
      CaloCalibrationHit* hit = getCopy( (*hit_itr) );
      m_allHits.push_back( hit );
      
      addHit( m_calorimeterCellInfo, hit );    
      addHit( m_calorimeterParticleInfo, hit );      
    } // end loop over calibration hits
    
    ATH_MSG_DEBUG( "Done loading hits in [" << (*hitcoll_itr)->Name() << "]"  );
    ATH_MSG_DEBUG( "Loaded information for [" << m_calorimeterParticleInfo.size() 
		   << "] particles and [" << m_calorimeterCellInfo.size() << "] cells" );
  } // end loop over calibration hit collections
  
  //
  // load calibration hit information for particles that deposit energy in 
  // dead material regions
  //
  
  hitContainers.clear();
  
  names_itr = m_calibrationDMHitContainerNames.begin();
  names_itr_end = m_calibrationDMHitContainerNames.end();
  for( ; names_itr != names_itr_end; ++names_itr )
  {
    ATH_CHECK( evtStore()->retrieve( handle, (*names_itr) ) );
    hitContainers.push_back( handle );
  }
  hitcoll_itr = hitContainers.begin();
  hitcoll_itr_end = hitContainers.end();
  for( ; hitcoll_itr != hitcoll_itr_end; ++hitcoll_itr ) 
  {
    hit_itr     = (*hitcoll_itr)->begin();
    hit_itr_end = (*hitcoll_itr)->end();
    for( ; hit_itr != hit_itr_end; ++hit_itr ) 
    { 
      if( !(m_detectorIDHelper->is_lar_dm((*hit_itr)->cellID()) || 
	    m_detectorIDHelper->is_tile_dm((*hit_itr)->cellID())) ) 
	continue;
      
      CaloCalibrationHit* hit = getCopy( (*hit_itr) );
      m_allHits.push_back( hit );
      
      addHit( m_deadMaterialParticleInfo, hit );      
    } // end loop over calibration hits
    
    ATH_MSG_DEBUG( "Done loading hits in [" << (*hitcoll_itr)->Name() << "]" );
    ATH_MSG_DEBUG( "Loaded information for [" << m_deadMaterialParticleInfo.size() 
		   << "] particles with hits in dead material." );
    
  } // end loop over dead material hit collections
  return StatusCode::SUCCESS;
}

/** ============================================================================ */
void CaloParticleIDTool::handle( const Incident& inc )
{
  if ( inc.type() == IncidentType::BeginEvent ) 
  {
    m_isConfigValid = false;
    ATH_MSG_DEBUG ( "[" << IncidentType::BeginEvent << "] event registered, "
		    << "reloading hit information" );
    if( setupEvent().isSuccess() ) 
      m_isConfigValid = true;
  }
  if( !m_isConfigValid )
    ATH_MSG_WARNING( "Could not update calibration hit information" );
}

/** ============================================================================ */
double CaloParticleIDTool::energyFromParticle( const INavigable* calObject, 
					       const HepMC::GenParticle* particle, 
					       EnergyType t /* = VISIBLE */ ) const 
{ 
  //
  // return the sum of the energy of all hits left by a particle inside a composite
  // calorimeter object
  //
  // if a calorimeter cell is shared (constituent weight < 1) then the sum is a
  // weighted sum over all cells, i.e., if h = sum of hits in a cell with weight w,
  // the total sum contains terms like (w * h)
  //
  // note that if the constituent weights are altered by a calibration, this
  // approach breaks down, so a warning is issued
  //
  
  const CaloCell* cell = dynamic_cast<const CaloCell*>( calObject );
  bool isCompositeObject = (cell == 0x0);
  
  double e( 0. );
  
  if( !m_isConfigValid )
  {
    ATH_MSG_WARNING( "in energyFromParticle(), not configured yet");
    return 0.;
  }
  
  if( !particle || !calObject )
  {
    // @fixme throwing an exception seems a bit heavy-handed here ... or is it?
    
    ATH_MSG_WARNING( "in energyFromParticle(), received 0x0");
    return 0.;
  }
  
  // @todo consider muons as interacting?
  TruthHelper::IsGenInteracting isInt;
  if( !isInt( particle ) )
    return 0.; // don't waste time on non-interacting particles
  
  if( isCompositeObject )
  {
    doSignalStateCheck( calObject );
  }
  
  // 
  // retrieve (i) all hits left by particle in the calorimeter
  //
  
  hitcollection_t part_hits;
  particleinfolist_t::const_iterator part_bookmark = m_calorimeterParticleInfo.find( particle->barcode() );
  if( part_bookmark == m_calorimeterParticleInfo.end() )
    return 0.;
  else
    std::copy( part_bookmark->hits().begin(), 
	       part_bookmark->hits().end(), 
	       std::back_inserter( part_hits ) );
  
  //
  // retrieve (ii) hits in constituent cells of calorimeter object 
  //
  
  hitcollection_t object_hits;
  
  NavigationToken<CaloCell,double> cellToken;
  calObject->fillToken(cellToken,double(1.)); 
  if( !isCompositeObject )
  {
    if( !cellToken.trySetObject( cell, 1.0 ) )
    {
      ATH_MSG_WARNING( "couldn't make token from cell" );
    }
  }
  if( cellToken.size() == 0 )
  {
    ATH_MSG_WARNING( "calorimeter object has 0 cells in token" );
  }
  std::map<Identifier,double> cellWeights;
  
  NavigationToken<CaloCell,double>::const_iterator cell_token_itr = cellToken.begin();
  NavigationToken<CaloCell,double>::const_iterator cell_token_itr_end = cellToken.end();
  for(; cell_token_itr != cell_token_itr_end; cell_token_itr++ ) 
  {
    cellinfolist_t::const_iterator cell_bookmark = m_calorimeterCellInfo.find( (*cell_token_itr)->ID() );
    if( cell_bookmark != m_calorimeterCellInfo.end() )
    {
      std::copy( cell_bookmark->hits().begin(), 
		 cell_bookmark->hits().end(), 
		 std::back_inserter( object_hits ) );
      if( m_useCellWeights )
	cellWeights.insert( std::pair<Identifier,double>( (*cell_token_itr)->ID(), 
							  cellToken.getParameter( cell_token_itr ) ) );
    }
  }
  
  if( object_hits.size() == 0 )
    return 0.;
  
  //
  // the result is sum( w * E ) over set_intersection( (i), (ii) )
  //
  
  hitcollection_t common_hits;
  
  set_intersection<calibhit_t>( object_hits, part_hits, common_hits );
  
  hitcollection_t::const_iterator hit_itr = common_hits.begin();
  hitcollection_t::const_iterator hit_itr_end = common_hits.end();
  for( ; hit_itr != hit_itr_end; ++hit_itr )
  {
    if( m_useCellWeights && cellWeights.find( (*hit_itr)->cellID() ) == cellWeights.end() )
    {
      ATH_MSG_ERROR( "Constituent weight for cell [" << (*hit_itr)->cellID() << "] not found" );
      throw std::exception();
    }
    
    double w = m_useCellWeights ? cellWeights[ (*hit_itr)->cellID() ] : 1.0;
    
    switch( t ) {
      case VISIBLE:
	e += w * ( (*hit_itr)->energyEM() + (*hit_itr)->energyNonEM() );
	break;
	
      case VISIBLE_EM:
	e += w * ( (*hit_itr)->energyEM() );
	break;
	
      case VISIBLE_HAD:
	e += w * ( (*hit_itr)->energyNonEM() );
	break;
	
      case INVISIBLE:
	e += w * ( (*hit_itr)->energyInvisible() + (*hit_itr)->energyEscaped() );
	break;
	
      case TOTAL:
	e += w * ( (*hit_itr)->energyTotal() );
	break;
    }
  }
  
  return e;
}

/** ============================================================================ */
double CaloParticleIDTool::energyInCalorimeter( const HepMC::GenParticle* particle,
						EnergyType t, /* = VISIBLE */ 
						void* caller,
						bool (*accept)( void*, const Identifier& ) ) const
{
  return energyFiltered( m_calorimeterParticleInfo, particle, t, caller, accept ); 
}

/** ============================================================================ */
double CaloParticleIDTool::etaInCalorimeter( const HepMC::GenParticle* particle,
					     EnergyType t, /* = VISIBLE */ 
					     void* caller,
					     bool (*accept)( void*, const Identifier& ) ) const
{
  if( !m_isConfigValid )
    return std::numeric_limits<double>::max();
  
  if( particle == NULL )
  {
    ATH_MSG_WARNING( "in retrieveHits(), received 0x0");
    return std::numeric_limits<double>::max();
  }
  
  particleinfolist_t::const_iterator part_bookmark = m_calorimeterParticleInfo.find( particle->barcode() );
  if( part_bookmark != m_calorimeterParticleInfo.end() )
  {
    hitcollection_t::const_iterator hit_itr = part_bookmark->hits().begin();
    hitcollection_t::const_iterator hit_itr_end = part_bookmark->hits().end();
    
    double etatot( 0 );
    double etot( 0 );
    for(; hit_itr != hit_itr_end; ++hit_itr)
    {
      Identifier id = (*hit_itr)->cellID();
      const CaloDetDescrElement* dde = m_caloDetDescrMgr->get_element( id );
      double eta = dde->eta();
      double edep( 0 );
      
      if(caller) {
        if(! (*accept)( caller, id ) ) {
	  continue;
	}
      }
      
      switch( t ) {
	case VISIBLE:
	  edep = (*hit_itr)->energyEM() + (*hit_itr)->energyNonEM();
	  break;
	  
	case VISIBLE_EM:
	  edep = (*hit_itr)->energyEM();
	  break;
	  
	case VISIBLE_HAD:
	  edep = (*hit_itr)->energyNonEM();
	  break;
	  
	case INVISIBLE:
	  edep = (*hit_itr)->energyInvisible() + (*hit_itr)->energyEscaped();
	  break;
	  
	case TOTAL:
	  edep = (*hit_itr)->energyTotal();
	  break;
      }
      
      etatot += edep * eta;
      etot   += edep;
    }
    if( etot > 0. )
    {
      return etatot / etot;
    }  
  }
  return std::numeric_limits<double>::max();
}

/** ============================================================================ */
double CaloParticleIDTool::energyInDeadMaterial( const HepMC::GenParticle* particle, 
						 EnergyType t, /* = TOTAL */
						 void* caller,
						 bool (*accept)( void*, const Identifier& ) ) const 
{
  return energyFiltered( m_deadMaterialParticleInfo, particle, t, caller, accept );
}

/** ============================================================================ */
double CaloParticleIDTool::energyFiltered( const particleinfolist_t& info,
					   const HepMC::GenParticle* particle,
					   EnergyType t, 
					   void* caller,
					   bool (*accept)( void*, const Identifier& ) ) const
{
  if( !m_isConfigValid )
  {
    ATH_MSG_WARNING( "in energyFiltered(), not configured yet");
    return 0.;
  }
  
  if( particle == NULL )
  {
    ATH_MSG_WARNING( "in energyFiltered(), received 0x0");
    return 0.;
  }
  
  particleinfolist_t::const_iterator part_bookmark = 
    info.find( particle->barcode() );
  
  if( part_bookmark == info.end() )
  {
    ATH_MSG_DEBUG( "particle with barcode [" << particle->barcode() << "] not in hit map" );
    return 0.;
  }
  
  if( caller )
  {
    particleinfo_t buffer( part_bookmark->barcode() );
    hitcollection_t::const_iterator hit_itr     = part_bookmark->hits().begin();
    hitcollection_t::const_iterator hit_itr_end = part_bookmark->hits().end();
    for( ; hit_itr != hit_itr_end; ++hit_itr )
    {
      Identifier id = (*hit_itr)->cellID();
      if( caller )
      {
	if( !((*accept)( caller, id )) )
	{
	  continue;
	}
      }
      buffer.add( (*hit_itr) );
    }
    return getEnergy( buffer, t );
  }
  else
  {
    return getEnergy( (*part_bookmark), t );
  }
}

/** ============================================================================ */
void CaloParticleIDTool::loadParticleMap( const INavigable* calObject, 
					  std::map<const HepMC::GenParticle*,double>& pmap, 
					  EnergyType t /* = VISIBLE */,
					  void* caller /* = 0x0 */, 
				          bool (*accept)(void*, const Identifier&) /* = &(ICaloParticleIDTool::idAccept) */  ) const
{
  if( !m_isConfigValid )
  {
    ATH_MSG_WARNING( "in loadParticleMap(), not configured yet");
    return;
  }
  
  if( !calObject )
  {
    // @fixme throwing an exception seems a bit heavy-handed here ... or is it?
    
    ATH_MSG_WARNING( "in loadParticleMap(), received 0x0");
    return;
  }
  
  const CaloCell* cell = dynamic_cast<const CaloCell*>( calObject );
  bool isCompositeObject = (cell == 0x0);
  
  if( isCompositeObject )
  {
    doSignalStateCheck( calObject );
  }
  
  //
  // retrieve hits in constituent cells of calorimeter object 
  //
  
  std::map<Identifier,double> cellWeights;
  std::map<int, double> buffer;
  
  NavigationToken<CaloCell,double> cellToken;
  calObject->fillToken(cellToken,double(1.));  
  
  if( !isCompositeObject )
  {
    if( !cellToken.trySetObject( cell, 1.0 ) )
    {
      ATH_MSG_WARNING( "couldn't make token from cell" );
    }
  }
  
  if( cellToken.size() == 0 )
  {
    ATH_MSG_WARNING( "calorimeter object has 0 cells in token" );
  }
  
  NavigationToken<CaloCell,double>::const_iterator cell_token_itr     = cellToken.begin();
  NavigationToken<CaloCell,double>::const_iterator cell_token_itr_end = cellToken.end();
  for(; cell_token_itr != cell_token_itr_end; cell_token_itr++ ) 
  {
    if(! (*accept)( caller, (*cell_token_itr)->ID() ) ) continue;
    
    double weight = m_useCellWeights ? cellToken.getParameter( cell_token_itr ) : 1.0;
    
    cellinfolist_t::const_iterator cell_bookmark =
      m_calorimeterCellInfo.find( (*cell_token_itr)->ID() );
    if( cell_bookmark == m_calorimeterCellInfo.end() )
      continue;
    
    hitcollection_t::const_iterator hit_itr     = cell_bookmark->hits().begin();
    hitcollection_t::const_iterator hit_itr_end = cell_bookmark->hits().end();
    for( ; hit_itr != hit_itr_end; ++hit_itr )
    {    
      double e( 0. );
      switch( t ) {
	case VISIBLE:
	  e = (*hit_itr)->energyEM() + (*hit_itr)->energyNonEM();
	  break;
	case VISIBLE_EM:
	  e = (*hit_itr)->energyEM();
	  break;
	case VISIBLE_HAD:
	  e = (*hit_itr)->energyNonEM();
	  break;
	case INVISIBLE:
	  e = (*hit_itr)->energyInvisible() + (*hit_itr)->energyEscaped();
	  break;
	case TOTAL:
	  e = (*hit_itr)->energyTotal();
	  break;
      }
      std::map<int,double>::iterator bookmark = buffer.find( (*hit_itr)->particleID() );
      if( bookmark == buffer.end() )
	buffer.insert( std::map<int,double>::value_type( (*hit_itr)->particleID(), weight * e ) );
      else
	bookmark->second += weight * e;
    }
  }
  
  std::map<int,double>::const_iterator buff_itr = buffer.begin();
  std::map<int,double>::const_iterator buff_itr_end = buffer.end();
  for( ; buff_itr != buff_itr_end; ++buff_itr )
  {
    const HepMC::GenParticle* part = getParticle( buff_itr->first );
    if( pmap.find( part ) == pmap.end() )
    {
      pmap.insert( std::pair<const HepMC::GenParticle*,double>( part, buff_itr->second ) );
    }
    else
    {
      pmap[ part ] += buff_itr->second;
    }
  }
}

/** ============================================================================ */
double CaloParticleIDTool::energyInObject( const INavigable* calObject, 
					   EnergyType t /* = VISIBLE */ ) const
{
  //
  // return the sum of the energy of all hits left inside a composite calorimeter object
  //
  // if a calorimeter cell is shared (constituent weight < 1) then the sum is a
  // weighted sum over all cells, i.e., if h = sum of hits in a cell with weight w,
  // the total sum contains a term (w * h)
  //
  // note that if the constituent weights are altered by a calibration, this
  // approach breaks down, so a warning is issued
  //
  
  if( !m_isConfigValid )
  {
    ATH_MSG_WARNING( "in energyInObject(), not configured yet");
    return 0;
  }
  
  double e( 0. );
  
  if( !calObject )
  {
    // @fixme throwing an exception seems a bit heavy-handed here ... or is it?
    
    ATH_MSG_WARNING( "CaloParticleIDTool::energyInObject() received 0x0");
    return 0.;
  }
  
  const CaloCell* cell = dynamic_cast<const CaloCell*>( calObject );
  bool isCompositeObject = (cell == 0x0);
  
  if( isCompositeObject )
  {
    doSignalStateCheck( calObject );
  }
  
  NavigationToken<CaloCell,double> cellToken;
  calObject->fillToken(cellToken,(double)1.);
  if( !isCompositeObject )
  {
    if( !cellToken.trySetObject( cell, 1.0 ) )
    {
      ATH_MSG_WARNING( "couldn't make token from cell" );
    }
  }
  
  if( cellToken.size() == 0 )
  {
    ATH_MSG_WARNING( "calorimeter object has 0 cells in token" );
  }
  NavigationToken<CaloCell,double>::const_iterator cell_token_itr = cellToken.begin();
  NavigationToken<CaloCell,double>::const_iterator cell_token_itr_end = cellToken.end();
  for(; cell_token_itr != cell_token_itr_end; ++cell_token_itr ) 
  {
    cellinfolist_t::const_iterator cell_bookmark = m_calorimeterCellInfo.find( (*cell_token_itr)->ID() );
    if( cell_bookmark != m_calorimeterCellInfo.end() )
    {
      double w = m_useCellWeights ? cellToken.getParameter( cell_token_itr ) : 1.0;
      e += w * getEnergy( (*cell_bookmark), t );
    }
  }
  
  return e;  
}

/** ============================================================================ */
void CaloParticleIDTool::retrieveHits( const HepMC::GenParticle* particle, 
				       CaloCalibrationHitContainer* hits ) const 
{
  if( !m_isConfigValid )
  {
    ATH_MSG_WARNING( "in retrieveHits(), not configured yet");
    return;
  }
  
  if( particle == NULL )
  {
    ATH_MSG_WARNING( "in retrieveHits(), received 0x0");
    return;
  }
  
  if( hits == 0 )
    hits = new CaloCalibrationHitContainer( );
  
  particleinfolist_t::const_iterator part_bookmark = 
    m_calorimeterParticleInfo.find( particle->barcode() );
  if( part_bookmark != m_calorimeterParticleInfo.end() )
  {
    hitcollection_t::const_iterator hit_itr = part_bookmark->hits().begin();
    hitcollection_t::const_iterator hit_itr_end = part_bookmark->hits().end();
    for(; hit_itr != hit_itr_end; ++hit_itr)
    {
      CaloCalibrationHit* tmp_hit = new CaloCalibrationHit( (*hit_itr)->cellID(),
							    (*hit_itr)->energyEM(),
							    (*hit_itr)->energyNonEM(),
							    (*hit_itr)->energyInvisible(),
							    (*hit_itr)->energyEscaped(),
							    (*hit_itr)->particleID() );
      hits->push_back( tmp_hit );
    }
  }
  
}

/** ============================================================================ */
void CaloParticleIDTool::retrieveHits( const INavigable* calObject, 
				       CaloCalibrationHitContainer* hits ) const 
{
  //
  // retrieve all the hits left in a composite calorimeter object,
  // expecting post-processing to handle kinematic weights 
  //
  
  if( !m_isConfigValid )
  {
    ATH_MSG_WARNING( "in retrieveHits(), not configured yet");
    return;
  }
  
  if( calObject == NULL )
  {
    ATH_MSG_WARNING( "in retrieveHits(), received 0x0");
    return;
  }
  
  const CaloCell* cell = dynamic_cast<const CaloCell*>( calObject );
  bool isCompositeObject = (cell == 0x0);
  
  if( hits == 0 )
    hits = new CaloCalibrationHitContainer( );
  
  if( isCompositeObject )
  {
    doSignalStateCheck( calObject );
  }  
  
  NavigationToken<CaloCell,double> cellToken;
  calObject->fillToken(cellToken,double(1.));
  if( !cellToken.trySetObject( cell, 1.0 ) )
  {
    ATH_MSG_WARNING( "couldn't make token from cell" );
  }
  if( cellToken.size() == 0 )
  {
    ATH_MSG_WARNING( "calorimeter object has 0 cells in token" );
  }
  NavigationToken<CaloCell,double>::const_iterator cell_token_itr = cellToken.begin();
  NavigationToken<CaloCell,double>::const_iterator cell_token_itr_end = cellToken.end();
  for(; cell_token_itr != cell_token_itr_end; cell_token_itr++ ) 
  {    
    cellinfolist_t::const_iterator cell_bookmark = 
      m_calorimeterCellInfo.find( (*cell_token_itr)->ID() );
    if( cell_bookmark != m_calorimeterCellInfo.end() )
    {
      hitcollection_t::const_iterator hit_itr = cell_bookmark->hits().begin();
      hitcollection_t::const_iterator hit_itr_end = cell_bookmark->hits().end();
      for(; hit_itr != hit_itr_end; ++hit_itr)
      {
	CaloCalibrationHit* tmp_hit = new CaloCalibrationHit( (*hit_itr)->cellID(),
							      (*hit_itr)->energyEM(),
							      (*hit_itr)->energyNonEM(),
							      (*hit_itr)->energyInvisible(),
							      (*hit_itr)->energyEscaped(),
							      (*hit_itr)->particleID() );
	hits->push_back( tmp_hit );
      }
    }
    
  }
}

/** ============================================================================ */
void CaloParticleIDTool::retrieveHits( const Identifier ID, 
				       const std::vector<const CaloCalibrationHit*>*& hits) const 
{
  //
  // retrieve all the hits in cell ID, expecting post-processing to handle kinematic weights 
  //
  
  cellinfolist_t::const_iterator cell_bookmark = m_calorimeterCellInfo.find( ID );
  if( cell_bookmark != m_calorimeterCellInfo.end() )
  {
    hits = &(cell_bookmark->hits());
  }
  
}

/** ============================================================================ */
double CaloParticleIDTool::getMeasurableEnergy( unsigned int barcode ) const
{ 
  const double electronMass = 0.510998901;
  const double protonMass   =   938.27203;
  const double neutronMass  =   939.56536;

  if( !m_isConfigValid )
  {
    ATH_MSG_WARNING( "in getMeasurableEnergy(), not configured yet");
    return -1;
  }
  
  const TruthParticle* particle( 0 );
  particle = m_truthParticles->truthParticle( barcode );
  
  if( 0 == particle )
    return -1.;
  
  double totalEnergy   = particle->e();
  double kineticEnergy = particle->e();
  long PDGEncoding     = particle->pdgId();
  
  double correctionForMass = 0.;
  if ( PDGEncoding == 11 )
  { // particleName == "e-"
    correctionForMass = - electronMass;
  }
  else if ( PDGEncoding == -11 )
  { // particleName == "e+"
    correctionForMass = + electronMass;
  }
  else if ( PDGEncoding == 2212 )
  { // particleName == "proton"
    correctionForMass = - protonMass;
  }
  else if ( PDGEncoding == 2112 )
  { // particleName == "neutron"
    correctionForMass = - neutronMass;
  }
  else if ( PDGEncoding > 1000010000 )
  { //for nuclei
    return kineticEnergy;
  }
  //   else if (particleName == "lambda" || particleName == "sigma+" ||
  //            particleName == "sigma0" || particleName == "xi-" ||
  //            particleName == "xi0" || particleName == "omega-"){
  else if (PDGEncoding == 3122 || PDGEncoding == 3222 ||
	   PDGEncoding == 3212 || PDGEncoding == 3312 ||
	   PDGEncoding == 3322 || PDGEncoding == 3334)
  {
    correctionForMass = - protonMass;
  }
  else if ( PDGEncoding == 3112 )
  {  // particleName == "sigma-"
    correctionForMass = - neutronMass;
  }
  else if ( PDGEncoding == -2112 )
  { //particleName == "anti_neutron"
    correctionForMass = + neutronMass;
  }
  //            else if (particleName == "anti_proton" || particleName == "anti_lambda" ||
  //                     particleName == "anti_sigma0" || particleName == "anti_sigma+" ||
  //                     particleName == "anti_xi0" || particleName == "anti_xi-" ||
  //                     particleName == "anti_omega-"){
  else if ( PDGEncoding == -2212 || PDGEncoding == -3122 ||
	    PDGEncoding == -3212 || PDGEncoding == -3222 ||
	    PDGEncoding == -3322 || PDGEncoding == -3312 ||
	    PDGEncoding == -3334 )
  {
    correctionForMass = + protonMass;
  }
  else if ( PDGEncoding == -3112 )
  { // particleName == "anti_sigma-"
    correctionForMass = + neutronMass;
  }
  
  double measurableEnergy = totalEnergy + correctionForMass;
  
  if( measurableEnergy < -0.1e-6 )
  {
    ATH_MSG_WARNING( "Calibration Hit: NEGATIVE measurableEnergy="
		     << measurableEnergy << " < -0.1e-6 MeV for " << PDGEncoding );
    measurableEnergy = 0.;
  }
  return measurableEnergy;
}

/** ============================================================================ */
void CaloParticleIDTool::doSignalStateCheck( const INavigable* calObject ) const
{
  if( !m_useCellWeights ) return;
  
  const CaloCluster* dummy_ptr = dynamic_cast<const CaloCluster*>( calObject );
  if( dummy_ptr != NULL )
  {
    if( ( dummy_ptr->signalState() != P4SignalState::UNCALIBRATED &&
	  dummy_ptr->signalState() != P4SignalState::UNKNOWN ) && s_numWarningsIssued < 10 )
    {
      ATH_MSG_WARNING( "Particle ID information for clusters with signal state '" 
		       << dummy_ptr->signalState() << "' is not well defined" );
      ++s_numWarningsIssued;
    }
  }
}

/** ============================================================================ */
void CaloParticleIDTool::addHit( particleinfolist_t& infolist, const calibhit_t* hit )
{
  particleinfolist_t::const_iterator particleinfo_bookmark =
    infolist.find( particleinfo_t( hit->particleID() ) );
  if( particleinfo_bookmark == infolist.end() )
  {
    particleinfo_t buffer( hit, hit->particleID() );
    buffer.energy_measurable = 0.; // this->getMeasurableEnergy( hit->particleID() );
    infolist.insert( buffer );
  }
  else 
  {
    particleinfo_bookmark->add( hit );
  }
}

/** ============================================================================ */
void CaloParticleIDTool::addHit( cellinfolist_t& infolist, const calibhit_t* hit )
{
  cellinfolist_t::const_iterator cellinfo_bookmark = 
    infolist.find( cellinfo_t( hit->cellID() ) );
  if( cellinfo_bookmark == infolist.end() ) 
  {
    const CaloDetDescrElement* dde = m_caloDetDescrMgr->get_element( hit->cellID() );
    double eta = dde->eta();
    double phi = dde->phi();
    infolist.insert( cellinfo_t( hit, eta, phi ) );
  }
  else
  {
    cellinfo_bookmark->add( hit );	
  }   
}

/** ============================================================================ */
void CaloParticleIDTool::registerHitTransformation( void* caller, transform callback ) 
{
  typedef CaloCalibrationHit* (*transform)( void*, const CaloCalibrationHit* );
  m_hitCallbacks.push_back( std::pair<void*,transform>( caller, callback ) );
}

/** ============================================================================ */
CaloCalibrationHit* CaloParticleIDTool::getCopy( const CaloCalibrationHit* hit ) 
{
  CaloCalibrationHit* buffer = new CaloCalibrationHit( hit->cellID(),
						       hit->energyEM(),
						       hit->energyNonEM(),
						       hit->energyInvisible(),
						       hit->energyEscaped(), 
						       hit->particleID() );
  unsigned int numTransforms = m_hitCallbacks.size();
  for( unsigned int ic = 0; ic < numTransforms; ++ic )
  {
    CaloCalibrationHit* orig = buffer;
    buffer = m_hitCallbacks[ic].second( m_hitCallbacks[ic].first, orig );
    delete orig;
  }
  return buffer;
}
