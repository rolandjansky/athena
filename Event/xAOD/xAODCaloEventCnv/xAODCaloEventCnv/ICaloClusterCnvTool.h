// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICaloClusterCnvTool.h 596350 2014-05-10 14:03:58Z krasznaa $
#ifndef XAODCALOEVENTCNV_ICALOCLUSTERCNVTOOL_H
#define XAODCALOEVENTCNV_ICALOCLUSTERCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODCaloEvent/CaloCluster.h"

// Forward declaration(s):
class CaloCluster;
class CaloClusterCellLinkContainer;

namespace xAODMaker {

   /// The interface provided by ICaloClusterCnvTool
   static const InterfaceID
   IID_ICaloClusterCnvTool( "xAODMaker::ICaloClusterCnvTool", 1, 0 );

   /**
    *  @short Interface for the tool doing AOD->xAOD conversion for clusters
    *
    *         This interface is used by the trigger to translate old style
    *         CaloCluster objects into xAOD::CaloCluster ones.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    * @author Walter Lampl <Walter.Lampl@cern.ch>
    *
    * $Revision: 596350 $
    * $Date: 2014-05-10 16:03:58 +0200 (Sat, 10 May 2014) $
    */
   class ICaloClusterCnvTool : public virtual IAlgTool {

   public:
      /// Function doing the conversion
      virtual StatusCode convert( const CaloCluster* aod,
                                  xAOD::CaloCluster* xaod,
                                  CaloClusterCellLinkContainer* ccclc = 0 ) = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ICaloClusterCnvTool;
      }

   }; // class ICaloClusterCnvTool

} // namespace xAODMaker

#endif // XAODCALOEVENTCNV_ICALOCLUSTERCNVTOOL_H
