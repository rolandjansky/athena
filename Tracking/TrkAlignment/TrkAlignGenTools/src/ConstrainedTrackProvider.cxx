/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkFitterInterfaces/IGlobalTrackFitter.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkAlignGenTools/ConstrainedTrackProvider.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"  
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"


#include "TFile.h"
#include "TH2F.h"


namespace Trk {
  
  //________________________________________________________________________
  ConstrainedTrackProvider::ConstrainedTrackProvider(const std::string& type, 
               const std::string& name,
               const IInterface* parent)
    
    : AthAlgTool(type,name,parent)
    , m_trackFitter("Trk::GlobalChi2Fitter/InDetTrackFitter")
    , m_inputTrackCollection("Tracks")
    , m_constraintInputFile_P(0)
    , m_etaphiMap_P(0)
    , m_constraintInputFile_d0(0)
    , m_etaphiMap_d0(0)
    , m_constraintInputFile_z0(0)
    , m_etaphiMap_z0(0)
    , m_scalepmaptogev(false)
   {
    declareInterface<ITrackCollectionProvider>(this);
    declareProperty("TrackFitter",              m_trackFitter                      );
    declareProperty("InputTracksCollection",    m_inputTrackCollection             );
    declareProperty("RunOutlierRemoval",        m_runOutlierRemoval      = true    ); 
    declareProperty("MaxRetrievalErrors",       m_maxRetrievalErrors     = 10      );
    declareProperty("UseConstrainedTrkOnly",    m_useConstrainedTrkOnly  = true    );
    declareProperty("MinPt",                    m_minPt         = 15.0             );        
    declareProperty("MaxPt",                    m_maxPt         = 100.0            );        
    declareProperty("MinPIXHits",               m_minPIXHits    = 0                ); //1
    declareProperty("MinSCTHits",               m_minSCTHits    = 0                ); //6
    declareProperty("MinTRTHits",               m_minTRTHits    = 0                );
    declareProperty("Maxd0",                    m_maxd0         = 500.             );
    declareProperty("Maxz0",                    m_maxz0         = 500.             );
    declareProperty("CorrectMomentum",          m_CorrectMomentum         = true              );
    declareProperty("MomentumConstraintFileName", m_constraintFileName_P  = "Constraint.root" ); 
    declareProperty("MomentumConstraintHistName", m_constraintHistName_P  = "EtaPhiMap"       ); 
    declareProperty("CorrectD0",                  m_CorrectD0             = false             );
    declareProperty("d0ConstraintFileName",       m_constraintFileName_d0 = "Constraint.root" ); 
    declareProperty("d0ConstraintHistName",       m_constraintHistName_d0 = "EtaPhiMap"       ); 
    declareProperty("CorrectZ0",                  m_CorrectZ0             = false             );
    declareProperty("z0ConstraintFileName",       m_constraintFileName_z0 = "Constraint.root" ); 
    declareProperty("z0ConstraintHistName",       m_constraintHistName_z0 = "EtaPhiMap"       ); 
    declareProperty("UseConstraintError",         m_useConstraintError = true   ,"Bla bla "   ); 
    declareProperty("ReduceConstraintUncertainty",m_reduceConstraintUncertainty = 1., "Reduce the uncertainty on teh track parmater constraint by this amount"  ); 
    declareProperty("DeltaScaling",               m_deltaScaling = 1.);
    declareProperty("ScalePMapToGeV",             m_scalepmaptogev);
  }

  //________________________________________________________________________
  ConstrainedTrackProvider::~ConstrainedTrackProvider()
  {

  }

  //________________________________________________________________________
  StatusCode ConstrainedTrackProvider::initialize()
  {
    // configure main track fitter
    if(m_trackFitter.retrieve().isFailure()) {
       msg(MSG::FATAL) << "Could not get " << m_trackFitter << endreq;
       return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Retrieved " << m_trackFitter);


    ATH_MSG_INFO( "init m_useConstraintError " <<m_useConstraintError );

    ATH_MSG_INFO( "Init m_reduceConstraintUncertainty " << m_reduceConstraintUncertainty );


    if(m_CorrectMomentum){
 
      m_constraintInputFile_P = new TFile(m_constraintFileName_P.c_str() ,"read");
      if ( m_constraintInputFile_P->IsZombie() || !(m_constraintInputFile_P->IsOpen()) ) {
        ATH_MSG_FATAL( " Problem reading TFile " << m_constraintFileName_P );
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Opened  file containing the constraints" << m_constraintFileName_P);
      m_etaphiMap_P = (TH2F*)m_constraintInputFile_P->Get(m_constraintHistName_P.c_str());
      if(!m_etaphiMap_P){
        ATH_MSG_FATAL( " Problem getting constraints Hist.  Name " << m_constraintHistName_P );
        m_constraintInputFile_P->Close();
        delete m_constraintInputFile_P;
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Opened constraints histogram " << m_constraintHistName_P);
    
    }
    
    if(m_CorrectD0){
 
      m_constraintInputFile_d0 = new TFile(m_constraintFileName_d0.c_str() ,"read");
      if ( m_constraintInputFile_d0->IsZombie() || !(m_constraintInputFile_d0->IsOpen()) ) {
        ATH_MSG_FATAL( " Problem reading TFile " << m_constraintFileName_d0 );
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Opened  file containing the constraints" << m_constraintFileName_d0);
      m_etaphiMap_d0 = (TH2F*)m_constraintInputFile_d0->Get(m_constraintHistName_d0.c_str());
      if(!m_etaphiMap_d0){
        ATH_MSG_FATAL( " Problem getting constraints Hist.  Name " << m_constraintHistName_d0 );
        m_constraintInputFile_d0->Close();
        delete m_constraintInputFile_d0;
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Opened constraints histogram " << m_constraintHistName_d0);
    
    }
    
    if(m_CorrectZ0){
 
      m_constraintInputFile_z0 = new TFile(m_constraintFileName_z0.c_str() ,"read");
      if ( m_constraintInputFile_z0->IsZombie() || !(m_constraintInputFile_z0->IsOpen()) ) {
        ATH_MSG_FATAL( " Problem reading TFile " << m_constraintFileName_z0 );
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Opened  file containing the constraints" << m_constraintFileName_z0);
      m_etaphiMap_z0 = (TH2F*)m_constraintInputFile_z0->Get(m_constraintHistName_z0.c_str());
      if(!m_etaphiMap_z0){
        ATH_MSG_FATAL( " Problem getting constraints Hist.  Name " << m_constraintHistName_z0 );
        m_constraintInputFile_z0->Close();
        delete m_constraintInputFile_z0;
        return StatusCode::FAILURE;
      }
      ATH_MSG_INFO("Opened constraints histogram " << m_constraintHistName_z0);
    
    }
    
    
    if(m_reduceConstraintUncertainty<=0){
      ATH_MSG_FATAL( " reduceConstraintUncertainty  is  <= 0 it must be >0 . Currently  " << m_reduceConstraintUncertainty );
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }





  //________________________________________________________________________
  StatusCode ConstrainedTrackProvider::finalize()
  {
    if(m_constraintInputFile_P){
      m_constraintInputFile_P->Close();
      delete m_constraintInputFile_P;
    }
    
    if(m_constraintInputFile_d0){
      m_constraintInputFile_d0->Close();
      delete m_constraintInputFile_d0;
    }
    
    if(m_constraintInputFile_z0){
      m_constraintInputFile_z0->Close();
      delete m_constraintInputFile_z0;
    }
        
    return StatusCode::SUCCESS;
  }

 
  StatusCode ConstrainedTrackProvider::trackCollection(const TrackCollection*& finalTracks)
  {
   

    const TrackCollection* originalTracks = 0;
    
    if ( StatusCode::SUCCESS != evtStore()->retrieve(originalTracks, m_inputTrackCollection) ){
      ATH_MSG_WARNING(" Can't retrieve " << m_inputTrackCollection << " from the evtStore()->retrieve ");
      return StatusCode::FAILURE;
    } 
    ATH_MSG_DEBUG("#Available tracks in this event: " << originalTracks->size());
    
    // define the track collection of the selected tracks (check: m_useConstrainedTrkOnly)
    TrackCollection* trackCollection = new TrackCollection;
    
    // going to loop on the input collection of tracks
    TrackCollection::const_iterator trackIt   = originalTracks->begin();
    TrackCollection::const_iterator trackItE  = originalTracks->end();
    
    for ( ; trackIt != trackItE; ++trackIt ) {
      
      if( passTrackSelection( *trackIt ) ){
        const Trk::Perigee* measuredPerigee =  (*trackIt)->perigeeParameters();
        if(!measuredPerigee){
          ATH_MSG_DEBUG(" no measuredPerigee");
          continue;
        } 
        
        if(!(measuredPerigee->covariance()) ){
          ATH_MSG_DEBUG(" no measuredPerigee");
          continue;
        }      
  
        const Trk::Surface* surf = dynamic_cast<const Trk::Surface*>( &(measuredPerigee->associatedSurface()) ) ;
        if( !surf ) {
          ATH_MSG_DEBUG("NO surface of the measuredPerigee");
          continue;
        }

        // Get corrections from histograms if they are requested
  
        double correctedQoverP(0), correctedQoverPError(0);
        if(m_CorrectMomentum){
          getCorrectedValues_P( measuredPerigee, correctedQoverP, correctedQoverPError);
        } else {
          correctedQoverP = measuredPerigee->parameters()[Trk::qOverP];
          correctedQoverPError = 1e6;
        }

        double correctedD0(0), correctedD0Error(0);
        if(m_CorrectD0){
          getCorrectedValues_d0( measuredPerigee, correctedD0, correctedD0Error);
        } else {
          correctedD0 = measuredPerigee->parameters()[Trk::d0];
          correctedD0Error = 1e6;
        }
        
        double correctedZ0(0), correctedZ0Error(0);
        if(m_CorrectZ0){
          getCorrectedValues_z0( measuredPerigee, correctedZ0, correctedZ0Error);
        } else {
          correctedZ0 = measuredPerigee->parameters()[Trk::z0];
          correctedZ0Error = 1e6;
        }
        
        // Construct pseudomeasurement 
        Trk::DefinedParameter constrainedD0( correctedD0     , Trk::d0) ;
        Trk::DefinedParameter constrainedZ0( correctedZ0     , Trk::z0) ;
        Trk::DefinedParameter constrainedqOverP( correctedQoverP , Trk::qOverP) ;
  
        std::vector<Trk::DefinedParameter> constrainedPars;
        constrainedPars.push_back( constrainedD0 ) ;
        constrainedPars.push_back( constrainedZ0 ) ;
        constrainedPars.push_back( constrainedqOverP ) ;
        
        Amg::MatrixX constrainedCov( 3,3 ) ;
	constrainedCov.setZero();
        constrainedCov( 0, 0 )   =  correctedD0Error;
        constrainedCov( 1, 1 )   =  correctedZ0Error;
        constrainedCov( 2, 2 )   =  correctedQoverPError;
        // constrainedCov( 1, 0 )   =  0;  //initially we will assume the constraints are independant
        // constrainedCov( 2, 0 )   =  0;
        // constrainedCov( 2, 1 )   =  0;
  
        Trk::PseudoMeasurementOnTrack *pmot = new Trk::PseudoMeasurementOnTrack( Trk::LocalParameters( constrainedPars ), constrainedCov, *surf) ;
  
  
        // Add pseusdo measurement to a list of hits to be fit;
        std::vector<const Trk::MeasurementBase*> vecOfMB;
        if (pmot) vecOfMB.push_back(pmot);
        
        // Add the remaining hits;
        DataVector<const Trk::MeasurementBase>::const_iterator it    = (*trackIt)->measurementsOnTrack()->begin();
        DataVector<const Trk::MeasurementBase>::const_iterator itend = (*trackIt)->measurementsOnTrack()->end();
        for (;it!=itend;++it) 
          vecOfMB.push_back(*it);
        
        if(m_runOutlierRemoval){
          it    = (*trackIt)->outliersOnTrack()->begin();
          itend = (*trackIt)->outliersOnTrack()->end();
          // Add the remaining hits;
          for (;it!=itend;++it) 
            vecOfMB.push_back(*it);
        }
      
        Trk::Track* constrainedFittedTrack = m_trackFitter->fit( vecOfMB, *measuredPerigee, m_runOutlierRemoval, Trk::pion);
        delete pmot;
        
        if (constrainedFittedTrack){
          ATH_MSG_DEBUG("Fit was successful");
          const Trk::Perigee* constrainedPerigee =  constrainedFittedTrack->perigeeParameters();
          if(!constrainedPerigee){
            ATH_MSG_DEBUG(" no constrainedmeasuredPerigee");          
          } else {
            ATH_MSG_DEBUG("Initial track had momentum     : " << 1/measuredPerigee->parameters()[Trk::qOverP] * 1e-3 );          
            if (m_CorrectMomentum) {
	      ATH_MSG_DEBUG("corrected momentum with map is : " << 1/correctedQoverP * 1e-3 );          
	    }
	    else {
	      ATH_MSG_DEBUG("momentum not constrained       : ");          
	    }
            ATH_MSG_DEBUG("Constrained momentum fit result: " << 1/constrainedPerigee->parameters()[Trk::qOverP] * 1e-3 );          

	    // d0
            ATH_MSG_DEBUG("Initial track had d0           : " << measuredPerigee->parameters()[Trk::d0]);          
	    if(m_CorrectD0){
	      ATH_MSG_DEBUG("corrected d0 is                : " << correctedD0);          
	    }
	    else {
	      ATH_MSG_DEBUG("d0 is not corrected            : ");          
	    }
            ATH_MSG_DEBUG("Constrained d0 fit result      : " << constrainedPerigee->parameters()[Trk::d0]);          

	    // z0
            ATH_MSG_DEBUG("Initial track had z0           : " << measuredPerigee->parameters()[Trk::z0]);          
	    if(m_CorrectD0){
	      ATH_MSG_DEBUG("corrected z0 is                : " << correctedZ0);          
	    }
	    else {
	      ATH_MSG_DEBUG("z0 is not corrected            : ");          
	    }
            ATH_MSG_DEBUG("Constrained z0 fit result      : " << constrainedPerigee->parameters()[Trk::z0]);          
          }
  
          trackCollection->push_back(constrainedFittedTrack);
          ++m_constrainedTracks;
        } else{
          ATH_MSG_DEBUG("Constrained fit was unsuccessful");          
        }
      } else {
      
        if(m_useConstrainedTrkOnly) continue;
        
        //Prob could just copy the track?
        Trk::Track* unconstrainedFittedTrack = m_trackFitter->fit( **trackIt,m_runOutlierRemoval, Trk::pion);
        
        if(unconstrainedFittedTrack) {
          ATH_MSG_DEBUG("Unconstrained fit was successful");
          trackCollection->push_back(unconstrainedFittedTrack);
          ++m_unconstrainedTracks;
        } else {
          ATH_MSG_DEBUG("Unconstrained fit was unsuccessful");                  
        }
      }
    }      



    if (StatusCode::SUCCESS != evtStore()->record(trackCollection, "AlignmentConstrainedTracks")){
      ATH_MSG_WARNING("Problem with recording AlignmentConstrainedTracks to evtStore()->record() :( !");
      delete trackCollection;
      return StatusCode::SUCCESS;
    }    

    ATH_MSG_DEBUG(" The final trackCollection size: " << trackCollection->size() << " name of track collection: ..." );
    //track Collection cannot be null here; it has already been dereferenced
    finalTracks = trackCollection;

    return StatusCode::SUCCESS;

  }



  
  bool  ConstrainedTrackProvider::passTrackSelection(const Trk::Track *track)
  {
    
    const Trk::Perigee* perigee = track->perigeeParameters();
    if(!perigee){
      ATH_MSG_DEBUG("NO perigee on this track");
      return false;
    }
       
    const Trk::TrackSummary* summary = track->trackSummary();   
    
    if(!summary){
      ATH_MSG_DEBUG("No summary on this track");
      return false;
    }
    
    const int nPixHits        = summary->get(Trk::numberOfPixelHits);
    const int nSCTHits        = summary->get(Trk::numberOfSCTHits);
    const int nTRTHits        = summary->get(Trk::numberOfTRTHits);       
    
    const double qoverP = perigee->parameters()[Trk::qOverP] * 1000.; 
    const double z0     = perigee->parameters()[Trk::z0];
    const double d0     = perigee->parameters()[Trk::d0];
    double pt          = 0.;
    if ( qoverP != 0 ) pt = fabs(1.0/qoverP)*sin(perigee->parameters()[Trk::theta]);
    
    ATH_MSG_DEBUG( " pt  : "<< pt );
    
    if(pt < m_minPt ||  pt > m_maxPt   ){
      ATH_MSG_VERBOSE("Track fails pt cut [ "<<m_minPt<<", "<< m_maxPt << "] " << pt );      
      return false;
    }
    
    if( nPixHits < m_minPIXHits ||
        nSCTHits < m_minSCTHits ||
        nTRTHits < m_minTRTHits ||
        d0 > m_maxd0    ||
        z0 > m_maxz0    ) {          
      ATH_MSG_VERBOSE("This track did not pass cuts --- nPixHits: " << nPixHits << " nSCTHits: " << nSCTHits << 
                   "nTRTHits: " << nTRTHits << " idd0atIP: " << d0 << " idz0atIP: " << z0  );
     return false;
    }

    return true;
  }
  
  void  ConstrainedTrackProvider::getCorrectedValues_P(const Trk::Perigee* measuredPerigee, double& correctedQoverP,double& correctedQoverPError)
  {
  
    // scale q/p not p
  
    double charge(0);
    if( measuredPerigee->parameters()[Trk::qOverP] > 0)
      charge = 1.;
    else 
      charge = -1.;
    
    double pt   = fabs(1./(measuredPerigee->parameters()[Trk::qOverP]))* sin(measuredPerigee->parameters()[Trk::theta]) *1e-3;
    double eta = -log(tan(measuredPerigee->parameters()[Trk::theta]/2.));
    double phi = measuredPerigee->parameters()[Trk::phi];
    double perr =  (*measuredPerigee->covariance())( Trk::qOverP, Trk::qOverP );
   
   
    int binNumber = m_etaphiMap_P->FindBin(eta, phi);
    double constraintErr = m_etaphiMap_P->GetBinError(binNumber);

    double delta = m_etaphiMap_P->GetBinContent(binNumber) * m_deltaScaling;
       
    if (m_scalepmaptogev) {
      ATH_MSG_DEBUG("Scale sagita bias map to GeV. deltaSagitta raw value " << delta << " ==> used value: " << delta * 0.001 << " GeV-1");
      delta = delta * 0.001;
    }

    correctedQoverP = measuredPerigee->parameters()[Trk::qOverP] * (1.+ charge *pt *delta );
    correctedQoverPError = perr;
   
    if(m_useConstraintError){
      double qoverpFracError =  perr/pow(measuredPerigee->parameters()[Trk::qOverP], 2);
      qoverpFracError += pow(constraintErr,2);    
      correctedQoverPError = qoverpFracError*correctedQoverP*correctedQoverP;
    }
    
    correctedQoverPError = correctedQoverPError * pow( m_reduceConstraintUncertainty,-2); 
    return;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void  ConstrainedTrackProvider::getCorrectedValues_d0(const Trk::Perigee* measuredPerigee, double& correctedD0,double& correctedD0Error)
  {
  
    // correcting d0
    double d0  = measuredPerigee->parameters()[Trk::d0];
    double eta = -log(tan(measuredPerigee->parameters()[Trk::theta]/2.));
    double phi = measuredPerigee->parameters()[Trk::phi]; 
    double d0err =  (*measuredPerigee->covariance())( Trk::d0, Trk::d0 );
   
   
    int binNumber = m_etaphiMap_d0->FindBin(eta, phi);
    double constraintErr = m_etaphiMap_d0->GetBinError(binNumber);
    double delta = m_etaphiMap_d0->GetBinContent(binNumber) * m_deltaScaling;
    ATH_MSG_DEBUG("d0 correction for (eta,phi) cell: ("<<eta <<", " <<phi <<") delta_d0 =  " << delta << " mm ");
       
    correctedD0 = d0 + delta;
    correctedD0Error = d0err;
    
    if(m_useConstraintError){
      correctedD0Error = d0err + pow( constraintErr, 2 );
    }
   
    correctedD0Error = correctedD0Error * pow( m_reduceConstraintUncertainty,-2); 
    return;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void  ConstrainedTrackProvider::getCorrectedValues_z0(const Trk::Perigee* measuredPerigee, double& correctedZ0,double& correctedZ0Error)
  {
  
    // correcting z0
    double z0  = measuredPerigee->parameters()[Trk::z0];
    double eta = -log(tan(measuredPerigee->parameters()[Trk::theta]/2.));
    double phi = measuredPerigee->parameters()[Trk::phi];
    double z0err =  (*measuredPerigee->covariance())( Trk::z0, Trk::z0 );
   
    int binNumber = m_etaphiMap_z0->FindBin(eta, phi);
    double constraintErr = m_etaphiMap_z0->GetBinError(binNumber);
    double delta = m_etaphiMap_z0->GetBinContent(binNumber) * m_deltaScaling;

    correctedZ0 = z0 + delta;
    correctedZ0Error = z0err;
    ATH_MSG_DEBUG("z0 correction for (eta,phi) cell: ("<<eta <<", " <<phi <<") delta_z0 =  " << delta << " mm.  New z0: " << correctedZ0);

    if(m_useConstraintError){
      correctedZ0Error = z0err + pow( constraintErr, 2 );
    }
   
    correctedZ0Error = correctedZ0Error * pow( m_reduceConstraintUncertainty,-2); 
    return;
  }




  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  void ConstrainedTrackProvider::printSummary(){

    if(m_logStream) {
  
      *m_logStream<<"*************************************************************"<<std::endl;
      *m_logStream<<"******        ConstrainedTrackProvider Summary       ******"<<std::endl;
      *m_logStream<<"*"<<std::endl;
      *m_logStream<<"* number of combined muons failed in refit:                           " << m_passconstrainedRequirements   << std::endl;
      *m_logStream<<"* number of combined muons failed in refit:                           " << m_constrainedTracks   << std::endl;
      *m_logStream<<"* number of combined muons succeeded in refit:                        " << m_unconstrainedTracks      << std::endl;

      *m_logStream<<"*"<<std::endl;
    }
  }






} // end namespace
