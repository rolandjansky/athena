/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigOpMoni.cxx
 * @brief  Operation monitoring tool
 * @author J. Philipp Grohs, johannes_philipp.grohs@mailbox.tu-dresden.de
 * @author Frank Winklmeier
 *
 */

#include "TrigOpMoni.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/DirSearchPath.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "AthenaKernel/IOVRange.h"
#include "EventInfo/EventInfo.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGate.h"
#include "eformat/DetectorMask.h"
#include "TrigMonitorBase/TrigLockedHist.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace std;
using namespace eformat;

TrigOpMoni::TrigOpMoni(const std::string& type,
                       const std::string& name,
                       const IInterface* parent) :
   TrigMonitorToolBase(type, name, parent),
   m_JobOptionsSvc("JobOptionsSvc", name),
   m_lumiTool("LuminosityTool"),
   m_monGroup(this, boost::algorithm::replace_all_copy(name,".","/"), TrigMonitorToolBase::expert)
{
   declareProperty("JobOptionsSvc",   m_JobOptionsSvc, "JobOptionsSvc");
   declareProperty("ReleaseDataFile", m_releaseData = "../ReleaseData",
                   "Path to ReleaseData file (relative to LD_LIBRARY_PATH entries");
   declareProperty("DetailedFolderHists", m_detailedHists = true,
                   "Detailed histograms for COOL folder updates during run");
   declareProperty("LuminosityTool", m_lumiTool, "Luminosity tool");
   declareProperty("MaxLumiblocks", m_maxLB = 3000, "Number of lumiblocks for histograms");
}

TrigOpMoni::~TrigOpMoni()
{
}

StatusCode TrigOpMoni::initialize()
{
  ATH_CHECK(m_JobOptionsSvc.retrieve());

  // Register incident handlers 
  ServiceHandle<IIncidentSvc> IncidSvc("IncidentSvc",name());  
  ATH_CHECK(IncidSvc.retrieve());
  ATH_CHECK(m_lumiTool.retrieve());
  IncidSvc->addListener(this, "EndOfBeginRun", 0);
                
  return StatusCode::SUCCESS;
}


void TrigOpMoni::handle(const Incident &IncidInfo) 
{
  if(IncidInfo.type() == "EndOfBeginRun") {
     // One-time fills
    if (!m_MagFieldHistFilled) FillMagFieldHist();  
    if (!m_IOVDbHistFilled)    FillIOVDbHist();  
    if (!m_SubDetHistFilled)   FillSubDetHist();
  }
}  	


StatusCode TrigOpMoni::bookHists()
{
   m_SubDetHistFilled = false;
   m_MagFieldHistFilled = false;
   m_IOVDbHistFilled = false;
   m_previousLB = 0;
   m_pEvent = 0;
   m_MagFieldHist = 0;
   m_iovChangeHist = 0;   
   m_currentIOVs.clear();
   m_folderHist.clear();
 
   if ( !m_IOVDbSvc ) service("IOVDbSvc", m_IOVDbSvc, /*createIf=*/ false).ignore();  
   if ( m_IOVDbSvc ) {
     m_iovChangeHist = new TH2I("CoolFolderUpdate_LB",
                                "COOL folder updates;Lumiblock;",
                                1, 0, 1, 1, 0, 1);
   }   
   
   // create histogram for magnetic field information
   if ( !m_MagFieldSvc ) service("AtlasFieldSvc", m_MagFieldSvc, /*createIf=*/ false).ignore();
   if ( m_MagFieldSvc ) {
     m_MagFieldHist = new TH2I("MagneticFieldSettings",
                               "Magnetic field settings",
                               2, 0, 2, 2, 0, 2);
     
     m_MagFieldHist->GetYaxis()->SetBinLabel(1, "off");
     m_MagFieldHist->GetYaxis()->SetBinLabel(2, "on");
     m_MagFieldHist->GetXaxis()->SetBinLabel(1, "Solenoid");
     m_MagFieldHist->GetXaxis()->SetBinLabel(2, "Toroid");
   }

   m_generalHist = new TH1I("GeneralOpInfo",
                            "General operational info;;Applications",
                            2, 0, 2);
   
   m_lbDiffHist = new TH1I("EventLumiblockDiff",
                           "Lumiblock difference between events;LB difference (!=0)",
                           16, -5.5, 10.5);
   
   m_lumiHist = new TProfile("Luminosity", "Luminosity;Lumiblock;Luminosity [10^{33} cm^{-2}s^{-1}]",
                             m_maxLB, 0, m_maxLB);

   m_muHist = new TProfile("Pileup", "Pileup;Lumiblock;Interactions per BX",
                           m_maxLB, 0, m_maxLB);
   
   TH1* hist[] = {m_generalHist, m_lbDiffHist, m_iovChangeHist, m_MagFieldHist, 
                  m_lumiHist, m_muHist};

   for (TH1* h : hist) regHist(h);

   // Release data can be filled immediately (and only once)
   if (m_generalHist) FillReleaseData();

   return StatusCode::SUCCESS;
}

