// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBiasCNV_TrigVertexCountsCNVTOOL_H
#define xAODTrigMinBiasCNV_TrigVertexCountsCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMinBiasCnv/ITrigVertexCountsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigVertexCountsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITrigVertexCountsCnvTool.
    *
    * @author Andrzej Zemla <azemla@cern.ch>
    *
    */
   class TrigVertexCountsCnvTool : public AthAlgTool,
                          public virtual ITrigVertexCountsCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigVertexCountsCnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function that fills an existing xAOD::TrigVertexCountsContainer
      virtual StatusCode convert( const TrigVertexCountsCollection* aod,
                                  xAOD::TrigVertexCountsContainer* xaod ) const override;

   }; // class TrigVertexCountsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_TrigVertexCountsCNVTOOL_H
