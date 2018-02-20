// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIG_TESTTRIGSF_H
#define TRIG_TESTTRIGSF_H

//
// Algorithm to test trigger SF
//

// C/C++
#include <set>

// ROOT
#include "TStopwatch.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"

// Local
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"

class TH1;
class TH2;

namespace Trig
{
    class Object;

    class TestTrigSF: virtual public AthAlgorithm
    {
        public:

            TestTrigSF(const std::string& name, ISvcLocator* pSvcLocator);
            virtual ~TestTrigSF() {
            }

            StatusCode initialize();
            StatusCode execute();
            StatusCode finalize();

        private:

            typedef std::map<std::string, std::string> L1Map;

        private:

            void RegHist(TH1 *h, const std::string &key);

            void CheckSF(const xAOD::Muon *ptr);

            void CheckSyst(const xAOD::Muon *ptr, const std::string &syst_name, int step, TH2 *h);

            void FillHist(TH2 *h, const xAOD::Muon *ptr, double val);

        private:

            // Properties:
            std::string m_outputStream;
            std::string m_inputContainerName;
            std::string m_triggerName;

            // Tools and services:
            ServiceHandle<ITHistSvc> m_histSvc;
            ToolHandle<CP::IMuonTriggerScaleFactors> m_trigEff;

            // Variables:
            TStopwatch m_timerEvent; // Total job timer
            TStopwatch m_timerTotal; // Total job timer

            int m_countEvent;

            L1Map m_mapL1;

            TH2 *m_barrel_sf;
            TH2 *m_endcap_sf;

            TH2 *m_barrel_sf_err_stat_up;
            TH2 *m_barrel_sf_err_stat_dw;

            TH2 *m_endcap_sf_err_stat_up;
            TH2 *m_endcap_sf_err_stat_dw;

            TH2 *m_barrel_sf_err_syst_up;
            TH2 *m_barrel_sf_err_syst_dw;

            TH2 *m_endcap_sf_err_syst_up;
            TH2 *m_endcap_sf_err_syst_dw;
    };
}

#endif
