// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetRoICnvTool.h 575635 2013-12-16 10:58:41Z krasznaa $
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
    *
    * $Revision: 575635 $
    * $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $
    */
   class JetRoICnvTool : public AthAlgTool,
                         public virtual IJetRoICnvTool {

   public:
      /// Regular AlgTool constructor
      JetRoICnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::JetRoIContainer
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::JetRoIContainer* xaod );

   }; // class JetRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_JETROICNVTOOL_H
