// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

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
    */
   class MuonRoICnvTool : public AthAlgTool,
                          public virtual IMuonRoICnvTool {

   public:
      /// Regular AlgTool constructor
      MuonRoICnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function that fills an existing xAOD::MuonRoIContainer
      virtual StatusCode convert( const LVL1_ROI* aod,
                                  xAOD::MuonRoIContainer* xaod ) override;

   }; // class MuonRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_MUONROICNVTOOL_H
