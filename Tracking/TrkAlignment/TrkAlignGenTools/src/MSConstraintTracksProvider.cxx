/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkFitterInterfaces/IGlobalTrackFitter.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkAlignGenTools/MSConstraintTracksProvider.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"  
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "muonEvent/Muon.h"
#include "muonEvent/MuonContainer.h"
#include "muonEvent/MuonParamDefs.h"
#include "MuonSegment/MuonSegment.h"
#include "Particle/TrackParticle.h"

#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"

#include "TTree.h"
#include "TFile.h"


namespace Trk {
  
  //________________________________________________________________________
  MSConstraintTracksProvider::MSConstraintTracksProvider(const std::string& type, 
               const std::string& name,
               const IInterface* parent)
    
    : AthAlgTool(type,name,parent)
    , m_trackFitter("Trk::GlobalChi2Fitter/InDetTrackFitter")
    , m_muonHitSummaryTool("Muon::MuonHitSummaryTool/MuonHitSummaryTool")
    , m_inputMuonCollection("MuidMuonCollection")
    , m_inputTracksCollection("Tracks")
    , m_nRetrievalErrors{}
    , m_nCBMuonsFromSG(0)
    , m_nCBMuonsHasEXandID(0)
    , m_nCBMuonsPassSelection(0)
    , m_nCBMuonsFailedRefit(0)
    , m_nCBMuonsSucRefit(0)
    , m_ntuple(nullptr)
    , m_tree(nullptr)
    , m_run{}
    , m_event{}
    , m_pID{}
    , m_pMS{}
    , m_ptID{}
    , m_ptMS{}
    , m_charge{}
    , m_combinedEta{}
    , m_IDEta{}
    , m_combinedPhi{}
    , m_IDPhi{}
    , m_pID_constrained{}
    , m_ptID_constrained{}
    , m_IDEta_constrained{}
    , m_IDPhi_constrained{}
    , m_charge_constrained{}
    , m_eBLhits{}
    , m_nBLhits{}
    , m_nPIXDS{}
    , m_nSCTDS{}
    , m_nPIXH{}
    , m_nSCTH{}
    , m_nPIXHits{}
    , m_nSCTHits{}
    , m_nTRTHits{}
    , m_sectors{}
    , m_phiLayers{}
    , m_stationLayers{}
    , m_sectorNum{}
    , m_phiLayerNum{}
    , m_stationLayerNum{}
  {

    declareInterface<ITrackCollectionProvider>(this);

    declareProperty("TrackFitter",              m_trackFitter                       );
    declareProperty("InputMuonCollection",      m_inputMuonCollection               );   
    declareProperty("InputTracksCollection",    m_inputTracksCollection             );
    declareProperty("RunOutlierRemoval",        m_runOutlierRemoval      = true     ); 
    declareProperty("MaxRetrievalErrors",       m_maxRetrievalErrors     = 10       );
    declareProperty("UseMSConstraintTrkOnly",   m_useMSConstraintTrkOnly = true     );
    declareProperty("DoTree",                   m_doTree                 = true     );
    declareProperty("MinPt",                    m_minPt                  = 15.0     );        
    declareProperty("MinPIXHits",               m_minPIXHits             = 1        );
    declareProperty("MinSCTHits",               m_minSCTHits             = 6        );
    declareProperty("MinTRTHits",               m_minTRTHits             = 0        );
    declareProperty("MaxIDd0",                  m_maxIDd0                = 500.     );
    declareProperty("MaxIDz0",                  m_maxIDz0                = 500.     ); 
    declareProperty("MinIDPt",                  m_minIDPt                = 10       );
    declareProperty("MDTHits",                  m_minMDTHits             = 15       );
    declareProperty("MinRPCPhiHits",            m_minRPCPhiHits          = 0        );
    declareProperty("MinTGCPhiHits",            m_minTGCPhiHits          = 0        );
    declareProperty("MaxMSd0",                  m_maxMSd0                = 500.     );
    declareProperty("MaxMSz0",                  m_maxMSz0                = 500.     );     
    declareProperty("MinMSPt",                  m_minMSPt                = 0        );
    declareProperty("MaxNumberOfSectors",       m_maxNumberOfSectors     = 1        );
    declareProperty("MinNumberOfPhiLayers",     m_minNumberOfPhiLayers   = 2        );
    declareProperty("MinStationLayers",         m_minStationLayers       = 3        );
 
  }

