/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERSHTFILTER_H
#define GENERATORFILTERSHTFILTER_H

#include "GeneratorModules/GenFilter.h"
#include <string>
#include "GaudiKernel/ServiceHandle.h"

class MsgStream;
class StoreGateSvc;

class HTFilter:public GenFilter {

public:

    HTFilter(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~HTFilter();
    virtual StatusCode filterInitialize();
    virtual StatusCode filterFinalize();
    virtual StatusCode filterEvent();

    bool fromTau( HepMC::ConstGenParticlePtr tp ) const;
    bool fromWZ( HepMC::ConstGenParticlePtr tp ) const;

private:

    double m_MinJetPt;  //!< Min pT for the truth jets
    double m_MaxJetEta; //!< Max eta for the truth jets
    double m_MinHT;  //!< Min HT for events
    double m_MaxHT;  //!< Max HT for events
    double m_MinLepPt;  //!< Min pT for the truth jets
    double m_MaxLepEta; //!< Max eta for the truth jets
    bool   m_UseNu;  //!< Use neutrinos in HT
    bool   m_UseLep;   //!< Use leptons in HT

    std::string m_TruthJetContainerName;  //!< Name of the truth jet container

    long m_total;    //!< Total number of events tested
    long m_passed;   //!< Number of events passing all cuts
    long m_ptfailed; //!< Number of events failing the pT cuts 
       
};

#endif
