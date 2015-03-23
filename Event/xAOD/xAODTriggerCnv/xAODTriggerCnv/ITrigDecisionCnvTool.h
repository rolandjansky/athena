// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ITrigDecisionCnvTool.h 578517 2014-01-15 13:48:04Z krasznaa $
#ifndef XAODTRIGGERCNV_ITRIGDECISIONCNVTOOL_H
#define XAODTRIGGERCNV_ITRIGDECISIONCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODTrigger/TrigDecision.h"

// Forward declaration(s):
namespace TrigDec {
   class TrigDecision;
}

namespace xAODMaker {

   /// The interface provided by ITrigDecisionCnvTool
   static const InterfaceID
   IID_ITrigDecisionCnvTool( "xAODMaker::ITrigDecisionCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool creating xAOD::TrigDecision from an AOD
    *
    *         This interface is implemented by the tool that copies the
    *         contents of the TrigDec::TrigDecision object into a newly created
    *         xAOD::TrigDecision object.
    *
    * @author Attila Krasznahorkay@cern.ch>
    *
    * $Revision: 578517 $
    * $Date: 2014-01-15 14:48:04 +0100 (Wed, 15 Jan 2014) $
    */
   class ITrigDecisionCnvTool : public virtual IAlgTool {

   public:
      /// Function filling an xAOD::TrigDecision object with information
      virtual StatusCode convert( const TrigDec::TrigDecision* aod,
                                  xAOD::TrigDecision* xaod ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ITrigDecisionCnvTool;
      }

   }; // class ITrigDecisionCnvTool

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_ITRIGDECISIONCNVTOOL_H
