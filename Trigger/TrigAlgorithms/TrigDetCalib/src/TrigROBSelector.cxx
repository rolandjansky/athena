/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "GaudiKernel/Algorithm.h"

#include "TrigROBSelector.h"
#include "TH1.h"

TrigROBSelector::TrigROBSelector(const std::string& type, const std::string& name,
                                 const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_regionSelector("RegSelSvc/RegSelSvc", name),
    m_tHistSvc("THistSvc/THistSvc", name)
{
  declareInterface<ITrigROBSelector>( this );
  
  declareProperty("etaMinMon", m_etaMinMon =  -4.8, "lower edge of eta range for monitoring");
  declareProperty("etaMaxMon", m_etaMaxMon =   4.8, "upper edge of eta range for monitoring");
  declareProperty("phiMinMon", m_phiMinMon = -M_PI, "lower edge of phi range for monitoring");
  declareProperty("phiMaxMon", m_phiMaxMon =  M_PI, "upper edge of phi range for monitoring");
  declareProperty("Subdetectors", m_detSpec = "All", "Subdetectors to include in readout");
  declareProperty("MonitoringHistoPath", m_outputPath = "", "Path to monitoring histograms");
  
  //std::string::size_type index;
  //std::string newName = name;
  //while ((index = newName.find(".")) != std::string::npos) newName.replace(index, 1, "_");  

  m_detNames[PIXEL] = "PIXEL";
  m_detNames[SCT]   = "SCT";
  m_detNames[TRT]   = "TRT";
  m_detNames[TTEM]  = "TTEM";
  m_detNames[TTHEC] = "TTHEC";
  m_detNames[TILE]  = "TILE";
  m_detNames[FCALEM]  = "FCALEM";
  m_detNames[FCALHAD] = "FCALHAD";
  m_detNames[MDT] = "MDT";
  m_detNames[RPC] = "RPC";
  m_detNames[TGC] = "TGC";
  /* on April 19, 2018, these subdetectors were not included by default, so do not change this
  m_detNames[LAR] = "LAR"; 
  m_detNames[FTK] = "FTK"; 
  */
  m_detNames[CSC] = "CSC"; // included since April 19, 2018, small size
}


StatusCode TrigROBSelector::initialize()
{
  const IMonitoredAlgo* parentMon = dynamic_cast<const IMonitoredAlgo*>(parent());
  if (!parentMon) {
    ATH_MSG_WARNING("Attached to algorithm which is not of type IMonitoredAlgo");
    return StatusCode::FAILURE;
  }
  const Algorithm* parentAlg = dynamic_cast<const Algorithm*>(parent());
  if (!parentAlg) {
    ATH_MSG_WARNING("Attached to algorithm which is not of type Algorithm");
    return StatusCode::FAILURE;
  }
  std::string parentName =  parentAlg->name();

  if (m_outputPath == "") m_outputPath = "/EXPERT/" + parentName + "/";
  

  // The RegionSelector is being retrieved here
  StatusCode sc = StatusCode::SUCCESS;
  
  sc = m_regionSelector.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve RegionSelector Service");
    return sc;
  }
  
  sc = m_tHistSvc.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve pointer to THistSvc");
    return sc;
  }

  std::map<DETID, bool> isIncluded;

  // Look for some groups:
  if (m_detSpec.find("ID") != std::string::npos)
    isIncluded[PIXEL] = isIncluded[SCT] = isIncluded[TRT] = true;

  if (m_detSpec.find("LAr") != std::string::npos) {
    isIncluded[TTEM] = isIncluded[TTHEC] = true;
    isIncluded[FCALEM] = isIncluded[FCALHAD] = true;
  }

  if (m_detSpec.find("Muons") != std::string::npos)
    isIncluded[MDT] = isIncluded[RPC] = isIncluded[TGC] = isIncluded[CSC] = true;

  if (m_detSpec.find("EM") != std::string::npos)
    isIncluded[TTEM] = isIncluded[FCALEM] = true;

  if (m_detSpec.find("Had") != std::string::npos)
    isIncluded[TTHEC] = isIncluded[TILE] = isIncluded[FCALHAD] = true;

  if (m_detSpec.find("All") != std::string::npos) {
    for (std::map<DETID,std::string>::const_iterator name = m_detNames.begin();
         name != m_detNames.end(); name++)
      isIncluded[name->first] = true;
  }

  m_detectors.clear();

  // Look for the registered detector names and fill the detector mask.
  for (std::map<DETID,std::string>::const_iterator name = m_detNames.begin();
       name != m_detNames.end(); name++) {
    if (m_detSpec.find(name->second)   != std::string::npos) 
      isIncluded[name->first] = true;
    if (isIncluded[name->first]) m_detectors.push_back(name->first);
  }
  
  msg() << MSG::DEBUG << "Selected subdetectors = ";
  for (std::vector<DETID>::const_iterator det = m_detectors.begin(); 
       det != m_detectors.end(); det++) {
        msg() << MSG::DEBUG << *det << " " << endmsg; 
  }
  msg() << endmsg;

  return sc;
}


StatusCode TrigROBSelector::finalize()
{
  return StatusCode::SUCCESS;
}


