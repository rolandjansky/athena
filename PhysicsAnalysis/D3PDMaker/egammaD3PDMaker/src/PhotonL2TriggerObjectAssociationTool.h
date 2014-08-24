// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/PhotonL2TriggerObjectAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate from a photon to its matching L2 trigger object.
 */


#ifndef EGAMMAD3PDMAKER_PHOTONL2TRIGGEROBJECTASSOCIATIONTOOL_H
#define EGAMMAD3PDMAKER_PHOTONL2TRIGGEROBJECTASSOCIATIONTOOL_H


#include "TriggerD3PDMaker/TriggerObjectAssociationTool.h"
#include "egammaEvent/egamma.h"
#include "TrigParticle/TrigPhoton.h"
#include "TrigParticle/TrigPhotonContainer.h"


namespace D3PD {


   /**
    *  @short Special metric for matching LVL2 photons to offline ones
    *
    *         TrigMatchTool doesn't provide a special metric for matching
    *         TrigPhoton objects to offline photons. To make sure that the
    *         distance between these object combinations is calculated as
    *         we would like, we should use this metric for now.
    *
    *         This metric compares the eta and phi position returned by the
    *         TrigPhoton object to the eta and phi of the cluster associated
    *         to the offline photon.
    *
    *         This metric is not motivated by the MC10a/MC10b problems, it
    *         should be used for all data types.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 363585 $
    * $Date$
    */
   class PhotonTrigPhotonDistanceFunctor :
      public DistanceFunctor< TrigPhoton, egamma > {

   private:
      virtual float calculateDistance( const TrigPhoton* l2,
                                       const egamma* offl ) const;

   }; // class PhotonTrigPhotonDistanceFunctor


/**
 * @brief Associate from a photon to its matching L2 trigger object.
 */
typedef TriggerObjectAssociationTool<egamma, TrigPhoton>
  PhotonL2TriggerObjectAssociationToolBase;
class PhotonL2TriggerObjectAssociationTool
  : public PhotonL2TriggerObjectAssociationToolBase
{
public:
  typedef PhotonL2TriggerObjectAssociationToolBase Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  PhotonL2TriggerObjectAssociationTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent);

private:
  /// Special distance metric
  PhotonTrigPhotonDistanceFunctor m_metric;
};


} // namespace D3PD



#endif // not EGAMMAD3PDMAKER_PHOTONL2TRIGGEROBJECTASSOCIATIONTOOL_H
