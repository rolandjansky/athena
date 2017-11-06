// for editors : this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_SMOOTHEDTOPTAGGER_H_
#define BOOSTEDJETSTAGGERS_SMOOTHEDTOPTAGGER_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"
#include "AsgTools/AsgTool.h"

class TF1;

class SmoothedTopTagger : public JSSTaggerBase {
  ASG_TOOL_CLASS0(SmoothedTopTagger)
  public:

  // Top tagging can run in different mode.
  // the mode is determined by the configuration.
  enum Mode {
    MassTau32,
    MassSplit23,
    Tau32Split23,
    QwTau32,
  };

  //Default - so root can load based on a name
  SmoothedTopTagger(const std::string& name);

  ~SmoothedTopTagger();

  // implement IJSSTagger interface. Returns true if all cuts pass
  virtual Root::TAccept tag(const xAOD::Jet& jet) const;

  // Run once at the start of the job to setup everything
  StatusCode initialize();

  StatusCode finalize();

  // returns the name of substructure variables used by this tool
  std::string var1() const {return m_var1CutName;}
  std::string var2() const {return m_var2CutName;}

  private:

  // used to build tau32 out of tau3 and tau2
  float buildTau32(const xAOD::Jet& j) const ;

  // need to set in initialization
  std::string m_wkpt;
  std::string m_configFile;

  // parameters for smooth fits
  std::string m_var1CutExpr;
  std::string m_var2CutExpr;

  // name of the variable used by this tool (ex: Mass, Tau32)
  std::string m_var1CutName;
  std::string m_var2CutName;

  // functions implementing the cut values vs pt
  // they are build from m_varXCutExpr
  TF1* m_var1CutFunc=0;
  TF1* m_var2CutFunc=0;

  // mode in which the tool is operating (ex: MassTau32 or Tau32Split23)
  std::string m_modeName;
  Mode m_mode;

  // string for decorating jets with DNN output
  std::string m_decorationName;

  // decorators
  SG::AuxElement::Decorator<float>    m_dec_mcut;
  SG::AuxElement::Decorator<float>    m_dec_tau32cut;
  SG::AuxElement::Decorator<float>    m_dec_split23cut;
  SG::AuxElement::Decorator<float>    m_dec_qwcut;


};

#endif
