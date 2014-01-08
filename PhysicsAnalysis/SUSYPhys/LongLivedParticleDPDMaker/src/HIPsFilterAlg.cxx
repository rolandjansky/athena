/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LongLivedParticleDPDMaker/HIPsFilterAlg.h"

#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "TRT_DriftFunctionTool/TRT_DriftFunctionTool.h"

#include "CaloEvent/CaloClusterContainer.h"
#include "Identifier/Identifier.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

const double PI = 3.1415926535;
const double TWOPI = 2.0*PI;

inline double delta_phi(double phi1, double phi2)
{
  float PHI=fabs(phi1-phi2);
  return (PHI<=PI)? PHI : TWOPI-PHI;
}

/// Standard Service Constructor
HIPsFilterAlg::HIPsFilterAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthFilterAlgorithm(name, pSvcLocator), 
    m_nProcessed(0),
    m_nEventsPassE(0),
    m_nEventsPassHTTRTnumber(0),
    m_nEventsPassHTTRTfraction(0),
    m_nEventsPassAll(0)
  
{

  declareProperty("cutMinE", m_cutMinE = 0.0);
  declareProperty("cutEtaMax", m_cutEtaMax = 3.0);
  declareProperty("cutMaxDeltaPhi", m_cutMaxDeltaPhi = 0.5);
  declareProperty("cutMinTRTHTHits", m_cutMinTRTHTHits = 100);
  declareProperty("cutMinTRTHTFraction", m_cutMinTRTHTFraction = 0.1);
  declareProperty("caloCluster", m_caloCluster = "CaloCalTopoCluster"); 
  declareProperty("passAll", m_passAll = false); 
  
}

/// Destructor
HIPsFilterAlg::~HIPsFilterAlg() {}

/// Gaudi Service Interface method implementations:
StatusCode HIPsFilterAlg::initialize()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  msg(MSG::INFO) << "==> initialize ..." << name() << endreq;
  msg(MSG::INFO)<< "******* HIPsFilterAlg"<< name() <<" Configuration **********" << endreq;  
  msg(MSG::INFO)<<"caloCluster       "<<      m_caloCluster     <<endreq;
  msg(MSG::INFO)<<"cutMinE          "<<         m_cutMinE        <<endreq;
  msg(MSG::INFO)<<"cutEtaMax           "<<          m_cutEtaMax         <<endreq;
  msg(MSG::INFO)<<"cutMaxDeltaPhi         "<<        m_cutMaxDeltaPhi       <<endreq;
  msg(MSG::INFO)<<"cutMinTRTHTHits         "<<        m_cutMinTRTHTHits       <<endreq;
  msg(MSG::INFO)<<"cutMinTRTHTFraction         "<<        m_cutMinTRTHTFraction       <<endreq;
  msg(MSG::INFO)<<"passAll             "<<            m_passAll           <<endreq;

  return sc;
}

