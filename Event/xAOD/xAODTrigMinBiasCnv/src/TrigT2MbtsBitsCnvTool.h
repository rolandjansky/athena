// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigT2MbtsBitsCnvTool.h 608072 2014-07-23 00:27:40Z azemla $
#ifndef xAODTrigMinBiasCNV_TrigT2MbtsBitsCNVTOOL_H
#define xAODTrigMinBiasCNV_TrigT2MbtsBitsCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMinBiasCnv/ITrigT2MbtsBitsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigT2MbtsBitsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITrigT2MbtsBitsCnvTool.
    *
    * @author Andrzej Zemla <azemla@cern.ch>
    *
    */
   class TrigT2MbtsBitsCnvTool : public AthAlgTool,
                          public virtual ITrigT2MbtsBitsCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigT2MbtsBitsCnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::TrigT2MbtsBitsContainer
      virtual StatusCode convert( const TrigT2MbtsBitsContainer* aod,
                                  xAOD::TrigT2MbtsBitsContainer* xaod );

   }; // class TrigT2MbtsBitsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_TrigT2MbtsBitsCNVTOOL_H
