// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
#ifndef XAODTRIGGERCNV_ITRIGNAVIGATIONVTOOL_H
#define XAODTRIGGERCNV_ITRIGNAVIGATIONVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigger/TrigNavigation.h"

// Forward declaration(s):
namespace HLT {
   class HLTResult;
}

namespace xAODMaker {

   /// The interface provided by ITrigNavigationCnvTool
   static const InterfaceID
   IID_ITrigNavigationCnvTool( "xAODMaker::ITrigNavigationCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating xAOD::TrigNavigation from an AOD
    *
    *         This interface is implemented by the tool that copies the
    *         contents of the HLT::HLTResult navigation payload to the
    *         xAOD::TrigNavigation object.
    *
    * @author Tomasz Bold 
    *
    * $Revision: $
    * $Date:$
    */
   class ITrigNavigationCnvTool : public virtual IAlgTool {

   public:
      /// Function filling an xAOD::TrigNavigation object with information
     virtual StatusCode convert( const HLT::HLTResult* aod,
                                  xAOD::TrigNavigation* xaod ) const = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigNavigationCnvTool;
      }

   }; // class ITrigNavigationCnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_ITRIGNAVIGATIONVTOOL_H
