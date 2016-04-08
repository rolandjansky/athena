/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArRawChannelBuilderToolBase
 * @author Rolf Seuster
 * @author Denis Damazio
 * @brief Base tool to make the interface with the driver
 * 
 * This tool acts differently in the offline and HLT contexts:
 * in the offline, the output is a LArRawChannel whereas in HLT, it is
 * directly a LArCell. The switch between the two modes is based on the
 * presence of a non-NULL LArRawChannelContainer pointer in the parent driver
 * 
 */

#ifndef LARROD_LARRAWCHANNELBUILDERTOOLBASE_H
#define LARROD_LARRAWCHANNELBUILDERTOOLBASE_H

#include "GaudiKernel/AlgTool.h"

#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArROD/LArRawChannelBuilderToolBaseClass.h"
#include "LArROD/ILArRawChannelBuilderToolBase.h"
#include "LArRawEvent/LArDigit.h"

class LArRawChannelContainer;

class LArRawChannelBuilderToolBase : public LArRawChannelBuilderToolBaseClass,
				     public virtual ILArRawChannelBuilderToolBase
{
 public:
  
  LArRawChannelBuilderToolBase(const std::string& type,
			       const std::string& name,
			       const IInterface* parent);
  void initEventHidden()
    { m_larRawChannelContainer=m_parent->m_larRawChannelContainer;
      if ( m_larRawChannelContainer !=NULL )
        m_buildIt = &LArRawChannelBuilderToolBase::BuildLArRawChannel;
      else
        m_buildIt = &LArRawChannelBuilderToolBase::BuildLArCell;
      initEvent(); }
  int energy() { return m_energy; }
  int time() { return m_time; }
  CaloGain::CaloGain gain() { return m_gain; }
  
  virtual StatusCode initTool()
    { return StatusCode::SUCCESS; };
  
  virtual bool buildRawChannel(const LArDigit* digit,
			       float pedestal,
			       const std::vector<float>& ramps,
			       MsgStream* pLog ) = 0;
  void (LArRawChannelBuilderToolBase::*m_buildIt)(int energy, int time, uint16_t quality, uint16_t provenance, const LArDigit* digit);

  void BuildLArCell(int energy, int time, uint16_t quality, uint16_t provenance, const LArDigit* digit);
  void BuildLArRawChannel(int energy, int time, uint16_t quality, uint16_t provenance, const LArDigit* digit);
  
 protected:
  
  LArRawChannelContainer     *m_larRawChannelContainer;
  int m_energy;
  int m_time;
  CaloGain::CaloGain m_gain;
  
};

inline void
LArRawChannelBuilderToolBase::BuildLArCell(int energy, int time, uint16_t /*iquality*/, uint16_t /*provenance*/, const LArDigit* digit){
        m_energy = energy;
        m_time   = time;
        m_gain   = digit->gain();
}

inline void
LArRawChannelBuilderToolBase::BuildLArRawChannel(int energy, int time, uint16_t quality, uint16_t provenance, const LArDigit* digit){
        // It is assumed that m_larRawChannelContainer exists
        LArRawChannel larRawChannel(digit->channelID(),
				    energy,time,
				    quality,provenance,digit->gain());
        m_larRawChannelContainer->add(larRawChannel);
}

#endif

