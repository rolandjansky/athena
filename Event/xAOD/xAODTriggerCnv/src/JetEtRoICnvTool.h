// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetEtRoICnvTool.h 575635 2013-12-16 10:58:41Z krasznaa $
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
    * $Revision: 575635 $
    * $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $
    */
   class JetEtRoICnvTool : public AthAlgTool,
                           public virtual IJetEtRoICnvTool {

   public:
      /// Regular AlgTool constructor
      JetEtRoICnvTool( const std::string& type, const std::string& name,
                       const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::JetEtRoI
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::JetEtRoI* xaod );

   }; // class JetEtRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_JETETROICNVTOOL_H
