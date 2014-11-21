// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMRoICnvTool.h 576052 2013-12-18 09:13:50Z morrisj $
#ifndef XAODTRIGL1CALOCNV_CPMROICNVTOOL_H
#define XAODTRIGL1CALOCNV_CPMROICNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/ICPMRoICnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the ICPMRoICnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in ICPMRoICnvTool.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576052 $
    * $Date: 2013-12-18 10:13:50 +0100 (Wed, 18 Dec 2013) $
    */
   class CPMRoICnvTool : public AthAlgTool,
                         public virtual ICPMRoICnvTool {

   public:
      /// Regular AlgTool constructor
      CPMRoICnvTool( const std::string& type, const std::string& name,
                     const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::CPMRoI>* esd,
                                  xAOD::CPMRoIContainer* xaod );

   }; // class CPMRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_CPMROICNVTOOL_H
