/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLBASECLASS
#define LARROD_LARRAWCHANNELBUILDERTOOLBASECLASS

#include "GaudiKernel/MsgStream.h"

#include "LArROD/LArRawChannelBuilderToolBaseClass.h"
#include "LArROD/LArRawChannelBuilderStatistics.h"
#include "LArROD/LArRawChannelBuilderDriver.h"

#include <iomanip>
#include <cmath>

LArRawChannelBuilderToolBaseClass::LArRawChannelBuilderToolBaseClass(const std::string& type,
								     const std::string& name,
								     const IInterface* parent):
  AthAlgTool(type,name,parent),
  pParent(NULL),
  helper(NULL),
  m_detStore(NULL),
  m_larCablingSvc(0)
  ,m_isSC(false)
{
declareProperty("IsSuperCell",              m_isSC  = false);
}

LArRawChannelBuilderToolBaseClass::~LArRawChannelBuilderToolBaseClass() {
  delete helper;
}

void
LArRawChannelBuilderToolBaseClass::printSummary()
{
  MsgStream log(msgSvc(), name());
  ulonglong sum=0;
  for( unsigned int i=0; i<helper->returnMaxErrors(); i++ )
    sum+=helper->returnErrorCount(i);
  
  log << MSG::INFO << "Printing Summary for tool " << name() << " :" << endreq;
  log << MSG::INFO
      << std::setw(40) << " " << " : "
      << std::setw(12) << " total "
      << std::setw(10) << " percent " << "  "
      << std::setw(12) << " per event "
      << std::setw(12) << " RMS "
      << endreq;
  
  for( unsigned int i=0; i<helper->returnMaxErrors(); i++ )
    {
      double fraction = 0.0;
      double perEvent = 0.0;
      double rmsPerEv = 0.0;
      if(sum>0)
        fraction=100 * double(helper->returnErrorCount(i)) / double(sum);
      if(helper->returnEvents()>0)
        {
          perEvent = double(helper->returnErrorCountPerEvent(i)) / double(helper->returnEvents());
          rmsPerEv = double(helper->returnErrorCountSqPerEvent(i)) / double(helper->returnEvents());
          rmsPerEv = sqrt(std::max(0.,rmsPerEv-perEvent*perEvent));
        }
      
      log << MSG::INFO
	  << std::setw(40)
	  << helper->returnErrorString(i) << " : "
	  << std::setw(12)
	  << helper->returnErrorCount(i)
	  << std::setw(10)
	  << std::fixed
	  << std::setprecision(4)
	  << fraction
	  << " %"
	  << std::setw(12)
	  << std::fixed
	  << std::setprecision(4)
	  << perEvent
	  << std::setw(12)
	  << std::fixed
	  << std::setprecision(4)
	  << rmsPerEv
	  << endreq;
    }
}

void
LArRawChannelBuilderToolBaseClass::finalEventHidden()
{
  helper->resetErrorcountPerEvent();
  finalEvent();
}

Identifier
LArRawChannelBuilderToolBaseClass::currentID( void )
{
  if(pParent->curr_id==0)
    {
      try {
        pParent->curr_id = m_larCablingSvc->cnvToIdentifier(pParent->curr_chid);
      } catch ( LArID_Exception & except ) {
	MsgStream log(msgSvc(), name());
        log << MSG::INFO
	    << "A larCablingSvc exception was caught for channel 0x!"
	    << MSG::hex << pParent->curr_chid.get_compact() << MSG::dec << endreq;
        // log << MSG::INFO<<m_onlineHelper->print_to_string(curr_chid)<<endreq;
        pParent->curr_id=0;
      }
    }
  return pParent->curr_id;
}


#endif
