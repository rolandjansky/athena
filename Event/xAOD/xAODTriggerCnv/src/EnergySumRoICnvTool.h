// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySumRoICnvTool.h 575635 2013-12-16 10:58:41Z krasznaa $
#ifndef XAODTRIGGERCNV_ENERGYSUMROICNVTOOL_H
#define XAODTRIGGERCNV_ENERGYSUMROICNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTriggerCnv/IEnergySumRoICnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IEnergySumRoICnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IEnergySumRoICnvTool.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 575635 $
    * $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $
    */
   class EnergySumRoICnvTool : public AthAlgTool,
                               public virtual IEnergySumRoICnvTool {

   public:
      /// Regular AlgTool constructor
      EnergySumRoICnvTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::EnergySumRoI
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::EnergySumRoI* xaod );

   }; // class EnergySumRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_ENERGYSUMROICNVTOOL_H
