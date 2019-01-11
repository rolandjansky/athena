/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFICIENCYCORRECTIONS_HISTOHANDLER_H
#define MUONEFFICIENCYCORRECTIONS_HISTOHANDLER_H

// EDM include(s):
#include "xAODMuon/Muon.h"

// supported SF histogram types
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TH2Poly.h>

#include "PATInterfaces/CorrectionCode.h"
#include <MuonEfficiencyCorrections/UtilFunctions.h>
// further ROOT includes
#include <TFile.h>
#include <TDirectory.h>

// STL includes
#include <string>
#include <iostream>
#include <exception>
#include <map>
#include <memory>
#include <cmath>

namespace CP {
    
    ///Forward declarations
    class AxisHandler;
   
    
    class HistHandler {
            /// @class HistHandler
            /// @brief  utility class to avoid having to determine the input histo at every single
            /// call to the tool
            /// Remembers the type and axis binning of a histogram
            /// and provides a common interface independent of these
            /// details.
        public:

            inline  double GetBinContent(int bin) const{
            if (!m_H) {
            return DBL_MAX;
        }
        return m_H->GetBinContent(bin);
            }
             void SetBinContent(int bin, float val) const;
            
            inline  double GetBinError(int bin) const{
                if (!m_H) {
            return DBL_MAX;
        }
        return m_H->GetBinError(bin);
            }
            inline TH1* GetHist() const{
                return m_H.get();
            }

            void SetBinError(int bin, float val) const;
            
            //Function that changes from Implementation to implementation
            virtual CorrectionCode FindBin(const xAOD::Muon & muon, int & bin) const = 0;
            virtual int NBins() const = 0;
            virtual std::string GetBinName(unsigned int bin) const=0;

            virtual ~HistHandler();
        protected:
            HistHandler(TH1* Hist);
            HistHandler(const HistHandler & other);
            void Copy(const HistHandler & other);
        private:
            std::unique_ptr<TH1> m_H;

    };

    

    class HistHandler_TH1: public HistHandler {

        public:

            HistHandler_TH1(TH1* hist);
            HistHandler_TH1(const HistHandler_TH1 & other);
            virtual HistHandler_TH1 & operator =(const HistHandler_TH1 & other);
            virtual ~HistHandler_TH1();

            virtual int NBins() const;
            virtual std::string GetBinName(unsigned int bin) const;
            virtual CorrectionCode FindBin(const xAOD::Muon & muon, int & bin) const;
        private:
            std::unique_ptr<AxisHandler> m_x_handler;
    };

    class HistHandler_TH2: public HistHandler {

        public:

            HistHandler_TH2(TH1* hist);
            HistHandler_TH2(const HistHandler_TH2 & other);
            virtual HistHandler_TH2 & operator =(const HistHandler_TH2 & other);
            virtual ~HistHandler_TH2();

            virtual int NBins() const;
            virtual std::string GetBinName(unsigned int bin) const;

            virtual CorrectionCode FindBin(const xAOD::Muon & muon, int & bin) const;
        private:
            std::unique_ptr<AxisHandler> m_x_handler;
            std::unique_ptr<AxisHandler> m_y_handler;
    };

    class HistHandler_TH3: public HistHandler {

        public:

            HistHandler_TH3(TH1* hist);
            HistHandler_TH3(const HistHandler_TH3 & other);
            virtual HistHandler_TH3 & operator =(const HistHandler_TH3 & other);
            virtual ~HistHandler_TH3();

            virtual int NBins() const;
            virtual std::string GetBinName(unsigned int bin) const;

            virtual CorrectionCode FindBin(const xAOD::Muon & muon, int & bin) const;

        private:
            std::unique_ptr<AxisHandler> m_x_handler;
            std::unique_ptr<AxisHandler> m_y_handler;
            std::unique_ptr<AxisHandler> m_z_handler;
    };

    class HistHandler_TH2Poly: public HistHandler {

        public:

            HistHandler_TH2Poly(TH2Poly* hist);
            HistHandler_TH2Poly(const HistHandler_TH2Poly & other);
            virtual HistHandler_TH2Poly & operator =(const HistHandler_TH2Poly & other);
            virtual ~HistHandler_TH2Poly();

            virtual int NBins() const;
            virtual std::string GetBinName(unsigned int bin) const;

            virtual CorrectionCode FindBin(const xAOD::Muon & muon, int & bin) const;

        private:
            TH2Poly* m_h;
            std::unique_ptr<AxisHandler> m_x_handler;
            std::unique_ptr<AxisHandler> m_y_handler;
    };

    class AxisHandler {
        public:
            virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value)=0;
            virtual ~AxisHandler() {
            }
    };
    class AxisHandlerProvider {
        public:
            static std::unique_ptr<AxisHandler> GetAxisHandler(const TAxis* axis);
    };

    class PtAxisHandler: public AxisHandler {
        public:
            virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) {
                value = mu.pt() / 1000.;
                return CorrectionCode::Ok;
            }
            virtual ~PtAxisHandler() {
            }

    };

    class ChargeAxisHandler: public AxisHandler {
        public:
            virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) {
                value = mu.charge();
                return CorrectionCode::Ok;
            }
            virtual ~ChargeAxisHandler() {
            }

    };
    
    class EtaAxisHandler: public AxisHandler {
        public:
            virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) {
                value = mu.eta();
                return CorrectionCode::Ok;
            }
            virtual ~EtaAxisHandler() {
            }

    };
    class AbsEtaAxisHandler: public AxisHandler {
        public:
            virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) {
                value = fabs(mu.eta());
                return CorrectionCode::Ok;
            }
            virtual ~AbsEtaAxisHandler() {
            }

    };
    class PhiAxisHandler: public AxisHandler {
        public:
            virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) {
                value = mu.phi();
                return CorrectionCode::Ok;
            }
            virtual ~PhiAxisHandler() {
            }

    };
    class dRJetAxisHandler: public AxisHandler {
        public:
            virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) {
                static SG::AuxElement::ConstAccessor<float> dRJet("dRJet");
                value = dRJet.isAvailable(mu) ? dRJet(mu) : -1;
                return CorrectionCode::Ok;
            }
            virtual ~dRJetAxisHandler() {
            }

    };
    class UndefinedAxisHandler: public AxisHandler {
        public:
            virtual CorrectionCode GetBinningParameter(const xAOD::Muon &, float &) {
                return CorrectionCode::Error;
            }
            virtual ~UndefinedAxisHandler() {
            }

    };

} // namespace CP
#endif
