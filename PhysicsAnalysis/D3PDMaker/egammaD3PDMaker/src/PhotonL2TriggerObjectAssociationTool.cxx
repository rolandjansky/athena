// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/PhotonL2TriggerObjectAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from a photon to its matching L2 trigger object.
 */


#include "PhotonL2TriggerObjectAssociationTool.h"
#include "SGTools/BaseInfo.h"


SG_ADD_BASE(TrigPhoton, SG_VIRTUAL(INavigable4Momentum));


namespace D3PD {


   /**
    * This function is used to calculate the distance between the TrigPhoton
    * and the cluster associated to the offline photon.
    *
    * @param l2 The LVL2 TrigPhoton object
    * @param offl The egamma offline photon object
    * @returns The "distance" between the two objects
    */
   float PhotonTrigPhotonDistanceFunctor::
   calculateDistance( const TrigPhoton* l2,
                      const egamma* offl ) const {

      // Check if we can actually perform this calculation:
      if( offl->cluster() ) {
         return DeltaRDistanceFunctor< TrigPhoton, CaloCluster >::deltaR( l2,
                                                                          offl->cluster() );
      } else {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::WARNING, "PhotonTrigPhotonDistanceFunctor" )
            << "Offline photon has no cluster. Using the position of the egamma object.";
         return DeltaRDistanceFunctor< TrigPhoton, egamma >::deltaR( l2,
                                                                     offl );
      }
   }


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PhotonL2TriggerObjectAssociationTool::PhotonL2TriggerObjectAssociationTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : Base (type, name, parent)
{
  setMetric( &m_metric );

  if (setProperty ("ChainPattern", "L2_g.*").isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't set ChainPattern property for tool "
                                << type << "/" << name;
  }
}


} //  namespace D3PD
