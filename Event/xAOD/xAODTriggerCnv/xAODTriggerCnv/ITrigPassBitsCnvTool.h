// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGGERCNV_ITRIGPASSBITSCNVTOOL_H
#define XAODTRIGGERCNV_ITRIGPASSBITSCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigger/TrigPassBitsContainer.h"

// Forward declaration(s):
class TrigPassBitsCollection;

namespace xAODMaker {

   /// The interface provided by ITrigPassBitsCnvTool
   static const InterfaceID
   IID_ITrigPassBitsCnvTool( "xAODMaker::ITrigPassBitsCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating TrigPassBitsContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         TrigPassBits objects from an existing AOD TrigPassBits collection into
    *         an xAOD::TrigPassBitsContainer.
    *
    * @author Oleg Kuprash <oleg.kuprash@cern.ch>
    *
    * $Revision$
    * $Date$
    */
   class ITrigPassBitsCnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::TrigPassBitsContainer
      virtual StatusCode convert( const TrigPassBitsCollection* aod,
                                  xAOD::TrigPassBitsContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigPassBitsCnvTool;
      }

   }; // class ITrigPassBitsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_ITRIGPASSBITSCNVTOOL_H
