/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcRecChannelTool.cxx
 *
 *  Created on: Nov 24, 2009
 *      Author: leite
 */


#include <iostream>
#include <fstream>

#include <vector>
#include <map>
#include <math.h>

#include "TMath.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"
#include "xAODForward/ZdcModuleToString.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "xAODForward/ZdcModuleAuxContainer.h"
#include "ZdcRec/ZdcRecChannelToolV2.h"
#include "ZdcRec/ZdcSignalSinc.h"
#include "ZdcByteStream/ZdcToString.h"


//#include "ZdcIdentifier/ZdcID.h"
#include "ZdcIdentifier/ZdcID.h"
#include "ZdcConditions/ZdcCablingService.h"

const int      slink2ppmChannel[64] =
  {0,  4,  8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,  60,
   3,  7, 11,  15,  19,  23,  27,  31,  35,  39,  43,  47,  51,  55,  59,  63,
   1,  5,  9,  13,  17,  21,  25,  29,  33,  37,  41,  45,  49,  53,  57,  61,
   2,  6, 10,  14,  18,  22,  26,  30,  34,  38,  42,  46,  50,  54,  58,  62 };


//==================================================================================================
ZdcRecChannelToolV2::ZdcRecChannelToolV2(const std::string& name):
  AsgTool(name),
  m_nsamples(7),
  m_sample_time(25.)
{
	//Declare properties here...

	declareInterface<ZdcRecChannelToolV2>(this);
	declareProperty("ZeroSuppress", m_zeroSupress = 0,"Supress channels with only 0");
	declareProperty("DeltaPeak", m_delta = 5,"Minimum difference between min and max to be considered a signal");
	declareProperty("SaturationADC",m_saturation = 1000,"ADC value above which a HG channel is considered saturated");
}
//==================================================================================================

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

void ZdcRecChannelToolV2::handle( const Incident& inc )
{
  if ( inc.type() == IncidentType::EndEvent) {

  }
}


//==================================================================================================
StatusCode ZdcRecChannelToolV2::initialize()
{
	msg(MSG::INFO) << "Initializing " << name() << endreq;

	//Get the pedestal information for the channels.
	//For now, this is a file; later on it will be moved to a database

	// NONE
	
	const ZdcID* zdcId = 0;
	if (detStore()->retrieve( zdcId ).isFailure() ) {
	    msg(MSG::ERROR) << "execute: Could not retrieve ZdcID object from the detector store" << endreq;
	    return StatusCode::FAILURE;
	}
	else {
	    msg(MSG::DEBUG) << "execute: retrieved ZdcID" << endreq;
	}
	m_zdcId = zdcId;

	ZdcCablingService::getInstance()->setZdcID(m_zdcId);

	msg(MSG::DEBUG) << "--> ZDC : END OF MODIFICATION 0" << endreq ;
	return StatusCode::SUCCESS;

	ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", name());
	CHECK(incidentSvc.retrieve());
	incidentSvc->addListener(this, IncidentType::EndEvent);
}
//==================================================================================================

//==================================================================================================
StatusCode ZdcRecChannelToolV2::finalize()
{
	msg(MSG::INFO) << "Finalizing " << name() << endreq;

	return StatusCode::SUCCESS;
}
//==================================================================================================