StatusCode TrigOpMoni::fillHists()
{
  if (evtStore()->retrieve(m_pEvent).isFailure()) {
    ATH_MSG_DEBUG("Could not find EventInfo object");
    m_pEvent = 0;
  }
    
  // One-time fills
  if (!m_MagFieldHistFilled) FillMagFieldHist();  
  if (!m_IOVDbHistFilled)    FillIOVDbHist();  
  if (!m_SubDetHistFilled)   FillSubDetHist();

  // Per-event fills
  FillLbDiffHist();

  // Per-LB fills  
  if ( m_pEvent && (m_previousLB != m_pEvent->event_ID()->lumi_block()) ) { // New LB

    // Fill LB Diff histogram
    if (m_lbDiffHist) m_lbDiffHist->Fill( static_cast<int>(m_pEvent->event_ID()->lumi_block()-m_previousLB) );

    // Fill IOV diff histogram
    FillIOVDbChangeHist();

    // Fill lumi histogram
    FillLumiHist();

    m_previousLB = m_pEvent->event_ID()->lumi_block();
  }
  
  return StatusCode::SUCCESS;
}

void TrigOpMoni::FillLbDiffHist()
{
  if (m_lbDiffHist==0) return;
}
    
      
void TrigOpMoni::FillMagFieldHist()
{
  if ( !m_MagFieldSvc || !m_MagFieldHist ) return;  

  m_MagFieldHist->Fill("Solenoid", m_MagFieldSvc->solenoidOn()   ? "on" : "off", 1.0);
  m_MagFieldHist->Fill("Toroid",   m_MagFieldSvc->toroidOn()     ? "on" : "off", 1.0);

  m_MagFieldHistFilled = true;  
}
  
void TrigOpMoni::FillIOVDbHist()
{  
  if ( !m_IOVDbSvc ) return;
  
   // create and fill histogram for IOVDb entries
  vector<string>             KeyList(m_IOVDbSvc->getKeyList());
  ostringstream              TmpStream;
  string                     FolderName;
  string                     Tag;
  IOVRange                   Range;
  bool                       Retrieved   = false;
  unsigned long long         bytesRead = 0;
  float                      readTime = 0;
      
  // set up histograms
  TH2I* IOVDbRunHist = new TH2I("IOVDbRunRange", "IOVDb Run Range", 1, 0, 1, 1, 0, 1);
  TH2I* IOVDbTimeHist = new TH2I("IOVDbTimeRange", "IOVDb Time Range", 1, 0, 1, 1, 0, 1);
  TProfile* IOVDbBytesReadHist = new TProfile("IOVDbBytesRead", "Folder data read", 1, 0, 1);
  IOVDbBytesReadHist->SetYTitle("Data read [bytes]");
  TProfile* IOVDbReadTimeHist = new TProfile("IOVDbReadTime", "Folder update time", 1, 0, 1);
  IOVDbReadTimeHist->SetYTitle("Update time [ms]");
  
  // fill histograms
  for(const string& key : KeyList) {
    
    if(m_IOVDbSvc->getKeyInfo(key, FolderName, Tag, Range, Retrieved, bytesRead, readTime) && Retrieved) {
      
      m_currentIOVs[key] = Range;
      
      IOVTime StartTime(Range.start());
      IOVTime StopTime(Range.stop());

      // fill start condition (run number)
      if (StartTime.isRunEvent()) {
        TmpStream.str("");
        TmpStream << StartTime.run();
        IOVDbRunHist->Fill(TmpStream.str().c_str(), FolderName.c_str(), 1.0);
      }
      
      // fill start condition (timestamp)
      if (StartTime.isTimestamp()) {
        TmpStream.str("");
        TmpStream << StartTime.timestamp();
        IOVDbTimeHist->Fill(TmpStream.str().c_str(), FolderName.c_str(), 1.0);
      }

      // fill stop condition (run number)
      if (StopTime.isRunEvent()) {
        if (StopTime.run() == IOVTime::MAXRUN) {
          IOVDbRunHist->Fill("infinity", FolderName.c_str(), 1.0);
        }
        else {
          TmpStream.str("");
          TmpStream << StopTime.run();
          IOVDbRunHist->Fill(TmpStream.str().c_str(), FolderName.c_str(), 1.0);
        }
      }
      
      // fill stop condition (timestamp)
      if (StopTime.isTimestamp()) {
        if(StopTime.timestamp() == IOVTime::MAXTIMESTAMP) {
          IOVDbTimeHist->Fill("infinity", FolderName.c_str(), 1.0);
        }
        else {
          TmpStream.str("");                
          TmpStream << StopTime.timestamp();
          IOVDbTimeHist->Fill(TmpStream.str().c_str(), FolderName.c_str(), 1.0);
        }
      }
      
      // fill read bytes
      IOVDbBytesReadHist->Fill(FolderName.c_str(), bytesRead, 1.0);
      
      // fill time read
      IOVDbReadTimeHist->Fill(FolderName.c_str(), readTime*1000, 1.0);
    }
  }
  
  // final configuration of histograms
  {
    scoped_lock_histogram lock;
    IOVDbRunHist->LabelsDeflate("X");
    IOVDbRunHist->LabelsDeflate("Y");
    IOVDbRunHist->GetYaxis()->LabelsOption("a");
  
    IOVDbTimeHist->LabelsDeflate("X");
    IOVDbTimeHist->LabelsDeflate("Y");
    IOVDbTimeHist->GetYaxis()->LabelsOption("a");

    IOVDbBytesReadHist->LabelsDeflate("X");  
    IOVDbReadTimeHist->LabelsDeflate("X");
  }
      
  // Register histograms
  const int nHists = 4;
  TH1* hist[nHists] = {IOVDbRunHist,
                       IOVDbTimeHist,
                       IOVDbBytesReadHist,
                       IOVDbReadTimeHist};
  
  for (int i=0; i<nHists; ++i) regHist(hist[i]);
  
  m_IOVDbHistFilled = true;
}

