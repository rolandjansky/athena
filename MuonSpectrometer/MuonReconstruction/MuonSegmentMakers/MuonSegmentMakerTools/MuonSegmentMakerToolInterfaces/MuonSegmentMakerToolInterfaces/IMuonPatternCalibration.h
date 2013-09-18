/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTMAKETOOLINTERFACES_MUON_IMUONPATTERNCALIBRATION_H
#define MUONSEGMENTMAKETOOLINTERFACES_MUON_IMUONPATTERNCALIBRATION_H
 
#include "GaudiKernel/IAlgTool.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonPattern/MuonPatternCombination.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

#include "Identifier/Identifier.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include <vector>
//                           
static const InterfaceID IID_IMuonPatternCalibration("Muon::IMuonPatternCalibration",1,0);

namespace Muon {
  class MuonClusterOnTrack;
  class MdtDriftCircleOnTrack;

  /** @brief The IMuonCalibration is a pure virtual interface for tools to calibrate PRD hits  */  
  class IMuonPatternCalibration : virtual public IAlgTool 
  {      
    public:

    typedef std::vector< const MuonClusterOnTrack* > ClusterVec;
    typedef std::vector< const MdtDriftCircleOnTrack* > MdtVec;
    typedef std::vector< MdtVec > MdtVecVec;

    struct ROTRegion{
      int regionId;
      Amg::Vector3D regionPos;
      Amg::Vector3D regionDir;
      ClusterVec clusters;
      MdtVecVec     mdts;
    };

    typedef std::vector< ROTRegion > ROTsPerRegion;

    
    
    /** access to tool interface */
    static const InterfaceID& interfaceID();

    virtual void calibrate(const MuonPatternCombination& pat, ROTsPerRegion& hitsPerRegion) const = 0;
    virtual int  getRegionId( const Identifier& id ) const = 0;
    virtual void clearRotsPerRegion( ROTsPerRegion& hitsPerRegion ) const = 0;
    virtual bool checkForPhiMeasurements( const MuonPatternCombination& pat ) const = 0;
  };
  
  inline const InterfaceID& IMuonPatternCalibration::interfaceID()
  {
    return IID_IMuonPatternCalibration;
  }

} // end of name space

#endif 

