/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigROBMoni.cxx
 * @brief  ROB data access monitoring
 * @author Frank Winklmeier
 *
 * $Id: TrigROBMoni.cxx,v 1.1 2009-03-16 13:52:57 fwinkl Exp $
 */

// Trigger includes
#include "TrigROBMoni.h"
#include "TrigSteering/TrigSteer.h"
#include "TrigInterfaces/TECreateAlgo.h"
#include "TrigSteeringEvent/Chain.h"

// ROOT includes
#include "TH1F.h"
#include "TH2I.h"
#include "TProfile.h"
#include "TProfile2D.h"

// TDAQ common includes
#include "eformat/SourceIdentifier.h"

// STL includes
#include <set>
#include <algorithm>

using namespace HLT;
using namespace std;
using robmonitor::ROBDataMonitorStruct;
using robmonitor::ROBDataStruct;

// statics
SubDetNamesMap TrigROBMoni::m_subDetNames;
SubDetNamesMap TrigROBMoni::m_subDetGroupNames;


//--------------------------------------------------------------------------------
// TrigROBMoni tool
//--------------------------------------------------------------------------------

TrigROBMoni::TrigROBMoni(const std::string& type,
                         const std::string& name,
                         const IInterface* parent) :
  TrigMonitorToolBase(type, name, parent),
  m_steering(0),
  m_hs_request_time(this, "ROBRequestTime", "Request time [ms]"),
  m_hs_request_size(this, "ROBRequestSize", "Request size [words]"),
  m_hs_cached_fraction(this, "CachedROBFraction", "Cached ROB fraction per event"),
  m_hs_history_total(this, "ROBHistory"),
  m_hs_history_event(this, "ROBHistoryEvent"),
  m_h_shared_requests(0)
{
  declareProperty("DetailedMonitoring", m_detailedMonitoring = false,
                  "Enable detailed monitoring");
  declareProperty("ROBDataMonitorCollectionSGName", m_ROBDataMonitorCollection_SG_Name="ROBDataMonitorCollection",
		  "Name of cost monitoring collection in SG");

  // Bin assignment for ROB history
  m_ROBHistoryToBin[robmonitor::UNCLASSIFIED] = 1;
  m_ROBHistoryToBin[robmonitor::SCHEDULED]    = 2;
  m_ROBHistoryToBin[robmonitor::RETRIEVED]    = 3;
  m_ROBHistoryToBin[robmonitor::CACHED]       = 4;
  m_ROBHistoryToBin[robmonitor::IGNORED]      = 5;
  m_ROBHistoryToBin[robmonitor::DISABLED]     = 6;

}

TrigROBMoni::~TrigROBMoni()
{
}


