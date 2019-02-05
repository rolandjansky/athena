/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ManagedMonitorToolBase_CXX
#define ManagedMonitorToolBase_CXX

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include <cctype>
#include <ctime>
#include <sstream>

#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TROOT.h"
#include "TFile.h"
#include "LWHists/LWHist.h"
#include "LWHists/LWHistControls.h"
#include "LWHistAthMonWrapper.h"
#include "AthMonBench.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "TrigDecisionInterface/ITrigDecisionTool.h"
#include "AthenaMonitoring/IDQFilterTool.h"
#include "GaudiKernel/ITHistSvc.h"

//____________________________________________________________________
class ManagedMonitorToolBase::Imp {
public:
  static ISvcLocator*    s_svcLocator;
  Imp(ManagedMonitorToolBase*tc) : m_theclass(tc),
				   m_warnAboutMissingInitialize(true),
				   m_doResourceMon(false),
				   m_benchNFillAfterbook(0) {}
  ~Imp() {}

  //Data members:
  ManagedMonitorToolBase * m_theclass;
  //To warn against clients that reimplemented initialize without calling ManagedMonitorToolBase::initialze():
  bool m_warnAboutMissingInitialize;

  //Internal methods and data members for detailed LWHists-aware CPU/mem monitoring:
  bool m_doResourceMon;
  AthMonBench m_bench_tmp;
  AthMonBench m_bench_book;
  AthMonBench m_bench_fillfirst;
  AthMonBench m_bench_fillall;
  AthMonBench m_bench_filllast;
  AthMonBench m_bench_proc;
  AthMonBench m_bench_total;
  unsigned m_benchNFillAfterbook;
  void benchPreBookHistograms();
  void benchPostBookHistograms();
  void benchPreFillHistograms();
  void benchPostFillHistograms();
  void benchPreProcHistograms();
  void benchPostProcHistograms();
  void benchFinalReport();
  void benchReset();
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////     Internal methods for detailed CPU/mem monitoring     ///////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//____________________________________________________________________
void ManagedMonitorToolBase::Imp::benchPreBookHistograms()
{
  if (!m_doResourceMon)
    return;
  benchReset();
  m_bench_book.startMeasurement();
}

//____________________________________________________________________
void ManagedMonitorToolBase::Imp::benchPostBookHistograms()
{
  if (!m_doResourceMon)
    return;
  m_bench_book.finishMeasurement();
  m_bench_total += m_bench_book;
  m_benchNFillAfterbook = 0;
}

//____________________________________________________________________
void ManagedMonitorToolBase::Imp::benchPreFillHistograms()
{
  if (!m_doResourceMon)
    return;
  m_bench_tmp.reset();
  m_bench_tmp.startMeasurement();
}

//____________________________________________________________________
void ManagedMonitorToolBase::Imp::benchPostFillHistograms()
{
  if (!m_doResourceMon)
    return;
  m_bench_tmp.finishMeasurement();
  m_bench_total += m_bench_tmp;
  ++m_benchNFillAfterbook;
  if (m_benchNFillAfterbook==1) {
    m_bench_fillfirst = m_bench_tmp;
  } else {
    m_bench_fillall += m_bench_tmp;
    m_bench_filllast = m_bench_tmp;
  }
}

//____________________________________________________________________
void ManagedMonitorToolBase::Imp::benchPreProcHistograms()
{
  if (!m_doResourceMon)
    return;
  m_bench_proc.startMeasurement();
}

//____________________________________________________________________
void ManagedMonitorToolBase::Imp::benchPostProcHistograms()
{ 
  if (!m_doResourceMon)
    return;
  m_bench_proc.finishMeasurement();
  m_bench_total += m_bench_proc;
  m_bench_fillall -= m_bench_filllast;
  m_benchNFillAfterbook = 0;
  benchFinalReport();
  benchReset();
}

//____________________________________________________________________
void ManagedMonitorToolBase::Imp::benchReset()
{
  if (!m_doResourceMon)
    return;
  m_benchNFillAfterbook = 0;
  m_bench_tmp.reset();
  m_bench_book.reset();
  m_bench_fillfirst.reset();
  m_bench_fillall.reset();
  m_bench_filllast.reset();
  m_bench_proc.reset();
  m_bench_total.reset();

}

//____________________________________________________________________
void ManagedMonitorToolBase::Imp::benchFinalReport()
{
  if (!m_doResourceMon)
    return;
  m_theclass->msg(AthMonBench::s_resourceMonThreshold) << "ResourceSummary --> Result of dedicated mon-tool resource monitoring:"<<endmsg;
  m_theclass->msg(AthMonBench::s_resourceMonThreshold) << "ResourceSummary --> Book        : "<<m_bench_book << endmsg;
  m_theclass->msg(AthMonBench::s_resourceMonThreshold) << "ResourceSummary --> First Fill  : "<<m_bench_fillfirst << endmsg;
  m_theclass->msg(AthMonBench::s_resourceMonThreshold) << "ResourceSummary --> Last Fill   : "<<m_bench_filllast << endmsg;
  m_theclass->msg(AthMonBench::s_resourceMonThreshold) << "ResourceSummary --> Other Fills : "<<m_bench_fillall << endmsg;
  m_theclass->msg(AthMonBench::s_resourceMonThreshold) << "ResourceSummary --> Proc        : "<<m_bench_proc << endmsg;
  m_bench_total.setUnitCount();//to avoid dividing by number of measurements
  m_theclass->msg(AthMonBench::s_resourceMonThreshold) << "ResourceSummary --> Grand Total : "<<m_bench_total << endmsg;
  //NB: Needs total also!
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

ISvcLocator* ManagedMonitorToolBase::Imp::s_svcLocator(0);

namespace {

   std::string strToLower( const std::string& str );
   //std::string strToUpper( const std::string& str );

} // unnamed namespace

static unsigned s_mmtb_mongroup_ncopies=0;
unsigned ManagedMonitorToolBase::MonGroup::ncopies() { return s_mmtb_mongroup_ncopies; }
void ManagedMonitorToolBase::MonGroup::badusage() { ++s_mmtb_mongroup_ncopies; }

// *********************************************************************
// Public Methods
// *********************************************************************
StatusCode
ManagedMonitorToolBase::MonGroup::
regHist( TH1* h )
{
   if( m_tool != 0 ) {
      std::string hName( h->GetName() );
      HistMap_t::value_type valToInsert( hName, h );
      m_map.insert( valToInsert );
      return m_tool->regHist( h, *this );
   }

   return StatusCode::FAILURE;
}

StatusCode
ManagedMonitorToolBase::MonGroup::
getHist( TH1*& h, const std::string& hName )
{
   if( m_tool != 0 ) {
      return m_tool->getHist( h, hName, *this );
   }

   return StatusCode::FAILURE;
}



StatusCode 
ManagedMonitorToolBase::MonGroup::
regHist( LWHist* h )
{
   if( m_tool != 0 ) {
      std::string hName( h->GetName() );
      HistMapLW_t::value_type valToInsert( hName, h );//Fixme: Just keep a list of the hists
      m_mapLW.insert( valToInsert );
      return m_tool->regHist( h, *this );
   }

   return StatusCode::FAILURE;
}



StatusCode ManagedMonitorToolBase::MonGroup::getHist( LWHist*& h, const std::string& hName )
{
   if( m_tool )
     return m_tool->getHist( h, hName, *this );

   return StatusCode::FAILURE;
}


StatusCode
ManagedMonitorToolBase::MonGroup::
getHist( TH2*& h, const std::string& hName )
{
   if( m_tool != 0 ) {
      return m_tool->getHist( h, hName, *this );
   }

   return StatusCode::FAILURE;
}


StatusCode
ManagedMonitorToolBase::MonGroup::
regGraph( TGraph* g )
{
   if( m_tool != 0 ) {
      return m_tool->regGraph( g, *this );
   }

   return StatusCode::FAILURE;
}


StatusCode
ManagedMonitorToolBase::MonGroup::
regTree( TTree* t )
{
   if( m_tool != 0 ) {
      return m_tool->regTree( t, *this );
   }

   return StatusCode::FAILURE;
}


StatusCode
ManagedMonitorToolBase::MonGroup::
writeAndDelete( TH1* h )
{
   if( m_tool != 0 ) {
      std::string hName( h->GetName() );
      m_map.erase( hName );
      return m_tool->writeAndDelete( h, *this );
   }

   return StatusCode::FAILURE;
}  


StatusCode
ManagedMonitorToolBase::MonGroup::
deregHist( TH1* h )
{
   if( m_tool != 0 ) {
      std::string hName( h->GetName() );
      m_map.erase( hName );
      return m_tool->deregHist( h );
   }

   return StatusCode::FAILURE;
}


StatusCode ManagedMonitorToolBase::MonGroup::deregHist( LWHist* h )
{
   if( m_tool != 0 ) {
      std::string hName( h->GetName() );
      m_mapLW.erase( hName );
      return m_tool->deregHist( h );
   }

   return StatusCode::FAILURE;
}


StatusCode
ManagedMonitorToolBase::MonGroup::
deregGraph( TGraph* g )
{
   if( m_tool != 0 ) {
      return m_tool->deregGraph( g );
   }

   return StatusCode::FAILURE;
}


StatusCode
ManagedMonitorToolBase::MonGroup::
deregObject( const std::string& objName )
{
   if( m_tool != 0 ) {
      return m_tool->deregObject( objName, *this );
   }

   return StatusCode::FAILURE;
}


StatusCode
ManagedMonitorToolBase::MonGroup::
deregAll()
{
   if( m_tool != 0 ) {
      bool isSuccess(true);
      StatusCode sc;
      sc.setChecked();

      typedef HistMap_t::const_iterator MapIter_t;
      MapIter_t mapEnd = m_map.end();
      for( MapIter_t i = m_map.begin(); i != mapEnd; ++i ) {
         sc = m_tool->deregHist( i->second );
         if( !sc.isSuccess() )
            isSuccess = false;
      }

      typedef HistMapLW_t::const_iterator MapIterLW_t;
      MapIterLW_t mapEndLW = m_mapLW.end();
      for( MapIterLW_t i = m_mapLW.begin(); i != mapEndLW; ++i ) {
         sc = m_tool->deregHist( i->second );
         if( !sc.isSuccess() )
            isSuccess = false;
      }

      m_map.clear();
      m_mapLW.clear();

      if( isSuccess )
         return StatusCode::SUCCESS;
   }

   return StatusCode::FAILURE;
}


ManagedMonitorToolBase::
ManagedMonitorToolBase( const std::string & type, const std::string & name,
      const IInterface* parent )
   : AthAlgTool( type, name, parent )
   , m_manager(0)
   , m_managerNameProp("AthenaMonManager")
   , m_fileKey("")
   , m_dataTypeStr("userDefined")
   , m_environmentStr("noOutput")
   , m_detailLevel(1)
   , m_dataType(AthenaMonManager::userDefined)
   , m_environment(AthenaMonManager::user)
   , m_streamNameFcn(0)
   , m_THistSvc("THistSvc",name)
   , m_trigDecTool("")
   , m_trigTranslator("")
   , m_DQFilterTools(this)
   , m_procNEventsProp(0)
   , m_path("")
   , m_preScaleProp(0)
   , m_triggerChainProp("")
   , m_triggerGroupProp("")
   , m_useTrigger(false)
   , m_lastLumiBlock(0)
   , m_lastRun(0)
   , m_lastLowStatInterval(-1)
   , m_lastMedStatInterval(-1)
   , m_lastHigStatInterval(-1)
   , m_nEvents(1)
   , m_nEventsIgnoreTrigger(1)
   , m_nLumiBlocks(1)
   , m_haveClearedLastEventBlock(true)
   , m_lumiTool("LuminosityTool")
   , m_liveTool("TrigLivefractionTool")
   , m_hasRetrievedLumiTool(false)
   , m_bookHistogramsInitial(false)
   , m_useLumi(false)
   , m_defaultLBDuration(60.)
    //, m_cycleNum(0)
   , m_d(new Imp(this))
{
//   ManagedMonitorToolBase_resetHistStatistics(this);
   declareInterface<IMonitorToolBase>(this);
   declareProperty( "ProcessNEvents", m_procNEventsProp );
   declareProperty( "histoPathBase", m_path );
   declareProperty( "PreScale", m_preScaleProp );
   declareProperty( "TriggerChain", m_triggerChainProp );
   declareProperty( "TriggerGroup", m_triggerGroupProp );

   // If a class would like to associate itself to a manager but cannot actually
   // be referenced by it, the manager name can be set explicitly.  Otherwise,
   // it is set automatically.
   declareProperty( "ManagerName", m_managerNameProp );

   // The TrigDecisionTool, clients normally should not have to set this
   declareProperty( "TrigDecisionTool", m_trigDecTool );
   
   // The TriggerTranslator
   declareProperty( "TriggerTranslatorTool", m_trigTranslator );

   // The filter tools, to be specified in jobOptions
   declareProperty( "FilterTools", m_DQFilterTools );

   // Enable luminosity tool?
   declareProperty( "EnableLumi", m_useLumi );

   // How detailed should the monitoring be?
   declareProperty( "DetailLevel", m_detailLevel );

   // If we don't know how long an LB is, here's the default
   declareProperty( "DefaultLBDuration", m_defaultLBDuration );

   // Properties that are overridden in initialize()---settings via jobOptions are ignored!
   declareProperty( "FileKey", m_fileKey );
   declareProperty( "DataType", m_dataTypeStr );
   declareProperty( "Environment", m_environmentStr );

   if( Imp::s_svcLocator==0 )
      Imp::s_svcLocator = svcLoc();

   for (auto interval: { eventsBlock, lumiBlock, lowStat, run, all }) {
     m_templateHistograms[interval] = std::vector< MgmtParams<TH1> >();
     m_templateGraphs[interval] = std::vector< MgmtParams<TGraph> >();
     m_templateTrees[interval] = std::vector< MgmtParams<TTree> >();
     m_templateLWHistograms[interval] = std::vector< MgmtParams<LWHist> >();
     m_supportedIntervalsForRebooking.insert(interval);
   }

   newLowStatInterval = false;
   newMedStatInterval = false;
   newHigStatInterval = false;
   newLowStat = false;
   newLumiBlock = false;
   newRun = false;
   newEventsBlock = false;
   endOfEventsBlock = false;
   endOfLowStat = false;
   endOfLumiBlock = false;
   endOfRun = false;

   m_newLowStatInterval = false;
   m_newMedStatInterval = false;
   m_newHigStatInterval = false;
   m_newLowStat = false;
   m_newLumiBlock = false;
   m_newRun = false;
   m_newEventsBlock = false;
   m_endOfEventsBlock = false;
   m_endOfLowStat = false;
   m_endOfLumiBlock = false;
   m_endOfRun = false;
}


ManagedMonitorToolBase::
~ManagedMonitorToolBase()
{
//   ManagedMonitorToolBase_printHistStatistics(this);
   delete m_streamNameFcn;

   std::map<std::string,OutputMetadata*>::iterator mdend = m_metadataMap.end();
   for( std::map<std::string,OutputMetadata*>::iterator i = m_metadataMap.begin();
         i != mdend; ++i ) {
      delete (i->second);
   }
   delete m_d;
}


ManagedMonitorToolBase::StreamNameFcn*
ManagedMonitorToolBase::
streamNameFunction()
{
   if( m_streamNameFcn == 0 ) {
      msg(MSG::ERROR) << "!! streamNameFunction() has not been initialized !!" << endmsg;
      msg(MSG::ERROR) << "  --> neither ManagedMonitorToolBase::initialize() nor" << endmsg;
      msg(MSG::ERROR) << "  --> ManagedMonitorToolBase::setMonManager() has been called." << endmsg;
      msg(MSG::ERROR) << "  --> Correct configuration cannot be guaranteed from this point." << endmsg;
      m_streamNameFcn = getNewStreamNameFcn();
   }
   return m_streamNameFcn;
}


/*
std::string
ManagedMonitorToolBase::
levelOfDetailEnumToString( LevelOfDetail_t level, bool useOnlineConventions )
{
   std::string str("summary");

   switch( level ) {
      case transient:
         str = "transient";
         break;
      case debug:
         str = "debug";
         break;
      case expert:
         str = "expert";
         break;
      case shift:
         str = "shift";
         break;
      case summary:
      default:
         if(useOnlineConventions)
            str = "runstat";
         else
            str = "summary";
   }

   if(useOnlineConventions)
      str = strToUpper( str );

   return str;
}
*/

/*
ManagedMonitorToolBase::LevelOfDetail_t
ManagedMonitorToolBase::
levelOfDetailStringToEnum( const std::string& str )
{
   std::string lcstr( strToLower(str) );

   if( lcstr == "summary" || lcstr == "runstat" )
      return summary;
   else if( lcstr == "shift" )
      return shift;
   else if( lcstr == "expert" )
      return expert;
   else if( lcstr == "debug" )
      return debug;
   else if( lcstr == "transient" )
      return transient;

   // This is a static method (shared by all instances), so no "this" pointer
   // is available (needed for msg). Have to use message service directly.
   if( Imp::s_svcLocator!=0 ) {
      IMessageSvc* ms(0);
      StatusCode sc = Imp::s_svcLocator->service( "MessageSvc", ms, true );
      if( sc.isSuccess() ) {
         MsgStream log( ms, "ManagedMonitorToolBase::levelOfDetailStringToEnum()" );
         log << MSG::WARNING << "Unknown ManagedMonitorToolBase::LevelOfDetail_t \""
            << str << "\", returning \"transient\"" << endmsg;
      }
   }

   return transient;
}
*/


std::string
ManagedMonitorToolBase::
intervalEnumToString( Interval_t interval )
{
   std::string str("file");

   switch( interval ) {
      case all:
         str = "all";
         break;
      case fill:
         str = "fill";
         break;
      case run:
         str = "run";
         break;
      case lowStat:
         str = "lowStat";
         break;
      case medStat:
         str = "medStat";
         break;
      case higStat:
         str = "higStat";
         break;
      case lumiBlock:
         str = "lumiBlock";
         break;
      case eventsBlock:
         str = "eventsBlock";
         break;
      case file:
 	 str = "file";
	 break;
      default:
         str = "unknown";
   }

   return str;
}


ManagedMonitorToolBase::Interval_t
ManagedMonitorToolBase::
intervalStringToEnum( const std::string& str )
{
   std::string lcstr( strToLower(str) );

   if( lcstr == "all" )
      return all;
   else if( lcstr == "fill" )
      return fill;
   else if( lcstr == "run" )
      return run;
   else if( lcstr == "lowStat" )
      return lowStat;
   else if( lcstr == "medStat" )
      return medStat;
   else if( lcstr == "higStat" )
      return higStat;
   else if( lcstr == "lumiBlock" )
      return lumiBlock;
   else if( lcstr == "eventsBlock" )
      return eventsBlock;
   else if( lcstr == "file" )
      return file;

   if( Imp::s_svcLocator!=0 ) {
      IMessageSvc* ms(0);
      StatusCode sc = Imp::s_svcLocator->service( "MessageSvc", ms, true );
      if( sc.isSuccess() ) {
         MsgStream log( ms, "ManagedMonitorToolBase::intervalStringToEnum()" );
         log << MSG::WARNING << "Unknown ManagedMonitorToolBase::Interval_t \""
            << str << "\", returning \"file\"" << endmsg;
      }
   }

   return file;
}


StatusCode
ManagedMonitorToolBase::
initialize()
{
   m_d->m_warnAboutMissingInitialize = false;

   StatusCode sc = AthAlgTool::initialize();
   if( !sc.isSuccess() )
      return StatusCode::FAILURE;

   ATH_MSG_DEBUG("ManagedMonitorToolBase::initialize():");
   
   m_d->m_doResourceMon = msgLvl(AthMonBench::s_resourceMonThreshold);

   sc = m_THistSvc.retrieve();
   if( !sc.isSuccess() ) {
      msg(MSG::ERROR) << "!! Unable to locate the THistSvc service !!" << endmsg;
      return sc;
   }
   ATH_MSG_DEBUG("  --> Found service \"THistSvc\"");

   if( !m_trigDecTool.empty() ) {
      sc = m_trigDecTool.retrieve();
      if( !sc.isSuccess() ) {
         msg(MSG::ERROR) << "!! Unable to retrieve the TrigDecisionTool !!" << endmsg;
         return sc;
      }
      ATH_MSG_DEBUG("  --> Found AlgTool \"TrigDecisionTool\"");

      if( !m_trigTranslator.empty() ) {
	sc = m_trigTranslator.retrieve();
	if ( !sc.isSuccess() ) {
	  ATH_MSG_ERROR(" Unable to retrieve the TrigTranslatorTool!" << endmsg);
	  return sc;
	}
      }

      if(m_triggerChainProp!="") {
	ATH_MSG_DEBUG("  --> Found nonempty trigger chain list");
         sc=parseList(m_triggerChainProp, m_vTrigChainNames);
         if(!sc.isSuccess()) {
            msg(MSG::WARNING) << "Error parsing the trigger chain list, using empty list" << endmsg;
            m_vTrigChainNames.clear();
         }
	 if (!m_trigTranslator.empty()) {
	   updateTriggersForGroups(m_vTrigChainNames);
	 }
      }
      else {
	ATH_MSG_DEBUG("  --> trigger chain list empty");
      }
      if(m_triggerGroupProp!="") {
	ATH_MSG_DEBUG("  --> Found nonempty trigger group list");
         sc=parseList(m_triggerGroupProp, m_vTrigGroupNames);
         if(!sc.isSuccess()) {
            msg(MSG::WARNING) << "Error parsing the trigger group names list, using empty list" << endmsg;
            m_vTrigGroupNames.clear();
         }
	 if (!m_trigTranslator.empty()) {
	   updateTriggersForGroups(m_vTrigGroupNames);
	 }
      }
      else {
	ATH_MSG_DEBUG("  --> trigger group list empty");
      }
   }
   else {
     ATH_MSG_DEBUG("  --> no trigger decision tool specified");
   }

   sc = m_DQFilterTools.retrieve();
   if( !sc.isSuccess() ) {
     ATH_MSG_ERROR("!! Unable to retrieve the Filter Tools !!");
     return sc;
   }

   ServiceHandle<IJobOptionsSvc> joSvc( "JobOptionsSvc", name() );
   sc = joSvc.retrieve();
   if( !sc.isSuccess() ) {
      msg(MSG::ERROR) << "!! Unable to locate the JobOptionsSvc service !!" << endmsg;
      return StatusCode::FAILURE;
   }
   ATH_MSG_DEBUG("  --> Found service \"JobOptionsSvc\"");

   std::string client( m_managerNameProp + "Properties" );
   ATH_MSG_DEBUG("  --> Asking for properties " << client);
   sc = joSvc->setMyProperties( client, this );
   if( !sc.isSuccess() ) {
      msg(MSG::ERROR) << "!! Unable to set properties in JobOptionsSvc !!" << endmsg;
   }
   ATH_MSG_DEBUG("  --> Set Properties in \"JobOptionsSvc\"");

   ATH_MSG_DEBUG("      * Properties set from " << client << " to the values:\n"
		 << "        FileKey: " << m_fileKey << "\n"
		 << "        DataType: " << m_dataTypeStr << "\n"
		 << "        Environment: " << m_environmentStr);

   m_dataType = AthenaMonManager::dataTypeStringToEnum( m_dataTypeStr );
   m_environment = AthenaMonManager::envStringToEnum( m_environmentStr );

   if (m_useLumi) {
     if (m_dataType == AthenaMonManager::monteCarlo) {
       ATH_MSG_WARNING("Lumi use in monitoring enabled but tool configured for MC; disabling lumi tool use");
     } else {
       // Get the luminosity tool
       //CHECK(m_lumiTool.retrieve());
       StatusCode sc_lumiTool = m_lumiTool.retrieve();
       
       // Get the livefration tool
       //CHECK(m_liveTool.retrieve());
       StatusCode sc_liveTool = m_liveTool.retrieve();

       // Set m_hasRetrievedLumiTool to true when both tools are retrieved successfully
       if ( sc_lumiTool.isSuccess() && sc_liveTool.isSuccess() )
           m_hasRetrievedLumiTool = true;
     }
   } else {
       ATH_MSG_DEBUG("!! Luminosity tool is disabled !!");
   }


   delete m_streamNameFcn;
   m_streamNameFcn = getNewStreamNameFcn();

   ATH_MSG_DEBUG("  --> Exiting successfully");

   return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolBase::
bookHists()
{
   // The Run/LumiBlock numbers are not set when beginRun() is called.  Therefore,
   // book histograms on the first call to fillHists(), which is called from execute().
   return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolBase::
fillHists()
{

   if (m_d->m_warnAboutMissingInitialize) {
       m_d->m_warnAboutMissingInitialize = false;
       msg(MSG::WARNING) << "ManagedMonitorToolBase::initialize() never called from reimplementation!" << endmsg;
   }


   bool isNewEventsBlock = ( m_procNEventsProp > 0  && ((m_nEvents % m_procNEventsProp) == 1) && m_haveClearedLastEventBlock );
   if (isNewEventsBlock) m_haveClearedLastEventBlock = false;

   m_newLowStat = false;
   m_newLumiBlock = false;
   m_newRun = false;
   newLowStat = false;
   newLumiBlock = false;
   newRun = false;

   m_newLowStatInterval = false;
   m_newMedStatInterval = false;
   m_newHigStatInterval = false;
   newLowStatInterval = false;
   newMedStatInterval = false;
   newHigStatInterval = false;
   
   m_useTrigger = ( (m_triggerChainProp != "" || m_triggerGroupProp != "")  && (!m_trigDecTool.empty()) );

   if( m_manager != 0 ) {
     m_newLumiBlock = ( (m_lastLumiBlock != m_manager->lumiBlockNumber()) || m_manager->forkedProcess());
      m_newRun = ( m_lastRun != m_manager->runNumber() );
      newLumiBlock = m_newLumiBlock;
      newRun = m_newRun;

      if(m_newRun) {
         m_newLumiBlock = true;
         newLumiBlock = m_newLumiBlock;
         isNewEventsBlock = true;
      }

      m_newEventsBlock = isNewEventsBlock;
      newEventsBlock = m_newEventsBlock;

      if( m_newLumiBlock ) {
         // check if a new LB interval has started
         // lowest lumiBlockNumber() is 1
         // m_lastLowStatInterval is -1 initially
         int currentLB = m_manager->lumiBlockNumber();
         int LBsLowStat = m_manager->getLBsLowStat();
         int LBsMedStat = m_manager->getLBsMedStat();
         int LBsHigStat = m_manager->getLBsHigStat();

         if( LBsLowStat*LBsMedStat*LBsHigStat == 0) {
            msg(MSG::WARNING) << "zero LBs requested for interval" << endmsg;
         }
         else {
            if( ((currentLB-1)/LBsLowStat) != m_lastLowStatInterval ) m_newLowStatInterval = true;
            if( ((currentLB-1)/LBsMedStat) != m_lastMedStatInterval ) m_newMedStatInterval = true;
            if( ((currentLB-1)/LBsHigStat) != m_lastHigStatInterval ) m_newHigStatInterval = true;
            newLowStatInterval = m_newLowStatInterval;
            newMedStatInterval = m_newHigStatInterval;
            newHigStatInterval = m_newHigStatInterval;
         }
      }

      // Allow inheriting classes the option of using the lastLumiBloc/lastRun values
      // before updating them
   }


   StatusCode sc0( StatusCode::SUCCESS );
   sc0.setChecked();
   StatusCode sc1( StatusCode::SUCCESS );
   sc1.setChecked();
   StatusCode sc2( StatusCode::SUCCESS );
   sc2.setChecked();
   StatusCode sc3( StatusCode::SUCCESS );
   sc3.setChecked();

   // Set end of LowStat, LumiBlock and Run variables
   // These are needed to be used in procHistograms().
   m_endOfEventsBlock = m_newEventsBlock;
   m_endOfLowStat = m_newLowStatInterval;
   m_endOfLumiBlock = m_newLumiBlock;
   m_endOfRun = m_newRun;
   endOfEventsBlock = m_newEventsBlock;
   endOfLowStat = m_newLowStatInterval;
   endOfLumiBlock = m_newLumiBlock;
   endOfRun = m_newRun;

   // just duplicates m_newLowStatInterval
   m_newLowStat = m_newLowStatInterval; 
   newLowStat = m_newLowStatInterval; 

   if( m_newEventsBlock || m_newLumiBlock || m_newRun ) {
     ATH_MSG_DEBUG("Interval transition processing");
      // Process histograms from the previous lumiBlock/run
      if( m_nEvents != 1 ) {
	     m_d->benchPreProcHistograms();
         sc0 = procHistograms();
	     m_d->benchPostProcHistograms();
         sc0.setChecked();
      }
      // Re-book new histograms
      m_d->benchPreBookHistograms();

      if (!m_bookHistogramsInitial) {
          sc1 = bookHistograms();
          m_bookHistogramsInitial = true;
      } else {
          std::vector<Interval_t> intervals_to_process;
          if (m_newEventsBlock) intervals_to_process.push_back(eventsBlock);
          if (m_newLumiBlock) intervals_to_process.push_back(lumiBlock);
          if (m_newLowStatInterval) intervals_to_process.push_back(lowStat);
          if (m_newRun) intervals_to_process.push_back(run);
          for (const auto interval: intervals_to_process) {
            sc1 = regManagedHistograms(m_templateHistograms[interval]);     
            sc1 = regManagedGraphs(m_templateGraphs[interval]);
            sc1 = regManagedTrees(m_templateTrees[interval]);
            sc1 = regManagedLWHistograms(m_templateLWHistograms[interval]);     
          }
      }
      for (const auto& interval: std::vector<Interval_t>{ eventsBlock, lumiBlock, lowStat, run }) {
	for (const auto& it: m_templateHistograms[interval]) {
	  //ATH_MSG_WARNING("Oi, considering " << it.m_templateHist->GetName() << it.m_group.histo_mgmt());
	  // is histogram too small in x axis for LB range?
	  if (it.m_group.histo_mgmt() == ATTRIB_X_VS_LB) {
	    //ATH_MSG_WARNING("We are rebinning for " << it.m_templateHist->GetName());
	    while ( it.m_templateHist->GetXaxis()->GetXmax() <= AthenaMonManager::lumiBlockNumber() ) {
	      it.m_templateHist->LabelsInflate("X");
	    }
	  }
	}
      }

      if (auto streamname = dynamic_cast<OfflineStream*>(streamNameFunction())) {
	streamname->updateRunLB();
      }
      
      sc3 = bookHistogramsRecurrent( );
      
      m_d->benchPostBookHistograms();
      sc1.setChecked();
      sc3.setChecked();
      

      if (m_manager->forkedProcess()) {
	ATH_MSG_INFO("Child process: Resetting all " << m_lwhists.size() <<  " LW Histograms");
	for (LWHist* h : m_lwhists) {
	  h->Reset();
	}
      }
   }//end if new RUN/LB/Block

   // check filters
   bool filterresult(true);
   if (! m_DQFilterTools.empty()) {
     ToolHandleArray<IDQFilterTool>::const_iterator ifilter(m_DQFilterTools.begin()), filterend(m_DQFilterTools.end());
     for (; filterresult && (ifilter != filterend);
   	  ++ifilter) {
       filterresult = (filterresult && (*ifilter)->accept());
     }
   }
   

   // ...and fill as normal
   if(filterresult &&
      (!m_useTrigger
       || (m_vTrigChainNames.size()>0 && trigChainsArePassed(m_vTrigChainNames))
       || (m_vTrigGroupNames.size()>0 && trigChainsArePassed(m_vTrigGroupNames))) ) {
     ATH_MSG_DEBUG("Passed trigger, presumably");
      m_d->benchPreFillHistograms();
      StatusCode sc3 = fillHistograms();
      m_haveClearedLastEventBlock = true;
      m_d->benchPostFillHistograms();
      sc3.setChecked();
      ++m_nEvents;
   } else { ATH_MSG_DEBUG("Failed trigger, presumably"); }

   ++m_nEventsIgnoreTrigger;
   if( m_newLumiBlock && (m_nEventsIgnoreTrigger != 1) ) {
      ++m_nLumiBlocks;
   }
   if( m_manager != 0 ) {
      m_lastRun = m_manager->runNumber();
      if( m_newLumiBlock ) {
         m_lastLumiBlock = m_manager->lumiBlockNumber();

         int LBsLowStat = m_manager->getLBsLowStat();
         int LBsMedStat = m_manager->getLBsMedStat();
         int LBsHigStat = m_manager->getLBsHigStat();
         if( LBsLowStat*LBsMedStat*LBsHigStat > 0) {
            if( m_newLowStatInterval ) m_lastLowStatInterval = (m_lastLumiBlock-1)/LBsLowStat;
            if( m_newMedStatInterval ) m_lastMedStatInterval = (m_lastLumiBlock-1)/LBsMedStat;
            if( m_newHigStatInterval ) m_lastHigStatInterval = (m_lastLumiBlock-1)/LBsHigStat;
         }
      }
   }

   return StatusCode::SUCCESS;
}

StatusCode
ManagedMonitorToolBase::
registerMetadata(const std::string& streamName, const std::string& hName, 
		 const MonGroup& group) {
  if( m_environment != AthenaMonManager::online ) {
    TTree* metadata(0);
    std::string mdStreamName( streamName );
    size_t found=mdStreamName.rfind("/");
    
    if ( found != std::string::npos )
      mdStreamName.replace( found, mdStreamName.length(), "/metadata" );
    
    MDMap_t::iterator i = m_metadataMap.find( mdStreamName );
    if( i == m_metadataMap.end() ) {
      metadata = new TTree( "metadata", "Monitoring Metadata" );
      if (! metadata) return StatusCode::FAILURE;
      StatusCode scmd = m_THistSvc->regTree( mdStreamName, metadata );
      if (scmd == StatusCode::FAILURE) return StatusCode::FAILURE;
      MDMap_t::value_type valToInsert( mdStreamName, new OutputMetadata(metadata) );
      i = m_metadataMap.insert( valToInsert ).first;
    }
    
    i->second->fill( hName,  group.interval(), group.chain(), group.merge() );
  }
  return StatusCode::SUCCESS;
}

StatusCode 
ManagedMonitorToolBase::
regManagedHistograms(std::vector< MgmtParams<TH1> >& templateHistograms)
{
      // The method registers histograms with the THistSvc and saves them to file.

      // The funky business with registering and deregistering the histogram is needed
      // to get the correct directory when saving histograms. THistSvc deals with ROOT 
      // to set up proper TDirectory, so we rely on it.
      // E.g. 
      //     m_THistSvc->regHist( streamName, h ): sets the correct TDirectory with streamName
      //     m_THistSvc->deReg( h ) - deregister from THistSvc otherwise THistSvc will try to save it
      //                          at the end of execution
      //  use passownership of the histogram and save it to file 
      //     m_manager->passOwnership( h, genericName );
      //     m_manager->writeAndDelete( genericName );
      bool allIsOk = true;  
   
      for( std::vector< MgmtParams<TH1> >::iterator it = templateHistograms.begin(); it != templateHistograms.end(); ++it ) {
          MonGroup& group = (*it).m_group;

          // Get a handle to the histogram
          TH1* theHist = (*it).m_templateHist;

          // Clone the histogram
          TH1* h = static_cast<TH1*>(theHist->Clone());
          theHist->Reset();

          // Get name
          std::string hName = h->GetName();

          // Get the streamName for the previous interval
          std::string streamName = streamNameFunction()->getStreamName( this, group, hName, true );

          // Register the histogram with the THistSvc
          StatusCode sc1 = m_THistSvc->deReg( theHist );
          if (sc1 == StatusCode::FAILURE) allIsOk = false;

          // Register clonned histogram under previous interval streamName
          StatusCode sc2 = m_THistSvc->regHist( streamName, h );
          if (sc2 == StatusCode::FAILURE) allIsOk = false;

          if( m_manager != 0 ) {
             std::string genericName = NoOutputStream().getStreamName( this, group, hName );
	         m_manager->passOwnership( h, genericName );
             m_manager->writeAndDelete( genericName );
          }
        
          // Get streamName for the current interval 
          streamName = streamNameFunction()->getStreamName( this, group, hName, false );
          // Register metadata information with the current interval streamname
	      StatusCode smd = registerMetadata(streamName, hName, group);
	      if (smd != StatusCode::SUCCESS) allIsOk = false;

          // Re-register the original histogram with the current interval streamName
          StatusCode sc3 = m_THistSvc->regHist( streamName, theHist );
          if (sc3 == StatusCode::FAILURE) allIsOk = false;

      }

      if (!allIsOk) return StatusCode::FAILURE;
      
      return StatusCode::SUCCESS;
}

StatusCode 
ManagedMonitorToolBase::
THistSvc_deReg_fixTGraph( TFile* file, TGraph* theGraph, std::string& directoryName)
{
    // THistSvc employs TDirectory Append method when registering TGraph.
    // When deReg is used to de-register TGraph object, THistSvc only removes the object
    // from its internal management but forgets to delete from TDirectory.
    // The current method fixes this problem by removing the TGraph object manually
    // after THistSvc->deReg(TGraph* obj) is called.

    // Saves and restores gFile and gDirectory
    GlobalDirectoryRestore restore;

    // This check is true when TGraph object is removed successfully
    bool graphRemoved = false;

    file->cd("/");
    TDirectory* dir = file->GetDirectory(directoryName.c_str());
    if (dir != 0) {
        dir->cd();
        TObject* obj = dir->Remove(theGraph);
        if (obj != 0) 
            graphRemoved = true;
    } 

    if (!graphRemoved) {
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode 
ManagedMonitorToolBase::
regManagedGraphs(std::vector< MgmtParams<TGraph> >& templateGraphs)
{
      // See the description for the regManagedHistograms method
      bool allIsOk = true;  
   
      for( std::vector< MgmtParams<TGraph> >::iterator it = templateGraphs.begin(); it != templateGraphs.end(); ++it ) {
          MonGroup group = (*it).m_group;

          // Get a handle to the graph
          TGraph* theGraph = (*it).m_templateHist;

          // Clone the graph
          TGraph* g = static_cast<TGraph*>(theGraph->Clone());
          theGraph->Set(0); // equivalent to Reset() for TH1

          // Get name
          std::string gName = g->GetName();

          // Get the streamName for the previous interval
          std::string streamName = streamNameFunction()->getStreamName( this, group, gName, true );

          // De-register the original graph with the THistSvc
          StatusCode sc1 = m_THistSvc->deReg( theGraph );
          if (sc1 == StatusCode::FAILURE)
              allIsOk = false;

          // *** begin ***
          // Fix THistSvc->deReg for TGraphs
          bool doneCleaning = false;
          std::string directoryName = streamNameFunction()->getDirectoryName( this, group, gName, true );
          TSeqCollection *filelist=gROOT->GetListOfFiles();
          for (int i=0; i<filelist->GetEntries(); i++) {
              ATH_MSG_DEBUG( "List of files: " << filelist->At(i)->GetName());
              TFile* file = static_cast<TFile*>(filelist->At(i));
              StatusCode sc2 = THistSvc_deReg_fixTGraph(file, theGraph, directoryName);
              if (sc2 == StatusCode::SUCCESS)
                  doneCleaning = true;
          }
         
          // Check if TGraph fix has been applied successfully 
          if (!doneCleaning) { 
              ATH_MSG_ERROR("THistSvc_deReg_fixTGraph: failed to apply TGraph fix for the THist Svc!");
              allIsOk = false;
          }
          // *** end ***

          // Register clonned histogram under previous interval streamName
          StatusCode sc3 = m_THistSvc->regGraph( streamName, g );
          if (sc3 == StatusCode::FAILURE) 
              allIsOk = false;

          // Get streamName for the current interval
          streamName = streamNameFunction()->getStreamName( this, group, gName, false );
          // Register metadata information with the current interval streamname
          StatusCode smd = registerMetadata(streamName, gName, group);
          if (smd != StatusCode::SUCCESS) 
              allIsOk = false;

          // Re-register the original graph with the current interval streamName
          StatusCode sc4 = m_THistSvc->regGraph( streamName, theGraph );
          if (sc4 == StatusCode::FAILURE) 
              allIsOk = false;

      }

      if (!allIsOk) return StatusCode::FAILURE;
      
      return StatusCode::SUCCESS;
}

StatusCode 
ManagedMonitorToolBase::
regManagedTrees(std::vector< MgmtParams<TTree> >& templateTrees)
{
      // See the description for the regManagedHistograms method
      bool allIsOk = true;  
   
      for( std::vector< MgmtParams<TTree> >::iterator it = templateTrees.begin(); it != templateTrees.end(); ++it ) {
          MonGroup group = (*it).m_group;

          // Get a handle to the original tree
          TTree* theTree = (*it).m_templateHist;

          // Clone the tree
          TTree* t = static_cast<TTree*>(theTree->Clone());
          theTree->Reset();

          // Dumping the tree
          std::string name = t->GetName();

          // Get the streamName for the previous interval
          std::string streamName = streamNameFunction()->getStreamName( this, group, name, true );

          // De-register original tree with the THistSvc
          StatusCode sc1 = m_THistSvc->deReg( theTree );
          if (sc1 == StatusCode::FAILURE) allIsOk = false;

          // Register clonned tree under previous interval streamName
          StatusCode sc2 = m_THistSvc->regTree( streamName, t );
          if (sc2 == StatusCode::FAILURE) allIsOk = false;

          if( m_manager != 0 ) {
             std::string genericName = NoOutputStream().getStreamName( this, group, name );
             m_manager->passOwnership( t, genericName );
             m_manager->writeAndDelete( genericName );
          }

          // Get streamName for the current interval
          streamName = streamNameFunction()->getStreamName( this, group, name, false );
          // Register metadata information with the current interval streamname
          StatusCode smd = registerMetadata(streamName, name, group);
          if (smd != StatusCode::SUCCESS) allIsOk = false;

          // Re-register the original graph with the current interval streamName
          StatusCode sc3 = m_THistSvc->regTree( streamName, theTree );
          if (sc3 == StatusCode::FAILURE) allIsOk = false;

      }

      if (!allIsOk) return StatusCode::FAILURE;
      
      return StatusCode::SUCCESS;
}

StatusCode
ManagedMonitorToolBase::
regManagedLWHistograms(std::vector<MgmtParams<LWHist> >& templateLWHistograms)
{
    StatusCode sc1;
    sc1.setChecked();

    for( std::vector< MgmtParams<LWHist> >::iterator it = templateLWHistograms.begin(); it != templateLWHistograms.end(); ++it ) {
        // Get histogram group
        MonGroup group = (*it).m_group;

        // Get handle to the histogram
        LWHist* h = (*it).m_templateHist;

        sc1 = regHist(h, group);
    }
    
   return sc1;
}

StatusCode
ManagedMonitorToolBase::
finalHists()
{

   // This assumes that the end of a file will naturally end a run, which is not always true.
   // A merging application run afterwards should be able to put parts of a run together.
   if( m_nEvents != 1 ) {
     m_d->benchPreProcHistograms();

     // Set end flags for the LowStat, LumiBlock and Run variables.
     // This is needed to be used in the procHistograms method below.
     m_endOfEventsBlock = true;
     m_endOfLowStat = true;
     m_endOfLumiBlock = true;
     m_endOfRun = true;
     endOfEventsBlock = true;
     endOfLowStat = true;
     endOfLumiBlock = true;
     endOfRun = true;

     StatusCode sc = procHistograms();

/*
     StatusCode sc1( StatusCode::SUCCESS );
     sc1.setChecked();

#if 0
     for (const auto interval: m_supportedIntervalsForRebooking) {
       //sc1 = regManagedHistograms(m_templateHistograms[interval], false);
       //sc1 = regManagedGraphs(m_templateGraphs[interval], false);
       //sc1 = regManagedTrees(m_templateTrees[interval], false);
       
       // Yura: commented out when fixing online environment
       //sc1 = regManagedLWHistograms(m_templateLWHistograms[interval], false, true);
       sc1.setChecked();
     }
*/

     m_d->benchPostProcHistograms();
     return sc;
   }
   return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolBase::
convertLWHists()
{
  // note that managed histograms will be converted by regMonitoredLWHistograms
  // hence they are not in m_lwhists
  if (m_manager) {
    std::set<LWHist*>::iterator it(m_lwhists.begin()),itE(m_lwhists.end());
    for (;it!=itE;++it)
      m_manager->writeAndDeleteLWHist( *it );
    m_lwhists.clear();
  }
  return StatusCode::SUCCESS;

}


StatusCode
ManagedMonitorToolBase::
bookHistogramsRecurrent( )
{
   if( m_newEventsBlock ) { }
   if( m_newLumiBlock ) { }
   if( m_newRun ) { }

   return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolBase::
bookHistograms( )
{

   return StatusCode::SUCCESS;
}

StatusCode
ManagedMonitorToolBase::
fillHistograms()
{
   return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolBase::
procHistograms( )
{
   if( m_endOfEventsBlock ) { }
   if( m_endOfLowStat ) { }
   if( m_endOfLumiBlock ) { }
   if( m_endOfRun) { }

   return StatusCode::SUCCESS;
}


void
ManagedMonitorToolBase::
setMonManager( AthenaMonManager* manager )
{
  ATH_MSG_DEBUG( "ManagedMonitorToolBase::setMonManager():");
  m_manager = manager;
  if( m_manager != 0 ) {
      ATH_MSG_DEBUG( "  --> Setting manager");
      m_managerNameProp = m_manager->name();
      m_fileKey = m_manager->fileKey();
      m_dataType = m_manager->dataType();
      m_environment = m_manager->environment();
      delete m_streamNameFcn;
      m_streamNameFcn = getNewStreamNameFcn();
   }
	ATH_MSG_DEBUG( "  --> Exiting successfully");
}

StatusCode
ManagedMonitorToolBase::
regHist( TH1* h, const std::string& system,
         Interval_t interval, MgmtAttr_t histo_mgmt, const std::string& chain, const std::string& merge )
{
   MonGroup group( this, system, interval, histo_mgmt, chain, merge );
   return regHist( h, group );
}

StatusCode
ManagedMonitorToolBase::
regHist( TH1* h, const MonGroup& group )
{
//   ManagedMonitorToolBase_addHistStatistics(this,h);

  if (!h)
    return StatusCode::FAILURE;
  
  // This part of the code deals with MANAGED type
  if ( group.histo_mgmt() != ATTRIB_UNMANAGED ) {
    /* 
       Create an unmanaged group based on the original MonGroup instance passed
       It is needed because managed histogram is presented as a number of unmanaged
       histograms (one per each interval)
       Update (PUEO) - I don't think it actually matters, and need to keep 
       track of "proper" attribute for X_VS_LB
    */
    
    if (group.histo_mgmt() == ATTRIB_X_VS_LB && group.merge() == "") {
      ATH_MSG_WARNING("HEY! You're attempting to register " << h->GetName() << " as a per-LB histogram, but you're not setting the merge algorithm! This is a SUPER-BAD idea! Use \"merge\", at least.");
    }
    
    if (m_supportedIntervalsForRebooking.count(group.interval())) {
      m_templateHistograms[group.interval()].push_back( MgmtParams<TH1>(h, group) );
    } else {
      ATH_MSG_ERROR("Attempt to book managed histogram " << h->GetName() << " with invalid interval type " << intervalEnumToString(group.interval()));
	       return StatusCode::FAILURE;
    }
    
    std::string hName = h->GetName();
    MonGroup group_unmanaged( this, group.system(), group.interval(), ATTRIB_UNMANAGED, group.chain(), group.merge());
    std::string streamName = streamNameFunction()->getStreamName( this, group_unmanaged, hName, false );
    StatusCode smd = registerMetadata(streamName, hName, group);
    smd.setChecked();
    return m_THistSvc->regHist( streamName, h );
  }
  
  // This part of the code deals with UNMANAGED type
  std::string hName = h->GetName();
  
  if( m_manager != 0 ) {
    std::string genericName = NoOutputStream().getStreamName( this, group, hName );
    m_manager->writeAndDelete( genericName );
    m_manager->passOwnership( h, genericName );
  }
  
  std::string streamName = streamNameFunction()->getStreamName( this, group, hName, false );
  
  StatusCode smd = registerMetadata(streamName, hName, group);
  if (smd != StatusCode::SUCCESS) return StatusCode::FAILURE;
  
  return m_THistSvc->regHist( streamName, h );
}

StatusCode ManagedMonitorToolBase::regHist( LWHist* h,const std::string& system,
                                           Interval_t interval, MgmtAttr_t histo_mgmt, const std::string& chain, const std::string& merge )
{
    MonGroup group( this, system, interval, histo_mgmt, chain, merge );
    return regHist( h, group );
}

StatusCode ManagedMonitorToolBase::regHist( LWHist* h, const MonGroup& group )
{
   // You may want to setROOTBackend to true in online environment
   //LWHistControls::setROOTBackend(true);
   
   if (!h)
      return StatusCode::FAILURE;

   if (!m_bookHistogramsInitial) {
           ATH_MSG_DEBUG("Yura: very first time");
	   if ( group.histo_mgmt() != ATTRIB_UNMANAGED ) {

               ATH_MSG_DEBUG("Yura: we have managed histograms");
	       if (m_supportedIntervalsForRebooking.count(group.interval())) {
                       ATH_MSG_DEBUG("        Yura: adding histogram" << h->GetName());
		       m_templateLWHistograms[group.interval()].push_back( MgmtParams<LWHist>(h, group) );
	       } else {
		       ATH_MSG_ERROR("Attempt to book managed histogram " << h->GetName() << " with invalid interval type " << intervalEnumToString(group.interval()));
		       return StatusCode::FAILURE;
	       }
	       //return StatusCode::SUCCESS; 
	   }
   }

    //FIXME: Code copied more or less verbatim from above. Collect most code (espc. for streamname) in common helpers!!
    std::string hName = h->GetName();

    if( m_manager )
    {
        std::string genericName = NoOutputStream().getStreamName(this, group, hName );
        LWHistAthMonWrapper::setKey(h,genericName);
        LWHist* prevLWHist = m_manager->ownedLWHistOfKey(genericName);
        if (prevLWHist)
        {
            std::set<LWHist*>::iterator it =  m_lwhists.find(prevLWHist);
            if (it!=m_lwhists.end())
            {
                if ( group.histo_mgmt() != ATTRIB_UNMANAGED ) {
                    m_manager->writeAndResetLWHist( genericName, LWHistAthMonWrapper::streamName(prevLWHist) );
                } else {
                    m_manager->writeAndDeleteLWHist( genericName, LWHistAthMonWrapper::streamName(prevLWHist) );
                }
                m_lwhists.erase(it);
            }
        }
        m_manager->passOwnership( h, genericName );
    }
    m_lwhists.insert(h);

    std::string streamName = streamNameFunction()->getStreamName( this, group, hName );
    LWHistAthMonWrapper::setStreamName(h,streamName);
    StatusCode smd = registerMetadata(streamName, hName, group);
    smd.setChecked();

    //Delay registration with THistSvc (unless root backend):
    //m_lwhistMap.insert(std::pair<LWHist*,std::string>(h,streamName));
    if (h->usingROOTBackend())
    {
        h->setOwnsROOTHisto(false);//Since might end up with thist svc
        return m_THistSvc->regHist( streamName, h->getROOTHistBase() );
    }

    return StatusCode::SUCCESS;

}

StatusCode
ManagedMonitorToolBase::
getHist( TH1*& h, const std::string& hName, const std::string& system,
      Interval_t interval )
{
   MonGroup group( this, system, interval );
   return getHist( h, hName, group );
}


StatusCode
ManagedMonitorToolBase::
getHist( TH1*& h, const std::string& hName, const MonGroup& group )
{
   std::string streamName = streamNameFunction()->getStreamName( this, group, hName );
   return m_THistSvc->getHist( streamName, h );
}


StatusCode ManagedMonitorToolBase::getHist( LWHist*& h, const std::string& hName, const std::string& system,
                                            Interval_t interval )
{
   MonGroup group( this, system, interval );
   return getHist( h, hName, group );
}


StatusCode ManagedMonitorToolBase::getHist( LWHist*& h, const std::string& hName, const MonGroup& group )
{
  h = 0;
  std::string streamName = streamNameFunction()->getStreamName( this, group, hName );
  std::set<LWHist*>::iterator it(m_lwhists.begin()),itE(m_lwhists.end());
  for (;it!=itE;++it) {
    if (LWHistAthMonWrapper::streamName(*it)==streamName) {
      h = *it;
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}


StatusCode
ManagedMonitorToolBase::
getHist( TH2*& h, const std::string& hName, const std::string& system,
         Interval_t interval )
{
   MonGroup group( this, system, interval );
   return getHist( h, hName, group );
}


StatusCode
ManagedMonitorToolBase::
getHist( TH2*& h, const std::string& hName, const MonGroup& group )
{
   std::string streamName = streamNameFunction()->getStreamName( this, group, hName );
   return m_THistSvc->getHist( streamName, h );
}


StatusCode
ManagedMonitorToolBase::
regGraph( TGraph* g, const std::string& system,
          Interval_t interval, MgmtAttr_t histo_mgmt, const std::string& chain, const std::string& merge )
{
   MonGroup group( this, system, interval, histo_mgmt, chain, merge );
   return regGraph( g, group );
}


StatusCode
ManagedMonitorToolBase::
regGraph( TGraph* g, const MonGroup& group )
{
   if (!g)
      return StatusCode::FAILURE;

   // This part of the code deals with MANAGED type
   if ( group.histo_mgmt() != ATTRIB_UNMANAGED ) {
       // Create an unmanaged group based on the original MonGroup instance passed
       // This is needed because managed graph is presented as a number of unmanaged
       // graphs (one per each interval)
       MonGroup group_unmanaged( this, group.system(), group.interval(), ATTRIB_UNMANAGED, group.chain(), group.merge());

       if (m_supportedIntervalsForRebooking.count(group.interval())) {
	       m_templateGraphs[group.interval()].push_back( MgmtParams<TGraph>(g, group_unmanaged) );
       } else {
	       ATH_MSG_ERROR("Attempt to book managed graph " << g->GetName() << " with invalid interval type " << intervalEnumToString(group.interval()));
	       return StatusCode::FAILURE;
       }

       std::string name = g->GetName();
       std::string streamName = streamNameFunction()->getStreamName( this, group_unmanaged, name, false );
       StatusCode smd = registerMetadata(streamName, name, group);
       smd.setChecked();
       return m_THistSvc->regGraph( streamName, g );
       //return m_THistSvc->regGraph( streamName );
   } 

   // This part of the code deals with UNMANAGED type
   std::string gName = g->GetName();

   if( m_manager != 0 ) {
      std::string genericName = NoOutputStream().getStreamName( this, group, gName );
      m_manager->writeAndDelete( genericName );
      m_manager->passOwnership( g, genericName );
   }

   std::string streamName = streamNameFunction()->getStreamName( this, group, gName, false );

   StatusCode smd = registerMetadata(streamName, gName, group);
   if (smd != StatusCode::SUCCESS) return StatusCode::FAILURE;

   return m_THistSvc->regGraph( streamName, g );
}


StatusCode
ManagedMonitorToolBase::
regTree( TTree* t, const std::string& system,
      Interval_t interval, MgmtAttr_t histo_mgmt, const std::string& chain, const std::string& merge )
{
   MonGroup group( this, system, interval, histo_mgmt, chain, merge );
   return regTree( t, group );
}


StatusCode
ManagedMonitorToolBase::
regTree( TTree* t, const MonGroup& group )
{

   // This part of the code deals with MANAGED type
   if ( group.histo_mgmt() != ATTRIB_UNMANAGED ) {
       // Create an unmanaged group based on the original MonGroup instance passed
       // This is needed because managed tree is presented as a number of unmanaged
       // trees (one per each interval)
       MonGroup group_unmanaged( this, group.system(), group.interval(), ATTRIB_UNMANAGED, group.chain(), group.merge());

       if (m_supportedIntervalsForRebooking.count(group.interval())) {
	       m_templateTrees[group.interval()].push_back( MgmtParams<TTree>(t, group_unmanaged) );
       } else {
	       ATH_MSG_ERROR("Attempt to book managed tree " << t->GetName() << " with invalid interval type " << intervalEnumToString(group.interval()));
	       return StatusCode::FAILURE;
       }

       std::string name = t->GetName();
       std::string genericName = NoOutputStream().getStreamName( this, group_unmanaged, name );
       std::string streamName = streamNameFunction()->getStreamName( this, group_unmanaged, name, false );
       StatusCode smd = registerMetadata(streamName, name, group);
       smd.setChecked();
       return m_THistSvc->regTree( streamName, t );
   }


   // This part of the code deals with UNMANAGED type
   std::string tName = t->GetName();

   if( m_manager != 0 ) {
      std::string genericName = NoOutputStream().getStreamName( this, group, tName );
      m_manager->writeAndDelete( genericName );
      m_manager->passOwnership( t, genericName );
   }

   std::string streamName = streamNameFunction()->getStreamName( this, group, tName, false );

   StatusCode smd = registerMetadata(streamName, tName, group);
   if (smd != StatusCode::SUCCESS) return StatusCode::FAILURE;

   return m_THistSvc->regTree( streamName, t );
}


StatusCode
ManagedMonitorToolBase::
writeAndDelete( TH1* h, const MonGroup& group ) {
  if (!h)
    return StatusCode::FAILURE;
  
  std::string hName = h->GetName();
  
  if( m_manager != 0 ) {
    std::string genericName = NoOutputStream().getStreamName( this, group, hName );
    m_manager->writeAndDelete( genericName );
  }
  return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolBase::
deregHist( TH1* h )
{
   return m_THistSvc->deReg( h );
}


StatusCode ManagedMonitorToolBase::deregHist( LWHist* h )
{
  std::set<LWHist*>::iterator it = m_lwhists.find(h);
  if (it==m_lwhists.end())
    return StatusCode::FAILURE;
  LWHistAthMonWrapper:: removeCustomData(h);
  m_lwhists.erase(it);
  if (h->usingROOTBackend()) {
    h->setOwnsROOTHisto(true);
    return m_THistSvc->deReg( h->getROOTHistBase() );
  }

  return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolBase::
deregGraph( TGraph* g )
{
   return m_THistSvc->deReg( g );
}


StatusCode
ManagedMonitorToolBase::
deregObject( const std::string& objName, const std::string& system,
             Interval_t interval )
{
   MonGroup group( this, system, interval );
   return deregObject( objName, group );
}


StatusCode
ManagedMonitorToolBase::
deregObject( const std::string& objName, const MonGroup& group )
{
   std::string streamName = streamNameFunction()->getStreamName( this, group, objName );
   std::set<LWHist*>::iterator it(m_lwhists.begin()),itE(m_lwhists.end());
   for (;it!=itE;++it) {
     LWHist * hlw = *it;
      if (LWHistAthMonWrapper::key(hlw)==objName) {
	m_lwhists.erase(it);
	if (hlw->usingROOTBackend()) {
	  hlw->setOwnsROOTHisto(true);
	  return m_THistSvc->deReg( hlw->getROOTHistBase() );
	}
	return StatusCode::SUCCESS;
      }
   }
   return m_THistSvc->deReg( streamName );
}


StatusCode
ManagedMonitorToolBase::
setupOutputStreams( std::vector<std::string> )
{
   // All instances should write to the stream(s) defined by the
   // AthenaMonManager.

   return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolBase::
runStat()
{
   return StatusCode::SUCCESS;
}


StatusCode
ManagedMonitorToolBase::
checkHists( bool )
{
   // Histograms will be checked using the data-quality monitoring framework (DQMF)

   return StatusCode::SUCCESS;
}


bool
ManagedMonitorToolBase::
preSelector()
{
   if( m_preScaleProp > 1 ) {
      return ( (m_nEvents % m_preScaleProp) == 1 );
   }
   return true;
}


// Average mu, i.e. <mu>
float
ManagedMonitorToolBase::
lbAverageInteractionsPerCrossing()
{
    if ( m_hasRetrievedLumiTool ) {
        return m_lumiTool->lbAverageInteractionsPerCrossing();
    } else {
        //ATH_MSG_FATAL("! Luminosity tool has been disabled ! lbAverageInteractionsPerCrossing() can't work properly! ");
        ATH_MSG_DEBUG("Warning: lbAverageInteractionsPerCrossing() - luminosity tools are not retrieved or turned on (i.e. EnableLumi = False)");
        return -1.0;
    }
    // not reached
}

// Instantaneous number of interactions, i.e. mu
float
ManagedMonitorToolBase::
lbInteractionsPerCrossing()
{
    if ( m_hasRetrievedLumiTool ) {
        float instmu = 0.;

        if (m_lumiTool->muToLumi() > 0.)
            instmu = m_lumiTool->lbLuminosityPerBCID()/m_lumiTool->muToLumi();

        return instmu;
    } else {
        //ATH_MSG_FATAL("! Luminosity tool has been disabled ! lbInteractionsPerCrossing() can't work properly! ");
        ATH_MSG_DEBUG("Warning: lbInteractionsPerCrossing() - luminosity tools are not retrieved or turned on (i.e. EnableLumi = False)");
        return -1.0;
    }
    // not reached
}

// Average luminosity (in ub-1 s-1 => 10^30 cm-2 s-1)
float
ManagedMonitorToolBase::
lbAverageLuminosity()
{
    if ( m_hasRetrievedLumiTool ) {
        return m_lumiTool->lbAverageLuminosity();
    } else {
        //ATH_MSG_FATAL("! Luminosity tool has been disabled ! lbAverageLuminosity() can't work properly! ");
        ATH_MSG_DEBUG("Warning: lbAverageLuminosity() - luminosity tools are not retrieved or turned on (i.e. EnableLumi = False)");
        return -1.0;
    }
    // not reached
}

// Instantaneous luminosity
float
ManagedMonitorToolBase::
lbLuminosityPerBCID()
{
    if ( m_hasRetrievedLumiTool ) {
        return m_lumiTool->lbLuminosityPerBCID();
    } else {
        //ATH_MSG_FATAL("! Luminosity tool has been disabled ! lbLuminosityPerBCID() can't work properly! ");
        ATH_MSG_DEBUG("Warning: lbLuminosityPerBCID() - luminosity tools are not retrieved or turned on (i.e. EnableLumi = False)");
        return -1.0;
    }
    // not reached
}


// Average luminosity livefraction
float
ManagedMonitorToolBase::
lbAverageLivefraction()
{
    if (m_environment == AthenaMonManager::online)
        return 1.0;

    if ( m_hasRetrievedLumiTool ) {
        return m_liveTool->lbAverageLivefraction();
    } else {
        //ATH_MSG_FATAL("! Luminosity tool has been disabled ! lbAverageLivefraction() can't work properly! ");
        ATH_MSG_DEBUG("Warning: lbAverageLivefraction() - luminosity tools are not retrieved or turned on (i.e. EnableLumi = False)");
        return -1.0;
    }
    // not reached
}

// Live Fraction per Bunch Crossing ID
float
ManagedMonitorToolBase::
livefractionPerBCID()
{
    if (m_environment == AthenaMonManager::online)
        return 1.0;

    if ( m_hasRetrievedLumiTool ) {
        return m_liveTool->livefractionPerBCID();
    } else {
        //ATH_MSG_FATAL("! Luminosity tool has been disabled ! livefractionPerBCID() can't work properly! ");
        ATH_MSG_DEBUG("Warning: livefractionPerBCID() - luminosity tools are not retrieved or turned on (i.e. EnableLumi = False)");
        return -1.0;
    }
    // not reached
}

// Average Integrated Luminosity Live Fraction
double
ManagedMonitorToolBase::
lbLumiWeight()
{
    if ( m_hasRetrievedLumiTool ) {
        return (lbAverageLuminosity()*lbDuration())*lbAverageLivefraction();
    } else{
        //ATH_MSG_FATAL("! Luminosity tool has been disabled ! lbLumiWeight() can't work properly! ");
        ATH_MSG_DEBUG("Warning: lbLumiWeight() - luminosity tools are not retrieved or turned on (i.e. EnableLumi = False)");
        return -1.0;
    }
    // not reached
}


// Luminosity block time (in seconds)
double
ManagedMonitorToolBase::
lbDuration()
{
    if ( m_environment == AthenaMonManager::online ) {
        return m_defaultLBDuration;
    }
    if ( m_hasRetrievedLumiTool ) {
        return m_lumiTool->lbDuration();
    } else {
        //ATH_MSG_FATAL("! Luminosity tool has been disabled ! lbDuration() can't work properly! ");
        ATH_MSG_DEBUG("Warning: lbDuration() - luminosity tools are not retrieved or turned on (i.e. EnableLumi = False)");
        return m_defaultLBDuration;
    }
    // not reached
}


// *********************************************************************
// Protected Methods
// *********************************************************************

ManagedMonitorToolBase::OutputMetadata::
   OutputMetadata( TTree* metadata )
   : m_charArrSize(100)
   , m_metadata(metadata)
   , m_nameData(0)
   //, m_levelData(0)
   , m_intervalData(0)
   , m_triggerData(0)
   , m_mergeData(0)
{
   m_nameData = new char[m_charArrSize];
   //m_levelData = new char[m_charArrSize];
   m_intervalData = new char[m_charArrSize];
   m_triggerData = new char[m_charArrSize];
   m_mergeData = new char[m_charArrSize];
   m_metadata->Branch( "Name", m_nameData, "Name/C" );
   //m_metadata->Branch( "LevelOfDetail", m_levelData, "LevelOfDetail/C" );
   m_metadata->Branch( "Interval", m_intervalData, "Interval/C" );
   m_metadata->Branch( "TriggerChain", m_triggerData, "TriggerChain/C" );
   m_metadata->Branch( "MergeMethod", m_mergeData, "MergeMethod/C" );
}


ManagedMonitorToolBase::OutputMetadata::
~OutputMetadata()
{
   delete [] m_mergeData;
   delete [] m_triggerData;
   delete [] m_intervalData;
   //delete [] m_levelData;
   delete [] m_nameData;
}


void
ManagedMonitorToolBase::OutputMetadata::
fill( const std::string& name,
      //LevelOfDetail_t level,
      Interval_t interval,
      std::string trigger,
      std::string merge )
{
   // ROOT does not properly write empty strings to output files in all cases.  I haven't reported
   // this to the ROOT developers yet because I don't have time to develope a simple test case
   // for them (independent of Atlas software).
   // --M.G.Wilson, 7 July 2008
   if( trigger == "" )
      trigger = "<none>";
   if( merge == "" )
      merge = "<default>";

   copyString( m_nameData, name );
   //copyString( m_levelData, ManagedMonitorToolBase::levelOfDetailEnumToString( level ) );
   copyString( m_intervalData, ManagedMonitorToolBase::intervalEnumToString( interval ) );
   copyString( m_triggerData, trigger );
   copyString( m_mergeData, merge );
   m_metadata->Fill();
}


void
ManagedMonitorToolBase::OutputMetadata::
copyString( char* to, const std::string& from )
{
   int i = 0;
   const char* f = from.c_str();
   while( (++i < m_charArrSize) && ((*to++ = *f++) != 0) ) {};
   if( i == m_charArrSize ) {
      *to = 0;
   }
}


std::string
ManagedMonitorToolBase::DefaultStream::
getStreamName( const ManagedMonitorToolBase*, const MonGroup& group, const std::string& objName, bool )
{
   std::ostringstream streamName;

   streamName << "/" << m_fileKey << "/";

   streamName << group.system() << "/" << objName;
   return streamName.str();
}

std::string
ManagedMonitorToolBase::DefaultStream::
getDirectoryName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, const bool usePreviousInterval )
{
    std::string streamName = getStreamName(tool, group, objName, usePreviousInterval);
    std::string root, rem;
    parseString(streamName, root, rem);
    // Remove object name at the end
    // to obtain directory path
    rem.erase(rem.rfind("/"), rem.length()); 
    return rem;
}


std::string
ManagedMonitorToolBase::NoOutputStream::
getStreamName( const ManagedMonitorToolBase*, const MonGroup& group, const std::string& objName, bool )
{
   std::ostringstream streamName;
   streamName << group.system() << "/" << objName;
   return streamName.str();
}


std::string
ManagedMonitorToolBase::NoOutputStream::
getDirectoryName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, const bool usePreviousInterval )
{
   return getStreamName(tool, group, objName, usePreviousInterval);
}

std::string
ManagedMonitorToolBase::OnlineStream::
getStreamName( const ManagedMonitorToolBase*, const MonGroup& group, const std::string& objName, bool )
{
   std::ostringstream streamName;
   //if( group.level() != ManagedMonitorToolBase::transient ) {
   //   streamName << "/" << ManagedMonitorToolBase::levelOfDetailEnumToString( group.level(), true ) << "/";
   //}
   streamName << "/";   // slash is required for online environment; 
                        // otherwise the output histograms are placed in 'temp' subdirectory

   streamName << group.system() << "/" << objName;
   return streamName.str();
}

std::string
ManagedMonitorToolBase::OnlineStream::
getDirectoryName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, const bool usePreviousInterval )
{
   return getStreamName(tool, group, objName, usePreviousInterval);
}

void
ManagedMonitorToolBase::OfflineStream::
getLBrange(int *start, int *end, int LB, int length) {
   if(start && end) {
      *start = ((LB-1)/length) * length + 1;
      *end = *start + length - 1;
   }
}

std::string
ManagedMonitorToolBase::OfflineStream::
getStreamName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, bool usePreviousInterval )
{
   std::ostringstream streamName;

   //bool isTemp =    ( group.level() == ManagedMonitorToolBase::transient );
   bool isTemp = false;

   bool useRunFolders = group.interval() != all;

   bool useLBFolders =    ( useRunFolders )
      && ( group.interval() == ManagedMonitorToolBase::lumiBlock );

   bool useLowStatInterval =    ( useRunFolders )
      && ( group.interval() == ManagedMonitorToolBase::lowStat );

   bool useMedStatInterval =    ( useRunFolders )
      && ( group.interval() == ManagedMonitorToolBase::medStat );

   bool useHigStatInterval =    ( useRunFolders )
      && ( group.interval() == ManagedMonitorToolBase::higStat );

   bool useEBFolders = ( group.interval() == ManagedMonitorToolBase::eventsBlock );

   if( !isTemp ) {
      streamName << "/" << m_fileKey << "/";
   }

   if( useRunFolders ) {
      if (usePreviousInterval && (group.interval() == ManagedMonitorToolBase::run) )
          streamName << "run_" << m_prev_run_number << "/";
      else
          streamName << "run_" << AthenaMonManager::runNumber() << "/";
   }

   int currentLB = AthenaMonManager::lumiBlockNumber();
   if( useLBFolders ) {
      if (usePreviousInterval && (group.interval() == ManagedMonitorToolBase::lumiBlock) )
          streamName << "lb_" << m_prev_lumi_block << "/";
      else
          streamName << "lb_" << currentLB << "/";
   }
   else if( useLowStatInterval ) {
      int start, end;
      if (usePreviousInterval && (group.interval() == ManagedMonitorToolBase::lowStat) )
          getLBrange(&start, &end, m_prev_lumi_block, AthenaMonManager::getLBsLowStat());
      else
          getLBrange(&start, &end, currentLB, AthenaMonManager::getLBsLowStat());
      streamName << "lowStat_LB" << start << "-" << end << "/";
   }
   else if( useMedStatInterval ) {
      int start, end;
      getLBrange(&start, &end, currentLB, AthenaMonManager::getLBsMedStat());
      streamName << "medStat_LB" << start << "-" << end << "/";
   }
   else if( useHigStatInterval ) {
      int start, end;
      getLBrange(&start, &end, currentLB, AthenaMonManager::getLBsHigStat());
      streamName << "higStat_LB" << start << "-" << end << "/";
   }
   else if( useEBFolders ) {
      // The number of events in an EventsBlock can vary for each ManagedMonitorToolBase object,
      // so there is no global way to determine when an EventsBlock has rolled over.
   
      // determine an eventsBlock number     
      long eventsBlockNumber = 1;       // assign some dafault value 
      long procNEventsProp = tool->get_procNEventsProp(); 
      unsigned int nEvents = tool->get_nEvents();
      if (procNEventsProp > 0) {
          eventsBlockNumber = (long) nEvents / procNEventsProp;
          if ((nEvents % procNEventsProp) != 0)
              eventsBlockNumber++;
       }
     
      // lower eventsBlock number by 1 if usePreviousInterval is true; 
      if (usePreviousInterval) {
          eventsBlockNumber--;
      }
      
      streamName << "eb_" << eventsBlockNumber << "/";
   }

   streamName << group.system() << "/" << objName;

   return streamName.str();
}

std::string
ManagedMonitorToolBase::OfflineStream::
getDirectoryName( const ManagedMonitorToolBase* tool, const MonGroup& group, const std::string& objName, const bool usePreviousInterval )
{
    std::string streamName = getStreamName(tool, group, objName, usePreviousInterval);
    std::string root, rem;
    parseString(streamName, root, rem);
    // Remove object name at the end
    // to obtain directory path
    rem.erase(rem.rfind("/"), rem.length()); 
    return rem;
}

void
ManagedMonitorToolBase::OfflineStream::
updateRunLB() {
  m_prev_run_number = AthenaMonManager::runNumber();
  m_prev_lumi_block = AthenaMonManager::lumiBlockNumber();
}

bool 
ManagedMonitorToolBase::
trigChainsArePassed(std::vector<std::string>& vTrigNames) 
{
  ATH_MSG_DEBUG( "ManagedMonitorToolBase::trigChainsArePassed:");

   for(unsigned int i=0; i<vTrigNames.size(); i++) {
      if( m_trigDecTool->isPassed(vTrigNames[i]) ) {
	ATH_MSG_DEBUG( "  + \"" << vTrigNames[i] << "\" passed, returning \'true\'");
	return true;
      }
      else {
	ATH_MSG_DEBUG( "  - \"" << vTrigNames[i] << "\" did not pass");
      }
   }

   return false;
}

StatusCode 
ManagedMonitorToolBase::
parseList(const std::string& line, std::vector<std::string>& result) {
   std::string item;
   std::stringstream ss(line);

   if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ManagedMonitorToolBase::parseList:";

   while ( std::getline(ss, item, ',') ) {
      std::stringstream iss(item); // remove 
      iss >> item;                 // whitespace 
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " " << item;
      result.push_back(item);
   }

   msg(MSG::DEBUG) << endmsg;
   return StatusCode::SUCCESS;
}

void
ManagedMonitorToolBase::
updateTriggersForGroups(std::vector<std::string>& vTrigChainNames) {
  for (size_t i = 0; i < vTrigChainNames.size(); ++i) {
    std::string& thisName = vTrigChainNames[i];
    if (thisName.substr(0, 9) == "CATEGORY_") {
      ATH_MSG_DEBUG("Found a trigger category: " << thisName << ". We will unpack it.");
      std::vector<std::string> triggers = m_trigTranslator->translate(thisName.substr(9,std::string::npos));
      std::ostringstream oss;
      oss << "(";
      for (size_t itrig = 0; itrig < triggers.size(); ++itrig) {
	if (itrig != 0) { 
	  oss << "|";
	}
	oss << triggers[itrig];
      }
      oss << ")";
      // replace with new value
      std::string newval = oss.str();
      ATH_MSG_DEBUG("Replaced with " << newval);
      vTrigChainNames[i] = newval;
    }
  }
}

ManagedMonitorToolBase::StreamNameFcn*
ManagedMonitorToolBase::
getNewStreamNameFcn() const
{
   StreamNameFcn* fcn(0);

   switch( m_environment ) {
      case AthenaMonManager::noOutput:
         fcn = new NoOutputStream();
         break;
      case AthenaMonManager::online:
         fcn = new OnlineStream();
         break;
      case AthenaMonManager::user:
         fcn = new DefaultStream( m_fileKey );
         break;
      case AthenaMonManager::tier0:
      case AthenaMonManager::tier0Raw:
      case AthenaMonManager::tier0ESD:
      case AthenaMonManager::AOD:
      case AthenaMonManager::altprod:
      default:
         fcn = new OfflineStream( m_fileKey, m_dataType, m_environment );
   }

   return fcn;
}

void
ManagedMonitorToolBase::StreamNameFcn::
parseString(const std::string& streamName, std::string& root, std::string& rem) {
  std::string::size_type pos = streamName.find("/");

  if (pos == std::string::npos) {
    root = "";
    rem = streamName;
    return;
  }

  if (pos == 0) {
    parseString(streamName.substr(1,streamName.length()),root,rem);
  } else {
    root = streamName.substr(0,pos);
    rem = streamName.substr(pos+1,streamName.length());
  }

}

// *********************************************************************
// Private Methods
// *********************************************************************

// GlobalDirectoryRestore ctor
ManagedMonitorToolBase::
GlobalDirectoryRestore::
GlobalDirectoryRestore() {
  m_gd = gDirectory;
  m_gf = gFile;
}

// GlobalDirectoryRestore dtor
ManagedMonitorToolBase::
GlobalDirectoryRestore::
~GlobalDirectoryRestore() {
  gDirectory = m_gd;
  gFile = m_gf;
}

namespace {

   std::string strToLower( const std::string& str )
   {
      std::string lstr(str);
      std::string::const_iterator  from   = str.begin();
      std::string::const_iterator  strend = str.end();
      std::string::iterator        to     = lstr.begin();
      while( from != strend ) {
         *to++ = tolower(*from++);
      }
      return lstr;
   }


#if 0
   std::string strToUpper( const std::string& str )
   {
      std::string ustr(str);
      std::string::const_iterator  from   = str.begin();
      std::string::const_iterator  strend = str.end();
      std::string::iterator        to     = ustr.begin();
      while( from != strend ) {
         *to++ = toupper(*from++);
      }
      return ustr;
   }
#endif

} // unnamed namespace

#endif