  //________________________________________________________________________
  MSConstraintTracksProvider::~MSConstraintTracksProvider()
  {

  }

  //________________________________________________________________________
  StatusCode MSConstraintTracksProvider::initialize()
  {
    // configure main track fitter
    ATH_CHECK(m_trackFitter.retrieve());
    ATH_MSG_DEBUG("Retrieved " << m_trackFitter);
    ATH_CHECK(m_muonHitSummaryTool.retrieve());
    // 
    bookNtuple();
    return StatusCode::SUCCESS;
  }




  void MSConstraintTracksProvider::setNtuple(TFile* ntuple) { 
    m_ntuple = ntuple; 
    bookNtuple();
  }
  



  bool MSConstraintTracksProvider::bookNtuple(){

    ATH_MSG_DEBUG("in  MSConstraintTracksProvider::bookNtuple");     
  
    if (m_doTree && !m_tree && m_ntuple) {
        m_ntuple->cd();
        m_tree = new TTree("MSMomentumConstraint","MSMomentumConstraint");

        ATH_MSG_DEBUG(" Start book Ntuple");

        m_tree->Branch("run",               &m_run,                 "run/I"     );
        m_tree->Branch("event",             &m_event,               "event/I"   );
        m_tree->Branch("pID",               &m_pID,                 "pID/D"     );
        m_tree->Branch("pMS",               &m_pMS,                 "pMS/D"     );
        m_tree->Branch("ptID",              &m_ptID,                "ptID/D"    );
        m_tree->Branch("ptMS",              &m_ptMS,                "ptMS/D"    );
        m_tree->Branch("charge",            &m_charge,              "charge/D"    );

        m_tree->Branch("combinedEta",       &m_combinedEta,         "combinedEta/D"   );
        m_tree->Branch("IDEta",             &m_IDEta,         "IDEta/D"   );
        m_tree->Branch("combinedPhi",       &m_combinedPhi,         "combinedPhi/D"   );
        m_tree->Branch("IDPhi",             &m_IDPhi,               "IDPhi/D"   );

        m_tree->Branch("pID_constrained",   &m_pID_constrained,     "pID_constrained/D" );
        m_tree->Branch("ptID_constrained",    &m_ptID_constrained,    "ptID_constrained/D"  );
        m_tree->Branch("IDEta_constrained",   &m_IDEta_constrained,   "IDEta_constrained/D"   );
        m_tree->Branch("IDPhi_constrained",   &m_IDPhi_constrained,   "IDPhi_constrained/D"   );
        m_tree->Branch("charge_constrained",    &m_charge_constrained,  "charge_constrained/D"  );

   
        m_tree->Branch("eBLhits",           &m_eBLhits,             "eBLhits/I"   );
        m_tree->Branch("nBLhits",           &m_nBLhits,             "nBLhits/I"   );

        m_tree->Branch("nPIXDS",            &m_nPIXDS,              "nPIXDS/I"    );
        m_tree->Branch("nSCTDS",            &m_nSCTDS,              "nSCTDS/I"    );

        m_tree->Branch("nPIXH",             &m_nPIXH,               "nPIXH/I"   );
        m_tree->Branch("nSCTH",             &m_nSCTH,               "nSCTH/I"   );

        m_tree->Branch("nPIXHits",            &m_nPIXHits,            "nPIXHits/I"    );
        m_tree->Branch("nSCTHits",            &m_nSCTHits,            "nSCTHits/I"    );
        m_tree->Branch("nTRTHits",            &m_nTRTHits,            "nTRTHits/I"    );

        m_tree->Branch("sectors",             &m_sectors,             "sectors/I"   );
        m_tree->Branch("phiLayers",           &m_phiLayers,           "phiLayers/I"   );
        m_tree->Branch("stationLayers",       &m_stationLayers,       "stationLayers/I" );

        m_tree->Branch("sectorNum",           &m_sectorNum,           "sectorNum/I"   );
        m_tree->Branch("phiLayerNum",           &m_phiLayerNum,         "phiLayerNum/I"   );
        m_tree->Branch("stationLayerNum",       &m_stationLayerNum,     "stationLayerNum/I" );

    }

    ATH_MSG_DEBUG("done with bookNtuple");
     
    return true;
  
}   



