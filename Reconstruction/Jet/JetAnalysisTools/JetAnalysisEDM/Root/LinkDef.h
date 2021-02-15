// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Helpers.py 472095 2011-12-02 10:27:01Z krasznaa $
#ifdef __CINT__


#include "../JetAnalysisEDM/VarHandle.h"
#include "../JetAnalysisEDM/Particle.h"
#include "../JetAnalysisEDM/Utils.h"
#include <map>

#include "../JetAnalysisEDM/ParticleContainer.h"
#include "../JetAnalysisEDM/EventObject.h"
#include "../JetAnalysisEDM/EventManager.h"

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

// #pragma link C++ class ROOT::Math::GenVector_exception;
// #pragma function C++ class ROOT::Math::GenVector_exception:fgOn;

//#pragma link C++ class ROOT::Math::PtEtaPhiE4D<float>+;
//#pragma link C++ class ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiE4D<float> >+;
//#pragma link C++ typedef JetAnalysisEDM::LorentzVector_t;
#pragma link C++ class std::pair<std::string, std::string>;
#pragma link C++ class JetAnalysisEDM::VarHandleBase+;
#pragma link C++ class JetAnalysisEDM::VarHandleLog+;
#pragma link C++ class JetAnalysisEDM::ParticleContainer+;
#pragma link C++ class JetAnalysisEDM::Particle+;
#pragma link C++ class JetAnalysisEDM::Session+;
#pragma link C++ class JetAnalysisEDM::EventObject+;

#pragma link C++ class vector<vector<int> >+;
#pragma link C++ class vector<vector<float> >+;
#pragma link C++ class vector<vector<unsigned int> >+;

// #pragma link C++ function JetAnalysisEDM::Particle::get<float>;
// #pragma link C++ function JetAnalysisEDM::Particle::get<int>;
// #pragma link C++ function JetAnalysisEDM::Particle::get<vector<float> >;
// #pragma link C++ function JetAnalysisEDM::Particle::get<vector<int> >;
// #pragma link C++ function JetAnalysisEDM::Particle::set<float>;
// #pragma link C++ function JetAnalysisEDM::Particle::set<int>;
// #pragma link C++ function JetAnalysisEDM::Particle::set<vector<float> >;
// #pragma link C++ function JetAnalysisEDM::Particle::set<vector<int> >;

// #pragma link C++ class std::map<std::string, JetAnalysisEDM::ParticleContainer*>+;
// #pragma link C++ class JetAnalysisEDM::EventManager+;

// #pragma link C++ function JetAnalysisEDM::EventObject::get<float>;
// #pragma link C++ function JetAnalysisEDM::EventObject::get<int>;
// #pragma link C++ function JetAnalysisEDM::EventObject::get<vector<float> >;
// #pragma link C++ function JetAnalysisEDM::EventObject::get<vector<int> >;

#endif // __CINT__