template <class HTYPE>
bool TrigOpMoni::regHist(HTYPE*& hist, bool verbose)
{
  if (!hist) return false;
  if ( m_monGroup.regHist(hist).isFailure() ) {
    if (verbose) {
      ATH_MSG_WARNING("Cannot register histogram " << hist->GetName());
    }
    delete hist;
    hist = 0;
  }
  return hist!=0;
}


std::string TrigOpMoni::folder2HistName(const std::string& folder)
{
  string tmp(folder);
  boost::algorithm::trim_left_if(tmp, boost::is_any_of(std::string("/")));  
  boost::algorithm::replace_all(tmp, "/", "_");
  return tmp;
}

void TrigOpMoni::FillIOVDbChangeHist()
{
  if ( m_IOVDbSvc==0 ) return;

  IOVRange iov;
  string folder, tag;
  bool retrieved;
  unsigned long long bytesRead;
  float readTime;

  vector<string> keys(m_IOVDbSvc->getKeyList());

  // Loop over all keys known to IOVDbSvc
  for(const string& k : keys) {
    if ( not m_IOVDbSvc->getKeyInfo(k, folder, tag, iov, retrieved, bytesRead, readTime) )
      continue;
    if ( not retrieved ) continue;

    map<string,IOVRange>::const_iterator curIOV = m_currentIOVs.find(k);
    if ( curIOV == m_currentIOVs.end() ) {
      m_currentIOVs[k] = iov;
      continue;
    }
    
    // Print IOV changes and fill histogram
    if ( iov != curIOV->second ) {
      ATH_MSG_INFO("IOV of " << k << " changed from " << curIOV->second
                    << " to " << iov
                    << " on event: " << *m_pEvent->event_ID() );

      if ( m_iovChangeHist ) {
        // Perform a locked fill and remove any empty bins to allow correct gathering
        scoped_lock_histogram lock;
        m_iovChangeHist->Fill(boost::lexical_cast<string>(m_pEvent->event_ID()->lumi_block()).c_str(), folder.c_str(), 1);
        m_iovChangeHist->LabelsDeflate("X");
        m_iovChangeHist->LabelsDeflate("Y");
      }

      // Create detailed histograms per changed folder
      if ( m_detailedHists ) {
        map<string, FolderHist>::iterator h = m_folderHist.find(folder);
        if ( h == m_folderHist.end() ) {
          string name(folder2HistName(folder));
          h = m_folderHist.insert(pair<string,FolderHist>(folder, FolderHist())).first;
          h->second.h_time = new TH1F((name+"_ReadTime").c_str(),
                                                 ("Update time for "+folder+";Time [ms];Entries").c_str(),
                                                 100, 0, 200);
                    
          m_folderHist[folder].h_bytes = new TH1F((name+"_BytesRead").c_str(),
                                                  ("Bytes read for "+folder+";Data [bytes];Entries").c_str(),
                                                  100, 0, 1000);

          regHist(m_folderHist[folder].h_time, false);
          regHist(m_folderHist[folder].h_bytes, false);
        }

        // Need to plot the difference because IOVDbSvc reports total time and bytes for entire job
        h->second.h_time->Fill(readTime*1000 - m_folderHist[folder].total_time);
        h->second.total_time += readTime*1000;
        
        h->second.h_bytes->Fill(bytesRead - m_folderHist[folder].total_bytes);
        h->second.total_bytes += bytesRead;
      }
      
      m_currentIOVs[k] = iov;
    }
  }
}

