// emacs: this is -*- c++ -*-

#ifndef __TRIGTRACKSELECTOR_H
#define __TRIGTRACKSELECTOR_H

/// L2 tracks
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
///TruthMap
#include "TrigInDetTruthEvent/TrigInDetTrackTruthMap.h"

/// offline and EF
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

/// MC truth
#include "McParticleEvent/TruthParticleContainer.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

///// stuff *merely* to get the particle charge!!!
/// #include "HepPDT/ParticleData.hh"
/// #include "HepMC/ParticleDataTable.h"
/// #include "McParticleEvent/TruthParticle.h"
/// #include "McParticleUtils/McUtils.h" // for chargeFromPdgId
///
///// FrameWork includes
/// #include "GaudiKernel/ServiceHandle.h"
/// #include "GaudiKernel/IPartPropSvc.h"
/// absolutely pathetic!! write my own class

#include "TrigInDetAnalysisUtils/particleType.h"

/// TrigInDetAnalysis classes
#include "TrigInDetAnalysis/TrackSelector.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackFilter.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkParameters/Perigee.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/Track.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/InDetTrackSummary.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "GaudiKernel/ToolHandle.h" 
#include "TrkParameters/Perigee.h"
#include "TMath.h"




// namspace TrigInDetAnalysis {

// class TrigTrackSelector : public TIDA::TrackSelector<TIDA::Track> { 

class TrigTrackSelector : public TrackSelector { 

public:

  //  TrigTrackSelector( bool (*selector)(const TIDA::Track*)=NULL ) : TrackSelector(selector) {  } 
  TrigTrackSelector( TrackFilter* selector ) : TrackSelector(selector), m_id(0) {  } 

  virtual void clear() { for ( unsigned i=mtracks.size() ; i-- ; ) delete mtracks[i]; mtracks.clear(); }   


  void selectTrack( const TrigInDetTrack* track, const TrigInDetTrackTruthMap* truthMap=0 ) {     
    // do the track extraction stuff here....
    if ( track ) { 

	double eta    = track->param()->eta();
	double phi    = track->param()->phi0();
	double z0     = track->param()->z0(); 
	double pT     = track->param()->pT(); 
	double d0     = track->param()->a0();

	double deta    = track->param()->eeta();
	double dphi    = track->param()->ephi0();
	double dz0     = track->param()->ez0(); 
	double dpT     = track->param()->epT(); 
	double dd0     = track->param()->ea0();

	int   algoid  = track->algorithmId(); 	      

	int nBlayerHits = (track->HitPattern() & 0x1);
	int nPixelHits  = 2 * track->NPixelSpacePoints();  // NB: for comparison with offline 
	int nSctHits    = 2 * track->NSCT_SpacePoints();   //     a spacepoint is 2 "hits"
	int nStrawHits  =  track->NStrawHits();
	int nTrHits     =  track->NTRHits();

	int nSiHits     = nPixelHits + nSctHits;

	bool expectBL   = false;                           //not filled in 

	unsigned long id = (unsigned long)track;

	unsigned hitPattern = track->HitPattern();
	unsigned multiPattern = 0;

	double chi2    = track->chi2();

	bool truth = false;
	int match_barcode = -1;
	
	if ( truthMap ) { 
	  const TrigInDetTrackTruth* trackTruth = truthMap->truth(track);
	  if (trackTruth!=0 && trackTruth->nrMatches() > 0) {
	    match_barcode = trackTruth->bestSiMatch()->barcode();
	    truth = true;
	  }
	}
	
	
	TIDA::Track* t = new TIDA::Track(  eta,  phi,  z0,  d0,  pT, chi2, 
								     deta, dphi, dz0, dd0, dpT, 
								     nBlayerHits, nPixelHits, nSctHits, nSiHits, 
								     nStrawHits, nTrHits, 
								     hitPattern, multiPattern, 
								     algoid, truth, -1, match_barcode,
								     expectBL, id) ; 
	
	//	std::cout << "SUTT ID track " << *t << "\t0x" << std::hex << track->HitPattern() << std::dec << std::endl;
	
	if ( !addTrack( t ) ) delete t;
    }
  }


