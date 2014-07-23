/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HISTOHANDLERFORMCPTOOL
#define HISTOHANDLERFORMCPTOOL

// EDM include(s):
#include "xAODMuon/Muon.h"
// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

// supported SF histogram types
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TH2Poly.h>

#include "PATInterfaces/CorrectionCode.h"
#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"

// further ROOT includes
#include <TFile.h>
#include <TDirectory.h>

// STL includes
#include <string>
#include <iostream>
#include <exception>
#include <map>
#include <cmath>


namespace CP{
    class HistHandler{
        /// @class HistHandler
        /// @brief  utility class to avoid having to determine the input histo at every single
        /// call to the tool
        /// Remembers the type and axis binning of a histogram
        /// and provides a common interface independent of these
        /// details.
    public:
        
        virtual double GetBinContent (int bin) = 0;
        virtual void SetBinContent (int bin, float val) = 0;
        virtual double GetBinError (int bin) = 0;
        virtual void SetBinError (int bin, float val) = 0;
        virtual TH1* GetHist() = 0;
        
        virtual int NBins() = 0;
        virtual CorrectionCode FindBin (const xAOD::Muon & muon, int & bin) = 0;
        virtual ~HistHandler(){};
    };

    class AxisHandler {
    public:
        
        virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value)=0;
        
        virtual ~AxisHandler(){};
    };

    class HistHandler_TH1F: public HistHandler{
        
    public:
        
        HistHandler_TH1F(TH1F* hist);
        virtual ~HistHandler_TH1F();
        virtual double GetBinContent (int bin){
            return m_h->GetBinContent(bin);
        }
        virtual void SetBinContent (int bin, float val){
            m_h->SetBinContent(bin,val);
        }
        virtual double GetBinError (int bin){
            return m_h->GetBinError(bin);
        }
        virtual void SetBinError (int bin, float val){
            m_h->SetBinError(bin,val);
        }
        virtual TH1* GetHist(){
            return m_h;
        }
        
        virtual int NBins();
        virtual CorrectionCode FindBin (const xAOD::Muon & muon, int & bin);
        
    private:
        TH1F* m_h;
        AxisHandler *m_x_handler;
    };

    class HistHandler_TH2F: public HistHandler{
        
    public:
        
        HistHandler_TH2F(TH2F* hist);
        virtual ~HistHandler_TH2F();
        virtual double GetBinContent (int bin){
            return m_h->GetBinContent(bin);
        }
        virtual void SetBinContent (int bin, float val){
            m_h->SetBinContent(bin,val);
        }
        virtual double GetBinError (int bin){
            return m_h->GetBinError(bin);
        }
        virtual void SetBinError (int bin, float val){
            m_h->SetBinError(bin,val);
        }
        virtual TH1* GetHist(){
            return m_h;
        }
        
        virtual int NBins();
        virtual CorrectionCode FindBin (const xAOD::Muon & muon, int & bin);
        
    private:
        TH2F* m_h;
        AxisHandler *m_x_handler;
        AxisHandler *m_y_handler;
    };

    class HistHandler_TH3F: public HistHandler{
        
    public:
        
        HistHandler_TH3F(TH3F* hist);
        virtual ~HistHandler_TH3F();
        virtual double GetBinContent (int bin){
            return m_h->GetBinContent(bin);
        }
        virtual void SetBinContent (int bin, float val){
            m_h->SetBinContent(bin,val);
        }
        virtual double GetBinError (int bin){
            return m_h->GetBinError(bin);
        }
        virtual void SetBinError (int bin, float val){
            m_h->SetBinError(bin,val);
        }
        virtual TH1* GetHist(){
            return m_h;
        }
        
        virtual int NBins();
        virtual CorrectionCode FindBin (const xAOD::Muon & muon, int & bin);
        
    private:
        TH3F* m_h;
        AxisHandler *m_x_handler;
        AxisHandler *m_y_handler;
        AxisHandler *m_z_handler;
    };

    class HistHandler_TH2Poly: public HistHandler{
        
    public:
        
        HistHandler_TH2Poly(TH2Poly* hist);
        virtual ~HistHandler_TH2Poly();
        virtual double GetBinContent (int bin){
            return m_h->GetBinContent(bin);
        }
        virtual void SetBinContent (int bin, float val){
            m_h->SetBinContent(bin,val);
        }
        virtual double GetBinError (int bin){
            return m_h->GetBinError(bin);
        }
        virtual void SetBinError (int bin, float val){
            m_h->SetBinError(bin,val);
        }
        virtual TH1* GetHist(){
            return m_h;
        }
        
        virtual int NBins();
        virtual CorrectionCode FindBin (const xAOD::Muon & muon, int & bin);
        
    private:
        TH2Poly* m_h;
        AxisHandler *m_x_handler;
        AxisHandler *m_y_handler;
    };



    class PtAxisHandler: public AxisHandler {
    public:
        virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value){
            value =   mu.pt() / 1000.;
            return CorrectionCode::Ok;
        }
        virtual ~PtAxisHandler(){
        };
    };

    class ChargeAxisHandler: public AxisHandler {
    public:
        virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value){
            value =   mu.primaryTrackParticle()->charge();
            return CorrectionCode::Ok;
        }
        virtual ~ChargeAxisHandler(){
        };
    };

    class FineEtaPhiAxisHandler: public AxisHandler {
    public:
        virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value){
            value =   m_fepb.bin(mu.p4());
            return CorrectionCode::Ok;
        }
        virtual ~FineEtaPhiAxisHandler(){
        };
    private:
        fineEtaPhiBinning m_fepb;
    };
    class EtaAxisHandler: public AxisHandler {
    public:
        virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value){
            value =   mu.eta();
            return CorrectionCode::Ok;
        }
        virtual ~EtaAxisHandler(){
        };
    };
    class PhiAxisHandler: public AxisHandler {
    public:
        virtual CorrectionCode GetBinningParameter(const xAOD::Muon & mu, float & value){
            value =  mu.phi();
            return CorrectionCode::Ok;
        }
        virtual ~PhiAxisHandler(){
        };
    };
    class UndefinedAxisHandler: public AxisHandler {
    public:
        virtual CorrectionCode GetBinningParameter(const xAOD::Muon & , float & ){
            return CorrectionCode::Error;
        }
        virtual ~UndefinedAxisHandler(){
        };
    };
    class AxisHandlerProvider{
        public:
            static AxisHandler *GetAxisHandler (std::string axis);
    };
    
}// namespace CP
#endif
