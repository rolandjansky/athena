/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**
* @file SCTCalib.cxx
*
* Find noisy strips, HV trips and dead strips/chips
* Upload NoiseOccupancy and BSErrors from monitoring histograms
*
* @author Jose E. Garcia,  jose.enrique.garcia@cern.ch
* @author Peter Vankov,    peter.vankov@cern.ch
* @author Kazu Hanagaki,   kazunori.hanagaki@cern.ch
* @author Minoru Hirose,   minoru.hirose@cern.ch
* @author Tim Andeen,      timothy.robert.andeen@cern.ch
* @author Junji Tojo,      junji.tojo@cern.ch
* @author Peter Rosendahl, peter.lundgaard.rosendahl@cern.ch
**/

#include "SCT_CalibAlgs/SCTCalib.h"
#include "SCT_CalibAlgs/SCT_LorentzAngleFunc.h"
#include "SCT_CalibUtilities.h"
#include "SCT_CalibBsErrorTool.h"
#include "SCT_CalibLbTool.h"
#include "SCT_CalibModuleListTool.h"
#include "SCT_CalibEventInfo.h"
#include "SCT_CalibHitmapTool.h"

#include "XmlHeader.h"
#include "XmlStreamer.h"

//InnerDetector
#include "InDetReadoutGeometry/SiDetectorElement.h"

//infrastructure
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

//Gaudi
#include "GaudiKernel/IEventProcessor.h"

//root
#include "TFile.h"
#include "TH1I.h"
#include "TH2D.h"
#include "TF1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TMath.h"
#include "Math/ProbFuncMathCore.h"

//STL, boost
#include <boost/lexical_cast.hpp>

using namespace SCT_CalibAlgs;
using namespace std;

namespace {
enum Bec { ENDCAP_C = -2, BARREL = 0, ENDCAP_A = 2 };
// String names for the detector parts
const std::string detectorNames[] = { "negativeEndcap", "Barrel", "positiveEndcap" };
// String names for the detector parts
const std::string shortNames[] = { "EndCapC", "Barrel", "EndCapA" };
// Path names to become part of the histogram paths
const std::string detectorPaths[] = { "SCTEC", "SCTB", "SCTEA" };

bool areConsecutiveIntervals(const std::pair<int, int> &i1, const std::pair<int, int> &i2, const int withinLimits) {
   return i1.second <= (i2.first + withinLimits);
}
/**
void
initQueue(std::queue<int> & q, const int numvals, const int setval){
  for (int i(0);i<numvals;++i) q.push(setval);
}
**/
const std::string xmlHeader("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
const std::string linefeed("\n");
std::string
associateStylesheet(const std::string &stylesheetName) {
   return std::string("<?xml-stylesheet type=\"text/xsl\" href=\"")+stylesheetName+"\"?>";
}
template <class T>
std::string
xmlPartData(const Bec bec, const int layer, const int eta, const std::string & dataName, const T data) {
   //agrohsje added spaces for proper output format
   ostringstream os;
   const std::string thisPart=shortNames[bec2Index(bec)];
   os<<"    <parts>"<<endl
     <<"    "<<xmlValue("part",thisPart)<<endl
     <<"    "<<xmlValue("layer", layer)<<endl
     <<"    ";
   std::string barrelEtaXml=xmlValue("eta", "all");
   std::string endcapEtaXml=xmlValue("eta",eta);
   if (bec==BARREL) os<<barrelEtaXml;
   else            os<<endcapEtaXml;
   os<<endl
     <<"    "<<xmlValue(dataName, data)<<endl
     <<"    </parts>"<<endl;
   return os.str();
}

template <class T>
std::string
xmlModuleData(const Bec bec, const int layer, const int side, const int phi, const int eta, const std::string & dataName, const T data, const std::string & serial,const std::string & listOfErrors) {
   //xmlModuleData(const Bec bec, const int layer, const int side, const int phi, const int eta, const std::string & dataName, const T data, const std::string & serial, const ostringstream listOfErrors){
   //agasconb added to generate BSErrors file for individual modules
   ostringstream os;
   os<<"    <module>"<<endl
     <<"    "<<xmlValue("SN", serial)<<endl;

   if (bec==ENDCAP_C) os<<"    "<<xmlValue("barrel_endcap","-2")<<endl;
   else if (bec==BARREL) os<<"    "<<xmlValue("barrel_endcap","0")<<endl;
   else if (bec==ENDCAP_A) os<<"    "<<xmlValue("barrel_endcap","2")<<endl;
   os<<"    "<<xmlValue("layer", layer)<<endl
     <<"    "<<xmlValue("side", side)<<endl
     <<"    "<<xmlValue("eta", eta)<<endl
     <<"    "<<xmlValue("phi", phi)<<endl
     <<"    "<<xmlValue(dataName, data)<<endl;
   os<<listOfErrors;
   os<<"    </module>"<<endl;
   return os.str();
}

std::string
normalizeList( const std::string& strList ) {
   std::string str = strList;
   if ( !str.empty() ) {
      //--- Remove 1st & last space
      str = str.substr( 1, str.length()-2 );
      //--- Replace double space to single space
      std::string strSingleSpace = " ";
      std::string strDoubleSpace = "  ";
      std::string::size_type pos = 0;
      while ( pos =str.find( strDoubleSpace, pos), pos != std::string::npos ) {
         str.erase( pos, strSingleSpace.length() );
         pos += strSingleSpace.length();
      }
   }
   return str;
}
}

//////////////////////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////////////////////

SCTCalib::SCTCalib( const std::string& name, ISvcLocator* pSvcLocator ) :
   AthAlgorithm( name, pSvcLocator ),
   m_pSCTHelper(nullptr),
   m_numOfLBsProcessed(0),
   m_numberOfEvents(0),
   m_numberOfEventsHist(0),
   m_LBRange(-999),
   m_inputHist(nullptr)
{
   m_readHIST = m_doNoiseOccupancy || m_doRawOccupancy || m_doEfficiency || m_doBSErrorDB || m_doLorentzAngle;
}

////////////////////////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalib::initialize() {
   if ( detStore()->retrieve( m_pSCTHelper, "SCT_ID").isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve SCTHelper");
      return StatusCode::FAILURE;
   }

   if ( not retrievedService(m_pCalibWriteTool)) return StatusCode::FAILURE;
   if ( m_doHV) ATH_MSG_FATAL("Not yet properly implemented and tested!");

   ATH_CHECK(m_ConfigurationConditionsTool.retrieve( EnableTool {m_useConfiguration} ) );

   if ( !m_useCalibration ) {
      ATH_MSG_DEBUG( "ReadCalibDataTool was removed in initialization" );
      m_ReadCalibDataTool.disable();
   } else {
      if (m_ReadCalibDataTool.retrieve().isFailure()) return StatusCode::FAILURE;
   }

   if ( !m_useMajority ) {
      ATH_MSG_DEBUG( "MajorityConditionsTool was removed in initialization" );
   } else {
      if (m_MajorityConditionsTool.retrieve().isFailure()) return StatusCode::FAILURE;
   }

   if ( not retrievedService(m_calibHitmapTool)) return StatusCode::FAILURE;

   if ( not retrievedService(m_calibModuleListTool)) return StatusCode::FAILURE;

   if ( not retrievedService(m_calibEvtInfoTool)) return StatusCode::FAILURE;

   if ( !m_useBSError ) {
      ATH_MSG_DEBUG( "ByteStreamErrorsSvc was removed in initialization" );
   } else {
      if ( not retrievedService(m_calibBsErrTool)) return StatusCode::FAILURE;
   }
   if ( not retrievedService(m_calibLbTool) ) return StatusCode::FAILURE;

   ATH_CHECK(m_CablingTool.retrieve());

   //--- LB range
   try {
      m_LBRange = std::stoi( m_LBMax );
      ISCT_CalibHistoTool::setNumberOfLb(m_LBRange);
   } catch (...) {
      ATH_MSG_ERROR( "Couldn't cast m_LBMax=\""<< m_LBMax <<"\" to m_LBRange...");
      m_LBRange = 0;
   }

   ISCT_CalibHistoTool::setLbToMerge(m_nLbsMerged);


   m_readHIST = m_doNoiseOccupancy || m_doRawOccupancy || m_doEfficiency || m_doBSErrorDB || m_doLorentzAngle;

   if ( m_readBS  && m_readHIST ) {
      ATH_MSG_ERROR( "Both BS and HIST are set to be read. Choose either of BS or HIST." );
      return StatusCode::FAILURE;
   }

   //--- Open BS
   if ( m_readBS ) {
      ATH_MSG_INFO( "------------> Reading from ByteStream <-------------");
      m_calibEvtInfoTool->setSource("BS");
      ATH_CHECK(m_eventInfoKey.initialize());
   }


   //--- Open HIST
   if ( m_readHIST ) {
      ATH_MSG_INFO( "------------> Reading from HIST <-------------" );
      m_calibEvtInfoTool->setSource("HIST");
      //--- List of HIST
      std::string hist("");
      std::vector<std::string> histCollection = m_input_hist.value();
      ATH_MSG_INFO( "The input histogram name is : " << m_input_hist );
      if (not histCollection.empty()) {
         hist=histCollection.back();
         if (histCollection.size() > 1) ATH_MSG_WARNING("The Histogram collection contains more than one histogram; using the last one.");
      } else {
         ATH_MSG_ERROR("The input histogram collection property is empty");
         return StatusCode::FAILURE;
      }

      //in case of DeadStrip or DeadChip, change open from EOS to
      //copy and open locally. Delete the file after processing
      m_inputHist = TFile::Open( hist.c_str() );

      ATH_MSG_INFO( "opening HIST file : " << hist.c_str() );

      if (m_inputHist) {
         //--- Check run number
         const std::string os=std::to_string(m_runNumber.value());
         ATH_MSG_INFO( "Getting HIST directory : " << m_runNumber.value() );
         if ( not m_inputHist->GetDirectory( "/run_"+TString(os) ) ) {
           ATH_MSG_ERROR("RunNumber in HIST is inconsistent with jobO : " << os);
           return StatusCode::FAILURE ;
         }
         ATH_MSG_INFO( "Getting Number of events: " << m_calibEvtInfoTool->counter() );
         //--- Read number of events : Get an entry of "tier0ESD" in "/GLOBAL/DQTDataFlow/events_lb"
         std::string osHist= std::string( "/run_") + std::to_string(m_runNumber.value())+"/GLOBAL/DQTDataFlow/events_lb";
         TH1I* hist_events = dynamic_cast<TH1I*>(m_inputHist->Get( osHist.c_str() ));
         m_numberOfEventsHist = hist_events->GetBinContent( 6 ); // Entry in "tier0ESD"
         if ( hist_events->GetBinContent( 4 ) > m_numberOfEventsHist) m_numberOfEventsHist = hist_events->GetBinContent( 4 ); // Entry in "tier0"
         //m_numberOfEventsHist = 13902;
      } else {
         ATH_MSG_WARNING( "can not open HIST : " << hist.c_str() );
      }


      ATH_MSG_INFO( "Initialization of TimeStamp/LB, taken from runInfo.txt" );
      //--- Initialization of TimeStamp/LB, taken from runInfo.txt
      m_calibEvtInfoTool->setSource("HIST");
      m_calibEvtInfoTool->setTimeStamp(m_runStartTime,m_runEndTime);
      m_calibEvtInfoTool->setRunNumber(m_runNumber);
   }

   //--- Booking histograms for hitmaps
   if ( m_doHitMaps ) m_calibHitmapTool->book();
   //--- Reading histograms for hitmaps
   if ( ( not m_doHitMaps && not m_doNoisyLB) && m_readHitMaps) {
      m_calibEvtInfoTool->setSource("HIST");
      m_calibHitmapTool->read("./SCTHitMaps.root");
      m_numberOfEventsHist = m_calibHitmapTool->size();
      m_calibEvtInfoTool->setTimeStamp(m_runStartTime,m_runEndTime);
      m_calibEvtInfoTool->setRunNumber(m_runNumber);
      m_calibLbTool->read("./SCTLB.root");
   }
   //--- Booking histograms for BSErrors
   if ( m_doBSErrors ) m_calibBsErrTool->book();
   //--- Hit-vs-LB for LBs in noisy links/chips
   if ( m_doNoisyLB ) m_calibLbTool->book();

   //--- Check statistics for NoiseOccupancy
   if ( m_doNoiseOccupancy and notEnoughStatistics(m_noiseOccupancyMinStat, m_numberOfEventsHist)) return StatusCode::FAILURE;
   //--- Check statistics for RawOccupancy
   if ( m_doRawOccupancy and notEnoughStatistics(m_rawOccupancyMinStat,m_numberOfEventsHist)) return StatusCode::FAILURE;
   //--- Check statistics for Efficiency
   if ( m_doEfficiency and notEnoughStatistics(m_efficiencyMinStat,m_numberOfEventsHist)) return StatusCode::FAILURE;
   //--- Check statistics for BSError
   if ( m_doBSErrorDB and notEnoughStatistics(m_BSErrorDBMinStat, m_numberOfEventsHist )) return StatusCode::FAILURE;
   //--- Check statistics for LorentzAngle
   if ( m_doLorentzAngle and notEnoughStatistics(m_LorentzAngleMinStat, m_numberOfEventsHist)) return StatusCode::FAILURE;
   //
   return StatusCode::SUCCESS;
}

bool
SCTCalib::notEnoughStatistics(const int required, const int obtained, const std::string & histogramName) const {
   if (obtained<required) {
      ATH_MSG_ERROR("Number of events in "<<histogramName<<": "<<obtained<<" is less than the required minimum number of events " <<required);
      return true;
   }
   return false;
}

///////////////////////////////////////////////////////////////////////////////////
/// beginRun - process after initialize() and  before execute()
///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::beginRun() {
   ATH_MSG_INFO ("----- in beginRun() -----" );
   //--- Check if calibration data is available or not
   return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
// Execute - on event by event
//////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::execute() {
   if ( m_readBS ) {
      SG::ReadHandle<EventInfo> evt(m_eventInfoKey);
      if (not evt.isValid()) {
         ATH_MSG_FATAL("Unable to get the EventInfo");
         return StatusCode::FAILURE;
      }
      const EventInfo* evt_ptr = &(*evt);
      ATH_MSG_VERBOSE(SCT_CalibAlgs::eventInfoAsString(evt_ptr));
      //--- TimeStamp/LB range analyzed
      const int timeStamp = evt->event_ID()->time_stamp();
      const int lumiBlock = evt->event_ID()->lumi_block();
      int timeStampBeginOld;
      int timeStampEndOld;
      m_calibEvtInfoTool->getTimeStamps(timeStampBeginOld, timeStampEndOld);
      m_calibEvtInfoTool->setTimeStamp(std::min(timeStamp, timeStampBeginOld), std::max(timeStamp, timeStampEndOld));
      int lbBeginOld;
      int lbEndOld;
      m_calibEvtInfoTool->getLumiBlock(lbBeginOld, lbEndOld);
      m_calibEvtInfoTool->setLumiBlock(std::min(lumiBlock, lbBeginOld), std::max(lumiBlock, lbEndOld));
      m_calibEvtInfoTool->setLumiBlock(lumiBlock);
      m_calibEvtInfoTool->setTimeStamp(timeStamp);
   }

   const bool majorityIsGoodOrUnused=( m_useMajority and m_MajorityConditionsTool->isGood() ) or !m_useMajority;
   //--- Fill histograms for (1) Number of events and (2) Hitmaps
   if ( m_doHitMaps  and majorityIsGoodOrUnused ) m_calibHitmapTool->fill(m_readBS);

   //--- Fill histograms for (1) Number of events and (2) Hits as a function of LB
   if ( m_doNoisyLB and majorityIsGoodOrUnused ) m_calibLbTool->fill(m_readBS);

   //--- Fill histograms for (1) Number of events and (2) BSErrors
   if ( m_doBSErrors ) m_calibBsErrTool->fill(m_readBS);

   //--- Increment event counter : to be ready for the next event loop
   m_calibEvtInfoTool->incrementCounter();

   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
/// endRun - process results accumulated in execute()
///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::endRun() {
   ATH_MSG_INFO( "----- in endRun() ----- " );
   //--- Number of events processed
   m_numberOfEvents = (m_readHIST || (!m_doHitMaps && m_readHitMaps)) ? m_numberOfEventsHist : m_calibEvtInfoTool->counter();
   m_calibEvtInfoTool->getTimeStamps(m_utcBegin,m_utcEnd);


   //  if ( m_doNoisyLB ) m_calibLbTool->binHistograms(m_nLbsMerged);

   //--- IOV range defined by RunNumber and LB
   unsigned int beginRun = static_cast<unsigned int>(m_runNumber.value());
   unsigned int endRun   = static_cast<unsigned int>(m_runNumber.value());
   unsigned int beginLB  = IOVTime::MINEVENT;
   unsigned int endLB    = IOVTime::MAXEVENT;
   m_iovStart.setRunEvent( static_cast<unsigned long>(beginRun), static_cast<unsigned long>(beginLB) );
   m_iovStop.setRunEvent( static_cast<unsigned long>(endRun), static_cast<unsigned long>(endLB) );

   //--- Find noisy strips from hitmaps
   const bool doNoisyStripAnalysis = ( ( !m_doHitMaps && m_readHitMaps ) || !m_readHitMaps ) && m_doNoisyStrip;
   if ( doNoisyStripAnalysis and getNoisyStrip().isFailure() ) {
      ATH_MSG_ERROR("Failed to run getNoisyStrip()");
      return StatusCode::FAILURE;
   }

   //--- Upload hv
   if ( m_doHV ) {
      m_gofile.open(m_badModulesFile.value().c_str(),std::ios::out);
      if (not m_gofile) ATH_MSG_ERROR("Problem opening "<<m_badModulesFile);
      //
      XmlHeader myXml(m_gofile);
      XmlStreamer root("modules", m_gofile);
      SCT_ID::const_id_iterator waferItr  = m_pSCTHelper->wafer_begin();
      SCT_ID::const_id_iterator waferItrE = m_pSCTHelper->wafer_end();
      const unsigned int onlyDummy(1);
      pair<int, int> timeInterval(0,0);
      pair<int, int> lbRange(0,0);
      const int withinLimits(m_maxtbins);
      //
      for ( ; waferItr not_eq waferItrE; ++waferItr ) {
         const Identifier waferId=*waferItr ;
         IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
         const vector< pair<int,int> > & tvec= m_summarytrips.at(waferHash.value());
         const vector< pair<int,int> > & tlbn= m_summarytripslb.at(waferHash.value());
         //tvec is a pair of times in general, although the very first one is a dummy
         const unsigned int numberOfElements=tvec.size();
         if (numberOfElements > onlyDummy) {
            //only care if something happened in this module
            timeInterval=tvec.at(1);
            lbRange=tlbn.at(1);
            for (unsigned int itrip(2); itrip != numberOfElements; ++itrip) { //skip 0 since that is just the dummy pair.
               if (areConsecutiveIntervals(tvec[itrip],timeInterval,withinLimits)) {
                  timeInterval.second  =  tvec.at(itrip).second;
                  lbRange.second = tlbn.at(itrip).second;
               } else {
                  //not consecutive, so first print out the old one
                  doHVPrintXML(timeInterval,lbRange,waferId);
                  timeInterval =  tvec.at(itrip);
                  lbRange =  tlbn.at(itrip);
               }
            } // end loop over times
            doHVPrintXML(timeInterval,lbRange,waferId );
         }
      } // end loop over wafers
   }

   //--- Find dead strips/chips from hitmaps
   if ( (m_doDeadStrip or m_doDeadChip ) and getDeadStrip().isFailure() ) {
      ATH_MSG_ERROR("Failed to run getDeadStrip()");
      return StatusCode::FAILURE;
   }

   //--- Upload noise occupancy
   if ( m_doNoiseOccupancy  and getNoiseOccupancy().isFailure() ) {
      ATH_MSG_ERROR("Failed to run getNoiseOccupancy()");
      return StatusCode::FAILURE;
   }

   //--- Upload raw occupancy
   if ( m_doRawOccupancy and getRawOccupancy().isFailure() ) {
      ATH_MSG_ERROR("Failed to run getRawOccupancy()");
      return StatusCode::FAILURE;
   }

   //--- Upload efficiency
   if ( m_doEfficiency and getEfficiency().isFailure() ) {
      ATH_MSG_ERROR("Failed to run getEfficiency()");
      return StatusCode::FAILURE;
   }

   //--- Upload ByteStream Errors
   if ( m_doBSErrorDB and getBSErrors().isFailure() ) {
      ATH_MSG_ERROR("Failed to run getBSErrors()");
      return StatusCode::FAILURE;
   }

   //--- Upload Lorentz Angle
   if ( m_doLorentzAngle  and getLorentzAngle().isFailure() ) {
      ATH_MSG_ERROR("Failed to run getLorentzAngle()");
      return StatusCode::FAILURE;
   }

   //--- Close HIST
   if ( m_readHIST ) m_inputHist->Close();
   return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap
///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::finalize() {
   ATH_MSG_INFO("----- in finalize() ----- ");

   ATH_MSG_INFO("Thank-you for using SCT_CalibAlgs, version "<<PACKAGE_VERSION);
   return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////
/// doHVPrintXML()
/// Prints XML file for hv modules
///////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalib::doHVPrintXML(const std::pair<int, int> & timeInterval, const std::pair<int,int> & lbRange, Identifier waferId) {
   const IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
   const SCT_SerialNumber sn        = m_CablingTool->getSerialNumberFromHash( waferHash );

   XmlStreamer mod("module", m_gofile);
   {
      XmlStreamer v("value","name","SN", m_gofile);
      m_gofile<<sn.str();
   }
   {
      XmlStreamer v("value", "name", "BecLayerPhiEta", m_gofile);
      m_gofile<< formatPosition(waferId, m_pSCTHelper,".",false);
   }
   {
      XmlStreamer v("value","name", "StartTime", m_gofile);
      m_gofile<<timeInterval.first;
   }
   {
      XmlStreamer v("value","name","EndTime",m_gofile);
      m_gofile<< timeInterval.second;
   }
   {
      XmlStreamer v("value","name", "StartLBN", m_gofile);
      m_gofile<<lbRange.first;
   }
   {
      XmlStreamer v("value","name","EndLBN",m_gofile);
      m_gofile<< lbRange.second;
   }
   return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
/// getNoisyStrip()
/// Find noisy strips from hitmaps and write out into xml/db formats
///////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalib::getNoisyStrip() {
   enum Categories {ALL, NEW, REF, N_CATEGORIES};
   //--- Check statistics
   //ATH_MSG_INFO(m_calibEvtInfoTool->counter() << "   " <<m_calibHitmapTool->size());
   //int numOfEventsProcessed=m_calibEvtInfoTool->counter();
   //sroe: This looks like a bug, so I change the code here

   // if ( noisyStripsToSummaryXmlFake( m_badStripsSummaryFile ).isFailure() ) {
   //   ATH_MSG_ERROR("Could not write XML file");
   //   return StatusCode::FAILURE;
   // }

   ATH_MSG_INFO( "----- in getNoisyStrip() ----- " );

   //--- Number of LBs processed
   m_numOfLBsProcessed = 0;
   for ( int iLB = 0; iLB != m_LBRange; ++iLB ) {
      if ( m_calibLbTool and m_calibLbTool->getNumberOfEventsInBin( iLB + 1 ) > 0 ) ++m_numOfLBsProcessed;
   }

   //--- Choice of threshold
   //three module lists: all, new, ref
   typedef std::map< Identifier, std::set<Identifier> > ModuleList_t;
   ModuleList_t moduleLists[N_CATEGORIES];
   //Reading data from COOL
   // original code switched on this :if (m_noisyUpdate)
   if (m_calibModuleListTool->readModuleList(moduleLists[REF]).isFailure()) {
      ATH_MSG_ERROR("Could not read moduleList");
      return StatusCode::FAILURE;
   }

   //two bad strip lists: all, new
   typedef std::set<Identifier> StripList_t;
   StripList_t stripIdLists[2];

   //--- Loop over wafers
   SCT_ID::const_id_iterator waferItr  = m_pSCTHelper->wafer_begin();
   SCT_ID::const_id_iterator waferItrE = m_pSCTHelper->wafer_end();
   int numNoisyWafers(0);
   for ( ; waferItr not_eq waferItrE; ++waferItr ) {
      //--- Identifier/SN
      Identifier       waferId   = *waferItr;
      Identifier       moduleId  = m_pSCTHelper->module_id( waferId );
      //--- Initialization in *module*
      if ( m_pSCTHelper->side( waferId ) == 0 ) {
         stripIdLists[ALL].clear();
         stripIdLists[NEW].clear();
      }
      std::pair<int, bool> noisy=getNumNoisyStrips(waferId);
      const int  numNoisyStripsInWafer=noisy.first;
      const bool isNoisyWafer=noisy.second;
      if (numNoisyStripsInWafer!=0) {
         if ( m_noisyWaferFinder && isNoisyWafer ) { //in noisy wafer
            ++numNoisyWafers;
            if (not m_noisyWaferWrite) break;
            if (m_noisyWaferAllStrips) { //write out all strips
               if ( addStripsToList( waferId, stripIdLists[ALL], false, false ).isFailure() or  addStripsToList( waferId, stripIdLists[NEW], false, true ).isFailure() ) {
                  ATH_MSG_ERROR("Could not add stripIds to the list");
                  return StatusCode::FAILURE;
               }
               break;
            } else {
               //only noisy strips in noisy wafer
               if ( addStripsToList( waferId, stripIdLists[ALL], true, false ).isFailure() or addStripsToList( waferId, stripIdLists[NEW], true, true  ).isFailure() ) {
                  ATH_MSG_ERROR("Could not add stripIds to the list");
                  return StatusCode::FAILURE;
               }
            }
         } else { // not in noisy wafer
            if ( addStripsToList( waferId, stripIdLists[ALL], true, false ).isFailure() or addStripsToList( waferId, stripIdLists[NEW], true, true  ).isFailure() ) {
               ATH_MSG_ERROR("Could not add stripIds to the list");
               return StatusCode::FAILURE;
            }
         }
      }//endif numnoisystrips!=0
      //--- Create objects for a module
      if ( m_pSCTHelper->side( waferId ) == 1 ) {
         if ( !stripIdLists[ALL].empty() ) moduleLists[ALL].insert( map< Identifier, std::set<Identifier> >::value_type( moduleId, stripIdLists[ALL] ) );
         if ( !stripIdLists[NEW].empty() ) moduleLists[NEW].insert( map< Identifier, std::set<Identifier> >::value_type( moduleId, stripIdLists[NEW] ) );
      }
   }//end loop over wafers

   //--- Local sqlite files here
   ATH_MSG_DEBUG( "------ Before writing into COOL ------" );
   if ( m_writeToCool ) {
      if ( writeModuleListToCool( moduleLists[ALL], moduleLists[NEW], moduleLists[REF] ).isFailure() ) {
         ATH_MSG_ERROR("Could not write NoisyStrips into COOL");
         return StatusCode::FAILURE;
      }
   }
   //--- XML outputs
   if ( noisyStripsToXml( moduleLists[ALL], m_badStripsAllFile ).isFailure() ) {
      ATH_MSG_ERROR("Could not write XML file");
      return StatusCode::FAILURE;
   }
   if ( noisyStripsToXml( moduleLists[NEW], m_badStripsNewFile ).isFailure() ) {
      ATH_MSG_ERROR("Could not write XML file");
      return StatusCode::FAILURE;
   }
   if ( noisyStripsToSummaryXml( moduleLists[ALL], moduleLists[NEW], moduleLists[REF], m_badStripsSummaryFile ).isFailure() ) {
      ATH_MSG_ERROR("Could not write XML file");
      return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;

}


