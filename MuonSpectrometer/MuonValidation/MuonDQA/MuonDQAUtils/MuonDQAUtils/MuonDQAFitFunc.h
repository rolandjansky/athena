/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Muon_MuonDQAFitFunc_H
#define Muon_MuonDQAFitFunc_H

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "TH1F.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"

static const InterfaceID IID_MuonDQAFitFunc("Muon::MuonDQAFitFunc",1,0);

namespace Muon {
  /**
     @brief Tool to fit histograms.
     The tool can be used to:
     - fit a Z-->mumu mass window 
     *  @author Nektarios Benekos Nectarios.Benekos@cern.ch
     */

  class MuonDQAFitFunc: public AthAlgTool {
  public:
 
    /** @brief constructor */
    MuonDQAFitFunc(const std::string& type, const std::string& name, const IInterface* parent);
    //const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonDQAFitFunc();
   
    /** @brief AlgTool initilize */
    StatusCode initialize();

    /** @brief access to tool interface */
    static const InterfaceID& interfaceID() { return IID_MuonDQAFitFunc; }
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
     
    void ZmumuFitHistograms(TH1F* hmass, TH1F* hwidth, TH1F* h1[], int nbins); 
    
    void MuonEffHisto1D(TH1F* h_Num, TH1F* h_Denom, TProfile* h_Eff) const;    
    void MuonEffHisto2D(TH2F* h_Num, TH2F* h_Denom, TProfile2D* h_Eff) const;

    /**================================================================
       Establishes a minimim window for the TProfile 
       ================================================================*/  
    void MinWindow1Set_from_TProf(TProfile* hProf, float windowMin, float windowMax) const;
    void MinWindow2Set_from_TProf(TProfile2D* hProf, float windowMin, float windowMax) const;
    
    void FillMeanRmsProj(TH2F*, TH1F*, int) const;
    //void FillRMSFromProfile(TProfile*, TProfile*) const;
    
    /**  Fills a histogram with the RMS values of a TProfile */
    void FillRMSFromProfile(TProfile*, TH1F*) const;
    
    /** Fills a histogram with the RMS values of a TProfile, which labeled bins->keeping the labeling for the bins */
    void FillRMSFromCharProfile(TProfile*, TH1F*) const;
    void FillGausMeanOrWidth(TH2F*, TH1F*, float, float, int) const;
    void FillGaussMeanAndWidth(TH2F*, TH1F*, float, float) const;
    void FillGaussMeanAndWidth(TH1F*, TH1F*, int, float, float) const;
    
  protected:

    const static int nbins=6;
    float m_minMuonEffWindow;
    float m_maxMuonEffWindow;

  private:

    //StoreGateSvc* m_storeGate;
    std::string m_stream;
    double m_ZmumuPDGmass;  
    mutable MsgStream  m_log; //!< message log 
  };
} 
#endif
