// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigVertexCountsCnvTool.h 603232 2014-06-23 17:24:35Z azemla $
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

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::TrigVertexCountsContainer
      virtual StatusCode convert( const TrigVertexCountsCollection* aod,
                                  xAOD::TrigVertexCountsContainer* xaod );

   }; // class TrigVertexCountsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_TrigVertexCountsCNVTOOL_H
