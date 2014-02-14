// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonRoICnvTool.h 575635 2013-12-16 10:58:41Z krasznaa $
#ifndef XAODTRIGGERCNV_MUONROICNVTOOL_H
#define XAODTRIGGERCNV_MUONROICNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTriggerCnv/IMuonRoICnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IMuonRoICnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IMuonRoICnvTool.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 575635 $
    * $Date: 2013-12-16 11:58:41 +0100 (Mon, 16 Dec 2013) $
    */
   class MuonRoICnvTool : public AthAlgTool,
                          public virtual IMuonRoICnvTool {

   public:
      /// Regular AlgTool constructor
      MuonRoICnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::MuonRoIContainer
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::MuonRoIContainer* xaod );

   }; // class MuonRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_MUONROICNVTOOL_H
