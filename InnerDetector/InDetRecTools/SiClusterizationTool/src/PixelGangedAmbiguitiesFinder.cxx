/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// 
// Implementation for GlobalPositionMaker
//
//****************************************************************************

#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include <vector>
#include <algorithm>
#include <cmath>

namespace InDet{

using namespace InDet;

// Constructor with parameters:
PixelGangedAmbiguitiesFinder::PixelGangedAmbiguitiesFinder(
                                     const std::string &type,
				     const std::string &name,
				     const IInterface *parent) :
  AthAlgTool(type,name,parent)
{ 
  declareInterface<PixelGangedAmbiguitiesFinder>(this);
} 

StatusCode PixelGangedAmbiguitiesFinder::initialize() {
  ATH_CHECK(m_pixelDetEleCollKey.initialize());

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
// Execute method:
// Called by the PixelClusterization algorithms of InDetPrepRawDataFormation
  // A map containing the pairs of Pixel Clusters which shares the same 
  // ganged pixel is created.
  // Inputs are the cluster collection of a module, and the map to be filled.
  // Output is the map.
void PixelGangedAmbiguitiesFinder::execute(
                              PixelClusterCollection* collection,
                              PixelGangedClusterAmbiguities& theMap) const{
    if (collection->size()<2) return;

    ATH_MSG_DEBUG(collection->size() << " clusters");
    ATH_MSG_DEBUG("The map has " << theMap.size() << " entries already");

    IdentifierHash elementHash = collection->identifyHash();

    // Get detector info.
    // Find detector element for these RDOs

    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey);
    const InDetDD::SiDetectorElementCollection* pixelDetEle(*pixelDetEleHandle);
    if (not pixelDetEleHandle.isValid() or pixelDetEle==nullptr) {
      ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " is not available.");
      return;
    }
    const InDetDD::SiDetectorElement* element = pixelDetEle->getDetectorElement(elementHash);
    const InDetDD::PixelModuleDesign* design =(dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design()));
	  if (not design){
			ATH_MSG_ERROR("Dynamic cast failed at line "<<__LINE__<<" of PixelGangedAmbiguitiesFinder.cxx.");
			return;
	  }
    int rowsPerFE = design->rows()/2;
    const PixelID* pixelID =(dynamic_cast<const PixelID*>(element->getIdHelper()));
    if (not pixelID){
			ATH_MSG_ERROR("Dynamic cast failed at line "<<__LINE__<<" of PixelGangedAmbiguitiesFinder.cxx.");
			return;
	  }
    //Pointer list of clusters to be removed
    std::vector<PixelClusterCollection::iterator> rmList;

    PixelClusterCollection::iterator cluster=collection->begin();
    PixelClusterCollection::iterator clend=collection->end();

    for( ; cluster!=clend ; ++cluster){
      // check if cluster has not already been flagged to be deleted
      if ( find(rmList.begin(),rmList.end(),cluster)!=rmList.end() ) continue;

      // extract from the list of cluster rdo the ganged ones.      
      bool hasGanged = false;
      int rmin1=999, cmin1=999; // bottom left corner of cluster
      std::vector<Identifier> gangedPixels;
      const std::vector<Identifier>& RDOs = (*cluster)->rdoList();

      std::vector<Identifier>::const_iterator rdo=RDOs.begin();
      std::vector<Identifier>::const_iterator rdoend=RDOs.end();      
      for( ; rdo < rdoend; rdo++){
        Identifier gangedID;
        bool ganged = isGanged(*rdo,element,gangedID);
        if(ganged){
	  hasGanged = true;
          gangedPixels.push_back(gangedID);
	}
        int thiscol=pixelID->eta_index(*rdo);
        if (thiscol<cmin1) cmin1=thiscol;
	int thisrow=pixelID->phi_index(*rdo);
	if (thisrow<rmin1) rmin1=thisrow;

      }
      
      // set ganged pixels flag
      (*cluster)->setGangedPixel(hasGanged);
      // find out if any other cluster shares any of the ganged pixels

      if(hasGanged){

	ATH_MSG_DEBUG("Ganged pixel, find combi...");
	std::vector<Identifier>::const_iterator gangedPixelsBegin = gangedPixels.begin();
	std::vector<Identifier>::const_iterator gangedPixelsEnd = gangedPixels.end();
	PixelClusterCollection::iterator cluster2=cluster+1;
	for( ; cluster2!=clend; ++cluster2){
          ATH_MSG_DEBUG("Comparing " 
                        << std::hex << (*cluster)->identify() 
                        << " and " << (*cluster2)->identify()
                        << std::dec);
	  bool sharedGanged = false;
          int rmin2=999, cmin2=999; // bottom left corner of cluster2
	  const std::vector<Identifier>& rdos2 = (*cluster2)->rdoList();
	  std::vector<Identifier>::const_iterator rdo2=rdos2.begin();
	  std::vector<Identifier>::const_iterator rdo2end=rdos2.end();
	  for( ; rdo2!=rdo2end; rdo2++) {
            if (!sharedGanged) {
	    for( std::vector<Identifier>::const_iterator gangedRDOs=gangedPixelsBegin;
		 gangedRDOs < gangedPixelsEnd; gangedRDOs++ ){
	      if(*rdo2 == *gangedRDOs){
		sharedGanged = true;
		break;
	      }
	    }
            }
            int thiscol=pixelID->eta_index(*rdo2);
            if (thiscol<cmin2) cmin2=thiscol;
            int thisrow=pixelID->phi_index(*rdo2);
            if (thisrow<rmin2) rmin2=thisrow;
	    //if ( sharedGanged ) break;
	  }
  
	  if(sharedGanged){
	    // ganged clusters with just one row are likely to
	    // be fake if the ganged image of the same pixel is
	    // part of a cluster with more rows. 
            int drow1=lrint((**cluster).width().colRow().x() );
            int drow2=lrint((**cluster2).width().colRow().x() );
            int dcol1=lrint((**cluster).width().colRow().y() );
            int dcol2=lrint((**cluster2).width().colRow().y() );

            //std::cout << "drow1: " << drow1 << " drow2: " << drow2 << " dcol1: " << dcol1 << " dcol2: " << dcol2 << " isfake1: " << (*cluster)->isFake() << " isfake2: " << (*cluster2)->isFake() << std::endl; 
	    if ( (drow1==1 && drow2>1 && drow2<4) || (dcol2>dcol1 && cmin2<=cmin1 && cmin2+dcol2>=cmin1+dcol1)){ 
	      // cluster 1 is likely to be fake
	      if (m_internalSolving) {
		rmList.push_back(cluster);
		ATH_MSG_DEBUG(std::hex 
                              << ": deleted " << (*cluster)->identify() 
                              << std::dec);
		break;
	      }
	      else (*cluster)->setFake(true);
	    }
	    else if ( (drow1>1 && drow1<4 && drow2==1) || (dcol1>dcol2 && cmin1<=cmin2 && cmin1+dcol1>=cmin2+dcol2)){
	      // cluster 2 is likely to be fake
	      if (m_internalSolving) {
		rmList.push_back(cluster2);
		ATH_MSG_DEBUG(std::hex 
                              << ": deleted " << (*cluster2)->identify() 
                              << std::dec);
		continue;
	      }
	      else (*cluster2)->setFake(true);
	    } 
            else if ( drow2>1 || drow1>1 ) { 
              if (cmin1==cmin2){
                rdo=RDOs.begin();
                std::vector<std::vector<int> > myvec(dcol1);
                for( ; rdo < rdoend; rdo++){
                  int row=pixelID->phi_index(*rdo);
                  int col=pixelID->eta_index(*rdo);
                  myvec[col-cmin1].push_back(row);
                }
                rdo=rdos2.begin();
                int dcol2=lrint((**cluster2).width().colRow().y() );
                std::vector<std::vector<int> > myvec2(dcol2);
                for( ; rdo < rdo2end; rdo++){
                  int row=pixelID->phi_index(*rdo);
                  int col=pixelID->eta_index(*rdo);
                  myvec2[col-cmin2].push_back(row);
                }
                for (int i=0;i<(int)myvec.size();i++) if (!myvec[i].empty()) std::sort(myvec[i].begin(),myvec[i].end());
                for (int i=0;i<(int)myvec2.size();i++) if (!myvec2[i].empty()) std::sort(myvec2[i].begin(),myvec2[i].end());
                int nvictory1=0,nvictory2=0;
                for (int i=0;i<(int)myvec.size();i++){
                  int maxsize1=0,maxsize2=0,maxsizeleft1=0,maxsizeright1=0,maxsizeleft2=0,maxsizeright2=0,thissize=0,thissizeright=0;
                  bool hasgap1=false,hasgap2=false;
                  for (int j=0;j<(int)myvec[i].size();j++){
                    if (j!=0 && thissize==0) hasgap1=true;   // there is a gap between the pixels in this column

                    thissize++;
                    if (myvec[i][j]>=rowsPerFE) thissizeright++;

                    if (thissize>maxsize1) maxsize1=thissize;
                    if (myvec[i][j]<rowsPerFE && thissize>maxsizeleft1) maxsizeleft1=thissize;
                    if (myvec[i][j]>=rowsPerFE && thissizeright>maxsizeright1) maxsizeright1=thissizeright;
                    if (j==(int)myvec[i].size()-1 || myvec[i][j+1]!=myvec[i][j]+1) thissize=thissizeright=0;
                  }
                  thissize=thissizeright=0;
                  for (int j=0;j<(int)myvec2[i].size();j++){
                    if (j!=0 && thissize==0) hasgap2=true;
                    thissize++;
                    if (myvec2[i][j]>=rowsPerFE) thissizeright++;
                    if (thissize>maxsize2) maxsize2=thissize;
                    if (myvec2[i][j]<rowsPerFE && thissize>maxsizeleft2) maxsizeleft2=thissize;
                    if (myvec2[i][j]>=rowsPerFE && thissizeright>maxsizeright2) maxsizeright2=thissizeright;
                    if (j==(int)myvec2[i].size()-1 || myvec2[i][j+1]!=myvec2[i][j]+1) thissize=thissizeright=0;
                    
                  }
                  //std::cout << "maxsize1: " << maxsize1 << " maxsize2: " << maxsize2 << " hasgap1: " << hasgap1 << " hasgap2: " << hasgap2 << std::endl; 
                  if (maxsize1>maxsize2 && maxsizeleft1>=maxsizeleft2 && maxsizeright1>=maxsizeright2 && !hasgap1) nvictory1++;
                  else if (maxsize2>maxsize1 && maxsizeleft2>=maxsizeleft1 && maxsizeright2>=maxsizeright1 && !hasgap2) nvictory2++;
                }
                //std::cout << "nvictory1: " << nvictory1 << " nvictory2: " << nvictory2 << std::endl;

                // check if fake image of big clusters in ganged region
	        //if ( (rmin1<rmin2 && (rmin2+drow2-1)<rowsPerFE )
		//   || (rmin1>rmin2 && rmin2>=rowsPerFE ) ) {
                if (nvictory1>nvictory2){
                  // cluster 2 is likely to be fake
                  if (m_internalSolving) {
                    rmList.push_back(cluster2);
                    ATH_MSG_DEBUG(std::hex
                                  << ": deleted " << (*cluster2)->identify()
                                  << std::dec);
                    continue;
                  }
                  else {
                    (*cluster2)->setFake(true);
                    //std::cout << "cluster2 fake!" << std::endl;
                  }
                }
                else if (nvictory2>nvictory1){
		  // cluster 1 is likely to be fake
		  if (m_internalSolving) {
		    rmList.push_back(cluster);
		    ATH_MSG_DEBUG(std::hex 
                                  << ": deleted " << (*cluster)->identify() 
                                  << std::dec);
                    continue;
                  }
                  else {
                    (*cluster)->setFake(true);
                    //std::cout << "cluster1 fake!" << std::endl;
                  }
                }  
              }
            }
	    // this point is reached:
	    // - always if m_internalSolving is false
	    // - in case no cluster could be removed
	    // in both cases the clusters are flagged as ambiguous
	    // and an entry in the ambiguity map is added. 

            //GP: temporary not optimal solution not to flag two 
            //subclusters within a cluster as ganged to each other
            //TO BE FIXED WITH A BETTER TREATMENT
            if (!((*cluster)->isSplit() && (*cluster2)->isSplit()))
            {
              (*cluster)->setAmbiguous(true);
              (*cluster2)->setAmbiguous(true);
              // a map is a sorted container with the first element
              // of the pair as key, for efficient lookup (~logN) need
              // to add both permutations
              theMap.insert(std::make_pair(*cluster,*cluster2));
              theMap.insert(std::make_pair(*cluster2,*cluster));
              ATH_MSG_DEBUG(std::hex 
                            << ": added ambiguity entry"
                            << std::dec);
            }
	  }
	}
      }
    }
    ATH_MSG_DEBUG("The map has " << theMap.size() << " entries ");

    // to remove clusters from the collection, the vector of iterators need to be 
    // sorted. Moreover, when preceeding iterators are removed, one must take into
    // account the iterator position is changed.
    int rmNumber=0;
    std::vector<PixelClusterCollection::iterator>::iterator rmit=rmList.begin();
    std::vector<PixelClusterCollection::iterator>::iterator rmend=rmList.end();
    std::sort(rmit,rmend);
    for ( ; rmit!=rmend ; ++rmit){
      ATH_MSG_DEBUG("Removed " << rmNumber+1 << " cluster: "  
                    << std::hex << (*(*rmit-rmNumber))->identify() << std::dec);
      collection->erase(*rmit-rmNumber); // The position of the iterator
      rmNumber++;
    }    
    ATH_MSG_DEBUG(rmNumber << " fake clusters from ganged pixel have been removed");

}
  


// Determines if a pixel cell (whose identifier is the first argument) is 
// a ganged pixel. If this is the case, the last argument assumes the 
// value of the identifier of the cell it is ganged with. 
// The second argument is the pixel module the hit belongs to.

bool PixelGangedAmbiguitiesFinder::isGanged(const Identifier& rdoID,
					    const InDetDD::SiDetectorElement* element,
					    Identifier& gangedID) const
{
  InDetDD::SiCellId cellID = element->cellIdFromIdentifier (rdoID);
  if (element->numberOfConnectedCells (cellID) > 1) {
    InDetDD::SiCellId gangedCellID = element->connectedCell (cellID,1);
    if ( gangedCellID==cellID ) gangedCellID = element->connectedCell (cellID,0);
    gangedID = element->identifierFromCellId (gangedCellID);
    return true;
  } else {
    gangedID = Identifier();
    return false;
  }
}

}