//====================================================================================================
//                           SCTCalib :: getDeadStrip
//====================================================================================================
StatusCode SCTCalib::getDeadStrip() {
   //Function to identify and print out the dead strips.
   ATH_MSG_INFO( "getDeadStrip() called" );

   // Bad Mods
   const std::set<Identifier>* badMods = m_ConfigurationConditionsTool->badModules();
   std::set<Identifier>::const_iterator ModItr(badMods->begin());
   std::set<Identifier>::const_iterator ModEnd(badMods->end());
   // Bad links
   const std::map<IdentifierHash, std::pair<bool, bool> >* badLinks = m_ConfigurationConditionsTool->badLinks();
   std::map<IdentifierHash, std::pair<bool, bool> >::const_iterator linkItr(badLinks->begin());
   std::map<IdentifierHash, std::pair<bool, bool> >::const_iterator linkEnd(badLinks->end());
   // Bad chips
   const std::map<Identifier, unsigned int>* badChips = m_ConfigurationConditionsTool->badChips();
   std::map<Identifier, unsigned int>::const_iterator chipItr(badChips->begin());
   std::map<Identifier, unsigned int>::const_iterator chipEnd(badChips->end());
   // Bad strips (w/o bad modules and chips)
   std::set<Identifier> badStripsExclusive;
   m_ConfigurationConditionsTool->badStrips(badStripsExclusive, true, true);
   //std::set<Identifier>::const_iterator stripItr(badStripsExclusive.begin());
   std::set<Identifier>::const_iterator stripEnd(badStripsExclusive.end());
   //To get #(Enabled Modules)
   int numEnabledModules_B[n_barrels]= {n_phiBinsB0*n_etaInBarrel,n_phiBinsB1*n_etaInBarrel,n_phiBinsB2*n_etaInBarrel,n_phiBinsB3*n_etaInBarrel};
   int numEnabledModules_EC[n_disks][n_etaBinsEC]= { {0}, {0} };
   for (int i=0; i<n_disks; i++) {
      for (int j=0; j<n_etaBinsEC; j++) {
         if (!((i==0&&j==2)||(i==6&&j==2)||(i==7&&j==2)||(i==8&&j==1)||(i==8&&j==2))) {
            numEnabledModules_EC[i][j] = j==0 ? n_phiBinsECOuter*2 : n_phiBinsECMiddle*2;
         }
      }
   }
   for (; ModItr!=ModEnd; ++ModItr) {
      Identifier moduleId = *ModItr;
      if (m_pSCTHelper->barrel_ec(moduleId)==BARREL) numEnabledModules_B[m_pSCTHelper->layer_disk(moduleId)]--;
      else numEnabledModules_EC[m_pSCTHelper->layer_disk(moduleId)][m_pSCTHelper->eta_module(moduleId)]--;
   }
   //calculate meanOccupancy of layer etc...
   double meanOccupancy_Barrel[n_barrels]= {0};
   double meanOccupancy_EC[n_disks][n_etaBinsEC]= { {0}, {0} };
   SCT_ID::const_id_iterator waferItr  = m_pSCTHelper->wafer_begin();
   SCT_ID::const_id_iterator waferItrE = m_pSCTHelper->wafer_end();
   for ( ; waferItr != waferItrE; ++waferItr ) {
      Identifier     waferId   = *waferItr;
      //    Identifier     moduleId  = m_pSCTHelper->module_id(waferId);
      IdentifierHash waferHash = m_pSCTHelper->wafer_hash(waferId);
      for (int j=0; j<n_stripPerChip*n_chipPerSide; j++) {
         double n_hits = m_calibHitmapTool->getBinForHistogramIndex( j+1, waferHash.value());
         if (n_hits/m_numberOfEvents<m_noisyThr4DeadFinding) {
            if (m_pSCTHelper->barrel_ec(waferId)==BARREL) {
               meanOccupancy_Barrel[m_pSCTHelper->layer_disk(waferId)]+=m_calibHitmapTool->getBinForHistogramIndex( j+1, waferHash.value());
            } else {
               meanOccupancy_EC[m_pSCTHelper->layer_disk(waferId)][m_pSCTHelper->eta_module(waferId)]+=m_calibHitmapTool->getBinForHistogramIndex( j+1, waferHash.value());
            }
         }
      }
   }

   for (int i=0; i<n_barrels; i++) {
      meanOccupancy_Barrel[i]/=static_cast<double>(m_numberOfEvents*nbins*2*numEnabledModules_B[i]);
      ATH_MSG_INFO("Barrel : layer="<<i<<", meanOccupancy="<<meanOccupancy_Barrel[i]<<", #enabledModule="<<numEnabledModules_B[i]);
   }

   for (int i=0; i<n_disks; i++) {
      for (int j=0; j<n_etaBinsEC; j++) {
         if (numEnabledModules_EC[i][j]!=0) {
            meanOccupancy_EC[i][j]/=static_cast<double>(m_numberOfEvents*nbins*2*numEnabledModules_EC[i][j]);
            ATH_MSG_INFO("EndCap : disk="<<i<<", eta="<<j<<", meanOccupancy="<<meanOccupancy_EC[i][j]<<", #enabledModule="<<numEnabledModules_EC[i][j]);
         }
      }
   }
   bool busyStream = meanOccupancy_Barrel[3]>m_busyThr4DeadFinding ? true : false;
   unsigned int minStat = busyStream ? static_cast<unsigned int>(m_deadStripMinStatBusy) : static_cast<unsigned int>(m_deadStripMinStat);
   if (m_doDeadStrip && m_numberOfEvents<minStat) {
      ATH_MSG_WARNING("required minimum statistics is "<<minStat/1E3<<"k events for DeadStrip search with this stream");
      m_doDeadStrip = false;
   }
   if (m_doDeadChip && m_numberOfEvents<m_deadChipMinStat) {
     ATH_MSG_WARNING("required minimum statistics is "<<static_cast<unsigned int>(m_deadChipMinStat)<<" events for DeadChip search");
      m_doDeadChip = false;
   }
   if (m_doDeadStrip==false && m_doDeadChip==false) {
      ATH_MSG_ERROR("Number of events "  << m_numberOfEvents<< " is less than the required minimum number of events... exit getDeadStrip()");
      return StatusCode::FAILURE;
   }
   //create XML files
   if (m_doDeadStrip) {
      if (openXML4DB(m_outDeadStrips, "DeadStrip", m_tagID4DeadStrips.value().c_str(), m_iovStart, m_iovStop).isFailure()) {
         ATH_MSG_ERROR("Problem opening "<<m_deadStripsFile);
         return StatusCode::FAILURE;
      }
   }
   if (m_doDeadChip) {
      if (openXML4DB(m_outDeadChips, "DeadChip", m_tagID4DeadChips.value().c_str(), m_iovStart, m_iovStop).isFailure()) {
         ATH_MSG_ERROR("Problem opening "<<m_deadChipsFile);
         return StatusCode::FAILURE;
      }
   }

   // Get SCT_DetectorElementCollection
   SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
   const InDetDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
   if (elements==nullptr) {
      ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
      return StatusCode::FAILURE;
   }

   //Dead identification
   bool hasDeadStrip=false;
   bool hasDeadChip=false;
   bool isNoHitLink=false;
   bool isDead=false;
   bool beforeIsDead=false;
   int n_deadStrip=0;
   int n_deadChip=0;
   int n_deadLink=0;
   int n_deadModule=0;
   int n_checkedChip=0;
   int beginDead=0;
   int endDead=0;
   std::string defectStrip;
   std::string defectChip;
   std::ostringstream summaryList;
   defectStrip.erase();
   defectChip.erase();
   const double deadStripDefinition = ROOT::Math::gaussian_cdf_c(m_deadStripSignificance);
   const double deadChipDefinition = ROOT::Math::gaussian_cdf_c(m_deadChipSignificance);


   //--- Loop over wafers
   waferItr = m_pSCTHelper->wafer_begin();
   for ( ; waferItr != waferItrE; ++waferItr ) {
      Identifier     waferId   = *waferItr;
      Identifier     moduleId  = m_pSCTHelper->module_id(waferId);
      IdentifierHash waferHash = m_pSCTHelper->wafer_hash(waferId);

      bool disabledChip[n_chipPerModule]= {false};
      unsigned int disabledChipFlag=0;
      double numHitsInStrip[n_stripPerChip*n_chipPerSide]= {0};
      double numHitsInChip[n_chipPerSide]= {0};
      double totalHitsInWafer=0;
      int n_noisyStrip=0;
      int n_noHitsStrip=0;
      int n_disabledStrip=0;
      int n_disabledInChip[n_chipPerSide]= {0};

      //initialize
      int side = m_pSCTHelper->side(waferId);
      if (side==0) {
         isDead=false;
         beforeIsDead=false;
         beginDead=0;
         endDead=0;
         defectStrip.erase();
         defectChip.erase();
      }

      //check if module/link is disabled or not
      bool disabled=false;
      if (badMods->find(moduleId)!=badMods->end()) disabled=true;
      linkItr=badLinks->find(waferHash);
      if (linkItr!=linkEnd) {
         std::pair<bool, bool> status = (*linkItr).second;
         if ((side==0 && status.first==true) || (side==1 && status.second==true)) disabled=true;
      }

      //check BS Error
      bool hasBSError=false;
      if (m_calibBsErrTool->size(waferHash.value())>0) hasBSError=true;
      if (disabled || hasBSError) { //goto WRITE_DB; //<-- who ever put this in should be shot; http://xkcd.com/292/
         if (side==1) {
            //write to DB & .xml.
            if (defectChip==" 0-5  6-11 ") {
               n_deadModule++;
            } else if (defectChip==" 0-5 " || defectChip==" 6-11 ") {
               n_deadLink++;
            }

            if (!(defectStrip.empty()) || !(defectChip.empty())) {
               if (addToSummaryStr(summaryList, waferId, "DEAD", defectStrip.c_str(), defectChip.c_str()).isFailure()) {
                  ATH_MSG_ERROR("Could not add dead strips to the summary");
                  return StatusCode::FAILURE;
               }
            }

            if (!(defectStrip.empty())) {
               if (m_writeToCool) {
                  if (m_pCalibWriteTool->createListStrip(moduleId,m_pSCTHelper,10000,"DEAD",m_deadStripSignificance,defectStrip).isFailure()) {
                     ATH_MSG_ERROR("Could not create list");
                     return StatusCode::FAILURE;
                  }
               }

               if (addToXML4DB(m_outDeadStrips, waferId, "DEAD", m_deadStripSignificance, defectStrip.c_str()).isFailure()) {
                  ATH_MSG_ERROR("Could not add dead strips to the summary");
                  return StatusCode::FAILURE;
               }

               hasDeadStrip=true;
            }

            if (!(defectChip.empty())) {
               if (m_writeToCool) {
                  if ( m_pCalibWriteTool->createListChip(moduleId,m_pSCTHelper,10000,"DEAD",m_deadChipSignificance,defectChip).isFailure()) {
                     ATH_MSG_ERROR("Could not create list");
                     return StatusCode::FAILURE;
                  }
               }

               if (addToXML4DB(m_outDeadChips, waferId, "DEAD", m_deadChipSignificance, defectChip.c_str()).isFailure()) {
                  ATH_MSG_ERROR("Could not add dead chips to the summary");
                  return StatusCode::FAILURE;
                 }

               hasDeadChip=true;

            }
         }
         continue;
      }
      //retrieving info of chip status
      chipItr=badChips->find(moduleId);
      if (chipItr!=chipEnd) disabledChipFlag = (*chipItr).second;
      for (unsigned int i(0); i<n_chipPerModule; i++) {
         disabledChip[i] = ((disabledChipFlag & (1<<i)) != 0);
      }

      //retrieving #hits in each strip
      for (int j=0; j<n_stripPerChip*n_chipPerSide; j++) {
         const InDetDD::SiDetectorElement* pElement = elements->getDetectorElement(waferHash);
         bool swap=(pElement->swapPhiReadoutDirection()) ? true : false;
         int chipNum=0;
         if (side==0) chipNum = swap ? 5-j/n_stripPerChip : j/n_stripPerChip;
         else chipNum = swap ? 11-j/n_stripPerChip : 6+j/n_stripPerChip;
         int stripNum = swap ? 767-j : j;
         Identifier stripId = m_pSCTHelper->strip_id(waferId,j);

         numHitsInStrip[stripNum] = m_calibHitmapTool->getBinForHistogramIndex( j+1, waferHash.value());
         bool misMatch=false;
         double n_hitsInDisable=numHitsInStrip[stripNum];
         if (((disabledChipFlag & (1<<chipNum))!=0) || badStripsExclusive.find(stripId)!=stripEnd) {
            if (numHitsInStrip[stripNum]!=0) misMatch = true;
            numHitsInStrip[stripNum] = -99;
         }
         if (misMatch) {
            ATH_MSG_WARNING("hits in disabled Strip : "
                            <<"n_hits="<<n_hitsInDisable<<", "
                                <<"bec="<<m_pSCTHelper->barrel_ec(stripId)<<", "
                            <<"layer="<<m_pSCTHelper->layer_disk(stripId)<<", "
                            <<"phi="<<m_pSCTHelper->phi_module(stripId)<<", "
                            <<"eta="<<m_pSCTHelper->eta_module(stripId)<<", "
                            <<"side="<<m_pSCTHelper->side(stripId)<<", "
                            <<"strip="<<m_pSCTHelper->strip(stripId));
         }

         if (numHitsInStrip[stripNum]==0) {
            n_noHitsStrip++;
            ATH_MSG_DEBUG("nohit strip : barrel_ec="<<m_pSCTHelper->barrel_ec(stripId)
                          <<", layer="<<m_pSCTHelper->layer_disk(stripId)<<", phi="<<m_pSCTHelper->phi_module(stripId)
                          <<", eta="<<m_pSCTHelper->eta_module(stripId)<<", side="<<m_pSCTHelper->side(stripId)
                          <<", strip=offline"<<m_pSCTHelper->strip(stripId));
         } else if (numHitsInStrip[stripNum]==-99) {
            n_disabledStrip++;
            n_disabledInChip[stripNum/n_stripPerChip]++;
            ATH_MSG_DEBUG("disabled strip : barrel_ec="<<m_pSCTHelper->barrel_ec(stripId)
                          <<", layer="<<m_pSCTHelper->layer_disk(stripId)<<", phi="<<m_pSCTHelper->phi_module(stripId)
                          <<", eta="<<m_pSCTHelper->eta_module(stripId)<<", side="<<m_pSCTHelper->side(stripId)
                          <<", strip=offline"<<m_pSCTHelper->strip(stripId));
         } else if (numHitsInStrip[stripNum]/m_numberOfEvents>m_noisyThr4DeadFinding) {
            n_noisyStrip++;
         } else {
            totalHitsInWafer+=numHitsInStrip[stripNum];
         }

      }//end strip loop

      if (n_disabledStrip==768) {
         if (side==1) {
            //write to DB & .xml.
            if (defectChip==" 0-5  6-11 ") {
               n_deadModule++;
            } else if (defectChip==" 0-5 " || defectChip==" 6-11 ") {
               n_deadLink++;
            }
            if (!(defectStrip.empty()) || !(defectChip.empty())) {
               if (addToSummaryStr(summaryList, waferId, "DEAD", defectStrip.c_str(), defectChip.c_str()).isFailure()) {
                  ATH_MSG_ERROR("Could not add dead strips to the summary");
                  return StatusCode::FAILURE;
               }
            }

            if (!(defectStrip.empty())) {
               if (m_writeToCool) {
                  if (m_pCalibWriteTool->createListStrip(moduleId,m_pSCTHelper,10000,"DEAD",m_deadStripSignificance,defectStrip).isFailure()) {
                     ATH_MSG_ERROR("Could not create strip list");
                     return StatusCode::FAILURE;
                  }
               }

               if (addToXML4DB(m_outDeadStrips, waferId, "DEAD", m_deadStripSignificance, defectStrip.c_str()).isFailure()) {
                  ATH_MSG_ERROR("Could not add xml strip list");
                  return StatusCode::FAILURE;
               }
               hasDeadStrip=true;

            }
            if (!(defectChip.empty())) {
               if (m_writeToCool) {
                  if (m_pCalibWriteTool->createListChip(moduleId,m_pSCTHelper,10000,"DEAD",m_deadChipSignificance,defectChip).isFailure()) {
                     ATH_MSG_ERROR("Could not create strip list");
                     return StatusCode::FAILURE;
                  }
               }

               if (addToXML4DB(m_outDeadChips, waferId, "DEAD", m_deadChipSignificance, defectChip.c_str()).isFailure()) {
                  ATH_MSG_ERROR("Could not add xml chip list");
                  return StatusCode::FAILURE;
               }

               hasDeadChip=true;

            }
         }
         continue;

      }

      isNoHitLink=false;
      if (n_noHitsStrip+n_disabledStrip==768) {
         n_checkedChip+=n_chipPerSide;
         isNoHitLink=true;

         double meanOccu=0;
         if (m_pSCTHelper->barrel_ec(waferId)==BARREL) meanOccu=meanOccupancy_Barrel[m_pSCTHelper->layer_disk(waferId)];
         else meanOccu=meanOccupancy_EC[m_pSCTHelper->layer_disk(waferId)][m_pSCTHelper->eta_module(waferId)];
         double sum_binomial = ROOT::Math::binomial_cdf(0, meanOccu, m_numberOfEvents*n_stripPerChip*n_chipPerSide);

         if (sum_binomial<deadChipDefinition) {
            ATH_MSG_INFO("DEADLINK : "<<moduleId<<", side="<<side);
            n_deadChip+=n_chipPerSide;

            //For DeadStrip
            if (m_doDeadStrip) {
               if (side==0) beginDead=0, endDead=767;
               else beginDead=768, endDead=1535;
               defectStrip = m_pCalibWriteTool->addDefect(defectStrip,beginDead,endDead);
            }

            //For DeadChip
            if (m_doDeadChip) {
               if (side==0) beginDead=0, endDead=5;
               else beginDead=6, endDead=11;
               defectChip = m_pCalibWriteTool->addDefect(defectChip,beginDead,endDead);
            }

            if (side==1) {
               //write to DB & .xml.
               if (defectChip==" 0-5  6-11 ") {
                  n_deadModule++;
               } else if (defectChip==" 0-5 " || defectChip==" 6-11 ") {
                  n_deadLink++;
               }

               if (!(defectStrip.empty()) || !(defectChip.empty())) {
                  if (addToSummaryStr(summaryList, waferId, "DEAD", defectStrip.c_str(), defectChip.c_str()).isFailure()) {
                     ATH_MSG_ERROR("Could not add dead strips to the summary");
                     return StatusCode::FAILURE;
                  }
               }
               if (!(defectStrip.empty())) {
                  if (m_writeToCool) {
                     if (m_pCalibWriteTool->createListStrip(moduleId,m_pSCTHelper,10000,"DEAD",m_deadStripSignificance,defectStrip).isFailure()) {
                        ATH_MSG_ERROR("Could not create strip list");
                        return StatusCode::FAILURE;
                    }
                  }

                  if (addToXML4DB(m_outDeadStrips, waferId, "DEAD", m_deadStripSignificance, defectStrip.c_str()).isFailure()) {
                     ATH_MSG_ERROR("Could not add xml strip list");
                     return StatusCode::FAILURE;
                  }

                  hasDeadStrip=true;

               }

               if (!(defectChip.empty())) {
                  if (m_writeToCool) {
                     if ( m_pCalibWriteTool->createListChip(moduleId,m_pSCTHelper,10000,"DEAD",m_deadChipSignificance,defectChip).isFailure()) {
                        ATH_MSG_ERROR("Could not create chip list");
                        return StatusCode::FAILURE;
                     } 
                  }

                  if (addToXML4DB(m_outDeadChips, waferId, "DEAD", m_deadChipSignificance, defectChip.c_str()).isFailure()) {
                     ATH_MSG_ERROR("Could not add xml chip list");
                     return StatusCode::FAILURE;
                  }

                  hasDeadChip=true;

               }
            }
            continue;
         }
      }//end DeadLink

      if (n_noHitsStrip>0) {
         int n_deadStripInWafer=0;
         int n_deadChipInWafer=0;

         double n_effectiveEvents=0;
         if (busyStream) n_effectiveEvents = m_numberOfEvents*(n_stripPerChip*n_chipPerSide-n_disabledStrip-n_noisyStrip-n_noHitsStrip);
         else n_effectiveEvents = m_numberOfEvents*(n_stripPerChip*n_chipPerSide-n_disabledStrip-n_noisyStrip);

         //First, check DeadChip
         double meanOccupancy = totalHitsInWafer/n_effectiveEvents;
         for (int j=0; j<n_stripPerChip*n_chipPerSide; j++) {
            if (numHitsInStrip[j]>0) numHitsInChip[j/n_stripPerChip] += numHitsInStrip[j];
         }

         for (int j=0; j<n_chipPerSide; j++) {
            isDead=false;
            int chipNum = side==0 ? j : j+6;
            if (numHitsInChip[j]==0 && !disabledChip[chipNum]) {
               if (!isNoHitLink) n_checkedChip++;
               double sum_binomial = ROOT::Math::binomial_cdf(0, meanOccupancy, m_numberOfEvents*(n_stripPerChip-n_disabledInChip[j]));
               if (sum_binomial<deadChipDefinition) {
                  ATH_MSG_INFO("DEADCHIP : "<<moduleId<<", side="<<side
                               <<", chip(online)="<<(side==0 ? j : j+n_chipPerSide));
                  isDead=true;
                  n_deadChip++;
                  n_deadChipInWafer++;
                  endDead = side==0 ? j : j+n_chipPerSide;
                  if (!beforeIsDead) beginDead = side==0 ? j : j+n_chipPerSide;
               }
            }

            if (m_doDeadChip) {
               if ((beforeIsDead && !isDead) || (j==5 && isDead)) defectChip = m_pCalibWriteTool->addDefect(defectChip,beginDead,endDead);
            }
            beforeIsDead = isDead;
         }//end chip loop

         //Second, check DeadStrip
         if (m_doDeadStrip) {
            double meanOccExceptDeadChip=totalHitsInWafer/(n_effectiveEvents-n_stripPerChip*n_deadChipInWafer);
            double numHitsInStripOnlineOrder[n_stripPerChip*n_chipPerSide]= {0};
            for (int j=0; j<n_stripPerChip*n_chipPerSide; j++) {
               numHitsInStripOnlineOrder[j] = side==0 ? numHitsInStrip[j] : numHitsInStrip[n_stripPerChip*n_chipPerSide-j];
               isDead=false;
               if (numHitsInStripOnlineOrder[j]==0) {
                  double sum_binomial = ROOT::Math::binomial_cdf(0, meanOccExceptDeadChip, m_numberOfEvents);
                  if (sum_binomial<deadStripDefinition) {
                     ATH_MSG_INFO("DEADSTRIP : "<<moduleId<<", side="<<side<<", strip(offline)="<<j);
                     isDead=true;
                     n_deadStrip++;
                     n_deadStripInWafer++;
                     endDead = side==0 ? j : j+n_stripPerChip*n_chipPerSide;
                     if (!beforeIsDead) beginDead = side==0 ? j : j+n_stripPerChip*n_chipPerSide;
                  }
               }

               if (m_doDeadStrip) {
                  if ((beforeIsDead && !isDead) || (j==5 && isDead)) defectStrip = m_pCalibWriteTool->addDefect(defectStrip,beginDead,endDead);
               }
               beforeIsDead = isDead;
            }
         }
      }

      //go to next wafer itr
   }//Wafer Loop end


   //Close Files
   if (m_doDeadStrip) {
      ATH_MSG_INFO("total #DeadStrip : "<<n_deadStrip);
      if (closeXML4DB(m_outDeadStrips).isFailure()) {
         ATH_MSG_ERROR("Problem closing "<<m_deadStripsFile);
         return StatusCode::FAILURE;
      }
   }
   if (m_doDeadChip) {
      ATH_MSG_INFO("total #DeadChip : "<<n_deadChip<<", #noHitChip : "<<n_checkedChip);
      if (closeXML4DB(m_outDeadChips).isFailure()) {
         ATH_MSG_ERROR("Problem closing "<<m_deadChipsFile);
         return StatusCode::FAILURE;
      }
   }

   //Making Summary File
   if (openXML4DeadSummary(m_outDeadSummary, "DEAD", n_deadModule, n_deadLink, n_deadChip, n_deadStrip).isFailure()) {
      ATH_MSG_ERROR("Problem opening "<<m_deadSummaryFile);
      return StatusCode::FAILURE;
   }
   if (wrapUpXML4Summary(m_outDeadSummary, "DEAD", summaryList).isFailure()) {
      ATH_MSG_ERROR("Problem closing "<<m_deadSummaryFile);
      return StatusCode::FAILURE;
   }

   if ( m_writeToCool ) {
      if (m_doDeadStrip && hasDeadStrip) {
         if ( m_pCalibWriteTool->wrapUpDeadStrips().isFailure() ) {
            ATH_MSG_ERROR("Could not get DeadStrips Info");
            return StatusCode::FAILURE;
         }
      }
      if (m_doDeadChip && hasDeadChip) {
         if ( m_pCalibWriteTool->wrapUpDeadChips().isFailure() ) {
            ATH_MSG_ERROR("Could not get DeadChips Info");
            return StatusCode::FAILURE;
         }
      }
   }

   ATH_MSG_INFO("END HERE");
   return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////
/// getNoiseOccupancy()
/// Read NoiseOccupancy from HIST and write out into local DB
///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::getNoiseOccupancy()
{
   ATH_MSG_INFO("----- in getNoiseOccupancy() -----");

   //--- Initialization
   int n_phiBinsBarrel[ n_barrels ] = { n_phiBinsB0, n_phiBinsB1, n_phiBinsB2, n_phiBinsB3 };
   int n_phiBinsEndcap[ n_disks ][ n_etaBinsEC ] = { { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter,                 0,                0 }
   };

   double meanNO_Barrel[ n_barrels ] = { 0 };
   double meanNO_ECA[ n_disks ][ n_etaBinsEC ] = { {0}, {0} };
   double meanNO_ECC[ n_disks ][ n_etaBinsEC ] = { {0}, {0} };

   //--- RunNumber
   std::ostringstream runnum;
   runnum << m_runNumber.value();

   //--- Directory in HIST
   std::string stem;

   //--- EndcapC
   stem = "/run_" + runnum.str() + "/SCT/SCTEC/Noise/";
   m_pnoiseoccupancymapHistoVectorECm.clear();
   for ( int iDisk = 0; iDisk < n_disks ; ++iDisk ) {
      for ( int iSide = 0; iSide < 2; ++iSide ) {
         ostringstream streamHist;
         streamHist << "noiseoccupancymap";
         if ( m_noiseOccupancyTriggerAware ) streamHist << "trigger";
         streamHist << "ECm_" << iDisk << "_" << iSide;
         std::string histName = stem + streamHist.str();
         TProfile2D* hist_tmp = dynamic_cast<TProfile2D*>(m_inputHist->Get( histName.c_str() ));
         m_pnoiseoccupancymapHistoVectorECm.push_back( hist_tmp );
      }
   }
   //--- Barrel
   stem = "/run_" + runnum.str() + "/SCT/SCTB/Noise/";
   m_pnoiseoccupancymapHistoVector.clear();
   for ( int iLayer = 0; iLayer < n_barrels ; ++iLayer ) {
      for ( int iSide = 0; iSide < 2; ++iSide ) {
         ostringstream streamHist;
         streamHist << "noiseoccupancymap";
         if ( m_noiseOccupancyTriggerAware ) streamHist << "trigger";
         streamHist << "_" << iLayer << "_" << iSide;
         std::string histName = stem + streamHist.str();
         TProfile2D* hist_tmp = dynamic_cast<TProfile2D*>(m_inputHist->Get( histName.c_str() ));
         m_pnoiseoccupancymapHistoVector.push_back( hist_tmp );
      }
   }
   //--- EndcapA
   stem = "/run_" + runnum.str() + "/SCT/SCTEA/Noise/";
   m_pnoiseoccupancymapHistoVectorECp.clear();
   for ( int iDisk = 0; iDisk < n_disks ; ++iDisk ) {
      for ( int iSide = 0; iSide < 2; ++iSide ) {
         ostringstream streamHist;
         streamHist << "noiseoccupancymap";
         if ( m_noiseOccupancyTriggerAware ) streamHist << "trigger";
         streamHist << "ECp_" << iDisk << "_" << iSide;
         std::string histName = stem + streamHist.str();
         TProfile2D* hist_tmp = dynamic_cast<TProfile2D*>(m_inputHist->Get( histName.c_str() ));
         m_pnoiseoccupancymapHistoVectorECp.push_back( hist_tmp );
      }
   }

   //--- XML file
   const char* outputNoiseOccupancyFileName = m_noiseOccupancyFile.value().c_str();
   ofstream outFile( outputNoiseOccupancyFileName, std::ios::out );
   if ( !outFile.good() ) {
      ATH_MSG_ERROR("Unable to open NoiseOccupancyFile : " << outputNoiseOccupancyFileName);
      return StatusCode::FAILURE;
   }

   //--- Header for XML outputs
   ostringstream osHeader;
   osHeader << "<channels server=\"ATLAS_COOLPROD\" schema=\"ATLAS_COOLOFL_SCT\" dbname=\"MONP200\" folder=\"SCT/Derived/NoiseOccupancy\" "
            << "since=\""   << m_iovStart.re_time()   << "\" "
            << "until=\""   << m_iovStop.re_time()    << "\" "
            << "tag=\""     << m_tagID4NoiseOccupancy << "\" "
            << "version=\"" << "multi\">"<< endl;
   outFile << osHeader.str();

   //--- EndcapC
   for ( int iDisk = 0; iDisk < n_disks ; ++iDisk ) {
      for ( int iSide = 0; iSide < 2; ++iSide ) {
         for ( int iEta = 0; iEta < n_etaBinsEC; ++iEta ) {
            for ( int iPhi = 0; iPhi < n_phiBinsEndcap[iDisk][iEta]; ++iPhi ) {
               Identifier waferId = m_pSCTHelper->wafer_id( ENDCAP_C, iDisk, iPhi, iEta, iSide );
               float occupancy = m_pnoiseoccupancymapHistoVectorECm[ 2*iDisk + iSide ]->GetBinContent( iEta+1, iPhi+1 );
               occupancy /= float( ntimeBins );
               occupancy /= 1E5;
               //--- For calculating average Noise Occupancy
               meanNO_ECC[iDisk][iEta]+=occupancy;
               //outFile << outFile << xmlChannelNoiseOccDataString(waferId, occupancy)<<endl;
               IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
               SCT_SerialNumber sn        = m_CablingTool->getSerialNumberFromHash( waferHash );
               outFile << xmlChannelNoiseOccDataString(waferId, occupancy, sn)<<endl;
               //--- DB output
               if ( m_writeToCool ) {
                  if ( m_pCalibWriteTool->createListNO( waferId, m_pSCTHelper, 10000, occupancy ).isFailure() ) {
                     ATH_MSG_ERROR("Unable to run createListNO");
                     return StatusCode::FAILURE;
                  }
               }
            }
         }
      }
   }
   //--- Barrel
   for ( int iLayer = 0; iLayer < n_barrels; ++iLayer ) {
      for ( int iSide = 0; iSide < 2; ++iSide ) {
         for ( int iEta = 0; iEta < n_etaBins; ++iEta ) {
            if ( iEta-6 == 0 ) continue;
            for ( int iPhi = 0; iPhi < n_phiBinsBarrel[iLayer]; ++iPhi ) {
               Identifier waferId = m_pSCTHelper->wafer_id( BARREL, iLayer, iPhi, iEta-6, iSide );
               float occupancy = m_pnoiseoccupancymapHistoVector[ 2*iLayer + iSide ]->GetBinContent( iEta+1, iPhi+1 );
               occupancy /= float( ntimeBins );
               occupancy /= 1E5;
               //--- For calculating average Noise Occupancy
               meanNO_Barrel[iLayer]+=occupancy;
               IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
               SCT_SerialNumber sn        = m_CablingTool->getSerialNumberFromHash( waferHash );
               outFile << xmlChannelNoiseOccDataString(waferId, occupancy, sn)<<endl;
               //--- DB output
               if ( m_writeToCool ) {
                  if ( m_pCalibWriteTool->createListNO( waferId, m_pSCTHelper, 10000, occupancy ).isFailure() ) {
                     ATH_MSG_ERROR("Unable to run createListNO");
                     return StatusCode::FAILURE;
                  }
               }
            }
         }
      }
   }
   //--- EndcapA
   for ( int iDisk = 0; iDisk < n_disks ; ++iDisk ) {
      for ( int iSide = 0; iSide < 2; ++iSide ) {
         for ( int iEta = 0; iEta < n_etaBinsEC; ++iEta ) {
            for ( int iPhi = 0; iPhi < n_phiBinsEndcap[iDisk][iEta]; ++iPhi ) {
               Identifier waferId = m_pSCTHelper->wafer_id( ENDCAP_A, iDisk, iPhi, iEta, iSide );
               float occupancy = m_pnoiseoccupancymapHistoVectorECp[ 2*iDisk + iSide ]->GetBinContent( iEta+1, iPhi+1 );
               occupancy /= float( ntimeBins );
               occupancy /= 1E5;
               //--- For calculating average Noise Occupancy
               meanNO_ECA[iDisk][iEta]+=occupancy;
               IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
               SCT_SerialNumber sn        = m_CablingTool->getSerialNumberFromHash( waferHash );
               outFile << xmlChannelNoiseOccDataString(waferId, occupancy, sn)<<endl;
               //--- DB output
               if ( m_writeToCool ) {
                  if ( m_pCalibWriteTool->createListNO( waferId, m_pSCTHelper, 10000, occupancy ).isFailure() ) {
                     ATH_MSG_ERROR("Unable to run createListNO");
                     return StatusCode::FAILURE;
                  }
               }
            }
         }
      }
   }

   //--- Tail of XML outputs
   outFile << "</channels>" << endl;

   //--- Summary XML output
   ostringstream summaryList;
   for ( int i = 0; i < n_disks; ++i ) {
      for (int j = 0; j < n_etaBinsEC; ++j ) {
         if ( n_phiBinsEndcap[i][j] != 0 ) {
            meanNO_ECC[i][j] /= (n_phiBinsEndcap[i][j]*2);
            summaryList<<xmlPartData(ENDCAP_C, i, j, "meanNO",meanNO_ECC[i][j]);
         }
      }
   }
   for ( int i = 0; i < n_barrels; ++i ) {
      meanNO_Barrel[i] /= (n_phiBinsBarrel[i]*n_etaInBarrel*2);
      summaryList<<xmlPartData(BARREL, i, 0, "meanNO",meanNO_Barrel[i] );
   }
   for ( int i = 0; i < n_disks; ++i ) {
      for ( int j = 0; j < n_etaBinsEC; ++j ) {
         if ( n_phiBinsEndcap[i][j] != 0 ) {
            meanNO_ECA[i][j] /= (n_phiBinsEndcap[i][j]*2);
            summaryList<<xmlPartData(ENDCAP_A, i, j, "meanNO",meanNO_ECA[i][j]);
         }
      }
   }

   if ( openXML4MonSummary( m_outNOSummary, "NoiseOccupancy" ).isFailure() ) {
      ATH_MSG_ERROR("Problem in opening NoiseOccupancy file");
      return StatusCode::FAILURE;
   }
   if ( wrapUpXML4Summary( m_outNOSummary, "NoiseOccupancy", summaryList ).isFailure() ) {
      ATH_MSG_ERROR("Problem in closing NoiseOccupancy file");
      return StatusCode::FAILURE;
   }

   //--- DB output
   if ( m_writeToCool ) {
      if ( m_pCalibWriteTool->wrapUpNoiseOccupancy().isFailure() ) {
         ATH_MSG_ERROR("Could not get NoiseOccupancy");
         return StatusCode::FAILURE;
      }
   }

   return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////////
/// getRawOccupancy()
/// Read RawOccupancy from Monitoring HIST and write out into local DB
///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::getRawOccupancy()
{
   ATH_MSG_INFO("----- in getRawOccupancy() -----");

   //--- Initialization
   int n_phiBinsBarrel[ n_barrels ] = { n_phiBinsB0, n_phiBinsB1, n_phiBinsB2, n_phiBinsB3 };
   int n_phiBinsEndcap[ n_disks ][ n_etaBinsEC ] = { { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter,                 0,                0 }
   };

   double meanRO_Barrel[ n_barrels ] = { 0 };
   double meanRO_ECA[ n_disks ][ n_etaBinsEC ] = { {0}, {0} };
   double meanRO_ECC[ n_disks ][ n_etaBinsEC ] = { {0}, {0} };

   //--- RunNumber
   std::ostringstream runnum;
   runnum << m_runNumber.value();

   //--- Directory in HIST
   std::vector< std::pair<std::string, int> > EC_stems;
   EC_stems.clear();
   std::pair<std::string, int> stem_C("/run_" + runnum.str() + "/SCT/SCTEC/hits/", ENDCAP_C);
   std::pair<std::string, int> stem_A("/run_" + runnum.str() + "/SCT/SCTEA/hits/", ENDCAP_A);
   EC_stems.push_back(stem_C);
   EC_stems.push_back(stem_A);
   std::vector< std::pair<std::string, int> >::iterator stemItr=EC_stems.begin();

   //--- Endcaps
   for (stemItr=EC_stems.begin(); stemItr!=EC_stems.end(); stemItr++) {
      for (int iDisk=0; iDisk<n_disks; ++iDisk) {
         for (int iSide=0; iSide<2; ++iSide) {
            for (int iEta=0; iEta<n_etaBinsEC; ++iEta) {
               for (int iPhi=0; iPhi<n_phiBinsEndcap[iDisk][iEta]; ++iPhi) {
                  Identifier waferId = m_pSCTHelper->wafer_id( (*stemItr).second, iDisk, iPhi, iEta, iSide );
                  std::string detector_part;
                  detector_part.erase();
                  if (m_histBefore2010) {
                     if ( (*stemItr).second==ENDCAP_C ) detector_part = "ECm_hitsmap";
                     else detector_part = "ECp_hitsmap";
                  } else {
                     if ( (*stemItr).second==ENDCAP_C ) detector_part = "hitsmapECm";
                     else detector_part = "hitsmapECp";
                  }
                  ostringstream streamHist;
                  streamHist << detector_part << "_" << iDisk << "_" << iSide;
                  std::string hitsmapname = stemItr->first + streamHist.str();
                  TH2D* hist_tmp = dynamic_cast<TH2D*>(m_inputHist->Get( hitsmapname.c_str() ));
                  unsigned long long n_hits = static_cast<unsigned long long>(hist_tmp->GetBinContent( iEta+1, iPhi+1 ));
                  float raw_occu = 0;
                  if (m_numberOfEvents!=0) {
                     raw_occu = float(n_hits)/(m_numberOfEvents*n_chipPerSide*n_stripPerChip);
                     //--- For calculating average Raw Occupancy
                     if (stemItr->second==ENDCAP_C ) meanRO_ECC[iDisk][iEta]+=static_cast<double>(raw_occu);
                     else if ( stemItr->second==ENDCAP_A ) meanRO_ECA[iDisk][iEta]+=static_cast<double>(raw_occu);
                  }
                  //--- DB writing
                  if ( m_writeToCool ) {
                     if ( m_pCalibWriteTool->createListRawOccu(waferId, m_pSCTHelper, m_numberOfEvents, raw_occu).isFailure() ) {
                        ATH_MSG_ERROR("Unable to run createListRawOccu");
                        return StatusCode::FAILURE;
                     }
                  }
               }
            }
         }
      }
   }
   //--- Barrel
   for (int iLayer=0; iLayer<n_barrels; ++iLayer) {
      for (int iSide=0; iSide<2; ++iSide) {
         for (int iEta=0; iEta<n_etaBins; ++iEta) {
            if (iEta-6==0) continue;
            for (int iPhi=0; iPhi<n_phiBinsBarrel[iLayer]; ++iPhi) {
               Identifier waferId = m_pSCTHelper->wafer_id( BARREL, iLayer, iPhi, iEta-6, iSide );
               ostringstream streamHist;
               streamHist << iLayer << "_" << iSide;
               std::string hitsmapname = "/run_" + runnum.str() + "/SCT/SCTB/hits/hitsmap_" + streamHist.str();
               TH2D* hist_tmp = dynamic_cast<TH2D*>(m_inputHist->Get( hitsmapname.c_str() ));
               unsigned long long n_hits = static_cast<unsigned long long>(hist_tmp->GetBinContent( iEta+1, iPhi+1 ));
               float raw_occu = 0;
               if (m_numberOfEvents!=0) {
                  raw_occu = float(n_hits)/(m_numberOfEvents*n_chipPerSide*n_stripPerChip);
                  //--- For calculating average Raw Occupancy
                  meanRO_Barrel[iLayer]+=static_cast<double>(raw_occu);
               }
               //--- DB writing
               if ( m_writeToCool ) {
                  if ( m_pCalibWriteTool->createListRawOccu(waferId, m_pSCTHelper, m_numberOfEvents, raw_occu).isFailure() ) {
                     ATH_MSG_ERROR("Unable to run createListRawOccu");
                     return StatusCode::FAILURE;
                  }
               }
            }
         }
      }
   }
   //--- Summary XML output
   ostringstream summaryList;
   for ( int i = 0; i < n_disks; ++i ) {
      for ( int j = 0; j < n_etaBinsEC; ++j ) {
         if ( n_phiBinsEndcap[i][j] != 0 ) {
            meanRO_ECC[i][j] /= (n_phiBinsEndcap[i][j]*2);
            summaryList<<xmlPartData(ENDCAP_C, i, j, "meanRO",meanRO_ECC[i][j]);
         }
      }
   }
   for ( int i = 0; i < n_barrels; ++i ) {
      meanRO_Barrel[i] /= (n_phiBinsBarrel[i]*n_etaInBarrel*2);
      summaryList<<xmlPartData(BARREL, i, 0, "meanRO",meanRO_Barrel[i]);
   }
   for ( int i = 0; i < n_disks; ++i ) {
      for ( int j = 0; j < n_etaBinsEC; ++j ) {
         if ( n_phiBinsEndcap[i][j] != 0 ) {
            meanRO_ECA[i][j] /= (n_phiBinsEndcap[i][j]*2);
            summaryList<<xmlPartData(ENDCAP_A, i, j, "meanRO",meanRO_ECA[i][j]);
         }
      }
   }

   if ( openXML4MonSummary( m_outROSummary, "RawOccupancy" ).isFailure() ) {
      ATH_MSG_ERROR("Problem in opening RawOccupancy file");
      return StatusCode::FAILURE;
   }
   if ( wrapUpXML4Summary( m_outROSummary, "RawOccupancy", summaryList ).isFailure() ) {
      ATH_MSG_ERROR("Problem in closing RawOccupancy file ");
      return StatusCode::FAILURE;
   }

   //--- DB output
   if ( m_writeToCool ) {
      if ( m_pCalibWriteTool->wrapUpRawOccupancy().isFailure() ) {
         ATH_MSG_ERROR("Could not get RawOccupancy");
         return StatusCode::FAILURE;
      }
   }

   return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////////
/// getEfficiency()
/// Read Efficiency from Monitoring HIST and write out into local DB
///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::getEfficiency() {
   ATH_MSG_INFO("----- in getEfficiency() -----");

   //--- Initialization
   int n_phiBinsBarrel[ n_barrels ] = { n_phiBinsB0, n_phiBinsB1, n_phiBinsB2, n_phiBinsB3 };
   int n_phiBinsEndcap[ n_disks ][ n_etaBinsEC ] = { { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter,                 0,                0 }
   };

   double meanEff_Barrel[ n_barrels ] = { 0 };
   double meanEff_ECA[ n_disks ][ n_etaBinsEC ] = { {0}, {0} };
   double meanEff_ECC[ n_disks ][ n_etaBinsEC ] = { {0}, {0} };

   //--- RunNumber
   std::ostringstream runnum;
   runnum << m_runNumber.value();

   //--- Directory in HIST
   std::vector< std::pair<std::string, int> > EC_stems;
   EC_stems.clear();
   std::pair<std::string, int> stem_C("/run_" + runnum.str() + "/SCT/SCTEC/eff/", ENDCAP_C);
   std::pair<std::string, int> stem_A("/run_" + runnum.str() + "/SCT/SCTEA/eff/", ENDCAP_A);
   EC_stems.push_back(stem_C);
   EC_stems.push_back(stem_A);
   std::vector< std::pair<std::string, int> >::iterator stemItr=EC_stems.begin();

   //--- XML file
   // if ( openXML4MonSummary( m_outEffSummary, "EfficiencyModule" ).isFailure() ) {
   //   ATH_MSG_ERROR("Problem in opening " << m_outEffSummary);
   //   return StatusCode::FAILURE;
   // }

   const char* outputEfficiencyFileName = m_efficiencyModuleFile.value().c_str();
   ofstream outFile( outputEfficiencyFileName, std::ios::out );
   if ( !outFile.good() ) {
      ATH_MSG_ERROR("Unable to open EfficiencyFile : " << outputEfficiencyFileName);
      return StatusCode::FAILURE;
   }

   std::string xslName="EfficiencyInfo.xsl";
   outFile << xmlHeader<< linefeed<< associateStylesheet(xslName) << linefeed<< "<run>"<< endl;
   outFile << xmlValue("RunNumber",  m_runNumber.value()               ) << linefeed
           << xmlValue("StartTime",  m_utcBegin                        ) << linefeed
           << xmlValue("EndTime",  m_utcEnd                            ) << linefeed
           << xmlValue("Duration",  m_calibEvtInfoTool->duration()     ) << linefeed
           << xmlValue("LB",  m_LBRange                                ) << linefeed
           << xmlValue("Events",  m_numberOfEvents                     ) << linefeed
           << "  <modules>"<< endl;


   //--- Endcaps
   for (stemItr=EC_stems.begin(); stemItr!=EC_stems.end(); stemItr++) {
      for (int iDisk=0; iDisk<n_disks; ++iDisk) {
         for (int iSide=0; iSide<2; ++iSide) {
            for (int iEta=0; iEta<n_etaBinsEC; ++iEta) {
               for (int iPhi=0; iPhi<n_phiBinsEndcap[iDisk][iEta]; ++iPhi) {
                  Identifier waferId = m_pSCTHelper->wafer_id( (*stemItr).second, iDisk, iPhi, iEta, iSide );
                  std::string detector_part;
                  detector_part.erase();
                  ostringstream streamProf;
                  if ( (*stemItr).second==ENDCAP_C ) {
                     detector_part = "m_eff";
                     streamProf << detector_part << "_" << iDisk << "_" << iSide;
                  } else {
                     detector_part = "p_eff";
                     streamProf << detector_part << "_" << iDisk << "_" << iSide;
                  }
                  std::string effmapname = stemItr->first + streamProf.str();
                  TProfile2D* prof_tmp = dynamic_cast<TProfile2D*>(m_inputHist->Get( effmapname.c_str() ));
                  int global_bin = prof_tmp->GetBin( iEta+1, iPhi+1 );
                  float eff = static_cast<float>(prof_tmp->GetBinContent( global_bin ));
                  unsigned long long eff_entry = static_cast<unsigned long long>(prof_tmp->GetBinEntries( global_bin ));
                  //--- For calculating average Efficiency
                  if ( stemItr->second==ENDCAP_C ) meanEff_ECC[iDisk][iEta]+=static_cast<double>(eff);
                  else if ( stemItr->second==ENDCAP_A ) meanEff_ECA[iDisk][iEta]+=static_cast<double>(eff);
                  //--- For Efficiency _not_ averaged over modules
                  IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
                  SCT_SerialNumber sn        = m_CablingTool->getSerialNumberFromHash( waferHash );
                  outFile << xmlChannelEfficiencyDataString(waferId, eff, sn)<<endl;
                  //--- DB writing
                  if ( m_writeToCool ) {
                     if ( m_pCalibWriteTool->createListEff(waferId, m_pSCTHelper, eff_entry, eff).isFailure() ) {
                        ATH_MSG_ERROR("Unable to run createListEff");
                        return StatusCode::FAILURE;
                     }
                  }
               }
            }
         }
      }
   }
   //--- Barrel
   for (int iLayer=0; iLayer<n_barrels; ++iLayer) {
      for (int iSide=0; iSide<2; ++iSide) {
         for (int iEta=0; iEta<n_etaBins; ++iEta) {
            if (iEta-6==0) continue;
            for (int iPhi=0; iPhi<n_phiBinsBarrel[iLayer]; ++iPhi) {
               Identifier waferId = m_pSCTHelper->wafer_id( BARREL, iLayer, iPhi, iEta-6, iSide );
               ostringstream streamProf;
               streamProf << iLayer << "_" << iSide;
               std::string effmapname = "/run_" + runnum.str() + "/SCT/SCTB/eff/eff_" + streamProf.str();
               TProfile2D* prof_tmp = dynamic_cast<TProfile2D*>(m_inputHist->Get( effmapname.c_str() ));
               int global_bin = prof_tmp->GetBin( iEta+1, iPhi+1 );
               float eff = static_cast<float>(prof_tmp->GetBinContent( global_bin ));
               unsigned long long eff_entry = static_cast<unsigned long long>(prof_tmp->GetBinEntries( global_bin ));
               //--- For calculating average Efficiency
               meanEff_Barrel[iLayer]+=static_cast<double>(eff);
               //--- For Efficiency _not_ averaged over modules
               IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
               SCT_SerialNumber sn        = m_CablingTool->getSerialNumberFromHash( waferHash );
               outFile << xmlChannelEfficiencyDataString(waferId, eff, sn)<<endl;
               //--- DB writing
               if ( m_writeToCool ) {
                  if ( m_pCalibWriteTool->createListEff(waferId, m_pSCTHelper, eff_entry, eff).isFailure() ) {
                     ATH_MSG_ERROR("Unable to run createListEff");
                     return StatusCode::FAILURE;
                  }
               }
            }
         }
      }
   }

   //--- Tail of XML outputs
   //  outFile << "</channels>" << endl;

   outFile << "  </modules>" << endl;
   outFile << "</run>" << endl;


   //--- Summary XML output
   ostringstream summaryList;
   for ( int i = 0; i < n_disks; ++i ) {
      for ( int j = 0; j < n_etaBinsEC; ++j ) {
         if ( n_phiBinsEndcap[i][j] != 0 ) {
            meanEff_ECC[i][j] /= (n_phiBinsEndcap[i][j]*2);
            summaryList<<xmlPartData(ENDCAP_C, i, j, "meanEff",meanEff_ECC[i][j]);
         }
      }
   }
   for ( int i = 0; i < n_barrels; ++i ) {
      meanEff_Barrel[i] /= (n_phiBinsBarrel[i]*n_etaInBarrel*2);
      summaryList<<xmlPartData(BARREL, i, 0, "meanEff",meanEff_Barrel[i]);
   }
   for ( int i = 0; i < n_disks; ++i ) {
      for ( int j = 0; j < n_etaBinsEC; ++j ) {
         if ( n_phiBinsEndcap[i][j] != 0 ) {
            meanEff_ECA[i][j] /= (n_phiBinsEndcap[i][j]*2);
            summaryList<<xmlPartData(ENDCAP_A, i, j, "meanEff",meanEff_ECA[i][j]);
         }
      }
   }

   if ( openXML4MonSummary( m_outEffSummary, "Efficiency" ).isFailure() ) {
      ATH_MSG_ERROR("Problem in opening Efficiency file");
      return StatusCode::FAILURE;
   }

   if ( wrapUpXML4Summary( m_outEffSummary, "Efficiency", summaryList ).isFailure() ) {
      ATH_MSG_ERROR("Problem in closing Efficiency file ");
      return StatusCode::FAILURE;
   }

   //--- DB output
   if ( m_writeToCool ) {
      if ( m_pCalibWriteTool->wrapUpEfficiency().isFailure() ) {
         ATH_MSG_ERROR("Could not get Efficiency");
         return StatusCode::FAILURE;
      }
   }

   return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////////
/// getBSErrors()
/// Read BSErrors from Monitoring HIST and write out into local DB
///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::getBSErrors() {
   ATH_MSG_INFO("----- in getBSErrors() -----");

   //--- Initialization
   int n_phiBinsBarrel[ n_barrels ] = { n_phiBinsB0, n_phiBinsB1, n_phiBinsB2, n_phiBinsB3 };
   int n_phiBinsEndcap[ n_disks ][ n_etaBinsEC ] = { { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle, n_phiBinsECShort },
      { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter, n_phiBinsECMiddle,                0 },
      { n_phiBinsECOuter,                 0,                0 }
   };

   unsigned long long nErrLink_Barrel[ n_barrels ] = { 0 };
   unsigned long long nErrLink_ECA[ n_disks ][ n_etaBinsEC ] = { {0}, {0} };
   unsigned long long nErrLink_ECC[ n_disks ][ n_etaBinsEC ] = { {0}, {0} };


   unsigned long long nErrLink_Barrel_module[ n_barrels ][ 2 ][ n_etaBins ][ n_phiBinsB3 ] = {{{{0}}}};
   unsigned long long nErrLink_ECA_module[ n_disks ][2][ n_etaBinsEC ][n_phiBinsECOuter] = {{{{0}}}};
   unsigned long long nErrLink_ECC_module[ n_disks ][2][ n_etaBinsEC ][n_phiBinsECOuter] = {{{{0}}}};

   std::string nErrLink_Barrel_module_serial[ n_barrels ][ 2 ][ n_etaBins ][ n_phiBinsB3 ];
   std::string nErrLink_ECA_module_serial[ n_disks ][2][ n_etaBinsEC ][n_phiBinsECOuter];
   std::string nErrLink_ECC_module_serial[ n_disks ][2][ n_etaBinsEC ][n_phiBinsECOuter];

   unsigned long long nErrs_Barrel_module[ n_barrels ][ 2 ][ n_etaBins ][ n_phiBinsB3 ][ 15 ] = {{{{{0}}}}};
   unsigned long long nErrs_ECA_module[ n_disks ][2][ n_etaBinsEC ][n_phiBinsECOuter][ 15 ]   = {{{{{0}}}}};
   unsigned long long nErrs_ECC_module[ n_disks ][2][ n_etaBinsEC ][n_phiBinsECOuter][ 15 ]   = {{{{{0}}}}};



   //--- RunNumber
   std::ostringstream runnum;
   runnum << m_runNumber.value();
   //--- ErrorList
   typedef std::map< int, std::string > IntStringMap;
   IntStringMap ErrMap_C, ErrMap;
   const int numberOfErrorTypes(12);
   boost::array<std::string, numberOfErrorTypes> errorNames= {{
         "BSParse","TimeOut", "BCID","LVL1ID", "Preamble", "Formatter",
         "ABCD","Raw", "MaskedLink", "RODClock",
         "TruncROD", (m_histBefore2010?"ROBFragment":"ROBFrag" )
      }
   };
   //
   boost::array<std::string, numberOfErrorTypes> errorNames_C= {{
         "BSParse","TimeOut","BCID","LVL1ID", "Preamble", "Formatter",
         "ABCD","Raw", (m_histBefore2010?"TmaskedLinks":"MaskedLink"), "RODClock",
         "TruncROD", (m_histBefore2010?"ROBFragment":"ROBFrag" )
      }
   };
   boost::array<int, numberOfErrorTypes> errorValues= {{0,1,2,3,4,5,9,10,11,12,13,14}};
   //should do compile time check to ensure the sizes are equal.
   ErrMap_C.clear();
   for (int indx(0); indx!=numberOfErrorTypes; ++indx) {
      ErrMap_C.insert(make_pair(errorValues[indx],errorNames_C[indx]));
   }
   ErrMap.clear();
   for (int indx(0); indx!=numberOfErrorTypes; ++indx) {
      ErrMap.insert(make_pair(errorValues[indx],errorNames[indx]));
   }

   //--- Directory in HIST
   const int N_ENDCAPS(2);
   boost::array<std::string, N_ENDCAPS> detectorStems= {{"/run_" + runnum.str() + "/SCT/SCTEC/errors/",  "/run_" + runnum.str() + "/SCT/SCTEA/errors/"}}; //barrel stem unused here
   boost::array<IntStringMap::iterator, N_ENDCAPS> detectorIterators= {{ErrMap_C.begin(), ErrMap.begin()}};
   boost::array<std::string, N_ENDCAPS> detectorParts= {{"ECm",  "ECp"}};
   std::string defecttype("");
   std::string n_defect("");
   int n_errorLink = 0;
   //--- Endcaps
   for (int stemIndex=0; stemIndex!=N_ENDCAPS; ++stemIndex) {
      // fix agrohsje const int thisBec=(2 * stemIndex) - 2; //map 0, 1 onto -2, 2
      const int thisBec=(4 * stemIndex) - 2; //map 0, 1 onto -2, 2
      const std::string detector_part=detectorParts[stemIndex];
      for (int iDisk=0; iDisk<n_disks; ++iDisk) {
         for (int iSide=0; iSide<2; ++iSide) {
            for (int iEta=0; iEta<n_etaBinsEC; ++iEta) {
               for (int iPhi=0; iPhi<n_phiBinsEndcap[iDisk][iEta]; ++iPhi) {
                  defecttype.erase();
                  n_defect.erase();
                  ostringstream osErrorList;
                  ostringstream osProbList;
                  Identifier waferId = m_pSCTHelper->wafer_id( thisBec, iDisk, iPhi, iEta, iSide );
                  IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
                  SCT_SerialNumber sn        = m_CablingTool->getSerialNumberFromHash( waferHash );

                  if ( thisBec==ENDCAP_C ) {
                     nErrLink_ECC_module_serial[iDisk][iSide][iEta][iPhi]=sn.str();
                  }
                  else if ( thisBec==ENDCAP_A ) {
                     nErrLink_ECA_module_serial[iDisk][iSide][iEta][iPhi]=sn.str();
                  }

                  IntStringMap::iterator errItr=detectorIterators[stemIndex];
                  for ( int iType = 0; iType < n_BSErrorType; ++iType ) {
                     float errorProb = 0.;
                     unsigned long long n_errors = 0;
                     if ( iType == errItr->first ) {
                        ostringstream streamHist;
                        //temporal fix: folder and histogram names should be Preamble
                        streamHist << errItr->second << "Errs" << "_" << iDisk << "_" << iSide;
                        //        streamHist << "T" << errItr->second << "Errs" << detector_part << "_" << iDisk << "_" << iSide;
                        std::string folder = errItr->second+std::string("/");
                        //histogram might or might not be inside a folder with the same name
                        std::string profname = detectorStems[stemIndex] + folder +streamHist.str();
                        std::string profnameShort = detectorStems[stemIndex] + streamHist.str();

                        TProfile2D* prof_tmp = dynamic_cast<TProfile2D*>(m_inputHist->Get( profname.c_str() ));
                        if (prof_tmp ==nullptr) {
                          prof_tmp = dynamic_cast<TProfile2D*>(m_inputHist->Get( profnameShort.c_str() ));
                        }
                        if (prof_tmp ==nullptr) {
                           ATH_MSG_ERROR("Unable to get profile for BSErrorsDB : " << profname);
                           return StatusCode::FAILURE;
                        }

                        n_errors = static_cast<unsigned long long>(prof_tmp->GetBinContent( iEta+1, iPhi+1 ));
                        if (n_errors!=0) {
                           defecttype = m_pCalibWriteTool->addNumber( defecttype, errItr->first );
                           n_defect = m_pCalibWriteTool->addNumber( n_defect, n_errors );
                           errorProb = static_cast<float>(n_errors) / static_cast<float>(m_numberOfEvents);
                           nErrs_ECC_module[iDisk][iSide][iEta][iPhi][errItr->first] = n_errors;
                           if ( thisBec==ENDCAP_C ) {
                              nErrLink_ECC_module[iDisk][iSide][iEta][iPhi]+=n_errors;
                           }
                           else if ( thisBec==ENDCAP_A ) {
                              nErrLink_ECA_module[iDisk][iSide][iEta][iPhi]+=n_errors;
                           }

                        }//end if (n_errors!=0)
                        ++errItr;
                     }//end if ( iType == (*errItr).first )
                     osErrorList << n_errors;
                     osProbList << errorProb;
                     if ( iType != n_BSErrorType-1 ) {
                        osErrorList << " ";
                        osProbList << " ";
                     }
                  }//end ErrorType Loop
                  //--- DB writing
                  if (!(defecttype.empty())) {
                     n_errorLink++;
                     if ( thisBec==ENDCAP_C ) {
                        nErrLink_ECC[iDisk][iEta]++;
                     }
                     else if ( thisBec==ENDCAP_A ) {
                        nErrLink_ECA[iDisk][iEta]++;
                     }
                     if ( m_writeToCool ) {
                        if ( m_pCalibWriteTool->createListBSErr(waferId, m_pSCTHelper, m_numberOfEvents, osErrorList.str(),osProbList.str()).isFailure() ) {
                           ATH_MSG_ERROR("Unable to run createListBSError");
                           return StatusCode::FAILURE;
                        }
                     }
                  }
               }// end of for iPhi
            }//implicit end of iEta
         }//implicit end of iside
      }//implicit end of iDisk
   }//end of stemIndex loop
   //--- Barrel
   for (int iLayer=0; iLayer<n_barrels; ++iLayer) {
      for (int iSide=0; iSide<2; ++iSide) {
         for (int iEta=0; iEta<n_etaBins; ++iEta) {
            if (iEta-6==0) continue;
            for (int iPhi=0; iPhi<n_phiBinsBarrel[iLayer]; ++iPhi) {
               defecttype.erase();
               n_defect.erase();
               ostringstream osErrorList;//agrohsje
               ostringstream osProbList;
               Identifier waferId = m_pSCTHelper->wafer_id( BARREL, iLayer, iPhi, iEta-6, iSide );
               IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
               SCT_SerialNumber sn        = m_CablingTool->getSerialNumberFromHash( waferHash );
               nErrLink_Barrel_module_serial[iLayer][iSide][iEta][iPhi] = sn.str();
               IntStringMap::iterator errItr=ErrMap.begin();
               for ( int iType = 0; iType < n_BSErrorType; ++iType ) {
                  float errorProb = 0.;
                  unsigned long long n_errors = 0;
                  if ( iType == errItr->first ) {
                     ostringstream streamHist;
                     streamHist << "T" << errItr->second << "Errs" << "_" << iLayer << "_" << iSide;
                     //histogram might or might not be inside a folder with the same name
                     std::string folder = errItr->second+std::string("/");
                     std::string profname = "/run_" + runnum.str() + "/SCT/SCTB/errors/" + folder + streamHist.str();
                     std::string profnameShort = "/run_" + runnum.str() + "/SCT/SCTB/errors/" + streamHist.str();

                     TProfile2D* prof_tmp = dynamic_cast<TProfile2D*>(m_inputHist->Get( profname.c_str() ));
                     if (prof_tmp ==nullptr) {
                        prof_tmp = dynamic_cast<TProfile2D*>(m_inputHist->Get( profnameShort.c_str() ));
                     }
                     if (prof_tmp ==nullptr) {
                        ATH_MSG_ERROR("Unable to get profile for BSErrorsDB : " << profname);
                        return StatusCode::FAILURE;
                     }
                     n_errors = static_cast<unsigned long long>(prof_tmp->GetBinContent( iEta+1, iPhi+1 ));
                     if (n_errors!=0) {
                        defecttype = m_pCalibWriteTool->addNumber( defecttype, errItr->first );
                        n_defect = m_pCalibWriteTool->addNumber( n_defect, n_errors );
                        errorProb = static_cast<float>(n_errors) / static_cast<float>(m_numberOfEvents);
                        nErrs_Barrel_module[iLayer][iSide][iEta][iPhi][errItr->first] = n_errors;
                        nErrLink_Barrel_module[iLayer][iSide][iEta][iPhi]+=n_errors;

                     }//end if (n_errors!=0)
                     ++errItr;
                  }//end if ( iType == (*errItr).first )
                  osErrorList << n_errors;
                  osProbList << errorProb;
                  if ( iType != n_BSErrorType-1 ) {
                     osErrorList << " ";
                     osProbList << " ";
                  }
               }   //end ErrorType Loop
               //--- DB writing
               if (!(defecttype.empty())) {
                  n_errorLink++;
                  nErrLink_Barrel[iLayer]++;
                  if ( m_writeToCool ) {
                     if ( m_pCalibWriteTool->createListBSErr(waferId, m_pSCTHelper, m_numberOfEvents, osErrorList.str(), osProbList.str()).isFailure() ) {
                        ATH_MSG_ERROR("Unable to run createListBSError");
                        return StatusCode::FAILURE;
                     }//end of if m_pCalib
                  }//end of if m_writeToCool
               } //end of if defecttype empty
            }//end of for iPhi
         }//endof for iEta, implicit end of for iSide and iLayer
      }
   }

   ATH_MSG_INFO("#Links which send BSError : " << n_errorLink);

   //--- Summary XML output
   ostringstream summaryList;
   for ( int i = 0; i < n_disks; ++i ) {
      for ( int j = 0; j < n_etaBinsEC; ++j ) {
         if ( n_phiBinsEndcap[i][j] != 0 ) {
            summaryList<<xmlPartData(ENDCAP_C, i, j, "nErrLink", nErrLink_ECC[i][j]);
         }
      }
   }
   for ( int i = 0; i < n_barrels; ++i ) {
      summaryList<<xmlPartData(BARREL, i, 0, "nErrLink", nErrLink_Barrel[i]);
   }

   for ( int i = 0; i < n_disks; ++i ) {
      for ( int j = 0; j < n_etaBinsEC; ++j ) {
         if ( n_phiBinsEndcap[i][j] != 0 ) {
            summaryList<<xmlPartData(ENDCAP_A, i, j, "nErrLink", nErrLink_ECA[i][j]);
         }
      }
   }

   if ( openXML4MonSummary( m_outBSErrSummary, "BSErrors" ).isFailure() ) {
      ATH_MSG_ERROR("Problem in opening BSErrors file");
      return StatusCode::FAILURE;
   }
   if ( wrapUpXML4Summary( m_outBSErrSummary, "BSErrors", summaryList ).isFailure() ) {
      ATH_MSG_ERROR("Problem in closing BSErrors file");
      return StatusCode::FAILURE;
   }

   //module XML output
   //agascon 06.02.2015
   ostringstream moduleList;
   std::string serial;
   for ( int i = 0; i < n_disks; ++i ) {
      for ( int j = 0; j < n_etaBinsEC; ++j ) {
         if ( n_phiBinsEndcap[i][j] != 0 ) {
            for ( int k = 0; k < 2; k++) {
               for ( int l = 0; l < n_phiBinsEndcap[i][j]; l++) {
                  serial = nErrLink_ECC_module_serial[i][k][j][l];

                  //fill ostringstream with number of error of each type for one particular module
                  ostringstream errList;
                  for ( int errCount = 0; errCount < numberOfErrorTypes; errCount++) {
                     int type = errorValues[errCount]; //
                     errList<<"    "<<xmlValue(ErrMap[type], nErrs_ECC_module[i][k][j][l][type])<<endl;
                  }

                  moduleList<<xmlModuleData(ENDCAP_C, i, k, j, l, "nErrors", nErrLink_ECC_module[i][k][j][l], serial, errList.str());
                  //          moduleList<<xmlModuleData(ENDCAP_C, i, k, j, l, "nErrLink", nErrLink_ECC_module[i][k][j][l], serial, errList);

               }
            }
         }
      }
   }


   for ( int i = 0; i < n_barrels; i++ ) {
      for ( int j = 0; j < 2; j++) {
         for ( int k = 0; k < n_etaBins; k++) {
            for ( int l = 0; l < n_phiBinsBarrel[i] ; l++) {
               serial = nErrLink_Barrel_module_serial[i][j][k][l];

               ostringstream errList;
               for ( int errCount = 0; errCount < numberOfErrorTypes; errCount++) {
                  int type = errorValues[errCount]; //
                  errList<<"    "<<xmlValue(ErrMap[type], nErrs_Barrel_module[i][j][k][l][type])<<endl;
               }

               moduleList<<xmlModuleData(BARREL, i, j, k, l, "nErrors", nErrLink_Barrel_module[i][j][k][l], serial, errList.str());
            }
         }
      }
   }

   for ( int i = 0; i < n_disks; ++i ) {
      for ( int j = 0; j < n_etaBinsEC; ++j ) {
         if ( n_phiBinsEndcap[i][j] != 0 ) {
            for ( int k = 0; k < 2; k++) {
               for ( int l = 0; l < n_phiBinsEndcap[i][j]; l++) {
                  serial = nErrLink_ECA_module_serial[i][k][j][l];

                  ostringstream errList;
                  for ( int errCount = 0; errCount < numberOfErrorTypes; errCount++) {
                     int type = errorValues[errCount]; //
                     errList<<"    "<<xmlValue(ErrMap[type], nErrs_ECA_module[i][k][j][l][type])<<endl;
                  }

                  moduleList<<xmlModuleData(ENDCAP_A, i, k, j, l, "nErrors", nErrLink_ECA_module[i][k][j][l], serial, errList.str());
               }
            }
         }
      }
   }

   if ( openXML4MonSummary( m_outBSErrModule, "BSErrorsModule" ).isFailure() ) {
      ATH_MSG_ERROR("Problem in opening BSErrorsModule file");
      return StatusCode::FAILURE;
   }
   if ( wrapUpXML4Summary( m_outBSErrModule, "BSErrors", moduleList ).isFailure() ) {
      ATH_MSG_ERROR("Problem in closing BSErrors file");
      return StatusCode::FAILURE;
   }

   //--- DB output
   if ( m_writeToCool ) {
      if ( m_pCalibWriteTool->wrapUpBSErrors().isFailure() ) {
         ATH_MSG_ERROR("Could not get ByteStream Errors");
         return StatusCode::FAILURE;
      }
   }

   return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////////
/// getLorentzAngle()
/// Read LorentzAngle from HIST and write out into local DB
///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::getLorentzAngle() {
   ATH_MSG_INFO("----- in getLorentzAngle() -----");

   //--- Initialization

   float A_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };
   float LA_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };
   float B_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };
   float Sigma_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };

   float Err_A_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };
   float Err_LA_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };
   float Err_B_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };
   float Err_Sigma_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };

   float MCW_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };
   float Err_MCW_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };
   float Chisq_BarrelSide[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };

   string DBUploadFlag = "G";  // fit status flag
   string module[2]= {"100","111"};
   int moduleint[2]= {100,111};

   int FitFlag[ n_barrels ][ 2 ][ 2 ] = { {{0},{0}}, {{0},{0}} };  // fit status flag

   TFile *fitFile;

   //--- RunNumber
   std::ostringstream runnum;
   runnum << m_runNumber.value();

   //--- Directory in HIST
   std::string stem;

   //--- Barrel
   stem = "/run_" + runnum.str() + "/SCT/GENERAL/lorentz/";
   m_h_phiVsNstripsSideHistoVector.clear();
   for ( int iLayer = 0; iLayer < n_barrels ; ++iLayer ) {
      for ( int iSide = 0; iSide < 2; ++iSide ) {
         for ( int iModule = 0; iModule < 2; ++iModule ) {
            ostringstream streamHist;
            streamHist << "h_phiVsNstrips_" << module[iModule] << "_" << iLayer << "Side" << iSide;
            std::string  histName = stem + streamHist.str();
            TProfile* hist_tmp = dynamic_cast<TProfile*>(m_inputHist->Get( histName.c_str() ));
            if (hist_tmp ==nullptr) {
               ATH_MSG_ERROR("Unable to get histogram for LorentzAngle : " << histName);
               return StatusCode::FAILURE;
            }
            m_h_phiVsNstripsSideHistoVector.push_back( hist_tmp );
         }
      }
   }

   //--- XML file
   const char* outputLorentzAngleFileName = m_LorentzAngleFile.value().c_str();
   ofstream outFile( outputLorentzAngleFileName, std::ios::out );
   if ( !outFile.good() ) {
      ATH_MSG_ERROR("Unable to open LorentzAngleFile : " << outputLorentzAngleFileName);
      return StatusCode::FAILURE;
   }

   //--- Header for XML outputs
   ostringstream osHeader;
   osHeader << "<folder>"<< endl;
   outFile << osHeader.str();

   fitFile = new TFile("FittingDebugFile.root","RECREATE");

   //--- Barrel
   for ( int iLayer = 0; iLayer < n_barrels; ++iLayer ) {
      for ( int iSide = 0; iSide < 2; ++iSide ) {
         for ( int iModule = 0; iModule < 2; ++iModule ) {
            if (iLayer==1 && iModule==0) continue; // Layer 1 doesn't contain 100 modules
            ATH_MSG_INFO("LorentzAngle fit start : " << 4*iLayer + iSide +1 + iModule << " / 16");
            Int_t fitResult;
            Double_t par[4],err_par[4];
            TF1 *LAfit = new TF1("LAfit", LA_func, -9., 2.,4 );
            ostringstream streamFile;
            streamFile << "h_phiVsNstrips_" << module[iModule] << "_" << iLayer << "Side" << iSide;

            LAfit->SetParLimits(3, 0.1, 50.);
            LAfit->SetParNames("a","LA","b","sigma");
            LAfit->SetParameters(1.,-5.,1.13,2.);
            fitResult = m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule ] -> Fit("LAfit", "E", "",-9.,2.);
            LAfit->GetParameters(par);
            err_par[0]=LAfit->GetParError(0);
            err_par[1]=LAfit->GetParError(1);
            err_par[2]=LAfit->GetParError(2);
            err_par[3]=LAfit->GetParError(3);

            //DEBUG MODE
            if ( m_LorentzAngleDebugMode  ) {
               ostringstream streamFileTmp;
               streamFileTmp << "h_phiVsNstrips_" << module[iModule] << "_" << iLayer << "Side" << iSide << "_First_Fit";
               std::string dn = streamFile.str();
               std::string tmp_hn = streamFileTmp.str();
               const char* dir_name = dn.c_str();
               const char* histo_name = tmp_hn.c_str();
               fitFile->cd();
               fitFile->mkdir(dir_name);            //Creating Directories
               fitFile->cd(dir_name);
               m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule] -> SetName(histo_name);
               m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule] -> Write();
               ATH_MSG_INFO("-------:Directory Name: " << dir_name << "--------");
            }



            if ( fitResult != 0 ) {
               ATH_MSG_INFO("Try to use parabola Fit to determine initial value!");
               TF1 *parafit = new TF1("parafit", "[0]*(x-[1])*(x-[1])+[2]", -9., 2. );
               ATH_MSG_INFO("LorentzAngle 2nd para fit start : " << 4*iLayer + iSide +1 + iModule << " / 16");
               parafit->SetParameters(par[0],par[1],LAfit->Eval(par[1],0,0,0));
               m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule ] -> Fit("parafit", "R", "",-9.,2.);
               ATH_MSG_INFO("LorentzAngle 2nd pre fit start : " << 4*iLayer + iSide +1 + iModule << " / 16");
               par[1]=parafit->GetParameter(1);
               LAfit->SetParameters(par[0],par[1],par[2],par[3]);
               LAfit->SetParLimits(1,par[1],par[1]);
               m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule] -> Fit("LAfit", "R", "",-9.,2.);
               LAfit->GetParameters(par);
               LAfit->SetParLimits(1, -90., 90.);
               LAfit->SetParameters(par[0],par[1],par[2],par[3]);
               ATH_MSG_INFO("LorentzAngle 2nd main fit start : " << 4*iLayer + iSide +1 + iModule << " / 16");
               fitResult = m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule] -> Fit("LAfit", "E", "",-9.,2.);
               LAfit->GetParameters(par);
               if ( m_LorentzAngleDebugMode  ) {

                  ostringstream streamFileTmp;
                  streamFileTmp << "h_phiVsNstrips_" << module[iModule] << "_" << iLayer << "Side" << iSide << "Second_Fit";
                  std::string tmp_hn = streamFileTmp.str();
                  const char* histo_name = tmp_hn.c_str();
                  m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule] -> SetName(histo_name);
                  m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule] -> Write();

               }

            }

            if ( fitResult != 0 ) {
               ATH_MSG_INFO("Try to fix one parameter sigma=2.0 to determine other initial value!");
               ATH_MSG_INFO("LorentzAngle 3rd pre fit start : " << 4*iLayer + iSide +1+ iModule << " / 16");
               LAfit->SetParameters(par[0],par[1],par[2],2.);
               LAfit->SetParLimits(3,2.,2.);
               m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule] -> Fit("LAfit", "R", "",-9.,2.);
               LAfit->GetParameters(par);
               LAfit->SetParLimits(3, 0., 50.);
               LAfit->SetParameters(par[0],par[1],par[2],par[3]);
               ATH_MSG_INFO("LorentzAngle 3rd main fit start : " << 4*iLayer + iSide +1 +iModule<< " / 16");
               fitResult = m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule] -> Fit("LAfit", "E", "",-9.,2.);
               LAfit->GetParameters(par);
               if ( m_LorentzAngleDebugMode  ) {
                  ostringstream streamFileTmp;
                  streamFileTmp << "h_phiVsNstrips_" << module[iModule] << "_" << iLayer << "Side" << iSide << "Third_Fit";
                  std::string tmp_hn = streamFileTmp.str();
                  const char* histo_name = tmp_hn.c_str();
                  m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule] -> SetName(histo_name);
                  m_h_phiVsNstripsSideHistoVector[ 4*iLayer + 2*iSide +iModule] -> Write();
               }

            }


            if ( fitResult == 0 ) {
               FitFlag[iLayer][iSide][iModule]= 1;
            } else {
               DBUploadFlag="R";
               FitFlag[iLayer][iSide][iModule]= 0;
               ATH_MSG_WARNING("Fit Failed! Unable to get LorentzAngle");
            }
            double A      = par[0];
            double LA     = par[1];                // Lorentz Angle
            double B      = par[2];
            double sigma  = par[3];
            double err_A      = err_par[0];
            double err_LA     = err_par[1];                // Lorentz Angle
            double err_B      = err_par[2];
            double err_sigma  = err_par[3];
            float MCW     = LAfit->Eval(LA,0,0,0); //Min-cluster-width
            float err_MCW     = LAfit->Eval(fabs(err_par[1]),0,0,0); //Min-cluster-width
            //      float lorentz = fabs(LA);

            A_BarrelSide[iLayer][iSide][iModule]= A;
            LA_BarrelSide[iLayer][iSide][iModule]=LA;
            B_BarrelSide[iLayer][iSide][iModule]=B;
            Sigma_BarrelSide[iLayer][iSide][iModule]=sigma;
            Err_A_BarrelSide[iLayer][iSide][iModule]= err_A;
            Err_LA_BarrelSide[iLayer][iSide][iModule]=err_LA;
            Err_B_BarrelSide[iLayer][iSide][iModule]=err_B;
            Err_Sigma_BarrelSide[iLayer][iSide][iModule]=err_sigma;
            MCW_BarrelSide[iLayer][iSide][iModule]=MCW;
            Err_MCW_BarrelSide[iLayer][iSide][iModule]=err_MCW;
            Chisq_BarrelSide[iLayer][iSide][iModule]=LAfit->GetChisquare();


         }

      }

   }


   if ( m_LorentzAngleDebugMode ) {
      fitFile->Close();
   }

   for ( int iLayer = 0; iLayer < n_barrels; ++iLayer ) {
      for ( int iSide = 0; iSide < 2; ++iSide ) {
         for ( int iModule = 0; iModule < 2; ++iModule ) {
            Identifier waferId = m_pSCTHelper->wafer_id( BARREL, iLayer, 0, 0, iSide );
            int ch = 0;
            outFile << "<folderDefinition folder=\"SCT/Derived/LorentzAngleRun2_v2\" version=\"multi\">"         << linefeed
                    << "  <folderDescription>" << linefeed
                    << "     <timeStamp>run-lumi</timeStamp>" << linefeed
                    << "     <addrHeader>" << linefeed
                    << "        <address_header service_type=\"71\" clid=\"1238547719\">" << linefeed
                    << "     </addrHeader>" << linefeed
                    << "     <typeName>CondAttrListCollection</typeName>" << linefeed
                    << "  </folderDescription>" << linefeed

                    << "  <payloadDescription>" << linefeed


                    << "    <payloadType name=\"moduleType\">"          <<  moduleint[iModule]  << "</payloadType>" << linefeed
                    << "    <payloadType name=\"lorentzAngle\">"        <<  LA_BarrelSide[iLayer][iSide][iModule] << "</payloadType>" << linefeed
                    << "    <payloadType name=\"err_lorentzAngle\">"    << Err_LA_BarrelSide[iLayer][iSide][iModule]<< "</payloadType>" << linefeed
                    << "    <payloadType name=\"chisq\">"               << Chisq_BarrelSide[iLayer][iSide][iModule]<< "</payloadType>" << linefeed
                    << "    <payloadType name=\"fitParam_a\">"          << A_BarrelSide[iLayer][iSide][iModule] << "</payloadType>" << linefeed
                    << "    <payloadType name=\"err_a\">"               << Err_A_BarrelSide[iLayer][iSide][iModule] << "</payloadType>" << linefeed
                    << "    <payloadType name=\"fitParam_b\">"          << B_BarrelSide[iLayer][iSide][iModule] << "</payloadType>" << linefeed
                    << "    <payloadType name=\"err_b\">"               << Err_B_BarrelSide[iLayer][iSide][iModule]      << "</payloadType>" << linefeed
                    << "    <payloadType name=\"fitParam_sigma\">"      << Sigma_BarrelSide[iLayer][iSide][iModule]                      << "</payloadType>" << linefeed
                    << "    <payloadType name=\"err_sigma\">"           << Err_Sigma_BarrelSide[iLayer][iSide][iModule]      << "</payloadType>" << linefeed
                    << "    <payloadType name=\"minClusterWidth\">"     << MCW_BarrelSide[iLayer][iSide][iModule]  << "</payloadType>" << linefeed
                    << "    <payloadType name=\"err_minClusterWidth\">" << Err_MCW_BarrelSide[iLayer][iSide][iModule]      << "</payloadType>" << linefeed

                    << "  </payloadDescription>" <<linefeed
                    << "  <channel id=\"" << ch << "\" name=\""<<iLayer<< "_"<<iSide<<" \" />" << linefeed
                    << "</folderDefinition>" << endl;

            ch++;

            //--- DB output
            if ( m_writeToCool ) {
               //  if ( m_pCalibWriteTool->createListLA( waferId, m_pSCTHelper, 10000, moduleint[iModule], LA_BarrelSide[iLayer][iSide][iModule],MCW_BarrelSide[iLayer][iSide][iModule] ).isFailure() ) {
               if ( m_pCalibWriteTool->createListLA( waferId, m_pSCTHelper, 10000, moduleint[iModule], LA_BarrelSide[iLayer][iSide][iModule], Err_LA_BarrelSide[iLayer][iSide][iModule],  Chisq_BarrelSide[iLayer][iSide][iModule], A_BarrelSide[iLayer][iSide][iModule], Err_A_BarrelSide[iLayer][iSide][iModule], B_BarrelSide[iLayer][iSide][iModule], Err_B_BarrelSide[iLayer][iSide][iModule], Sigma_BarrelSide[iLayer][iSide][iModule], Err_Sigma_BarrelSide[iLayer][iSide][iModule], MCW_BarrelSide[iLayer][iSide][iModule], Err_MCW_BarrelSide[iLayer][iSide][iModule] ).isFailure() ) {
                  ATH_MSG_ERROR("Unable to run createListLA");
                  return StatusCode::FAILURE;
               }
            }

         }
      }
   }

   //--- Tail of XML outputs
   outFile << "</folder>" << endl;

   //--- Summary XML output
   ostringstream summaryList;
   for ( int i = 0; i < n_barrels; ++i ) {
      for ( int iSide = 0; iSide < 2; ++iSide ) {
         for ( int iModule = 0; iModule < 2; ++iModule ) {
            const std::string thisPart=shortNames[bec2Index(BARREL)];
            summaryList<< "    <parts>" << linefeed
                       << xmlValue("part",thisPart) << linefeed
                       << xmlValue("layer", i) << linefeed
                       << xmlValue("Side", iSide) << linefeed
                       << xmlValue("Module", module[iModule]) << linefeed
                       << xmlValue("lorentzAngle", LA_BarrelSide[i][iSide][iModule]) << linefeed
                       << xmlValue("minClusterWidth",  MCW_BarrelSide[i][iSide][iModule]) << linefeed
                       << xmlValue("Fit",  FitFlag[i][iSide][iModule]) << linefeed
                       <<"    </parts>" << linefeed;
         }
      }
   }

   std::ofstream & file = m_outLASummary;
   typedef std::pair<string, string> TwoStrings;
   typedef std::map<std::string, TwoStrings > Names;
   Names nameAssociation;
   nameAssociation["LorentzAngle"]=TwoStrings(m_LorentzAngleSummaryFile, "LorentzAngleInfo.xsl");
   Names::iterator found=nameAssociation.find("LorentzAngle");
   if (found!=nameAssociation.end()) {
      std::string filename=found->second.first;
      std::string xslName=found->second.second;
      file.open( filename.c_str(), std::ios::out );
      if ( !file.good() ) return StatusCode::FAILURE;
      file << xmlHeader<< linefeed<< associateStylesheet(xslName) << linefeed<< "<run>"<< endl;
   }
   else {
      ATH_MSG_ERROR(" argument \"type\" needs to be  LorentzAngle.");
      return StatusCode::FAILURE;
   }

   file << xmlValue("RunNumber",  m_runNumber.value()) << linefeed
        << xmlValue("StartTime",  m_utcBegin) << linefeed
        << xmlValue("EndTime",  m_utcEnd) << linefeed
        << xmlValue("Duration",  m_calibEvtInfoTool->duration() ) << linefeed
        << xmlValue("LB",  m_LBRange) << linefeed
        << xmlValue("Events",  m_numberOfEvents) << linefeed
        << xmlValue("Flag",  DBUploadFlag) << linefeed
        << "  <data>"<< endl;


   if ( wrapUpXML4Summary( m_outLASummary, "LorentzAngle", summaryList ).isFailure() ) {
      ATH_MSG_ERROR("Problem in closing LorentzAngle file");
      return StatusCode::FAILURE;
   }

   //--- DB output
   if ( m_writeToCool ) {
      if ( m_pCalibWriteTool->wrapUpLorentzAngle().isFailure() ) {
         ATH_MSG_ERROR("Could not get LorentzAngle");
         return StatusCode::FAILURE;
      }
   }

   return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////////