HLT::ErrorCode TrigROBSelector::fillPEBInfo(PartialEventBuildingInfo& pebInfo,
                                            const IRoiDescriptor& roi,
                                            std::vector<DETID>* dets, std::vector<int>* nRobs)
{
  std::vector<uint32_t> allROBs;

  for (std::vector<DETID>::const_iterator detector = m_detectors.begin();
       detector != m_detectors.end(); detector++)
  {
    std::vector<uint32_t> detectorROBs;
    m_regionSelector->DetROBIDListUint(*detector, roi, detectorROBs);
    
    allROBs.insert(allROBs.end(), detectorROBs.begin(), detectorROBs.end());
    msg() << MSG::DEBUG << "ROBs requested for detector " << *detector << " : ";
    
    for (unsigned int i = 0; i < detectorROBs.size(); i++) msg() << MSG::DEBUG << "0x" << std::hex << detectorROBs[i] << std::dec << " ";
    msg() << endmsg;
    
    if (dets) (*dets).push_back(*detector);
    if (nRobs) (*nRobs).push_back(detectorROBs.size());
    
    TH1I*& robHist = m_robHists[*detector];
    if (!robHist) {
      ATH_MSG_DEBUG("Monitoring of detector " << *detector << " not implemented yet.");
      continue;
    }

    for (unsigned int i=0; i<detectorROBs.size(); i++)
      robHist->Fill(m_robIdMap[detectorROBs[i]]);
  }

  for (std::vector<uint32_t>::const_iterator rob = allROBs.begin(); rob != allROBs.end(); rob++)
    pebInfo.add(*rob);

  return HLT::OK;
}
    

bool TrigROBSelector::reset()
{
  return true;
}


HLT::ErrorCode TrigROBSelector::setupMonitoring()
{
  ATH_MSG_INFO("in setupMonitoring() ");
  
  for (std::vector<DETID>::const_iterator det = m_detectors.begin(); 
       det != m_detectors.end(); det++) {
    std::vector<uint32_t> range;

    double eta = 0.5*(m_etaMinMon+m_etaMaxMon);
    double phi = HLT::phiMean(m_phiMinMon, m_phiMaxMon); 
    TrigRoiDescriptor roi( eta, m_etaMinMon, m_etaMaxMon, phi, m_phiMinMon, m_phiMaxMon );
    m_regionSelector->DetROBIDListUint(*det, roi, range );
    ATH_MSG_DEBUG("Range for detId = " << *det << " = " << range.size());
    m_ranges[*det] = range;
        
    int bin = 0;
    for (std::vector<uint32_t>::const_iterator robId = range.begin(); 
         robId != range.end(); ++robId, ++bin)
      m_robIdMap[*robId] = bin;
  }
  
  // Now setup all the histograms
  HLT::ErrorCode ec = bookMonitoringHistograms();
  if (ec != HLT::OK) return ec;
  
  for (std::vector<DETID>::const_iterator det = m_detectors.begin(); 
       det != m_detectors.end(); det++) {

    std::stringstream ss;
    std::string label;
    const std::vector<uint32_t>& range = m_ranges[*det];
    TH1I*& hist = m_robHists[*det];
    if (!hist) continue; 
   
    for (unsigned int j = 0; j < range.size(); j++) {
      ss << std::hex << "0x" << range[j] << std::dec;
      ss >> label;
      hist->GetXaxis()->SetBinLabel(j+1, label.c_str());
    }
  }

  ATH_MSG_INFO("done setupMonitoring() ");
  return HLT::OK;
}


HLT::ErrorCode TrigROBSelector::bookMonitoringHistograms()
{
  ATH_MSG_DEBUG("Going to book and register monitoring histograms");

  for (std::vector<DETID>::const_iterator det = m_detectors.begin(); 
       det != m_detectors.end(); det++) {
		
		// temporarily disable these histograms
		// since the following errors occured
		// TTHEC and TTEM can't be summed (nbins 0)
		// in the other histograms all bins have the same axis labels
		m_robHists[*det] = 0; continue;

			 
    if (*det == MDT || *det == RPC || *det == TGC || *det == CSC)  { m_robHists[*det] = 0; continue; } // Not implemented yet
    TH1I* h = bookAndRegisterTH1I("RobId" + m_detNames[*det], m_outputPath, m_ranges[*det].size(), 0, m_ranges[*det].size());
    
    if (!h) return HLT::BAD_JOB_SETUP;
    m_robHists[*det] = h;
  }
  
  ATH_MSG_DEBUG("done : book and register monitoring histograms");

  return HLT::OK;
}


TH1I* TrigROBSelector::bookAndRegisterTH1I(std::string name, std::string outpath, int nBins, int minX, int maxX) 
{
  // Say hello
  ATH_MSG_DEBUG("Now trying to register histogram: " << name << " (TH1I)");
  TH1I* h = new TH1I(name.c_str(), name.c_str(), nBins, minX, maxX); 
  if (m_tHistSvc->regHist(outpath + h->GetName(), h).isFailure()) {
     ATH_MSG_WARNING("Can't book " << outpath + h->GetName());
     return 0;
  }
  ATH_MSG_DEBUG("done : register histograms");
  return h;
}


StatusCode TrigROBSelector::queryInterface( const InterfaceID& riid,
                                                    void** ppvIf )
{
  if ( riid == ITrigROBSelector::interfaceID() )  {
    *ppvIf = (ITrigROBSelector*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  
  return AthAlgTool::queryInterface( riid, ppvIf );
}
