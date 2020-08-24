/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          SimpleVertexClusterFinder.cxx - Description in header file
*********************************************************************/
#include "TrkVertexSeedFinderUtils/SimpleVertexClusterFinder.h"

namespace Trk
{
  
  SimpleVertexClusterFinder::SimpleVertexClusterFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
    m_weightThreshold( 0.0015        ) ,
    m_seedCutScale(    1.2           ) 
  {   
    //threshold to consider a bin for clusterization
    declareProperty("weightThreshold", m_weightThreshold );
    //cut on max bin in a cluster as a multiple of the threshold above
    declareProperty("seedCutScale"   , m_seedCutScale    );
  }
  
  
  // --------------------------------------------------------------------------------
  // Find vertex clusters of input histogram. Note that this is the public function that
  // Is called from the ImagingSeedFinder tool. Also, if the m_xbins, m_ybins, m_zbins,
  // is not exactly what they were in ImagingSeedFinder (length of hist_rs is not binstot), 
  // the code WILL and should fail. 
  std::vector<Amg::Vector3D> SimpleVertexClusterFinder::findVertexClusters( const VertexImage & image ) const
  {
    int binstot = image.getNBins();

    std::vector<Amg::Vector3D> vertices;    

    std::map<int,float> binsRemaining;

    for( int binnum = 0; binnum<image.getArraySize(); ++binnum ){ //Loop over all histogram bins

      float test_val= image.getHist()[binnum] / binstot; //divide by binstot because of scaling after fourier transforms
      if( test_val > m_weightThreshold) { //Tested value greater that thresshold
	binsRemaining[binnum] = test_val;
      }
    }

    std::vector<std::unique_ptr<Cluster> > clusts;
    while (!binsRemaining.empty()) {
      //get a new cluster, but check that it is filled.  Break if not.  in this case not all the bins above threshold are used because of the seed quality cut
      std::unique_ptr<Cluster> tmpClust = highestCluster( image, binsRemaining );
      if(!tmpClust->empty())
	clusts.push_back( std::move (tmpClust) );
      else {
	break;
      }
    }
    
    //all clusters found passed the check on seed cut in highestCluster method, so can safely add them all and delete them.  The vertices are returned with relative position (in distance space) to center of histogram
    for (std::unique_ptr<Cluster>& cl : clusts) {
      float xbin,ybin,zbin;
      getCenter(image, *cl, xbin,ybin,zbin);
      vertices.emplace_back( image.getRelPosX(xbin),
                                         image.getRelPosY(ybin),
                                         image.getRelPosZ(zbin) );
    }

    ATH_MSG_DEBUG( "returning " << vertices.size() << " clusters"  );
    return vertices;

  } //End findSimpleVertexClusterFinders

  // --------------------------------------------------------------------------------
  // Private functions below
  // --------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------
  // Get highest cluster in binsremaining map
  std::unique_ptr<SimpleVertexClusterFinder::Cluster>
  SimpleVertexClusterFinder::highestCluster( const VertexImage& image,
                                             std::map<int, float>& binsRemaining ) const
  {
    auto clust = std::make_unique<Cluster>();
    
    //find highest bin left above the seed cut scale.  for simple finding that doesnt need merging no point starting a cluster only to delete it later
    float maxWeight = m_weightThreshold*m_seedCutScale;
    std::map<int,float>::iterator maxIdx= binsRemaining.end();
    for(std::map<int,float>::iterator bit = binsRemaining.begin(); bit!= binsRemaining.end(); bit++) {
      if(bit->second > maxWeight) {
	maxIdx=bit;
	maxWeight= bit->second;
      }
    }
    
    if( maxIdx==binsRemaining.end() ) { //nothing above threshold
      return clust;
    } //start the cluster with the highest bin, removing it from future consideration
      clust->push_back(binContent( maxIdx->first, maxIdx->second ));
      binsRemaining.erase(maxIdx);
    
    
    //find more bins to add
    addBinsToCluster( image, *clust, 0, binsRemaining );
    
    return clust;
    
  } // End GethighestCluster
  

  // --------------------------------------------------------------------------------
  // Add bins to cluster
  void SimpleVertexClusterFinder::addBinsToCluster( const VertexImage& image,
                                                    Cluster& clust,
                                                    Cluster::size_type oldEdge,
                                                    std::map<int, float>& binsRemaining ) const
  {
    //plan:
    //loop over all "edge bins" = last set added by this method
    //get all their adjacent bins (that are in the binsRemaining?)
    //any adjacent bin gets put in a list
    //...bin could be picked by 2 bins or more from the edfe
    //...so run a duplicate removal
    //add all new bins to the cluster and update edgeStart
    //if we added any bins then run this method again
    
    int numadded = 0;
    Cluster::size_type newEdge = clust.size();
    //loop over all edges first
    Cluster::const_iterator currEnd = clust.end();
    for( Cluster::const_iterator bit= clust.begin()+oldEdge; bit!=currEnd; bit++) {
      
      //find adjacents, check if they are less and add them to the list
      std::vector<int> adjIdx = image.getAdjacentBins( bit->first );
      for(unsigned int i=0; i< adjIdx.size(); i++) {

        //try to find it in bins remaining
	std::map<int,float>::iterator mit= binsRemaining.find( adjIdx[i] );
	if( mit!= binsRemaining.end()) {
	  clust.push_back(binContent(adjIdx[i], mit->second)  );

	  if(numadded==0) {
            //keep track of the first bin added to the cluster on this call to method
	    newEdge = clust.size()-1;
	  }

	  numadded++;

	  binsRemaining.erase( mit );  // delete from remaining bins now
	  
	} 
      }
    }
    
    //if we added anything need to continue looking for more
    if(numadded > 0) {
      addBinsToCluster( image, clust, newEdge, binsRemaining);
    }
    
    
  } //End Add bins to cluster

  
  // --------------------------------------------------------------------------------
  // get center
  void SimpleVertexClusterFinder::getCenter( const VertexImage& image,
                                             const Cluster& cl,
                                             float & xpos,
                                             float & ypos,
                                             float & zpos) const
  {
    xpos=0;
    ypos=0;
    zpos=0;
    
    float totw=0;
    
    for (const binContent& bin : cl) {
      const double w = bin.second;
      totw += w;
      
      int x,y,z;
      image.getInvRMBin( bin.first, x, y, z);
      
      xpos+= ((float) x)*w;
      ypos+= ((float) y)*w;
      zpos+= ((float) z)*w;
      
    }
    
    //in bin space
    xpos= xpos/totw;
    ypos= ypos/totw;
    zpos= zpos/totw;
    
    //
  } //end get center
  

} //End Trk namespace
