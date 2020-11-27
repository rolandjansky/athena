/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOOSTEDJETSTAGGERS_SMOOTHEDTOPTAGGER_H_
#define BOOSTEDJETSTAGGERS_SMOOTHEDTOPTAGGER_H_

#include "BoostedJetTaggers/JSSTaggerBase.h"

class SmoothedTopTagger :
  public JSSTaggerBase {
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

      // implement IJetSelectorTool interface. Returns true if all cuts pass
      virtual Root::TAccept& tag(const xAOD::Jet& jet) const override;

      // Run once at the start of the job to setup everything
      virtual StatusCode initialize() override;

      // returns the name of substructure variables used by this tool
      std::string var1() const {return m_var1CutName;}
      std::string var2() const {return m_var2CutName;}

    private:

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
      std::unique_ptr<TF1> m_var1CutFunc;
      std::unique_ptr<TF1> m_var2CutFunc;

      // mode in which the tool is operating (ex: MassTau32 or Tau32Split23)
      std::string m_modeName;
      Mode m_mode;

      // decorators
      SG::AuxElement::Decorator<float>    m_dec_mcut;
      SG::AuxElement::Decorator<float>    m_dec_tau32cut;
      SG::AuxElement::Decorator<float>    m_dec_split23cut;
      SG::AuxElement::Decorator<float>    m_dec_qwcut;
  
  };

#endif
