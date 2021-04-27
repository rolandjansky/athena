// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGERCNV_JETROICNVTOOL_H
#define XAODTRIGGERCNV_JETROICNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTriggerCnv/IJetRoICnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IJetRoICnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IJetRoICnvTool.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    */
   class JetRoICnvTool : public AthAlgTool,
                         public virtual IJetRoICnvTool {

   public:
      /// Regular AlgTool constructor
      JetRoICnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function that fills an existing xAOD::JetRoIContainer
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::JetRoIContainer* xaod ) override;

   }; // class JetRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_JETROICNVTOOL_H
