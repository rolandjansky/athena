// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigT2JetFillerTool.h 486968 2012-03-06 20:40:16Z chapleau $
#ifndef __D3PD_TrigT2JetFillerTool_h__
#define __D3PD_TrigT2JetFillerTool_h__
/*
  D3PD::TrigT2JetFillerToo.h
*/
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigCaloEvent/TrigT2Jet.h"

namespace D3PD {

   /**
    * @short Tool saving the properties of TrigT2Jet objects
    *
    * @author John Morris <john.morris@cern.ch>
    *         17/5/2010
    *
    */
   class TrigT2JetFillerTool : public BlockFillerTool< TrigT2Jet > {

   public:
      /// Regular AlgTool constructor
      TrigT2JetFillerTool( const std::string& type, const std::string& name, 
                              const IInterface* parent);

      /// Function booking the variables
      StatusCode book();
      /// Function filling the variables
      StatusCode fill( const TrigT2Jet& x );

   private:
      // Properties
      bool m_writeBasics; ///< Write basic information about the object
      bool m_writeDetails; ///< Write detailed information
      bool m_writeCleaning; ///< Write cleaning variables

      // D3PD variables
     /** Internal RoI index to which this object belongs */
     uint32_t* m_roiWord;
     /** Hadronic Jet Energy before calibration */
     double *m_ehad0;
     /** Electromagnetic Jet Energy before calibration */
     double *m_eem0;
     /** number of leading cells */
     int *m_nLeadingCells;
     /** Fraction of energy in HEC */
     float *m_hecf;
     /** fraction of jet energy from bad cells */
     float *m_jetQuality;
     /** Fraction of energy in EMC*/
     float *m_emf;
     /** Jet time */
     float *m_jetTimeCells;

     // input type
     std::string * m_inputType;
     // output type
     std::string * m_outputType; 

     //counter
     int * m_counter;

   }; // class TrigT2JetFillerTool

} // namespace D3PD

#endif // __D3PD_TrigT2JetFillerTool_h__

