/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
// --------------------------------------------------
// 
// File:  Generators/Starlight_i.h
// Description:
//    This code is used to get a Starlight Monte Carlo event.
//    genInitialize() is used to read parameters
//    callGenerator() makes the event
//    genFinalize() writes log files etc
//    fillEvt(GeneratorEvent* evt) passes the event to HepMC
//
//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
//
// AuthorList:
//         Andrzej Olszewski, March 2015

#ifndef GENERATORMODULESSTARLIGHT_H
#define GENERATORMODULESSTARLIGHT_H

#include "GeneratorModules/GenModule.h"

#include "starlight.h"
#include "upcevent.h"
#include "inputParameters.h"

using std::string;
typedef std::vector<std::string> CommandVector;

class StoreGateSvc;
// ---------------------

class Starlight_i:public GenModule {
public:
    Starlight_i(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~Starlight_i();
    
    virtual StatusCode	genInitialize	();
    virtual StatusCode 	callGenerator	();
    virtual StatusCode 	genFinalize	();
    virtual StatusCode 	fillEvt		(HepMC::GenEvent* evt);

protected:

    // event counter
    int m_events;

    starlight*       m_starlight;         // pointer to starlight instance
    inputParameters* m_inputParameters;   // pointer to parameter instance

    upcEvent *m_event;

    std::string  m_configFileName;
    unsigned int m_beam1Z;
    unsigned int m_beam1A;
    unsigned int m_beam2Z;
    unsigned int m_beam2A;
    double       m_beam1Gamma; 
    double       m_beam2Gamma; 
    double       m_maxW; 
    double       m_minW;
    unsigned int m_nmbWBins;
    double       m_maxRapidity;
    unsigned int m_nmbRapidityBins;
    bool         m_accCutPt;
    double       m_minPt;
    double       m_maxPt;
    bool         m_accCutEta;
    double       m_minEta;
    double       m_maxEta;
    int          m_productionMode;
    unsigned int m_nmbEventsTot;
    int          m_prodParticleId;
    int          m_randomSeed;
    int          m_outputFormat;
    int          m_beamBreakupMode;
    bool         m_interferenceEnabled;
    double       m_interferenceStrength;
    bool         m_coherentProduction;
    double       m_incoherentFactor;
    double       m_bford;
    double       m_maxPtInterference;
    int          m_nmbPtBinsInterference;
    double       m_ptBinWidthInterference; 
    bool         m_xsecMethod;
    int          m_nThreads;
    bool         m_pythFullRec;

    // Commands to setup starlight
    CommandVector m_InitializeVector;

    bool set_user_params();
    bool prepare_params_file();

    StoreGateSvc* m_storeGate;  
};

#endif

