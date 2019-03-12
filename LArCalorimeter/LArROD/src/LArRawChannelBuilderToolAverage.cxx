/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderToolAverage.h"
#include "GaudiKernel/MsgStream.h"

#include "LArRawEvent/LArDigit.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnlineID.h"
#include "Identifier/Identifier.h"
#include <math.h>

using CLHEP::nanosecond;
using CLHEP::picosecond;

#define MAXINT 2147483000
#define MAXINT2 -2147483000



LArRawChannelBuilderToolAverage::LArRawChannelBuilderToolAverage(const std::string& type,
								 const std::string& name,
								 const IInterface* parent):
  LArRawChannelBuilderToolBase(type,name,parent),
  m_emId(NULL),
  m_fcalId(NULL),
  m_hecId(NULL),
  m_onlineHelper(NULL)
{
  m_helper = new LArRawChannelBuilderStatistics( 1,      // number of possible errors
					       0x01);  // bit pattern special for this tool,
                                                       // to be stored in  "uint16_t provenance"
  m_helper->setErrorString(0, "no errors");
  
  declareProperty("AverageSamplesEM",m_averageSamplesEM=5); 
  declareProperty("AverageSamplesHEC",m_averageSamplesHEC=5); 
  declareProperty("AverageSamplesFCAL",m_averageSamplesFCAL=3); 
  declareProperty("AverageScaleEM",m_averageScaleEM=2.6); 
  declareProperty("AverageScaleHEC",m_averageScaleHEC=2.6); 
  declareProperty("AverageScaleFCAL",m_averageScaleFCAL=1.8); 

  declareProperty("NScan", m_nScan=32); 
}

StatusCode LArRawChannelBuilderToolAverage::initTool()
{
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_emId=idHelper->em_idHelper();
  m_fcalId=idHelper->fcal_idHelper();
  m_hecId=idHelper->hec_idHelper();
  

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  return StatusCode::SUCCESS;
}

bool LArRawChannelBuilderToolAverage::buildRawChannel(const LArDigit* digit,
						      float pedestal,
						      const std::vector<float>& ramps,
						      MsgStream* /* plog */ )
{
  HWIdentifier chid = m_parent->curr_chid; 
  //int nMin = 0;
  unsigned int nAverage = 1;
  float myScale = 1;
  
  if (m_onlineHelper->isEMBchannel(chid) ||m_onlineHelper->isEMECchannel(chid)) {
    nAverage = m_averageSamplesEM;
    myScale = m_averageScaleEM;
  }
  else if (m_onlineHelper->isFCALchannel(chid) ) {
    nAverage = m_averageSamplesFCAL;
    myScale = m_averageScaleFCAL;
  }
  else if (m_onlineHelper->isHECchannel(chid) ) {
    nAverage = m_averageSamplesHEC;
    myScale = m_averageScaleHEC;
  }
  
  // look for maximum of all possible windows of size nAverage
  long sum=0;
  unsigned nSummedSamples=0;
  for( unsigned int i=m_parent->curr_shiftTimeSamples;
       i<(m_parent->curr_shiftTimeSamples+nAverage) &&
	 i<m_parent->curr_nsamples; i++,nSummedSamples++ )
    sum += digit->samples()[i];
  
  long max=sum;
  unsigned int maxsample=m_parent->curr_shiftTimeSamples;

  unsigned int nmax = m_parent->curr_shiftTimeSamples+1+m_nScan;
  if(nmax>m_parent->curr_nsamples-nAverage+1)
	nmax = m_parent->curr_nsamples-nAverage+1 ; 

  for( unsigned int i=m_parent->curr_shiftTimeSamples+1;
       i<nmax; i++ )
    {
      sum += digit->samples()[i+nAverage-1] - digit->samples()[i-1];
      if(sum>max)
	{
	  max=sum;
	  maxsample=i;
	}
    }

  float ADCPeak =  (max - nSummedSamples*pedestal)/myScale;
  float power=1;
  float energy=0;
  for( unsigned int i=0; i<ramps.size(); i++)
    {
      energy += ramps[i] * power;
      power  *= ADCPeak;
    }
  
  // LArRawChannel has a non-standard unit of pico-second.
  int time=int((25.0 * nanosecond /picosecond) * maxsample );
  uint16_t iquality=0;
  
  // store which tool created this channel
  uint16_t iprovenance=0;
  iprovenance |= m_parent->qualityBitPattern;
  iprovenance |= m_helper->returnBitPattern();
  iprovenance = iprovenance & 0x3FFF;

  if (time>MAXINT) time=MAXINT;
  if (time<MAXINT2) time=MAXINT2;

  if (energy>MAXINT) energy=MAXINT;
  if (energy<MAXINT2) energy=MAXINT2;

  
  (this->*m_buildIt)((int)(floor(energy+0.5)),time,iquality,iprovenance,digit);  

  m_helper->incrementErrorCount(0);
  
  return true;
}

