/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigT2JetFillerTool.cxx
*/
#include "TrigT2JetFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "TrigT2CaloJet/T2L1Tools.h"

namespace D3PD {

   TrigT2JetFillerTool::TrigT2JetFillerTool( const std::string& type, 
                                                   const std::string& name, 
                                                   const IInterface* parent )
      : BlockFillerTool< TrigT2Jet >( type, name, parent )
   {
      // Avoid coverity warnings.
      m_writeBasics = true;
      m_writeDetails = true;
      m_writeCleaning = true;
      book().ignore();

      declareProperty( "WriteBasics", m_writeBasics = false, 
                       "Write basic information" );
      declareProperty("WriteDetails", m_writeDetails = false, 
                      "Write detailed variables" );
      declareProperty("WriteCleaning", m_writeCleaning = false,
                      "Write cleaning variables" );
   }

   StatusCode TrigT2JetFillerTool::book() {

      if( m_writeBasics ) {
         CHECK(addVariable("RoIWord", m_roiWord));
         CHECK(addVariable("InputType", m_inputType));
         CHECK(addVariable("OutputType", m_outputType));
         CHECK(addVariable("counter", m_counter));
      }

      if( m_writeDetails ) {
         CHECK(addVariable("ehad0",         m_ehad0));
         CHECK(addVariable("eem0",          m_eem0));
      }
      if( m_writeCleaning ){
          CHECK(addVariable("nLeadingCells", m_nLeadingCells));
          CHECK(addVariable("hecf",          m_hecf));
          CHECK(addVariable("jetQuality",    m_jetQuality));
          CHECK(addVariable("emf",           m_emf));
          CHECK(addVariable("jetTimeCells",  m_jetTimeCells));
      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigT2JetFillerTool::fill( const TrigT2Jet& x ) {

      if( m_writeBasics ) {
         *m_roiWord = x.RoIword();
         
         *m_inputType = "";
         *m_outputType = "";
         *m_counter = -1;
         T2L1Tools m_t2l1_tools;
         m_t2l1_tools.RoIWordToStrings(x.RoIword(), *m_inputType, *m_outputType, *m_counter);
       
      }
      if( m_writeDetails ) {
         *m_ehad0 = x.ehad0();
         *m_eem0  = x.eem0();
      }
      if( m_writeCleaning ){
         *m_nLeadingCells = x.nLeadingCells();
         *m_hecf = x.hecf();
         *m_jetQuality = x.jetQuality();
         *m_emf = x.emf();
         *m_jetTimeCells = x.jetTimeCells();
      }

      return StatusCode::SUCCESS;
   }

}
