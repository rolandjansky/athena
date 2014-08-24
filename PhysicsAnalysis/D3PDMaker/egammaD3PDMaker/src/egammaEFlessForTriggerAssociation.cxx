/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: egammaEFlessForTriggerAssociation.cxx 364392 2011-05-07 15:08:08Z krasznaa $

// Local include(s):
#include "egammaEFlessForTriggerAssociation.h"
#include "egammaEFElectronTriggerObjectAssociationTool.h"
#include "egammaEFPhotonTriggerObjectAssociationTool.h"

namespace D3PD {

   /**
    * @brief Rank results of trigger association.
    * @param tool The tool that calls us.
    * @param p0 The source object for the association.
    * @param p1 First object to compare.
    * @param p2 Second object to compare.
    *
    * When we associate to trigger objects, we may have several candidates
    * at the same (eta, phi).  Just picking the first one is bad, because
    * the ordering of these objects can change from run to run.  So we use
    * this to rank among objects with the same (eta, phi).
    *
    * This same function is used by egammaEFElectronTriggerObjectAssociationTool
    * and egammaEFPhotonTriggerObjectAssociationTool. Unfortunately the original
    * design kind of breaks down in this use case...
    */
   bool lessForTriggerAssociation( IAlgTool* tool,
                                   const I4Momentum* /*p0*/,
                                   const egamma* p1,
                                   const egamma* p2 ) {

      // Break ties by the order in the collection, if it was supplied.
      egammaEFElectronTriggerObjectAssociationTool* eltool = 
         dynamic_cast<egammaEFElectronTriggerObjectAssociationTool*> (tool);

      if( eltool ) {
         return eltool->compareInSequence (p1, p2);
      }

      egammaEFPhotonTriggerObjectAssociationTool* phtool = 
         dynamic_cast<egammaEFPhotonTriggerObjectAssociationTool*> (tool);

      if( phtool ) {
         return phtool->compareInSequence (p1, p2);
      }

      REPORT_MESSAGE_WITH_CONTEXT( MSG::ERROR, "lessForTriggerAssociation" )
         << "Unknown tool type received";
      return false;
   }

} // namespace D3PD
