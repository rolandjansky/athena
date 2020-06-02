/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HITOWERWEIGHTTOOL_H
#define HIEVENTUTILS_HITOWERWEIGHTTOOL_H

#include "HIEventUtils/IHITowerWeightTool.h"
#include "AsgTools/IAsgTool.h"
#include "AsgTools/AsgTool.h"
#include <map>

class TH3F;

////////////////////////////////////////////////////////////
// Tool that gets the tower weights for by tower by tower
// difference in response during the HI jet reconstruction.
////////////////////////////////////////////////////////////

class HITowerWeightTool : virtual public asg::AsgTool, virtual public IHITowerWeightTool
{
    ASG_TOOL_CLASS(HITowerWeightTool,IHITowerWeightTool)
  public:
    HITowerWeightTool(const std::string& n);

    virtual ~HITowerWeightTool() {};
    virtual StatusCode initialize() override;
    virtual StatusCode configureEvent() override;
    virtual float getEtaPhiResponse(float eta, float phi) const override;
    virtual float getEtaPhiOffset(float eta, float phi) const override;
    virtual float getWeight(float eta, float phi, int sampling) const override;
    virtual float getWeightEta(float eta, float phi, int sampling) const override;
    virtual float getWeightPhi(float eta, float phi, int sampling) const override;
    virtual float getWeightMag(float eta, float phi, int sampling) const override;

  private:
    bool m_applycorrection;
    std::string m_inputFile;
    std::string m_configDir;
    bool m_init;
    TH3F* m_h3W;
    TH3F* m_h3Eta;
    TH3F* m_h3Phi;
    TH3F* m_h3Mag;
    TH3F* m_h3EtaPhiResponse;
    TH3F* m_h3EtaPhiOffset;
    unsigned int m_runNumber;
    int m_runIndex;
    std::map<unsigned int, int> m_runMap;


};


#endif
