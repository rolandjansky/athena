// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ICaloClusterCnvTool.h 785752 2016-11-22 15:06:34Z ssnyder $
#ifndef XAODCALOEVENTCNV_ICALOCLUSTERCNVTOOL_H
#define XAODCALOEVENTCNV_ICALOCLUSTERCNVTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// EDM include(s):
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// Forward declaration(s):
class CaloCluster;
class CaloClusterContainer;
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
    * $Revision: 785752 $
    * $Date: 2016-11-22 16:06:34 +0100 (Tue, 22 Nov 2016) $
    */
   class ICaloClusterCnvTool : public virtual IAlgTool {

   public:
      /// Function doing the conversion
      virtual StatusCode convert( const CaloCluster* aod,
                                  xAOD::CaloCluster* xaod,
                                  CaloClusterCellLinkContainer* ccclc = 0 ) const = 0;
     /// Converting function usable by TrigBStoxAODTool
      virtual StatusCode convert( const CaloClusterContainer* aod,
                                  xAOD::CaloClusterContainer* xaod ) const = 0;

      /// Gaudi interface definition
      static const InterfaceID& interfaceID() {
         return IID_ICaloClusterCnvTool;
      }

   }; // class ICaloClusterCnvTool

} // namespace xAODMaker

#endif // XAODCALOEVENTCNV_ICALOCLUSTERCNVTOOL_H
