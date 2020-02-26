// This is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PROMPT_DECORATEPROMPTLEPTONRNN_H
#define PROMPT_DECORATEPROMPTLEPTONRNN_H

/**********************************************************************************
 * @Package: LeptonTaggers
 * @Class  : DecoratePromptLeptonImproved
 * @Author : Fudong He
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
#include "xAODJet/JetContainer.h"

// ROOT
#include "TMVA/Reader.h"
#include "TStopwatch.h"

// Local
#include "VarHolder.h"
#include "PromptUtils.h"

namespace Prompt
{
  class DecoratePromptLeptonImproved: public AthAlgorithm
  {
  /*
    This class is mainly used to calculate and  decorate the PromptLeptonImproved BDT to the lepton, workflow like:
    1. Calculate the input variables of the BDT:
      1.1 Find the track jet nearby and calculate the variables relate to the track jet.
      1.2 Calculate the secondary vertex variables with the vertices that associate to the lepton.
      1.3 Get the dedicated RNN score of the lepton
    2. Predict the PromptLeptonImproved for electrons or muons by TMVA
    3. Decorate the PromptLeptonImproved BDT and its input variables to the leptons

  */

  public:

    DecoratePromptLeptonImproved(const std::string& name, ISvcLocator *pSvcLocator);
    virtual ~DecoratePromptLeptonImproved() {}
       
    StatusCode initialize();
    StatusCode execute(); 
    StatusCode finalize();

  private:

    bool initializeDecorators();
    
    void initializeConstAccessors();

    StatusCode initializeTMVAReader();

    void decorateElec(const xAOD::Electron             &electron, 
		      const xAOD::JetContainer         &trackJets,
                      const xAOD::CaloClusterContainer &clusters,  
		      const xAOD::Vertex               *primaryVertex);
    
    void decorateMuon(const xAOD::Muon         &muon, 
		      const xAOD::JetContainer &trackJets,
		      const xAOD::Vertex       *primaryVertex);

    void getMutualVariables(const xAOD::IParticle     &particle, 
			    const xAOD::Jet           &track_jet,
			    const xAOD::TrackParticle *track,
			    Prompt::VarHolder         &vars);

    void getMuonAnpVariables(const xAOD::Muon   &muon,
                             Prompt::VarHolder  &vars,
                             const xAOD::Vertex *primaryVertex);
    
    void getElectronAnpVariables(const xAOD::Electron             &elec,
                                 const xAOD::CaloClusterContainer &clusters, 
                                 Prompt::VarHolder                &vars,
                                 const xAOD::Vertex               *primaryVertex);
    
    float accessIsolation(SG::AuxElement::ConstAccessor<float> &isoAccessor, 
			  const xAOD::IParticle &particle);

    void addVarsToTMVA(Prompt::VarHolder &vars);

    void fillVarDefault(Prompt::VarHolder &vars) const;

    void decorateAuxLepton(const xAOD::IParticle &particle, 
			   const Prompt::VarHolder &vars);

    template<class T> const xAOD::Jet* FindTrackJet(const T &part, const xAOD::JetContainer &jets);

    void printAuxVars(const xAOD::IParticle *particle) const;

    double GetVertexLongitudinalNormDist(const xAOD::IParticle &lepton,
                                         const xAOD::Vertex    *secondaryVertex,
                                         const xAOD::Vertex    *primaryVertex);

    double GetVertexCosThetaWithLepDir(const xAOD::IParticle &lepton,
                                       const xAOD::Vertex    *secondaryVertex,
                                       const xAOD::Vertex    *primaryVertex);

  private:
    
    typedef std::map<Prompt::Def::Var, SG::AuxElement::Decorator<short> > shortDecoratorMap;
    typedef std::map<Prompt::Def::Var, SG::AuxElement::Decorator<float> > floatDecoratorMap;

    typedef SG::AuxElement::ConstAccessor<float>                                             AccessFloat;
    typedef SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::VertexContainer> > > AccessVertex;

    typedef std::map<Prompt::Def::Var, AccessFloat> floatAccessorMap;

  private:

    // Properties:
    std::string                           m_leptonsName;
    std::string                           m_trackJetsName;
    std::string                           m_primaryVertexName;
    std::string                           m_clusterContainerName;

    std::string                           m_configFileVersion;
    std::string                           m_configPathOverride;
    std::string                           m_methodTitleMVA;
    std::string                           m_BDTName;
    std::string                           m_inputVarDecoratePrefix;

    std::vector<std::string>              m_accessorRNNVars;
    std::vector<std::string>              m_stringIntVars;
    std::vector<std::string>              m_stringFloatVars;
    std::vector<std::string>              m_extraDecoratorFloatVars;
    std::vector<std::string>              m_extraDecoratorShortVars;
    std::vector<std::string>              m_vetoDecoratorFloatVars;
    std::vector<std::string>              m_vetoDecoratorShortVars;
    std::vector<double>                   m_leptonPtBinsVector;

    bool                                  m_printAuxVars;
    bool                                  m_printTime;

    std::string                           m_vertexLinkName;
    double                                m_vertexMinChiSquaredProb;
    double                                m_vertexMinThetaBarrElec;
    double                                m_vertexMinThetaEcapElec;
    double                                m_vertexBarrEcapAbsEtaAt;
    double                                m_elecMinCalErelConeSize;
    double                                m_maxLepTrackJetDR;

    // Variables:
    std::vector<Prompt::Def::Var>                        m_intVars;   
    std::vector<Prompt::Def::Var>                        m_floatVars; 
    std::vector<Prompt::Def::Var>                        m_allVars;  
 
    Prompt::Def::Var                                     m_BDTVarKey;

    shortDecoratorMap                                    m_shortMap;
    floatDecoratorMap                                    m_floatMap;

    std::unique_ptr<TMVA::Reader>                        m_TMVAReader;
    std::vector<Float_t>                                 m_varTMVA;

    std::unique_ptr<AccessFloat>                         m_accessCalIsolation30;
    std::unique_ptr<AccessFloat>                         m_accessTrackIsolation30;
    std::unique_ptr<AccessFloat>                         m_accessTrackIsolation30TTVA;
    std::unique_ptr<AccessFloat>                         m_accessMuonCalE;
    std::unique_ptr<AccessFloat>                         m_accessMuonParamEnergyLoss;
    std::unique_ptr<AccessVertex>                        m_accessDeepSecondaryVertex;

    floatAccessorMap                                     m_accessRNNMap;

    std::unique_ptr<TH1D>                                m_leptonPtBinHist;

    TStopwatch                                           m_timerAll;
    TStopwatch                                           m_timerExec;
    TStopwatch                                           m_timerMuon;
    TStopwatch                                           m_timerElec;
  };
}

#endif // PROMPT_DECORATEPROMPTLEPTON_H
