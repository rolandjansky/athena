// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigTrackCountsCnvTool.h 603232 2014-06-23 17:24:35Z azemla $
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

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::TrigTrackCountsContainer
      virtual StatusCode convert( const TrigTrackCountsCollection* aod,
                                  xAOD::TrigTrackCountsContainer* xaod );

   }; // class TrigTrackCountsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_TrigTrackCountsCNVTOOL_H
