/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArRawChannelBuilderToolTileInfo.h"
#include "LArROD/LArRawChannelBuilderDriver.h"
#include "GaudiKernel/MsgStream.h"
#include "CommissionEvent/ComTime.h"

#include "LArRawEvent/LArDigit.h"

// #include "LArRecUtils/LArOFPeakRecoTool.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"

#include <math.h>

using CLHEP::Hep3Vector;
using CLHEP::c_light;
using CLHEP::nanosecond;
using CLHEP::picosecond;

#define MAXINT 2147483000
#define MAXINT2 -2147483000


LArRawChannelBuilderToolTileInfo::LArRawChannelBuilderToolTileInfo(const std::string& type,
									   const std::string& name,
									   const IInterface* parent) : 
  LArRawChannelBuilderToolBase(type,name,parent),
  m_storeGate(0),
  m_detStore(0),
  m_man(0),
  m_peakReco("LArOFPeakRecoTool"),
  m_delayTile(0)
{
  m_helper = new LArRawChannelBuilderStatistics( 3,      // number of possible errors
                                               0x06);  // bit pattern special for this tool,
                                                       // to be stored in  "uint16_t provenance"
  m_helper->setErrorString(0, "no errors");
  m_helper->setErrorString(1, "channel saturated");
  m_helper->setErrorString(2, "OFC not valid");

  declareProperty("ComTimeKey",m_comTimeKey="ComTimeTileMuon");
  declareProperty("GlobalTimeOffsetTop", m_globaltimeoffsettop=-23.7);
  declareProperty("GlobalTimeOffsetBottom", m_globaltimeoffsetbottom=-19.2);
  declareProperty("defaultPhase", m_defaultPhase=12);
  declareProperty("ADCMax", m_AdcMax=4095);
  declareProperty("Skip", m_skipSaturatedCells=true);
  declareProperty("ECut", m_Ecut=50);
  declareProperty("doQual", m_doQual=true);
  declareProperty("minSample", m_minSample=0);
  declareProperty("maxSample", m_maxSample=31);
}

StatusCode LArRawChannelBuilderToolTileInfo::initTool()
{
  MsgStream log(msgSvc(), name());
  
  StatusCode sc=m_peakReco.retrieve();
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Unable to retrieve LArOFPeakRecoTool" <<endmsg;
    return StatusCode::FAILURE;
  }
  
  log << MSG::INFO << " DefaultPhase  "<<m_defaultPhase <<endmsg;
  
  sc = service("StoreGateSvc", m_storeGate);
  if ( sc.isFailure() ) {
    log << MSG::ERROR
	<< "Unable to get pointer to StoreGate Service" << endmsg;
    return sc;
  }
  
  sc = service("DetectorStore", m_detStore);
  if ( sc.isFailure() ) {
    log << MSG::ERROR
        << "Unable to get pointer to DetectorStore Service" << endmsg;
    return sc;
  }

  //retrieve CaloDetDescrManager from det store

  sc = m_detStore->retrieve(m_man);
  if (sc.isFailure()) {
    log << MSG::ERROR
        << "Unable to retrieve CaloDetDescrManager from DetectorStore" << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}


