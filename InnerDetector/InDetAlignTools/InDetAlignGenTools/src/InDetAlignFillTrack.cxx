/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
// ================================================
// InDetAlignFillTrack
// ================================================
//
// InDetAlignFillTrack.cxx
// Source file for InDetAlignFillTrack
//
// Carlos Escobar, started 27/12/2007
//
// AlgTool to fill track information (including truth) in a ntuple


#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "GaudiKernel/IPartPropSvc.h"

#include "DataModel/DataVector.h"

#include "TrkTrack/Track.h"

#include "TrkTruthData/TrackTruth.h"
#include "TrkTruthData/TrackTruthCollection.h"

#include "TrkParameters/TrackParameters.h"

#include "TrackRecord/TrackRecord.h"
#include "TrackRecord/TrackRecordCollection.h"


#include "TrkToolInterfaces/ITruthToTrack.h"

#include "TrkTrackSummary/TrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"

#include "InDetAlignGenTools/InDetAlignFillTrack.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include <string>

static const int maxTracks = 10000; // maximal number of tracks per event

//=====================================================================
// InDetAlignFillTrack()
//=====================================================================
InDetAlignFillTrack::InDetAlignFillTrack (const std::string& type,
					  const std::string& name,
					  const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_ntupleSvc(0),
    m_totaltrks(0),
    m_totalhits(0),
    m_totalPixhits(0),
    m_totalSCThits(0),
    m_totalTRThits(0),
    m_totalUptrks(0),
    m_totalUphits(0),
    m_totalUpPixhits(0),
    m_totalUpSCThits(0),
    m_totalUpTRThits(0),
    m_totalLowtrks(0),
    m_totalLowhits(0),
    m_totalLowPixhits(0),
    m_totalLowSCThits(0),
    m_totalLowTRThits(0),
    m_events(0),
    m_truthToTrack("Trk::TruthToTrack", this),
    m_extrapolator("Trk::Extrapolator/CosmicsExtrapolator", this),
    m_trackSumTool("Trk::TrackSummaryTool", this)
{
  declareInterface<IInDetAlignFillTrack>(this);
  declareProperty("InputTrkCol", m_inputCol="Tracks");
  declareProperty("InputUpTrkCol", m_inputUpCol="");
  declareProperty("InputLowTrkCol", m_inputLowCol="");

  // cosmic segments matching
  declareProperty("doMatching", m_doMatching = true);
  declareProperty("dRCut",      m_matchedRcut = 100.);
  declareProperty("minimumdR",  m_mindR = 10000.);

  // Truth information
  declareProperty("doTruth",     m_doTruth     = false);
  declareProperty("TruthTrkCol", m_TruthTrkCol = "TrackTruthCollection");

  // Ntuple
  declareProperty("NtupleName", m_ntupleName="/NTUPLES/GLOBFILE");

  // Tools
  declareProperty("TruthToTrackTool", m_truthToTrack, 
		  "tool to produce perigee track parameters from generated parameters");
  declareProperty("ExtrapolationTool",m_extrapolator,
		  "tool to extrapolate tracks");
  declareProperty("TrackSummaryTool",m_trackSumTool,
		  "tool to extract track info");
}


//=====================================================================
// ~FillTrack()
//=====================================================================
InDetAlignFillTrack::~InDetAlignFillTrack() {}


//=====================================================================
// initialize()
//===================================================================== 
StatusCode InDetAlignFillTrack::initialize() {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialize() of FillTrack" << endmsg;
  
  // retrieve the NTuple Service
  if (StatusCode::SUCCESS != service("NTupleSvc", m_ntupleSvc)) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "NTupleSvc service not found" << endmsg;
    return StatusCode::FAILURE;
  }

  // get TrackSummaryTool
  if ( m_trackSumTool.retrieve().isFailure() ) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Failed to retrieve tool " << m_trackSumTool << endmsg;
    return StatusCode::FAILURE;
  } 
  else { if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_trackSumTool << endmsg; }
  
  // if Truth...
  if (m_doTruth) {
    // Get TruthToTrack
    if ( m_truthToTrack.retrieve().isFailure() ) {
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Failed to retrieve tool " << m_truthToTrack << endmsg;
      return StatusCode::FAILURE;
    }
    else { if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_truthToTrack << endmsg; }

    // Get Extrapolator Tool
    if ( m_extrapolator.retrieve().isFailure() ) {
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Failed to retrieve tool " << m_extrapolator << endmsg;
      return StatusCode::FAILURE;
    }
    else { if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_extrapolator << endmsg; }
    
    // retrieve the PartPropSvc service (need for Cosmics)
    IPartPropSvc* p_PartPropSvc;
    static const bool CREATEIFNOTTHERE(true);
    StatusCode PartPropStatus = svcLoc()->service( "PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE );
    
    if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) {
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "FillTrack: could not find PartPropSvc" << endmsg;
      return StatusCode::FAILURE;
    }   
    m_mctable = const_cast<HepPDT::ParticleDataTable*>(p_PartPropSvc->PDT());
    if(m_mctable == 0){
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "FillTrack: PDG table not found" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Book Ntuple
  bookNtuple();
  if (m_inputUpCol!="") bookUpNtuple();
  if (m_inputLowCol!="") bookLowNtuple(); 
  if (m_doMatching && m_inputUpCol!="" && m_inputLowCol!="") bookMatchingNtuple();

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialize() of FillTrack successful" << endmsg;
  return StatusCode::SUCCESS;
}


//=====================================================================
// finalize()
//===================================================================== 
StatusCode InDetAlignFillTrack::finalize() {

  if (msgLvl(MSG::DEBUG)) {

    msg(MSG::DEBUG) << "Finalize() of FillTrack" << endmsg;

    msg(MSG::DEBUG) << "________________________________________________________" << endmsg;
    msg(MSG::DEBUG) << endmsg;
    msg(MSG::DEBUG) << " InDetAlignFillTrack Summary: " << endmsg;
    msg(MSG::DEBUG) << " - " << m_events << " events" << endmsg;
    msg(MSG::DEBUG) << " - " << m_totaltrks << " tracks" << endmsg;
    msg(MSG::DEBUG) << " - " << m_totalhits << " hits" << endmsg;
    msg(MSG::DEBUG) << "  - " << m_totalPixhits << " Pixel hits" << endmsg;
    msg(MSG::DEBUG) << "  - " << m_totalSCThits << " SCT hits" << endmsg;
    msg(MSG::DEBUG) << "  - " << m_totalTRThits << " TRT hits" << endmsg;
    msg(MSG::DEBUG) << "________________________________________________________" << endmsg;

    if (m_inputUpCol!="") {
      msg(MSG::DEBUG) << " Up Track Summary: " << endmsg;
      msg(MSG::DEBUG) << " - " << m_totalUptrks << " tracks" << endmsg;
      msg(MSG::DEBUG) << " - " << m_totalUphits << " hits" << endmsg;
      msg(MSG::DEBUG) << "  - " << m_totalUpPixhits << " Pixel hits" << endmsg;
      msg(MSG::DEBUG) << "  - " << m_totalUpSCThits << " SCT hits" << endmsg;
      msg(MSG::DEBUG) << "  - " << m_totalUpTRThits << " TRT hits" << endmsg;
      msg(MSG::DEBUG) << "________________________________________________________" << endmsg;
    }
  
    if (m_inputLowCol!="") {
      msg(MSG::DEBUG) << " Low Track Summary: " << endmsg;
      msg(MSG::DEBUG) << " - " << m_totalLowtrks << " tracks" << endmsg;
      msg(MSG::DEBUG) << " - " << m_totalLowhits << " hits" << endmsg;
      msg(MSG::DEBUG) << "  - " << m_totalLowPixhits << " Pixel hits" << endmsg;
      msg(MSG::DEBUG) << "  - " << m_totalLowSCThits << " SCT hits" << endmsg;
      msg(MSG::DEBUG) << "  - " << m_totalLowTRThits << " TRT hits" << endmsg;
      msg(MSG::DEBUG) << "________________________________________________________" << endmsg;
    }

    msg(MSG::DEBUG) << endmsg;

  }

  return StatusCode::SUCCESS;
}


//=====================================================================
// FillTrack()
//===================================================================== 
StatusCode InDetAlignFillTrack::FillTrack() {

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "In FillTrack()" << endmsg;
    msg(MSG::DEBUG) << " event " << m_events << endmsg;    
  }

  StatusCode sc;

  const TrackCollection* tracks = new TrackCollection;
  const TrackCollection* Uptracks = new TrackCollection;
  const TrackCollection* Lowtracks = new TrackCollection;

  // retrieve all tracks from TDS  
  if (StatusCode::SUCCESS!=evtStore()->retrieve(tracks,m_inputCol)) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot find " << m_inputCol  << endmsg;
    return StatusCode::FAILURE;
  }

  // retrieve all Up tracks from TDS
  if (m_inputUpCol!="") {
    if (StatusCode::SUCCESS!=evtStore()->retrieve(Uptracks,m_inputUpCol)) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot find " << m_inputUpCol  << endmsg;
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG( "Collection with name "<< m_inputUpCol <<" found in StoreGate" );
  }

  // retrieve all Low tracks from TDS
  if (m_inputLowCol!="") {
    if (StatusCode::SUCCESS!=evtStore()->retrieve(Lowtracks,m_inputLowCol)) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot find " << m_inputLowCol  << endmsg;
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG( "Collection with name "<< m_inputLowCol <<" found in StoreGate" );
  }

  m_nt_ntracks = tracks->size();
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved Track Collection size: " << tracks->size() << endmsg;

  if (m_inputUpCol!="") {
    m_nt_nUptracks = Uptracks->size();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved Up Track Collection size: " << Uptracks->size() << endmsg;
  }

  if (m_inputLowCol!="") {
    m_nt_nLowtracks = Lowtracks->size();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved Low Track Collection size: " << Lowtracks->size() << endmsg;
  }

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Printing input track collection" << endmsg; 
  m_totaltrks += dumpTrackCol(tracks);
  if (m_inputUpCol!="") m_totalUptrks += dumpTrackCol(Uptracks,"Up");
  if (m_inputLowCol!="") m_totalLowtrks += dumpTrackCol(Lowtracks,"Low");

  // matching
  if (m_doMatching && m_inputUpCol!="" && m_inputLowCol!="") 
    if(StatusCode::SUCCESS!=dumpMatching(Uptracks,Lowtracks)) { 
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "dumpMatching failure" << endmsg;
      return StatusCode::FAILURE;
    }
      

  // if truth is available...
  if (m_doTruth) {

    int nTracks = 0;

    const TrackTruthCollection *truthCol = NULL;
    
    // retrieve all track truth collection from TDS  
    if (StatusCode::SUCCESS!=evtStore()->retrieve(truthCol, m_TruthTrkCol)) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot find " << m_inputCol  << endmsg;
      return StatusCode::FAILURE;
    }
    else {
      if (!truthCol)
	{
	  if (msgLvl(MSG::ERROR)) {msg(MSG::ERROR) <<  "Failure retrieving " << m_TruthTrkCol << endmsg;}
	  return StatusCode::FAILURE;
	}
      
      if (msgLvl(MSG::DEBUG)) {
	msg(MSG::DEBUG) << "Collection with name "<< m_TruthTrkCol <<" found in StoreGate" << endmsg;
	msg(MSG::DEBUG) << "Retrieved "<< truthCol->size() <<" truth tracks from StoreGate" << endmsg;
      }
      
      m_nt_nmctracks = truthCol->size();
      
      TrackCollection::const_iterator trackItr  = tracks->begin();
      TrackCollection::const_iterator trackItrE = tracks->end();
      
      //looping over tracks
      for (; trackItr != trackItrE && nTracks < maxTracks; ++trackItr) {
	
        const Trk::Track* track = *trackItr;
        if(track == NULL){
          if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "No associated Trk::Track object found for track "
                  << nTracks << endmsg;
          continue;
        }
	//Coverity fix: 14309: useless check. Commented out
        //if (truthCol) {
           
          // the key for the truth std::map is an ElementLink<TrackCollection> object
          // comprises a pointer to the track and reconstructed track collection
	ElementLink<TrackCollection> trackLink;
	trackLink.setElement(const_cast<Trk::Track*>(track));
	trackLink.setStorableObject(*tracks);
	const ElementLink<TrackCollection> trackLink2 = trackLink;
	
	// trying to find the std::map entry for this reconstructed track
	TrackTruthCollection::const_iterator found = truthCol->find(trackLink2);
    
	if (found != truthCol->end()) {
	  
	  // getting the TrackTruth object - the map element
	  TrackTruth trkTruth = found->second;
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "got trkTruth" << endmsg;
	  
	  // probability of the reco<->truth match
	  float trkTruthProb =  trkTruth.probability();
	  
	  HepMcParticleLink HMPL = trkTruth.particleLink();
	  
	  if ( HMPL.isValid()) {
	    const HepMC::GenParticle *genParticle = HMPL.cptr(); 
	    
	    double charge = 1.0;      
	    if (genParticle->pdg_id()<0) charge=-charge;
	    
	    Amg::Vector3D productionVertex(genParticle->production_vertex()->position().x(),
					   genParticle->production_vertex()->position().y(),
					   genParticle->production_vertex()->position().z() );
	    
	    if (msgLvl(MSG::DEBUG)) {
	      msg(MSG::DEBUG) << nTracks << ". Generated Particle "<< endmsg;
	      msg(MSG::DEBUG) << "   * PDG " << genParticle->pdg_id() 
			      << ", Status " << genParticle->status() 
			      << ", mass " << genParticle->momentum().m() << " CLHEP::MeV/c"
			      << endmsg;
	    }
	    
	    float genPt = sqrt((genParticle->momentum().x())*(genParticle->momentum().x()) 
			       + (genParticle->momentum().y())*(genParticle->momentum().y()));
	    
	    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "   * pt " << genPt/CLHEP::GeV << " CLHEP::GeV/c"
						    << ", p " << genParticle->momentum().e()/CLHEP::GeV << " CLHEP::GeV/c"
						    << ", eta " << genParticle->momentum().eta() 
						    << ", phi "<< genParticle->momentum().phi() << " CLHEP::rad"
						    << endmsg;
	    
	    m_nt_mc_trkistruth[nTracks] = 1;
	    m_nt_mc_Trk_pdg[nTracks] = genParticle->pdg_id();
	    m_nt_mc_Trk_prob[nTracks] = trkTruthProb;
	    float pX = genParticle->momentum().px();  float pY = genParticle->momentum().py();
	    float genParticlePt = sqrt((pX*pX)+(pY*pY));
	    m_nt_mc_Trk_genParticlePt[nTracks] = genParticlePt;
	    m_nt_mc_Trk_genParticleEta[nTracks] = genParticle->momentum().eta();
	    m_nt_mc_Trk_genParticlePhi[nTracks] = genParticle->momentum().phi();
	    
	    if(genParticle->pdg_id()==0) 
	      ATH_MSG_WARNING("Particle with PDG 0!");
	    else if(!genParticle->production_vertex()) 
	      ATH_MSG_WARNING( "No GenVertex (generator level) production vertex found!");
	    else {
	      // currently cannot configure the TruthToTrack tool properly
	      
	      const Trk::TrackParameters* generatedTrackPerigee = 0;
	      
	      //using a tool to produce perigee track parameters from generated parameters
	      generatedTrackPerigee = m_truthToTrack->makePerigeeParameters(genParticle);
	      
	      if (!generatedTrackPerigee) {
		if (msgLvl(MSG::DEBUG)) {
		  msg(MSG::DEBUG) <<  "Unable to extrapolate genParticle to perigee!" << endmsg;   
		  msg(MSG::DEBUG) <<  "Trying to extrapolate directly to exclude material effects!" << endmsg;   
		}
		
		const TrackRecordCollection* recordCollection;
		
		sc = evtStore()->retrieve(recordCollection, "CaloEntryLayer");
		if (sc==StatusCode::FAILURE) {
		  if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get track record!" << endmsg;
		  return sc;
		}
		if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "reading from track record, size = " 
							<< recordCollection->size() << endmsg;
		
		int nmctracks = 0;
		
		for (TrackRecordCollection::const_iterator record = recordCollection->begin();  
		     record != recordCollection->end();++record) {
		  
		  const HepPDT::ParticleData* particle = m_mctable->particle(abs((*record).GetPDGCode()));
		  if (!particle) continue;
		  
		  double charge=particle->charge();
		  if (std::abs(charge)<0.01) continue;
		  
		  HepGeom::Point3D<double> productionVertex = (*record).GetPosition();
		  if ((*record).GetPDGCode()<0) charge=-charge; 
		  if (fabs((*record).GetPDGCode())!=13) continue;
		  
		  Amg::Vector3D direction((*record).GetMomentum().x(), 
					  (*record).GetMomentum().y(), 
					  (*record).GetMomentum().z());
		  
		  double momentum = direction.mag();
		  if (momentum<500) continue;
		  double genPar_qOverP = 1./direction.mag();
		  direction *= genPar_qOverP;	      
		  if (charge<0) genPar_qOverP = -genPar_qOverP;
		  
		  
		  double genPar_phi = direction.phi();
		  if(genPar_phi<0.) genPar_phi = genPar_phi+(4*asin(1.));
		  
		  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Production vertex (x,y,z): (" 
							  << productionVertex.x() << ", " 
							  << productionVertex.y() << ", " 
							  << productionVertex.z() << ")" 
							  << endmsg;
		  
		  double genPar_theta  = direction.theta();
		  
		  // Create a planar surface and transform the vertex information to a TrackParameters object      
		  Amg::Transform3D* globalSurfaceCentre = new Amg::Transform3D();
		  globalSurfaceCentre->setIdentity();
		  *globalSurfaceCentre *= Amg::Translation3D(productionVertex.x(), productionVertex.y(), productionVertex.z() );
		  
		  Trk::PlaneSurface planeSurface( globalSurfaceCentre, 5., 5. );
		  
		  const Amg::Vector3D productionVertexAsGlobalPosition( productionVertex.x(), 
									productionVertex.y(), 
									productionVertex.z() );
		  
		  const Trk::AtaPlane* productionVertexTrackParams 
		    = new Trk::AtaPlane( productionVertexAsGlobalPosition,
					 genPar_phi, genPar_theta, genPar_qOverP,
					 planeSurface );
		  
		  // Create a new perigee surface
		  Trk::PerigeeSurface perigeeSurface;
		  
		  if (!tracks->empty())
		    perigeeSurface=((**tracks->begin()).perigeeParameters()->associatedSurface());
		  
		  const Amg::Vector3D& perigeeGlobalPosition = perigeeSurface.center();
		  
		  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Surface global centre (x,y,z): (" 
							  << perigeeGlobalPosition.x() << ", " 
							  << perigeeGlobalPosition.y() << ", " 
							  << perigeeGlobalPosition.z() << ")"
							  << endmsg;
		  
		  // Extrapolate the TrackParameters object to the perigee
		  
		  // ( Establish the distance between perigee and generated vertex.
		  // If less than tolerance don't bother with the propagation )
		  const Amg::Vector3D difference = productionVertexAsGlobalPosition - perigeeGlobalPosition;
		  
		  double distance = sqrt( difference.x() * difference.x() + difference.y() * difference.y() );
		  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Distance between perigee point and generated vertex: " 
							  << distance/CLHEP::m << " m" << endmsg;
		  
		  const Trk::TrackParameters* generatedTrackPerigee = 0;
		  
		  // Extrapolate directly to exclude material effects!
		  if ( distance > 1.e-4 ) {
		    
		    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Distance between perigee and generated vertex exceeds tolerance (" 
							    << 1.e-4 << " mm)... Extrapolating!" << endmsg;
		    
		    generatedTrackPerigee = m_extrapolator->extrapolateDirectly( *productionVertexTrackParams,
										 perigeeSurface,
										 Trk::anyDirection,
										 false,
										 Trk::nonInteracting );
		    if (!generatedTrackPerigee) continue;
		  }
		  
		  else {
		    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Distance between perigee and generated vertex is less than tolerance (" 
							    << 1.e-4 << " CLHEP::mm)... " << "No propagation to perigee required" << endmsg;
		    
		    // Clone the parameters from the AtaPlane object on to perigee
		    generatedTrackPerigee = new Trk::Perigee( 0., 0.,
							      genPar_phi, genPar_theta, genPar_qOverP,
							      Trk::PerigeeSurface(productionVertexAsGlobalPosition) );
		    
		  }
		  
		  dumpPerigee(generatedTrackPerigee, nmctracks); 
		  m_nt_mc_Trk_vtxX[nmctracks] = productionVertex.x();
		  m_nt_mc_Trk_vtxY[nmctracks] = productionVertex.y();
		  m_nt_mc_Trk_vtxZ[nmctracks] = productionVertex.z();
		  
		  delete productionVertexTrackParams;
		  delete generatedTrackPerigee;
		  
		  nmctracks++;
		}
	      }
	      
	      if (generatedTrackPerigee) {
		dumpPerigee(generatedTrackPerigee, nTracks);     
		m_nt_mc_Trk_vtxX[nTracks] = genParticle->production_vertex()->position().x();
		m_nt_mc_Trk_vtxY[nTracks] = genParticle->production_vertex()->position().y();
		m_nt_mc_Trk_vtxZ[nTracks] = genParticle->production_vertex()->position().z();
		
		delete generatedTrackPerigee;
		
	      }
	      
	    }
	  }
	  m_nt_mc_trkistruth[nTracks] = 0;
	}
	//} // if (truthCol)  commented out for coverity 14309
  
        nTracks++; 
	 
      }    
    }
  } // end if m_doTruth

  // Store TrkTrack branch
  std::string nt0id = m_ntupleName + "/TrkTrack";
  sc = m_ntupleSvc->writeRecord(nt0id);
  if (sc.isFailure()) if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not write " << nt0id << "!" << endmsg;
  
  if (m_inputUpCol!="") {
    // Store TrkTrack_Up branch
    std::string nt1id = m_ntupleName + "/TrkTrack_Up";
    sc = m_ntupleSvc->writeRecord(nt1id);
    if (sc.isFailure()) if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not write " << nt1id << "!" << endmsg;
  }
  
  if (m_inputLowCol!="") {
    // Store TrkTrack_Low branch
    std::string nt2id = m_ntupleName + "/TrkTrack_Low";
    sc = m_ntupleSvc->writeRecord(nt2id);
    if (sc.isFailure()) if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not write " << nt2id << "!" << endmsg;
  }
  
  if (m_doMatching && m_inputUpCol!="" && m_inputLowCol!="") {
    // Store Matching Up/Low TrkTracks branch
    std::string nt3id = m_ntupleName + "/TrkTrack_Matching";
    sc = m_ntupleSvc->writeRecord(nt3id);
    if (sc.isFailure()) if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not write " << nt3id << "!" << endmsg;
  }
  

  m_events++;

  // check for negative values
  if (m_totaltrks<0) m_totaltrks = 0;
  if (m_totalhits<0) m_totalhits = 0;
  if (m_totalPixhits<0) m_totalPixhits = 0;
  if (m_totalSCThits<0) m_totalSCThits = 0;
  if (m_totalTRThits<0) m_totalTRThits = 0;

  if (m_inputUpCol!="") {
    if (m_totalUptrks<0) m_totalUptrks = 0;
    if (m_totalUphits<0) m_totalUphits = 0;
    if (m_totalUpPixhits<0) m_totalUpPixhits = 0;
    if (m_totalUpSCThits<0) m_totalUpSCThits = 0;
    if (m_totalUpTRThits<0) m_totalUpTRThits = 0;
  }

  if (m_inputLowCol!="") {
    if (m_totalLowtrks<0) m_totalLowtrks = 0;
    if (m_totalLowhits<0) m_totalLowhits = 0;
    if (m_totalLowPixhits<0) m_totalLowPixhits = 0;
    if (m_totalLowSCThits<0) m_totalLowSCThits = 0;
    if (m_totalLowTRThits<0) m_totalLowTRThits = 0;
  }

  if (m_events<0) m_events = 0;

  return StatusCode::SUCCESS;
}


//=====================================================================
// bookNtuple()
//===================================================================== 
void InDetAlignFillTrack::bookNtuple() {

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Booking Trk::Track Info..." << endmsg;
    
  NTupleFilePtr file1(m_ntupleSvc, m_ntupleName);
  std::string nt0id    = m_ntupleName + "/TrkTrack";
  std::string comments = "Trk::Track Information";
  
  NTuplePtr nt0(m_ntupleSvc, nt0id);
  if (nt0) {if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ntuple is already booked" << endmsg;}
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Attempting to book general ntuple" << endmsg;
    nt0 = m_ntupleSvc->book(nt0id,CLID_ColumnWiseTuple,comments);
    
    if (nt0) {
      StatusCode sc;
      // nt0->addItem("event", m_nt_event); // event number
      sc =  nt0->addItem("nTracks", m_nt_ntracks, 0, maxTracks); // number of tracks
      if (m_doTruth) sc =nt0->addItem("mc_nTracks", m_nt_nmctracks, 0, maxTracks); // number of mc tracks
      
      // ----------------------------------------------------------------------
      // Trk::Track parameters
      sc = nt0->addItem("Trk_d0",     m_nt_ntracks, m_nt_Trk_d0);
      sc = nt0->addItem("Trk_z0",     m_nt_ntracks, m_nt_Trk_z0);
      sc = nt0->addItem("Trk_phi0",   m_nt_ntracks, m_nt_Trk_phi0);
      sc = nt0->addItem("Trk_theta0", m_nt_ntracks, m_nt_Trk_theta0);
      sc = nt0->addItem("Trk_qoverp", m_nt_ntracks, m_nt_Trk_qoverp);
      sc = nt0->addItem("Trk_pt",     m_nt_ntracks, m_nt_Trk_pt);
      // ----------------------------------------------------------------------

      // ----------------------------------------------------------------------
      // Trk::Track hits...
      sc = nt0->addItem("Trk_nHits",       m_nt_ntracks, m_nt_Trk_nHits);
      sc = nt0->addItem("Trk_nhitsPixels", m_nt_ntracks, m_nt_Trk_nhitspix);
      sc = nt0->addItem("Trk_nhitsSCT",    m_nt_ntracks, m_nt_Trk_nhitssct);
      sc = nt0->addItem("Trk_nhitsTRT",    m_nt_ntracks, m_nt_Trk_nhitstrt);

      sc = nt0->addItem("Trk_nsharedPixels", m_nt_ntracks, m_nt_Trk_nsharedPixels);
      sc = nt0->addItem("Trk_nsharedSCT",    m_nt_ntracks, m_nt_Trk_nsharedSCT);
      sc = nt0->addItem("Trk_nshared",       m_nt_ntracks, m_nt_Trk_nshared);

      sc = nt0->addItem("Trk_nholesPixels", m_nt_ntracks, m_nt_Trk_nholesPixels);
      sc = nt0->addItem("Trk_nholesSCT",    m_nt_ntracks, m_nt_Trk_nholesSCT);
      sc = nt0->addItem("Trk_nholes",       m_nt_ntracks, m_nt_Trk_nholes);

      sc = nt0->addItem("Trk_chi2",     m_nt_ntracks, m_nt_Trk_chi2);
      sc = nt0->addItem("Trk_ndof",     m_nt_ntracks, m_nt_Trk_ndof);
      sc = nt0->addItem("Trk_chi2Prob", m_nt_ntracks, m_nt_Trk_chi2Prob);
      // ----------------------------------------------------------------------

      if (m_doTruth) {
	// ----------------------------------------------------------------------
	sc = nt0->addItem("mc_TrkIsTruth",  m_nt_ntracks, m_nt_mc_trkistruth);
	
	// generated particle parameters
	sc = nt0->addItem("mc_Trk_genParticlePt",  m_nt_ntracks, m_nt_mc_Trk_genParticlePt);
	sc = nt0->addItem("mc_Trk_genParticleEta", m_nt_ntracks, m_nt_mc_Trk_genParticleEta);
	sc = nt0->addItem("mc_Trk_genParticlePhi", m_nt_ntracks, m_nt_mc_Trk_genParticlePhi);
	
	// MonteCarlo Track parameters
	sc = nt0->addItem("mc_Trk_d0",      m_nt_ntracks, m_nt_mc_Trk_d0);
	sc = nt0->addItem("mc_Trk_z0",      m_nt_ntracks, m_nt_mc_Trk_z0);
	sc = nt0->addItem("mc_Trk_phi0",    m_nt_ntracks, m_nt_mc_Trk_phi0);
	sc = nt0->addItem("mc_Trk_theta",   m_nt_ntracks, m_nt_mc_Trk_theta0);
	sc = nt0->addItem("mc_Trk_eta",     m_nt_ntracks, m_nt_mc_Trk_eta);
	sc = nt0->addItem("mc_Trk_qoverp",  m_nt_ntracks, m_nt_mc_Trk_qoverp);
	sc = nt0->addItem("mc_Trk_qoverpt", m_nt_ntracks, m_nt_mc_Trk_qoverpt);
	sc = nt0->addItem("mc_Trk_pt",      m_nt_ntracks, m_nt_mc_Trk_pt);
	sc = nt0->addItem("mc_Trk_charge",  m_nt_ntracks, m_nt_mc_Trk_charge);
	sc = nt0->addItem("mc_Trk_prob",    m_nt_ntracks, m_nt_mc_Trk_prob);
	sc = nt0->addItem("mc_Trk_pdg",     m_nt_ntracks, m_nt_mc_Trk_pdg);

	sc = nt0->addItem("mc_Trk_vtxX",    m_nt_ntracks, m_nt_mc_Trk_vtxX);
	sc = nt0->addItem("mc_Trk_vtxY",    m_nt_ntracks, m_nt_mc_Trk_vtxY);
	sc = nt0->addItem("mc_Trk_vtxZ",    m_nt_ntracks, m_nt_mc_Trk_vtxZ);
	// ----------------------------------------------------------------------
      }

      if (sc.isFailure()) msg(MSG::FATAL) << "Failed ntupleSvc()" << endmsg;
      else  msg(MSG::DEBUG) << "Ntuple " << nt0id << " has been booked successfully! " << endmsg;
      //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ntuple " << nt0id << " has been booked successfully! " << endmsg;
    }

    else { if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Error booking ntuple" << endmsg; }
  }

  // return;
}


//=====================================================================
// bookUpNtuple()
//===================================================================== 
void InDetAlignFillTrack::bookUpNtuple() {
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Booking Up Trk::Track Info..." << endmsg;
    
  std::string nt1id    = m_ntupleName + "/TrkTrack_Up";
  std::string comments = "Trk::UpTrack Information";
    
  NTuplePtr nt1(m_ntupleSvc, nt1id);
  if (nt1) {if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ntuple is already booked" << endmsg;}
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Attempting to book general ntuple" << endmsg;
    nt1 = m_ntupleSvc->book(nt1id,CLID_ColumnWiseTuple,comments);
    
    if (nt1) {
      StatusCode sc;
      // nt1->addItem("event", m_nt_event); // event number
      sc = nt1->addItem("nTracks_Up", m_nt_nUptracks, 0, maxTracks); // number of tracks

      // ----------------------------------------------------------------------
      // Trk::Track parameters
      sc = nt1->addItem("Trk_d0_Up",     m_nt_nUptracks, m_nt_Trk_d0_Up);
      sc = nt1->addItem("Trk_z0_Up",     m_nt_nUptracks, m_nt_Trk_z0_Up);
      sc = nt1->addItem("Trk_phi0_Up",   m_nt_nUptracks, m_nt_Trk_phi0_Up);
      sc = nt1->addItem("Trk_theta0_Up", m_nt_nUptracks, m_nt_Trk_theta0_Up);
      sc = nt1->addItem("Trk_qoverp_Up", m_nt_nUptracks, m_nt_Trk_qoverp_Up);
      sc = nt1->addItem("Trk_pt_Up",     m_nt_nUptracks, m_nt_Trk_pt_Up);
      // ----------------------------------------------------------------------

      // ----------------------------------------------------------------------
      // Trk::Track hits...
      sc = nt1->addItem("Trk_nHits_Up",       m_nt_nUptracks, m_nt_Trk_nHits_Up);
      sc = nt1->addItem("Trk_nhitsPixels_Up", m_nt_nUptracks, m_nt_Trk_nhitspix_Up);
      sc = nt1->addItem("Trk_nhitsSCT_Up",    m_nt_nUptracks, m_nt_Trk_nhitssct_Up);
      sc = nt1->addItem("Trk_nhitsTRT_Up",    m_nt_nUptracks, m_nt_Trk_nhitstrt_Up);
      
      sc = nt1->addItem("Trk_nsharedPixels_Up", m_nt_nUptracks, m_nt_Trk_nsharedPixels_Up);
      sc = nt1->addItem("Trk_nsharedSCT_Up",    m_nt_nUptracks, m_nt_Trk_nsharedSCT_Up);
      sc = nt1->addItem("Trk_nshared_Up",       m_nt_nUptracks, m_nt_Trk_nshared_Up);
      
      sc = nt1->addItem("Trk_nholesPixels_Up", m_nt_nUptracks, m_nt_Trk_nholesPixels_Up);
      sc = nt1->addItem("Trk_nholesSCT_Up",    m_nt_nUptracks, m_nt_Trk_nholesSCT_Up);
      sc = nt1->addItem("Trk_nholes_Up",       m_nt_nUptracks, m_nt_Trk_nholes_Up);
      
      sc = nt1->addItem("Trk_chi2_Up",     m_nt_nUptracks, m_nt_Trk_chi2_Up);
      sc = nt1->addItem("Trk_ndof_Up",     m_nt_nUptracks, m_nt_Trk_ndof_Up);
      sc = nt1->addItem("Trk_chi2Prob_Up", m_nt_nUptracks, m_nt_Trk_chi2Prob_Up);
      // ----------------------------------------------------------------------
     
      if (sc.isFailure()) msg(MSG::FATAL) << "Failed ntupleSvc()" << endmsg;
      else  msg(MSG::DEBUG) << "Ntuple " << nt1id << " has been booked successfully! " << endmsg;
    }
    else { if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Error booking ntuple" << endmsg; }
  }

}


//=====================================================================
// bookLowNtuple()
//===================================================================== 
void InDetAlignFillTrack::bookLowNtuple() {

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Booking Low Trk::Track Info..." << endmsg;
    
  std::string nt2id    = m_ntupleName + "/TrkTrack_Low";
  std::string comments = "Trk::LowTrack Information";
    
  NTuplePtr nt2(m_ntupleSvc, nt2id);
  if (nt2) {if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ntuple is already booked" << endmsg;}
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Attempting to book general ntuple" << endmsg;
    nt2 = m_ntupleSvc->book(nt2id,CLID_ColumnWiseTuple,comments);
    
    if (nt2) {
      StatusCode sc;
      // sc = nt2->addItem("event", m_nt_event); // event number
      sc = nt2->addItem("nTracks_Low", m_nt_nLowtracks, 0, maxTracks); // number of tracks

      // ----------------------------------------------------------------------
      // Trk::Track parameters
      sc = nt2->addItem("Trk_d0_Low",     m_nt_nLowtracks, m_nt_Trk_d0_Low);
      sc = nt2->addItem("Trk_z0_Low",     m_nt_nLowtracks, m_nt_Trk_z0_Low);
      sc = nt2->addItem("Trk_phi0_Low",   m_nt_nLowtracks, m_nt_Trk_phi0_Low);
      sc = nt2->addItem("Trk_theta0_Low", m_nt_nLowtracks, m_nt_Trk_theta0_Low);
      sc = nt2->addItem("Trk_qoverp_Low", m_nt_nLowtracks, m_nt_Trk_qoverp_Low);
      sc = nt2->addItem("Trk_pt_Low",     m_nt_nLowtracks, m_nt_Trk_pt_Low);
      // ----------------------------------------------------------------------

      // ----------------------------------------------------------------------
      // Trk::Track hits...
      sc = nt2->addItem("Trk_nHits_Low",       m_nt_nLowtracks, m_nt_Trk_nHits_Low);
      sc = nt2->addItem("Trk_nhitsPixels_Low", m_nt_nLowtracks, m_nt_Trk_nhitspix_Low);
      sc = nt2->addItem("Trk_nhitsSCT_Low",    m_nt_nLowtracks, m_nt_Trk_nhitssct_Low);
      sc = nt2->addItem("Trk_nhitsTRT_Low",    m_nt_nLowtracks, m_nt_Trk_nhitstrt_Low);

      sc = nt2->addItem("Trk_nsharedPixels_Low", m_nt_nLowtracks, m_nt_Trk_nsharedPixels_Low);
      sc = nt2->addItem("Trk_nsharedSCT_Low",    m_nt_nLowtracks, m_nt_Trk_nsharedSCT_Low);
      sc = nt2->addItem("Trk_nshared_Low",       m_nt_nLowtracks, m_nt_Trk_nshared_Low);

      sc = nt2->addItem("Trk_nholesPixels_Low", m_nt_nLowtracks, m_nt_Trk_nholesPixels_Low);
      sc = nt2->addItem("Trk_nholesSCT_Low",    m_nt_nLowtracks, m_nt_Trk_nholesSCT_Low);
      sc = nt2->addItem("Trk_nholes_Low",       m_nt_nLowtracks, m_nt_Trk_nholes_Low);

      sc = nt2->addItem("Trk_chi2_Low",     m_nt_nLowtracks, m_nt_Trk_chi2_Low);
      sc = nt2->addItem("Trk_ndof_Low",     m_nt_nLowtracks, m_nt_Trk_ndof_Low);
      sc = nt2->addItem("Trk_chi2Prob_Low", m_nt_nLowtracks, m_nt_Trk_chi2Prob_Low);
      // ----------------------------------------------------------------------
      if (sc.isFailure()) msg(MSG::FATAL) << "Failed ntupleSvc()" << endmsg;
      else  msg(MSG::DEBUG) << "Ntuple " << nt2id << " has been booked successfully! " << endmsg;
      
    }
    else { if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Error booking ntuple" << endmsg; }
  }
}


//=====================================================================
// bookMatchingNtuple()
//===================================================================== 
void InDetAlignFillTrack::bookMatchingNtuple() {

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Booking Matching between Up and Low Trk::Track Collections..." << endmsg;
    
  std::string nt3id    = m_ntupleName + "/TrkTrack_Matching";
  std::string comments = "Matching between Up and Low Trk::Track Collections";
  
  NTuplePtr nt3(m_ntupleSvc, nt3id);
  if (nt3) {if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ntuple is already booked" << endmsg;}
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "Attempting to book general ntuple" << endmsg;
    m_ntupleSvc->book(nt3id,CLID_ColumnWiseTuple,comments);
    
    if (nt3) {
      StatusCode sc;
      // sc = nt3->addItem("event", m_nt_event); // event number
     
      sc = nt3->addItem("nTracks_Match",m_nt_matchingTrk, 0, maxTracks); // number of tracks

      // ----------------------------------------------------------------------
      // Matching for the usual Trk::Track parameters
      sc = nt3->addItem("Trk_delta_d0",       m_nt_matchingTrk, m_nt_Trk_delta_d0);
      sc = nt3->addItem("Trk_delta_phi0",      m_nt_matchingTrk, m_nt_Trk_delta_phi0);
      sc = nt3->addItem("Trk_delta_theta0",    m_nt_matchingTrk, m_nt_Trk_delta_theta0);
      sc = nt3->addItem("Trk_delta_eta",       m_nt_matchingTrk, m_nt_Trk_delta_eta);
      sc = nt3->addItem("Trk_delta_z0",        m_nt_matchingTrk, m_nt_Trk_delta_z0);
      sc = nt3->addItem("Trk_delta_qoverpt",   m_nt_matchingTrk, m_nt_Trk_delta_qoverpt);
      sc = nt3->addItem("Trk_delta_pt",        m_nt_matchingTrk, m_nt_Trk_delta_pt);
      sc = nt3->addItem("Trk_delta_charge",    m_nt_matchingTrk, m_nt_Trk_delta_charge);
      // ----------------------------------------------------------------------
      
      if (sc.isFailure()) msg(MSG::FATAL) << "Failed ntupleSvc()" << endmsg;
      else msg(MSG::DEBUG) << "Ntuple " << nt3id << " has been booked successfully! " << endmsg;
      
    }
    else { if (msgLvl(MSG::ERROR)) msg(MSG::DEBUG) << "Error booking ntuple" << endmsg; }
  }
  
  return;
}


//=====================================================================
//  InDetAlignFillTrack::dumpTrackCol()
//=====================================================================
int InDetAlignFillTrack::dumpTrackCol(const TrackCollection* tracks) {
  return dumpTrackCol(tracks,"");
}

//=====================================================================
//  InDetAlignFillTrack::dumpTrackCol()
//=====================================================================
int InDetAlignFillTrack::dumpTrackCol(const TrackCollection* tracks,
				      const std::string TrkColName) { 

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In dump" << TrkColName << "TrackCol()" << endmsg;
  
  int itrk = 0;
  
  TrackCollection::const_iterator trackItr  = tracks->begin();
  TrackCollection::const_iterator trackItrE = tracks->end();
  
  //looping over tracks
  for (; trackItr != trackItrE && itrk < maxTracks; ++trackItr) {
    
    if(*trackItr!=0)
      dumpTrack(itrk, (*trackItr), TrkColName);
    
    itrk++;
  }
  
  return itrk;
}


//=====================================================================
//  InDetAlignFillTrack::dumpTrack()
//=====================================================================
void InDetAlignFillTrack::dumpTrack(int itrk, const Trk::Track* trk,
				    std::string TrkColName) {
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In dump" << TrkColName << "Track()" << endmsg;

  const Trk::Perigee *aMeasPer =trk->perigeeParameters();
  
  if (aMeasPer==0){
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get Trk::MeasuredPerigee" << endmsg;}
  else {
    double d0 = aMeasPer->parameters()[Trk::d0];
    double z0 = aMeasPer->parameters()[Trk::z0];
    double phi0 = aMeasPer->parameters()[Trk::phi0];
    double theta = aMeasPer->parameters()[Trk::theta];
    double qOverP = aMeasPer->parameters()[Trk::qOverP];
    
    if (msgLvl(MSG::DEBUG)) {
    	msg(MSG::DEBUG) << itrk << ". " << TrkColName 
			<< " Track Parameters (d0, z0, phi0, theta, q/p)"  << endmsg;
	    msg(MSG::DEBUG) << " " << d0 << ", " << z0  << ", " 
			<< phi0 << ", " << theta  << ", " << qOverP << endmsg;
    }
    
    float transverseMomentum = sqrt((aMeasPer->momentum().x())*(aMeasPer->momentum().x()) 
            + (aMeasPer->momentum().y())*(aMeasPer->momentum().y()));
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  p = " << aMeasPer->momentum().mag()/CLHEP::GeV << " CLHEP::GeV/c, "
              << " pT = " << transverseMomentum/CLHEP::GeV << " CLHEP::GeV/c" << endmsg;

    // number of hits
    int nHits = (*trk).measurementsOnTrack()->size();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " - number of hits : " << nHits << endmsg;
    if (nHits!=0) {
      if (TrkColName=="Up") m_totalUphits += nHits;
      else if (TrkColName=="Low") m_totalLowhits += nHits;
      else m_totalhits += nHits;
    }
  
    // number of hits, shared hits and holes
    int nhitspix=0, nhitssct=0, nhitstrt=0;
    int nshared=0, nshpix=0, nshsct=0;
    int nholes=0, nhpix=0, nhsct=0;
    
    const Trk::TrackSummary* summary = m_trackSumTool->createSummary((*trk));
    if(summary==0)
      ATH_MSG_ERROR("Could not create TrackSummary");
    
    else {

        // hits
      nhitspix = summary->get(Trk::numberOfPixelHits);
      nhitssct = summary->get(Trk::numberOfSCTHits);
      nhitstrt = summary->get(Trk::numberOfTRTHits);

      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG)  << "   -- number of Pixel hits : " << nhitspix << endmsg;
        msg(MSG::DEBUG)  << "   -- number of SCT hits : " << nhitssct << endmsg;
        msg(MSG::DEBUG)  << "   -- number of TRT hits : " << nhitstrt << endmsg;
      }

      if (nhitspix!=0) {
        if (TrkColName=="Up") m_totalUpPixhits += nhitspix;
        else if (TrkColName=="Low") m_totalLowPixhits += nhitspix;
        else m_totalPixhits += nhitspix;
      }
      if (nhitssct!=0) {
        if (TrkColName=="Up") m_totalUpSCThits += nhitssct;
        else if (TrkColName=="Low") m_totalLowSCThits += nhitssct;
        else m_totalSCThits += nhitssct;
      }
      if (nhitstrt!=0) {
        if (TrkColName=="Up") m_totalUpTRThits += nhitstrt;
        else if (TrkColName=="Low") m_totalLowTRThits += nhitstrt;
        else m_totalTRThits += nhitstrt;
      }

      // shared hits
      nshpix = summary->get(Trk::numberOfPixelSharedHits);
      nshsct = summary->get(Trk::numberOfSCTSharedHits);
      nshared = nshpix + nshsct;

      if (nshpix<0) nshpix=0;
      if (nshsct<0) nshsct=0;
      if (nshared<0) nshared=0;

      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG)  << " - number of shared hits : " << nshared << endmsg;
        msg(MSG::DEBUG)  << "  -- number of Pixel shared hits : " << nshpix << endmsg;
        msg(MSG::DEBUG)  << "  -- number of SCT shared hits : " << nshsct << endmsg;
      }

      // holes
      nhpix = summary->get(Trk::numberOfPixelHoles);
      nhsct = summary->get(Trk::numberOfSCTHoles);
      nholes =  nhpix + nhsct;

      if (nhpix<0) nhpix=0;
      if (nhsct<0) nhsct=0;
      if (nholes<0) nholes=0;

      if (msgLvl(MSG::DEBUG)) {
        msg(MSG::DEBUG)  << " - number of Pixel holes : " << nhpix << endmsg;
        msg(MSG::DEBUG)  << "  -- number of SCT holes : " << nhsct << endmsg;
        msg(MSG::DEBUG)  << "  -- number of holes : " << nholes << endmsg;
      }

    }

    delete summary;
    
    // get fit quality and chi2 probability of track
    // chi2Prob = TMath::Prob(chi2,DoF) ROOT function
    double chi2Prob=0.;
    const Trk::FitQuality* fitQual = (*trk).fitQuality();
    if (fitQual==0) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "No fit quality assigned to the track" << endmsg; 
      chi2Prob = -1e12; // return big value
    }
    else {
      if (fitQual->chiSquared() > 0. && fitQual->numberDoF() > 0) {
        Genfun::CumulativeChiSquare probabilityFunction( fitQual->numberDoF() );
        chi2Prob = 1 - probabilityFunction( fitQual->chiSquared() );
  
        if (msgLvl(MSG::DEBUG)) {
          msg(MSG::DEBUG)  << "  - chi2             : " << fitQual->chiSquared() << endmsg;
          msg(MSG::DEBUG)  << "  - ndof             : " << fitQual->numberDoF() << endmsg;
          msg(MSG::DEBUG)  << "  - chi2 propability : " << chi2Prob << endmsg;
        }
	
    	}
    }  
    // Fill ntuple
    if (TrkColName=="Up") {
      m_nt_Trk_d0_Up[itrk] = d0;
      m_nt_Trk_z0_Up[itrk] = z0;
      m_nt_Trk_phi0_Up[itrk] = phi0;
      m_nt_Trk_theta0_Up[itrk] = theta;
      m_nt_Trk_qoverp_Up[itrk] = qOverP;
      m_nt_Trk_pt_Up[itrk] = transverseMomentum;
      
      m_nt_Trk_nHits_Up[itrk] = nHits;
      m_nt_Trk_nhitspix_Up[itrk] = nhitspix;
      m_nt_Trk_nhitssct_Up[itrk] = nhitssct;
      m_nt_Trk_nhitstrt_Up[itrk] = nhitstrt;
      
      m_nt_Trk_nsharedPixels_Up[itrk] = nshpix;
      m_nt_Trk_nsharedSCT_Up[itrk] = nshsct;
      m_nt_Trk_nshared_Up[itrk] = nshared;
      
      m_nt_Trk_nholesPixels_Up[itrk] = nhpix;
      m_nt_Trk_nholesSCT_Up[itrk] = nhsct;
      m_nt_Trk_nholes_Up[itrk] = nholes;
      
      m_nt_Trk_chi2_Up[itrk] = fitQual->chiSquared();
      m_nt_Trk_ndof_Up[itrk] = fitQual->numberDoF();
      m_nt_Trk_chi2Prob_Up[itrk] = chi2Prob;
    }
    else if (TrkColName=="Low") {
      m_nt_Trk_d0_Low[itrk] = d0;
      m_nt_Trk_z0_Low[itrk] = z0;
      m_nt_Trk_phi0_Low[itrk] = phi0;
      m_nt_Trk_theta0_Low[itrk] = theta;
      m_nt_Trk_qoverp_Low[itrk] = qOverP;
      m_nt_Trk_pt_Low[itrk] = transverseMomentum;
      
      m_nt_Trk_nHits_Low[itrk] = nHits;
      m_nt_Trk_nhitspix_Low[itrk] = nhitspix;
      m_nt_Trk_nhitssct_Low[itrk] = nhitssct;
      m_nt_Trk_nhitstrt_Low[itrk] = nhitstrt;
      
      m_nt_Trk_nsharedPixels_Low[itrk] = nshpix;
      m_nt_Trk_nsharedSCT_Low[itrk] = nshsct;
      m_nt_Trk_nshared_Low[itrk] = nshared;
      
      m_nt_Trk_nholesPixels_Low[itrk] = nhpix;
      m_nt_Trk_nholesSCT_Low[itrk] = nhsct;
      m_nt_Trk_nholes_Low[itrk] = nholes;
      
      m_nt_Trk_chi2_Low[itrk] = fitQual->chiSquared();
      m_nt_Trk_ndof_Low[itrk] = fitQual->numberDoF();
      m_nt_Trk_chi2Prob_Low[itrk] = chi2Prob;
    }
    else {
      m_nt_Trk_d0[itrk] = d0;
      m_nt_Trk_z0[itrk] = z0;
      m_nt_Trk_phi0[itrk] = phi0;
      m_nt_Trk_theta0[itrk] = theta;
      m_nt_Trk_qoverp[itrk] = qOverP;
      m_nt_Trk_pt[itrk] = transverseMomentum;
      
      m_nt_Trk_nHits[itrk] = nHits;
      m_nt_Trk_nhitspix[itrk] = nhitspix;
      m_nt_Trk_nhitssct[itrk] = nhitssct;
      m_nt_Trk_nhitstrt[itrk] = nhitstrt;
      
      m_nt_Trk_nsharedPixels[itrk] = nshpix;
      m_nt_Trk_nsharedSCT[itrk] = nshsct;
      m_nt_Trk_nshared[itrk] = nshared;
      
      m_nt_Trk_nholesPixels[itrk] = nhpix;
      m_nt_Trk_nholesSCT[itrk] = nhsct;
      m_nt_Trk_nholes[itrk] = nholes;
      
      m_nt_Trk_chi2[itrk] = fitQual->chiSquared();
      m_nt_Trk_ndof[itrk] = fitQual->numberDoF();
      m_nt_Trk_chi2Prob[itrk] = chi2Prob;
    }
  }
  
  return;
}


//=====================================================================
//  InDetAlignFillTrack::dumpPerigee()
//=====================================================================
void InDetAlignFillTrack::dumpPerigee(const Trk::TrackParameters* generatedTrackPerigee, 
				      int index) {

  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In dumpPerigee()" << endmsg;

  float d0 = generatedTrackPerigee->parameters()[Trk::d0];
  float z0 = generatedTrackPerigee->parameters()[Trk::z0];           
  float phi0 = generatedTrackPerigee->parameters()[Trk::phi0];
  float theta = generatedTrackPerigee->parameters()[Trk::theta];
  float eta = generatedTrackPerigee->eta();
  float charge = generatedTrackPerigee->charge();
  float qoverp = generatedTrackPerigee->parameters()[Trk::qOverP];
  float qoverpt = generatedTrackPerigee->parameters()[Trk::qOverP]/(sin(theta));
  float pt = (1/qoverpt)*(charge);
  // int pdg = genParticle->pdg_id();
  
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << " - Extrapolated genParticle perigee parameters " 
	  << "(d0, z0, phi0, theta, q/p)"  << endmsg;
    msg(MSG::DEBUG) << " " << d0 << ", " << z0  << ", " 
	  << phi0 << ", " << theta  << ", " << qoverp << endmsg;
  
    msg(MSG::DEBUG) << "  p = " << fabs(1/qoverp)/CLHEP::GeV << " CLHEP::GeV/c, "
	  << " pT = " << pt/CLHEP::GeV << " CLHEP::GeV/c" << endmsg;
  }

  m_nt_mc_Trk_d0[index] = d0;
  m_nt_mc_Trk_z0[index] = z0;
  m_nt_mc_Trk_phi0[index] = phi0;
  m_nt_mc_Trk_theta0[index] = theta;
  m_nt_mc_Trk_eta[index] = eta;
  m_nt_mc_Trk_qoverp[index] = qoverp;
  m_nt_mc_Trk_qoverpt[index] = qoverpt;
  m_nt_mc_Trk_pt[index] = pt;
  m_nt_mc_Trk_charge[index] = charge;
 
  return;  
}


