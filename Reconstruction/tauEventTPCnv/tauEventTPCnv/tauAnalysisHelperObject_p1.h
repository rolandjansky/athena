/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef tauEventTPCnv_TAUANALYSISHELPEROBJECT_P1_H
#define tauEventTPCnv_TAUANALYSISHELPEROBJECT_P1_H

#include <vector>
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

class tauAnalysisHelperObject_p1
{
    public:
        int    m_decmode;
        int    m_jettype;
        double m_emradii[6];
        double m_hadradii[6];
        double m_TowEMRadius;
        double m_TowET12Frac;
        double m_ImpactParameter[10];
        double m_RawImpactParameter[10];
        double m_SignedImpactParameter[10];
        double m_ImpactParameterSignificance[10];
        double m_SignedImpactParameterSignificance[10];

        double m_d0prf;
        double m_d0iso;
        double m_d0isoet;
        double m_d0ettr;
        double m_d0etem;
        double m_d0etem2;
        double m_d0emclet;
        double m_d0emcleta;
        double m_d0emclphi;
        double m_d0et05;
        double m_d0eta05;
        double m_d0phi05;

        double m_d0hadet;
        double m_d0hadeta;
        double m_d0hadphi;

        double m_d0type;
        double m_d0deltaR1;
        double m_d0eTosumpT;

        double m_d0deltaR1had;
        double m_d0em3iso;
        double m_d0mtrem3;

        double m_d0deltaR2;
        double m_d0ntr1030;

        double m_d0EM12isof;
        double m_d0e1e2otaupT;
        double m_d0ettro123;
        double m_d0ett1oEtiso;
        double m_d0ett1oEtisoet;
        double m_d0dalpha;
        double m_d0e1e2;

        double m_d0mtr1tr2;
        double m_d0mtr1tr2tr3;

        double m_d0sumtaupt;
        double m_d0sumnontaupt;
        double m_d0sumpt;

        std::vector<double> m_towere;
        std::vector<double> m_towereta;
        std::vector<double> m_towerphi;

        double m_d0uncaletlayers[25];

        ElementLinkIntVector_p1 m_d0_emcluster;
        ElementLinkIntVector_p1 m_d0_05_Tracks;
        ElementLinkIntVector_p1 m_d0_tau_Tracks;
        ElementLinkIntVector_p1 m_d0_nontau_Tracks;
};

#endif
