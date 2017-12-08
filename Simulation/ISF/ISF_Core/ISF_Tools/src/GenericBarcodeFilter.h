/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericBarcodeFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_GENERICBARCODEFILTER_H
#define ISF_TOOLS_GENERICBARCODEFILTER_H 1

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_Interfaces/IParticleFilter.h"
// Barcode Includes
#include "BarcodeEvent/Barcode.h"

namespace ISF {

    
  /** @class GenericBarcodeFilter
  
      A generic ISFParticle filter using the particle barcode to make it's filter decisions:

      *) filter out all particles with an unset barcode

      *) filter all but primary particles according to the MC12 barcode scheme: 
          A tool that only allows particles to pass, which are primary (or modified primary) particles,
          according to the MC12 barcode numbering scheme.

      @author Elmar.Ritsch -at- cern.ch
     */
  class GenericBarcodeFilter : public AthAlgTool,
                               public IParticleFilter { 
      
    public: 
      /** Constructor with parameters */
      GenericBarcodeFilter( const std::string& t, const std::string& n, const IInterface* p );

      /** Virtual destructor */
      virtual ~GenericBarcodeFilter();

      // Athena algtool's Hooks
      StatusCode  initialize();
      StatusCode  finalize();
       
      /** Returns a pass boolean on the particle  */
      virtual bool passFilter(const ISFParticle& isp) const;
     
    private:
      bool                          m_filterOutUnsetBarcodes;        //!< turn unset barcode filtering on/off

      /** filter primary particles according to the barcode offsets/increments as used by MC12 */
      bool                          m_onlyLegacyPrimaries;           //!< turn legacy primary filter on/off
      Barcode::ParticleBarcode      m_legacyPartGenerationIncrement; //!< increment for updated particles
      Barcode::ParticleBarcode      m_legacyFirstSecondary;          //!< first secondary barcode produced by simulation

  }; 
  
}

#endif //> !ISF_TOOLS_GENERICBARCODEFILTER_H

