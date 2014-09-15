/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonAlignGenTools/MuonAlignHelperTool.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"

namespace Muon {
  
  //________________________________________________________________________
  MuonAlignHelperTool::MuonAlignHelperTool(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
    : AthAlgTool(type,name,parent)  
    , m_muonIdHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")    
  {

    declareInterface<MuonAlignHelperTool>(this);

    declareProperty("MuonIdHelperTool", m_muonIdHelper);
  }
  
  //________________________________________________________________________
  MuonAlignHelperTool::~MuonAlignHelperTool() 
  {
    
  }

  //________________________________________________________________________
  StatusCode MuonAlignHelperTool::initialize()
  {   
    // set up MuonIdHelperTool
    if (m_muonIdHelper.retrieve().isSuccess()) 
      ATH_MSG_INFO("Retrieved  "<<m_muonIdHelper);
    else {
      ATH_MSG_FATAL("Could not get MuonIdHelperTool!");
      return StatusCode::FAILURE;
    }   
    
    return StatusCode::SUCCESS;
  }
  
  //________________________________________________________________________
  StatusCode MuonAlignHelperTool::finalize()
  {
    return StatusCode::SUCCESS;
  }

  //________________________________________________________________________
  bool MuonAlignHelperTool::isEndcap(Identifier id) const
  {
    if (m_muonIdHelper->isEndcap(id)) return true;        
    if (m_muonIdHelper->mdtIdHelper().stationNameString(m_muonIdHelper->mdtIdHelper().stationName(id)).substr(0,3)=="BEE") return true;
    return false;
  }

  //________________________________________________________________________
  bool MuonAlignHelperTool::isBIS8(Identifier id) const
  {
    if (m_muonIdHelper->mdtIdHelper().stationNameString(m_muonIdHelper->mdtIdHelper().stationName(id)).substr(0,3)=="BIS" && std::abs(m_muonIdHelper->mdtIdHelper().stationEta(id))==8) return true;
    return false;
  }
 
  /* 
  //________________________________________________________________________
  Trk::Track* MuonAlignHelperTool::redoHitsOnTrack(const Trk::Track& track,
						   bool rescaleErrorsOnly) const {
    
    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_DEBUG("track without states, discarding track");
      return 0;
    }
    
    // vector to store states, the boolean indicated whether the state was create in this routine (true) or belongs to the track (false)
    // If any new state is created, all states will be cloned and a new track will beformed from them.
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > newStates;
    newStates.reserve(states->size()+5);
    
    Identifier currentMdtChId;
    
    // loop over TSOSs
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check
      
      // check whether state is a measurement
      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if (!pars) {
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }

      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if (!meas) {
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }

      Identifier id = m_helperTool->getIdentifier(*meas);
      
      // Not a ROT, else it would have had an identifier. Keep the TSOS.
      if (!id.is_valid() || !m_idHelperTool->isMuon(id)) {
	newStates.push_back( std::make_pair(false,*tsit) );	
	continue;
      }
      
      ATH_MSG_VERBOSE(" new measurement " << m_idHelperTool->toString(id));
      
      if( m_idHelperTool->isMdt(id) ) {
	
	const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
	if( !mdt ){
	  ATH_MSG_WARNING(" Measurement with MDT identifier that is not a MdtDriftCircleOnTrack ");
	  continue;
	}
	bool hasT0=mdt->timingMode()==3;
	const Trk::RIO_OnTrack* newMdt;
	if (rescaleErrorsOnly) 
	  newMdt=(hasT0)?
	    m_mdtRotCreatorAdjT0->updateError( *mdt, pars ):
	    m_mdtRotCreator     ->updateError( *mdt, pars );
	else
	  newMdt=(hasT0)?
	    m_mdtRotCreatorAdjT0->correct( *(mdt->prepRawData()), *pars ):
	    m_mdtRotCreator     ->correct( *(mdt->prepRawData()), *pars );
	    
	
	if( !newMdt ) {
	  ATH_MSG_WARNING(" Failed to recalibrate MDT ");
	  continue;
	}
	const Trk::FitQualityOnSurface* fqos=(**tsit).fitQualityOnSurface() ? 
	  new Trk::FitQualityOnSurface(*(**tsit).fitQualityOnSurface()) : 0;
	const Trk::MaterialEffectsBase* meb=( (*tsit)->materialEffectsOnTrack()) ?
	  (*tsit)->materialEffectsOnTrack()->clone() : 0;
	Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface( newMdt, pars->clone(), fqos, meb, 
								       (*tsit)->types() );
	newStates.push_back( std::make_pair(true,tsos) );	
	
	
	
      }else if( m_idHelperTool->isCsc(id) ) {
	
	const CscClusterOnTrack* csc = dynamic_cast<const CscClusterOnTrack*>(meas);
	if( !csc ){
	  ATH_MSG_WARNING(" Measurement with MDT identifier that is not a MdtDriftCircleOnTrack ");
	  continue;
	}
	const Trk::RIO_OnTrack* newCsc = m_cscRotCreator->correct( *csc->prepRawData(), *pars );
	if( !newCsc ) {
	  ATH_MSG_WARNING(" Failed to recalibrate CSC ");
	  continue;
	}
	const Trk::FitQualityOnSurface* fqos=(**tsit).fitQualityOnSurface() ? 
	  new Trk::FitQualityOnSurface(*(**tsit).fitQualityOnSurface()) : 0;
	const Trk::MaterialEffectsBase* meb=( (*tsit)->materialEffectsOnTrack()) ?
	  (*tsit)->materialEffectsOnTrack()->clone() : 0;	
	Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface( newCsc, pars->clone(), fqos, meb, 
								       (*tsit)->types() );
	newStates.push_back( std::make_pair(true,tsos) );	
	
	
	
      }
      else if (  m_idHelperTool->isTrigger(id) ) {
	
	tsit = insertClustersWithCompetingRotCreation( tsit, tsit_end, newStates );	
	
      }
      else {
	ATH_MSG_WARNING(" unknown Identifier ");
      }      
    }
    
    ATH_MSG_DEBUG(" original track had " << states->size() 
		  << " TSOS, adding " << newStates.size() - states->size() 
		  << " new TSOS ");
    
    // states were added, create a new track
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
    trackStateOnSurfaces->reserve( newStates.size() );
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >::iterator nit = newStates.begin();
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >::iterator nit_end = newStates.end();
    for( ;nit!=nit_end;++nit ){
      // add states. If nit->first is true we have a new state. If it is false the state is from the old track and has to be cloned
      trackStateOnSurfaces->push_back( nit->first ? nit->second : nit->second->clone() );
    }
    Trk::Track* newTrack =  new Trk::Track( track.info(), trackStateOnSurfaces, track.fitQuality() ? track.fitQuality()->clone():0 );
    
    return newTrack;
  }
  
  //________________________________________________________________________
  DataVector<const Trk::TrackStateOnSurface>::const_iterator 
  MuonAlignHelperTool::
  insertClustersWithCompetingRotCreation( DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit,
					  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end,
					  std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states) const {
    
    // iterator should point to a valid element
    if ( tsit == tsit_end ) {
      ATH_MSG_WARNING(" iterator pointing to end of vector, this should no happen ");
      return --tsit;
    }
    
    // check whether state is a measurement
    const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
    const Trk::TrackParameters* pars = (*tsit)->trackParameters();
    if ( !meas  || !pars ) {
      ATH_MSG_WARNING(" iterator pointing to a TSOS without a measurement or TrackParameters ");
      if ( tsit+1 == tsit_end ) --tsit;
      return tsit;
    }
    
    ATH_MSG_VERBOSE(" inserting with competing ROT creation ");

    // loop over states until we reached the last tgc hit in this detector element
    // keep trackof the identifiers and the states
    std::list<const Trk::PrepRawData*> etaPrds;
    std::list<const Trk::PrepRawData*> phiPrds;
    const Trk::TrkDetElementBase* currentDetEl = 0;
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > newStates;
    // keep track of outliers as we might have to drop them..
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > outlierStates;
    bool hasPhi = false;
    bool hasEta = false;

    for ( ;tsit!=tsit_end;++tsit ){
      
      if ( !*tsit ) continue;
      
      // check whether state is a measurement, keep if not
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if ( !meas ) {
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }

      // get identifier, keep state if it has no identifier.
      Identifier id = m_helperTool->getIdentifier(*meas);
      if ( !id.is_valid() ) {
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }


      // sanity check, this SHOULD be a RPC, TGC or CSC measurement
      if ( !( m_idHelperTool->isTrigger(id) ) ){
	break;
      }

      bool measuresPhi = m_idHelperTool->measuresPhi(id);
      if ( !hasPhi && measuresPhi )  hasPhi = true;
      if ( !hasEta && !measuresPhi ) hasEta = true;

      // check whether state is a measurement
      if ( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) {	
	outlierStates.push_back( std::make_pair(measuresPhi,*tsit) );
	continue;
      }

      
      // check whether we are still in the same chamber, stop loop if not

      ATH_MSG_VERBOSE(" handling " << m_idHelperTool->toString(id));
      
      std::list<const Trk::PrepRawData*>& prdList = measuresPhi ? phiPrds : etaPrds;
      const MuonClusterOnTrack* clus = dynamic_cast<const MuonClusterOnTrack*>(meas);
      if ( clus ) {
	const Trk::TrkDetElementBase* detEl = clus->detectorElement();
	if ( !currentDetEl ) currentDetEl = detEl;
	if ( detEl != currentDetEl ){
	  ATH_MSG_VERBOSE(" new detector element stopping ");
	  break;
	}
	prdList.push_back(clus->prepRawData());
      }
      else {
	// split competing ROTs into constituents
	const CompetingMuonClustersOnTrack* comp = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
	if ( comp ) {
	  
	  const Trk::TrkDetElementBase* detEl = 0; 
	  if ( comp->containedROTs().empty() ) {
	    ATH_MSG_WARNING(" CompetingROT without constituents ");
	    break;
	  }
	  detEl = comp->containedROTs().front()->detectorElement();
	  if ( !currentDetEl ) currentDetEl = detEl;
	  if ( detEl != currentDetEl ) {
	    ATH_MSG_VERBOSE(" new detector element stopping ");
	    break;
	  }	  
	  std::vector<const MuonClusterOnTrack*>::const_iterator clit = comp->containedROTs().begin();
	  std::vector<const MuonClusterOnTrack*>::const_iterator clit_end = comp->containedROTs().end();
	  for ( ;clit!=clit_end;++clit ) {
	    prdList.push_back((*clit)->prepRawData());
	  }
	  
	}
	else {
	  ATH_MSG_WARNING(" Unknown trigger hit type! ");
	  continue;
	}
      }
    }
    
    // now that we have the lists of prds we can create the competing rots
    if ( !etaPrds.empty() ) {
      const CompetingMuonClustersOnTrack* etaCompRot = m_compRotCreator->createBroadCluster(etaPrds,0.);
      if ( !etaCompRot ) {
	ATH_MSG_WARNING(" Failed to create CompetingMuonClustersOnTrack for eta hits! ");
      }
      else {
	
	const Trk::TrackParameters* etaPars = 0;
	// check whether original parameters are on surface, if so clone original parameters
	if ( pars->associatedSurface() && etaCompRot->associatedSurface() == *pars->associatedSurface() ) {
	  etaPars = pars->clone();
	}
	else {
	  etaPars = m_propagator->propagate(*pars,etaCompRot->associatedSurface(),Trk::anyDirection,
					    false,*m_magFieldProperties);
	}
	if ( !etaPars ) {
	  ATH_MSG_WARNING(" Failed to calculate TrackParameters for eta hits! ");
	  delete etaCompRot;
	}
	else {
	  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
    typePattern.set(Trk::TrackStateOnSurface::Measurement);
    typePattern.set(Trk::TrackStateOnSurface::Parameter);
	  Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface( etaCompRot, etaPars, 0, 0, typePattern );
	  newStates.push_back( std::make_pair(true,tsos) );
	}
      }
    }
    
    if ( !phiPrds.empty() ) {
      const CompetingMuonClustersOnTrack* phiCompRot = m_compRotCreator->createBroadCluster(phiPrds,0.);
      if ( !phiCompRot ) {
	ATH_MSG_WARNING(" Failed to create CompetingMuonClustersOnTrack for phi hits! ");
      }
      else {
	
	const Trk::TrackParameters* phiPars = 0;
	// check whether original parameters are on surface, if so clone original parameters
	if ( pars->associatedSurface() && phiCompRot->associatedSurface() == *pars->associatedSurface() ) {
	  phiPars = pars->clone();
	}
	else {
	  phiPars = m_propagator->propagate(*pars,phiCompRot->associatedSurface(),Trk::anyDirection,
					    false,*m_magFieldProperties);
	}
	if ( !phiPars ) {
	  ATH_MSG_WARNING(" Failed to calculate TrackParameters for phi hits! ");
	  delete phiCompRot;
	}
	else {
	  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern; 
    typePattern.set(Trk::TrackStateOnSurface::Measurement);
	  typePattern.set(Trk::TrackStateOnSurface::Parameter);
	  Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface( phiCompRot, phiPars, 0, 0, typePattern );
	  newStates.push_back( std::make_pair(true,tsos) );
	}
      }
    }
    
    // add outliers if there was no measurement on track in the same projection
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >::iterator outIt = outlierStates.begin();
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >::iterator outIt_end = outlierStates.end();
    for( ;outIt!=outIt_end;++outIt ){
      if ( hasPhi && outIt->first )  newStates.push_back( std::make_pair(false,outIt->second) );
      else if ( hasEta && !outIt->first ) newStates.push_back( std::make_pair(false,outIt->second));
      else ATH_MSG_DEBUG(" Dropping outlier ");
    }    
    
    // sort all states in this chamber
    std::stable_sort(newStates.begin(),newStates.end(),SortTSOSByDistanceToPars(pars) );
    
    // insert the states into 
    states.insert( states.end(),newStates.begin(),newStates.end() );
    
    // iterator should point to the last TGC in this chamber
    return --tsit;
  }
  */

  //________________________________________________________________________
  float MuonAlign::RadsToDegrees(float Radians)
  {
    if (Radians < 0) Radians = (-1 * Radians) + M_PI;
    float Degrees = Radians * (180.)/M_PI;
    return Degrees;
  }
  
  //________________________________________________________________________
  double MuonAlign::FixRad(double inputRad)
  {
    double Rad=inputRad;
    if (Rad>M_PI)       Rad -= 2.*M_PI; 
    else if (Rad<-M_PI) Rad += 2.*M_PI;
    
    if (fabs(Rad)>M_PI/2.) {
      if ( Rad > 0. ) Rad -=M_PI ;
      else            Rad +=M_PI ;	
    }
    return Rad;
  }

  //________________________________________________________________________
  StationLayer MuonAlignHelperTool::getMdtStatLayer(Identifier id) const 
  {
    
    const std::string stName=
      m_muonIdHelper->mdtIdHelper().stationNameString(m_muonIdHelper->mdtIdHelper().stationName(id));
    
    // station layer
    switch ( stName[1] ) {
    case 'I' :
      return Inner;
    case 'M' :
      return Middle;
    case 'O' :
      return Outer;
    default:
      return Other;
    }    
    return Other;
  }

} // end namespace
