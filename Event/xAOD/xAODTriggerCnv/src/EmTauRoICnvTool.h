// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGERCNV_EMTAUROICNVTOOL_H
#define XAODTRIGGERCNV_EMTAUROICNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTriggerCnv/IEmTauRoICnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IEmTauRoICnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IEmTauRoICnvTool.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    */
   class EmTauRoICnvTool : public AthAlgTool,
                           public virtual IEmTauRoICnvTool {

   public:
      /// Regular AlgTool constructor
      EmTauRoICnvTool( const std::string& type, const std::string& name,
                       const IInterface* parent );

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::EmTauRoIContainer* xaod ) override;

   }; // class EmTauRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_EMTAUROICNVTOOL_H
