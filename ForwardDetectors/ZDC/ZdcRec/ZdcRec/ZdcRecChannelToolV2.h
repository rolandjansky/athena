/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZdcRecChannelToolV2.h
 *
 * Two steps:
 * 1. Convert the towers to ZdcModules
 * 2. Process the ZdcModules and add Energy/time info
 * Created on: September 27, 2015
 *      Author: steinber
 */

#ifndef ZDCRECCHANNELTOOLV2_H_
#define ZDCRECCHANNELTOOLV2_H_


#include <string>
#include <map>

//#include "AthenaBaseComps/AthAlgTool.h"
#include "AsgTools/AsgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

#include "xAODForward/ZdcModuleContainer.h"
#include "xAODForward/ZdcModuleAuxContainer.h"

class StatusCode;
class Identifier;
class ZdcID ;

class ZdcRecChannelToolV2: public asg::AsgTool, virtual public IIncidentListener 
{
  ASG_TOOL_INTERFACE(ZdcRecChannelToolV2)
  ASG_TOOL_CLASS0(ZdcRecChannelToolV2)
 public:
  ZdcRecChannelToolV2(const std::string& name);
  virtual ~ZdcRecChannelToolV2() {};
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual void handle( const Incident& ) override;

  int convertTT2ZM(const xAOD::TriggerTowerContainer* ttCollection, xAOD::ZdcModuleContainer* zdcModules) const;
  int makeRawFromDigits(xAOD::ZdcModuleContainer& zdcModules) const; // NOT const -- we're going to modify the objects to add signal processing
  int makeWaveformFromDigits(xAOD::ZdcModule& module) const;
  int splitWaveform(std::map<int,float>& waveform, std::vector<float>& times, std::vector<float>& adcs) const;
  int getPeakProperties(std::vector<float>& times, std::vector<float>& adcs, float& time, float& amp, float& qual, float& presamp) const;

private:

  // internal parameters, set in pythion
  unsigned int m_nsamples;
  float        m_sample_time;
  float        m_gainFactor;
  bool         m_useDelay;
  int          m_delta; // A number to check for signal
  int          m_zeroSupress;
  int          m_saturation; // saturation level (absolute ADC)
  float        m_pedestalValue; // pedestal value (set to 100)

  static int s_ppmChannel;
  const ZdcID* m_zdcId;

};


#endif /* ZDCRECCHANNELTOOLV2_H_ */
