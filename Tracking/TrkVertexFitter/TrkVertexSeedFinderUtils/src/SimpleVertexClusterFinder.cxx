/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          SimpleVertexClusterFinder.cxx - Description in header file
*********************************************************************/
#include "TrkVertexSeedFinderUtils/SimpleVertexClusterFinder.h"

namespace Trk
{
  
  SimpleVertexClusterFinder::SimpleVertexClusterFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_weightThreshold( 0.0015        ) ,
    m_seedCutScale(    1.2           ) 
  {   
    //threshold to consider a bin for clusterization
    declareProperty("weightThreshold", m_weightThreshold );
    //cut on max bin in a cluster as a multiple of the threshold above
    declareProperty("seedCutScale"   , m_seedCutScale    );
    declareInterface<IVertexClusterFinder>(this);
  }
  
  SimpleVertexClusterFinder::~SimpleVertexClusterFinder() {}
  
  StatusCode SimpleVertexClusterFinder::initialize() 
  { 
    //no initializiation needed
    msg(MSG::INFO) << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  StatusCode SimpleVertexClusterFinder::finalize() 
  {
    msg(MSG::INFO)  << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;
  }


  // --------------------------------------------------------------------------------
  // Find vertex clusters of input histogram. Note that this is the public function that
  // Is called from the ImagingSeedFinder tool. Also, if the m_xbins, m_ybins, m_zbins,
  // is not exactly what they were in ImagingSeedFinder (length of hist_rs is not binstot), 
  // the code WILL and should fail. 
  std::vector<Amg::Vector3D> SimpleVertexClusterFinder::findVertexClusters( const VertexImage & image ){

    // This cluster finder needs a copy of the image so it can access it throughout many sub-functions
    // Not needed for general cluster finders
    // The image only stores a pointer to the memory location of the actual histogram, so cheap to copy
    m_image = image;

    int binstot = m_image.getNBins();

    std::vector<Amg::Vector3D> vertices;    

    std::map<int,float> *binsRemaining = new std::map<int,float>();

    for( int binnum = 0; binnum<m_image.getArraySize(); ++binnum ){ //Loop over all histogram bins

      float test_val= m_image.getHist()[binnum] / binstot; //divide by binstot because of scaling after fourier transforms
      if( test_val > m_weightThreshold) { //Tested value greater that thresshold
	(*binsRemaining)[binnum] = test_val;
      }
    }
    

    std::vector< Cluster*> *clusts = new std::vector<Cluster*>();
    while (binsRemaining->size() > 0) {
      //get a new cluster, but check that it is filled.  Break if not.  in this case not all the bins above threshold are used because of the seed quality cut
      Cluster* tmpClust = highestCluster( binsRemaining );
      if(tmpClust->size() > 0)
	clusts->push_back( tmpClust );
      else {
	delete tmpClust; //tmpClust was still created with new and need to delete now before losing track
	break;
      }
    }
    
    //dont need the map anymore.
    delete binsRemaining;

    //all clusters found passed the check on seed cut in highestCluster method, so can safely add them all and delete them.  The vertices are returned with relative position (in distance space) to center of histogram
    for(std::vector< Cluster*>::iterator cit = clusts->begin(); cit!=clusts->end(); cit++) {
      float xbin,ybin,zbin;
      getCenter(*cit, xbin,ybin,zbin);
      vertices.push_back( Amg::Vector3D( image.getRelPosX(xbin),
                                         image.getRelPosY(ybin),
                                         image.getRelPosZ(zbin) ) );
      delete *cit;      
    }

    //dont need the cluster vector anymore.  all the elements have been deleted in above loop
    delete clusts;

    msg(MSG::DEBUG) << "returning " << vertices.size() << " clusters" << endmsg;
    return vertices;

  } //End findSimpleVertexClusterFinders

  // --------------------------------------------------------------------------------
  // Private functions below
  // --------------------------------------------------------------------------------

  // --------------------------------------------------------------------------------
  // Get highest cluster in binsremaining map
  SimpleVertexClusterFinder::Cluster * SimpleVertexClusterFinder::highestCluster( std::map<int, float> *binsRemaining ){

    Cluster *clust = new Cluster;
    
    //find highest bin left above the seed cut scale.  for simple finding that doesnt need merging no point starting a cluster only to delete it later
    float maxWeight = m_weightThreshold*m_seedCutScale;
    std::map<int,float>::iterator maxIdx= binsRemaining->end();
    for(std::map<int,float>::iterator bit = binsRemaining->begin(); bit!= binsRemaining->end(); bit++) {
      if(bit->second > maxWeight) {
	maxIdx=bit;
	maxWeight= bit->second;
      }
    }
    
    if( maxIdx==binsRemaining->end() ) { //nothing above threshold
      return clust;
    } else { //start the cluster with the highest bin, removing it from future consideration
      clust->push_back(binContent( maxIdx->first, maxIdx->second ));
      binsRemaining->erase(maxIdx);
    }
    
    //find more bins to add
    addBinsToCluster( clust, 0, binsRemaining );
    
    return clust;
    
  } // End GethighestCluster
  

  // --------------------------------------------------------------------------------
  // Add bins to cluster
  void SimpleVertexClusterFinder::addBinsToCluster( Cluster *clust, Cluster::size_type oldEdge, std::map<int, float> * binsRemaining ){
    
    //plan:
    //loop over all "edge bins" = last set added by this method
    //get all their adjacent bins (that are in the binsRemaining?)
    //any adjacent bin gets put in a list
    //...bin could be picked by 2 bins or more from the edfe
    //...so run a duplicate removal
    //add all new bins to the cluster and update edgeStart
    //if we added any bins then run this method again
    
    int numadded = 0;
    Cluster::size_type newEdge = clust->size();
    //loop over all edges first
    Cluster::iterator currEnd = clust->end();
    for( Cluster::iterator bit= clust->begin()+oldEdge; bit!=currEnd; bit++) {
      
      //find adjacents, check if they are less and add them to the list
      std::vector<int> adjIdx = m_image.getAdjacentBins( bit->first );
      for(unsigned int i=0; i< adjIdx.size(); i++) {

        //try to find it in bins remaining
	std::map<int,float>::iterator mit= binsRemaining->find( adjIdx[i] );
	if( mit!= binsRemaining->end()) {
	  clust->push_back(binContent(adjIdx[i], mit->second)  );

	  if(numadded==0) {
            //keep track of the first bin added to the cluster on this call to method
	    newEdge = clust->size()-1;
	  }

	  numadded++;

	  binsRemaining->erase( mit );  // delete from remaining bins now
	  
	} 
      }
    }
    
    //if we added anything need to continue looking for more
    if(numadded > 0) {
      addBinsToCluster( clust, newEdge, binsRemaining);
    }
    
    
  } //End Add bins to cluster

  
  // --------------------------------------------------------------------------------
  // get center
  void SimpleVertexClusterFinder::getCenter( Cluster * cl, float & xpos, float & ypos, float & zpos){
    
    xpos=0;
    ypos=0;
    zpos=0;
    
    float totw=0;
    
    int x,y,z;
    float w;
    for(Cluster::iterator bit=cl->begin(); bit!=cl->end(); bit++) {
      
      w=bit->second;
      totw+=w;
      
      m_image.getInvRMBin( bit->first, x, y, z);
      
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