StatusCode HIPsFilterAlg::execute()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);

  m_nProcessed += 1;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "==> execute " << name() << " on " << m_nProcessed << " event..." << endreq;

  // Reject al events from runs for which the hiptrt trigger was running online
  const EventInfo* eventInfo;
  sc = evtStore()->retrieve(eventInfo);
  if ( sc.isFailure() ){
    msg(MSG::ERROR) << "Could not retrieve event info" << endreq;
    return sc;
  }

  //const EventID* myEventID=eventInfo->event_ID();  
  //if(myEventID->run_number() < 211522){
    
    //If passAll is selected, accept all events
    if(m_passAll){
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << name() << " event passed because passAll is true" << endreq;
      setFilterPassed(true);
      return sc;
    }
    
    //  msg(MSG::INFO) << "==> calorimeter store gate" << endreq;
    const CaloClusterContainer* caloclust = 0;
    sc = evtStore()->retrieve(caloclust, m_caloCluster);
    if(sc.isFailure() || !caloclust){
      msg(MSG::WARNING) << m_caloCluster <<" container not found in StoreGate " << endreq;
      setFilterPassed(false);
      return StatusCode::SUCCESS;
    }
    
    std::vector<std::pair<double, double> > calophieta;
    
    /// iterators over the container 
    CaloClusterContainer::const_iterator emtopoItr  = caloclust->begin();
    CaloClusterContainer::const_iterator emtopoItrE = caloclust->end();
    
    //  msg(MSG::INFO) << "==> calorimeter loop" << endreq;
    
    /// loop over the ESD electron container
    /// and fill the egamma pt, eta and E/P histograms
    for (; emtopoItr != emtopoItrE; ++emtopoItr) {
      
      double calE = (*emtopoItr)->e();
      double calEta = (*emtopoItr)->eta();
      double calPhi = (*emtopoItr)->phi();
      
      if (calE > m_cutMinE && fabs(calEta) < m_cutEtaMax) {
	msg(MSG::DEBUG) << "Energy=" << (*emtopoItr)->e()
			<< "  eta=" << (*emtopoItr)->eta()
			<< "  phi=" << (*emtopoItr)->phi()
			<< "  m=" << (*emtopoItr)->m()
			<< endreq;
	//      calophi.push_back(calPhi);
	//      calomap.insert(std::make_pair(calE,std::make_pair(calPhi,calEta)));
	calophieta.push_back(std::make_pair(calPhi,calEta));
      }	
    }
    
    if (!calophieta.empty()) m_nEventsPassE++;
    
    const TRT_ID *m_trtID = 0;
    
    sc = detStore()->retrieve(m_trtID, "TRT_ID");
    
    if (sc.isFailure() || !m_trtID) {
      msg(MSG::WARNING) << "Could not get TRT ID helper" << endreq;
      return StatusCode::SUCCESS;
    }
    
    const InDet::TRT_DriftCircleContainer* driftCircleContainer = 0;  
    
    sc = evtStore()->retrieve( driftCircleContainer, "TRT_DriftCircles");
    
    if (sc.isFailure() || !driftCircleContainer) {
      msg(MSG::DEBUG) << "No TRT RDO in StoreGate " << endreq;
    } else {
      msg(MSG::DEBUG) << "found TRT driftcircles in StoreGate " << endreq; 
    }
    
    std::map<std::pair<double, double>, int> calotrt;
    std::map<std::pair<double, double>, int> calohttrt;
    
    for (std::vector<std::pair<double, double> >::iterator it1 = calophieta.begin(); it1 != calophieta.end(); it1++){
      calotrt[*it1] = 0;
      calohttrt[*it1] = 0;
    }
    
    InDet::TRT_DriftCircleContainer::const_iterator trtdriftContainerItr  = driftCircleContainer->begin();
    InDet::TRT_DriftCircleContainer::const_iterator trtdriftContainerItrE = driftCircleContainer->end();
    
    for (; trtdriftContainerItr != trtdriftContainerItrE; ++trtdriftContainerItr) {
      
      InDet::TRT_DriftCircleCollection::const_iterator trtItr = (*trtdriftContainerItr)->begin();
      InDet::TRT_DriftCircleCollection::const_iterator trtEnd = (*trtdriftContainerItr)->end();
      
      for(; trtItr!=trtEnd; trtItr++){
	
	// find out which detector element the hit belongs to
	const InDetDD::TRT_BaseElement *det = (*trtItr)->detectorElement();
	
	Identifier ID = (*trtItr)->identify();
	
	double streta = det->strawCenter(m_trtID->straw(ID)).eta();
	double strphi = det->strawCenter(m_trtID->straw(ID)).phi();
	
	for (std::vector<std::pair<double, double> >::iterator it1 = calophieta.begin(); it1 != calophieta.end(); it1++){
	  if (delta_phi(it1->first, strphi) < m_cutMaxDeltaPhi) {
	    //	  if (fabs(it1->second) < 0.3 || it1->second * streta > 0.0){    // match eta requirements
	    if ((fabs(it1->second) < 0.1 && abs(m_trtID->barrel_ec(ID)) == 1) || it1->second * streta > 0.0){    // match eta requirements
	      calotrt[*it1]++;
	      if ((*trtItr)->highLevel()) calohttrt[*it1]++;
	      
	      
	      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)
		<< "timeOverThreshold=" << (*trtItr)->timeOverThreshold()
		<< " highLevel=" << (*trtItr)->highLevel()
		<< " rawDriftTime=" << (*trtItr)->rawDriftTime()
		<< " barrel_ec=" << m_trtID->barrel_ec(ID)
		<< " phi_module=" << m_trtID->phi_module(ID)
		<< " layer_or_wheel=" << m_trtID->layer_or_wheel(ID)
		<< " straw_layer=" << m_trtID->straw_layer(ID)
		<< " straw=" << m_trtID->straw(ID)
		<< " scR=" << det->strawCenter(m_trtID->straw(ID)).perp()
		<< " scPhi=" << det->strawCenter(m_trtID->straw(ID)).phi()
		<< endreq;		    
	      
	      
	    }
	  }
	}
      }
    }
    
    bool bingo = false;
    bool countedHTTRTnumber = false;
    
    for (std::vector<std::pair<double, double> >::iterator it1 = calophieta.begin(); it1 != calophieta.end(); it1++){
      double ratioHT = (calotrt[*it1] > 0) ?  (double)calohttrt[*it1]/(double)calotrt[*it1] : 0.0;
      if ( calohttrt[*it1] >= m_cutMinTRTHTHits) {      
	if (!countedHTTRTnumber) {
	  m_nEventsPassHTTRTnumber++;
	  countedHTTRTnumber = true;
	}
	if (ratioHT >= m_cutMinTRTHTFraction) {
	  m_nEventsPassHTTRTfraction++;
	  bingo = true;
	  break;
	}
      }
    }
    
    if (bingo) {
      setFilterPassed(true);
      m_nEventsPassAll++;
    }
    else setFilterPassed(false);
    
    //  } else setFilterPassed(false); ////// Closing if(runMunber<XXXXXX)
  
  
  return sc;  
}

