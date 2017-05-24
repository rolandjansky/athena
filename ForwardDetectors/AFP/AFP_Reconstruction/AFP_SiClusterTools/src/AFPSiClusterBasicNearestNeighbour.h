/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SICLUSTERTOOLS_AFPSICLUSTERBASICNEARESTNEIGHBOUR_H
#define AFP_SICLUSTERTOOLS_AFPSICLUSTERBASICNEARESTNEIGHBOUR_H


/**
 * @file   AFPSiClusterBasicNearestNeighbour.h
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-05-02
 * 
 * @brief  Header file for AFPSiClusterBasicNearestNeighbour used for clustering hits.
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
class AFPSiClusterBasicNearestNeighbour
  : virtual public ::IAFPSiClusterAlgTool,
    public ::AthAlgTool
{
public:
  AFPSiClusterBasicNearestNeighbour(const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  // Athena algtool's Hooks
  /// does nothing
  virtual StatusCode initialize();

  /// does nothing
  virtual StatusCode finalize();

  /// Creates a cluster from the neighbouring pixels, joining only two pixels with charge above #m_chargeThreshold
  virtual StatusCode doClustering (const std::list<const xAOD::AFPSiHit*>& hits, std::list<AFPSiClusterBasicObj>& outputClusters);

  /// @copydoc AFPSiClusterBasicNearestNeighbour::m_chargeThreshold
  float chargeThreshold () const {return m_chargeThreshold;}

  void setChargeThreshold (const float threshold)  {m_chargeThreshold = threshold;}

private:
  float m_chargeThreshold;	///< charge above which hits are used for clustering
};


#endif //  AFP_SICLUSTERTOOLS_AFPSICLUSTERBASICNEARESTNEIGHBOUR_H