bool LArRawChannelBuilderToolTileInfo::buildRawChannel(const LArDigit* digit,
							   float pedestal,
							   const std::vector<float>& ramps,
						       MsgStream* pLog)
{

  HWIdentifier chid=m_parent->curr_chid;
  if(bool(pLog))
    (*pLog) << MSG::DEBUG << "Start " <<MSG::hex<< chid<<MSG::dec<< endmsg;
  CaloGain::CaloGain gain=m_parent->curr_gain;
  
  if ( m_parent->curr_maximum > m_AdcMax )
    {
      if(bool(pLog))
	(*pLog) << MSG::DEBUG << "Saturation on channel 0x"
	     << MSG::hex << chid.get_compact() << MSG::dec << ". ";
      if ( m_skipSaturatedCells )
	{
	  if(bool(pLog))
	    (*pLog) << "Skipping channel." << endmsg; 
	  m_helper->incrementErrorCount(1);
	  return false;
	}
      if(bool(pLog))
	(*pLog) << endmsg;
    }

  const std::vector < short >& samples = digit->samples();
  unsigned int sampsize = (unsigned int) samples.size();
  //float peakval = -999.;
  unsigned int ipeak = 0;  
  std::vector<float> signal ; 
  float currval = 0.;
  for (unsigned int ii = 0; ii < sampsize; ii++) {
  	  currval = (float)(samples[ii] - pedestal);
  	  signal.push_back(currval);
  	  //if ((ii >= m_minSample)&&(ii <= m_maxSample)&&(currval > peakval)) { ipeak = ii; peakval = currval; }
  }
  
  ipeak = m_parent->curr_shiftTimeSamples + 2;
  m_parent->curr_Phase =  m_defaultPhase;
  double globaltimeoffset = -25;

  //Retrieve TileMuonFitter ComTime object
  const ComTime* comTime;
    StatusCode sc = m_storeGate->retrieve(comTime, m_comTimeKey);
 
  if (sc.isFailure()) {
    if(bool(pLog))
      (*pLog) << MSG::ERROR
	      << "Unable to retrieve ComTime from StoreGate" << endmsg;
    return static_cast<bool>(sc);
  }
  
  else {//TileComTime exists
    
    Identifier id=currentID();
    const CaloDetDescrElement* caloDDE = m_man->get_element(id);

    double tileComTime = comTime->getTime();
    Hep3Vector muonpos = comTime->GetCounterPosition();
    Hep3Vector muondir = comTime->GetcosThetaDirection();

    if(tileComTime!=0 || muonpos.x()!=0 || muonpos.z()!=0) {
        
      double timeTOF = 0.; //For time of flight correction from TileComTime Y=0 position to LAr cell position
      double IPTOF = 0.; //For time of flight correction to Interaction Point
      globaltimeoffset = 0.; //Determined from cosmic data (LArTime-TileComTime) 
    
      if (caloDDE != NULL) {

	double xpos=caloDDE->x();
	double ypos=caloDDE->y();
	double zpos=caloDDE->z();

        const double inv_c_light = 1. / c_light;
	timeTOF = (sqrt((muonpos.x()-xpos)*(muonpos.x()-xpos) + (muonpos.y()-ypos)*(muonpos.y()-ypos) + (muonpos.z()-zpos)*(muonpos.z()-zpos))) * inv_c_light;
	IPTOF = (sqrt(xpos*xpos+ypos*ypos+zpos*zpos)) * inv_c_light;
	IPTOF = (-1)*IPTOF;
      
	if (ypos>0) {//top
	  globaltimeoffset = m_globaltimeoffsettop;
	}
	else {//bottom
	  globaltimeoffset = m_globaltimeoffsetbottom;
	}

	if ((ypos>0 && muondir.y()<0) || (ypos<0 && muondir.y()>0)) timeTOF=timeTOF*(-1); //Correct TOF for downward going muons at top and upward going muons at bottom

      }

      double larTimeOffset = tileComTime + timeTOF + IPTOF + globaltimeoffset; //Predicted time at LAr cell from all time offsets and corrections
      //peakSampleTile = int(((larTimeOffset*24./25. - 25.)/24. + 5) + 0.5) ; 
      //m_delayTile = int((24*(peakSampleTile-3) - larTimeOffset*24./25. + 25.) + 0.5) ;

      // adapt to current OFC timing, such that delay is between 0 ns and 25 ns
      // assumes that larTimeOffset = 0 corresponds to peak exactly at the third sample (ipeak==2 and delay=0 ns)
      //  (delay = 25 means that the peak is 1 sample before the sample used to center OFC)
      double peakSampleTile = int( (larTimeOffset*(1./25.)) + 2.);
      m_delayTile = 25.*(peakSampleTile-2) - larTimeOffset;
      // the following logic is suited to the regular case where OFC phases are between 0 and 25 ns
      //   for dedicated OFC sets where phase<0 are produced this is not optimal
      if (m_delayTile<0) {
        peakSampleTile += 1;
        m_delayTile += 25.;
      }

      m_parent->curr_shiftTimeSamples = peakSampleTile;
      m_parent->curr_Phase = static_cast<int> (m_delayTile);
    }
    ipeak = m_parent->curr_shiftTimeSamples;
  }

   
  float delay = m_delayTile;

  if (ipeak < 2) ipeak = 2; 
  if (ipeak > sampsize - 3) ipeak = sampsize - 3 ; 

  LArOFPeakRecoTool::Result results ; 

  unsigned int peak_min = ipeak - 1 ; 
  if (peak_min < 2) peak_min = 2; 

  unsigned int peak_max = ipeak + 1 ; 
  if (peak_max > sampsize - 3) peak_max = sampsize - 3 ;  

  float ADCPeak=0;
  float time=0.;
  results = m_peakReco->peak(signal, chid, gain, delay, 0, ipeak, peak_min, peak_max);

  if (results.getValid()) {
    ADCPeak = results.getAmplitude();
    // FIXME: this time definition still misses the tstart from the OFC to be absolutely computed
    time = (25.*((int)(results.getPeakSample_final())-2-1)-(results.getDelay_final()-results.getTau()-25));
  }
  else {
    if(bool(pLog))
      (*pLog) << MSG::DEBUG << ". OFC not valid for channel 0x"
	      << MSG::hex << chid.get_compact() << MSG::dec 
	      << " Gain = " << gain << ". Skipping channel." << endmsg;
    m_helper->incrementErrorCount(2);
    return false;
  }

  float power=1;
  float energy=0;
  for( unsigned int i=0; i<ramps.size(); i++)
    {
      energy += ramps[i] * power;
      power  *= ADCPeak;
    }

  //(*pLog) << MSG::DEBUG << "ADCPeak = " << ADCPeak <<", time = "<< time<<
  //	" energy "<<energy <<endmsg;

  uint16_t iquality=0;
  uint16_t iprovenance=0;
  if ((m_doQual) && (energy>m_Ecut) && (results.getValid())) {
    iquality = ((int)(results.getQuality())) & 0xFFFF;
    iprovenance = iprovenance | 0x2000;
  }

  iprovenance |= m_parent->qualityBitPattern;
  iprovenance |= m_helper->returnBitPattern();
  iprovenance = iprovenance & 0x3FFF;
  
  time=time*(nanosecond/picosecond); //Convert time to ps

  if (time>MAXINT) time=MAXINT;
  if (time<MAXINT2) time=MAXINT2;

  if (energy>MAXINT) energy=MAXINT;
  if (energy<MAXINT2) energy=MAXINT2;
  
  //Make LArRawChannel Object with new data
  LArRawChannel larRawChannel(digit->channelID(),
			      (int)floor(energy+0.5),
			      int(time),
			      iquality,iprovenance,digit->gain());   
  m_larRawChannelContainer->add(larRawChannel);
  
  m_helper->incrementErrorCount(0);
  
  return true;
}
  
