/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MergedPixelsAlg.h
// SimplePixelClusteringTool plus implementation of merging clusters which
// contain a common RDO Identifier  
// Input from RDOs  
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SICLUSTERIZATIONTOOL_MERGEDPIXELSTOOL_H
#define SICLUSTERIZATIONTOOL_MERGEDPIXELSTOOL_H

#include "SiClusterizationTool/PixelClusteringToolBase.h"

#include "Identifier/Identifier.h"
// forward declare not possible (typedef)
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "PixelGeoModel/IBLParameterSvc.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"

#include <atomic>
#include <vector>

class IIncidentSvc;
class PixelID;

namespace InDetDD {
  class SiDetectorElement;
}

namespace InDet {

  class PixelCluster;
  class IPixelClusterSplitter;
  class IPixelClusterSplitProbTool;


  class MergedPixelsTool : public PixelClusteringToolBase
  {
  public:


    // Constructor with parameters:
    MergedPixelsTool
      (const std::string& type,
       const std::string& name,
       const IInterface* parent);

    virtual ~MergedPixelsTool() = default;

    // Called by the PixelPrepRawDataFormation algorithm once for every pixel 
    // module (with non-empty RDO collection...). 
    // It clusters together the RDOs with a pixell cell side in common.
    virtual PixelClusterCollection *clusterize
      (const InDetRawDataCollection<PixelRDORawData>& RDOs,
       const PixelID& pixelID) const;
    //   void init(int posstrategy, int errorstrategy);

    // Once the lists of RDOs which makes up the clusters have been found by the
    // clusterize() method, this method is called for each of these lists.
    // The method computes the local position of the cluster, and create 
    // a "cluster object" with all the required information (including charge 
    // interpolation variables Omegax and Omegay, and whether the cluster 
    // contains ganged pixels) 
    // This method calls the ClusterMakerTool to compute global position and 
    // position errors.
    // Input parameters are the list of RDOs identifier of the would-be 
    // cluster, the list of TOT values, the module the cluster belongs to,
    // the pixel helper tool and the number of RDOS of the cluster.
    PixelCluster* makeCluster(const std::vector<Identifier>& group,
                              const std::vector<int>& totgroup,
                              const std::vector<int>& lvl1group,
                              const InDetDD::SiDetectorElement* element,
                              const PixelID& pixelID,
                              int& clusterNumber,
                              bool split=false,
                              double splitProb1=0.,
                              double splitProb2=0.) const;

    ///Retrieve the necessary services in initialize                
    StatusCode initialize();
        
    ///Statistics output                
    StatusCode finalize();


  private:
    typedef std::vector<Identifier> RDO_Vector;
    typedef std::vector<RDO_Vector*> RDO_GroupVector;
    typedef std::vector<int> TOT_Vector;
    typedef std::vector<TOT_Vector*> TOT_GroupVector;

    MergedPixelsTool();
    MergedPixelsTool(const MergedPixelsTool&);
    MergedPixelsTool &operator=(const MergedPixelsTool&);

    // Determines if a pixel cell (whose identifier is the first argument) is 
    // a ganged pixel. If this is the case, the last argument assumes the 
    // value of the identifier of the cell it is ganged with. 
    // The second argument is the pixel module the hit belongs to.

    bool isGanged(const Identifier& rdoID,
                  //  const Identifier& elementID,
                  //  const PixelID& pixelID,
                  const InDetDD::SiDetectorElement* element,
                  Identifier & gangedID) const;


    // Checks if two RDO lists (would be clusters) whould be merged, 
    // which do happen if there is a pair of pixel cells belonging to the 
    // two different groups which have a side in common
    void checkForMerge(const Identifier& id,
                       RDO_GroupVector::iterator baseGroup,
                       RDO_GroupVector::iterator lastGroup,
                       TOT_GroupVector::iterator totGroup,
                       TOT_GroupVector::iterator lvl1Group,
                       const InDetDD::SiDetectorElement* element,
                       const PixelID& pixelID) const;

