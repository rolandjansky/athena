/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "TrkSurfaces/RectangleBounds.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"

#include <atomic>
#include <vector>

class PixelID;

namespace InDetDD {
  class SiDetectorElement;
}

namespace InDet {
  
  class rowcolID {
  public:
    
    rowcolID(int ncl, int row, int col, int tot, int lvl1, Identifier id):
      NCL(ncl), ROW(row), COL(col), TOT(tot), LVL1(lvl1), ID(id) {};
      
    ~rowcolID() {};      
    
    int        NCL;
    int        ROW;
    int        COL;
    int        TOT;
    int       LVL1;
    Identifier ID ;
  };

  class network {
  public:
    network():
      NC(0), CON({0,0,0,0}) {};
      
    ~network() {};

    int               NC;
    std::array<int,4> CON;
  };
  
  const auto pixel_less = [] (rowcolID const&  id1,rowcolID const& id2) -> bool {
    if(id1.COL == id2.COL) 
      return id1.ROW < id2.ROW;
    return id1.COL < id2.COL;
  };
  
  
  class PixelCluster;
  class IPixelClusterSplitter;
  class IPixelClusterSplitProbTool;
  
  class MergedPixelsTool : public PixelClusteringToolBase
  {
  public:


    // Constructor with parameters:
    MergedPixelsTool(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

    virtual ~MergedPixelsTool() = default;

    // Called by the PixelPrepRawDataFormation algorithm once for every pixel 
    // module (with non-empty RDO collection...). 
    // It clusters together the RDOs with a pixell cell side in common.
    virtual PixelClusterCollection *clusterize(const InDetRawDataCollection<PixelRDORawData>& RDOs,
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
    
    
                       
    // Checks if RDOs would be merged. This is based on a 4 cell connected component finding. 
    PixelClusterCollection* clusterizeFast(const InDetRawDataCollection<PixelRDORawData> &collection,
                                           const PixelID& pixelID) const;                   

                                          
    void addClusterNumber(const int& r, 
                          const int& Ncluster,
                          const std::vector<network>& connections,    
                          std::vector<rowcolID>& collectionID) const;
                          
    bool checkDuplication(const PixelID& pixelID,
                          const Identifier& rdoID, 
                          const int& lvl1, 
                          std::vector<rowcolID>& collectionID) const;
                       

    ServiceHandle<IBLParameterSvc>                      m_IBLParameterSvc;
    /// for cluster splitting
    BooleanProperty m_emulateSplitter{this, "EmulateSplitting", false, "don't split - only emulate the split"};
    UnsignedIntegerProperty m_minSplitSize{this, "MinimalSplitSize", 1, "minimum split size, regulates also the cluster splitting"};
    UnsignedIntegerProperty m_maxSplitSize{this, "MaximalSplitSize", 1000, "minimum split size, regulates also the cluster splitting"};
    DoubleProperty m_minSplitProbability{this, "MinimalSplitProbability", 0., "minimal split probability"};
    ToolHandle<InDet::IPixelClusterSplitProbTool> m_splitProbTool{this, "SplitProbTool", "", "ToolHandle for the split probability tool"};
    ToolHandle<InDet::IPixelClusterSplitter> m_clusterSplitter{this, "ClusterSplitter", "", "ToolHandle for the split probability tool"};
    BooleanProperty m_doIBLSplitting{this, "DoIBLSplitting", false};
    BooleanProperty m_doFastClustering{this, "DoFastClustering", false};
    BooleanProperty m_checkDuplicatedRDO{this, "CheckDuplicatedRDO", false, "Check duplicated RDOs using isDuplicated method"};

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
