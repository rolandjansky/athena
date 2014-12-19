/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigPhotonFillerTool.cxx
*/
#include "TrigPhotonFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {

   TrigPhotonFillerTool::TrigPhotonFillerTool( const std::string& type, 
                                               const std::string& name, 
                                               const IInterface* parent )
      : BlockFillerTool< TrigPhoton >( type, name, parent )
   {
      // Avoid coverity warnings.
      m_writeBasics = true;
      m_writeHypoVars = true;
      m_writeDetails = true;
      book().ignore();

      declareProperty( "WriteBasics", m_writeBasics = false, 
                       "Write basic information" );
      declareProperty("WriteHypoVars", m_writeHypoVars = false, 
                      "Write variables used in hypo" );
      declareProperty("WriteDetails", m_writeDetails = false, 
                      "Write detailed variables" );
   }

   StatusCode TrigPhotonFillerTool::book() {

      if( m_writeBasics ) {
         CHECK(addVariable("RoIWord", m_roiWord));
      }
      if( m_writeHypoVars ) {
         CHECK(addVariable("HadEt1", m_HadEt1));
         CHECK(addVariable("Eratio", m_Eratio));
         CHECK(addVariable("Reta",   m_Reta));
         CHECK(addVariable("dPhi",   m_dPhi));
         CHECK(addVariable("dEta",   m_dEta));
         CHECK(addVariable("F1",     m_F1));
      }
      if( m_writeDetails ) {
         CHECK(addVariable("Fside", m_Fside));
         CHECK(addVariable("Weta2", m_Weta2));
         CHECK(addVariable("F0",    m_F0));
         CHECK(addVariable("F2",    m_F2));
         CHECK(addVariable("F3",    m_F3));
      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigPhotonFillerTool::fill( const TrigPhoton& x ) {

      if( m_writeBasics ) {
         *m_roiWord = x.roiWord();
      }
      if( m_writeHypoVars ) {
         *m_HadEt1 = x.HadEt1();
         *m_Eratio = x.Eratio();
         *m_Reta = x.Reta();
         *m_dPhi = x.dPhi();
         *m_dEta = x.dEta();
         *m_F1 = x.F1();
      }
      if( m_writeDetails ) {
         *m_Fside = x.Fside();
         *m_Weta2 = x.Weta2();
         *m_F0 = x.F0();
         *m_F2 = x.F2();
         *m_F3 = x.F3();
      }

      return StatusCode::SUCCESS;
   }

}
