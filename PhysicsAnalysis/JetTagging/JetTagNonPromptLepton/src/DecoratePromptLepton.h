// This is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PROMPT_DECORATEPROMPTLEPTON_H
#define PROMPT_DECORATEPROMPTLEPTON_H

/**********************************************************************************
 * @Package: JetTagNonPromptLepton
 * @Class  : DecoratePromptLepton
 * @Author : Rhys Roberts
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Decorate leptons with prompt BDT output 
 * 
 **********************************************************************************/

// Tools
#include "PathResolver/PathResolver.h"

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"

// xAOD
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetContainer.h"

// ROOT
#include "TMVA/Reader.h"
#include "TStopwatch.h"

// Local
#include "VarHolder.h"
#include "PromptUtils.h"


namespace Prompt
{
  class DecoratePromptLepton: virtual public AthAlgorithm
  {

  public:

    DecoratePromptLepton(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~DecoratePromptLepton() {}
       
    StatusCode initialize();
    StatusCode execute(); 
    StatusCode finalize();

  private:

    bool initializeTMVAReader();
    bool initializeTMVAReaderTau();

    void initializeDecorators();
    
    void initializeConstAccessors();

    void decorateElectron(const xAOD::Electron* electron, 
			  const xAOD::JetContainer* trackJets);
    void decorateMuon    (const xAOD::Muon*     muon, 
		          const xAOD::JetContainer* trackJets);
    void decorateTau     (const xAOD::TauJet*   tau, 
		          const xAOD::JetContainer* trackJets);

    int  getJetVariables   (const xAOD::Jet* jet,            Prompt::VarHolder &vars);
    void getLeptonVariables(const xAOD::IParticle* particle, Prompt::VarHolder &vars);
    void getMutualVariables(const xAOD::IParticle* particle, 
			    const xAOD::Jet* jet,
			    const xAOD::TrackParticle* track,
			    Prompt::VarHolder &vars,
			    float DRlj);

    void getMutualTauVariables(const xAOD::TauJet* tau, 
			       const xAOD::Jet* jet,
			       Prompt::VarHolder &vars,
			       float DRlj);

    float accessIsolation(SG::AuxElement::ConstAccessor<float>* isoAccessor, 
			  const xAOD::IParticle* particle);

    void addVarsToTMVA(Prompt::VarHolder &vars);

    void fillVarDefault(Prompt::VarHolder &vars);

    void decorateAuxLepton(const xAOD::IParticle* particle, 
			   Prompt::VarHolder &vars, 
			   bool goodJet);

    void decorateAuxTau(const xAOD::TauJet* tau, 
			Prompt::VarHolder &vars, 
			bool goodJet,
			const int ntracktau);

    template<class T> std::pair<double, const xAOD::Jet*>
      FindNearestTrackJet(const T &part, const xAOD::JetContainer* jets);

    void printAuxVars(const xAOD::IParticle* particle) const;

  private:
    
    typedef std::map<Prompt::Def::Var, SG::AuxElement::Decorator<short> > shortDecoratorMap;
    typedef std::map<Prompt::Def::Var, SG::AuxElement::Decorator<float> > floatDecoratorMap;

    typedef SG::AuxElement::ConstAccessor<float>                                             AccessFloat;
    typedef SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::VertexContainer> > > AccessVertex;

    typedef std::pair<Prompt::Def::Var, double> VarPair;

  private:

    // Properties:
    std::string                           m_leptonContainerName;
    std::string                           m_trackJetContainerName;
    std::string                           m_configFileVersion;
    std::string                           m_configFileVersionOneTrack;
    std::string                           m_configFileVersionThreeTrack;
    std::string                           m_configPathOverride;
    std::string                           m_methodTitleMVA;
    std::string                           m_methodTitleMVAOneTrack;
    std::string                           m_methodTitleMVAThreeTrack;
    std::string                           m_BDTName;
    std::string                           m_auxVarPrefix;

    std::vector<std::string>              m_stringIntVars;
    std::vector<std::string>              m_stringFloatVars;

    bool                                  m_printAuxVars;
    bool                                  m_printTime;

    // Variables:
    const xAOD::ElectronContainer*        m_electrons;
    const xAOD::MuonContainer*            m_muons;
    const xAOD::TauJetContainer*          m_taus;

    std::vector<Prompt::Def::Var>         m_intVars;
    std::vector<Prompt::Def::Var>         m_floatVars;
    std::vector<Prompt::Def::Var>         m_allVars;

    shortDecoratorMap                     m_shortMap;
    floatDecoratorMap                     m_floatMap;

    TMVA::Reader                         *m_TMVAReader;
    TMVA::Reader                         *m_TMVAReaderOneTrack;
    TMVA::Reader                         *m_TMVAReaderThreeTrack;

    std::vector<Float_t*>                 m_varTMVA;

    AccessFloat                          *m_accessCalIsolation20;
    AccessFloat                          *m_accessCalIsolation30;
    AccessFloat                          *m_accessTrackIsolation30;
    AccessVertex                         *m_accessSV1Vertices;

    SG::AuxElement::Decorator<float>     *m_decoratorBDT;
    SG::AuxElement::Decorator<float>     *m_decoratorDR;

    TStopwatch                            m_timerAll;
    TStopwatch                            m_timerExec;
    TStopwatch                            m_timerMuon;
    TStopwatch                            m_timerTau;
    TStopwatch                            m_timerElec;
    TStopwatch                            m_timerJet;
  };
}

#endif // PROMPT_DECORATEPROMPTLEPTON_H
