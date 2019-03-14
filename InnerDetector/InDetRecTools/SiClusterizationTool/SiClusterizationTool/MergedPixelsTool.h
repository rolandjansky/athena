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

#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"

#include "Identifier/Identifier.h"

#include "GaudiKernel/ServiceHandle.h"

// forward declare not possible (typedef)
#include "InDetPrepRawData/PixelClusterCollection.h"

#include <atomic>
#include <vector>

class IIncidentSvc;
class PixelID;
//class StoreGateSvc;
class IBLParameterSvc;

namespace InDetDD {
    class SiDetectorElement;
    class SiDetectorManager;
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

        virtual ~MergedPixelsTool();

  // Called by the PixelPrepRawDataFormation algorithm once for every pixel 
  // module (with non-empty RDO collection...). 
  // It clusters together the RDOs with a pixell cell side in common.
        virtual PixelClusterCollection *clusterize
            (const InDetRawDataCollection<PixelRDORawData>& RDOs,
            const InDetDD::SiDetectorManager& manager,
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
        //		const Identifier& elementID,
        //		const PixelID& pixelID,
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
            InDetDD::SiDetectorElement* element,
            const PixelID& pixelID) const;

	// Functions for merging broken cluster segments (to be used for upgrade studies)
	// ITk: This function checks if two barrel clusters are potentially a single cluster which is broken into two pieces  
	bool mergeTwoBrokenClusters(const std::vector<Identifier>& group1, 
				    const std::vector<Identifier>& group2,
				    InDetDD::SiDetectorElement* element,
				    const PixelID& pixelID) const;
	// ITk: this function checks if two to-be-merged barrel proto-clusters have sizeZ consistent with cluster positions 
	bool mergeTwoClusters(const std::vector<Identifier>& group1, 
			      const std::vector<Identifier>& group2,
			      InDetDD::SiDetectorElement* element,
			      const PixelID& pixelID) const;

	// ITk: checkSizeZ compares cluster sizeZ with expected cluster size for this cluster position (+/-200 mm for beam spread)
	// checkSizeZ()=-1 if cluster is too small
	// checkSizeZ()=0 if cluster sizeZ is within allowed range
        // checkSizeZ()=1 if cluster is too large
	// in the future, it may be changed to return deltaSizeZ 
	int checkSizeZ(int colmin, int colmax, int row, InDetDD::SiDetectorElement* element) const;
	// this function returns expected sizeZ
	int expectedSizeZ(int colmin, int colmax, int row, InDetDD::SiDetectorElement* element) const;
	// this function returns size of the maximum gap between two cluster fragments  
	int maxGap(int colmin, int colmax, int row, InDetDD::SiDetectorElement* element) const;
	//------- end of declaration of new functions


        ServiceHandle<IIncidentSvc>                         m_incidentSvc;   //!< IncidentSvc to catch begin of event and end of envent
	ServiceHandle<IBLParameterSvc>                      m_IBLParameterSvc;        
        bool						                        m_emulateSplitter;      //!< don't split - only emulate the split    
        unsigned int                                        m_minSplitSize;         //!< minimum split size, regulates also the cluster splitting
        unsigned int                                        m_maxSplitSize;         //!< minimum split size, regulates also the cluster splitting
        double                                              m_minSplitProbability;  //!< minimal split probability
        ToolHandle<InDet::IPixelClusterSplitProbTool>       m_splitProbTool;        //!< ToolHandle for the split probability tool
        ToolHandle<InDet::IPixelClusterSplitter>            m_clusterSplitter;      //!< ToolHandle for the split probability tool
	bool						    m_doIBLSplitting;
	bool						    m_IBLAbsent;

	bool                                                m_doMergeBrokenClusters; // ITk: switch to turn ON/OFF merging of broken clusters
	bool                                                m_doRemoveClustersWithToTequalSize; // ITk: switch to remove clusters with ToT=size
	bool                                                m_doCheckSizeBeforeMerging; // ITk: switch to check size of to-be-merged clusters
	double                                              m_beam_spread; // ITK: size of the luminous region, need to check cluster size
	double                                              m_lossThreshold; // maximum probability to loose N_mis consequitive pixels in a cluster 
	double                                              m_pixelEff;      // pixel efficiency (it depends on cluster eta; use smaller pixel efficiency) 
	
        std::string                                         m_splitClusterMapName;//No longer used Remove later //!< split cluster ambiguity map

        mutable std::atomic_uint                            m_processedClusters;    //!< statistics output
        mutable std::atomic_uint                            m_modifiedOrigClusters; //!< statistics output
        mutable std::atomic_uint                            m_splitOrigClusters;    //!< statistics output
        mutable std::atomic_uint                            m_splitProdClusters;    //!< statistics output
        mutable std::atomic_uint                            m_largeClusters;        //!< statistics output
        mutable std::atomic_bool                            m_printw;
        std::vector<int>          			    m_minToT;               /**< ToT cut */
        //ServiceHandle< StoreGateSvc >                       m_detStore;
        //const PixelID*                                      m_idHelper;
    };

}

#endif // SICLUSTERIZATIONTOOL_MERGEDPIXELSALG_H
