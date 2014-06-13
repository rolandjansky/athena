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
#include "DetDescrCondTools/ICoolHistSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "CaloEvent/CaloCellContainer.h"
#include "InDetPrepRawData/SiClusterContainer.h"

#include "TMath.h"
#include "TH1F.h"

#include <vector>
#include <iostream>

using std::vector;
using std::cout; using std::endl;

HICentrality::HICentrality(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator),
  m_log(msgSvc(),name),
  p_detstore(0),p_coolhistsvc(0),
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
  
  m_log.setLevel(outputLevel());
  m_log << MSG::DEBUG << name() << " initialize()" << endreq;

  m_log << MSG::DEBUG << "Applying trigger efficiency correction of " << m_effCorr << endreq;

  m_initialized = true;

  // retrieve the StoreGate Service (delete if not needed)
  StatusCode sc= service("StoreGateSvc",m_sgSvc);
  if (sc.isFailure()) 
    m_log << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
  else 
    m_log << MSG::DEBUG << "StoreGateSvc retrieved!" << endreq;

  // Detector store:
  if(StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    m_log << MSG::FATAL << "#HEAVYION# Detector store not found" << endreq;
    return StatusCode::FAILURE;
  }
  // CoolHistSvc:
  if(StatusCode::SUCCESS!=service("CoolHistSvc",p_coolhistsvc)) {
    m_log << MSG::FATAL << "#HEAVYION# Could not get CoolHistSvc" << endreq;
    return StatusCode::FAILURE;
  }
  // register IOV callback function for the COOL folder
  const DataHandle<CondAttrListCollection> aptr;
  if (StatusCode::SUCCESS==p_detstore->regFcn(&HICentrality::callBack,
					      this,aptr,m_histfolder)) {
    m_log << MSG::DEBUG << "Registered IOV callback from HICentrality" 
	  << endreq;
  } else {
    m_log << MSG::ERROR << "Registration of IOV callback failed" << endreq;
  }
  m_log << "Will access histogram name " << m_histfolder << " from COOL folder " << m_histfolder
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode HICentrality::callBack( IOVSVC_CALLBACK_ARGS_P(/* I */,keys) ) {
  // printout the list of keys invoked - will normally only be for our
  // histogram folder
  m_log << MSG::DEBUG << "HICentrality callback invoked for keys:";
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end();++itr) m_log << *itr << " ";
  m_log << endreq;
  // check all the keys, if we find the histogram folder, update the pointer
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end();++itr) {
    if (*itr==m_histfolder) {
      // try to retrieve the histogram
      const std::string histname1("b_perch");
      if (StatusCode::SUCCESS==
	  p_coolhistsvc->getHist(m_histfolder,1,histname1,m_b_perch)) {
	if (m_b_perch!=0) {
	  m_log << MSG::DEBUG << "Cached pointer to histogram: " << histname1 << endreq;
	} else { 
	  m_log << MSG::ERROR << "Could not cache pointer to histogram " << histname1 << endreq;
	}
      } else {
	m_log << MSG::ERROR << "Cannot retrieve pointer to histogram " << histname1 << endreq;
      }
      const std::string histname2("nw_perch");
      if (StatusCode::SUCCESS==
	  p_coolhistsvc->getHist(m_histfolder,1,histname2,m_nw_perch)) {
	if (m_nw_perch!=0) {
	  m_log << MSG::DEBUG << "Cached pointer to histogram: nw_perch" << endreq;
	} else { 
	  m_log << MSG::ERROR << "Could not cache pointer to histogram nw_perch" << endreq;
	}
      } else {
	m_log << MSG::ERROR << "Cannot retrieve pointer to histogram nw_perch" << endreq;
      }
      const std::string histname3("ncoll_perch");
      if (StatusCode::SUCCESS==
	  p_coolhistsvc->getHist(m_histfolder,1,histname3,m_ncoll_perch)) {
	if (m_ncoll_perch!=0) {
	  m_log << MSG::DEBUG << "Cached pointer to histogram: ncoll_perch" << endreq;
	} else { 
	  m_log << MSG::ERROR << "Could not cache pointer to histogram ncoll_perch" << endreq;
	}
      } else {
	m_log << MSG::ERROR << "Cannot retrieve pointer to histogram ncoll_perch" << endreq;
      }
      const std::string histname4("calocell_energy_perch");
      if (StatusCode::SUCCESS==
	  p_coolhistsvc->getHist(m_histfolder,1,histname4,m_calocell_energy_perch)) {
	if (m_calocell_energy_perch!=0) {
	  m_log << MSG::DEBUG << "Cached pointer to histogram: calocell_energy_perch" << endreq;
	} else { 
	  m_log << MSG::ERROR << "Could not cache pointer to histogram calocell_energy_perch" << endreq;
	}
      } else {
	m_log << MSG::ERROR << "Cannot retrieve pointer to histogram calocell_energy_perch" << endreq;
      }
      const std::string histname5("number_of_siclusters_perch");
      if (StatusCode::SUCCESS==
	  p_coolhistsvc->getHist(m_histfolder,1,histname5,m_number_of_siclusters_perch)) {
	if (m_number_of_siclusters_perch!=0) {
	  m_log << MSG::DEBUG << "Cached pointer to histogram: number_of_siclusters_perch" << endreq;
	} else { 
	  m_log << MSG::ERROR << "Could not cache pointer to histogram number_of_siclusters_perch" << endreq;
	}
      } else {
	m_log << MSG::ERROR << "Cannot retrieve pointer to histogram number_of_siclusters_perch" << endreq;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode  HICentrality::finalize()
{
  // Code entered here will be executed once at the end of the program run
  m_log << MSG::DEBUG << name() << " finalize()" << endreq;
  return StatusCode::SUCCESS;
}

//================ Execution =================================================

StatusCode HICentrality::HijingParsVect(vector<float> &hijing_event_params)
{
  StatusCode sc;

  hijing_event_params.resize(7);

  const HijingEventParams *hijing_pars;
  sc = m_sgSvc->retrieve(hijing_pars, "Hijing_event_params");
  if ( sc.isFailure() ) {
    m_log << MSG::INFO << "Could not retrieve Hijing_event_params." << endreq;
    return StatusCode::FAILURE;
  }

  hijing_event_params[0] = hijing_pars->get_b();
  hijing_event_params[1] = hijing_pars->get_np();
  hijing_event_params[2] = hijing_pars->get_nt();
  hijing_event_params[3] = hijing_pars->get_n0();
  hijing_event_params[4] = hijing_pars->get_n01();
  hijing_event_params[5] = hijing_pars->get_n10();
  hijing_event_params[6] = hijing_pars->get_n11();

  return StatusCode::SUCCESS;
}

StatusCode HICentrality::CaloCellEnergy(vector<float> &calocell_energy_by_detector) 
{
  StatusCode sc;

  calocell_energy_by_detector.resize(24);
  for(int i=0; i<24; i++) calocell_energy_by_detector[i]=0;
  
  const CaloCellContainer  * CellContainer=0;
  sc = m_sgSvc->retrieve (CellContainer,"AllCalo");
  if ( sc.isFailure() ) {
    m_log << MSG::ERROR  << "Could not retrieve AllCalo" << endreq;
    return StatusCode::FAILURE;
  }

  CaloCellContainer::const_iterator f_cell=CellContainer->begin();
  CaloCellContainer::const_iterator l_cell=CellContainer->end();

  for ( ; f_cell!=l_cell; ++f_cell) {
    const CaloCell* cell = (*f_cell) ;
    int det = cell->caloDDE()->getSampling();
    if(det < 24) 
      calocell_energy_by_detector[det] += cell->energy();
  }
  
  return StatusCode::SUCCESS;
}

StatusCode HICentrality::NumberOfSiClusters(vector<float> &npix_clusters, vector<float> &nsct_clusters) 
{
  StatusCode sc;

  // Pixel clusters
  npix_clusters.resize(4);
  for (int i=0;i<4;i++) npix_clusters[i]=0;

  const InDet::SiClusterContainer* prdContainer;
  sc = m_sgSvc->retrieve(prdContainer, "PixelClusters");
  if (sc.isFailure() || !prdContainer){
    m_log << MSG::ERROR << "Pixel Cluster Container NOT found"<< endreq;
    return StatusCode::FAILURE;
  }

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
	m_log<< MSG::ERROR  << 
	  "no (*nextCluster)->globalPosition() " << endreq; }
    }
  }

  // SCT clusters
  nsct_clusters.resize(5);
  for (int i=0;i<5;i++) nsct_clusters[i]=0;

  sc = m_sgSvc->retrieve(prdContainer, "SCT_Clusters");
  if (sc.isFailure() || !prdContainer){
    m_log << MSG::ERROR << "SCT Cluster Container NOT found"<< endreq;
    return StatusCode::FAILURE;
  }

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
	m_log<< MSG::ERROR  << 
	  "no (*nextCluster)->globalPosition() " << endreq; }
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
    m_log << MSG::ERROR << "Could not retrieve centrality calibration" << endreq;
    m_initialized = false;
  }
  m_log << MSG::DEBUG << "successfully retrieved calibration histograms!" << name() << endreq;

  if( !m_initialized) 
    return StatusCode::FAILURE;

  StatusCode sc;
  
  HICentralityData *hcd;
  if(m_CentralityPercSchema.size()) 
    hcd = new HICentralityData(m_CentralityPercSchema);
  else 
    hcd = new HICentralityData((char*) "HICentrality");
  //if(m_CentralityPercSchema.size()) hcd->SetPerc(m_CentralityPercSchema);

  unsigned short bin, bincorr;

  if(! m_isData) {

    // calculate centrality bin from truth impact parameter (MC only)
    vector<float> hijing_event_params;
    sc = HijingParsVect(hijing_event_params);
    
    if (!sc.isFailure()) {
      bool failed = false;
      float b = hijing_event_params[0];
      if( binsearch((TH1F*)m_b_perch, b, bin) ) {
	bin = 99 - bin;
        bincorr = corrbin(bin,m_effCorr);
	hcd->SetBin((char*)"b",bincorr);
	hcd->SetVar((char*)"b",b);
	m_log << MSG::DEBUG
	      << "Impact parameter: " << b 
	      << " bin: " << bin << " bincorr: " << bincorr << endreq;
      } else failed = true;
      float nw = hijing_event_params[1] + hijing_event_params[2];
      if( binsearch((TH1F*)m_nw_perch, nw, bin) ) {
        bincorr = corrbin(bin,m_effCorr);
	hcd->SetBin((char*)"nw",bincorr);
	hcd->SetVar((char*)"nw",nw);
	m_log << MSG::DEBUG
	      << "Number participating nucleons: " << nw 
	      << " bin: " << bin << " bincorr: " << bincorr << endreq;
      } else failed = true;
      float ncoll = hijing_event_params[3] + hijing_event_params[4] +
	hijing_event_params[5] + hijing_event_params[6];
      if( binsearch((TH1F*)m_ncoll_perch, ncoll, bin) ) {
        bincorr = corrbin(bin,m_effCorr);
	hcd->SetBin((char*)"ncoll",bincorr);
	hcd->SetVar((char*)"ncoll",ncoll);
	m_log << MSG::DEBUG
	      << "Number n-n collisions: " << ncoll 
	      << " bin: " << bin << " bincorr: " << bincorr << endreq;
      } else failed = true;
      if (failed)
	m_log << MSG::DEBUG
	      << "Problem in bin calculation from Hijing event parameters" 
	      << endreq; 
    } 
  } else {
    m_log << MSG::DEBUG
	  << "Running on Data, not using Hijing event parameters" 
	  << endreq; 
  }

  // calculate centrality bin from calocell energy
  vector <float> calocell_energy_by_detector;
  sc = CaloCellEnergy(calocell_energy_by_detector);
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
      m_log << MSG::DEBUG
	    << "CaloCell energy: " << calocell_energy 
	    << " bin: " << bin << " bincorr: " << bincorr << endreq;
    } else {
      m_log << MSG::INFO
	    << "CaloCell energy bin calculation problem" << endreq; 
    }
  }  

  // calculate centrality bin from siclusters
  vector <float> npix_clusters;
  vector <float> nsct_clusters;
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
      m_log << MSG::DEBUG
	    << "Number of siclusters: " << number_of_siclusters 
	    << " bin: " << bin << " bincorr: " << bincorr << endreq;
    } else {
      m_log << MSG::INFO
	    << "Number of siclusters bin calculation problem" << endreq; 
    }
  }  
  
  sc = m_sgSvc->record( hcd,"HICentrality" );
  if ( sc.isFailure() )
    {
      m_log << MSG::ERROR << "Failed to register HICentrality in SG" << endreq;
    } 

  return StatusCode::SUCCESS;
}

bool HICentrality::binsearch (TH1F *hist_perc, float value, unsigned short &bin)
{
  // determine bin in [0,99] range
  Int_t nbins =  hist_perc->GetNbinsX();
  Int_t tmpbin = TMath::BinarySearch(nbins, hist_perc->GetArray(),value);
  if(tmpbin > nbins-0.99) {
    m_log << MSG::WARNING << "value " << value 
	  << " above max centrality measure in histogram " << hist_perc->GetName()
	  << endreq;
    bin = nbins -1;
  } else if(tmpbin < -0.01) {
    m_log << MSG::WARNING << "value " << value 
	  << " below 0 in histogram " << hist_perc->GetName()
	  << endreq;
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

//============================================================================================
