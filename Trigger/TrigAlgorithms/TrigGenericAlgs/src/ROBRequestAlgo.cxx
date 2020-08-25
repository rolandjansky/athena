/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief  Algorithm to perform predefined ROB requests
 * @author Frank Winklmeier
 *
 * $Id: $
 */

#include "ROBRequestAlgo.h"

#include "GaudiKernel/IJobOptionsSvc.h"
#include "Gaudi/Property.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include <math.h>
#include <sstream>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

using namespace std;

const short N_ROBID_BITS(24);  // 3 bytes for ROB ID

ROBRequestAlgo::ROBRequestAlgo(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_robDataProviderSvc("ROBDataProviderSvc", name)
{
  declareProperty("ROBRequest", m_robRequest, "List of ROB requests");

  /* Uncomment the following properties for testing purposes */
  //declareProperty("enabledROBs", m_enabledROBs);
  //declareProperty("RobRosMapping", m_rob_ros_map);
}


HLT::ErrorCode ROBRequestAlgo::hltInitialize()
{
  ServiceHandle<IJobOptionsSvc> jobOptSvc("JobOptionsSvc", name());

  // get the list of enabled ROBs
  const Gaudi::Details::PropertyBase* p = Gaudi::Utils::getProperty(jobOptSvc->getProperties("DataFlowConfig"),
                                                "DF_Enabled_ROB_IDs");
  if (p) m_enabledROBs.assign(*p);
  else ATH_MSG_DEBUG("Could not find property DataFlowConfig.DF_Enabled_ROB_IDs");

  // get ROB/ROS mapping
  p = Gaudi::Utils::getProperty(jobOptSvc->getProperties("DataFlowConfig"), "DF_ROB_ROS_Mapping");
  if (p) m_rob_ros_map.assign(*p);
  else ATH_MSG_DEBUG("Could not find property DataFlowConfig.DF_ROB_ROS_Mapping");

  // fill the map of  ROS -> list of assigned ROBs
  for (std::map< int,int >::const_iterator it = m_rob_ros_map.value().begin();
       it != m_rob_ros_map.value().end(); ++it) {
    m_ros_rob_map[ (*it).second ].push_back( (*it).first ) ;
  }
  
  if (m_robDataProviderSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Cannot retrieve " << m_robDataProviderSvc);
    return HLT::BAD_JOB_SETUP;
  }

  m_robRequest.declareUpdateHandler(&ROBRequestAlgo::updateHandler, this);
  parseROBRequest();
  return HLT::OK;
}


void ROBRequestAlgo::updateHandler(Gaudi::Details::PropertyBase&)
{
  parseROBRequest();
}



/**
 * Parse job property
 */
void ROBRequestAlgo::parseROBRequest()
{
  m_requestList.clear();

  for( const string& s : m_robRequest.value() ) {
    boost::tokenizer<boost::char_separator<char> > w(s, boost::char_separator<char>(","));
    
    vector<ROBRequest> req;
    for( string spec : w ) { // e.g. 0x11#2
      boost::trim(spec);
      
      string robspec;
      ROBRequest r;
      boost::tokenizer<boost::char_separator<char> > tok(spec, boost::char_separator<char>("#"));
      vector<string> v(tok.begin(), tok.end());
      if ( v.size()==1 ) {
        robspec = v[0];
      }
      else if ( v.size()==2 ) {
        robspec = v[0];
        char *p;
        r.N = strtoul(v[1].c_str(), &p, 0);
        if ( *p != 0) {
          ATH_MSG_ERROR("Cannot convert " << v[1] << "to integer");
          continue;
        }        
      }
      else {
        ATH_MSG_ERROR("Invalid request specification " << spec);
        continue;
      }
      boost::trim(robspec);
      if (!getROBs(robspec, r)) {
        ATH_MSG_ERROR("Error parsing " << robspec);
        continue;
      }
      req.push_back(r);      
    }
    m_requestList.push_back(req);
  }

  // Print
  int i(1);
  for( const vector<ROBRequest>& v : m_requestList ) {
    ostringstream os;
    os << "ROB request #" << i++ << ": ";
    for( const ROBRequest& r : v ) {
      if (r.N<0) os << "all of [";
      else os << r.N << " of [";
      os << hex;
      for(uint32_t rob : r.robs) {
        os << " 0x" << rob;
      }
      os << dec;
      os << "] ";
    }    
    ATH_MSG_INFO(os.str());
  }
}

/**
 * Get list of enabled ROBs matching robSpec
 */
bool ROBRequestAlgo::getROBs(const std::string& robSpec, ROBRequest& req)
{
  req.robs.clear();
  char *p;

  // In case ROS ID is specified
  if (robSpec.substr(0,4)=="ROS-") {
    uint32_t rosid = strtoul(robSpec.substr(4,string::npos).c_str(), &p, 0);
    if ( *p != 0 ) return false;
    req.robs = m_ros_rob_map[rosid];
    return true;
  }
  
  uint32_t rob_mask = strtoul(robSpec.c_str(), &p, 0);  // Convert string to int (also does hex conversion)
  if ( *p != 0 ) return false;
  
  if (rob_mask==0) {
    req.robs.assign(m_enabledROBs.value().begin(), m_enabledROBs.value().end());
    return true;
  }

  // number of bits (in multiple of half-bytes) used by robSpec
  short bits = 4*(int(log(rob_mask)/log(16))+1);

  if (bits>N_ROBID_BITS)           // invalid spec
    return false;
  else if (bits==N_ROBID_BITS)     // full ROB ID given
    req.robs.push_back(rob_mask);  
  else {                           // ROB mask given
    for(uint32_t rob : m_enabledROBs.value()) {
      if ((rob>>(N_ROBID_BITS-bits))==rob_mask) req.robs.push_back(rob);
    }
  }
  return true;
}


HLT::ErrorCode ROBRequestAlgo::hltExecute(std::vector<HLT::TEVec>&, unsigned int)
{

  beforeExecMonitors().ignore();
  
  for( const vector<ROBRequest>& v : m_requestList ) {

    // Prepare ROB list
    ROBList roblist;
    for( ROBRequest r : v ) r.select(roblist);

    // Retrieve ROBs
    std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFragments;
    m_robDataProviderSvc->addROBData(roblist);
    m_robDataProviderSvc->getROBData(roblist, robFragments);
    
    if (msgLvl(MSG::DEBUG)) {
      ostringstream os;
      os << hex;
      for(uint32_t rob : roblist) os << " 0x" << rob;
      ATH_MSG_DEBUG(roblist.size() << "/" << robFragments.size()
                    << " ROBs requested/retrieved:" << os.str());
    }
  }

  afterExecMonitors().ignore();
  return HLT::OK;
}

/**
 * Select specified number of ROBs from list
 */
void ROBRequest::select(ROBList& roblist)
{
  if (N>0) {
    random_shuffle(robs.begin(), robs.end());
    roblist.insert(roblist.end(), robs.begin(), robs.begin()+std::min(size_t(N),robs.size()));
  }
  else if (N<0) {
    roblist.insert(roblist.end(), robs.begin(), robs.end());
  }
}
