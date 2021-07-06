// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
    */
   class EnergySumRoICnvTool : public AthAlgTool,
                               public virtual IEnergySumRoICnvTool {

   public:
      /// Regular AlgTool constructor
      EnergySumRoICnvTool( const std::string& type, const std::string& name,
                           const IInterface* parent );

      /// Function that fills an existing xAOD::EnergySumRoI
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::EnergySumRoI* xaod ) override;

   }; // class EnergySumRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_ENERGYSUMROICNVTOOL_H
