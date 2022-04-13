/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#ifndef EVGENPRODTOOLS_TESTLHE_H
#define EVGENPRODTOOLS_TESTLHE_H

#include "GeneratorModules/GenBase.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TruthHelper/IsGenNonInteracting.h"
#include "TruthUtils/PIDHelpers.h"
#include "AGDDControl/XercesParser.h"
#include "TFile.h"
#include "TH1.h"
#include "HepMC/GenEvent.h"
#include <cmath>
#include <ctype.h>
#include <locale>

#include <fstream>

using std::vector;
using std::string;

struct mom
{
    double px = 0.;
    double py = 0.;
    double pz = 0.;
    double e = 0.;
    double mass = 0.;
};

typedef std::array<int, 5> initInfo;
typedef std::array<int, 9> parInfo;
typedef std::array<int, 3> rwgtInfo;

class TestLHE : public GenBase {

    public:

        TestLHE(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

        initInfo check_InitBlock(const vector<std::string>& groupname, const vector<std::string>& weightname, const vector<int>& weightid);
        parInfo  check_particleInfo(int numline, int nPar, float energyDiff, float momDiff, bool doTest, const vector<int>& pdgid, const vector<int>& unknownID, const vector<int>& Parstatus, const vector<mom>& FourMom);
        rwgtInfo check_rwgtBlock(const vector<int>& initWeightId, const vector<int>& rwgtId);

        int check_eventInfoFormat(const vector<int>& intInfo, const vector<float>& floatInfo);
        int check_particleInfoFormat(const vector<int>& intInfo, const vector<float>& floatInfo);
        int check_weightConsistent(const vector<float>& eventWeight, const vector<float>& listWeight, float weightDiff);
        std::string weight_Modification(int numLine, std::string& input, const std::string& modifyWeight, float eventWeight, int eventLine);

    private:

        std::ifstream LHEfile;
        std::ofstream ModifiedFile;

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

        std::array<int, 5> initList;
        std::array<int, 9> particleList;
        std::array<int, 3> rwgtList;

        std::string m_unknownPDGIDFile;
        std::string m_eventsLheFile;
        std::string m_modifiedLheFile;
        std::string m_paramFile;

        ServiceHandle<ITHistSvc> m_thistSvc;

        std::vector<int> m_unknownPDGID_tab;
        std::vector<int> m_G4pdgID_tab;
        std::vector<int> m_SusyPdgID_tab;

        std::vector<int> m_storeRwgtLine;

        //========Initial Weight Check=======
        std::vector<std::string> m_GroupName;                //weightGroupName
        std::vector<int>         m_InitID;
        std::vector<int>         m_WeightInitID;             //Initial part weight ID
        std::vector<std::string> m_WeightInitName;           //Initial part weight name
        std::vector<int>         m_WeightID;                 //rwgt part weight ID
        std::vector<float>       m_WeightValue;              //rwgt part weight value
        std::vector<std::string>       modify_weight;

        //========End Weight Check========
        std::vector<float> m_storeEventWeight;
        std::vector<float> m_storeListWeight;
        std::vector<int >  m_EventIntInfo_tab;
        std::vector<float> m_EventFloatInfo_tab;
        std::vector<std::vector<int>> m_ParticleIntInfo_tab;
        std::vector<int>   m_particleTempInt_tab;
        std::vector<std::vector<float>> m_ParticleFloatInfo_tab;
        std::vector<float> m_particleTempFloat_tab;
        std::vector<int>   m_InitIntInfo_tab;
        std::vector<float> m_InitFloatInfo_tab;


        int m_invalidEventformatCheckRate;
        int m_invalidParticleFormatCheckRate;
        int m_NegativeParticleCheckRate;
        int m_EnergyBalanceCheckRate;
        int m_MomentumBalanceCheckRate;
        int m_particleNanCheckRate;
        int m_particleInfiniteCheckRate;
        int m_tachyonParticleCheckRate;
        int m_unknownPDGIDCheckRate;
        int m_idDuplicateCheckRate;
        int m_nameDuplicateCheckRate;
        int m_idInitDuplicateCheckRate;
        int m_nameSpecialLetterCheckRate;
        int m_isAscendOrderCheckRate;
        int m_isInitAscendOrderCheckRate;
        int m_nameForId0CheckRate;
        int m_particleNumberCheckRate;
        int m_weightSameValueCheckRate;
        int m_weightConsistentCheckRate;
        int m_weightExceedRangeCheckRate;

        int n_events;
        int n_particles;
        int version;
        int InitWeightNumber;

        // ======= initialization information in LHE file =========
        std::pair<int, int> IDBMUP;      //PDG id of beam partibles
        std::pair<double, double> EBMUP; //Energy of beam partibles given in GeV
        std::pair<int, int> PDFGUP;      //author group for the PDG used for the beams according to the PDFLib specification
        std::pair<int, int> PDFSUP;      //id number the PDF used for the beams
        int IDWTUP;                      //event weight from ME generator
        int NPRUP;                       //number of different subprocesses 
        double XSECUP;                   //cross-section for different subprocesses (pb)
        double XERRUP;                   //statistical error in the cross-section (pb)
        double XMAXUP;                   //maximum event weights
        int LPRUP;                       //subprocess code

        // ======= event information in LHE file ==========
        int NUP = 0;
        int IDPRUP = 0;
        double XWGTUP = 0.;
        double SCALUP = 0.;
        double AQEDUP = 0.;
        double AQCDUP = 0.;

        // ====== particle information in LHE file ===========
        std::vector<int> IDUP;
        std::vector<int> ISTUP;
        std::vector<int> MOTHUP1;
        std::vector<int> MOTHUP2;
        std::vector<int> ICOLUP1;
        std::vector<int> ICOLUP2;
        std::vector<mom> FourMomentum;
        std::vector<double> VTIMUP;
        std::vector<double> SPINUP;

        bool currentFind(const std::string& str, const std::string& currentLine) const {
            return currentLine.find(str) != std::string::npos;
        }

        bool nameDuplicate(const std::vector<std::string>& weightName)
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

        bool idDuplicate(const std::vector<int>& weightID)
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

        bool CheckTextNumberOrLetter(const std::vector<std::string>& GroupName)
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

        int check_lheSymbol(const std::string& symbolLine);
        std::string GetGroupName(const std::string& Groupline, const std::string& symbol);
        int GetWeightId(const std::string& IDline, const std::string& symbol);
        std::string GetWeightName(const std::string& wline, const std::string& symbol);
        double GetWeight(const std::string& wline);
        vector<int> GetEventIntInfo(const std::string& eventLine);
        vector<float> GetEventFloatInfo(const std::string& eventLine);

};

#endif

#endif
