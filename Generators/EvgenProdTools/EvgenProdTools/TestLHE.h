/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#ifndef EVGENPRODTOOLS_TESTLHE_H
#define EVGENPRODTOOLS_TESTLHE_H

#include "GeneratorModules/GenBase.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TruthHelper/IsGenNonInteracting.h"
#include "AGDDControl/XercesParser.h"
#include "TFile.h"
#include "TH1.h"
#include "HepMC/GenEvent.h"
#include<cmath>
#include<ctype.h>
#include<locale>

#include<fstream>

class TestLHE : public GenBase {

    public:

        TestLHE(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

    private:

        bool m_doHist;
        int m_pdg;
        int m_nparticle;
        int m_nPass;
        int m_nFail;
        int m_nparticles;

        float m_abs_XWGTUP;
        float m_cm_energy;
        float m_momentumBalance;
        float m_energy_diff;
        float m_weight_diff;

        bool m_infoStructureTest;
        bool m_beamEnergyTest;
        bool m_unknownPDGIDTest;
        bool m_isLargeWeight;

        bool m_ifCreateNewLheFile;

        std::string m_unknownPDGIDFile;
        std::string m_eventsLheFile;
        std::string m_modifiedLheFile;
        std::string m_paramFile;

        ServiceHandle<ITHistSvc> m_thistSvc;

        std::vector<int> m_unknownPDGID_tab;

        //========Initial Weight Check=======
        std::vector<std::string> m_GroupName;                //weightGroupName
        std::vector<std::vector<int>> m_WeightInitID;        //Initial part weightID
        std::vector<std::vector<std::string>> m_WeightInitName;   //Initial part weightName
        std::vector<int> weightTemp;
        std::vector<int> weightBlockTemp;             //rwgt part weight id temp vector
        std::vector<std::vector<int>> m_WeightID;            //rwgt part weight ID
        std::vector<std::vector<double>> m_WeightValue;      //rwgt part weight value
        std::vector<double> weightValueTemp;
        std::vector<std::string> modifyweightTemp;
        std::vector<std::string> WeightInitTemp;
        std::vector<std::string> InformationTemp;          //event and particle information Temp vector
        std::vector<std::vector<std::string>> modify_weight;
        std::vector<std::vector<std::string>> target_information;

        //========End Weight Check========
        std::vector<int> m_storeEventLine;
        std::vector<int> m_G4pdgID_tab;
        std::vector<int> m_SusyPdgID_tab;
        std::vector<std::vector<int> > m_EventIntInfo_tab;
        std::vector<std::vector<float>> m_EventFloatInfo_tab;
        std::vector<std::vector<std::vector<int>>> m_ParticleIntInfo_tab;
        std::vector<int> m_tempInt_tab;
        std::vector<std::vector<int>> m_particleTempInt_tab;
        std::vector<std::vector<std::vector<float>>> m_ParticleFloatInfo_tab;
        std::vector<float> m_tempFloat_tab;
        std::vector<std::vector<float>> m_particleTempFloat_tab;
        std::vector<int> m_InitIntInfo_tab;
        std::vector<float> m_InitFloatInfo_tab;

        //=======comment line information
        std::vector<int> storeCommentline;

        int m_invalidEventformatCheckRate;
        int m_invalidParticleFormatCheckRate;
        int m_NegativeParticleCheckRate;
        int m_EnergyBalanceCheckRate;
        int m_MomentumBalanceCheckRate;
        int m_particleNanCheckRate;
        int m_particleInfiniteCheckRate;
        int m_tachyonParticleCheckRate;
        int m_unknownPDGIDCheckRate;

        int n_events;
        int n_particles;

        std::ifstream LHEfile;

        //calculate transverse momentum
        float calculateMomentum(float x_momentum, float y_momentum)
        {
            float momentum = std::sqrt(x_momentum*x_momentum + y_momentum*y_momentum);
            return momentum;
        }

        bool nameDuplicate(std::vector<std::string> &weightName)
        {
            int length = weightName.size();
            for(int i = 0 ; i < length-1 ; i++)
            {
                for( int j = i+1; j < length ; j++)
                {
                    if (weightName.at(i) == weightName.at(j)) { 
                        return true;
                    }
                }
            }
            return false;
        }

        bool idDuplicate(std::vector<int> &weightID)
        {
            int length = weightID.size();
            for(int i = 0 ; i < length-1 ; i++)
            {
                for( int j = i+1; j < length ; j++)
                {
                    if (weightID.at(i) == weightID.at(j)) { 
                        return true;
                    }
                }
            }
            return false;
        }

        bool CheckTextNumberOrLetter(std::vector<std::string> GroupName)
        {
            bool isSpecial = false;
            std::locale loc;
            int length = GroupName.size();
            for (int m = 0 ; m < length ; m++)
            {
                std::string name = GroupName[m];
                for(std::string::iterator it = name.begin(); it != name.end(); it++)
                {
                    if ( *it == '_' || *it == '=' || *it == '.' || *it == '+' || *it == '-' ) continue;
                    else {
                        if ( std::ispunct(*it, loc) || std::isspace(*it,loc) )
                        {
                            isSpecial = true;
                            break;
                        }
                    }
                }
            }
            return isSpecial;
        }

};

#endif

#endif
