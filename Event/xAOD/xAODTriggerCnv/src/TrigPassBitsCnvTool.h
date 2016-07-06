// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODTRIGGERCNV_TRIGPASSBITSCNVTOOL_H
#define XAODTRIGGERCNV_TRIGPASSBITSCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTriggerCnv/ITrigPassBitsCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ITrigPassBitsCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ITrigPassBitsCnvTool.
    *
    * @author Oleg Kuprash <oleg.kuprash@cern.ch>
    *
    * $Revision$
    * $Date$
    */
   class TrigPassBitsCnvTool : public AthAlgTool,
                           public virtual ITrigPassBitsCnvTool {

   public:
      /// Regular AlgTool constructor
      TrigPassBitsCnvTool( const std::string& type, const std::string& name,
                       const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::TrigPassBits
      virtual StatusCode convert( const TrigPassBitsCollection* aod,
                                  xAOD::TrigPassBitsContainer* xaod );

   }; // class TrigPassBitsCnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGPASSBITSCNVTOOL_H
