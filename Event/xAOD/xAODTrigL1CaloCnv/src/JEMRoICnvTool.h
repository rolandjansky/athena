// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMRoICnvTool.h 576328 2013-12-19 16:09:32Z morrisj $
#ifndef XAODTRIGL1CALOCNV_JEMROICNVTOOL_H
#define XAODTRIGL1CALOCNV_JEMROICNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"

// Local include(s):
#include "xAODTrigL1CaloCnv/IJEMRoICnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IJEMRoICnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IJEMRoICnvTool.
    *
    * @author John Morris <john.morris@cern.ch>
    *
    * $Revision: 576328 $
    * $Date: 2013-12-19 17:09:32 +0100 (Thu, 19 Dec 2013) $
    */
   class JEMRoICnvTool : public AthAlgTool,
                         public virtual IJEMRoICnvTool {

   public:
      /// Regular AlgTool constructor
      JEMRoICnvTool( const std::string& type, const std::string& name,
                     const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::EmTauRoIContainer
      virtual StatusCode convert( const DataVector<LVL1::JEMRoI>* esd,
                                  xAOD::JEMRoIContainer* xaod );

   }; // class JEMRoICnvTool

} // namespace xAODMaker

#endif // XAODTRIGL1CALOCNV_JEMROICNVTOOL_H