void TrigOpMoni::FillLumiHist()
{
  m_lumiHist->Fill(m_pEvent->event_ID()->lumi_block(), m_lumiTool->lbAverageLuminosity());
  m_muHist->Fill(m_pEvent->event_ID()->lumi_block(), m_lumiTool->lbAverageInteractionsPerCrossing());
}

void TrigOpMoni::FillSubDetHist()
{
  // create and fill histogram for subdetector states and ROB information
   if(!m_SubDetHistFilled)
   {
      vector<SubDetector>                 SubDetOnIdList;
      vector<SubDetector>                 SubDetOffIdList;
      vector<SubDetector>                 SubDetAllIdList;
      SimpleProperty<vector<uint32_t> >   EnabledROBsArray;
      map<SubDetector, uint32_t>          SubDetROBCountMap;
      TH2I*                               SubDetHist    = 0;
      const EventInfo*                    Event         = 0;
      const EventID*                      EventId       = 0;
      const Property*                     ROBProperty   = 0;
      uint64_t                            SubDetMask    = 0;
      int                                 Loop          = 0;

      // create histogram
      if(!(SubDetHist = new TH2I))
      {
         ATH_MSG_WARNING("Cannot create instance of class TH2I");
         m_SubDetHistFilled = true; // do not retry
         return;
      }

      // get event information
      if(evtStore()->retrieve(Event).isFailure())
         ATH_MSG_WARNING("Could not find EventInfo object");

      // get event ID
      else if(!(EventId = Event->event_ID()))
         ATH_MSG_WARNING("Could not find EventID object");

      // calculate subdetector mask
      else SubDetMask = ((uint64_t)EventId->detector_mask0())
            | (((uint64_t)EventId->detector_mask1()) << 32);

      // get list of enabled ROBs
      if(!(ROBProperty = Gaudi::Utils::getProperty(m_JobOptionsSvc->getProperties("DataFlowConfig"), "DF_Enabled_ROB_IDs"))) {
         msg(MSG::DEBUG) << "Could not find enabled ROB IDs: ";

         if(!m_JobOptionsSvc->getProperties("DataFlowConfig"))
         {
            msg() << "\"DataFlowConfig\" does not exist!" << endmsg;
            m_SubDetHistFilled = true;
         }

         else {
            msg() << "\"DF_Enabled_ROB_IDs\" does not exist! Available properties are:" << endmsg;

            const vector<const Property* >* PropTmp = m_JobOptionsSvc->getProperties("DataFlowConfig");

            for(unsigned int i = 0; i != PropTmp->size(); i++) {
              msg() << MSG::DEBUG << (*PropTmp)[i]->name() << endmsg;
            }
         }
      }

      else {
        if (!EnabledROBsArray.assign(*ROBProperty)) {
          ATH_MSG_WARNING("Could not read DF_Enabled_ROB_IDs property");
          m_SubDetHistFilled = true;
        }
      }

      // decode subdetector masks
      eformat::helper::DetectorMask(SubDetMask).sub_detectors(SubDetOnIdList);
      eformat::helper::DetectorMask(~SubDetMask).sub_detectors(SubDetOffIdList);
      eformat::helper::DetectorMask(numeric_limits<uint64_t>::max()).sub_detectors(SubDetAllIdList);

      // set up histogram
      SubDetHist->SetNameTitle("Subdetectors", "State of Subdetectors");
      SubDetHist->SetBins(SubDetAllIdList.size(), 0, SubDetAllIdList.size(), 3, 0, 3);
      SubDetHist->GetYaxis()->SetBinLabel(1, "off");
      SubDetHist->GetYaxis()->SetBinLabel(2, "on");
      SubDetHist->GetYaxis()->SetBinLabel(3, "# ROB");

      Loop = 1;

      // set bin labels for subdetectors
      for(vector<SubDetector>::iterator SubDetIter = SubDetAllIdList.begin();
         SubDetIter != SubDetAllIdList.end(); ++SubDetIter)
      {
         eformat::helper::SourceIdentifier SID(static_cast<eformat::SubDetector>(*SubDetIter), 0);
         SubDetHist->GetXaxis()->SetBinLabel(Loop, SID.human_detector().c_str());
         Loop++;
      }

      // fill histogram with subdetectors switched on
      for(vector<SubDetector>::iterator SubDetIter = SubDetOnIdList.begin();
         SubDetIter != SubDetOnIdList.end(); ++SubDetIter)
      {
         eformat::helper::SourceIdentifier SID(static_cast<eformat::SubDetector>(*SubDetIter), 0);
         SubDetHist->Fill(SID.human_detector().c_str(), 1.0, 1.0);
      }

      // fill histogram with subdetectors switched off
      for(vector<SubDetector>::iterator SubDetIter = SubDetOffIdList.begin();
         SubDetIter != SubDetOffIdList.end(); ++SubDetIter)
      {
         eformat::helper::SourceIdentifier SID(static_cast<eformat::SubDetector>(*SubDetIter), 0);
         SubDetHist->Fill(SID.human_detector().c_str(), 0.0, 1.0);
      }

      // fill SubDetROBCountMap
      for(unsigned int i = 0; i != EnabledROBsArray.value().size(); i++)
      {
         eformat::helper::SourceIdentifier SID(EnabledROBsArray.value()[i]);
         SubDetROBCountMap[SID.subdetector_id()]++;
      }

      // fill histogram with subdetectors found in ROBs
      for(vector<SubDetector>::iterator SubDetIter = SubDetAllIdList.begin();
         SubDetIter != SubDetAllIdList.end(); ++SubDetIter)
      {
         eformat::helper::SourceIdentifier SID(static_cast<eformat::SubDetector>(*SubDetIter), 0);
         SubDetHist->Fill(SID.human_detector().c_str(), 2.0, SubDetROBCountMap[*SubDetIter]);
      }

      // final configuration of histogram
      {
        scoped_lock_histogram lock;
        SubDetHist->LabelsDeflate("X");
        SubDetHist->LabelsDeflate("Y");
        SubDetHist->GetYaxis()->LabelsOption("a");
      }
      // register histogram
      regHist(SubDetHist);

      m_SubDetHistFilled = true;
   }
}


