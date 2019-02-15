/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HI_HITOWERWEIGHTTOOL_H
#define HI_HITOWERWEIGHTTOOL_H

#include "AsgTools/IAsgTool.h"
#include "AsgTools/AsgTool.h"
#include <map>

class TH3F;

class IHITowerWeightTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IHITowerWeightTool)
  
  public:

  virtual ~IHITowerWeightTool() {};
  virtual StatusCode initialize() = 0;
  virtual StatusCode configureEvent() = 0;
  virtual float getEtaPhiResponse(float, float) const = 0;
  virtual float getEtaPhiOffset(float, float) const = 0;
  virtual float getWeight(float, float, int) const = 0;
  virtual float getWeightEta(float, float, int) const = 0;
  virtual float getWeightPhi(float, float, int) const = 0;
  virtual float getWeightMag(float, float, int) const = 0;

  
};

class HITowerWeightTool : virtual public asg::AsgTool, virtual public IHITowerWeightTool
{
  ASG_TOOL_CLASS(HITowerWeightTool,IHITowerWeightTool)
  public:
  HITowerWeightTool(const std::string& n);

  virtual ~HITowerWeightTool() {};
  virtual StatusCode initialize();
  virtual StatusCode configureEvent();
  virtual float getEtaPhiResponse(float eta, float phi) const;
  virtual float getEtaPhiOffset(float eta, float phi) const;
  virtual float getWeight(float eta, float phi, int sampling) const;
  virtual float getWeightEta(float eta, float phi, int sampling) const;
  virtual float getWeightPhi(float eta, float phi, int sampling) const;
  virtual float getWeightMag(float eta, float phi, int sampling) const;
  
private:
  bool m_applycorrection;
  std::string m_input_file;
  std::string m_config_dir;
  bool m_init;
  TH3F* m_h3_w;
  TH3F* m_h3_eta;
  TH3F* m_h3_phi;  
  TH3F* m_h3_mag;
  TH3F* m_h3_eta_phi_response;
  TH3F* m_h3_eta_phi_offset;
  unsigned int m_run_number;
  int m_run_index;
  std::map<unsigned int, int> m_run_map;


};


#endif
