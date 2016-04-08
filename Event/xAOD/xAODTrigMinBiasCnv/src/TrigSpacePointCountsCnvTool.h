// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:$
#ifndef xAODTrigMinBiasCNV_TrigSpacePointCountsCNVTOOL_H
#define xAODTrigMinBiasCNV_TrigSpacePointCountsCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMinBiasCnv/ITrigSpacePointCountsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigSpacePointCountsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITrigSpacePointCountsCnvTool.
    *
    * @author Andrzej Zemla <azemla@cern.ch>
    *
    */
   class TrigSpacePointCountsCnvTool : public AthAlgTool,
                          public virtual ITrigSpacePointCountsCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigSpacePointCountsCnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::TrigSpacePointCountsContainer
      virtual StatusCode convert( const TrigSpacePointCountsCollection* aod,
                                  xAOD::TrigSpacePointCountsContainer* xaod );

   }; // class TrigSpacePointCountsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_TrigSpacePointCountsCNVTOOL_H
