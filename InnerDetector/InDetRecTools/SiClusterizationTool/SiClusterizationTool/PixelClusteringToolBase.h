/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelClusteringToolBase.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SICLUSTERIZATIONTOOL_PIXELCLUSTERINGTOOLBASE_H
#define SICLUSTERIZATIONTOOL_PIXELCLUSTERINGTOOLBASE_H

//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/MsgStream.h"

class IInDetConditionsSvc;
template <class T> class ServiceHandle;

namespace InDet {

class ClusterMakerTool;

class PixelClusteringToolBase : public AthAlgTool, virtual public IPixelClusteringTool 
{

public:
     PixelClusteringToolBase
           (const std::string& type,
	    const std::string& name,
	    const IInterface* parent);
     
     virtual ~PixelClusteringToolBase() {};
     virtual StatusCode initialize();
     virtual StatusCode finalize();

protected:  
   
  // Determines whether the pixel cell whose identifier is passed as second 
  // argument has a side in common with one of the pixel cells belonging to 
  // the list passed as first argument (which are the pixel cells of 
  // an existing cluster, most likely).
  // This method is called inside the clusterize() method in order to 
  // determine if the pixel cell should be added to the cluster
  // The last argument is the Pixel Identifier helper class
     bool areNeighbours(const std::vector<Identifier>& group, 
                        const Identifier& rdoID,
                        InDetDD::SiDetectorElement* element,
   	                const PixelID& pixelID) const;

     // Check wether the RDO is a duplicate of one already in the cluster
     //       if it is, choose the larger of the two LVL1 for the list
     //       
     bool isDuplicated(const std::vector<Identifier>& group, 
                       std::vector<int>& lvl1vec, 
                       const Identifier& rdoID,
   	               int lvl1,
                       const PixelID& pixelID) const;

     ToolHandle< ClusterMakerTool > m_clusterMaker;

     int m_posStrategy;
     int m_errorStrategy;
     int m_acceptDiagonalClusters;
     int m_splitClusters;
     ServiceHandle<IInDetConditionsSvc> m_summarySvc;
     bool m_useModuleMap;
     bool m_usePixelMap;
     //     mutable MsgStream m_log;

};

}
#endif // SICLUSTERIZATIONTOOL_PIXELCLUSTERINGTOOLBASE_H
