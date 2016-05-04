/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_JMSCORRECTION_H
#define JETCALIBTOOLS_JMSCORRECTION_H 1

 /*
 *  Class definition of JMSCorrection - see Root/JMSCorrection.cxx for more details
 *  Jonathan Bossio (jbossios@cern.ch) , July 2015
 */

#include <TEnv.h>
#include <TAxis.h>
#include <TH2F.h>

#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"

class JMSCorrection 
  : virtual public ::IJetCalibrationTool,
    virtual public ::JetCalibrationToolBase
{

  ASG_TOOL_CLASS( JMSCorrection, IJetCalibrationTool )

 public:
  //Some convenient typedefs
  typedef std::vector<TH2F*> VecTH2F;
  typedef std::vector<double> VecD;
  typedef unsigned int uint;

  JMSCorrection();
  JMSCorrection(const std::string& name);
  JMSCorrection(const std::string& name, TEnv * config, TString jetAlgo, TString calibAreaTag, bool dev);
  virtual ~JMSCorrection();

  virtual StatusCode initializeTool(const std::string& name);

 protected:
  virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const;

 private:
  float getMassCorr(double pT_uncorr, double m_uncorr, int etabin) const;
  float getTrackAssistedMassCorr(double pT_uncorr, double m_uncorr, int etabin) const;

  void setMassEtaBins(VecD etabins) { 
    if (etabins.size()==0) ATH_MSG_ERROR("Please check that the mass eta binning is properly set in your config file");
    m_massEtaBins=etabins;
  }

 private:

  //Private members set in the constructor
  TEnv * m_config;
  TString m_jetAlgo, m_calibAreaTag;
  bool m_dev;

  double m_pTMinCorr;

  bool m_trackAssistedJetMassCorr;

  //Private members set during initialization
  VecTH2F m_respFactorsMass;
  VecD m_massEtaBins;
  VecTH2F m_respFactorsTrackAssistedMass;
  
};

#endif
