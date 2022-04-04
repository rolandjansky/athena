/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_AFPSICLUSTERSINGLEHITTOOL_H
#define AFP_SICLUSTERTOOLS_AFPSICLUSTERSINGLEHITTOOL_H 1


/**
 * @file   AFPSiClusterSingleHitTool.h
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-18
 * 
 * @brief  Header file for AFPSiClusterSingleHitTool used for clustering hits.
 * 
 */

// STL includes
#include <string>
#include <list>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// AFP_LocReco includes
#include "AFP_SiClusterTools/IAFPSiClusterAlgTool.h"

// Forward declaration
class AFPSiClusterSingleHitTool
  : public extends<AthAlgTool, IAFPSiClusterAlgTool>
{
public:
  AFPSiClusterSingleHitTool(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  /// does nothing
  virtual StatusCode initialize() override;

  /// does nothing
  virtual StatusCode finalize() override;

  /// Creates a cluster from each hit with charge above #m_chargeThreshold
  virtual StatusCode doClustering (const std::list<const xAOD::AFPSiHit*>& hits, std::list<AFPSiClusterBasicObj>& outputClusters) const override;

  /// @copydoc AFPSiClusterSingleHitTool::m_chargeThreshold
  float chargeThreshold () const {return m_chargeThreshold;}

private:
  Gaudi::Property<float> m_chargeThreshold {this, "chargeThreshold", 1000., "charge above which hits are used for clustering"};
};


#endif // > !AFP_SICLUSTERTOOLS_AFPSICLUSTERSINGLEHITTOOL_H
