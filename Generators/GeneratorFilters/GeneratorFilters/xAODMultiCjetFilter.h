/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
// Written by Dominik Derendarz (dominik.derendarz@cern.ch)
// Based on MultiBjetFilter by Bill Balunas

#ifndef GENERATORFILTERSXAODMULTICJETFILTER_H
#define GENERATORFILTERSXAODMULTICJETFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

//Random number generator required for accepting light jets
class TRandom3;

class xAODMultiCjetFilter:public GenFilter {

  public:
    xAODMultiCjetFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~xAODMultiCjetFilter();
    virtual StatusCode filterInitialize();
    virtual StatusCode filterFinalize();
    virtual StatusCode filterEvent();

private:

    // Basic jet requirements
    Gaudi::Property<double> m_deltaRFromTruth{this,"DeltaRFromTruth",0.3,"Delta R from truth particle"};
    Gaudi::Property<double> m_jetPtMin{this,"JetPtMin",15000.,"Minimal jet Pt"};
    Gaudi::Property<double> m_jetEtaMax{this,"JetEtaMax",2.7,"Maximal jet eta"};
    Gaudi::Property<int> m_nJetsMin{this,"NJetsMin",0,"Minimal jet multiplicity"};
    Gaudi::Property<int> m_nJetsMax{this,"NJetsMax",-1,"Maximal jet multiplicity"};
    
    // Variables for cutting sample into pt slices
    Gaudi::Property<double> m_leadJet_ptMin{this,"LeadJetPtMin",0,"Minimal leading b-jet Pt"};
    Gaudi::Property<double> m_leadJet_ptMax{this,"LeadJetPtMax",-1,"Maximal leading b-jet Pt"};
    
    // Flavor filter variables
    Gaudi::Property<double> m_bottomPtMin{this,"BottomPtMin",5000.,"Minimal bottom Pt"};
    Gaudi::Property<double> m_bottomEtaMax{this,"BottomEtaMax",3.0,"Maximal bottom eta"};
    Gaudi::Property<int> m_nCJetsMin{this,"NCJetsMin",0,"Minimal c-jet multiplicity"};
    Gaudi::Property<int> m_nCJetsMax{this,"NCJetsMax",-1,"Maximal c-jet multiplicity"};
    Gaudi::Property<double> m_charmPtMin{this,"BottomPtMin",5000.,"Minimal charm Pt"};
    Gaudi::Property<double> m_charmEtaMax{this,"BottomEtaMax",3.0,"Maximal charm eta"};
    Gaudi::Property<std::string> m_TruthJetContainerName{this,"TruthContainerName","AntiKt4TruthJets","Truht jet container name"};

    // Internal bookkeeping variables
    int    m_NPass;
    int    m_Nevt;
    double m_SumOfWeights_Pass;
    double m_SumOfWeights_Evt;

    bool isBwithWeakDK(const int pID) const;
    bool isCwithWeakDK(const int pID) const;


};

#endif


