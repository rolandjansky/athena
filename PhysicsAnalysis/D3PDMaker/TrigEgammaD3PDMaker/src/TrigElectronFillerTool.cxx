/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 /*
  TrigElectronFillerTool.cxx
*/
#include "TrigElectronFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {

   TrigElectronFillerTool::TrigElectronFillerTool( const std::string& type, 
                                                   const std::string& name, 
                                                   const IInterface* parent )
      : BlockFillerTool< TrigElectron >( type, name, parent )
   {
      // Avoid coverity warnings.
      m_writeBasics = true;
      m_writeHypoVars = true;
      m_writeDetails = true;
      book().ignore();

      declareProperty( "WriteBasics", m_writeBasics = false, 
                       "Write basic information" );
      declareProperty( "WriteHypoVars", m_writeHypoVars = false, 
                       "Write variables used in hypo" );
      declareProperty("WriteDetails", m_writeDetails = false, 
                      "Write detailed variables" );
   }

   StatusCode TrigElectronFillerTool::book() {

      if( m_writeBasics ) {
         CHECK(addVariable("RoIWord", m_roiWord));
         CHECK(addVariable("zvertex", m_Zvtx));
         CHECK(addVariable("charge",  m_charge));
      }
      if( m_writeHypoVars ) {
         CHECK(addVariable("trackAlgo",        m_trackAlgo));
         CHECK(addVariable("TRTHighTHitsRatio",m_nTRTHiTHitsRatio));
         CHECK(addVariable("deltaeta1",        m_DeltaEta1));
         CHECK(addVariable("deltaphi2",        m_DeltaPhi2));
         CHECK(addVariable("EtOverPt",         m_EtOverPt));
         CHECK(addVariable("reta",             m_Reta));
         CHECK(addVariable("Eratio",           m_Eratio));
         CHECK(addVariable("Ethad1",           m_Ethad1));
         CHECK(addVariable("nTRTHits",            m_nTRTHits));
         CHECK(addVariable("nTRTHighTHits",       m_nTRTHiThresholdHits));
         CHECK(addVariable("trackIndx",           m_trackIndx));
         CHECK(addVariable("trkPt",               m_trkPt));
         CHECK(addVariable("trkEtaAtCalo",        m_trkEtaAtCalo));
         CHECK(addVariable("trkPhiAtCalo",        m_trkPhiAtCalo));
         CHECK(addVariable("caloEta",             m_caloEta));
         CHECK(addVariable("caloPhi",             m_caloPhi));
         CHECK(addVariable("F1",                  m_F1));
      }
      if( m_writeDetails ) {
         CHECK(addVariable("err_trkPt",           m_err_trkPt));
         CHECK(addVariable("err_eta",             m_err_eta));
         CHECK(addVariable("err_phi",             m_err_phi));
         CHECK(addVariable("errz",                m_err_Zvtx));
         CHECK(addVariable("F0",                  m_F0));
         CHECK(addVariable("F2",                  m_F2));
         CHECK(addVariable("F3",                  m_F3));
      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigElectronFillerTool::fill( const TrigElectron& x ) {

      if( m_writeBasics ) {
         *m_roiWord = x.roiWord();
         *m_Zvtx = x.Zvtx();
         *m_charge = x.charge();
      }
      if( m_writeHypoVars ) {
         *m_trackAlgo = x.trackAlgo();
         *m_nTRTHiTHitsRatio = x.nTRTHiTHitsRatio();
         *m_DeltaEta1 = x.DeltaEta1();
         *m_DeltaPhi2 = x.DeltaPhi2();
         *m_EtOverPt = x.EtOverPt();
         *m_Reta = x.Reta();
         *m_Eratio = x.Eratio();
         *m_Ethad1 = x.Ethad1();
         *m_nTRTHits = x.nTRTHits();
         *m_nTRTHiThresholdHits = x.nTRTHiThresholdHits();
         *m_trackIndx = x.trackIndx();
         *m_trkPt = x.trkPt();
         *m_trkEtaAtCalo = x.trkEtaAtCalo();
         *m_trkPhiAtCalo = x.trkPhiAtCalo();
         *m_caloEta = x.caloEta();
         *m_caloPhi = x.caloPhi();
         *m_F1 = x.F1();
      }
      if( m_writeDetails ) {
         *m_err_trkPt = x.err_trkPt();
         *m_err_eta = x.err_eta();
         *m_err_phi = x.err_phi();
         *m_err_Zvtx = x.err_Zvtx();
         *m_F0 = x.F0();
         *m_F2 = x.F2();
         *m_F3 = x.F3();
      }

      return StatusCode::SUCCESS;
   }

}
