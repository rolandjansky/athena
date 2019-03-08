/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "LArROD/LArRawChannelBuilderToolCubic.h"
#include "GaudiKernel/MsgStream.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "Identifier/Identifier.h"
#include "LArRawEvent/LArDigit.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include <math.h>

using CLHEP::ns;
using CLHEP::nanosecond;
using CLHEP::picosecond;

#define MAXINT 2147483000
#define MAXINT2 -2147483000


LArRawChannelBuilderToolCubic::LArRawChannelBuilderToolCubic(const std::string& type,
							     const std::string& name,
							     const IInterface* parent):
  LArRawChannelBuilderToolBase(type,name,parent),
  m_fcalId(NULL)
{
  m_helper = new LArRawChannelBuilderStatistics( 3,      // number of possible errors
					       0x04);  // bit pattern special for this tool,
					               // to be stored in  "uint16_t provenance"
  m_helper->setErrorString(0, "no errors");
  m_helper->setErrorString(1, "Fit failed");
  m_helper->setErrorString(2, "is FCAL");

  declareProperty("UseMaxSample",      m_useMaxSample = false);
  declareProperty("minADCforCubic",             m_minADC=30);

}

StatusCode LArRawChannelBuilderToolCubic::initTool()
{
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_fcalId=idHelper->fcal_idHelper();
  
  return StatusCode::SUCCESS;
}

bool LArRawChannelBuilderToolCubic::buildRawChannel(const LArDigit* digit,
						    float pedestal,
						    const std::vector<float>& ramps,
						    MsgStream* pLog)
{
  float ADCPeak=0;
  
  bool CubicFailed=false;
  
  float time=0;
   
  // use fixed sample 
  unsigned int maxSample = m_parent->curr_shiftTimeSamples+2 ;

  // use max Sample 
  if(m_useMaxSample) 
    maxSample = m_parent->curr_maxsample ; 


  if( maxSample>= digit->samples().size() )
    return false; 

  // apply min ADC cut.
  if( (digit->samples()[maxSample]-pedestal) < m_minADC )
    return false; 

 
  // FCAL needs special treatment
  if(m_fcalId->is_lar_fcal(currentID()))
    {
      if(bool(pLog))
	(*pLog) << MSG::VERBOSE << "FCAL using special reconstuction !" << endmsg;
      
      unsigned int it0;
      
      const float invT[3][3] 
	= { {    1,       0,       0 },
	    { -1.5,       2,    -0.5 },
	    {  0.5,      -1,     0.5 }  };
      
      // peak slice very early
      if ( (maxSample) <= 1 )
	{
	  it0 = 1;
	} else if ( (maxSample) >= (m_parent->curr_nsamples) )
	  {
	    // peak is late
	    it0 = (m_parent->curr_nsamples) - 3;
	  } else {
	    // peak in safe region
	    it0 = (maxSample) - 1;
	  }
      
      // Quadratic interpolation using
      // 3 samples to be used start at 0 <= t0 <= nsamples-3
      float A[3] = {0, 0, 0};
      float dtmax = 0.0;
      // S = TA -> A = inv(T)S
      for (int ia = 0; ia < 3; ia++) 
	for (int it = 0; it < 3; it++) 
	  A[ia] += invT[ia][it] * (digit->samples()[it0+it] - (pedestal));
      
      // fit parameters
      if ( not ( CubicFailed = ( A[2] == 0 ) ) )   {
	dtmax = -1.0 * A[1] / 2.0 / A[2];
	if ( ! ( CubicFailed = ( dtmax < 0 || dtmax > 2 ) ) ) {
	  //time = (float(it0) + dtmax) * 25.0 * nanosecond; // nsec
	  time=dtmax*25.0*ns;
	  for(int ia = 0; ia < 3; ia++)
	    ADCPeak += A[ia] * pow(dtmax, ia);
	}
      }
    }else{
      
      unsigned int it0;
      const float invT[4][4] 
	= { {        1,       0,       0,        0},
	    { -1.83333,       3,    -1.5, 0.333333},
	    {        1,    -2.5,       2,     -0.5},
	    {-0.166666,     0.5,    -0.5, 0.166666}  };
      
      // peak slice very early
      if ( (maxSample) <= 1 )
	{
	  it0 = 1;
	} else if ( (maxSample) >= (m_parent->curr_nsamples) - 2 )
	  {  // peak is late
	    it0 = (m_parent->curr_nsamples) - 4;
	  } else { // peak in safe region
	    it0 = ( digit->samples()[(maxSample)-2] > digit->samples()[(maxSample)+2] )
	      ? (maxSample) - 2
	      : (maxSample) - 1;
	  }
      
      // 4 samples to be used start at 0 <= t0 <= nsamples-4
      float A[4] = {0, 0, 0, 0};
      float dtmax = 0.0;
      float disc;
      // S = TA -> A = inv(T)S
      for (int ia = 0; ia < 4; ia++) 
	for (int it = 0; it < 4; it++) 
	  A[ia] += invT[ia][it] * (digit->samples()[it0+it] - (pedestal));
      
      // fit parameters
      disc = A[2]*A[2] - 3*A[1]*A[3];
      if ( ! ( CubicFailed = ( disc < 0 || A[3] == 0 ) ) )   {
	dtmax = (-A[2]-sqrt(disc))/(A[3]*3);
	if ( ! ( CubicFailed = ( dtmax < 0 || dtmax > 3 ) ) ) {
	  time = (float(it0) + dtmax) * (25.0 * nanosecond/picosecond); // nsec
	  for(int ia = 0; ia < 4; ia++)
	    ADCPeak += A[ia] * pow(dtmax, ia);
	}
      }
      
      if(CubicFailed)
	{
	  m_helper->incrementErrorCount(1);
	  return false;
	}
    }
  
  float power=1;
  float energy=0;
  for( unsigned int i=0; i<ramps.size(); i++)
    {
      energy += ramps[i] * power;
      power  *= ADCPeak;
    }
  
  // store which tools created this channel
  uint16_t iquality=0;
  uint16_t iprovenance=0;

  iprovenance |= m_parent->qualityBitPattern;
  iprovenance |= m_helper->returnBitPattern();
  iprovenance = iprovenance & 0x3FFF;

  if (time>MAXINT) time=MAXINT;
  if (time<MAXINT2) time=MAXINT2;

  if (energy>MAXINT) energy=MAXINT;
  if (energy<MAXINT2) energy=MAXINT2;
  
  
  (this->*m_buildIt)((int)(floor(energy+0.5)),(int)floor(time+0.5),iquality,iprovenance,digit);
  m_helper->incrementErrorCount(0);
  
  return true;
}
