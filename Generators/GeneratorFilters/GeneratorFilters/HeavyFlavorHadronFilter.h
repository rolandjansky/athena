/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// --------------------------------------------------
// 
// File:  GeneratorFilters/HeavyFlavorHadronFilter.h

// AuthorList:
// Marjorie Shapiro
//
// Based on SoftLeptonInJetFilter written by:
// A. Kaczmarska, F. Derue


#ifndef GENERATORFILTERSHEAVYFLAVORHADRONFILTER_H
#define GENERATORFILTERSHEAVYFLAVORHADRONFILTER_H

#include "GeneratorModules/GenFilter.h"

class HeavyFlavorHadronFilter : public GenFilter {
public:
        HeavyFlavorHadronFilter(const std::string& fname, ISvcLocator* pSvcLocator);
        virtual StatusCode filterInitialize();
        virtual StatusCode filterFinalize();
        virtual StatusCode filterEvent();

private:
	// Setable Properties:-

        double m_charmPtMin;
        double m_bottomPtMin;
        double m_charmEtaMax;
        double m_bottomEtaMax;
        double m_cPtMin;
        double m_bPtMin;
        double m_cEtaMax;
        double m_bEtaMax;
        double m_PDGPtMin;
        double m_PDGEtaMax;
        double m_deltaRFromTruth;
        double m_jetPtMin;
        double m_jetEtaMax;
        bool   m_RequestCharm;
        bool   m_RequestBottom;
        bool   m_Request_cQuark;
        bool   m_Request_bQuark;
        bool   m_RequireTruthJet;
        bool   m_RequestSpecificPDGID;
        bool   m_PDGAntiParticleToo;
        int    m_PDGID;
        std::string m_TruthJetContainerName;

	// Local Member Data:-
        int    m_NPass;
        int    m_Nevt;
        int    m_NbPass;
        int    m_NcPass;
        int    m_NBHadronPass;
        int    m_NDHadronPass;
        int    m_NPDGIDPass;

	// Private Methods:=

        // decide whether given particle is a B-hadron
        bool isBwithWeakDK(const int pID) const;
        // decide whether given particle is a D-hadron
        bool isDwithWeakDK(const int pID) const;

};

#endif