StatusCode TrigROBMoni::initialize()
{
  msg() << MSG::INFO << " SG key for ROB monitoring collect. = " << m_ROBDataMonitorCollection_SG_Name << endmsg;

  // Get pointer to steering (should be our parent)
  m_steering = dynamic_cast<const HLT::TrigSteer*>(parent());
  
  if ( m_steering ) {
    // Make list of algorithms able to request data (all but Hypo)
    vector<Gaudi::Algorithm*>::const_iterator a;
    for ( a = m_steering->subAlgorithms()->begin();
          a != m_steering->subAlgorithms()->end(); ++a ) {
      if ( dynamic_cast<const HLT::TECreateAlgo*>(*a) ) m_fexAlgos.push_back(*a);
    }
  }
  else {
    msg() << MSG::INFO << "Parent algorithm is not of type TrigSteer. Some monitoring histograms will not be available." << endmsg;
  }

  /*
   * Fill list of available sub detectors.
   * Ideally this list would be provided by the eformat lib.
   */
  if ( m_subDetNames.empty() ) {
    for (uint16_t sid = 0; sid <= eformat::OTHER; ++sid) {
      eformat::helper::SourceIdentifier SID(static_cast<eformat::SubDetector>(sid), 0);
      const string detector = SID.human_detector();
      const string group = SID.human_group();
      if (detector.find("UNKNOWN")==std::string::npos) m_subDetNames[sid] = detector;
      if (group.find("UNKNOWN") == std::string::npos) m_subDetGroupNames[SID.subdetector_group()] = group;
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigROBMoni::bookHists()
{
  const INamedInterface* inamed = dynamic_cast<const INamedInterface*>(parent());
  const std::string name = inamed ? inamed->name()+"/" : "";
  TrigMonGroup expertHists(this, name + "TrigROBMoni", TrigMonitorToolBase::expert);

  // Labels for ROB history
  vector<string> robHistory;
  robHistory.reserve(robmonitor::NUM_ROBHIST_CODES);
  robHistory.push_back("UNCLASSIFIED");
  robHistory.push_back("SCHEDULED");
  robHistory.push_back("RETRIEVED");
  robHistory.push_back("CACHED");
  robHistory.push_back("IGNORED");
  robHistory.push_back("DISABLED");

    
  // Book histogram sets
  StatusCode sc;
  if ((sc = m_hs_request_time.book(expertHists)) != StatusCode::SUCCESS) 
    msg() << MSG::ERROR << "Cannot book histogram Set " << m_hs_request_time.name  << endmsg;
    
  if ((sc = m_hs_request_size.book(expertHists))!= StatusCode::SUCCESS) 
    msg() << MSG::ERROR << "Cannot book histogram Set " << m_hs_request_size.name  << endmsg;

  if ((sc = m_hs_cached_fraction.book(expertHists))!= StatusCode::SUCCESS) 
    msg() << MSG::ERROR << "Cannot book histogram Set " << m_hs_cached_fraction.name  << endmsg;

  if ((sc = m_hs_history_total.book(expertHists, &robHistory))!= StatusCode::SUCCESS) 
    msg() << MSG::ERROR << "Cannot book histogram Set " <<  m_hs_history_total.name  << endmsg;

  if ((sc = m_hs_history_event.book(expertHists, &robHistory, false))!= StatusCode::SUCCESS) 
    msg() << MSG::ERROR << "Cannot book histogram Set " <<  m_hs_history_event.name  << endmsg;


  // Book additional histograms
  m_h_shared_requests = new TH2F("SharedROBRequests_Algo",
				 "Fraction of shared ROB requests per event",
				 m_fexAlgos.size(), 0, m_fexAlgos.size(),
				 m_fexAlgos.size(), 0, m_fexAlgos.size());
  
  vector<Gaudi::Algorithm*>::const_iterator a;
  int bin;
  for ( a = m_fexAlgos.begin(), bin=1; a != m_fexAlgos.end(); ++a, ++bin ) {
    m_h_shared_requests->GetXaxis()->SetBinLabel(bin, (*a)->name().c_str());
    m_h_shared_requests->GetYaxis()->SetBinLabel(bin, (*a)->name().c_str());
  }

  // Register histograms
  if ( expertHists.regHist(m_h_shared_requests).isFailure() ){
    msg() << MSG::ERROR << "Cannot register histogram " << m_h_shared_requests->GetName() << endmsg;
    return StatusCode::FAILURE;
  }
    
  return StatusCode::SUCCESS;
}



StatusCode TrigROBMoni::fillHists()
{
  // Do nothing if no monitoring collection found in StoreGate
  ROBDataMonitorCollection* robMonColl(0);
  if ( evtStore()->transientContains<ROBDataMonitorCollection>(m_ROBDataMonitorCollection_SG_Name.value()) ) {
    StatusCode sc = evtStore()->retrieve(robMonColl);
    if ( sc.isFailure() ) {
      msg() << MSG::ERROR << "Could not retrieve ROBDataMonitorCollection" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else {
    if (msgLvl(MSG::DEBUG)) {
      msg() << "No ROBDataMonitorCollection found in StoreGate" << endmsg;
    }
    return StatusCode::SUCCESS;
  }  

  // Loop over monitoring information in StoreGate
  m_hs_history_event.reset();
  map<string, set<uint32_t> > algoROBs;   // to calculate shared ROB fraction
  ROBDataMonitorCollection::const_iterator monIter; 
  map<const uint32_t,ROBDataStruct>::const_iterator robIter;
  string algo_name;

  for ( monIter = robMonColl->begin(); monIter != robMonColl->end(); ++monIter ) {
    const ROBDataMonitorStruct& robMon = **monIter;
    
    for ( robIter = robMon.requested_ROBs.begin();
          robIter != robMon.requested_ROBs.end(); ++robIter ) {
      algo_name = robMon.requestor_name;
      if (algo_name.find("_pref")!=std::string::npos) algo_name.erase(algo_name.find("_pref"));//remove _pref

      const ROBDataStruct& rob = (*robIter).second;
      algoROBs[algo_name].insert(rob.rob_id);
      
      // Fill request size
      if (m_hs_request_size.fill(rob.rob_size, rob, algo_name) != StatusCode::SUCCESS) 
	msg() << MSG::ERROR << "Cannot fill histogram Set " <<  m_hs_request_size.name  << endmsg;

      // Fill request time
      if (m_hs_request_time.fill(robMon.elapsedTime(), rob, algo_name) != StatusCode::SUCCESS) 
	msg() << MSG::ERROR << "Cannot fill histogram Set " <<  m_hs_request_time.name  << endmsg;
      
      // Fill ROB history (need to fill with bin-1 since y-axis starts at 0 !)
      if ( m_hs_history_event.fill(m_ROBHistoryToBin[rob.rob_history]-1, rob, algo_name)!= StatusCode::SUCCESS) 
	msg() << MSG::ERROR << "Cannot fill histogram Set " << m_hs_history_event.name  << endmsg;

      // Fill ROB history (need to fill with bin-1 since y-axis starts at 0 !)
      if ( m_hs_history_total.fill(m_ROBHistoryToBin[rob.rob_history]-1, rob, algo_name)!= StatusCode::SUCCESS) 
	msg() << MSG::ERROR << "Cannot fill histogram Set " << m_hs_history_total.name  << endmsg;

    }    
  }

  // Fill cached ROB fractions
  for ( short n = 0; n < m_hs_history_event.NHISTS; ++n ) {
    TH1* h = m_hs_history_event.hist[n];
    for ( int b = 1; b <= h->GetNbinsX(); ++b ) {
      double cached = h->GetBinContent(b, m_ROBHistoryToBin[robmonitor::CACHED]);
      double retrieved = h->GetBinContent(b, m_ROBHistoryToBin[robmonitor::RETRIEVED]);
      if ( cached+retrieved > 0 ) {
        m_hs_cached_fraction.hist[n]->Fill(h->GetXaxis()->GetBinLabel(b),
                                           cached/(cached+retrieved));
      }
    }
  }

  // Fill shared ROB requests
  map<string, set<uint32_t> >::const_iterator i1, i2;
  vector<uint32_t>::iterator iv;
  for ( i1=algoROBs.begin(); i1!=algoROBs.end(); ++i1 ) {
    for ( i2=algoROBs.begin(); i2!=algoROBs.end(); ++i2 ) {
      vector<uint32_t> shared( max(i1->second.size(),i2->second.size()) );
      iv = set_intersection(i1->second.begin(), i1->second.end(),
                            i2->second.begin(), i2->second.end(), shared.begin());
      if (i1->second.size() !=0){
	m_h_shared_requests->Fill(i1->first.c_str(), i2->first.c_str(),
				  (double)(iv - shared.begin()) / i1->second.size());                       
      }
      else msg() << MSG::WARNING << "Cannot fill m_h_shared_requests correctly" <<endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}



//--------------------------------------------------------------------------------
// HistSet helper class
//--------------------------------------------------------------------------------

template <class HTYPE>
StatusCode HistSet<HTYPE>::book(TrigMonitorToolBase::TrigMonGroup& monGroup,
                                const std::vector<std::string>* yLabels,
                                bool regHist)
{
  for ( int n = 0; n < NHISTS; ++n ) hist[n] = new HTYPE();

  // Per subdetector histogram
  hist[SUBDET]->SetNameTitle((name+"_SubDet").c_str(),
                             (name + " per subdetector").c_str());  

  // Per subdetector group histogram
  hist[SDGROUP]->SetNameTitle((name+"_SubDetGroup").c_str(),
                              (name + " per subdetector group").c_str());

  // Per algorithm histogram
  hist[ALGO]->SetNameTitle((name+"_Algo").c_str(),
                           (name + " per Algorithm").c_str());

  
  /*
   * Binning
   */
  if ( hist[SUBDET]->GetDimension()==1 ) {
    hist[SUBDET]->SetBins(montool->m_subDetNames.size(), 0, montool->m_subDetNames.size());
    hist[SDGROUP]->SetBins(montool->m_subDetGroupNames.size(), 0, montool->m_subDetGroupNames.size());
    hist[ALGO]->SetBins(1, 0, 1);
    hist[ALGO]->SetCanExtend(TH1::kAllAxes);
  }
  else if ( hist[SUBDET]->GetDimension()==2 ) {

    size_t yBins = yLabels ? yLabels->size() : 0;
    hist[SUBDET]->SetBins(montool->m_subDetNames.size(),
                     0, montool->m_subDetNames.size(),
                     yBins, 0, yBins);
    
    hist[SDGROUP]->SetBins(montool->m_subDetGroupNames.size(),
                          0, montool->m_subDetGroupNames.size(),
                          yBins, 0, yBins);

    hist[ALGO]->SetBins(1, 0, 1,yBins, 0, yBins);
    hist[ALGO]->SetCanExtend(TH2::kAllAxes);

    // Set y-labels if given
    if ( yLabels ) {
      for ( size_t i=0; i<yLabels->size(); ++i ) {
        for ( int n=0; n<NHISTS; ++n )
          hist[n]->GetYaxis()->SetBinLabel(i+1, (*yLabels)[i].c_str());        
      }
    }
  } 
  
  
  /*
   * Bin labels
   */
  SubDetNamesMap::const_iterator sd;
  int bin;
  for ( sd=montool->m_subDetNames.begin(), bin=1;
        sd!=montool->m_subDetNames.end(); ++sd, ++bin ) {
    hist[SUBDET]->GetXaxis()->SetBinLabel(bin, (*sd).second.c_str());      
  }

  SubDetGroupNamesMap::const_iterator sdg;
  for ( sdg=montool->m_subDetGroupNames.begin(), bin=1;
        sdg!=montool->m_subDetGroupNames.end(); ++sdg, ++bin ) {
    hist[SDGROUP]->GetXaxis()->SetBinLabel(bin, (*sdg).second.c_str());
  }

  //vector<Gaudi::Algorithm*>::const_iterator a;
  //for ( a = montool->m_fexAlgos.begin(), bin=1;
  //      a != montool->m_fexAlgos.end(); ++a, ++bin ) {
  //  hist[ALGO]->GetXaxis()->SetBinLabel(bin, (*a)->name().c_str());
  //}
    

  /*
   * Register and other common tasks
   */
  for ( int n = 0; n < NHISTS; ++n ) {
    hist[n]->SetYTitle(axisLabel.c_str());
    if (regHist) {
      if ( monGroup.regHist(hist[n]).isFailure() )  
        return StatusCode::FAILURE;
    }
  }
    
  return StatusCode::SUCCESS;
}


template <class HTYPE>
StatusCode HistSet<HTYPE>::fill(double value, const robmonitor::ROBDataStruct& rob,
                                const std::string& requester)
{
  eformat::helper::SourceIdentifier SID(rob.rob_id);
  int dim = hist[SUBDET]->GetDimension();

  // Fill interface is different for 1D/2D histograms
  if ( dim==1 ) {
    hist[SUBDET]->Fill(SID.human_detector().c_str(), value);
    hist[SDGROUP]->Fill(SID.human_group().c_str(), value);
    hist[ALGO]->Fill(requester.c_str(), value); 
    hist[ALGO]->LabelsDeflate("X");   
  }
  else if ( dim==2 ) {
    const double w = 1;
    static_cast<TH2*>(hist[SUBDET])->Fill(SID.human_detector().c_str(), value, w);
    static_cast<TH2*>(hist[SDGROUP])->Fill(SID.human_group().c_str(), value, w);
    static_cast<TH2*>(hist[ALGO])->Fill(requester.c_str(), value, w);    
    static_cast<TH2*>(hist[ALGO])->LabelsDeflate("X");    
  }
    
  return StatusCode::SUCCESS;
}


  
