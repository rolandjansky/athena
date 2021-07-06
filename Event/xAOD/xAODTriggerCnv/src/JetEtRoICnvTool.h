// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGGERCNV_JETETROICNVTOOL_H
#define XAODTRIGGERCNV_JETETROICNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTriggerCnv/IJetEtRoICnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IJetEtRoICnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IJetEtRoICnvTool.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    */
   class JetEtRoICnvTool : public AthAlgTool,
                           public virtual IJetEtRoICnvTool {

   public:
      /// Regular AlgTool constructor
      JetEtRoICnvTool( const std::string& type, const std::string& name,
                       const IInterface* parent );

      /// Function that fills an existing xAOD::JetEtRoI
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::JetEtRoI* xaod ) override;

   }; // class JetEtRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_JETETROICNVTOOL_H