  void MSConstraintTracksProvider::initializeNtuple(){
  
    m_run      = -999;
    m_event      = -999; 
    m_pID      = -999.;
    m_pMS      = -999.;
    m_ptID     = -999.;
    m_ptMS     = -999.;
    m_charge     = -999.;

    m_combinedEta    = -999.;
    m_IDEta      = -999.;
    m_combinedPhi    = -999.;
    m_IDPhi      = -999.;             

    m_pID_constrained    = -999.;
    m_ptID_constrained   = -999.;
    m_IDEta_constrained  = -999.;
    m_IDPhi_constrained  = -999.;
    m_charge_constrained = -999.;


    m_eBLhits    = -999;
    m_nBLhits    = -999;
    m_nPIXDS     = -999,
    m_nSCTDS     = -999,

    m_nPIXH    = -999;
    m_nSCTH    = -999;


    m_nPIXHits     = -999;
    m_nSCTHits     = -999;
    m_nTRTHits     = -999;

    m_sectors    =  -999;
    m_phiLayers    = -999;
    m_stationLayers  = -999;

    m_sectorNum    = -999;
    m_phiLayerNum  = -999;
    m_stationLayerNum  = -999;

  }



  StatusCode MSConstraintTracksProvider::fillNtuple()
  {
    ATH_MSG_DEBUG("writing tree");
    int success=1;
    if (m_tree) {
        m_ntuple->cd();
       success = m_tree->Write();
    }
    return success>0 ? StatusCode::SUCCESS : StatusCode::FAILURE;
  }



