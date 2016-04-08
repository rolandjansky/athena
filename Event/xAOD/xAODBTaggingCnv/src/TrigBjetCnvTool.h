// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigBjetCnvTool.h 601176 2014-06-11 10:54:22Z lidiaz $
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
    *
    * @author
    *
    */
   class TrigBjetCnvTool : public AthAlgTool,
                          public virtual ITrigBjetCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigBjetCnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::BTaggingContainer
      virtual StatusCode convert( const TrigEFBjetContainer* aod,
                                  xAOD::BTaggingContainer* xaod );

   }; // class TrigBjetCnvTool

} // namespace xAODMaker

#endif // xAODBTaggingCNV_TrigBjetCNVTOOL_H
