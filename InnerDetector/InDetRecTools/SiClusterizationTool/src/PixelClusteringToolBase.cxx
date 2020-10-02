/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelClusteringToolBase.cxx
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SiClusterizationTool/PixelClusteringToolBase.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "InDetIdentifier/PixelID.h"


namespace InDet
{

  PixelClusteringToolBase::PixelClusteringToolBase(const std::string &type,
      const std::string &name,
      const IInterface *parent) :
      base_class(type,name,parent)
  {
  }

  StatusCode PixelClusteringToolBase::initialize()
  {
    ATH_MSG_INFO("initialize()");
    if ( m_clusterMaker.retrieve().isFailure() )
    {
      ATH_MSG_FATAL(m_clusterMaker.propertyName() << ": Failed to retrieve tool " << m_clusterMaker.type());
      return StatusCode::FAILURE;
    }
    else
    {
      ATH_MSG_INFO(m_clusterMaker.propertyName() << ": Retrieved tool " << m_clusterMaker.type());
    }

   StatusCode sc = m_summaryTool.retrieve();
   if (sc.isFailure() || !m_summaryTool) {
      ATH_MSG_WARNING(m_summaryTool.type() << " not found! ");
      if (m_useModuleMap ) {
	ATH_MSG_FATAL(m_summaryTool.type() << " is compulsory with this tool configuration");
	return StatusCode::FAILURE;
      }
   }
   else{
     ATH_MSG_INFO("Retrieved service " <<  m_summaryTool.type());
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
   const InDetDD::SiDetectorElement* /*element*/,
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
