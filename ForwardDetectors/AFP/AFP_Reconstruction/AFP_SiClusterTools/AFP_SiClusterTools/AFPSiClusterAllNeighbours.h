/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_AFPSICLUSTERAllNEIGHBOURS_H
#define AFP_SICLUSTERTOOLS_AFPSICLUSTERAllNEIGHBOURS_H


/**
 * @file   AFPSiClusterAllNeighbours.h
 * @author Petr Balek <petr.balek@cern.ch>
 * @date   2021-12-07
 * 
 * @brief  Header file for AFPSiClusterAllNeighbours used for clustering hits.
 * 
 */



// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// AFP_LocReco includes
#include "AFP_SiClusterTools/IAFPSiClusterAlgTool.h"

// STL includes
#include <string>
#include <list>

class AFPSiClusterAllNeighbours
  : public extends<AthAlgTool, IAFPSiClusterAlgTool>
{
public:
  AFPSiClusterAllNeighbours(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual ~AFPSiClusterAllNeighbours() override {}

  /// does nothing
  virtual StatusCode initialize() override;

  /// does nothing
  virtual StatusCode finalize() override;

  /// Creates a cluster from the neighbouring pixels, joining only two pixels with charge above #m_chargeThreshold
  virtual StatusCode doClustering (const std::list<const xAOD::AFPSiHit*>& hits, std::list<AFPSiClusterBasicObj>& outputClusters) const override;

  /// @copydoc AFPSiClusterAllNeighbours::m_chargeThreshold
  float chargeThreshold () const {return m_chargeThreshold;}

private:
  Gaudi::Property<float> m_chargeThreshold {this, "chargeThreshold", 1000., "charge above which hits are used for clustering"};
  
  Gaudi::Property<std::string> m_neighbourhoodType {this, "neighbourhoodType", "X", "type of hit neighbourhood in cluster, either X (only long-edge) or XY (both long- and short-edge)"};
  
  bool m_doOnlyHorz;
};


#endif //  AFP_SICLUSTERTOOLS_AFPSICLUSTERAllNEIGHBOURS_H
