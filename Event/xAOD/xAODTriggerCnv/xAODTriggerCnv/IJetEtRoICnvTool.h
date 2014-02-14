// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IJetEtRoICnvTool.h 575635 2013-12-16 10:58:41Z krasznaa $
#ifndef XAODTRIGGERCNV_IJETETROICNVTOOL_H
#define XAODTRIGGERCNV_IJETETROICNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigger/JetEtRoI.h"

// Forward declaration(s):
class LVL1_ROI;

namespace xAODMaker {

   /// The interface provided by IJetEtRoICnvTool
   static const InterfaceID
   IID_IJetEtRoICnvTool( "xAODMaker::IJetEtRoICnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating JetEtRoI from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         jet-Et RoI object from an existing AOD LVL1_ROI container into
    *         an xAOD::JetEtRoI.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 575635 $
    * $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $
    */
   class IJetEtRoICnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::JetEtRoIContainer
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::JetEtRoI* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IJetEtRoICnvTool;
      }

   }; // class IJetEtRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_IJETETROICNVTOOL_H
