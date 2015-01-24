/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HICentrality.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HICentrality.h"
#include "HIGlobal/HICentralityData.h"
#include "GeneratorObjects/HijingEventParams.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "CaloEvent/CaloCellContainer.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "DetDescrCondTools/ICoolHistSvc.h"

#include "TMath.h"
#include "TH1F.h"

#include <vector>
#include <iostream>

HICentrality::HICentrality(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_coolhistsvc("CoolHistSvc", name),
  m_histfolder("/GLOBAL/HeavyIon/CentralityCalib"),
  m_channel(1)
{
  // default file name with calibration histos
  declareProperty("HICentralityCalibrationsFileName", 
	 m_HICentralityCalibrationsFileName="HICentralityCalibrations.root");
  // if not requested in python script it will not be used
  declareProperty("isData",m_isData);
  declareProperty("effCorr",m_effCorr);
  declareProperty("CentralityPercSchema",m_CentralityPercSchema);

  m_calibfile = 0;
  m_isData = false;
  m_effCorr = 1.0;
  m_initialized = false;

  m_b_perch = 0;
  m_nw_perch = 0;
  m_ncoll_perch = 0;
  m_calocell_energy_perch = 0;
  m_number_of_siclusters_perch = 0;
}

 HICentrality::~HICentrality()
{
  // destructor
  // deletes histograms to avoid memory leak

  //delete m_calibfile;
  //delete m_b_perch;
  //delete m_nw_perch;
  //delete m_ncoll_perch;
  //delete m_calocell_energy_perch;
  //delete m_number_of_siclusters_perch;
}


StatusCode HICentrality::initialize()
{
  // Initializes services and calibration histograms
  
  ATH_MSG_DEBUG( name() << " initialize()" );

  ATH_MSG_DEBUG( "Applying trigger efficiency correction of " << m_effCorr );

  m_initialized = true;

  ATH_CHECK (m_coolhistsvc.retrieve() );

  // register IOV callback function for the COOL folder
  const DataHandle<CondAttrListCollection> aptr;
  ATH_CHECK( detStore()->regFcn(&HICentrality::callBack,
                                this,aptr,m_histfolder) );
  ATH_MSG_VERBOSE( "Will access histogram name " << m_histfolder << " from COOL folder " << m_histfolder );

  return StatusCode::SUCCESS;
}

StatusCode HICentrality::callBack( IOVSVC_CALLBACK_ARGS_P(/* I */,keys) ) {
  // printout the list of keys invoked - will normally only be for our
  // histogram folder
  msg(MSG::DEBUG) << "HICentrality callback invoked for keys:";
  for (const std::string& key : keys)
    msg() << key << " ";
  msg() << endreq;
  // check all the keys, if we find the histogram folder, update the pointer
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end();++itr) {
    if (*itr==m_histfolder) {
      ATH_CHECK( m_coolhistsvc->getHist(m_histfolder,1,"b_perch",m_b_perch) );
      ATH_CHECK( m_coolhistsvc->getHist(m_histfolder,1,"mw_perch",m_nw_perch) );
      ATH_CHECK( m_coolhistsvc->getHist(m_histfolder,1,"ncoll_perch",m_ncoll_perch) );
      ATH_CHECK( m_coolhistsvc->getHist(m_histfolder,1,"calocell_energy_perch",m_calocell_energy_perch) );
      ATH_CHECK( m_coolhistsvc->getHist(m_histfolder,1,"number_of_siclusters_perch",m_number_of_siclusters_perch) );
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode  HICentrality::finalize()
{
  // Code entered here will be executed once at the end of the program run
  ATH_MSG_DEBUG( name() << " finalize()" );
  return StatusCode::SUCCESS;
}

//================ Execution =================================================

StatusCode HICentrality::HijingParsVect(std::vector<float> &hijing_event_params)
{
  hijing_event_params.resize(7);

  const HijingEventParams *hijing_pars;
  ATH_CHECK( evtStore()->retrieve(hijing_pars, "Hijing_event_params") );

  hijing_event_params[0] = hijing_pars->get_b();
  hijing_event_params[1] = hijing_pars->get_np();
  hijing_event_params[2] = hijing_pars->get_nt();
  hijing_event_params[3] = hijing_pars->get_n0();
  hijing_event_params[4] = hijing_pars->get_n01();
  hijing_event_params[5] = hijing_pars->get_n10();
  hijing_event_params[6] = hijing_pars->get_n11();

  return StatusCode::SUCCESS;
}

StatusCode HICentrality::CaloCellEnergy(std::vector<float> &calocell_energy_by_detector) 
{
  calocell_energy_by_detector.resize(24);
  for(int i=0; i<24; i++) calocell_energy_by_detector[i]=0;
  
  const CaloCellContainer* cellContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve (cellContainer,"AllCalo") );

  for (const CaloCell* cell : *cellContainer) {
    int det = cell->caloDDE()->getSampling();
    if(det < 24) 
      calocell_energy_by_detector[det] += cell->energy();
  }
  
  return StatusCode::SUCCESS;
}

StatusCode HICentrality::NumberOfSiClusters(std::vector<float> &npix_clusters, std::vector<float> &nsct_clusters) 
{
  // Pixel clusters
  npix_clusters.resize(4);
  for (int i=0;i<4;i++) npix_clusters[i]=0;

  const InDet::SiClusterContainer* prdContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve(prdContainer, "PixelClusters") );

  InDet::SiClusterContainer::const_iterator colNext;
  InDet::SiClusterContainer::const_iterator lastCol;

  colNext = prdContainer->begin();
  lastCol = prdContainer->end();  
  for (; colNext != lastCol; ++colNext) {
    // iterators over PRDs in container
    InDet::SiClusterCollection::const_iterator nextCluster = 
      (*colNext)->begin();
    InDet::SiClusterCollection::const_iterator lastCluster = 
      (*colNext)->end();
    for (; nextCluster!=lastCluster; ++nextCluster){
      if ( &((*nextCluster)->globalPosition() )!=0 )
	{
	  float x = (*nextCluster)->globalPosition().x();
	  float y = (*nextCluster)->globalPosition().y();
	  float z = (*nextCluster)->globalPosition().z();
	  float r = sqrt(x*x+y*y);
	  float absz = fabs(z);
	  if(r<150 && absz<450) { // Pixel barrel
	    if(r<70       )npix_clusters[0]++;
	    if(r<110&&r>70)npix_clusters[1]++; 
	    if(r>110      )npix_clusters[2]++; 
	  }
	  if(absz>450) { // EndCaps
	    npix_clusters[3]++;
	  }
	}
      else{
	ATH_MSG_ERROR( "no (*nextCluster)->globalPosition() " );
      }
    }
  }

  // SCT clusters
  nsct_clusters.resize(5);
  for (int i=0;i<5;i++) nsct_clusters[i]=0;

  ATH_CHECK( evtStore()->retrieve(prdContainer, "SCT_Clusters") );

  colNext = prdContainer->begin(); 
  lastCol = prdContainer->end();
  for (; colNext != lastCol; ++colNext) {
    // iterators over PRDs in container
    InDet::SiClusterCollection::const_iterator nextCluster = 
      (*colNext)->begin();
    InDet::SiClusterCollection::const_iterator lastCluster = 
      (*colNext)->end();
    for (; nextCluster!=lastCluster; ++nextCluster){
      if ( &((*nextCluster)->globalPosition() )!=0 )
	{
	  float x = (*nextCluster)->globalPosition().x();
	  float y = (*nextCluster)->globalPosition().y();
	  float z = (*nextCluster)->globalPosition().z();
	  float r = sqrt(x*x+y*y);
	  float absz = fabs(z);
	  if(r<550 && absz<750) { // SCT barrel
	    if(r<330       )nsct_clusters[0]++;
	    if(r<400&&r>330)nsct_clusters[1]++; 
	    if(r<480&&r>400)nsct_clusters[2]++;
	    if(r>480       )nsct_clusters[3]++;
	  }
	  if(absz>750) { // EndCaps
	    nsct_clusters[4]++;
	  }
	}
      else{
	ATH_MSG_ERROR( "no (*nextCluster)->globalPosition() " );
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

StatusCode  HICentrality::execute()
{
  // Creates HICentralityData object and saves it in SG
  //cout << " HICentrality::execute() " << endl;

  /*
  // histograms should have been retrieved automatically 
  // by a callback function
  m_b_perch = getHistogram("b_perch");
  m_nw_perch = getHistogram("nw_perch");
  m_ncoll_perch = getHistogram("ncoll_perch");
  m_calocell_energy_perch = getHistogram("calocell_energy_perch");
  m_number_of_siclusters_perch = getHistogram("number_of_siclusters_perch");
  */

  if(!(m_b_perch && m_nw_perch && m_ncoll_perch &&
       m_calocell_energy_perch && m_number_of_siclusters_perch)) {
    ATH_MSG_ERROR( "Could not retrieve centrality calibration" );
    m_initialized = false;
  }
  ATH_MSG_DEBUG( "successfully retrieved calibration histograms!" << name() );

  if( !m_initialized) 
    return StatusCode::FAILURE;

  HICentralityData *hcd;
  if(m_CentralityPercSchema.size()) 
    hcd = new HICentralityData(m_CentralityPercSchema);
  else 
    hcd = new HICentralityData((char*) "HICentrality");
  //if(m_CentralityPercSchema.size()) hcd->SetPerc(m_CentralityPercSchema);

  unsigned short bin, bincorr;

  if(! m_isData) {

    // calculate centrality bin from truth impact parameter (MC only)
    std::vector<float> hijing_event_params;
    StatusCode sc = HijingParsVect(hijing_event_params);
    
    if (!sc.isFailure()) {
      bool failed = false;
      float b = hijing_event_params[0];
      if( binsearch((TH1F*)m_b_perch, b, bin) ) {
	bin = 99 - bin;
        bincorr = corrbin(bin,m_effCorr);
	hcd->SetBin((char*)"b",bincorr);
	hcd->SetVar((char*)"b",b);
	ATH_MSG_DEBUG( "Impact parameter: " << b 
                       << " bin: " << bin << " bincorr: " << bincorr );
      } else failed = true;
      float nw = hijing_event_params[1] + hijing_event_params[2];
      if( binsearch((TH1F*)m_nw_perch, nw, bin) ) {
        bincorr = corrbin(bin,m_effCorr);
	hcd->SetBin((char*)"nw",bincorr);
	hcd->SetVar((char*)"nw",nw);
	ATH_MSG_DEBUG( "Number participating nucleons: " << nw 
                       << " bin: " << bin << " bincorr: " << bincorr );
      } else failed = true;
      float ncoll = hijing_event_params[3] + hijing_event_params[4] +
	hijing_event_params[5] + hijing_event_params[6];
      if( binsearch((TH1F*)m_ncoll_perch, ncoll, bin) ) {
        bincorr = corrbin(bin,m_effCorr);
	hcd->SetBin((char*)"ncoll",bincorr);
	hcd->SetVar((char*)"ncoll",ncoll);
	ATH_MSG_DEBUG( "Number n-n collisions: " << ncoll 
                       << " bin: " << bin << " bincorr: " << bincorr );
      } else failed = true;
      if (failed)
	ATH_MSG_DEBUG( "Problem in bin calculation from Hijing event parameters" 
                       );
    } 
  } else {
    ATH_MSG_DEBUG( "Running on Data, not using Hijing event parameters" );
  }

  // calculate centrality bin from calocell energy
  std::vector<float> calocell_energy_by_detector;
  StatusCode sc = CaloCellEnergy(calocell_energy_by_detector);
  if (!sc.isFailure()) {
    float etotem=0; float etothad=0; float etotfcal=0; 
    for(int det= 0; det< 8; det++) etotem  += calocell_energy_by_detector[det];
    for(int det= 8; det<21; det++) etothad += calocell_energy_by_detector[det];
    for(int det=21; det<24; det++) etotfcal += calocell_energy_by_detector[det];
    float calocell_energy;
    calocell_energy = etotem + etothad + etotfcal;
    calocell_energy /= 1000000.;
    if( binsearch((TH1F*)m_calocell_energy_perch, calocell_energy, bin) ) {
      bincorr = corrbin(bin,m_effCorr);
      hcd->SetBin((char*)"calocell_energy",bincorr);
      hcd->SetVar((char*)"calocell_energy",calocell_energy);
      ATH_MSG_DEBUG( "CaloCell energy: " << calocell_energy 
                     << " bin: " << bin << " bincorr: " << bincorr );
    } else {
      ATH_MSG_INFO( "CaloCell energy bin calculation problem" );
    }
  }  

  // calculate centrality bin from siclusters
  std::vector<float> npix_clusters;
  std::vector<float> nsct_clusters;
  sc = NumberOfSiClusters(npix_clusters, nsct_clusters);

  float number_of_siclusters = 0;
  if (!sc.isFailure()) {
    number_of_siclusters += 
      npix_clusters[0] + npix_clusters[1] + 
      npix_clusters[2] + npix_clusters[3];
    number_of_siclusters += 
      nsct_clusters[0] + nsct_clusters[1] + 
      nsct_clusters[2] + nsct_clusters[3] + nsct_clusters[4];

    if( binsearch((TH1F*)m_number_of_siclusters_perch, 
		  number_of_siclusters, bin) ) {
      bincorr = corrbin(bin,m_effCorr);
      hcd->SetBin((char*)"number_of_siclusters",bincorr);
      hcd->SetVar((char*)"number_of_siclusters",number_of_siclusters);
      ATH_MSG_DEBUG( "Number of siclusters: " << number_of_siclusters 
                     << " bin: " << bin << " bincorr: " << bincorr );
    } else {
      ATH_MSG_INFO( "Number of siclusters bin calculation problem" );
    }
  }  
  
  ATH_CHECK( evtStore()->record( hcd,"HICentrality" ) );
  return StatusCode::SUCCESS;
}

bool HICentrality::binsearch (TH1F *hist_perc, float value, unsigned short &bin)
{
  // determine bin in [0,99] range
  Int_t nbins =  hist_perc->GetNbinsX();
  Int_t tmpbin = TMath::BinarySearch(nbins, hist_perc->GetArray(),value);
  if(tmpbin > nbins-0.99) {
    ATH_MSG_WARNING( "value " << value 
	  << " above max centrality measure in histogram " << hist_perc->GetName() );
    bin = nbins -1;
  } else if(tmpbin < -0.01) {
    ATH_MSG_WARNING( "value " << value 
                     << " below 0 in histogram " << hist_perc->GetName() );
    bin = 0;
  } else
    bin = tmpbin;

  return true;
}

unsigned short HICentrality::corrbin (unsigned short bin, float effCorr)
{
  unsigned short bincorr = 
    TMath::Nint((1 - effCorr)*100 + effCorr*bin);

  return bincorr;
}

//===========================================================================
