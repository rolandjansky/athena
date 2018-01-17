/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EntryLayerFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_ENTRYLAYERFILTER_H
#define ISF_TOOLS_ENTRYLAYERFILTER_H 1

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_Interfaces/IParticleFilter.h"
// Barcode Includes
#include "BarcodeEvent/Barcode.h"

namespace ISF {

    
  /** @class EntryLayerFilter
  
      A generic ISFParticle filter using the particle barcode to make it's filter decisions:

      *) filter out all particles with an unset barcode

      *) filter all but primary particles according to the MC12 barcode scheme: 
          A tool that only allows particles to pass, which are primary (or modified primary) particles,
          according to the MC12 barcode numbering scheme.

      @author Elmar.Ritsch -at- cern.ch
     */
  class EntryLayerFilter : public extends<AthAlgTool, IParticleFilter> { 
      
    public: 
      /** Constructor with parameters */
      EntryLayerFilter( const std::string& t, const std::string& n, const IInterface* p );

      /** Virtual destructor */
      virtual ~EntryLayerFilter();

      // Athena algtool's Hooks
      StatusCode  initialize();
      StatusCode  finalize();
       
      /** Returns a pass boolean on the particle  */
      virtual bool passFilter(const ISFParticle& isp) const;
     
    private:
      double                        m_ekinCharged;     //!< mininum Ekin cut for charged particles
      double                        m_ekinNeutral;     //!< mininum Ekin cut for neutral particles
  }; 
  
}

#endif //> !ISF_TOOLS_ENTRYLAYERFILTER_H

