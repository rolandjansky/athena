/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     MonitorToolBase.cxx
//
// DESCRIPTION: Base abstract class for creating monitoring histogramming tools
//
// AUTHOR:   Manuel Diaz & Peter Loch
//           Rolf Seuster  28.10.2005:  added support for logging output of
//                                      checkHists() into logfile
//           Rolf Seuster  24.10.2005:  added support for THistSvc
//           Rob McPherson 20.03.2004:  added finalHists()
//           Rob McPherson 29.03.2004:  removed execute() default.
//                                      reworked book flag routines
//  	     Manuel Diaz   01.04.2004   slight change in class definition(inherit from IMonitorToolBase)
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "AthenaMonitoring/LogFileMsgStream.h"
#include "AthenaMonitoring/MonitorToolBase.h"

/*---------------------------------------------------------*/
MonitorToolBase::MonitorToolBase(const std::string & type, 
				 const std::string & name,
				 const IInterface* parent)
  : AthAlgTool(type, name, parent)
    , m_histsvc(0)
    , m_rootsvc(0)
    , m_isBooked(false)
    , m_path("/stat")
    , m_setupStreamMap(false)
    , m_LogFileName("my.default")
    , m_FormatString("")
    , m_preScale(1)
    , m_counter(1)
/*---------------------------------------------------------*/
{
  declareProperty("histoPathBase",m_path);
  declareProperty("THistSvc_OutStream",m_THistSvc_streamname="/MonitorOutPut");
  declareProperty("THistSvc_RefStream",m_THistSvc_refStream="none");
  
  m_THistSvc_streamnameMapping.clear();
  m_map_THistSvc_streamnames.clear();
  declareProperty("THistSvc_OutPutStreamMapping",m_THistSvc_streamnameMapping);
  
  // use a rather stupid default name, the real default
  // later will be "CheckHist_"+RunNumber+".log"
  // but now, we don't know the RunNumber yet!
  // for now, overwritten by jobOptions
  declareProperty("logfileName",  m_LogFileName);
  declareProperty("Format",       m_FormatString);
  declareProperty("PreScale",     m_preScale);
}

/*---------------------------------------------------------*/
MonitorToolBase::~MonitorToolBase()
/*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "destructor has been called" << endmsg;
}

/*---------------------------------------------------------*/
std::string
MonitorToolBase::getStreamName(unsigned int number, bool useDefault /* =true */ )
/*---------------------------------------------------------*/
{
  std::string str="empty"; // no trailing slash, histogram will be temporary
  
  if( ! m_setupStreamMap)
    if( ! this->setupOutputStreams().isSuccess())
      {
	MsgStream log(msgSvc(), name ());
	log << MSG::ERROR
	    << "Could not setup Outputstreams !" << endmsg;
      }
  
  // valid entry in vector ?
  if( number < m_THistSvc_streamnameMapping.size() )
    return m_THistSvc_streamnameMapping[number];
  
  // shall we use the default instead ?
  if( useDefault )
    return m_THistSvc_streamname;
  
  // return "empty" string to make histogram temporary
  return str;
}

/*---------------------------------------------------------*/
std::string
MonitorToolBase::getStreamName(std::string stream, bool useDefault /* = true */ )
/*---------------------------------------------------------*/
{
  std::string str="empty"; // no trailing slash, histogram will be temporary
  
  if( ! m_setupStreamMap)
    if( ! this->setupOutputStreams().isSuccess() )
      {
	MsgStream log(msgSvc(), name ());
	log << MSG::ERROR
	    << "Could not setup Outputstreams !" << endmsg;
      }
  
  // valid entry in map ?
  map_type::iterator itr = m_map_THistSvc_streamnames.find(stream);
  if( itr != m_map_THistSvc_streamnames.end() )
    return itr->second;
  
  // shall we use the default instead ?
  if( useDefault )
    return m_THistSvc_streamname;
  
  // return "empty" string to make histogram temporary
  return str;
}

/*---------------------------------------------------------*/
StatusCode
MonitorToolBase::setupOutputStreams(std::vector<std::string> Mapping
				    /* = std::vector<std::string>()  */ )