  //________________________________________________________________________
  StatusCode MSConstraintTracksProvider::finalize()
  {
    return StatusCode::SUCCESS;
  }

 
  bool MSConstraintTracksProvider::combinedMuonSelection(const Analysis::Muon* it) {

       const Trk::Perigee* IDTrkMeasuredPerigee = dynamic_cast<const Trk::Perigee*>(& (it->inDetTrackParticle()->definingParameters()));
       if(!IDTrkMeasuredPerigee){
           ATH_MSG_DEBUG("NO inDetTrackParticle or no IDTrkMeasuredPerigee");
           return false;
       }

       const Trk::Perigee* METrkMeasuredPerigee = dynamic_cast<const Trk::Perigee*>(&(it->muonExtrapolatedTrackParticle()->definingParameters()));
       if(!METrkMeasuredPerigee){
           ATH_MSG_DEBUG("NO muonExtrapolatedTrackParticle or no METrkMeasuredPerigee");
           return false;
       }

       const double pt  = fabs(it->pt())/1000.;
       const double eta = fabs(it->eta()); 
       ATH_MSG_DEBUG(" the combined pt : "<< pt );  

       if (  pt   < m_minPt     ||
             eta  > 2.7         || 
            (eta > 0.8 && eta < 1.2) ){ 
             ATH_MSG_DEBUG(" this combinedMuon not pass basic pt and eta cuts --- pt: "<< pt << " eta: " << eta);
           return false;
       }

       const int nPixHits        = it->numberOfPixelHits();
       const int nSCTHits        = it->numberOfSCTHits();
       const int nTRTHits        = it->numberOfTRTHits();  

       //  Do you need fit quality cuts???
       // const Trk::FitQuality* idfq = idtrk->fitQuality(); 
       // const double chisqID        = idfq->chiSquared();
       // const int    ndofID         = idfq->numberDoF();
    

       const double idQoverPatIP = IDTrkMeasuredPerigee->parameters()[Trk::qOverP] * 1000.; 
       const double idz0atIP     = IDTrkMeasuredPerigee->parameters()[Trk::z0];
       const double idd0atIP     = IDTrkMeasuredPerigee->parameters()[Trk::d0];
       double ptIDatIP           = 0.;
       if ( idQoverPatIP != 0 ) ptIDatIP = fabs(1.0/idQoverPatIP)*sin(IDTrkMeasuredPerigee->parameters()[Trk::theta]);
  
       ATH_MSG_DEBUG( " ID pt  : "<< ptIDatIP );


       if( nPixHits < m_minPIXHits ||
           nSCTHits < m_minSCTHits ||
           nTRTHits < m_minTRTHits ||
           idd0atIP > m_maxIDd0    ||
           idz0atIP > m_maxIDz0    ||
           ptIDatIP < m_minIDPt     ) {          
         ATH_MSG_DEBUG("this combined muon not pass ID cuts --- nPixHits: " << nPixHits << " nSCTHits: " << nSCTHits << 
                         "nTRTHits: " << nTRTHits << " idd0atIP: " << idd0atIP << " idz0atIP: " << idz0atIP <<" ptIDatIP: " << ptIDatIP );
         return false;
       }



       const int nMDTHits        = it->numberOfMDTHits();
       const int nRPCPhiHits     = it->numberOfRPCPhiHits();
       const int nTGCPhiHits     = it->numberOfTGCPhiHits();    

       const double msQoverPatIP = METrkMeasuredPerigee->parameters()[Trk::qOverP] * 1000.; 
       const double msz0atIP     = METrkMeasuredPerigee->parameters()[Trk::z0];
       const double msd0atIP     = METrkMeasuredPerigee->parameters()[Trk::d0];
  
       double ptMSatIP = 0.;
       if ( msQoverPatIP != 0 ) ptMSatIP = fabs(1.0/msQoverPatIP)*sin(METrkMeasuredPerigee->parameters()[Trk::theta]);

       ATH_MSG_DEBUG( " ME pt  : "<< ptMSatIP);



       if( nMDTHits    < m_minMDTHits    ||
           nRPCPhiHits < m_minRPCPhiHits ||
           nTGCPhiHits < m_minTGCPhiHits ||
           msd0atIP    > m_maxMSd0       ||
           msz0atIP    > m_maxMSz0       ||
           ptMSatIP    < m_minMSPt        ){

           ATH_MSG_DEBUG("this combined muon not pass MS cuts --- nMDTHits: " << nMDTHits << " nRPCPhiHits: " << nRPCPhiHits << 
                         " nTGCPhiHits: " << nTGCPhiHits << " msd0atIP: " << msd0atIP << 
                         " msz0atIP: "    << msz0atIP    << " pMSatIP:"   << ptMSatIP);
           return false;
       }


       Muon::IMuonHitSummaryTool::CompactSummary summary;
       const Rec::TrackParticle* aTrackParticle = it->track();
    
       const Trk::Track* aTrack = 0;
       if (aTrackParticle) aTrack = aTrackParticle->originalTrack();

       if (aTrack && aTrack != it->inDetTrkTrack()) {
           summary = m_muonHitSummaryTool->summary(*aTrack);  
       } 
       else {
         ATH_MSG_WARNING("aTrack possible null !");
         std::vector<const Muon::MuonSegment*> segments;
         unsigned int nSeg = it->numberOfSegments();
         segments.reserve(nSeg); // avoid death by push back
         for( unsigned int si=0; si<nSeg; ++si ){
              const Muon::MuonSegment* seg = dynamic_cast<const Muon::MuonSegment*>(it->muonSegment(si));
              if( seg ) segments.push_back(seg);  
         }
         summary = m_muonHitSummaryTool->summary(segments);  
       }

       // it is possible to avoid the verbose summary construction in the above, 
       const int sectors       = summary.sectors.size();
       const int phiLayers     = summary.phiLayers.size();
       const int stationLayers = summary.stationLayers.size(); 


       if ( sectors       > m_maxNumberOfSectors   ||
            phiLayers     < m_minNumberOfPhiLayers ||
            stationLayers < m_minStationLayers      ){

            ATH_MSG_DEBUG("this combined muon not pass muonHitSummary cuts ---  sectors: "<< sectors << " phiLayers:  "<< phiLayers << " stationLayers: " << stationLayers);   

            return false;
       }

       m_pID       = it->inDetTrackParticle()->p();
       m_pMS       = it->muonExtrapolatedTrackParticle()->p();
       m_ptID      = it->inDetTrackParticle()->pt();
       m_ptMS      = it->muonExtrapolatedTrackParticle()->pt();
       m_charge    = it->inDetTrackParticle()->charge();

       m_combinedEta   = it->eta();
       m_IDEta     = it->inDetTrackParticle()->eta();
       m_combinedPhi   = it->phi();
       m_IDPhi     = it->inDetTrackParticle()->phi();

       m_nPIXHits        = it->numberOfPixelHits();
       m_nSCTHits        = it->numberOfSCTHits();
       m_nTRTHits        = it->numberOfTRTHits();

       m_sectors         = sectors;
       m_phiLayers       = phiLayers;
       m_stationLayers   = stationLayers;

       m_sectorNum       = *(summary.sectors.begin());
       //m_phiLayerNum     = (summary.phiLayers)[0];
       //m_stationLayerNum = (summary.stationLayers)[0];



    return true;
  }
 

  
StatusCode MSConstraintTracksProvider::trackCollection(const TrackCollection*& originalTracks){
  initializeNtuple();

  originalTracks = nullptr;

  const Analysis::MuonContainer* muonContainer = 0;
  if ( StatusCode::SUCCESS != evtStore()->retrieve( muonContainer , m_inputMuonCollection) ){
    ATH_MSG_WARNING(" Can't retrieve " << m_inputMuonCollection << " from the StoreGate ");
    ATH_MSG_WARNING("One probability is that you are not running on ESD/DESD ");

    // Can't do MS constraint refit, resort to retrieve tracks directly 
    if ( StatusCode::SUCCESS != evtStore()->retrieve(originalTracks, m_inputTracksCollection) ){
      originalTracks = nullptr;
      ATH_MSG_WARNING(" Can't retrieve " << m_inputTracksCollection << " from the StoreGate ");
    } else {
      if (originalTracks){ 
        ATH_MSG_DEBUG(" have tracks of this event: " << originalTracks->size());
      }
    }
    
    return StatusCode::SUCCESS;
  } else {

    TrackCollection* trackCollection = new TrackCollection;
    ATH_MSG_DEBUG("have combined Muons of the event:  " << muonContainer->size());

    Analysis::MuonContainer::const_iterator combinedTrackIt   = muonContainer->begin();
    Analysis::MuonContainer::const_iterator combinedTrackItE  = muonContainer->end();
    int goodQualityCombinedMuon = 0;

    for ( ; combinedTrackIt != combinedTrackItE; ++combinedTrackIt ) {
      ++m_nCBMuonsFromSG;
      const Analysis::Muon* muon = *combinedTrackIt;  
      if ( muon->isCombinedMuon() && muon->hasMuonExtrapolatedTrackParticle() && muon->hasInDetTrackParticle() ){
        ++m_nCBMuonsHasEXandID;
        if(combinedMuonSelection(muon)) {
          ATH_MSG_DEBUG("have good quality combined Muons of the event:  " << ++goodQualityCombinedMuon); 
          ++m_nCBMuonsPassSelection;
          const Trk::Perigee* METrkMeasuredPerigee = dynamic_cast<const Trk::Perigee*>(&(muon->muonExtrapolatedTrackParticle()->definingParameters()));
          if(!METrkMeasuredPerigee){
            ATH_MSG_DEBUG("NO muonExtrapolatedTrackParticle or no METrkMeasuredPerigee");
            continue;
          }
 
          const Trk::Surface* surf =  &(METrkMeasuredPerigee->associatedSurface()) ;
          if( !surf ) {
            ATH_MSG_DEBUG("NO surface of the METrkMeasuredPerigee");
            continue;
          }

          const Trk::Perigee* IDTrkMeasuredPerigee = dynamic_cast<const Trk::Perigee*>(&(muon->inDetTrackParticle()->definingParameters()));
          if(!IDTrkMeasuredPerigee){
            ATH_MSG_DEBUG("NO inDetTrackParticle or no IDTrkMeasuredPerigee");
            continue;
          } 

          Trk::DefinedParameter qOverPFromMS(METrkMeasuredPerigee->parameters()[Trk::qOverP], Trk::qOverP) ;

          std::vector<Trk::DefinedParameter> parFromMSVec;
          parFromMSVec.push_back( qOverPFromMS) ;

          Amg::MatrixX covFromMS( 1,1 ) ;
          covFromMS( 1, 1 )   = (*METrkMeasuredPerigee->covariance())( Trk::qOverP, Trk::qOverP ) ;

          Trk::PseudoMeasurementOnTrack *pmot = new Trk::PseudoMeasurementOnTrack( Trk::LocalParameters( parFromMSVec ), 
										   covFromMS, *surf) ;


          DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;
          trackStateOnSurfaces->reserve(muon->inDetTrackParticle()->originalTrack()->trackStateOnSurfaces()->size() + 1);
          DataVector<const Trk::TrackStateOnSurface>::const_iterator sb = muon->inDetTrackParticle()->originalTrack()->trackStateOnSurfaces()->begin();

          std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
          type.set(Trk::TrackStateOnSurface::Measurement);

          const Perigee* IDPerigeeParameters  = muon->inDetTrackParticle()->originalTrack()->perigeeParameters(); 
          const Perigee* IDPerigeeParametersClone(0);
          if (IDPerigeeParameters) IDPerigeeParametersClone  = IDPerigeeParameters->clone(); 


          // std::cout << "muon->inDetTrackParticle()->originalTrack()->perigeeParameters(): " << IDPerigeeParameters << std::endl;
          // std::cout << "muon->inDetTrackParticle()->originalTrack()->perigeeParameters()->clone(): "<< IDPerigeeParametersClone << std::endl;

          if(IDPerigeeParameters && IDPerigeeParametersClone ){
            trackStateOnSurfaces->push_back(new const Trk::TrackStateOnSurface(pmot, IDPerigeeParametersClone, 0, 0, type));

            for ( ; sb != muon->inDetTrackParticle()->originalTrack()->trackStateOnSurfaces()->end(); ++sb)  trackStateOnSurfaces->push_back((**sb).clone());

            Trk::Track* tmpTrack = new Trk::Track(muon->inDetTrackParticle()->originalTrack()->info(), trackStateOnSurfaces, 0);

            Trk::Track* MSConstraintFittedTrack = m_trackFitter->fit(*tmpTrack, m_runOutlierRemoval, Trk::muon);

            if(!MSConstraintFittedTrack){
              ++m_nCBMuonsFailedRefit;    
              ATH_MSG_WARNING("MSConstraintFittedTrack refit failed!");

              if(!m_useMSConstraintTrkOnly){  
                ATH_MSG_WARNING("Try to push the originalIDTrack into the trackCollection");
                Trk::Track* IDOriginalTrackClone = new Trk::Track(*(muon->inDetTrackParticle()->originalTrack()));
                if(IDOriginalTrackClone){
                  const Trk::Perigee * aMeasPerClone = IDOriginalTrackClone->perigeeParameters();
                  if( aMeasPerClone ){
                    ATH_MSG_DEBUG("IDOriginalTrackClone parameters --- pt: " << fabs(1./(aMeasPerClone->parameters()[Trk::qOverP]))*sin(aMeasPerClone->parameters()[Trk::theta]) << " d0: "<< aMeasPerClone->parameters()[Trk::d0] );   
                  }
                }     
                if(!IDOriginalTrackClone){
                  ATH_MSG_WARNING("Exception when IDOriginalTrackClone!");
                } else 
                  trackCollection->push_back(IDOriginalTrackClone);  
              }
            } else {
              ++m_nCBMuonsSucRefit;
              ATH_MSG_DEBUG("Got 1 successful MSConstraintFittedTrack ");

              Trk::Track* MSConstraintFittedTrackClone = new Trk::Track(*MSConstraintFittedTrack);
              const Trk::Perigee * MSConstraintFittedTrackMPClone = (MSConstraintFittedTrackClone->perigeeParameters());
              m_pID_constrained    = fabs(1./(MSConstraintFittedTrackMPClone->parameters()[Trk::qOverP]));
              m_IDEta_constrained  = -log(tan(MSConstraintFittedTrackMPClone->parameters()[Trk::theta]/2.));
              m_IDPhi_constrained  = MSConstraintFittedTrackMPClone->parameters()[Trk::phi];
              m_ptID_constrained   = fabs( m_pID_constrained*sin(MSConstraintFittedTrackMPClone->parameters()[Trk::theta]) );
              m_charge_constrained = MSConstraintFittedTrackMPClone->parameters()[Trk::qOverP]/fabs(MSConstraintFittedTrackMPClone->parameters()[Trk::qOverP]);

              delete MSConstraintFittedTrackClone;          
              // only fill the tracks used in the alignment 
              m_ntuple->cd();
              m_tree->Fill();

              trackCollection->push_back(MSConstraintFittedTrack);
            } 

          // clean up
          delete tmpTrack;
          } else{
            ATH_MSG_WARNING("failed in IDPerigeeParameters or IDPerigeeParametersClone  !");
            delete pmot;
            delete trackStateOnSurfaces;
          }

        }
      }
    }// end loop over tracks 

    if (StatusCode::SUCCESS != evtStore()->record(trackCollection, "MSMomentumConstraintTracks")){
    ATH_MSG_WARNING("problem with recording MSMomentumConstraintTracks to StoreGate!");
    return StatusCode::SUCCESS;
    }    

    ATH_MSG_DEBUG(" The final trackCollection size : " << trackCollection->size() );
    if ( not trackCollection->empty() ){
           originalTracks = trackCollection;
    }

  }
  return StatusCode::SUCCESS;
}

 
  void MSConstraintTracksProvider::printSummary(){

    if(m_logStream) {
  
      *m_logStream<<"*************************************************************"<<std::endl;
      *m_logStream<<"******        MSConstraintTracksProvider Summary       ******"<<std::endl;
      *m_logStream<<"*"<<std::endl;
      *m_logStream<<"* number of combined muons From SG :                                  " << m_nCBMuonsFromSG        << std::endl;
      *m_logStream<<"* number of combined muons have Extrapolated && InDet TrackParticle : " << m_nCBMuonsHasEXandID    << std::endl;
      *m_logStream<<"* number of combined muons pass selection:                            " << m_nCBMuonsPassSelection << std::endl;
      *m_logStream<<"* number of combined muons failed in refit:                           " << m_nCBMuonsFailedRefit   << std::endl;
      *m_logStream<<"* number of combined muons succeeded in refit:                        " << m_nCBMuonsSucRefit      << std::endl;

      *m_logStream<<"*"<<std::endl;
    }
  }


} // end namespace
