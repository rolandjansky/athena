// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/ElectronL2TriggerObjectAssociationTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from an electron to its matching L2 trigger object.
 */


#include "ElectronL2TriggerObjectAssociationTool.h"
#include "D3PDMakerInterfaces/ICollectionGetterRegistryTool.h"
#include "SGTools/BaseInfo.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"


SG_ADD_BASE(TrigElectron, SG_VIRTUAL(INavigable4Momentum));


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
 * Note that for TrigElectron objects, the metric used for trigger matching
 * is based on the cluster (eta,phi), not the electron (eta,phi).
 * Thus, we can have two TrigElectron's with the same metric
 * distance from the source object but with different DR.
 *
 * This version ranks first by @c dr, then by @c pt, then by tracking algorithm,
 * then by ethad1.
 */
inline
bool lessForTriggerAssociation (IAlgTool* tool,
                                const I4Momentum* p0,
                                const TrigElectron* p1,
                                const TrigElectron* p2)
{
  float dr1 = P4Helpers::deltaR (*p0, *p1);
  float dr2 = P4Helpers::deltaR (*p0, *p2);
  if (CxxUtils::fpcompare::less (dr2, dr1))
    return true;
  if (CxxUtils::fpcompare::less (dr1, dr2))
    return false;

  if (CxxUtils::fpcompare::less (p1->pt(), p2->pt()))
    return true;
  if (CxxUtils::fpcompare::less (p2->pt(), p1->pt()))
    return false;
  if ((int)p1->trackAlgo() < (int)p2->trackAlgo())
    return true;
  if ((int)p2->trackAlgo() < (int)p1->trackAlgo())
    return false;
  if (CxxUtils::fpcompare::less (p2->Ethad1(), p1->Ethad1()))
    return true;
  if (CxxUtils::fpcompare::less (p1->Ethad1(), p2->Ethad1()))
    return false;

  // Break remaining ties by the order in the collection, if it was supplied.
  ElectronL2TriggerObjectAssociationTool* atool = 
    dynamic_cast<ElectronL2TriggerObjectAssociationTool*> (tool);
  if (!atool) return false;
  return atool->compareInSequence (p1, p2);
}


   /**
    * This function is used to calculate the distance between the TrigElectron
    * and the track associated to the offline electron.
    *
    * @param l2 The LVL2 TrigElectron object
    * @param offl The egamma offline electron object
    * @returns The "distance" between the two objects
    */
   float ElectronTrigElectronDistanceFunctor::
   calculateDistance( const TrigElectron* l2,
                      const egamma* offl ) const {

      // Check if we can actually perform this calculation:
      if( offl->nTrackParticles() && offl->trackParticle() ) {
         return DeltaRDistanceFunctor< TrigElectron,
                                       Rec::TrackParticle >::deltaR( l2,
                                                                     offl->trackParticle() );
      } else {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::DEBUG, "ElectronTrigElectronDistanceFunctor" )
            << "Offline electron has no track. Using the position of the egamma object.";
         return DeltaRDistanceFunctor< TrigElectron, egamma >::deltaR( l2,
                                                                       offl );
      }
   }


/**
 * @brief Compare two objects by their position in a supplied collection.
 * @param p1 First object to compare.
 * @param p2 Second object to compare.
 */
bool
ElectronL2TriggerObjectAssociationTool::compareInSequence 
  (const TrigElectron* p1,
   const TrigElectron* p2)
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
ElectronL2TriggerObjectAssociationTool::ElectronL2TriggerObjectAssociationTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
      : Base (type, name, parent),
        m_map (this, m_targetLabel, m_registry, true)
{
  declareProperty ("Target",        m_targetLabel,
                   "Label of getter for the sequence to define the final "
                   "disambiguation of TrigElectrons.  "
                   "Leave blank to not do this.");
  declareProperty ("CollectionGetterRegistry", m_registry,
                   "The ICollectionGetterRegistryTool instance.");
  declareProperty ("UseMCMetric", m_useMCMetric = false,
                   "Use a special metric designed for MC files?");

  if (setProperty ("ChainPattern", "L2_e.*").isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Can't set ChainPattern property for tool "
                                << type << "/" << name;
  }
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
ElectronL2TriggerObjectAssociationTool::configureD3PD (D3PD::IAddVariable* tree,
                                                       const std::type_info& ti)
{
  CHECK( ElectronL2TriggerObjectAssociationToolBase::configureD3PD (tree, ti) );

  if( m_useMCMetric ) setMetric( &m_metric );

  if (!m_targetLabel.empty())
    CHECK( m_map.configureD3PD() );

  return StatusCode::SUCCESS;
}


} //  namespace D3PD
