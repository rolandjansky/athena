// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMRoICnvTool.h 576007 2013-12-17 18:07:37Z morrisj $
#ifndef XAODTRIGL1CALOCNV_CMMROICNVTOOL_H
#define XAODTRIGL1CALOCNV_CMMROICNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/ICMMRoICnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ICMMRoICnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ICMMRoICnvTool.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576007 $
    * $Date: 2013-12-17 19:07:37 +0100 (Tue, 17 Dec 2013) $
    */
   class CMMRoICnvTool : public AthAlgTool,
                         public virtual ICMMRoICnvTool {

   public:
      /// Regular AlgTool constructor
      CMMRoICnvTool( const std::string& type, const std::string& name,
                     const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const LVL1::CMMRoI* esd,
                                  xAOD::CMMRoI* xaod );

   }; // class CMMRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_CMMROICNVTOOL_H