  // extract all the tracks from a TrigInDetTrack collection and associated TruthMap and convert them
  void selectTracks( const TrigInDetTrackCollection* trigtracks, const TrigInDetTrackTruthMap* truthMap=0 ) {     
    // do the track extraction stuff here....
    TrigInDetTrackCollection::const_iterator  trackitr = trigtracks->begin();
    TrigInDetTrackCollection::const_iterator  trackend = trigtracks->end();
    while ( trackitr!=trackend ) { 
      selectTrack( *trackitr, truthMap );
      trackitr++;
    }
  }


  // add a TrackParticle 
  void selectTrack( const Rec::TrackParticle* track ) { 
        
    // do the track extraction stuff here....

    static int hpmap[20] = { 0, 1, 2,  7, 8, 9,  3, 4, 5, 6, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

    if ( track ) { 
	    
      const Trk::MeasuredPerigee* measPer = track->measuredPerigee();

      CLHEP::HepVector perigeeParams = measPer->parameters();
      
      double pT    = measPer->pT(); 
      double eta   = measPer->eta();
      double phi   = perigeeParams[Trk::phi0];
      double z0    = perigeeParams[Trk::z0];
      double d0    = perigeeParams[Trk::d0];

      double theta = perigeeParams[Trk::theta];
      double  p    = 1/perigeeParams[Trk::qOverP];
  
      // AAARCH!!!!! the TrackParticle pT is NOT SIGNED!!!! ( I ask you! ) 
      if ( perigeeParams[Trk::qOverP]<0 && pT>0 ) pT *= -1;

      const Trk::ErrorMatrix err = measPer->localErrorMatrix();

      double dtheta = err.error(Trk::theta);
      double dqovp  = err.error(Trk::qOverP); 
      double covthetaOvP  = err.covValue(Trk::qOverP,Trk::theta); 


      double deta = 0.5*dtheta/(std::cos(0.5*theta)*std::cos(0.5*theta)*std::tan(0.5*theta));
      double dphi = err.error(Trk::phi0);
      double dz0  = err.error(Trk::z0);
      double dd0  = err.error(Trk::d0);
      double dpT  = 0; 


      double sintheta = std::sin(theta);
      double costheta = std::cos(theta);
      double dpt2 = (p*p*sintheta)*(p*p*sintheta)*dqovp*dqovp + (p*costheta)*(p*costheta)*dtheta*dtheta - 2*(p*p*sintheta)*(p*costheta)*covthetaOvP;

      if ( dpt2>0 ) dpT = std::sqrt( dpt2 );




      // Check number of hits
      // NB: a spacepoint is two offline "hits", so a pixel spacepoint is really 
      // 2 "hits" and an offline SCT "hit" is really a 1D cluster, so two intersetcting
      // stereo clusters making a spacepoint are two "hits"
      const Trk::TrackSummary *summary = track->trackSummary();
      int nBlayerHits = 2*summary->get(Trk::numberOfBLayerHits); 
      int nPixelHits  = 2*summary->get(Trk::numberOfPixelHits);  
      int nSctHits    = summary->get(Trk::numberOfSCTHits); 
      int nStrawHits  = summary->get(Trk::numberOfTRTHits);
      int nTrHits     = summary->get(Trk::numberOfTRTHighThresholdHits);

      int nSiHits     = nPixelHits + nSctHits;
      bool expectBL   = summary->get(Trk:: expectBLayerHit);

      const Trk::FitQuality *quality   = track->fitQuality();
      double chi2 = quality->chiSquared();

      unsigned bitmap = 0;
      
      unsigned long id = (unsigned long)track;

      for ( int ih=0 ; ih<20 ; ih++ ) {
	if ( summary->isHit(Trk::DetectorType(ih)) ) bitmap |= ( 1<<hpmap[ih] ); 	
      }

      /// now some *ridiculous* code to get the author of the 
      /// TrackParticle (offline) tracks
      
      //      std::cout << "fetching author info :" << track->info().trackFitter() << ":" 
      //		<< track->info().dumpInfo() << ": bm 0x" << std::hex << bitmap << std::dec << std::endl;
      
      int fitter = track->info().trackFitter();
      std::string dumpinfo = track->info().dumpInfo();
      
      int trackAuthor = -1; 
      if ( fitter>0 && fitter<Trk::TrackInfo::NumberOfTrackFitters ) {
	if      ( dumpinfo.find("TRTStandalone")!=std::string::npos)        trackAuthor = 2;
	else if ( dumpinfo.find("TRTSeededTrackFinder")!=std::string::npos) trackAuthor = 1;
	else                                                                trackAuthor = 0;
      }
      
#if 0
      std::cout << "\t\t\tSUTT TP track" 
		<< "\teta=" << eta  // << " +- " << (*trackitr)->params()->deta()
		<< "\tphi=" << phi  // << " +- " << (*trackitr)->params()->dphi()
		<< "\tz0="  << z0 
		<< "\tpT="  << pT // << "\t( " << 1/qoverp << ")"
		<< "\td0="  << d0
		<< "\tNsi=" << nSiHits 
		<< "\tNtrt=" << nTrHits 
		<< "\tNstr=" << nStrawHits
		<< "\tauthor=" << trackAuthor
		<< std::endl;
#endif	

      // Create and save Track
      
      TIDA::Track* t = new TIDA::Track(eta, phi, z0, d0, pT, chi2,
								 deta, dphi, dz0, dd0, dpT,
								 nBlayerHits, nPixelHits, nSctHits, nSiHits,
								 nStrawHits, nTrHits, bitmap, 0,
								 trackAuthor,
								 expectBL, id) ;  

      //      std::cout << "SUTT TP track " << *t << "\t0x" << std::hex << bitmap << std::dec << std::endl; 
      
      if ( !addTrack( t ) ) delete t;
      
    }
  }
  

  // extract all the tracks from a TrackParticle collection and add them
  void selectTracks( const Rec::TrackParticleContainer* trigtracks ) { 
    
    //    std::cout << "\t\t\tSUTT \tTrackParticleContainer->size() = " << trigtracks->size() << std::endl;
    
    Rec::TrackParticleContainer::const_iterator  trackitr = trigtracks->begin();
    Rec::TrackParticleContainer::const_iterator  trackend = trigtracks->end();
   
    while ( trackitr!=trackend ) { 
    
      selectTrack( *trackitr );
      
      trackitr++;

    } // loop over tracks
    
  }



  // extract all the tracks from a TrackParticle collection and add them
  void selectTracks( const TruthParticleContainer* truthtracks ) { 
    
    //    std::cout << "\t\t\tSUTT \tTrackParticleContainer->size() = " << trigtracks->size() << std::endl;
    
    TruthParticleContainer::const_iterator  trackitr = truthtracks->begin();
    TruthParticleContainer::const_iterator  trackend = truthtracks->end();
   
    while ( trackitr!=trackend ) { 
    
      selectTrack( *trackitr );
      
      trackitr++;

    } // loop over tracks
    
  }



  // add a TruthParticle from a GenParticle - easy, bet it doesn't work 
  void selectTrack( const HepMC::GenParticle* track ) {

    /// not a "final state" particle
    if ( track->status() != 1 ) return;

    /// set this so can use it as the identifier - don't forget to reset!!
    m_id = (unsigned long)track; 
    selectTrack( TruthParticle(track) );
    m_id = 0;
  }


  // add a TruthParticle 
  void selectTrack( const TruthParticle& track ) { selectTrack( &track ); }


  // add a TruthParticle 
  void selectTrack( const TruthParticle* track ) { 
    TIDA::Track* t = makeTrack( track, m_id );
     if ( t && !addTrack(t) ) delete t;
  }


  // make a TIDA::Track from a GenParticle 
  static TIDA::Track* makeTrack( const HepMC::GenParticle* track ) { 
    unsigned long id = (unsigned long)track;
    TruthParticle t = TruthParticle(track); 
    return  makeTrack( &t, id );
  }

  // make a TIDA::Track from a TruthParticle 
  static TIDA::Track* makeTrack( const TruthParticle* track, unsigned long tid=0 ) { 

    if ( track==0 ) return 0; 


    //bool proto = true; /// proto final state - Removed 24/06/13

    /// NB: Could replace this with a simple tests on production
    ///     and decay vertices - ie define some cylindrical plane,  
    ///     and produced within this plane, and either doesn't 
    ///     decay, or decays outside this plane then take as stable
    ///     gets round duplicated, or secondary tracks, brem electrons
    ///     etc. (actually done this now - see code later) 
    /// only want final state, charged particles - this is a quick hack
    //if ( track->nDecay()>0 ) proto = false; /// what if decay is inside the calorimeter? - Removed 24/06/13
    /// charge information ios not filled in TruthParticle unless build 
    /// with the TruthParticleCnvTool ! I ask you!!!
    /// if ( !track->hasCharge() || track->charge()==0 ) return;
    //smh Hack to get rid of internal MC ids 
    //smh This one can probably stay: 83 to 100 are reserved for MC
    //smh Only stable entries from generator
    //if ( abs(track->pdgId()) > 1000000000) proto = false; /// does this work? - Removed 24/06/13
    //if ( track->pdgId() >= 83 && track->pdgId() <= 100 ) proto = false; /// shouldn't be needed - Removed 24/06/13 
    //if ( track->status() != 1 ) proto = false; - Removed 24/06/13

    double phi = track->phi();
    double eta = track->eta();

    ////  ABSOLUTELY STUPID!!! to get the production vertex, you need to navigate to 
    ////  the genparticle, the production vertex, etc, at each stage they could be a 
    ////  null pointer, so you would have to check all of them to be robust 
    double xp[3] = { 0, 0, 0 };

    if ( track->genParticle()->production_vertex() ) { 
      // std::cout << "ptr->production_vertex() " << ptr->production_vertex() << std::endl;
      xp[0] = track->genParticle()->production_vertex()->point3d().x();
      xp[1] = track->genParticle()->production_vertex()->point3d().y();
      xp[2] = track->genParticle()->production_vertex()->point3d().z();
    }


    // CHANGED BY JK - z0 with respect to (0,0)
    //    double z0 = xp[2];
    double theta = 2*std::atan( exp( (-1)*eta ) );
    double z0 = xp[2] - (xp[0]*std::cos(phi) + xp[1]*std::sin(phi))/std::tan(theta);
    
    double xd[3] = { 0, 0, 0 };

    if ( track->genParticle()->end_vertex() ) { 
      // std::cout << "ptr->production_vertex() " << ptr->production_vertex() << std::endl;
      xd[0] = track->genParticle()->end_vertex()->point3d().x();
      xd[1] = track->genParticle()->end_vertex()->point3d().y();
      xd[2] = track->genParticle()->end_vertex()->point3d().z();
    }

    double rp = std::sqrt( xp[0]*xp[0] + xp[1]*xp[1] ); 
    double rd = std::sqrt( xd[0]*xd[0] + xd[1]*xd[1] ); 

    
    bool final_state = false; 

    /// the is our new "final state" requirement
    /// the inner and outer radii are in some sense 
    /// arbitrary - these correspond to an envelope
    /// around the pixel detector, so the track must 
    /// pass through the entire pixel detector 
    /// NB: In order to ensure you don't miss any 
    ///     tracks they really need to be the same
    ///     ie if a track brems in your "volume"
    ///     then you will miss that track, and
    ///     also the resulting track, even if it is
    ///     a high et track  
    const double inner_radius = 47;
    //    const double outer_radius = 47;
    const double outer_radius = 47;
    if ( ( track->genParticle()->production_vertex() && rp<=inner_radius ) && 
	 ( track->genParticle()->end_vertex()==0 || rd>outer_radius ) ) final_state = true; 
      
    if ( !final_state ) return 0; /// keep anything over 10 GeV with the old requirement
    
    //// AAAARGHHH!!!! For some *stupid* reason, when converting from a 
    //// GenParticle to a TruthParticle, the charge is not filled (or in
    //// fact it *is* filled, but with a default value of -999) and so 
    //// we would need to access the PDT in order to find it out
    //// *surely* this lookup should be done in the TruthParticle 
    //// constructor, by a helper class, and not be dependent on the 
    //// subsequent invocation of some external helper class at some 
    //// later stage? I mean, who leaves it up to some external class
    //// to provide values inside a class based on information already 
    //// in the class? It's perverse! 

    double q = track->charge();
    
    static particleType ptype;

    /// avoid default (unset) TruthParticle charge
    if ( q==-999 ) q = ptype.charge( track->pdgId() );

    ///  only use charged tracks
    if ( q==0 ) return 0;
    
    double pT  = q*track->pt();

    //// BP this is slightly confusing, since d0 =? a0?,
    //// ToolHandle<Trk::ITruthToTrack> truthToTrack;
    //// const HepMC::GenParticle* particle = track->genParticle();
    //// const HepMC::GenParticle* gp = track->genParticle();
    //// boost::scoped_ptr<const Trk::TrackParameters> generatedTrackPerigee( truthToTrack->makePerigeeParameters(particle) );
    //// const Trk::TrackParameters* generatedTrackPerigee(0);
    //// cout<<"BP BP here "<<endl;
    //// if ( gp->production_vertex() ){
    ////   cout<<"BP BP gp->production_vertex() "<<endl;
    ////   generatedTrackPerigee = truthToTrack->makePerigeeParameters( gp );
    ////   if (generatedTrackPerigee == NULL && gp->barcode() > 1000000 ) {
    ////      std::cout<<"BP No perigee available for interacting truth particle."<<std::endl;
    ////   }
    //// }

    //// generatedTrackPerigee = truthToTrack->makePerigeeParameters( gp );
    //// cout<<"BP BP "<<generatedTrackPerigee->parameters()[Trk::d0]<<endl;
    //// double d0 = generatedTrackPerigee ? generatedTrackPerigee->parameters()[Trk::d0] : 999.;

    double d0  = 0;

    /// what a faff - refuse to mess about with the classes to swim tracks etc - why can't 
    /// they just encode this sort of information in the class!! It's not as if it doesn't 
    /// actually have members for anything else useless!! Classes should be designed for 
    /// ease of use !!!!

    /// is there a sign issue here ?


    // CHANGED BY JK - d0 with respect to (0,0)
    //d0 = q*rp*std::sin(phi);
    d0  =  xp[1]*std::cos(phi) -  xp[0]*std::sin(phi);


    //// AAAARGHHH!!!! For some *stupid* reason, when converting from a 
 
    /// static HepPDT::ParticleDataTable* m_pdt = new ParticleDataTable();
    ///
    /// // Get the Particle Properties Service
    /// if ( m_pdt==0 ) {  
    ///   ServiceHandle<IPartPropSvc> partPropSvc("PartPropSvc", "TrigTestMonToolAC"); // , name());
    ///   if ( !partPropSvc.retrieve().isSuccess() ) {
    ///      m_pdt = partPropSvc->PDT();
    ///   }
    ///   else { 
    ///      std::cerr << " Could not initialize Particle Properties Service" << std::endl;
    ///      return; // StatusCode::FAILURE;
    ///   }
    /// }    

    /// this abomination didn't work
    ///  if ( m_pdt ) { 
    ///      q = McUtils::chargeFromPdgId( track->pdgId(), m_pdt );
    ///      double q   = track->charge();
    ///  }

    /// how about storing barcode/status/pidg info?
    int author  = track->pdgId();   /// this isn't good!! but it will do for testing 
    int barcode = track->barcode(); /// probably won't work either


    unsigned long id = (unsigned long)track;
    if ( tid!=0 ) id = tid;

    /// get the production vertex for the z0 and d0 but should we store the z0 of 
    /// the production vertex? or swim the track to the perigee point with respect 
    /// to 0,0 and use the parameters there? 

    /// const HepMC::GenParticle* gp = track->genParticle();
    /// const HepMC::GenVertex*   gv = gp->production_vertex();

    ////    double x0 = gv->point3d().x();
    ////    double y0 = gv->point3d().x();
    ////    double z0 = gv->point3d().z();

    /// what to do with these???
		
    //    std::cout << "\t\t\tSUTT Truth track" 
    //	            << "\teta=" << eta  // << " +- " << (*trackitr)->params()->deta()
    //	            << "\tphi=" << phi  // << " +- " << (*trackitr)->params()->dphi()
    //	            << "\tz0="  << z0 
    //              << "\tpT="  << pT // << "\t( " << 1/qoverp << ")"
    //              << "\td0="  << d0
    //              << "\tauthor=" << author
    //              << std::endl;
    


    TIDA::Track* t = new TIDA::Track(eta, phi, z0, d0, pT, 0,
							       0, 0, 0, 0, 0,
							       0, 0, 0, 0,
							       0, 0, 0, 0,
							       author, false, barcode, -1,
							       false, 
							       id ) ;  

    return t;

 }



  // add a Trk::Track
  void selectTrack( const Trk::Track* track ) { 
        
    // do the track extraction stuff here....

    static int hpmap[20] = { 0, 1, 2,  7, 8, 9,  3, 4, 5, 6, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
    //    std::cout << __FILE__<< " " <<__LINE__<<std::endl;

    if ( track ) { 
	    
      // const Trk::Perigee* startPerigee = track->perigeeParameters();
      // const Trk::MeasuredPerigee* measPer = dynamic_cast<const Trk::MeasuredPerigee*>(track->perigeeParameters());
      const Trk::MeasuredPerigee* startPerigee = dynamic_cast<const Trk::MeasuredPerigee*>(track->perigeeParameters());
      //      CLHEP::HepVector perigeeParams = measPer->parameters();      
      //      double pT    = measPer->pT(); 
      //      double eta   = measPer->eta();
      //      double phi   = perigeeParams[Trk::phi0];
      //      double z0    = perigeeParams[Trk::z0];
      //      double d0    = perigeeParams[Trk::d0];
      //      // AAARCH!!!!! the TrackParticle pT is NOT SIGNED!!!! ( I ask you! ) 
      //      if ( perigeeParams[Trk::qOverP]<0 ) pT *= -1;
      //      std::cout <<pT1<<" pt1vspT "<<pT<<std::endl;
      
      if (startPerigee){
      

	double theta   = startPerigee->parameters()[Trk::theta];  
	double p       = 1/startPerigee->parameters()[Trk::qOverP];
	double qOverPt = startPerigee->parameters()[Trk::qOverP]/std::sin(theta);
	double charge  = startPerigee->charge();
	double eta     = startPerigee->eta();
	double phi     = startPerigee->parameters()[Trk::phi0];
	double z0      = startPerigee->parameters()[Trk::z0];
	double d0      = startPerigee->parameters()[Trk::d0];
	//	double pT = (1./qOverPt)*(charge);	 
	double pT      = (1./qOverPt); // always use signed PT
	
	if ( charge<0 && pT>0 ) pT *= -1;
	if ( charge<0 && p>0 )  p  *= -1;

	const Trk::ErrorMatrix err = startPerigee->localErrorMatrix();

	double dtheta = err.error(Trk::theta);
	double dqovp  = err.error(Trk::qOverP); 
	double covthetaOvP  = err.covValue(Trk::qOverP,Trk::theta); 
	
	
	double deta = 0.5*dtheta/(std::cos(0.5*theta)*std::cos(0.5*theta)*std::tan(0.5*theta));
	double dphi = err.error(Trk::phi0);
	double dz0  = err.error(Trk::z0);
	double dd0  = err.error(Trk::d0);
	double dpT  = 0; 
	
	
	double sintheta = std::sin(theta);
	double costheta = std::cos(theta);
	double dpT2 = (p*p*sintheta)*(p*p*sintheta)*dqovp*dqovp + (p*costheta)*(p*costheta)*dtheta*dtheta - 2*(p*p*sintheta)*(p*costheta)*covthetaOvP;
	
	if ( dpT2>0 ) dpT = std::sqrt( dpT2 );

	// Check number of hits
	// NB: a spacepoint is two offline "hits", so a pixel spacepoint is really 
	// 2 "hits" and an offline SCT "hit" is really a 1D cluster, so two intersetcting
	// stereo clusters making a spacepoint are two "hits"
	// const Trk::TrackSummary *summary = dynamic_cast<const Trk::TrackSummary*>(track->trackSummary());
	//ToolHandle< Trk::ITrackSummaryTool > m_trackSumTool;
	//m_trackSumTool  = ToolHandle<Trk::ITrackSummaryTool>("Trk::TrackSummaryTool/InDetTrackSummaryTool");
	//const Trk::TrackSummary* summary = NULL;       
	//summary = m_trackSumTool->createSummary(*track); 

	const Trk::TrackSummary * summary = track->trackSummary();
	int nBlayerHits = 0;
	int nPixelHits  = 0;
	int nSctHits    = 0;
	int nStrawHits  = 0;
	int nTrHits     = 0;
	int nSiHits     = 0;
	bool expectBL = false;
	unsigned bitmap = 0;

	if(summary==0){
            std::cout << "Could not create TrackSummary  - Track will likely fail hits requirements" << std::endl;
	}    
	else{      
            nBlayerHits = 2*summary->get(Trk::numberOfBLayerHits); 
            nPixelHits  = 2*summary->get(Trk::numberOfPixelHits);  
	    nSctHits    = summary->get(Trk::numberOfSCTHits); 
            nStrawHits  = summary->get(Trk::numberOfTRTHits);
            nTrHits     = summary->get(Trk::numberOfTRTHighThresholdHits);
	    nSiHits     = nPixelHits + nSctHits;
	    expectBL    = summary->get(Trk::expectBLayerHit);
	    for ( int ih=0 ; ih<20 ; ih++ ) {
	      if ( summary->isHit(Trk::DetectorType(ih)) ) bitmap |= ( 1<<hpmap[ih] ); 	
	    }
	}

	unsigned long id = (unsigned long)track;
	double chi2 =  0.;
	//const Trk::FitQuality *quality   = dynamic_cast<const Trk::FitQuality*>(track->fitQuality());
	const Trk::FitQuality *quality   = (track->fitQuality());
	if(quality==0) std::cout << "Could not create FitQuality  - Track will likely fail hits requirements" << std::endl;
	else{
            chi2 = quality->chiSquared();          
	}

	int trackAuthor = -1;

	/// now some *ridiculous* code to get the author of the 
	/// TrackParticle (offline) tracks

	//      std::cout << "fetching author info :" << track->info().trackFitter() << ":" 
	//		<< track->info().dumpInfo() << ": bm 0x" << std::hex << bitmap << std::dec << std::endl;

	int fitter = track->info().trackFitter();
	//      std::string dumpinfo = track->info().dumpInfo();

	if ( fitter>0 && fitter<Trk::TrackInfo::NumberOfTrackFitters ) {
	  if	 ((track->info().dumpInfo()).find("TRTStandalone")        != std::string::npos) trackAuthor = 2;
	  else if	((track->info().dumpInfo()).find("TRTSeededTrackFinder") != std::string::npos) trackAuthor = 1;
	  else                                                                                    trackAuthor = 0;
	}

  #if 0
	std::cout << "\t\t\tSUTT TP track" 
		  << "\teta=" << eta  // << " +- " << (*trackitr)->params()->deta()
		  << "\tphi=" << phi  // << " +- " << (*trackitr)->params()->dphi()
		  << "\tz0="  << z0 
		  << "\tpT="  << pT // << "\t( " << 1/qoverp << ")"
		  << "\td0="  << d0
		  << "\tNsi=" << nSiHits 
		  << "\tNtrt=" << nTrHits 
		  << "\tNstr=" << nStrawHits
		  << "\tauthor=" << trackAuthor
		  << std::endl;
  #endif	
	// Create and save Track      
	TIDA::Track* t = new TIDA::Track(eta, phi, z0, d0, pT, chi2,
								   deta, dphi, dz0, dd0, dpT,
								   nBlayerHits, nPixelHits, nSctHits, nSiHits,
								   nStrawHits, nTrHits, bitmap, 0,
								   trackAuthor,
								   expectBL, id) ;  

	 if ( !addTrack( t ) ) delete t;

	 //std::cout << "SUTT TP track " << *t << "\t0x" << std::hex << bitmap << std::dec << std::endl; 
      }
    }
  }
  
  // extract all the tracks from a TrackCollection and add them
  void selectTracks( const TrackCollection* trigtracks ) { 
    
    //    std::cout << "\t\t\tSUTT \tTrackContainer->size() = " << trigtracks->size() << std::endl;
    
    TrackCollection::const_iterator  trackitr = trigtracks->begin();
    TrackCollection::const_iterator  trackend = trigtracks->end();
   
    while ( trackitr!=trackend ) { 
      selectTrack( *trackitr );
      trackitr++;
    } // loop over tracks
    
  }


private:

  unsigned long m_id;

};


// }


#endif // __TRIGTRACKSELECTOR_H
