/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPERFORMANCEALGS_MUONTPMETADATAALG_H
#define MUONPERFORMANCEALGS_MUONTPMETADATAALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include <TTree.h>
#include <set>

class ITHistSvc;
class MuonTPMetaDataAlg: public ::AthAnalysisAlgorithm {
    public:
        MuonTPMetaDataAlg(const std::string& name, ISvcLocator* pSvcLocator);
        virtual ~MuonTPMetaDataAlg();

        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

        virtual StatusCode beginInputFile();

    private:
        ServiceHandle<ITHistSvc> m_histSvc;
        TTree* m_MetaDataTree;
        void FillTree(bool isData, unsigned int runNumber, int mcChannelNumber);
        bool m_isData;
        //This flag decides whether the AOD is derived or not. In  the derived case, TotalEvents, SumOfweights as well as the SumOfWeights2 are taken from the CutBookKeeper
        bool m_isDerivedAOD;
        //Store the total number of events in the AOD file
        Long64_t m_TotEvents;
        //Store the number of events processed by the job. (Only relevant for jobs on the DAOD files)
        Long64_t m_ProcEvents;
        //Store the current runNumber
        unsigned int m_RunNumber;

        //For MC the mcChannelNumber
        int m_mcChannelNumber;

        //Sum of weights
        float m_SumOfWeights;
        //Sum of weights2
        float m_SumOfWeights2;

        std::set<unsigned int> m_TotalLumi;
        std::set<unsigned int> m_ProcessedLumi;
};

#endif //> !MUONPERFORMANCEALGS_MUONTPMETADATAALG_H
