/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATOR_PYTHIA8B_H
#define GENERATOR_PYTHIA8B_H

#include "Pythia8_i/Pythia8_i.h"
#include "Pythia8/UserHooks.h"
/**
 *  Authors: James Catmore and Maria Smizanska
 *  James.Catmore@cern.ch / Maria.Smizanska@cern.ch
 *  Inherits from Pythia8_i by James Monk
 **/

class Pythia8B_i: public Pythia8_i {
    
public:
    Pythia8B_i(const std::string &name, ISvcLocator *pSvcLocator);
    
    ~Pythia8B_i();
    
    virtual StatusCode genInitialize();
    virtual StatusCode genuserInitialize();
    virtual StatusCode callGenerator();
    virtual StatusCode genFinalize();
    virtual StatusCode fillEvt(HepMC::GenEvent*);
    
    bool leptonSelect(Pythia8::Event&, std::vector<double>, double, std::vector<int>, int, double, bool);
    bool cleanUndecayed(Pythia8::Event&, std::vector<int>);
    bool pairProperties(Pythia8::Event&, std::vector<int>, double, bool);
    void descendThroughDecay(Pythia8::Event&, std::vector<Pythia8::Particle>&, int);
    std::vector<int> getCodes(std::vector<Pythia8::Particle>);
    bool compare(std::vector<int>, std::vector<int>);
    bool passesCuts(std::vector<Pythia8::Particle>, std::vector<double>, std::string);
    bool signalAccept(Pythia8::Event&, std::vector<int>, std::vector<double>, std::vector<double>, unsigned int);
    bool userSelection(Pythia8::Event&, std::string, std::vector<double>);
    void printSignalSelections(std::vector<int>, std::vector<double>, std::vector<double>, unsigned int);
    
    static IAtRndmGenSvc* p_AtRndmGenSvc;
    
private:
    
    bool m_useRndmGenSvc;
    unsigned int m_had, m_dec;
    int m_trigCode; 
    unsigned int m_nSignalRequired;
    std::vector<int> m_bcodes, m_sigCodes, m_cutCount;
    std::vector<double> m_trigPtCut;
    std::vector<double> m_sigPtCuts, m_sigEtaCuts;
    int m_totalPythiaCalls,m_totalBQuark, m_totalCQuark, m_totalBBarQuark, m_totalCBarQuark, m_totalClone, m_passingTriggerCuts, m_internal_event_number, m_totalHard, m_atLeastOneAcc;
    std::map<int,int> m_speciesCount;
    std::vector<long int> m_seeds;
    bool m_and, m_oppCharges, m_vetoDoubleB, m_vetoDoubleC, m_selectBQuarks, m_selectCQuarks;
    double m_qPtCut, m_aqPtCut, m_qEtaCut, m_aqEtaCut, m_invMass, m_trigEtaCut;
    std::string m_userString;
    std::vector<double> m_userVar;
    std::vector<Pythia8::Event> m_BEventBuffer;
    std::vector<int> m_internalEventNumbers;
    bool m_doSuppressSmallPT;
    double m_pt0timesMPI;
    double m_numberAlphaS;
    bool m_sameAlphaSAsMPI;
    Pythia8::SuppressSmallPT *m_SuppressSmallPT;
    unsigned int m_maxFailures;
    unsigned int m_failureCount;

};

#endif
