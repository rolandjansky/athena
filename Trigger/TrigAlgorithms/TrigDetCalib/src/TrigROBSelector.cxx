/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "GaudiKernel/Algorithm.h"

#include "TrigDetCalib/TrigROBSelector.h"
#include "TH1.h"

TrigROBSelector::TrigROBSelector(const std::string& type, const std::string& name,
                                 const IInterface* parent)
  : AlgTool(type, name, parent),
    m_msg(0),
    m_msgLvl(0),
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
}


StatusCode TrigROBSelector::initialize()
{
  m_msg = new MsgStream( msgSvc(), name());
  m_msgLvl = m_msg->level();

  const IMonitoredAlgo* parentMon = dynamic_cast<const IMonitoredAlgo*>(parent());
  if (!parentMon) {
    msg() << MSG::WARNING << "Attached to algorithm which is not of type IMonitoredAlgo" << endreq;
    return StatusCode::FAILURE;
  }
  const Algorithm* parentAlg = dynamic_cast<const Algorithm*>(parent());
  std::string parentName =  parentAlg->name();

  if (m_outputPath == "") m_outputPath = "/EXPERT/" + parentName + "/";
  

  // The RegionSelector is being retrieved here
  StatusCode sc;
  
  sc = m_regionSelector.retrieve();
  if (sc.isFailure()) {
    msg() << MSG::FATAL << "Unable to retrieve RegionSelector Service" << endreq;
    return sc;
  }
  
  sc = m_tHistSvc.retrieve();
  if (sc.isFailure()) {
    msg() << MSG::ERROR << "Unable to retrieve pointer to THistSvc" << endreq;
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
    isIncluded[MDT] = isIncluded[RPC] = isIncluded[TGC] = true;

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
  
  msg() << MSG::INFO << "Selected subdetectors = ";
  for (std::vector<DETID>::const_iterator det = m_detectors.begin(); 
       det != m_detectors.end(); det++) msg() << *det << " ";
  msg() << endreq;
  //msg() << MSG::INFO << "njpb " << m_outputPath << endreq;

  return StatusCode::SUCCESS;
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
    
    for (unsigned int i = 0; i < detectorROBs.size(); i++) msg() << "0x" << std::hex << detectorROBs[i] << std::dec << " ";
    msg() << endreq;
    
    if (dets) (*dets).push_back(*detector);
    if (nRobs) (*nRobs).push_back(detectorROBs.size());
    
    TH1I*& robHist = m_robHists[*detector];
    static bool warnedAlready = false; 
    if (!robHist) {
      if (!warnedAlready) {
        msg() << MSG::DEBUG << "Monitoring of detector " << *detector << " not implemented yet." << endreq;
        warnedAlready = true;
      }
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
  msg() << MSG::INFO << "in setupMonitoring() " << endreq;
  
  for (std::vector<DETID>::const_iterator det = m_detectors.begin(); 
       det != m_detectors.end(); det++) {
    std::vector<uint32_t> range;

    double eta = 0.5*(m_etaMinMon+m_etaMaxMon);
    double phi = HLT::phiMean(m_phiMinMon, m_phiMaxMon); 
    TrigRoiDescriptor roi( eta, m_etaMinMon, m_etaMaxMon, phi, m_phiMinMon, m_phiMaxMon );
    m_regionSelector->DetROBIDListUint(*det, roi, range );
    msg() << MSG::DEBUG << "Range for detId = " << *det << " = " << range.size() << std::endl;
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

  msg() << MSG::INFO << "done setupMonitoring() " << endreq;
  return HLT::OK;
}


HLT::ErrorCode TrigROBSelector::bookMonitoringHistograms()
{
  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Going to book and register monitoring histograms" << endreq;

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
  
  msg() << MSG::DEBUG << "done : book and register monitoring histograms" << endreq;

  return HLT::OK;
}


TH1I* TrigROBSelector::bookAndRegisterTH1I(std::string name, std::string outpath, int nBins, int minX, int maxX) 
{
  // Say hello
  if (msgLvl()<=MSG::DEBUG) 
    msg() << MSG::DEBUG << "Now trying to register histogram: " << name << " (TH1I)" << endreq;
  TH1I* h = new TH1I(name.c_str(), name.c_str(), nBins, minX, maxX); 
  if (m_tHistSvc->regHist(outpath + h->GetName(), h).isFailure()) {
     msg() << MSG::WARNING << "Can't book " << outpath + h->GetName() << endreq;
     return 0;
  }
  msg() << MSG::DEBUG << "done : register histograms" << endreq;
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
  
  return AlgTool::queryInterface( riid, ppvIf );
}