// Functions to handle XML File for COOL
///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::openXML4DB( std::ofstream& file, const char* type, const char* tag, IOVTime start, IOVTime end ) const {
   if ( !strcmp( type, "DeadStrip" ) ) {
      file.open( m_deadStripsFile.value().c_str(), std::ios::out );
      if ( !file.good() ) return StatusCode::FAILURE;
      file << "<channels server=\"ATLAS_COOLPROD\" schema=\"ATLAS_COOLOFL_SCT\" dbname=\"MONP200\" folder=\"SCT/Derived/DeadStrips\" ";
   } else if ( !strcmp( type, "DeadChip" ) ) {
      file.open( m_deadChipsFile.value().c_str(), std::ios::out );
      if ( !file.good() ) return StatusCode::FAILURE;
      file << "<channels server=\"ATLAS_COOLPROD\" schema=\"ATLAS_COOLOFL_SCT\" dbname=\"MONP200\" folder=\"SCT/Derived/DeadChips\" ";
   } else {
      ATH_MSG_ERROR("in openXML4DB : argument \"type\" needs to be (DeadStrip, DeadChip).");
      return StatusCode::FAILURE;
   }
   file << "since=\""   << start.re_time() << "\" "
        << "until=\""   << end.re_time()   << "\" "
        << "tag=\""     << tag             << "\" "
        << "version=\"" << "multi\">"      << linefeed;
   return StatusCode::SUCCESS;
}