/*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name ());
  
  // following statement is useless in the initialize,
  //  if the concrete tool implements its own iniitalize
  m_counter=m_preScale;
  
  // did we get some stream mapping from AthenaMon in Mapping ?
  if( Mapping.size() > 0 )
    // yes, insert it into m_THistSvc_streamnameMapping
    m_THistSvc_streamnameMapping.insert(m_THistSvc_streamnameMapping.begin(),
					Mapping.begin(),Mapping.end());
  
  std::vector<std::string>::iterator itr = m_THistSvc_streamnameMapping.begin();
  while( itr != m_THistSvc_streamnameMapping.end() )
    {
      std::string::size_type location = itr->find("/", 0);
      if( location == std::string::npos )
	{
	  log << MSG::ERROR
	      << "No '/' found in StreamName " << *itr
	      << " ! Will not use this stream !" << endmsg;
	  itr = m_THistSvc_streamnameMapping.erase(itr);
	  continue;
	}
      
      // check, if already in map and issue warning that it's being replaced
      std::map<std::string,std::string>::iterator iter =
	m_map_THistSvc_streamnames.find(itr->substr(0,location));
      if( iter != m_map_THistSvc_streamnames.end() )
	{
	  log << MSG::WARNING
	      << "Will overwrite mapping of mnemonic/stream \""
	      << iter->first << iter->second << "\" with \""
	      << itr->substr(0,location)
	      << itr->substr(location) << "\" !!"
	      << endmsg;
	  iter->second=itr->substr(location);
	}else{
	  m_map_THistSvc_streamnames[itr->substr(0,location)]=itr->substr(location);
	}
      itr->erase(0,location);
      ++itr;
    }
  
  if(m_map_THistSvc_streamnames.size()>0)
    {
      log << MSG::INFO
	  << "Using following mapping of mnemonics to streams (in alphabetical order):"
	  << endmsg;
      for( map_type::const_iterator iter = m_map_THistSvc_streamnames.begin();
	   iter != m_map_THistSvc_streamnames.end(); ++iter )
	log << MSG::INFO 
	    << std::setw(20)
	    << iter->first << " writes to "
	    << std::setw(20)
	    << iter->second
	    << endmsg;
    }
  // don't call again
  m_setupStreamMap=true;
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
bool MonitorToolBase::preSelector()
/*---------------------------------------------------------*/
{
  if(m_counter==1)
    {
      m_counter=m_preScale;
      return true;
    }
  else
    if(m_counter>1)
      m_counter--;
  return false;
}

/*---------------------------------------------------------*/
void MonitorToolBase::SetBookStatus(bool status)
/*---------------------------------------------------------*/
{
  m_isBooked = status;
}

/*---------------------------------------------------------*/
bool MonitorToolBase::histsNotBooked()
/*---------------------------------------------------------*/
{
 return (not m_isBooked);
}

/*---------------------------------------------------------*/
StatusCode MonitorToolBase::finalHists() // Default finalHists(): do nothing.
/*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "default finalHists() called." << endmsg;
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode MonitorToolBase::runStat() // Default runStat(): do nothing.
/*---------------------------------------------------------*/
{
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode MonitorToolBase::checkHists(bool fromFinalize) // Default checkHists(): do nothing.
/*---------------------------------------------------------*/
{
  LogFileMsgStream log(msgSvc(), name(), m_LogFileName, fromFinalize);
  if(m_FormatString!="")
    log.setFormat(m_FormatString);
  
  if(fromFinalize)
    log << MSG::INFO << "default checkHists() called from finalize()." << endmsg;
  else
    log << MSG::INFO << "default checkHists() called periodically." << endmsg;
  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/
IHistogramSvc* MonitorToolBase::ToolHistoSvc()
/*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
 
  StatusCode sc = service("HistogramDataSvc",m_histsvc, true);
  if( sc.isFailure() ) {
    log << MSG::WARNING << ">>> Unable to locate the Histogram service" << endmsg;
  }

  return m_histsvc;
}
/*---------------------------------------------------------*/
ITHistSvc* MonitorToolBase::ToolRootHistSvc()
/*---------------------------------------------------------*/
{
  MsgStream log(msgSvc(), name());
 
  StatusCode sc = service("THistSvc",m_rootsvc, true);
  if( sc.isFailure() ) {
    log << MSG::WARNING << ">>> Unable to locate the Histogram service" << endmsg;
  }
  
  return m_rootsvc;
}
/*---------------------------------------------------------*/