//=====================================================================
//  InDetAlignFillTrack::dumpMatching()
//=====================================================================
StatusCode InDetAlignFillTrack::dumpMatching(const TrackCollection* tracksUpper,
					     const TrackCollection* tracksLower) {
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In dumpMatching()" << endmsg;
  
  int nTracksUpper = 0;

  // looping over the upper barrel tracks
  TrackCollection::const_iterator trackItrUpper  = tracksUpper->begin();
  TrackCollection::const_iterator trackItrUpperE = tracksUpper->end();
  for (; trackItrUpper != trackItrUpperE; ++trackItrUpper) { 
    
    const Trk::Track* trackUpper = *trackItrUpper;
    if(trackUpper == NULL) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No associated Trk::Track object found for track "<< nTracksUpper << endmsg;
      continue;
    }
    
    const Trk::Perigee* measUpperPer = trackUpper->perigeeParameters();
    
    // Get the track parameters from the Upper Track
    float d0Up      = measUpperPer->parameters()[Trk::d0];
    float phi0Up    = measUpperPer->parameters()[Trk::phi0];
    float eta0Up    = measUpperPer->eta();
    float z0Up      = measUpperPer->parameters()[Trk::z0];
    float thetaUp   = measUpperPer->parameters()[Trk::theta];  
    float qOverPtUp = measUpperPer->parameters()[Trk::qOverP]*1000/sin(thetaUp);
    float chargeUp  = measUpperPer->charge();
    float ptUp      = measUpperPer->pT()/1000.; 
    
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << nTracksUpper << ". UpTrack -> Track Parameters:"  << endmsg;
      msg(MSG::DEBUG) << "     d0, z0, phi0, theta, q/p" << d0Up << ", " << z0Up  << ", " 
		      << phi0Up << ", " << thetaUp  << ", " << qOverPtUp << endmsg;
    }
    
    int nTracksLower = 0;
    
    bool matchFound = false;
    float Matched_Low_d0      = -999;
    float Matched_Low_phi0    = -999;
    //**
    float Matched_Low_theta    = -999;
    float Matched_Low_eta0    = -999;
    float Matched_Low_z0      = -999;
    float Matched_Low_qOverPt = -999;
    float Matched_Low_charge  = -999;
    float Matched_Low_pt      = -999;
    
    // looping over the lower barrel tracks
    DataVector<Trk::Track>::const_iterator trackItrLower  = tracksLower->begin();
    DataVector<Trk::Track>::const_iterator trackItrLowerE = tracksLower->end();
    for (; trackItrLower != trackItrLowerE; ++trackItrLower) { //looping over Lower tracks
      
      const Trk::Track* trackLower = *trackItrLower;
      if(trackLower == NULL){
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No associated Trk::Track object found for track "<< nTracksLower << endmsg;
	continue;
      }
      
      const Trk::Perigee* measLowerPer = trackLower->perigeeParameters();
      
      float d0Low = measLowerPer->parameters()[Trk::d0];
      float phi0Low = measLowerPer->parameters()[Trk::phi0];
      float eta0Low = measLowerPer->eta();
      float z0Low = measLowerPer->parameters()[Trk::z0];
      float thetaLow = measLowerPer->parameters()[Trk::theta];  
      float qOverPtLow = measLowerPer->parameters()[Trk::qOverP]*1000/sin(thetaLow);
      float chargeLow = measLowerPer->charge();
      float ptLow = measLowerPer->pT()/1000.;

      //selecting Lower track that is closest to Upper in eta-phi
      float dphi2 = (phi0Up - phi0Low)*(phi0Up - phi0Low);
      
      //For TRT only tracks we will ignore the delta eta
      // and just require a delta phi match
      float deta2 = (eta0Up - eta0Low)*(eta0Up - eta0Low);

      // look for a matching track within a cone
      float dR = sqrt(dphi2 + deta2);
      ATH_MSG_DEBUG("dR (sqrt(dphi+deta2): " << dR); 
      ATH_MSG_DEBUG("minmdR (sqrt(dphi+deta2): " << m_mindR); 
      if(dR < m_mindR) {
        //	m_mindR = dR;
        Matched_Low_d0      = d0Low;
        Matched_Low_phi0    = phi0Low;
        //**
        Matched_Low_theta   = thetaLow;
        Matched_Low_eta0    = eta0Low;
        Matched_Low_z0      = z0Low;
        Matched_Low_qOverPt = qOverPtLow;
        Matched_Low_charge  = chargeLow;
        Matched_Low_pt      = ptLow;

        if(dR < m_matchedRcut) {

          if (msgLvl(MSG::DEBUG)) {
            msg(MSG::DEBUG) << nTracksLower << ". LowTrack -> Track Parameters:"  << endmsg;
            msg(MSG::DEBUG) << "     d0, z0, phi0, theta, q/p: " << d0Low << ", " << z0Low  << ", " 
            << phi0Low << ", " << thetaLow  << ", " << qOverPtLow << endmsg;
          }

          matchFound = true;
        }
      }
      else {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<  "No matching low track found!!"  << endmsg;
      }
      nTracksLower++;
      
    } //looping over lower tracks
    
    if(matchFound) {
      
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Match found!" <<endmsg;
      m_nt_matchingTrk = 1;
      m_nt_Trk_delta_d0[nTracksUpper] = d0Up - Matched_Low_d0;
      m_nt_Trk_delta_phi0[nTracksUpper] = phi0Up - Matched_Low_phi0;
      //**
      m_nt_Trk_delta_theta0[nTracksUpper] = thetaUp - Matched_Low_theta;
      m_nt_Trk_delta_eta[nTracksUpper] = eta0Up - Matched_Low_eta0;
      m_nt_Trk_delta_z0[nTracksUpper] = z0Up - Matched_Low_z0;
      m_nt_Trk_delta_qoverpt[nTracksUpper] = qOverPtUp - Matched_Low_qOverPt;
      m_nt_Trk_delta_pt[nTracksUpper] = ptUp - Matched_Low_pt;
      m_nt_Trk_delta_charge[nTracksUpper] = chargeUp - Matched_Low_charge;
      
    } // end match found
    
    
    nTracksUpper++;
    
  } //looping over upper tracks
  
  return StatusCode::SUCCESS;
}