StatusCode SCTCalib::closeXML4DB( std::ofstream& file ) const {
   file << "</channels>" << endl;
   if ( file.is_open() ) {
      file.close();
      return StatusCode::SUCCESS;
   } else {
      return StatusCode::FAILURE;
   }
}

StatusCode SCTCalib::addToXML4DB( std::ofstream& file, const Identifier& waferId, const char* DefectType, float Threshold, const char* DefectList ) const {
   std::string tmp = DefectList;
   int length = tmp.length();
   std::string Defect4DB = tmp.substr( 1, length-2 ); // Removing first&end spaces in DefectList

   file << xmlOpenChannel( m_pSCTHelper->module_id( waferId ).get_identifier32().get_compact(), m_iovStart.re_time(), m_iovStop.re_time() ) << linefeed
        << xmlValue("SampleSize","10000") << linefeed
        << xmlValue("BarrelEndcap", m_pSCTHelper->barrel_ec( waferId ) )<< linefeed
        << xmlValue("Layer", m_pSCTHelper->layer_disk( waferId ))<< linefeed
        << xmlValue("Eta",  m_pSCTHelper->eta_module( waferId )) << linefeed
        << xmlValue("Phi", m_pSCTHelper->phi_module( waferId )) << linefeed
        << xmlValue("DefectType",  DefectType)<< linefeed
        << xmlValue("Threshold", Threshold ) << linefeed
        << xmlValue("DefectList",Defect4DB) << linefeed
        << xmlCloseChannel() << endl;

   return StatusCode::SUCCESS;

}

