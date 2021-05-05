// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODBTaggingCNV_TrigBjetCNVTOOL_H
#define xAODBTaggingCNV_TrigBjetCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODBTaggingCnv/ITrigBjetCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigBjetCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITrigBjetCnvTool.
    */
   class TrigBjetCnvTool : public AthAlgTool,
                          public virtual ITrigBjetCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigBjetCnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function that fills an existing xAOD::BTaggingContainer
      virtual StatusCode convert( const TrigEFBjetContainer* aod,
                                  xAOD::BTaggingContainer* xaod ) const override;

   }; // class TrigBjetCnvTool

} // namespace xAODMaker

#endif // xAODBTaggingCNV_TrigBjetCNVTOOL_H
