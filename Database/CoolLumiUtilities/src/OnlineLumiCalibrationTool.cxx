/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoolLumiUtilities/OnlineLumiCalibrationTool.h"

#include "CoolKernel/IObject.h"
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

OnlineLumiCalibrationTool::OnlineLumiCalibrationTool(const std::string& type,
			     const std::string& name,
			     const IInterface* parent) :
  AthAlgTool(type, name, parent), 
  m_calibrationFolderName("") // /TDAQ/OLC/CALIBRATIONS")
{
  declareInterface<IOnlineLumiCalibrationTool>(this);
  declareProperty("CalibrationFolderName", m_calibrationFolderName);
}

StatusCode
OnlineLumiCalibrationTool::initialize()
{
  ATH_MSG_DEBUG("OnlineLumiCalibrationTool::initialize() begin");

  if (m_calibrationFolderName.empty()) {
    ATH_MSG_INFO("CalibrationFolderName.empty() is TRUE, skipping...");
  } else {

    ATH_MSG_INFO("OnlineLumiCalibrationTool::initialize() registering " << m_calibrationFolderName);

    // Setup callback on COOL folder change
    // This must be done with the interface (IOnlineLumiCalibrationTool) so external code can use this to trigger
    // their own callbacks
    
    if (detStore()->contains<CondAttrListCollection>(m_calibrationFolderName)) {

      const DataHandle<CondAttrListCollection> aptr;

      CHECK(detStore()->regFcn(&IOnlineLumiCalibrationTool::updateCache, dynamic_cast<IOnlineLumiCalibrationTool*>(this) , aptr, m_calibrationFolderName));

      ATH_MSG_INFO( " Registered a callback for " << m_calibrationFolderName << " COOL folder " );
    } else {
      ATH_MSG_ERROR( " cannot find " << m_calibrationFolderName << " in DetectorStore" );
    }
  }

  ATH_MSG_DEBUG( "OnlineLumiCalibrationTool::initialize() end" );
  return StatusCode::SUCCESS;
}

// Accessor function
float
OnlineLumiCalibrationTool::getMuToLumi(unsigned int channel) const
{
  // Gymnastics to keep this function const.  Normal operator[] is not const...
  std::map<unsigned int, OnlineLumiCalibrator>::const_iterator p;
  if ((p = m_cali.find(channel)) == m_cali.end()) return -1.;

  return p->second.getMuToLumi();
}

bool
OnlineLumiCalibrationTool::calibrateLumi(unsigned int channel, float raw, float& lumi) const
{
  std::map<unsigned int, OnlineLumiCalibrator>::const_iterator p;
  if ((p = m_cali.find(channel)) == m_cali.end()) return false;

  return p->second.calibrateLumi(raw, lumi);
}

bool
OnlineLumiCalibrationTool::calibrateMu(unsigned int channel, float raw, float& mu) const
{
  std::map<unsigned int, OnlineLumiCalibrator>::const_iterator p;
  if ((p = m_cali.find(channel)) == m_cali.end()) return false;

  bool err = p->second.calibrateMu(raw, mu);

  ATH_MSG_DEBUG( " raw " << raw << " calibrated to mu = " << mu );
  return err;
}

// Callback function
StatusCode
OnlineLumiCalibrationTool::updateCache( IOVSVC_CALLBACK_ARGS_P(/*idx*/, /*keys*/) )
{
  
  ATH_MSG_DEBUG( "in updateCache()");

  // Clear old data
  m_cali.clear();

  if (m_calibrationFolderName.empty()) {
    ATH_MSG_WARNING("In updateCache, but CalibrationFolderName.empty() is TRUE!");
    return StatusCode::SUCCESS;
  }

  const CondAttrListCollection* attrListColl = 0;
  CHECK(detStore()->retrieve(attrListColl, m_calibrationFolderName));

  // Loop over collection and save all channels
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) {
    cool::UInt32 channel = (*first).first;
    const coral::AttributeList& attrList = (*first).second;
    OnlineLumiCalibrator lc;
    if (!lc.setCalibration(attrList)) {
      ATH_MSG_WARNING( "error processing calibration for channel " << channel );
    } else{
      ATH_MSG_DEBUG( "Calibration for channel " << channel << ": " << lc );
      m_cali[channel] = lc;
    }
  }


  return StatusCode::SUCCESS;
}