///////////////////////////////////////////////////////////////////////////////////
// Functions to handle XML File for Summary
///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalib::openXML4DeadSummary( std::ofstream& file, const char* type, int n_Module, int n_Link, int n_Chip, int n_Strip ) const {
   if ( !strcmp( type, "DEAD" ) ) {
      file.open( m_deadSummaryFile.value().c_str(), std::ios::out );
      if ( !file.good() ) return StatusCode::FAILURE;
      file << xmlHeader<< linefeed<< associateStylesheet("DeadInfo.xsl") << linefeed
           << "<run>"<< linefeed;
   } else {
      ATH_MSG_ERROR("in openXML4DeadSummary : argument \"type\" needs to be \"DEAD\".");
      return StatusCode::FAILURE;
   }

   //--- Upload flag
   string strUploadFlag = "U";
   bool isNonZero( false );

   if ( (m_doDeadChip && m_deadChipUploadTest) || (m_doDeadStrip && m_deadStripUploadTest) ) {
      if ( n_Chip > 0 ) {
         isNonZero = true;
         strUploadFlag = "G";
      } else {
         strUploadFlag = "R";
      }
   }

   //--- Upload test result
   ostringstream osNonZero;
   osNonZero << "#chips or #strips is non-zero";
   ostringstream osFlagReason;
   if ( !isNonZero ) osFlagReason << "FAILED in " << osNonZero.str();
   string strFlagEnable = ( m_deadChipUploadTest || m_deadStripUploadTest ) ? "ENABLED" : "DISABLED";
   ostringstream osCheckList;
   osCheckList << osNonZero.str();

   file <<xmlValue("RunNumber",      m_runNumber.value())               << linefeed
        <<xmlValue("StartTime",      m_utcBegin)                        << linefeed
        <<xmlValue("EndTime",        m_utcEnd)                          << linefeed
        <<xmlValue("Duration",       m_calibEvtInfoTool->duration())     << linefeed
        <<xmlValue("LB",             m_calibEvtInfoTool->numLumiBlocks())<< linefeed
        <<xmlValue("Events",         m_numberOfEvents )                 << linefeed
        <<xmlValue("Modules",        n_Module)                          << linefeed
        <<xmlValue("Links",          n_Link)                            << linefeed
        <<xmlValue("Chips",          n_Chip)                            << linefeed
        <<xmlValue("Strips",         n_Strip)                           << linefeed
        <<xmlValue("Flag",           strUploadFlag)                     << linefeed
        <<xmlValue("FlagReason",     osFlagReason.str())                << linefeed
        <<xmlValue("FlagEnable",     strFlagEnable)                     << linefeed
        <<xmlValue("CheckList",      osCheckList.str())                 << linefeed
        <<"  <modules>"                                                 << endl;

   return StatusCode::SUCCESS;
}


