// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IEmTauRoICnvTool.h 575635 2013-12-16 10:58:41Z krasznaa $
#ifndef XAODTRIGGERCNV_IEMTAUROICNVTOOL_H
#define XAODTRIGGERCNV_IEMTAUROICNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigger/EmTauRoIContainer.h"

// Forward declaration(s):
class LVL1_ROI;

namespace xAODMaker {

   /// The interface provided by IEmTauRoICnvTool
   static const InterfaceID
   IID_IEmTauRoICnvTool( "xAODMaker::IEmTauRoICnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating EmTauRoIContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         em/tau RoI objects from an existing AOD LVL1_ROI container into
    *         an xAOD::EmTauRoIContainer.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 575635 $
    * $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $
    */
   class IEmTauRoICnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::EmTauRoIContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IEmTauRoICnvTool;
      }

   }; // class IEmTauRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_IEMTAUROICNVTOOL_H
