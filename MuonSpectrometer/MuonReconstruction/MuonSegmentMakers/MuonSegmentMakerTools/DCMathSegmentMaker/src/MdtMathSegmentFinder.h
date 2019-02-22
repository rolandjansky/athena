/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATHSEGMENTMAKER_MDTMATHSEGMENTFINDER_H
#define DCMATHSEGMENTMAKER_MDTMATHSEGMENTFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecToolInterfaces/IMdtSegmentFinder.h"

class MsgStream;

namespace TrkDriftCircleMath {
  class SegmentFinder;
  class DCSLFitter;
}


namespace Muon {

  class MuonIdHelperTool;
  class IDCSLFitProvider;

  class MdtMathSegmentFinder : virtual public IMdtSegmentFinder::IMdtSegmentFinder, public AthAlgTool
  {

  public:
    MdtMathSegmentFinder (const std::string& t, const std::string& n, const IInterface*  p);
    
    ~MdtMathSegmentFinder();
    
    /** initialize method, method taken from bass-class AlgTool */
    virtual StatusCode initialize();
    
    /** finialize method, method taken from bass-class AlgTool  */
    virtual StatusCode finalize();
    
    /** IMdtMdtMathSegmentFinder interface implementation              */
    virtual const TrkDriftCircleMath::SegVec findSegments ( const TrkDriftCircleMath::DCVec&                    dcvec,
							    const TrkDriftCircleMath::CLVec&                    clvec,
							    const TrkDriftCircleMath::Road&                     road,
							    const TrkDriftCircleMath::DCStatistics&             dcstats,
							    const TrkDriftCircleMath::ChamberGeometry*          multiGeo ) const;
  protected:

    ToolHandle<IDCSLFitProvider> m_dcslFitProvider;
    ToolHandle<MuonIdHelperTool> m_idHelperTool;

    int  m_finderDebugLevel; //<! additional debug output
    bool m_doDrop;             //<! enable dropping of hits from segment   
    bool m_useChamberTheta;    //<! enable to usage of the angle from the chamber position to seed the finder
    bool m_enableSeedCleaning;     //<! enable seed cleaning for busy events
    double m_occupancyThreshold; //<! threshold use when seeding is enabled
    double m_occupancyCutOff;    //<! threshold above which no segment finding is performed
    double m_roadWidth;          //<! width of road used to associate hits to seed lines
    double m_chi2PerDofDrop;     //<! maximum chi2 per dof used during cleaning
    double m_ratioEmptyTubesCut; //<! cut on the ratio empty tubes/(hits on track + delta)
    double m_mdtAssociationPullCut; //<! cut on the pull of MDT hits during association to segment
    double m_rpcAssociationPullCut; //<! cut on the pull of RPC hits during association to segment
    double m_tgcAssociationPullCut; //<! cut on the pull of TGC hits during association to segment
    bool   m_doAllHitSort;       //<! configure the SegmentFinder to include trigger hits in sorting of segments
    bool   m_doRoadAngleSeeding; //<! use angle of road to seed segment search
    bool   m_doIPAngleSeeding;   //<! use angle of IP to seed segment search
    double m_tightRoadCut; //<! tight cut on angle with prediction, used for very busy chambers
    bool   m_doSingleMultiLayerScan; //<! perform single ml scan
    bool   m_recoverMdtOutliers; //<! recover MDT outliers
    bool   m_removeSingleOutliers; //<! remove single bad hits from segment
    bool   m_doCurvedSegmentFinder; //<! use curved segment finding routine
    double m_deltaCutT0Segments; //<! delta cut for segments with T0
    double m_residualCutT0Segments; //<! residual cut for segments with T0
    bool   m_useSegmentQuality; //<! use segment quality in hit dropping instead of chi2

    unsigned int m_maxHitsPerFullSearch; //<! maximum number of hits, above will use faster search mode
  };

}

#endif