StatusCode SCTCalib::openXML4MonSummary( std::ofstream& file, const char* type ) const {
   typedef std::pair<string, string> TwoStrings;
   typedef std::map<std::string, TwoStrings > Names;
   Names nameAssociation;
   nameAssociation["NoiseOccupancy"]=TwoStrings(m_noiseOccupancySummaryFile, "NoiseOccupancyInfo.xsl");
   nameAssociation["RawOccupancy"]=TwoStrings(m_rawOccupancySummaryFile, "RawOccupancyInfo.xsl");
   nameAssociation["Efficiency"]=TwoStrings(m_efficiencySummaryFile, "EfficiencyInfo.xsl");
   nameAssociation["BSErrors"]=TwoStrings(m_BSErrorSummaryFile, "BSErrorInfo.xsl");
   nameAssociation["BSErrorsModule"]=TwoStrings(m_BSErrorModuleFile, "BSErrorInfo.xsl");
   //  nameAssociation["LorentzAngle"]=TwoStrings(m_LorentzAngleSummaryFile, "LorentzAngleInfo.xsl");
   Names::iterator found=nameAssociation.find(type);
   if (found!=nameAssociation.end()) {
      std::string filename=found->second.first;
      std::string xslName=found->second.second;
      //
      file.open( filename.c_str(), std::ios::out );
      if ( !file.good() ) return StatusCode::FAILURE;
      file << xmlHeader<< linefeed<< associateStylesheet(xslName) << linefeed<< "<run>"<< endl;
   } else {
      ATH_MSG_ERROR("in openXML4MonSummary : argument \"type\" needs to be ( NoiseOccupancy, RawOccupancy, Efficiency, BSErrors ).");
      return StatusCode::FAILURE;
   }
   file << xmlValue("RunNumber", m_runNumber.value() ) << linefeed
        << xmlValue("StartTime",  m_utcBegin                        ) << linefeed
        << xmlValue("EndTime",  m_utcEnd                          ) << linefeed
        << xmlValue("Duration",  m_calibEvtInfoTool->duration() ) << linefeed
        << xmlValue("LB",  m_LBRange                         ) << linefeed
        << xmlValue("Events",  m_numberOfEvents                  ) << linefeed
        << "  <data>"<< endl;
   return StatusCode::SUCCESS;
}


StatusCode SCTCalib::wrapUpXML4Summary( std::ofstream& file, const char* type, std::ostringstream& list ) const {
   file << list.str();
   if ( !strcmp( type, "DEAD" ) ) {
      file << "  </modules>" << endl;
   } else if ( !strcmp( type, "NoiseOccupancy" ) || !strcmp( type, "RawOccupancy" ) || !strcmp( type, "Efficiency" ) || !strcmp( type, "BSErrors" ) || !strcmp( type, "LorentzAngle" ) ) {
      file << "  </data>" << endl;
   }
   file << "</run>" << endl;

   if ( file.is_open() ) {
      file.close();
      return StatusCode::SUCCESS;
   } else {
      return StatusCode::FAILURE;
   }

}


StatusCode SCTCalib::addToSummaryStr( std::ostringstream& list, const Identifier& waferId, const char* type, const char* stripId, const char* chipId ) const {
   //--- Remove first&end spaces in DefectList
   const std::string tmpstrip(stripId);
   const std::string tmpchip(chipId);
   int len_strip = tmpstrip.length();
   int len_chip  = tmpchip.length();
   std::string stripList("");
   std::string chipList("");
   if ( len_strip > 0 ) stripList = tmpstrip.substr( 1, len_strip-2 );
   if ( len_chip  > 0 ) chipList  = tmpchip.substr( 1, len_chip-2 );
   //--- Identifier/SN
   IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
   SCT_SerialNumber sn        = m_CablingTool->getSerialNumberFromHash( waferHash );
   //--- Preparing linkList
   //std::string linkList=chipList2LinkList(chipList);
   std::string linkList=chipList2LinkList(stripList);
   //--- Push to summary stream
   XmlStreamer m("module", list);
   {
      XmlStreamer v("value", "name", "SN",list);
      list<<sn.str();
   }
   {
      XmlStreamer v("value", "name", "BecLayerPhiEta", list);
      list<<formatPosition(waferId, m_pSCTHelper,".",false);
   }
   {
      XmlStreamer v("value", "name", "LinkID", list);
      list<<linkList;
   }
   //  {XmlStreamer v("value", "name", "ChipID", list); list<<chipList;}
   {
      XmlStreamer v("value", "name", "ChipID", list);
      list<<stripList;
   }
   if ( !strcmp( type, "DEAD" ) ) {

      XmlStreamer v("value", "name", "StripIDOnline", list);
      list << stripList;

   } else {

      ATH_MSG_ERROR("in addToSummaryStr : argument \"type\" needs to be \"DEAD\".");
      return StatusCode::FAILURE;

   }

   return StatusCode::SUCCESS;

}

std::string
SCTCalib::xmlChannelNoiseOccDataString(const Identifier & waferId,  const float occupancy, const SCT_SerialNumber & serial) const {
   //agrohsje added space and ostringstream for proper xml output
   ostringstream os;
   os<<xmlOpenChannel(waferId.get_identifier32().get_compact(), m_iovStart.re_time(), m_iovStop.re_time())<<endl
     <<"  "<<xmlValue("SN", serial.str())<<endl
     <<"  "<<xmlValue("SampleSize",  "10000")<<endl
     <<"  "<<xmlValue("barrel_endcap",  m_pSCTHelper->barrel_ec( waferId )  )<<endl
     <<"  "<<xmlValue("Layer",  m_pSCTHelper->layer_disk( waferId ) )<< linefeed
     <<"  "<<xmlValue("Eta",  m_pSCTHelper->eta_module( waferId ) )<<endl
     <<"  "<<xmlValue("Phi",  m_pSCTHelper->phi_module( waferId ) )<<endl
     <<"  "<<xmlValue("NoiseOccupancy", occupancy)<<endl
     <<"  "<<xmlCloseChannel();
   return os.str();
}

//agasconb 02.02.2015: block for Efficiency output
//requested by Naoki Ishijima
std::string
SCTCalib::xmlChannelEfficiencyDataString(const Identifier & waferId,  const float efficiency, const SCT_SerialNumber & serial) const {
   ostringstream os;
   os<<"   <module>"<<endl
     //    os<<xmlOpenChannel(waferId.get_identifier32().get_compact(), m_iovStart.re_time(), m_iovStop.re_time())<<endl
     <<"  "<<xmlValue("SN", serial.str())<<endl
     <<"  "<<xmlValue("SampleSize",  "10000")<<endl
     <<"  "<<xmlValue("barrel_endcap",  m_pSCTHelper->barrel_ec( waferId )  )<<endl
     <<"  "<<xmlValue("Layer",  m_pSCTHelper->layer_disk( waferId ) )<< linefeed
     <<"  "<<xmlValue("Eta",  m_pSCTHelper->eta_module( waferId ) )<<endl
     <<"  "<<xmlValue("Phi",  m_pSCTHelper->phi_module( waferId ) )<<endl
     <<"  "<<xmlValue("Efficiency", efficiency)<<endl
     <<"   </module>";
   //      <<"  "<<xmlCloseChannel();
   return os.str();
}

std::pair< int, bool >
SCTCalib::getNumNoisyStrips( const Identifier& waferId ) const {
   IdentifierHash waferHash = m_pSCTHelper->wafer_hash( waferId );
   //--- Check if there are noisy strips in the wafer
   int numNoisyStripsInTheWafer = 0;
   bool isNoisyWafer(false);
   float noisyStripThr = m_noisyStripThrDef?(m_noisyStripThrOffline):(m_noisyStripThrOnline);
   for ( int iStrip = 0; iStrip != nbins; ++iStrip ) {
      if ( m_calibHitmapTool->getBinForHistogramIndex( iStrip + 1, waferHash.value() ) / m_numberOfEvents > noisyStripThr ) ++numNoisyStripsInTheWafer;
   }
   //--- Define/counts noisy wafers using wafer occupancy and number of noisy strips
   double averageOccupancy = m_calibHitmapTool->size(waferHash.value())/static_cast<double>(nbins)/static_cast<double>(m_numberOfEvents);
   const int subdetector(m_pSCTHelper->barrel_ec( waferId ) );
   isNoisyWafer = ( numNoisyStripsInTheWafer > m_noisyWaferFraction*nbins ) and
                  ( ( subdetector == ENDCAP_C && averageOccupancy > m_noisyWaferThrECC) or
                    ( subdetector == BARREL   && averageOccupancy > m_noisyWaferThrBarrel) or
                    ( subdetector == ENDCAP_A && averageOccupancy > m_noisyWaferThrECA) );
   if (isNoisyWafer) {
      ATH_MSG_INFO("Module: " << waferHash.value());
      ATH_MSG_INFO("Hits, Nevts, Occ: " << m_calibHitmapTool->size(waferHash.value()) << ", "
                   << m_numberOfEvents << ", "
                   << averageOccupancy);
   }
   return std::make_pair( numNoisyStripsInTheWafer, isNoisyWafer );
}

StatusCode
SCTCalib::addStripsToList( Identifier& waferId, std::set<Identifier>& stripIdList, bool isNoisy, bool isNew ) {
   IdentifierHash waferHash = m_pSCTHelper->wafer_hash( waferId );
   float noisyStripThr = m_noisyStripThrDef?(m_noisyStripThrOffline):(m_noisyStripThrOnline);
   for ( int iStrip = 0; iStrip != nbins; ++iStrip ) {
      Identifier stripId = m_pSCTHelper->strip_id( waferId, iStrip );
      if ( !isNoisy ) { //--- Add all strips
         stripIdList.insert( stripId );
      } else {
         const float stripOccupancy =  m_calibHitmapTool->getBinForHistogramIndex( iStrip + 1, waferHash.value() )  / m_numberOfEvents;
         if ( stripOccupancy > noisyStripThr ) {
            if ( !isNew ) { //--- All noisy strips
               stripIdList.insert( stripId );
            } else { //--- New noisy strips : compared with configuration and calibration
               const bool isGoodInConfiguration = m_useConfiguration ? m_ConfigurationConditionsTool->isGood( stripId, InDetConditions::SCT_STRIP ) : true;
               const bool isGoodInCalibration   = m_useCalibration   ? m_ReadCalibDataTool->isGood( stripId, InDetConditions::SCT_STRIP )           : true;
               if ( m_useConfiguration or m_useCalibration ) {
                  if ( isGoodInConfiguration && isGoodInCalibration ) {
                     stripIdList.insert( stripId );
                  }
               }
            }
         }
      }
   }
   return StatusCode::SUCCESS;
}

StatusCode
SCTCalib::writeModuleListToCool( const std::map< Identifier, std::set<Identifier> >& moduleListAll,
                                 const std::map< Identifier, std::set<Identifier> >& moduleListNew,
                                 const std::map< Identifier, std::set<Identifier> >& moduleListRef ) const {
   //--- Write out strips
   float noisyStripThr = m_noisyStripThrDef?(m_noisyStripThrOffline):(m_noisyStripThrOnline);
   int nDefects = 0;
   SCT_ID::const_id_iterator idItr  = m_pSCTHelper->wafer_begin();
   SCT_ID::const_id_iterator idItrE = m_pSCTHelper->wafer_end();
   for ( ; idItr != idItrE; ++idItr ) {
      if ( m_pSCTHelper->side( *idItr ) == 0 ) {
         Identifier moduleId = m_pSCTHelper->module_id( *idItr );
         map< Identifier, std::set<Identifier> >::const_iterator moduleAllItr = moduleListAll.find( moduleId );
         map< Identifier, std::set<Identifier> >::const_iterator moduleNewItr = moduleListNew.find( moduleId );
         map< Identifier, std::set<Identifier> >::const_iterator moduleRefItr = moduleListRef.find( moduleId );
         std::string defectStripsAll =  moduleAllItr != moduleListAll.end() ? getStripList( (*moduleAllItr).second ) : "";
         std::string defectStripsNew =  moduleNewItr != moduleListNew.end() ? getStripList( (*moduleNewItr).second ) : "";
         std::string defectStripsRef =  moduleRefItr != moduleListRef.end() ? getStripList( (*moduleRefItr).second ) : "";
         if ( m_noisyUpdate ) { //--- UPD1/UPD4
            if ( defectStripsAll != defectStripsRef ) {
               if (m_pCalibWriteTool->createCondObjects( moduleId, m_pSCTHelper, 10000, "NOISY", noisyStripThr, defectStripsAll ).isFailure()) {
                  ATH_MSG_ERROR("Could not create defect strip entry in the CalibWriteTool.");
               }
               nDefects++;
            } else ATH_MSG_DEBUG("Module "<< moduleId  <<" is identical to the reference output");
         } else {
            if ( m_noisyStripAll ) { //--- ALL noisy strips
               if ( !defectStripsAll.empty() ) m_pCalibWriteTool->createCondObjects( moduleId, m_pSCTHelper, 10000, "NOISY", noisyStripThr, defectStripsAll );
            } else { //--- Only NEW noisy strips
               if ( !defectStripsNew.empty() ) m_pCalibWriteTool->createCondObjects( moduleId, m_pSCTHelper, 10000, "NOISY", noisyStripThr, defectStripsNew );
            }
         }
      }
   }
   //ATH_MSG_INFO("Number of modules for which conditions were created: " << nDefects <<"  !!!!"<< endmsg;
   if ( moduleListAll.empty() || nDefects==0 ) {
      ATH_MSG_INFO("Number of noisy strips was zero or the same list of noisy strips. No local DB was created.");
   } else {
      if ( m_pCalibWriteTool->wrapUpNoisyChannel().isFailure() ) {
         ATH_MSG_ERROR("Could not get NoisyStrips info");
         return StatusCode::FAILURE;
      }
   }
   return StatusCode::SUCCESS;
}

std::string
SCTCalib::getStripList( const std::set<Identifier>& stripIdList ) const {
   std::string strList;
   if ( !stripIdList.empty() ) {
      int firstStrip = -1;
      int groupSize  = -1;

      std::set<Identifier>::const_iterator stripItrFirst = stripIdList.begin();
      std::set<Identifier>::const_iterator stripItrLast  = --stripIdList.end();

      std::set<Identifier>::const_iterator stripItr  = stripIdList.begin();
      std::set<Identifier>::const_iterator stripItrE = stripIdList.end();
      for ( ; stripItr != stripItrE; ++stripItr ) {
         Identifier stripId = *stripItr;
         int stripNum = m_pSCTHelper->side( stripId )*nbins + m_pSCTHelper->strip( stripId );
         if ( stripItr == stripItrFirst ) {
            firstStrip = stripNum;
            groupSize  = 1;
         } else {
            if ( stripNum == firstStrip + groupSize ) {
               ++groupSize;
            } else {
               int stripBegin = firstStrip;
               int stripEnd   = firstStrip + groupSize -1;
               strList = m_pCalibWriteTool->addDefect( strList, stripBegin, stripEnd );
               firstStrip = stripNum;
               groupSize  = 1;
            }
         }
         if ( stripItr == stripItrLast ) {
            int stripBegin = firstStrip;
            int stripEnd   = stripNum;
            strList = m_pCalibWriteTool->addDefect( strList, stripBegin, stripEnd );
         }
      }
   }
   return strList;
}