StatusCode HIPsFilterAlg::finalize()
{
  // Declare the simple StatusCode
  StatusCode sc(StatusCode::SUCCESS);
  
  // Calculate the efficiencies and their errors
  
  // Efficiencies
  double effiEMClusters = m_nProcessed > 0 ? m_nEventsPassE / (1.0*m_nProcessed) : 0.0;
  double effiHTTRTnumber = m_nEventsPassE > 0 ? m_nEventsPassHTTRTnumber / (1.0*m_nEventsPassE) : 0.0;
  double effiHTTRTfraction = m_nEventsPassHTTRTnumber > 0 ? m_nEventsPassHTTRTfraction / (1.0*m_nEventsPassHTTRTnumber) : 0.0;
  double effiPassedAll  = m_nProcessed > 0 ? m_nEventsPassAll / (1.0*m_nProcessed) : 0.0;
  
  // Error on efficiencies
  
  double rootn = m_nProcessed > 0 ? sqrt((double)m_nProcessed) : 0.0;
  double rootHTnumber = m_nEventsPassE > 0 ? sqrt((double)m_nEventsPassE) : 0.0;
  double rootHTfraction = m_nEventsPassHTTRTnumber > 0 ? sqrt((double)m_nEventsPassHTTRTnumber) : 0.0;
  
  double effiErrEMClusters = (rootn > 0 && effiEMClusters >= 0.0 && effiEMClusters <= 1.0)? sqrt(effiEMClusters*(1.0-effiEMClusters)) / rootn : 0.0;
  double effiErrHTTRTnumber = (rootHTnumber > 0 && effiHTTRTnumber >= 0.0 && effiHTTRTnumber <= 1.0)? sqrt(effiHTTRTnumber*(1.0-effiHTTRTnumber)) / rootHTnumber : 0.0;
  double effiErrHTTRTfraction = (rootHTfraction > 0 && effiHTTRTfraction >= 0.0 && effiHTTRTfraction <= 1.0)? sqrt(effiHTTRTfraction*(1.0-effiHTTRTfraction)) / rootHTfraction : 0.0;
  double effiErrPassedAll = (rootn > 0 && effiPassedAll >= 0.0 && effiPassedAll <= 1.0)? sqrt(effiPassedAll*(1.0-effiPassedAll)) / rootn : 0.0;
  
  effiEMClusters *= 100.0;
  effiHTTRTnumber *= 100.0;
  effiHTTRTfraction *= 100.0;
  effiPassedAll *= 100.0;
  effiErrEMClusters *= 100.0;
  effiErrHTTRTnumber *= 100.0;
  effiErrHTTRTfraction *= 100.0;
  effiErrPassedAll *= 100.0;
  
  if (msgLvl(MSG::INFO)) {
    msg(MSG::INFO) << "==> finalize " << name() << endreq;
    msg(MSG::INFO) << "***************************************************************" << endreq;
    msg(MSG::INFO) << "Cut-flow table of " << name() << " skimming algorithm:" << endreq;
    msg(MSG::INFO) << "-------------" << endreq;
    msg(MSG::INFO) << " Number of processed events: " << m_nProcessed << endreq;
    msg(MSG::INFO) << " Events passed EM cluster cuts:  " << m_nEventsPassE << " and resulting efficiency = (" << effiEMClusters << "+/-" << effiErrEMClusters << ")" << endreq;
    msg(MSG::INFO) << " Events passed HTTRT number cut:  " << m_nEventsPassHTTRTnumber << " and efficiency wrt EM = (" << effiHTTRTnumber << "+/-" << effiErrHTTRTnumber << ")" << endreq;
    msg(MSG::INFO) << " Events passed HTTRT fraction cut:  " << m_nEventsPassHTTRTfraction << " and efficiency wrt HT number = (" << effiHTTRTfraction << "+/-" << effiErrHTTRTfraction << ")" << endreq;
    msg(MSG::INFO) << " Events passed all cuts:  " << m_nEventsPassAll << " and resulting efficiency = (" << effiPassedAll << "+/-" << effiErrPassedAll << ")" << endreq;
    msg(MSG::INFO) << "***************************************************************" << endreq;
  }
  return sc;  
}
