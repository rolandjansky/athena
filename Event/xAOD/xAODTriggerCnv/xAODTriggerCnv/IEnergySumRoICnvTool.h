// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IEnergySumRoICnvTool.h 575635 2013-12-16 10:58:41Z krasznaa $
#ifndef XAODTRIGGERCNV_IENERGYSUMROICNVTOOL_H
#define XAODTRIGGERCNV_IENERGYSUMROICNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigger/EnergySumRoI.h"

// Forward declaration(s):
class LVL1_ROI;

namespace xAODMaker {

   /// The interface provided by IEnergySumRoICnvTool
   static const InterfaceID
   IID_IEnergySumRoICnvTool( "xAODMaker::IEnergySumRoICnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating EnergySumRoI from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         energy-sum RoI object from an existing AOD LVL1_ROI container into
    *         an xAOD::EnergySumRoI.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 575635 $
    * $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $
    */
   class IEnergySumRoICnvTool : public virtual IAlgTool {

   public:
      /// Function that fills an existing xAOD::EnergySumRoIContainer
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::EnergySumRoI* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_IEnergySumRoICnvTool;
      }

   }; // class IEnergySumRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_IENERGYSUMROICNVTOOL_H
