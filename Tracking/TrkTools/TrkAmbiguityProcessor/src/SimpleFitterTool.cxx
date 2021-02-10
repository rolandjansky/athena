/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "SimpleFitterTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkTrackSummary/TrackSummary.h"

#include <map>
#include <ext/functional>
#include <iterator>
#include "TString.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"


bool Trk::SimpleFitterTool::_checkTrack( const Trk::Track *track) const {
  if (!track )return true;
  bool error=false;
  if (track->trackParameters()){
    int counter=0;
    for (const  Trk::TrackParameters *param: *(track->trackParameters())) {
      if (param && param->covariance() && param->covariance()->determinant() < 0) {
        ATH_MSG_DEBUG( " negative determinant for track param " << counter << " "
        << *(param)  << " cov=" << *(param->covariance())
        << std::endl
        << " det=" << param->covariance()->determinant() );
        error=true;
      }
      ++counter;
      if (counter>=2) break;
    }
  }
  return !error;
}


//==================================================================================================
Trk::SimpleFitterTool::SimpleFitterTool(const std::string& t, 
                                const std::string& n,
                                const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_particleHypothesis(undefined),
  m_trkSummaryTool("Trk::TrackSummaryTool"),
  m_assoTool("Trk::PRD_AssociationTool/PRD_AssociationTool"),
  m_finalTracks(0),
  m_Nevents(0),
  m_Ncandidates(5), m_Naccepted(5), m_NacceptedBrem(5),
  m_NbremFits(5),m_Nfits(5),m_NrecoveryBremFits(5),m_NgoodFits(5),m_NfailedFits(5),
  m_rejectInvalidTracks(false)
{
  // statitics stuff
  m_etabounds.push_back(0.8);
  m_etabounds.push_back(1.6);
  m_etabounds.push_back(2.5);
  m_etabounds.push_back(4.0);

  m_fitterTool.push_back("Trk::KalmanFitter/InDetTrackFitter");

  declareInterface<ITrackAmbiguityProcessorTool>(this);
  declareProperty("SummaryTool"          , m_trkSummaryTool);
  declareProperty("AssociationTool"      , m_assoTool);
  declareProperty("Fitter"               , m_fitterTool );
  declareProperty("MatEffects"           , m_matEffects         = 3); // pion
  declareProperty("tryBremFit"           , m_tryBremFit         = false);
  declareProperty("caloSeededBrem"       , m_caloSeededBrem     = false);
  declareProperty("pTminBrem"            , m_pTminBrem          = 1000.);
  declareProperty("etaBounds"            , m_etabounds,"eta intervals for internal monitoring");
  declareProperty("RejectTracksWithInvalidCov"     ,m_rejectInvalidTracks   );
  
}
//==================================================================================================

Trk::SimpleFitterTool::~SimpleFitterTool()
{
}
//==================================================================================================

StatusCode Trk::SimpleFitterTool::initialize()
{
  ATH_CHECK( m_trkSummaryTool.retrieve() );
  ATH_CHECK( m_assoTool.retrieve() );
  ATH_CHECK( m_fitterTool.retrieve() );
  
  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_particleHypothesis = particleSwitch.particle[m_matEffects];

  // brem fitting enabled ?
  if (m_tryBremFit)
    ATH_MSG_DEBUG( "Try brem fit and recovery for electron like tracks." );

  // statistics
  for (int i=0; i<5; i++) {
    m_Ncandidates[i]      = 0;
    m_Nfits[i]            = 0;
    m_NrecoveryBremFits[i]= 0;
    m_NbremFits[i]        = 0;
    m_NgoodFits[i]        = 0;
    m_NfailedFits[i]      = 0;
    m_Naccepted[i]        = 0;
    m_NacceptedBrem[i]    = 0;
  }

  return StatusCode::SUCCESS;
}
//==================================================================================================

StatusCode Trk::SimpleFitterTool::finalize()
{
  ATH_MSG_DEBUG (name() << "::finalize() -- statistics:");
  StatusCode sc = AlgTool::finalize();
  return sc;
}

void Trk::SimpleFitterTool::statistics()
{
  ATH_MSG_DEBUG (name() << " -- statistics:");
  std::streamsize ss = std::cout.precision();
  if (msgLvl(MSG::INFO)) {
    int iw=9;
    std::cout << "------------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of events processed      :   "<< m_Nevents << std::endl;
    std::cout << "  statistics by eta range          ------All---Barrel---Trans.-- Endcap-- Forwrd-- " << std::endl;
    std::cout << "------------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of candidates at input   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ncandidates[Trk::SimpleFitterTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ncandidates[Trk::SimpleFitterTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ncandidates[Trk::SimpleFitterTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ncandidates[Trk::SimpleFitterTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)        
              << m_Ncandidates[Trk::SimpleFitterTool::iForwrd] << std::endl;
    std::cout << "------------------------------------------------------------------------------------" << std::endl;
    std::cout << "  number of normal fits           :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::SimpleFitterTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::SimpleFitterTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::SimpleFitterTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::SimpleFitterTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::SimpleFitterTool::iForwrd] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + 2nd brem fit for failed fit   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[Trk::SimpleFitterTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[Trk::SimpleFitterTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[Trk::SimpleFitterTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[Trk::SimpleFitterTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)        
                << m_NrecoveryBremFits[Trk::SimpleFitterTool::iForwrd] << std::endl;
      std::cout << "  normal brem fits for electrons  :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[Trk::SimpleFitterTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[Trk::SimpleFitterTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[Trk::SimpleFitterTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[Trk::SimpleFitterTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)        
                << m_NbremFits[Trk::SimpleFitterTool::iForwrd] << std::endl;
    }
    std::cout << "------------------------------------------------------------------------------------" << std::endl;
    std::cout << "  sum of succesful fits           :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NgoodFits[Trk::SimpleFitterTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NgoodFits[Trk::SimpleFitterTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NgoodFits[Trk::SimpleFitterTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NgoodFits[Trk::SimpleFitterTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)        
              << m_NgoodFits[Trk::SimpleFitterTool::iForwrd] << std::endl;
    std::cout << "  sum of failed fits              :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NfailedFits[Trk::SimpleFitterTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NfailedFits[Trk::SimpleFitterTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NfailedFits[Trk::SimpleFitterTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NfailedFits[Trk::SimpleFitterTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)        
              << m_NfailedFits[Trk::SimpleFitterTool::iForwrd] << std::endl;
    std::cout << "------------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of tracks accepted       :" << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Naccepted[Trk::SimpleFitterTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Naccepted[Trk::SimpleFitterTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Naccepted[Trk::SimpleFitterTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Naccepted[Trk::SimpleFitterTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)        
              << m_Naccepted[Trk::SimpleFitterTool::iForwrd] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  including number of brem fits   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NacceptedBrem[Trk::SimpleFitterTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NacceptedBrem[Trk::SimpleFitterTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NacceptedBrem[Trk::SimpleFitterTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NacceptedBrem[Trk::SimpleFitterTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)        
                << m_NacceptedBrem[Trk::SimpleFitterTool::iForwrd] << std::endl;
    }
    std::cout << "------------------------------------------------------------------------------------" << std::endl;
    std::cout << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
              << "    definition: ( 0.0 < Barrel < " << m_etabounds[iBarrel-1] << " < Transition < " << m_etabounds[iTransi-1]
              << " < Endcap < " << m_etabounds[iEndcap-1] << " < Forward < " << m_etabounds[iForwrd-1] << " )" << std::endl;
    std::cout << "------------------------------------------------------------------------------------" << std::endl;
  }
  std::cout.precision (ss);
  return;
}

//==================================================================================================

/** helper function for statistics */

void Trk::SimpleFitterTool::increment_by_eta(std::vector<int>& Ntracks, const Track* track, bool updateAll) {

  if (updateAll) Ntracks[Trk::SimpleFitterTool::iAll] += 1;

  // test
  if (!track)
  {
     ATH_MSG_ERROR ("track pointer zero, should not happen!");
     return;
  }

  // use first parameter 
  if (!track->trackParameters())
  {
    ATH_MSG_WARNING ("No track parameters, needed for statistics code in Trk::SimpleFitterTool!");
  }
  else
  {
    double eta = track->trackParameters()->front()->eta();
    if (fabs(eta) < m_etabounds[0]) ++Ntracks[Trk::SimpleFitterTool::iBarrel];
    else if (fabs(eta) < m_etabounds[1]) ++Ntracks[Trk::SimpleFitterTool::iTransi];
    else if (fabs(eta) < m_etabounds[2]) ++Ntracks[Trk::SimpleFitterTool::iEndcap];
    else if (fabs(eta) < m_etabounds[3]) ++Ntracks[Trk::SimpleFitterTool::iForwrd];
  }
}

//==================================================================================================

/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/

TrackCollection*  Trk::SimpleFitterTool::process(const TrackCollection* tracks)
{
  using namespace std;
  
  ATH_MSG_DEBUG("Starting processing...");
  
  // clear prdAssociationTool
  m_assoTool->reset();
  
  //final copy - ownership is passed out of algorithm
  m_finalTracks = new TrackCollection;
  
  ATH_MSG_DEBUG("Number of tracks at Input: "<<tracks->size());
 
  TrackCollection::const_iterator trackIt    = tracks->begin();
  TrackCollection::const_iterator trackItEnd = tracks->end();
  
  for ( ; trackIt != trackItEnd ; ++trackIt)
  {
    // statistics
    increment_by_eta(m_Ncandidates,*trackIt);
    
    ATH_MSG_DEBUG ("Processing track candidate "<<*trackIt);
    const Trk::Track* fittedTrack = refitPrds (*trackIt);
    
    if (fittedTrack) {
      ATH_MSG_DEBUG ("New track successfully fitted "<<fittedTrack);
      // statistics
      increment_by_eta(m_Naccepted,fittedTrack);
      if (m_tryBremFit && fittedTrack->info().trackProperties(Trk::TrackInfo::BremFit))
        increment_by_eta(m_NacceptedBrem,fittedTrack);
      StatusCode sc = m_assoTool->addPRDs(*fittedTrack);
      if (sc.isFailure()) ATH_MSG_ERROR( "addPRDs() failed" );
      
      Trk::TrackInfo newInfo;
      newInfo.setPatternRecognitionInfo(Trk::TrackInfo::FastTrackingFitterTool);
      fittedTrack->info().addPatternReco(newInfo);
      
      // filling the final collection of tracks with the fitted tracks
      // and fill the track summary (this fills as well outliers evaluated while fitting)
      m_finalTracks->push_back( const_cast<Track*>(fittedTrack));
      m_trkSummaryTool->updateTrack(*m_finalTracks->back());      
      
    }
    else {
      ATH_MSG_DEBUG ("Fit failed...");
      delete fittedTrack;
    }    
  }
  ATH_MSG_DEBUG("Ending processing...");
  return m_finalTracks;
  
}

void Trk::SimpleFitterTool::reset()
{
  m_assoTool->reset();

  return;
}

//==================================================================================================

const Trk::Track* Trk::SimpleFitterTool::refitPrds( const Trk::Track* track)
{

  // get vector of PRDs
  std::vector<const Trk::PrepRawData*> prds = m_assoTool->getPrdsOnTrack(*track);

  if ( 0==prds.size() ) {
    ATH_MSG_WARNING( "No PRDs on track");
    return 0;
  }
     
  ATH_MSG_DEBUG ("Track "<<track<<"\t has "<<prds.size()<<"\t PRDs");

  const TrackParameters* par = track->perigeeParameters();
  if (par==0) {
    ATH_MSG_DEBUG ("Track ("<<track<<") has no perigee! Try any other ?");
    par = track->trackParameters()->front();
    if (par==0) {
      ATH_MSG_DEBUG ("Track ("<<track<<") has no Track Parameters ! No refit !");
      return 0;
    }
  }

  // refit using first parameter, do outliers
  const Trk::Track* newTrack = 0;

  if (m_tryBremFit && track->info().trackProperties(Trk::TrackInfo::BremFit))
  {
    // statistics
    increment_by_eta(m_NbremFits,track);

    ATH_MSG_DEBUG ("Brem track, refit with electron brem fit");
    newTrack = fit(*track, true, Trk::electron);
  }
  else
  {
    // statistics
    increment_by_eta(m_Nfits,track);

    ATH_MSG_DEBUG ("Normal track, refit");
    newTrack = fit(prds, *par, true, m_particleHypothesis);

    if (!newTrack && m_tryBremFit && par->pT() > m_pTminBrem &&
        (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
    {
      // statistics
      increment_by_eta(m_NrecoveryBremFits,track);

      ATH_MSG_DEBUG ("Normal fit failed, try brem recovery");
      newTrack = fit(*track, true, Trk::electron);
    }
  }
  
  if(newTrack)
  {
    // statistic
    increment_by_eta(m_NgoodFits,newTrack);

    //keeping the track of previously accumulated TrackInfo
    const Trk::TrackInfo old_info = track->info();
    newTrack->info().addPatternReco(old_info);
  }
  else
  {
    // statistic
    increment_by_eta(m_NfailedFits,track);
  }
  return newTrack;
}






