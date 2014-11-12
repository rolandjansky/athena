// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IRingerRingsCnvTool.h  $
#ifndef XAODTRIGRINGERCNV_ITRIGRINGERRINGSCNVTOOL_H
#define XAODTRIGRINGERCNV_ITRIGRINGERRINGSCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRingerRingsContainer.h"

// Forward declaration(s):
class RingerRingsContainer;

namespace xAODMaker {

   /// The interface provided by IRingerRingsCnvTool
   static const InterfaceID
   IID_ITrigRingerRingsCnvTool( "xAODMaker::ITrigRingerRingsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating RingerRings from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         RingerRings object from an existing AOD RingerRings container into
    *         an xAOD::RingerRings.
    *
    * @author joao victor da fonseca pinto <joao.victor.da.fonseca.pinto@cern.ch>
    *
    * $Revision:  $
    * $Date: $
    */
   class ITrigRingerRingsCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::RingerRingsContainer
      virtual StatusCode convert( const RingerRingsContainer* aod,
				  xAOD::TrigRingerRingsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigRingerRingsCnvTool;
      }

   }; // class ITrigRingerRingsCnvTool

} // namespace xAODMaker

#endif // XAODRINGERRINGSCNV_IRINGERRINGSCNVTOOL_H
