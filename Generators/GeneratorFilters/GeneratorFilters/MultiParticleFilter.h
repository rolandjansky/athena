/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/MultiParticleFilter.h
// Description:
//   Allows the user to search for Np particles with multiple statuses passing pT and Eta cuts.
//   The particles are required to be listed in the vectors particlePDG[] and particleStatus[].
//   The filter will search for these particles and if Np particles are found which fulfil these requirements and the kinematic cuts, the filter returns success,
//   e.g., setting Np = 2, particlePDG = [23,24] with particleStatus = [23] and ptMinParticle = 10 in the jobOptions, 
//   will accepts events with at least two W/Z bosons with status=23 and pT > 10 GeV.
//
// AuthorList:
//   Orel Gueta March 2014


#ifndef GENERATORFILTERSMULTIPARTICLEFILTER_H
#define GENERATORFILTERSMULTIPARTICLEFILTER_H

#include "GeneratorModules/GenFilter.h"

class MultiParticleFilter:public GenFilter {

public:
        MultiParticleFilter(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~MultiParticleFilter();
        virtual StatusCode filterInitialize();
        virtual StatusCode filterFinalize();
        virtual StatusCode filterEvent();

private:
	// Setable Properties:

	// Local Member Data:
  bool m_useIsBSM;
	std::vector<int> m_particlePDG;
	std::vector<int> m_particleStatus;
  bool m_considerStatus;
	double m_ptMinParticle;
  double m_ptMaxParticle;
  double m_etaMinParticle;
  double m_etaMaxParticle;
  bool m_useAbsEta;
  double m_phiMinParticle;
  double m_phiMaxParticle;
  unsigned int m_Np;

  long m_total;    // Total number of events tested
  long m_passed;   // Number of events passing cuts

	// Private Methods:

};

#endif
