// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigT2ZdcSignalsCnvTool.h 608072 2014-07-23 00:27:40Z azemla $
#ifndef xAODTrigMinBiasCNV_TrigT2ZdcSignalsCNVTOOL_H
#define xAODTrigMinBiasCNV_TrigT2ZdcSignalsCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigMinBiasCnv/ITrigT2ZdcSignalsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigT2ZdcSignalsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITrigT2ZdcSignalsCnvTool.
    *
    * @author Andrzej Zemla <azemla@cern.ch>
    *
    */
   class TrigT2ZdcSignalsCnvTool : public AthAlgTool,
                          public virtual ITrigT2ZdcSignalsCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigT2ZdcSignalsCnvTool( const std::string& type, const std::string& name,
                      const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::TrigT2ZdcSignalsContainer
      virtual StatusCode convert( const TrigT2ZdcSignalsContainer* aod,
                                  xAOD::TrigT2ZdcSignalsContainer* xaod );

   }; // class TrigT2ZdcSignalsCnvTool

} // namespace xAODMaker

#endif // xAODTrigMinBiasCNV_TrigT2ZdcSignalsCNVTOOL_H
