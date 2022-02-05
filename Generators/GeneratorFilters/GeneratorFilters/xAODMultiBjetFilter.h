/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
// Written by Bill Balunas (balunas@cern.ch)
// Based on DiBjetFilter by Stephen Bienek

#ifndef GENERATORFILTERSxAODMULTIBJETFILTER_H
#define GENERATORFILTERSxAODMULTIBJETFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"

//Random number generator required for accepting light jets
class TRandom3;

class xAODMultiBjetFilter:public GenFilter {

  public:
    xAODMultiBjetFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~xAODMultiBjetFilter();
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
    Gaudi::Property<int> m_nBJetsMin{this,"NBJetsMin",0,"Minimal b-jet multiplicity"};
    Gaudi::Property<int> m_nBJetsMax{this,"NBJetsMax",-1,"Maximal b-jet multiplicity"};
    Gaudi::Property<std::string> m_TruthJetContainerName{this,"TruthContainerName","AntiKt4TruthJets","Truht jet container name"};

    // Internal bookkeeping variables
    int    m_NPass;
    int    m_Nevt;
    double m_SumOfWeights_Pass;
    double m_SumOfWeights_Evt;

    bool isBwithWeakDK(const int pID) const;


};

#endif


