/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "AsgMessaging/MessageCheck.h"
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

ANA_MSG_HEADER (msgMuonEfficiency)

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
                if (!m_H) return DBL_MAX;
                return m_H->GetBinContent(bin);
            }
            
            void SetBinContent(int bin, float val) const;
            
            inline  double GetBinError(int bin) const{
                if (!m_H) return DBL_MAX;
                return m_H->GetBinError(bin);
            }
            
            inline TH1* GetHist() const{
                return m_H.get();
            }

            void SetBinError(int bin, float val) const;
            
            ///Function that changes from Implementation to implementation
            virtual CorrectionCode FindBin(const xAOD::Muon & muon, int & bin) const = 0;
            /// Return the total number of bins in the histogram -> (GetNbins() +2)**n
            virtual int nBins() const = 0;
            /// Return the number of overflow bins 
            virtual int nOverFlowBins() const = 0;
            /// States whether a bin is overflow or not
            virtual bool isOverFlowBin(int b) const =0;
            /// Translates the bin number into the borders and return them as name
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

             int nBins() const override;
             
             int nOverFlowBins() const override;
             bool isOverFlowBin(int b) const override;


             std::string GetBinName(unsigned int bin) const override;
             CorrectionCode FindBin(const xAOD::Muon & muon, int & bin) const override;
        private:
            std::unique_ptr<AxisHandler> m_x_handler;
    };

    class HistHandler_TH2: public HistHandler {

        public:
            /// The HistHandler TH2 handles 2D histograms which are not TH2Poly, since TH2 inhertis from TH1,
            /// a TH1* object is parsed avoiding the dynamic cast. Via the GetDimension() method of the TH1 
            /// the class ensures that the histogram has actual dimension 2. No specific TH2 method is required 
            /// in all considered usecases of TH1.
            HistHandler_TH2(TH1* hist);
            HistHandler_TH2(const HistHandler_TH2 & other);
            virtual HistHandler_TH2 & operator =(const HistHandler_TH2 & other);
            virtual ~HistHandler_TH2();

             int nBins() const override;
           
             int nOverFlowBins() const override;
             bool isOverFlowBin(int b) const override;
           
             std::string GetBinName(unsigned int bin) const override;

             CorrectionCode FindBin(const xAOD::Muon & muon, int & bin) const override;
        private:
            std::unique_ptr<AxisHandler> m_x_handler;
            std::unique_ptr<AxisHandler> m_y_handler;
    };

    class HistHandler_TH3: public HistHandler {

        public:
            /// The HistHandler TH3 handles 3D histograms, since TH3 inhertis from TH1,a TH1* object is parsed avoiding the dynamic cast. 
            /// Via the GetDimension() method of the TH1 the class ensures that the histogram has actual dimension 3. No specific TH3 method
            /// is required in all use-caes of the class.
            
            HistHandler_TH3(TH1* hist);
            HistHandler_TH3(const HistHandler_TH3 & other);
            virtual HistHandler_TH3 & operator =(const HistHandler_TH3 & other);
            virtual ~HistHandler_TH3();

             int nBins() const override;
             
             int nOverFlowBins() const override;
             bool isOverFlowBin(int b) const override;


             std::string GetBinName(unsigned int bin) const override;

             CorrectionCode FindBin(const xAOD::Muon & muon, int & bin) const override;

        private:
            std::unique_ptr<AxisHandler> m_x_handler;
            std::unique_ptr<AxisHandler> m_y_handler;
            std::unique_ptr<AxisHandler> m_z_handler;
    };

    class HistHandler_TH2Poly: public HistHandler {

        public:
            /// The HistHandler_TH2Poly handles the TH2Poly histograms of the scale-factor maps. Since the bin-finding,
            /// algoritihms of the TH2Poly differ from the ones of TH1, TH2 & TH3 explicit methods of TH2Poly are needed
            /// to pick-up the proper bin.
            HistHandler_TH2Poly(TH2Poly* hist);
            HistHandler_TH2Poly(const HistHandler_TH2Poly & other);
            virtual HistHandler_TH2Poly & operator =(const HistHandler_TH2Poly & other);
            virtual ~HistHandler_TH2Poly();

             int nOverFlowBins() const override;
             bool isOverFlowBin(int b) const override;

             int nBins() const override;
             std::string GetBinName(unsigned int bin) const override;
             CorrectionCode FindBin(const xAOD::Muon & muon, int & bin) const override;

        private:
            TH2Poly* m_h;
            std::unique_ptr<AxisHandler> m_x_handler;
            std::unique_ptr<AxisHandler> m_y_handler;
    };

    class AxisHandler {
        public:
            virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) const =0;
            virtual ~AxisHandler()= default;
    };
    class AxisHandlerProvider {
        public:
            static std::unique_ptr<AxisHandler> GetAxisHandler(const TAxis* axis);
    };

    class PtAxisHandler: public AxisHandler {
        public:
            CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) const override {
                value = mu.pt() / 1000.;
                return CorrectionCode::Ok;
            }
            virtual ~PtAxisHandler() = default;

    };

    class ChargeAxisHandler: public AxisHandler {
        public:
            CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) const override {
                value = mu.charge();
                return CorrectionCode::Ok;
            }
            virtual ~ChargeAxisHandler() = default;

    };
    
    class EtaAxisHandler: public AxisHandler {
        public:
            CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) const override {
                value = mu.eta();
                return CorrectionCode::Ok;
            }
            virtual ~EtaAxisHandler() = default;

    };
    class AbsEtaAxisHandler: public AxisHandler {
        public:
            CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) const override {
                value = fabs(mu.eta());
                return CorrectionCode::Ok;
            }
            virtual ~AbsEtaAxisHandler() = default;

    };
    class PhiAxisHandler: public AxisHandler {
        public:
            CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) const override {
                value = mu.phi();
                return CorrectionCode::Ok;
            }
            virtual ~PhiAxisHandler() = default;

    };
    class dRJetAxisHandler: public AxisHandler {
        public:
            CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value) const override {
                static const SG::AuxElement::ConstAccessor<float> dRJet("dRJet");
                value = dRJet.isAvailable(mu) ? dRJet(mu) : -2;
		// We want these warnings to be printed few times per job, so that they're visible, then stop before log file's size blows up 
                static std::atomic<unsigned int> warned = {0};
                if (warned<5 && !dRJet.isAvailable(mu)){
                    using namespace msgMuonEfficiency;
                    ANA_MSG_WARNING("The dRJet decoration has not been found for the Muon. Isolation scale-factors are now also binned in #Delta R(jet,#mu)");
                    ANA_MSG_WARNING("using the closest calibrated AntiKt4EMTopo jet with p_{T}>20~GeV and surving the standard OR criteria.");
                    ANA_MSG_WARNING("You should decorate your muon appropiately before passing to the tool, and use dRJet = -1 in case there is no jet in an event.");
                    ANA_MSG_WARNING("For the time being the inclusive scale-factor is going to be returned.");
		    ANA_MSG_WARNING("In future derivations, muons will also be decorated centrally with dRJet, for your benefit.");
		    warned++;
                }
                return CorrectionCode::Ok;
            }
            virtual ~dRJetAxisHandler() = default;

    };
    class UndefinedAxisHandler: public AxisHandler {
        public:
            CorrectionCode GetBinningParameter(const xAOD::Muon &, float &) const override {
                return CorrectionCode::Error;
            }
            virtual ~UndefinedAxisHandler() = default;

    };

} // namespace CP
#endif
