/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigISHelper.cxx
 * @brief  Helper tool for IS access
 * @author Frank Winklmeier, Werner Wiedenmann
 *
 * $Id: TrigISHelper.cxx 5 2013-05-14 10:33:04Z ricab $
 */

#include "TrigServices/TrigISHelper.h"


using namespace std;

//=========================================================================
// Standard methods
//=========================================================================
TrigISHelper::TrigISHelper(const std::string &type,
                           const std::string &name,
                           const IInterface *parent)
  : AthAlgTool(type, name, parent)
{
  // No abstract interface due to templates
  declareInterface<TrigISHelper>(this);
  
  declareProperty("PartitionName", m_partitionName = "NONE",
                  "Default partition name if not explicitly specified in IS identifier properties");
  m_partitionName.declareUpdateHandler(&TrigISHelper::initPartitionName, this);

  declareProperty("ignoreIfMissing", m_ignoreIfMissing = false,
                  "Return SUCCESS if IS object is missing (for testing only)");

  const string& help = "IS identifier: [Partition/]ISServer.ISInfoName";

  declareProperty("RunParams",
                  m_isprop[RunParams] = "RunParams.RunParams",
                  help);
  
  declareProperty("LumiBlock",
                  m_isprop[LumiBlock] = "RunParams.LumiBlock",
		  help);
    
  declareProperty("SolenoidCurrent",
                  m_isprop[SolenoidCurrent] = "initial/DCS_GENERAL.MagnetSolenoidCurrent.value",
                  help);
  
  declareProperty("ToroidCurrent",
                  m_isprop[ToroidCurrent] = "initial/DCS_GENERAL.MagnetToroidsCurrent.value",
                  help);
}


TrigISHelper::~TrigISHelper()
{}


StatusCode TrigISHelper::initialize()
{
  if (m_ignoreIfMissing ) {
    ATH_MSG_WARNING("Will ignore any missing IS objects and return SUCCESS");
  }

  return StatusCode::SUCCESS;
}



void TrigISHelper::initPartitionName(Property& /*prop*/)
{
  m_isid.clear();
  
  ATH_MSG_DEBUG("Configured ISInfo objects (partition : name):");
  
  // Fill map with partition and IS names
  map<ISObject, string>::const_iterator iter = m_isprop.begin();
  for (; iter != m_isprop.end(); ++iter ) {
    m_isid[iter->first] = split(iter->second);

    ATH_MSG_DEBUG("   " << m_isid[iter->first].first
                  << " : " << m_isid[iter->first].second);
  } 
}

//=========================================================================
// Read IS value
//=========================================================================
StatusCode TrigISHelper::findValue(ISObject obj, ISInfo& value)
{
  const string& partName = m_isid[obj].first;
  const string& isName = m_isid[obj].second;

  // read values from IS
  try {
    IPCPartition part(partName);
    ISInfoDictionary isInfoDict(part);
    isInfoDict.findValue(isName, value);
  }
  catch (daq::is::Exception& e) {
    ATH_MSG_WARNING("IS Exception reading " << isName << " from partition "
                    << partName << ". Exception was: " << e);
    
    return m_ignoreIfMissing ? StatusCode::SUCCESS : StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Successfully read " << isName << " from IS: " << value);

  return StatusCode::SUCCESS;  
}


const std::string& TrigISHelper::partition(ISObject obj) const
{
  return m_isid.find(obj)->second.first;
}


const std::string& TrigISHelper::isName(ISObject obj) const
{
  return m_isid.find(obj)->second.second;
}



//=========================================================================
// Helper to split ISObject property string
//=========================================================================
pair<string,string> TrigISHelper::split(const std::string& isid)
{
  size_t i = isid.find("/");
  if (i==string::npos) return make_pair(m_partitionName.value(), isid);

  return make_pair(isid.substr(0, i), isid.substr(i+1, isid.length()-i));
}
