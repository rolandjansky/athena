// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IMuonRoICnvTool.h 575635 2013-12-16 10:58:41Z krasznaa $
#ifndef XAODTRIGGERCNV_IMUONROICNVTOOL_H
#define XAODTRIGGERCNV_IMUONROICNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigger/MuonRoIContainer.h"

// Forward declaration(s):
class LVL1_ROI;

namespace xAODMaker {

   /// The interface provided by IMuonRoICnvTool
   static const InterfaceID
   IID_IMuonRoICnvTool( "xAODMaker::IMuonRoICnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating MuonRoIContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         muon RoI objects from an existing AOD LVL1_ROI container into
    *         an xAOD::MuonRoIContainer.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 575635 $
    * $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $
    */
   class IMuonRoICnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::MuonRoIContainer
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::MuonRoIContainer* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IMuonRoICnvTool;
      }

   }; // class IMuonRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_IMUONROICNVTOOL_H
