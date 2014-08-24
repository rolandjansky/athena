// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaEFTriggerObjectAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from an egamma to its matching EF trigger object.
 */


#include "egammaEFPhotonTriggerObjectAssociationTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"


namespace D3PD {

   /**
    * This function is used to calculate the distance between the EF photon
    * and the cluster associated to the offline photon.
    *
    * @param ef The EF photon object
    * @param offl The egamma offline photon object
    * @returns The "distance" between the two objects
    */
   float PhotonEFPhotonDistanceFunctor::
   calculateDistance( const egamma* ef,
                      const egamma* offl ) const {

      // Check if we can actually perform this calculation:
      if( offl->cluster() ) {
         return DeltaRDistanceFunctor< egamma,
                                       CaloCluster >::deltaR( ef,
                                                              offl->cluster() );
      } else {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::WARNING, "PhotonEFPhotonDistanceFunctor" )
            << "Offline photon has no cluster. Using the position of the egamma object.";
         return DeltaRDistanceFunctor< egamma, egamma >::deltaR( ef,
                                                                 offl );
      }
   }


/**
 * @brief Compare two objects by their position in a supplied collection.
 * @param p1 First object to compare.
 * @param p2 Second object to compare.
 */
bool
egammaEFPhotonTriggerObjectAssociationTool::compareInSequence 
  (const egamma* p1,
   const egamma* p2)
{
  if (m_targetLabel.empty())
    return false;

  // No way to pass a StatusCode back from here.
  // If this gets an error, it'll be logged.
  // We generally wouldn't get an error anyway,
  // since we turn on AllowMissing.
  m_map.reset().ignore();

  int i1 = m_map.findIndex (p1);
  int i2 = m_map.findIndex (p2);
  return i1 < i2;
}


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
egammaEFPhotonTriggerObjectAssociationTool::egammaEFPhotonTriggerObjectAssociationTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : Base (type, name, parent),
        m_map (this, m_targetLabel, m_registry, true)
{
  declareProperty ("Target",        m_targetLabel,
                   "Label of getter for the sequence to define the final "
                   "disambiguation of egammas.  "
                   "Leave blank to not do this.");
  declareProperty ("CollectionGetterRegistry", m_registry,
                   "The ICollectionGetterRegistryTool instance.");
}


/**
 * @brief Configure during initialization: type-check.
 * @param tree Our parent for tuple making.
 * @param ti Gives the type of the object being passed to @c resetUntyped.
 *
 * @c configureD3PD should check that the type of the object coming as input
 * (to @c getUntyped)
 * is compatible with what it expects, and raise an error otherwise.
 */
StatusCode
egammaEFPhotonTriggerObjectAssociationTool::configureD3PD (D3PD::IAddVariable* tree,
                                                           const std::type_info& ti)
{
  CHECK( Base::configureD3PD (tree, ti) );

  setMetric( &m_metric );

  if (!m_targetLabel.empty())
    CHECK( m_map.configureD3PD() );

  return StatusCode::SUCCESS;
}


} //  namespace D3PD