    // Functions for merging broken cluster segments (to be used for upgrade studies)
    // ITk: This function checks if two barrel clusters are potentially a single cluster which is broken into two pieces  
    bool mergeTwoBrokenClusters(const std::vector<Identifier>& group1, 
                                const std::vector<Identifier>& group2,
                                const InDetDD::SiDetectorElement* element,
                                const PixelID& pixelID) const;
    // ITk: this function checks if two to-be-merged barrel proto-clusters have sizeZ consistent with cluster positions 
    bool mergeTwoClusters(const std::vector<Identifier>& group1, 
                          const std::vector<Identifier>& group2,
                          const InDetDD::SiDetectorElement* element,
                          const PixelID& pixelID) const;

    // ITk: checkSizeZ compares cluster sizeZ with expected cluster size for this cluster position (+/-200 mm for beam spread)
    // checkSizeZ()=-1 if cluster is too small
    // checkSizeZ()=0 if cluster sizeZ is within allowed range
    // checkSizeZ()=1 if cluster is too large
    // in the future, it may be changed to return deltaSizeZ 
    int checkSizeZ(int colmin, int colmax, int row, const InDetDD::SiDetectorElement* element) const;
    // this function returns expected sizeZ
    int expectedSizeZ(int colmin, int colmax, int row, const InDetDD::SiDetectorElement* element) const;
    // this function returns size of the maximum gap between two cluster fragments  
    int maxGap(int colmin, int colmax, int row, const InDetDD::SiDetectorElement* element) const;
    //------- end of declaration of new functions


    ServiceHandle<IIncidentSvc>                         m_incidentSvc;   //!< IncidentSvc to catch begin of event and end of envent
    ServiceHandle<IBLParameterSvc>                      m_IBLParameterSvc;
    /// for cluster splitting
    BooleanProperty m_emulateSplitter{this, "EmulateSplitting", false, "don't split - only emulate the split"};
    UnsignedIntegerProperty m_minSplitSize{this, "MinimalSplitSize", 1, "minimum split size, regulates also the cluster splitting"};
    UnsignedIntegerProperty m_maxSplitSize{this, "MaximalSplitSize", 1000, "minimum split size, regulates also the cluster splitting"};
    DoubleProperty m_minSplitProbability{this, "MinimalSplitProbability", 0., "minimal split probability"};
    ToolHandle<InDet::IPixelClusterSplitProbTool> m_splitProbTool{this, "SplitProbTool", "", "ToolHandle for the split probability tool"};
    ToolHandle<InDet::IPixelClusterSplitter> m_clusterSplitter{this, "ClusterSplitter", "", "ToolHandle for the split probability tool"};
    BooleanProperty m_doIBLSplitting{this, "DoIBLSplitting", false};
    BooleanProperty m_doMergeBrokenClusters{this, "DoMergeBrokenClusters", false, "ITk: switch to turn ON/OFF merging of broken clusters"};
    BooleanProperty m_doRemoveClustersWithToTequalSize{this, "DoRemoveClustersWithToTequalSize", false, "ITk: switch to remove clusters with ToT=size"};
    BooleanProperty m_doCheckSizeBeforeMerging{this, "DoCheckSizeBeforeMerging", false, "ITk: switch to check size of to-be-merged clusters"};
    DoubleProperty m_beam_spread{this, "BeamSpread", 200.0, "ITK: size of the luminous region, need to check cluster size"};
    DoubleProperty m_lossThreshold{this, "LossProbability", 0.001, "ITk: maximum probability to loose N_mis consequitive pixels in a cluster"};
    DoubleProperty m_pixelEff{this, "MinPixelEfficiency", 0.90, "ITK: pixel efficiency (it depends on cluster eta; use smaller pixel efficiency)"};
    IntegerArrayProperty m_minToT{this, "ToTMinCut", {0,0,0,0,0,0,0}, "Minimum ToT cut [IBL, b-layer, L1, L2, Endcap, DBM, ITk extra"};

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

    bool m_IBLAbsent{true};

    mutable std::atomic_uint                            m_processedClusters{0};    //!< statistics output
    mutable std::atomic_uint                            m_modifiedOrigClusters{0}; //!< statistics output
    mutable std::atomic_uint                            m_splitOrigClusters{0};    //!< statistics output
    mutable std::atomic_uint                            m_splitProdClusters{0};    //!< statistics output
    mutable std::atomic_uint                            m_largeClusters{0};        //!< statistics output
    mutable std::atomic_bool                            m_printw{true};
  };

}

#endif // SICLUSTERIZATIONTOOL_MERGEDPIXELSALG_H
