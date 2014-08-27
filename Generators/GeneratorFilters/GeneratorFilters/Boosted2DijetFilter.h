/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/Boosted2DijetFilter.h

// AuthorList:
// Stephen Bieniek, Luke Lambourne
//
// Based on Filter HeavyFlavorHadronFilter written by:
// Marjorie Shapiro


#ifndef GENERATORFILTERSBOOSTED2DIJETFILTER_H
#define GENERATORFILTERSBOOSTED2DIJETFILTER_H

#include "GeneratorModules/GenFilter.h"

class Boosted2DijetFilter:public GenFilter {
public:
        Boosted2DijetFilter(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~Boosted2DijetFilter();
        virtual StatusCode filterInitialize();
        virtual StatusCode filterFinalize();
        virtual StatusCode filterEvent();

private:
	// Setable Properties:-

        double m_jetPtMin;
        double m_jetEtaMax;
        double m_dijet_ptMin;
        double m_akt8PtMin;
        double m_dijet_dRMax;
        std::string m_TruthJetContainerName;
        std::string m_TruthJetContainerName_akt8;

	// Local Member Data:-
        int    m_NPass;
        int    m_Nevt;
        double m_SumOfWeigths_Pass;
        double m_SumOfWeigths_Evt;

};

#endif