int ZdcRecChannelToolV2::convertTT2ZM(const xAOD::TriggerTowerContainer* ttCollection, xAOD::ZdcModuleContainer* zdcModules )
{

  //typedef std::map<uint64_t,xAOD::ZdcModule*> hashmapType;
  typedef std::map<uint32_t,xAOD::ZdcModule*> hashmapType;
  hashmapType digits_map;
  Identifier chan_id;
  
  xAOD::TriggerTowerContainer::const_iterator tt_itr = ttCollection->begin();
  xAOD::TriggerTowerContainer::const_iterator tt_end = ttCollection->end();

  //std::cout << "Zdc TT's have " << ttCollection->size() << " towers" << std::endl;

  for (;tt_itr != tt_end;tt_itr++)
    {
      //std::cout << "ZdcTT coolId = " << (*tt_itr)->coolId() << std::endl;
      //std::cout << ZdcToString(*(*tt_itr)) << std::endl;

      uint32_t coolId = (*tt_itr)->coolId();
      uint32_t pin = (coolId>>8) & 0xf;
      uint32_t asic = coolId & 0xf;
      uint32_t slinkChannel = asic*16 + pin;
      uint32_t ppmChannel = slink2ppmChannel[slinkChannel];

      uint32_t module = (coolId>>16) & 0xf;

      ATH_MSG_DEBUG( "--> ZCS: " << ZdcCablingService::getInstance() << " mod=" << module << " slinkC=" << slinkChannel << " ppmC=" << ppmChannel );

      chan_id = ZdcCablingService::getInstance()->h2s_channel_id(module, ppmChannel);

      const uint32_t chan_hash = chan_id.get_identifier32().get_compact();
      //const uint64_t chan_hash = chan_id.get_identifier32().get_compact();

      // std::cout << "Trying to get gain_delay" << std::endl;

      int gain  = ZdcCablingService::getInstance()->hwid2gain(module,ppmChannel);
      int delay = ZdcCablingService::getInstance()->hwid2delay(module,ppmChannel);
      
      ATH_MSG_DEBUG("Trying to find chan_hash " << chan_hash << " g" << gain << "d" << delay);

      //std::cout << "Trying to make map" << std::endl;

      hashmapType::iterator iter = digits_map.find(chan_hash);
      if (iter == digits_map.end())
	{
	  ATH_MSG_DEBUG("new channel for " << chan_id);
	  xAOD::ZdcModule* new_mod = new xAOD::ZdcModule();
	  zdcModules->push_back(new_mod);
	  digits_map.insert(std::pair<uint32_t,xAOD::ZdcModule*>(chan_hash,new_mod));
	  iter = digits_map.find(chan_hash);
	}
      if (iter != digits_map.end())
	{
	  ATH_MSG_DEBUG("new elementlinks for " << chan_id);
	  //std::cout << "Trying to set properties of " << chan_id << std::endl;
	  (*iter).second->setId(chan_hash);
	  //std::cout << "Set id=" << std::hex << chan_hash << std::endl;
	  (*iter).second->setSide(m_zdcId->side(chan_id));
	  (*iter).second->setZdcModule(m_zdcId->module(chan_id));
	  (*iter).second->setType(m_zdcId->type(chan_id));
	  (*iter).second->setChannel(m_zdcId->channel(chan_id));
	  //std::cout << "Trying to set element links of " << chan_id << std::dec << std::endl;
	  if (gain==0&&delay==0) (*iter).second->setTTg0d0Link( ElementLink<xAOD::TriggerTowerContainer_v2> ((*tt_itr),*ttCollection ) );
	  if (gain==0&&delay==1) (*iter).second->setTTg0d1Link( ElementLink<xAOD::TriggerTowerContainer_v2> ((*tt_itr),*ttCollection ) );
	  if (gain==1&&delay==0) (*iter).second->setTTg1d0Link( ElementLink<xAOD::TriggerTowerContainer_v2> ((*tt_itr),*ttCollection ) );
	  if (gain==1&&delay==1) (*iter).second->setTTg1d1Link( ElementLink<xAOD::TriggerTowerContainer_v2> ((*tt_itr),*ttCollection ) );
	}
      
    }

  //std::unique_ptr< xAOD::ZdcModuleContainer > zdcModules( new xAOD::ZdcModuleContainer() );
  //std::unique_ptr< xAOD::ZdcModuleAuxContainer > zdcModulesAux( new xAOD::ZdcModuleAuxContainer() );
  //zdcModules->setStore( zdcModulesAux.get() );

  hashmapType::iterator iter = digits_map.begin();
  hashmapType::iterator iter_end = digits_map.end();

  for (;iter != iter_end;iter++)
    {
      makeWaveformFromDigits( *((*iter).second) );
    }

  return 0;


}

