// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MDTSimHitFillerTool.h 499698 2012-05-08 16:02:28Z krasznaa $
#ifndef MUOND3PDMAKER_MDTSIMHITFILLERTOOL_H
#define MUOND3PDMAKER_MDTSIMHITFILLERTOOL_H

// EDM include(s):
#include "MuonSimEvent/MDTSimHit.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Filler tool for the basic properties of MDTSimHit
    *
    *         This tool can be used to save the simple properties of an
    *         MDTSimHit object.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 499698 $
    * $Date: 2012-05-08 18:02:28 +0200 (Tue, 08 May 2012) $
    */
   class MDTSimHitFillerTool : public BlockFillerTool< MDTSimHit > {

   public:
      /// Regular Gaudi AlgTool constructor
      MDTSimHitFillerTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Function booking the ntuple variables
      virtual StatusCode book();
      /// Function filling the ntuple variables for a single object
      virtual StatusCode fill( const MDTSimHit& obj );

   private:
      float* m_globalTime;
      float* m_driftRadius;
      float* m_stepLength;
      float* m_energyDeposit;
      int*   m_particleEncoding;
      float* m_kineticEnergy;
      int*   m_MDTid;
      int*   m_trackNumber; ///< Number of track which released this energy

   }; // class MDTSimHitFillerTool

} // namespace D3PD

#endif // MUOND3PDMAKER_MDTSIMHITFILLERTOOL_H
