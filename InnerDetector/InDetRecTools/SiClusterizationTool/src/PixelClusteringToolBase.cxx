/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelClusteringToolBase.cxx
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SiClusterizationTool/PixelClusteringToolBase.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

namespace InDet
{

  PixelClusteringToolBase::PixelClusteringToolBase(const std::string &type,
      const std::string &name,
      const IInterface *parent) :
      AthAlgTool(type,name,parent),
      m_clusterMaker("InDet::ClusterMakerTool"),
      m_posStrategy(0),
      m_errorStrategy(1),
      m_acceptDiagonalClusters(1),
      m_splitClusters(0),
      m_summarySvc("PixelConditionsSummarySvc", name),
      m_useModuleMap(true),
      m_usePixelMap(true)
  {
    declareProperty("globalPosAlg" , m_clusterMaker);
    declareProperty("posStrategy"  , m_posStrategy);
    declareProperty("errorStrategy", m_errorStrategy);
    declareProperty("acceptDiagonalClusters"  , m_acceptDiagonalClusters);
    declareProperty("splitClusters", m_splitClusters);
    declareProperty("UsePixelModuleMap",m_useModuleMap,"Use bad modules map");
    declareProperty("UseSpecialPixelMap",m_usePixelMap,"Use bad pixel map");
    declareProperty("PixelConditionsSummarySvc",m_summarySvc);
  }

  StatusCode PixelClusteringToolBase::initialize()
  {
    msg(MSG::INFO) << "initialize()" << endreq;
    if ( m_clusterMaker.retrieve().isFailure() )
    {
      msg(MSG::FATAL) << m_clusterMaker.propertyName() << ": Failed to retrieve tool " << m_clusterMaker.type() << endreq;
      return StatusCode::FAILURE;
    }
    else
    {
      msg(MSG::INFO) << m_clusterMaker.propertyName() << ": Retrieved tool " << m_clusterMaker.type() << endreq;
    }

   StatusCode sc = m_summarySvc.retrieve();
   if (sc.isFailure() || !m_summarySvc) {
      msg(MSG::WARNING) <<  m_summarySvc.type() << " not found! "<< endreq;
      if ( m_usePixelMap || m_useModuleMap ) {
	msg(MSG::FATAL) << m_summarySvc.type() << " is compulsory with this tool configuration" << endreq;
	return StatusCode::FAILURE;
      }
   }
   else{
      msg(MSG::INFO) << "Retrieved service " <<  m_summarySvc.type() << endreq;
   }
   
    return StatusCode::SUCCESS;
  }

   
//-------------------------------------------------------------------
  // Determines whether the pixel cell whose identifier is passed as second 
  // argument should be added to the list passed as first argument  
  //
  // This method is called inside the clusterize() method in order to 
  // determine if the pixel cell should be added to the cluster
  // The last argument is the Pixel Identifier helper class
  //
  // Different clustering strategies are foreseen:
  // 
  // m_acceptDiagonalClusters 
  //   = 0: Add pixels which have a side in common with a pixel cell 
  //        in the list
  //   = 1: A common corner suffices
  // m_splitClusters 
  //   = 0: no limitation on cluster size 
  //   = 1: clusters larger than expected (in Rphi and/or z) are rejected
  //   = 2: no clustering at all along columns! 

bool 
PixelClusteringToolBase::areNeighbours(const std::vector<Identifier>& group, const Identifier& rdoID,
   InDetDD::SiDetectorElement* /*element*/,
   const PixelID& pixelID) const{
    std::vector<Identifier>::const_iterator groupBegin = group.begin();
    std::vector<Identifier>::const_iterator groupEnd = group.end();
    int row2 = pixelID.phi_index(rdoID);
    int col2 = pixelID.eta_index(rdoID);
    int maxZsize=999;
    if(m_splitClusters == 1){
       int etamodule = pixelID.eta_module(rdoID);
       int isBarrel = pixelID.barrel_ec(rdoID);
       if(isBarrel == 0){
          // this goes from 0 (central stave) to 6
          int dz = abs(etamodule-6);
          if(dz<4){ maxZsize=2; }
          else{ maxZsize=3; }
       }else{
          maxZsize = 2;
       }
    }
    if(m_splitClusters == 2) maxZsize = 2;
 
    int rowmin = row2;
    int rowmax = row2;
    int colmin = col2;
    int colmax = col2;
    bool match=false;
    while (groupBegin!=groupEnd)
    {
	Identifier id = *groupBegin;
	int row1 = pixelID.phi_index(id);
	int col1 = pixelID.eta_index(id);
      if(row1 > rowmax) rowmax = row1;
      int deltarow = abs(row2-row1);
      int deltacol = abs(col2-col1);

      // a side in common
      if(deltacol+deltarow < 2) match = true;
      if(m_acceptDiagonalClusters != 0 && deltacol == 1 
                                       && deltarow == 1) match = true;
 
     // check cluster size
      if(m_splitClusters != 0){
        if(row1 > rowmax) rowmax = row1;
        if(row1 < rowmin) rowmin = row1;
        if(col1 > colmax) colmax = row1;
        if(col1 < colmin) colmin = row1;  
      }

	++groupBegin;
    }
    if(match && m_splitClusters != 0){
       if(colmax-colmin > maxZsize-1) match = false;
       if(rowmax-rowmin > 1) match = false;
    }

    return match;
}

bool PixelClusteringToolBase::isDuplicated
(const std::vector<Identifier>& group,
 std::vector<int>& lvl1vec,
 const Identifier& rdoID,
 int lvl1,
 const PixelID& pixelID) const
{

    std::vector<Identifier>::const_iterator groupBegin = group.begin();
    std::vector<Identifier>::const_iterator groupEnd = group.end();
    std::vector<int>::iterator lvl1It = lvl1vec.begin();

    int row2 = pixelID.phi_index(rdoID);
    int col2 = pixelID.eta_index(rdoID);
    bool duplicate = false;
    while (groupBegin!=groupEnd)
    {
	Identifier id = *groupBegin;
	int row1 = pixelID.phi_index(id);
	int col1 = pixelID.eta_index(id);
 
       // duplicate
	if(row1 == row2 && col1 == col2){
	  duplicate = true;
	  if( *lvl1It < lvl1 ) *lvl1It = lvl1;
        }
	++lvl1It;
	++groupBegin;
    }

    return duplicate;
}


  StatusCode PixelClusteringToolBase::finalize()
  {
    return StatusCode::SUCCESS;
  }

}
