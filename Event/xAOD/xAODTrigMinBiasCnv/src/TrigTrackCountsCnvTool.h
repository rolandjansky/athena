// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBiasCNV_TrigTrackCountsCNVTOOL_H
#define xAODTrigMinBiasCNV_TrigTrackCountsCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMinBiasCnv/ITrigTrackCountsCnvTool.h"

class TrigTrackCountsCollection;

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigTrackCountsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITrigTrackCountsCnvTool.
    *
    * @author Andrzej Zemla <azemla@cern.ch>
    *
    */
   class TrigTrackCountsCnvTool : public AthAlgTool,
                          public virtual ITrigTrackCountsCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigTrackCountsCnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function that fills an existing xAOD::TrigTrackCountsContainer
      virtual StatusCode convert( const TrigTrackCountsCollection* aod,
                                  xAOD::TrigTrackCountsContainer* xaod ) const override;

   }; // class TrigTrackCountsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_TrigTrackCountsCNVTOOL_H