// The release metadata file can be found in InstallArea/$CMTCONFIG/ReleaseData
StatusCode TrigOpMoni::readReleaseData(const string& file, map<string,string>& result)
{
  ifstream f;
  f.open(file.c_str(), ifstream::in);
  if ( !f.is_open() ) return StatusCode::FAILURE;
  
  string line;
  while ( getline(f, line) ) {
    // Format: name:value    
    string::size_type loc = line.find(":");
    if ( loc==string::npos || loc==0 ) continue;    
    result[line.substr(0, loc)] = line.substr(loc+1);
  }
  f.close();
  return StatusCode::SUCCESS;
}


void TrigOpMoni::FillReleaseData()
{
  const char* ld_lib_path = getenv("LD_LIBRARY_PATH");
  if (ld_lib_path==0) {
    ATH_MSG_WARNING("LD_LIBRARY_PATH is not defined. Will not fill release histogram." );
    return;
  }

  // Find all release metadata files
  list<DirSearchPath::path> file_list = DirSearchPath(ld_lib_path, ":").find_all(m_releaseData);

  if ( file_list.empty() ) {
    ATH_MSG_WARNING("Could not find release metadata file " << m_releaseData      
                    << " in LD_LIBRARY_PATH" );
    m_generalHist->Fill("Release ?", 1);
    return;
  }

  list<DirSearchPath::path>::const_iterator fit = file_list.begin();  
  size_t n = 1;
  for (; fit!=file_list.end(); ++fit, ++n) {
    if ( n > 2 ) break;   // Only check first two metadata files (typically AtlasP1HLT and AtlasHLT)

    // Read metadata file
    map<string, string> result;
    if ( readReleaseData(fit->string(), result).isFailure() ) {
      ATH_MSG_WARNING("Could not read release metadata from " << *fit);
      m_generalHist->Fill("Release ?", 1);    
      return;
    }

    // Check format
    if ( result.find("project name")==result.end() ||
	 result.find("release")==result.end() ) {
      ATH_MSG_WARNING("Invalid release metadata format in " << *fit );
      m_generalHist->Fill("Release ?", 1);    
      return;
    }
    
    m_generalHist->Fill((result["project name"]+" "+result["release"]).c_str(), 1);
  }
}
