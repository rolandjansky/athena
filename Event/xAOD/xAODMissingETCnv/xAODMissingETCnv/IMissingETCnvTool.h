// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IMissingETCnvTool.h 586557 2014-03-06 17:56:05Z krasznaa $
#ifndef XAODMISSINGETCNV_IMISSINGETCNVTOOL_H
#define XAODMISSINGETCNV_IMISSINGETCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODMissingET/MissingETContainer.h"

// Forward declaration(s):
class MissingET;

namespace xAODMaker {

   /// The interface provided by IMissingETCnvTool
   static const InterfaceID
   IID_IMissingETCnvTool( "xAODMaker::IMissingETCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool translating AOD MET objects to xAOD ones
    *
    *         Documentation to be written...
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 586557 $
    * $Date: 2014-03-06 18:56:05 +0100 (Thu, 06 Mar 2014) $
    */
   class IMissingETCnvTool : public virtual IAlgTool {

   public:
      /// Function doing the translation
      virtual StatusCode add( const MissingET* aod,
                              xAOD::MissingETContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IMissingETCnvTool;
      }

   }; // class IMissingETCnvTool

} // namespace xAODMaker

#endif // XAODMISSINGETCNV_IMISSINGETCNVTOOL_H