int ZdcRecChannelToolV2::makeWaveformFromDigits(xAOD::ZdcModule& module)
{
  
  const size_t nsamp = 5;
  const float gain = 10;
  std::map<int,float> waveform; // using int so i can do direct tests on whether a time exists.

  std::map<int,float> waveformG0; // using int so i can do direct tests on whether a time exists.
  std::map<int,float> waveformG1; // using int so i can do direct tests on whether a time exists.

  float amp=0;
  float ampL=0; // low gain
  float rawamp=0;
  float rawampL=0; // low gain
  
  for (size_t isamp = 0;isamp<nsamp;isamp++)
    {

      if ( module.TTg1d0Link().isValid() )
	{
	  if ( (*module.TTg1d0Link())->adc().size())
	    {
	      rawamp =  (*module.TTg1d0Link())->adc().at(isamp);
	      amp =  rawamp - (*module.TTg1d0Link())->adc().at(0);
	      waveform[isamp*250] = amp;
	      waveformG1[isamp*250] = amp;
	    }
	}
      
      if ( module.TTg0d0Link().isValid() )
	{
	  if ( (*module.TTg0d0Link())->adc().size())
	    {
	      rawampL = (*module.TTg0d0Link())->adc().at(isamp);
	      ampL = rawampL - (*module.TTg0d0Link())->adc().at(0);
	      waveformG0[isamp*250] = ampL;
	      if (rawamp > m_saturation) // i.e. if high gain defined for D0 saturates
		{
		  waveform[isamp*250] = ampL * gain;
		}
	    }
	}

      if ( module.TTg1d1Link().isValid() )
	{
	  if ( (*module.TTg1d1Link())->adc().size())
	    {
	      rawamp =  (*module.TTg1d1Link())->adc().at(isamp);
	      amp =  rawamp - (*module.TTg1d1Link())->adc().at(0);
	      waveform[isamp*250-125] = amp;
	      waveformG1[isamp*250-125] = amp;
	    }
	}

      if ( module.TTg0d1Link().isValid() )
	{
	  if ((*module.TTg0d1Link())->adc().size())
	    {
	      rawampL = (*module.TTg0d1Link())->adc().at(isamp);
	      ampL = rawampL - (*module.TTg0d1Link())->adc().at(0);
	      waveformG0[isamp*250-125] = ampL;
	      if (rawamp > m_saturation) // i.e. if high gain defined for D1 saturates
		{
		  waveform[isamp*250-125] = ampL * gain;
		}
	    }
	}
    }

  // copy constructed waveform into module for subsequent processing
  // also add summary information on amplitude, time, qual
  std::vector<float> times;
  std::vector<float> adcs;
  float peak_amp=0, peak_time=0, peak_qual=0;

  splitWaveform(waveform,times,adcs);
  getPeakProperties(times,adcs,peak_time,peak_amp,peak_qual);
  // copy them to the module
  module.setWaveformTime(times);
  module.setWaveformADC(adcs);
  module.setAmplitude(peak_amp);
  module.setTime(peak_time);
  module.setQual(peak_qual);

  splitWaveform(waveformG0,times,adcs);
  getPeakProperties(times,adcs,peak_time,peak_amp,peak_qual);
  // copy them to the module
  module.setWaveformTimeG0(times);
  module.setWaveformADCG0(adcs);
  module.setAmplitudeG0(peak_amp);
  module.setTimeG0(peak_time);
  module.setQualG0(peak_qual);

  splitWaveform(waveformG1,times,adcs);
  getPeakProperties(times,adcs,peak_time,peak_amp,peak_qual);
  // copy them to the module
  module.setWaveformTimeG1(times);
  module.setWaveformADCG1(adcs);
  module.setAmplitudeG1(peak_amp);
  module.setTimeG1(peak_time);
  module.setQualG1(peak_qual);

  return 0;
}

//Returns a pointer to a RawChannel Collection with energy and time
//==================================================================================================
int  ZdcRecChannelToolV2::makeRawFromDigits(xAOD::ZdcModuleContainer&  m_ChannelCollection)
{

	Identifier id;

	
	msg(MSG::DEBUG) << "--> ZDC : ZdcRecChannelToolV2 m_ChannelCollection size " << m_ChannelCollection.size() << endreq ;
	return 0;
}

int ZdcRecChannelToolV2::splitWaveform(std::map<int,float>& waveform, std::vector<float>& times, std::vector<float>& adcs)
{
  times.clear();
  adcs.clear();
  std::map<int,float>::const_iterator wf_it = waveform.begin();
  std::map<int,float>::const_iterator wf_end = waveform.end();
  for (;wf_it != wf_end;wf_it++)
    {
      //std::cout << iwf++ << "t=" << (*wf_it).first << " a=" << (*wf_it).second << std::endl;
      times.push_back((*wf_it).first / 10. ); // convert back to ns
      adcs.push_back((*wf_it).second );
    }
  return 1;
}

int ZdcRecChannelToolV2::getPeakProperties(std::vector<float>& times, std::vector<float>& adcs, float& time, float& amp, float& qual)
{

  if (times.size() != adcs.size()) 
    {
      msg(MSG::WARNING) << "times and ADCs inconsistent!" << endreq;
      return 0;
    }

  float max_adc = -999;
  float max_time = -999;
  float max_qual = 0;

  for (size_t i = 0;i<times.size();i++)
    {
      if (adcs.at(i) > max_adc)
	{
	  max_adc = adcs.at(i);
	  max_time = times.at(i);
	  max_qual = 1.;
	}
    }
  
  time = max_time;
  amp = max_adc;
  qual = max_qual;

  return 1;
}

