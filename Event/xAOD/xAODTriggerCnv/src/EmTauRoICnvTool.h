// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTauRoICnvTool.h 575635 2013-12-16 10:58:41Z krasznaa $
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
    * $Revision: 575635 $
    * $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $
    */
   class EmTauRoICnvTool : public AthAlgTool,
                           public virtual IEmTauRoICnvTool {

   public:
      /// Regular AlgTool constructor
      EmTauRoICnvTool( const std::string& type, const std::string& name,
                       const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::EmTauRoIContainer* xaod );

   }; // class EmTauRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_EMTAUROICNVTOOL_H