StatusCode
SCTCalib::noisyStripsToXml( const std::map< Identifier, std::set<Identifier> >& moduleList, const std::string& badStripsFile ) const {
   //--- Open
   const char* outputFileName = badStripsFile.c_str();
   ofstream outFile( outputFileName, std::ios::out );
   if ( !outFile.good() ) {
      ATH_MSG_ERROR("Unable to open " << outputFileName);
      return( StatusCode::FAILURE );
   }
   float noisyStripThr = m_noisyStripThrDef?(m_noisyStripThrOffline):(m_noisyStripThrOnline);
   //--- Create module list
   ostringstream osModuleList;
   //--- Loop over wafers
   SCT_ID::const_id_iterator waferItr  = m_pSCTHelper->wafer_begin();
   SCT_ID::const_id_iterator waferItrE = m_pSCTHelper->wafer_end();
   for ( ; waferItr != waferItrE; ++waferItr ) {
      Identifier waferId  = *waferItr;
      Identifier moduleId = m_pSCTHelper->module_id( waferId );
      if ( m_pSCTHelper->side( waferId ) != 0 ) continue;
      map< Identifier, std::set<Identifier> >::const_iterator moduleItr = moduleList.find( moduleId );
      if ( moduleItr != moduleList.end() ) {
         std::string defectStrips = getStripList( (*moduleItr).second );
         osModuleList << "  <channel id=\""    << m_pSCTHelper->module_id( waferId ).get_compact() << "\" "
                      <<            "since=\"" << m_iovStart.re_time()                             << "\" "
                      <<            "until=\"" << m_iovStop.re_time()                              << "\">"       << linefeed
                      << "    <value name=\"SampleSize\">"   << "10000"                            << "</value>" << linefeed
                      << "    <value name=\"BarrelEndcap\">" << m_pSCTHelper->barrel_ec( waferId ) << "</value>" << linefeed
                      << "    <value name=\"Layer\">"        << m_pSCTHelper->layer_disk( waferId )<< "</value>" << linefeed
                      << "    <value name=\"Eta\">"          << m_pSCTHelper->eta_module( waferId )<< "</value>" << linefeed
                      << "    <value name=\"Phi\">"          << m_pSCTHelper->phi_module( waferId )<< "</value>" << linefeed
                      << "    <value name=\"DefectType\">"   << "NOISY"                            << "</value>" << linefeed
                      << "    <value name=\"Threshold\">"    << noisyStripThr                      << "</value>" << linefeed
                      << "    <value name=\"DefectList\">"   << normalizeList( defectStrips )        << "</value>" << linefeed
                      << "  </channel>"                                                                          << endl;
      }
   }
   //--- Write out the contents
   outFile << "<channels server=\"ATLAS_COOLPROD\" schema=\"ATLAS_COOLOFL_SCT\" dbname=\"CONDBR2\" folder=\"SCT/Derived/Monitoring\" "
           <<           "since=\""   << m_iovStart.re_time() << "\" "
           <<           "until=\""   << m_iovStop.re_time()  << "\" "
           <<           "tag=\""     << m_tagID4NoisyStrips  << "\" "
           <<           "version=\"" << "multi\">"           << endl
           <<    osModuleList.str()
           << "</channels>" << endl;

   return StatusCode::SUCCESS;
}

StatusCode SCTCalib::noisyStripsToSummaryXml( const std::map< Identifier, std::set<Identifier> >& moduleListAll,
      const std::map< Identifier, std::set<Identifier> >& moduleListNew,
      const std::map< Identifier, std::set<Identifier> >& moduleListRef,
      const std::string& badStripsFile) const {
   //--- Open
   const char* outputFileName = badStripsFile.c_str();
   ofstream outFile( outputFileName, std::ios::out );
   if ( !outFile.good() ) {
      ATH_MSG_ERROR("Unable to open " << outputFileName);
      return( StatusCode::FAILURE );
   }

   //--- Initialization
   int numLinksAll( 0 ), numChipsAll( 0 );
   int numModulesAll( 0 ), numModulesNew( 0 ), numModulesRef( 0 );
   int numStripsAll( 0 ), numStripsNew( 0 ), numStripsRef( 0 );
   int numModulesDiff( 0 );

   std::string defectLinks, defectChips;
   std::string defectStripsAll, defectStripsNew, defectStripsRef;
   ostringstream osModuleList, osChipList;

   //--- Create module list
   SCT_ID::const_id_iterator waferItr  = m_pSCTHelper->wafer_begin();
   SCT_ID::const_id_iterator waferItrE = m_pSCTHelper->wafer_end();
   for ( ; waferItr != waferItrE; ++waferItr ) {
      //--- Identifier
      Identifier       waferId   = *waferItr;
      Identifier       moduleId  = m_pSCTHelper->module_id( waferId );
      IdentifierHash   waferHash = m_pSCTHelper->wafer_hash( waferId );
      SCT_SerialNumber sn        = m_CablingTool->getSerialNumberFromHash( waferHash );

      //--- Initialization for a module
      if ( m_pSCTHelper->side( waferId ) == 0 ) {
         defectLinks.erase();
         defectChips.erase();
         defectStripsAll.erase();
         defectStripsNew.erase();
         defectStripsRef.erase();
      }

      //--- Noisy links
      bool isNoisyWafer = getNumNoisyStrips( waferId ).second; // true if this wafer is noisy
      if ( isNoisyWafer ) {
         int link = m_pSCTHelper->side( waferId );
         defectLinks = m_pCalibWriteTool->addDefect( defectLinks, link, link );
         ++numLinksAll;
      }

      //--- Execute once in this module
      if ( m_pSCTHelper->side( waferId ) == 1 ) {
         //--- Noisy strips : All
         map< Identifier, std::set<Identifier> >::const_iterator moduleAllItr = moduleListAll.find( moduleId );
         if ( moduleAllItr != moduleListAll.end() ) {
            defectStripsAll = getStripList( (*moduleAllItr).second );
            ++numModulesAll;
            numStripsAll += (*moduleAllItr).second.size();
         }
         //--- Noisy strips : New
         map< Identifier, std::set<Identifier> >::const_iterator moduleNewItr = moduleListNew.find( moduleId );
         if ( moduleNewItr != moduleListNew.end() ) {
            defectStripsNew = getStripList( (*moduleNewItr).second );
            ++numModulesNew;
            numStripsNew += (*moduleNewItr).second.size();
         }
         //--- Noisy strips : Ref
         map< Identifier, std::set<Identifier> >::const_iterator moduleRefItr = moduleListRef.find( moduleId );
         if ( moduleRefItr != moduleListRef.end() ) {
            defectStripsRef = getStripList( moduleRefItr->second );
            ++numModulesRef;
            numStripsRef += moduleRefItr->second.size();
         }

         //--- Noisy chips : stripIdList -> chipIdList
         if ( moduleAllItr != moduleListAll.end() ) {
            std::set<int> chipIdList = getNoisyChips( moduleAllItr->second );
            if ( !chipIdList.empty() ) {
               ++numChipsAll; // CS: I think we should count this!?
               std::set<int>::iterator chipItr  = chipIdList.begin();
               std::set<int>::iterator chipItrE = chipIdList.end();
               for ( ; chipItr != chipItrE; ++chipItr ) {
                  int chipId = *chipItr;
                  //--- To be written into module list
                  defectChips = m_pCalibWriteTool->addDefect( defectChips, chipId, chipId );
                  //--- LBs where this chip was noisy
                  std::pair< string, float > defectLB = getNoisyLB( moduleId, chipId );
                  //--- Chip list written to XML
                  osChipList << "    <chip>"                                                                                 << linefeed
                             << "      <value name=\"SN\">"             << sn.str()                            << "</value>" << linefeed
                             << "      <value name=\"BecLayerPhiEta\">" << m_pSCTHelper->barrel_ec( waferId )  << "."
                             <<                                            m_pSCTHelper->layer_disk( waferId ) << "."
                             <<                                            m_pSCTHelper->phi_module( waferId ) << "."
                             <<                                            m_pSCTHelper->eta_module( waferId ) << "</value>" << linefeed
                             << "      <value name=\"ChipID\">"         << chipId                              << "</value>" << linefeed
                             << "      <value name=\"LB\">"             << normalizeList( defectLB.first )     << "</value>" << linefeed
                             << "      <value name=\"LBFraction\">"     << defectLB.second                     << "</value>" << linefeed
                             << "    </chip>"                                                                                << endl;
               }
            }
         }
         //--- Difference between All & Ref
         if ( defectStripsAll != defectStripsRef ) ++numModulesDiff;
         //--- Module list written to XML
         if ( !defectStripsAll.empty() || ( m_noisyUpdate && defectStripsAll != defectStripsRef ) ) {
            osModuleList << "    <module>"                                                                                << linefeed
                         << "      <value name=\"SN\">"              << sn.str()                            << "</value>" << linefeed
                         << "      <value name=\"BecLayerPhiEta\">"  << m_pSCTHelper->barrel_ec( waferId )  << "."
                         <<                                             m_pSCTHelper->layer_disk( waferId ) << "."
                         <<                                             m_pSCTHelper->phi_module( waferId ) << "."
                         <<                                             m_pSCTHelper->eta_module( waferId ) << "</value>" << linefeed
                         << "      <value name=\"LinkID\">"          << normalizeList( defectLinks )        << "</value>" << linefeed
                         << "      <value name=\"ChipID\">"          << normalizeList( defectChips )        << "</value>" << linefeed
                         << "      <value name=\"StripOfflineAll\">" << normalizeList( defectStripsAll )    << "</value>" << linefeed
                         << "      <value name=\"StripOfflineNew\">" << normalizeList( defectStripsNew )    << "</value>" << linefeed
                         << "      <value name=\"StripOfflineRef\">" << normalizeList( defectStripsRef )    << "</value>" << linefeed
                         << "    </module>"                                                                               << endl;
         }
      }
   }//--- end loop : waferItr

   //--- Upload flag
   string strUploadFlag = "U";

   bool isRunsInCool( false );
   bool isNoisyMinStat( false ), isNoisyModuleList( false ), isNoisyModuleDiff( false ), isNoisyStripDiff( false );
   if ( m_noisyUploadTest ) {
      isRunsInCool = m_noisyModuleAverageInDB != -1. && m_noisyStripLastRunInDB != -999;
      if ( isRunsInCool ) {
         isNoisyMinStat    = m_numberOfEvents > m_noisyMinStat;
         isNoisyModuleList = numModulesAll < m_noisyModuleList;
         isNoisyModuleDiff = ( ( static_cast<float>(numModulesAll) - m_noisyModuleAverageInDB )/m_noisyModuleAverageInDB ) < m_noisyModuleDiff;
         isNoisyStripDiff  = ( numStripsAll - m_noisyStripAverageInDB ) < m_noisyStripDiff;
         if ( !isNoisyMinStat || !isNoisyModuleList ) {
            strUploadFlag = "R";
         } else {
            if ( !isNoisyModuleDiff || !isNoisyStripDiff ) {
               strUploadFlag = "Y";
            } else {
               strUploadFlag = "G";
            }
         }
      }
   }

   //--- Upload test result to XML
   ostringstream osNoisyMinStat, osNoisyModuleList, osNoisyModuleDiff, osNoisyStripDiff;
   osNoisyMinStat    << "#events more than "                                                                      << m_noisyMinStat.value();
   osNoisyModuleList << "#(modules w/ at least 1 noisy strip) less than "                                         << m_noisyModuleList.value();
   osNoisyModuleDiff << "Increase of #(modules w/ at least 1 noisy strip) from average of recent runs less than " << m_noisyModuleDiff*100 << "%";
   osNoisyStripDiff  << "Increase of #(noisy strips) from average of recent runs less than "                      << m_noisyStripDiff.value();

   ostringstream osFlagReason;
   if ( !isNoisyMinStat    ) osFlagReason << "FAILED in " << osNoisyMinStat.str()    << "; ";
   if ( !isNoisyModuleList ) osFlagReason << "FAILED in " << osNoisyModuleList.str() << "; ";
   if ( !isNoisyModuleDiff ) osFlagReason << "FAILED in " << osNoisyModuleDiff.str() << "; ";
   if ( !isNoisyStripDiff  ) osFlagReason << "FAILED in " << osNoisyStripDiff.str();

   string strFlagEnable = m_noisyUploadTest ? "ENABLED"   : "DISABLED";
   string strRunsInCool = isRunsInCool      ? "AVAILABLE" : "UNAVAILABLE";

   ostringstream osCheckList;
   osCheckList << osNoisyMinStat.str()    << "; "
               << osNoisyModuleList.str() << "; "
               << osNoisyModuleDiff.str() << "; "
               << osNoisyStripDiff.str();

   //--- Write out the contents to XML file
   outFile << xmlHeader                                       << linefeed
           << associateStylesheet("BadStrips.xsl")                          << linefeed
           << "<run>"                                                                                  << linefeed
           << "  <value name=\"RunNumber\">"        << m_runNumber.value()               << "</value>" << linefeed
           << "  <value name=\"StartTime\">"        << m_utcBegin                        << "</value>" << linefeed
           << "  <value name=\"EndTime\">"          << m_utcEnd                          << "</value>" << linefeed
           << "  <value name=\"Duration\">"         << m_calibEvtInfoTool->duration() << "</value>" << linefeed
           << "  <value name=\"LB\">"               << m_numOfLBsProcessed               << "</value>" << linefeed
           << "  <value name=\"Events\">"           << m_numberOfEvents            << "</value>" << linefeed
           << "  <value name=\"Modules\">"          << numModulesAll                     << "</value>" << linefeed
           << "  <value name=\"Links\">"            << numLinksAll                       << "</value>" << linefeed
           << "  <value name=\"Chips\">"            << numChipsAll                       << "</value>" << linefeed
           << "  <value name=\"StripsOfflineAll\">" << numStripsAll                      << "</value>" << linefeed
           << "  <value name=\"StripsOfflineNew\">" << numStripsNew                      << "</value>" << linefeed
           << "  <value name=\"ModulesRef\">"       << numModulesRef                     << "</value>" << linefeed
           << "  <value name=\"StripsOfflineRef\">" << numStripsRef                      << "</value>" << linefeed
           << "  <value name=\"ModulesDiff\">"      << numModulesDiff                << "</value>" << linefeed
           << "  <value name=\"Flag\">"             << strUploadFlag                     << "</value>" << linefeed
           << "  <value name=\"FlagReason\">"       << osFlagReason.str()                << "</value>" << linefeed
           << "  <value name=\"FlagEnable\">"       << strFlagEnable                     << "</value>" << linefeed
           << "  <value name=\"ReadCool\">"         << strRunsInCool                     << "</value>" << linefeed
           << "  <value name=\"CheckList\">"        << osCheckList.str()                 << "</value>" << linefeed
           << "  <chips>"                                                                              << linefeed
           <<      osChipList.str()
           << "  </chips>"                                                                             << linefeed
           << "  <modules>"                                                                            << linefeed
           <<  osModuleList.str()
           << "  </modules>"                                                                           << linefeed
           << "</run>"                                                                                 << endl;

   return StatusCode::SUCCESS;
}

std::set<int>
SCTCalib::getNoisyChips( const std::set<Identifier>& stripIdList ) const {
   std::set<int> chipIdList;
   chipIdList.clear();

   // Get SCT_DetectorElementCollection
   SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
   const InDetDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
   if (elements==nullptr) {
      ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
      return chipIdList;
   }

   //--- Minimum number of noisy strips for a noisy chip
   unsigned int noisyChipThr = m_noisyChipFraction*n_stripPerChip;
   if ( stripIdList.size() > noisyChipThr ) {
      unsigned int numStripsPerChip[ n_chipPerModule ] = { 0 };
      //--- Loop over stripIdList
      std::set<Identifier>::const_iterator stripItr  = stripIdList.begin();
      std::set<Identifier>::const_iterator stripItrE = stripIdList.end();
      for ( ; stripItr != stripItrE; ++stripItr ) {
         Identifier stripId = *stripItr;
         int stripOffline = m_pSCTHelper->strip( stripId );
         //--- Chip number : taken from SCT_ConfigurationConditionsSvc::getChip
         IdentifierHash waferHash = m_pSCTHelper->wafer_hash( m_pSCTHelper->wafer_id( stripId ) );
         const InDetDD::SiDetectorElement* pElement = elements->getDetectorElement( waferHash );
         if ( !pElement ) {
            ATH_MSG_FATAL("Element pointer is NULL");
            continue;
         }
         int stripOnline = ( pElement->swapPhiReadoutDirection() ) ? lastStrip - stripOffline : stripOffline;
         int chipId = m_pSCTHelper->side( stripId ) == 0 ? stripOnline/n_stripPerChip : stripOnline/n_stripPerChip + n_chipPerSide;
         //--- Count number of noisy strips per chips
         ++numStripsPerChip[ chipId ];
      }

      //--- Insert noisy chips
      for ( int iChip = 0; iChip != n_chipPerModule; ++iChip ) {
         if ( numStripsPerChip[iChip] > noisyChipThr ) chipIdList.insert( iChip );
      }
   }
   return chipIdList;
}

std::pair< string, float >
SCTCalib::getNoisyLB( const Identifier& moduleId, int& chipId ) const {
   std::string defectLB(""); //return value if invalid
   float defectLBFrac(0.0); //return value if invalid
   float noisyStripThr = m_noisyStripThrDef?(m_noisyStripThrOffline):(m_noisyStripThrOnline);

   //--- Identifier
   Identifier waferId = m_pSCTHelper->wafer_id( m_pSCTHelper->barrel_ec( moduleId ),
                        m_pSCTHelper->layer_disk( moduleId ),
                        m_pSCTHelper->phi_module( moduleId ),
                        m_pSCTHelper->eta_module( moduleId ),
                        chipId < n_chipPerSide ? 0 : 1 );
   IdentifierHash waferHash = m_pSCTHelper->wafer_hash( waferId );
   //--- Histogram for this chip
   int chipPositionInSide = m_pSCTHelper->side( waferId ) == 0 ? chipId : chipId - n_chipPerSide;
   int histIndex = (waferHash.value())*n_chipPerSide + chipPositionInSide;

   //--- Find LBs where this chip was noisy
   double chipOccupancyThr = noisyStripThr*n_stripPerChip*m_noisyChipFraction;
   std::set<int> LBList;
   LBList.clear();
   if (!m_calibLbTool) {
      ATH_MSG_ERROR("NULL pointer m_calibLbTool line "<<__LINE__);
      return std::make_pair( defectLB, defectLBFrac );
   }

   for ( int iLB = 0; iLB != m_LBRange; ++iLB ) {
      double numEventsInLB = m_calibLbTool->getNumberOfEventsInBin( iLB + 1 );
      if ( numEventsInLB == 0 ) continue;
      double chipOccupancy = m_calibLbTool->getBinForHistogramIndex( iLB + 1, histIndex )/numEventsInLB;
      if ( chipOccupancy > chipOccupancyThr ) LBList.insert( iLB );
   }
   //--- Transform LBList to string and calculate a fraction of noisy LBs
   if ( LBList.size() != 0 ) {
      defectLB = getLBList( LBList );
      defectLBFrac = static_cast<float>(LBList.size()) / m_numOfLBsProcessed;
   }

   return std::make_pair( defectLB, defectLBFrac );
}

std::string SCTCalib::getLBList( const std::set<int>& LBList ) const {
   std::string strList;
   strList.erase();
   if ( !LBList.empty() ) {
      int firstLB = -1;
      int LBSize  = -1;

      std::set<int>::const_iterator LBItrFirst = LBList.begin();
      std::set<int>::const_iterator LBItrLast  = --LBList.end();

      std::set<int>::const_iterator LBItr  = LBList.begin();
      std::set<int>::const_iterator LBItrE = LBList.end();
      for ( ; LBItr != LBItrE; ++LBItr ) {
         int iLB = *LBItr;
         if ( LBItr == LBItrFirst ) {
            firstLB = iLB;
            LBSize  = 1;
         } else {
            if ( iLB == firstLB + LBSize ) {
               ++LBSize;
            } else {
               int LBBegin = firstLB;
               int LBEnd   = firstLB + LBSize -1;
               strList = m_pCalibWriteTool->addDefect( strList, LBBegin, LBEnd );
               firstLB = iLB;
               LBSize  = 1;
            }
         }
         if ( LBItr == LBItrLast ) {
            int LBBegin = firstLB;
            int LBEnd   = iLB;
            strList = m_pCalibWriteTool->addDefect( strList, LBBegin, LBEnd );
         }
      }
   }
   return strList;
}
