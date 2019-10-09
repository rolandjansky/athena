/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CscRDO_Decoder.h"
#include "GaudiKernel/ListItem.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonRDO/CscRawData.h"

/** constructor */ 
Muon::CscRDO_Decoder::CscRDO_Decoder
( const std::string& type, const std::string& name,const IInterface* parent )
  :  base_class(type,name,parent),
     m_cabling( "CSCcablingSvc" ,name),
     m_cscCalibTool( "CscCalibTool") {
  
  declareProperty("CscCalibTool",        m_cscCalibTool );
}

StatusCode Muon::CscRDO_Decoder::initialize()
{
  
  ATH_MSG_DEBUG ( "CscRDO_Decoder::initialize"); 
  
  ATH_CHECK( m_muonIdHelperTool.retrieve() );

  // get the cabling service
  if ( m_cabling.retrieve().isFailure() )  {
    ATH_MSG_ERROR ( " Cannot get CablingSvc " );
    return StatusCode::FAILURE;
  }
  // get cscCalibTool
  if (m_cscCalibTool.retrieve().isFailure()){
    ATH_MSG_ERROR ( "Can't get handle on CSC calibration tools" );
    return StatusCode::FAILURE;
  } 

  
  
  ATH_MSG_DEBUG (" Parameters are from CscCalibTool ") ;

  m_samplingTime = m_cscCalibTool->getSamplingTime();
  m_signalWidth  = m_cscCalibTool->getSignalWidth();
  m_timeOffset   = m_cscCalibTool->getTimeOffset();

  ATH_MSG_DEBUG (" Initialization is done!");
  
  /** initialize CSC Id Helper :: it is needed now! */
  m_rodReadOut.set(m_muonIdHelperTool.get());
  m_rodReadOut.setChamberBitVaue(1);

  
  return StatusCode::SUCCESS;
}


void Muon::CscRDO_Decoder::getDigit(const CscRawData * rawData, 
                                    Identifier& moduleId, Identifier& channelId, 
                                    double& adc, double& time)  {

  // get the raw data
  uint32_t address = rawData->address();

  // initialize some parameters
  m_rodReadOut.setParams(m_timeOffset, m_samplingTime, m_signalWidth);

  adc = m_rodReadOut.findCharge( rawData->samples(), time);

  // now decode the endcoded fragments 
  // find the Identifier and charge
  m_rodReadOut.setAddress(address);
  moduleId   = m_rodReadOut.decodeAddress();
  channelId  = m_rodReadOut.decodeAddress(moduleId);
}



Identifier Muon::CscRDO_Decoder::stationIdentifier(const CscRawData * rawData) 
{
  /** get the raw data */
  uint32_t address = rawData->address();

  // initialize some parameters
  m_rodReadOut.setParams(m_timeOffset, m_samplingTime, m_signalWidth);

  /** now decode the endcoded fragments find the Identifiers */
  m_rodReadOut.setAddress(address);
  return m_rodReadOut.decodeAddress();
}

Identifier Muon::CscRDO_Decoder::channelIdentifier(const CscRawData * rawData, int j) 
{
  /** get the raw data */
  uint32_t address = rawData->address();

  // initialize some parameters
  m_rodReadOut.setParams(m_timeOffset, m_samplingTime, m_signalWidth);

  /** now decode the endcoded fragments find the Identifiers */
  m_rodReadOut.setAddress(address);
  Identifier moduleId   = m_rodReadOut.decodeAddress();
  
  ATH_MSG_DEBUG ( " CscRDO_Decoder OUTPUT ::: "
                  << m_timeOffset << "  " << m_samplingTime << " " << m_signalWidth << " "
                  << "  " << m_detdescr << "  " << address << "   "
                  << moduleId << " " << j );

  return m_rodReadOut.decodeAddress(moduleId, j);
}

