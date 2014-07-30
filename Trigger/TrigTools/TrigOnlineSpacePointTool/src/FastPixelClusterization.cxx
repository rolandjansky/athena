/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOnlineSpacePointTool/FastPixelClusterization.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"


//#define CLUSTERING_DBG 

FastPixelClusterization::FastPixelClusterization () {
  initializeHitmap();

  int ncolumn; 

  // initialization for B layer
  ncolumn = 24;
  for (int ix_chip=0; ix_chip<8; ix_chip++) {
    for (int ix_column=0; ix_column<ncolumn; ix_column++) {
      if ( ix_column == 0 || ix_column == (ncolumn-1) ) {
        int eta_index =  ix_column + ix_chip*ncolumn;
        m_set_longpixel_Blayer.insert(eta_index);
      }  
    } 
  }

  // initialization for other layers or disks
  ncolumn = 18;
  for (int ix_chip=0; ix_chip<8; ix_chip++) {
    for (int ix_column=0; ix_column<ncolumn; ix_column++) {
      if ( ix_column == 0 || ix_column == (ncolumn-1) ) {
        int eta_index =  ix_column + ix_chip*ncolumn;
        m_set_longpixel_normal.insert(eta_index);
      }  
    } 
  }

  // fill in the map for ganged pixels
  typedef std::map<unsigned int, unsigned int>::value_type valType;
  m_map_gangedPixels.insert(valType(153, 160));
  m_map_gangedPixels.insert(valType(155, 161));
  m_map_gangedPixels.insert(valType(157, 162));
  m_map_gangedPixels.insert(valType(159, 163));

  m_map_gangedPixels.insert(valType(168, 164));
  m_map_gangedPixels.insert(valType(170, 165));
  m_map_gangedPixels.insert(valType(172, 166));
  m_map_gangedPixels.insert(valType(174, 167)); 
}


void FastPixelClusterization::initializeGeometry(const InDetDD::PixelDetectorManager* manager){

  ///@todo WARNING - hack needed here for non-const method of PixelDetectorManager
  m_man = const_cast<InDetDD::PixelDetectorManager*>(manager);

  // Barrel geometry : get a B-layer barrel element (part 0 layer 0 phi 0, eta 1)
  InDetDD::SiDetectorElement* element = m_man->getDetectorElement(0, 0, 1, 1);
  m_pixelPitch = element->phiPitch();

  Identifier waferId=element->identify();
  m_nPhi_Cache[0]=m_pixelID->phi_index_max(waferId)+1;
  m_nRz_Cache[0]=m_pixelID->eta_index_max(waferId)+1;

  int iPitch = static_cast<int> (element->etaPitch() * 10.);
  double pitch = 0.1 * static_cast<double> (iPitch);
  m_length_B_normalpixel = pitch; // mm
  m_length_B_longpixel   = pitch + 0.2; // mm
  m_B_etaPitch = element->etaPitch();

#ifdef CLUSTERING_DBG   
  std::cout << " intialize geometry " << std::endl;
  std::cout << " B-layer: " << std::endl;
  std::cout << " phi pitch " <<  element->phiPitch() << " mean eta pitch " << element->etaPitch() << std::endl;
  std::cout << " normal pixels " << m_length_B_normalpixel << " long pixels " << m_length_B_longpixel << std::endl;
  std::cout << " length " << element->length()   << " width " << element->width() << std::endl;
#endif

  // Barrel geometry : get a non-Blayer barrel element (part 0 layer 1, phi 0, eta 1)

  element = m_man->getDetectorElement(0, 1, 1, 1);  
  if(element==NULL)
   {
      element = m_man->getDetectorElement(0, 2, 1, 1);
   }
  waferId=element->identify();
  m_nPhi_Cache[1]=m_pixelID->phi_index_max(waferId)+1;
  m_nRz_Cache[1]=m_pixelID->eta_index_max(waferId)+1;


  iPitch = static_cast<int> (element->etaPitch() * 10.);
  pitch = 0.1 * static_cast<double> (iPitch);
  m_length_nonB_normalpixel = pitch; // mm
  m_length_nonB_longpixel   = pitch + 0.2; // mm
  m_nonB_etaPitch = element->etaPitch();

#ifdef CLUSTERING_DBG   
  std::cout << " non B-layer: " << std::endl;
  std::cout << " phi pitch " <<  element->phiPitch() << " mean eta pitch " << element->etaPitch() << std::endl;
  std::cout << " normal pixels " << m_length_nonB_normalpixel << " long pixels " << m_length_nonB_longpixel << std::endl;
  std::cout << " length " << element->length()   << " width " << element->width() << std::endl;
#endif


#ifdef CLUSTERING_DBG   
  // find the geometry of the Pixel endcap wafers:
  // find the forward element. part 2 disk 0, phi 0 eta 0
  element = man->getDetectorElement(2, 0, 0, 0);
  std::cout << " endcap: " << std::endl;
  std::cout << " phi pitch " <<  element->phiPitch() << " mean eta pitch " << element->etaPitch() << std::endl;
  std::cout << " length " << element->length()   << " width " << element->width() << std::endl;
#endif
}



void FastPixelClusterization::addHit( const Identifier waferId, const IdentifierHash  hashElemId, const unsigned int phi_channel, const unsigned int rz_channel, const PixelRDORawData* pHit ) {

#ifdef CLUSTERING_DBG   
  std::cout << "phi_channel = " << phi_channel << "  rz_channel = " << rz_channel << std::endl; 
  if ( m_pixelID->barrel_ec(waferId)==0 &&
       m_pixelID->layer_disk(waferId)==0 && 
       rz_channel >= 192 ) 
    std::cout << " FastPixelClusterization:: Error B-layer  etaIndex = " << rz_channel << std::endl;

  if ( !( m_pixelID->barrel_ec(waferId) == 0 &&
          m_pixelID->layer_disk(waferId)==0 ) && 
          rz_channel >= 144 ) 
    std::cout << " FastPixelClusterization:: Error B-layer Other layers  etaIndex = " 
              << rz_channel << std::endl;
#endif 
  int tot=(pHit!=0)?pHit->getToT():0;
  if (m_firstWord) {
    m_firstWord = false;
    m_splittedCollection=false;
    // set the max values of row and column 
    setPixelPara(waferId);

    // Initialization for the hit
    m_element = waferId;
    m_currentClusterColl = new InDet::PixelClusterCollection(hashElemId);
    m_currentClusterColl->setIdentifier(waferId);
    m_detEl=m_man->getDetectorElement(hashElemId);

    // Add the hit
    filling_withHit(phi_channel, rz_channel,tot);
    return;
  }
 
  // Do clustering only when all the hits in one module 
  // have been collected  
  if (m_element == waferId) {
    // Add the hit

    filling_withHit(phi_channel, rz_channel,tot);


#ifdef CLUSTERING_DBG 
    std::cout << " The hit  in the same module. " << std::endl;
#endif
  
  } else { // new wafer
   
#ifdef CLUSTERING_DBG
    std::cout << " The hit is in a different module. " <<std::endl;
#endif 

    // Do pixel clustering for previous wafer
    clustering();

    // Add the cluster collection to the collection list   
    if(!m_splittedCollection)
      {
	m_clusterCollections.push_back(m_currentClusterColl); 
      }

    // Initialization for the new wafer 

    setPixelPara(waferId);

    m_element = waferId;

    bool found=false;
    for(std::vector<InDet::PixelClusterCollection*>::iterator collIt=m_clusterCollections.begin();
	collIt!=m_clusterCollections.end();++collIt)
      {
	if(hashElemId==(*collIt)->identifyHash())
	  {
	    found=true;
	    m_splittedCollection=true;
	    m_currentClusterColl=(*collIt);
	    break;
	  }
      }
    if(!found)
      {
	m_currentClusterColl = new InDet::PixelClusterCollection(hashElemId);
	m_currentClusterColl->setIdentifier(waferId);
	m_splittedCollection=false;
      }

    m_detEl=m_man->getDetectorElement(hashElemId);

    // Add the hit

    filling_withHit(phi_channel, rz_channel, tot);

  }
}

void FastPixelClusterization::finishHits() {

    clustering();

    // Add the cluster collection to the collection list   
    
    if(!m_splittedCollection)
      {
	m_clusterCollections.push_back(m_currentClusterColl); 
      }
}

void FastPixelClusterization::initializeHitmap() {
  for (int i=0; i < m_hitmapSize; m_hitmap[i++] = 0);
}

void FastPixelClusterization::initializeClusterPara() {
  
  m_sum_rz = 0;
  m_sum_phi =0;
  m_phi_min = 1000;
  m_phi_max = 0; 
  m_rz_min = 1000;
  m_rz_max = 0;
  m_entries = 0;  
}

void FastPixelClusterization::setPixelPara(const Identifier& waferId) {

  /*
  m_nPhi = m_pixelID->phi_index_max(waferId)+1;
  m_nRz = m_pixelID->eta_index_max(waferId)+1;
  printf("m_nPhi=%d m_nRz=%d\n",m_nPhi,m_nRz);
  */

  if((m_length_nonB_longpixel!= m_length_B_longpixel) && 
     m_pixelID->barrel_ec(waferId)==0 &&
     m_pixelID->layer_disk(waferId)==0)
    {
      m_nPhi = m_nPhi_Cache[0];
      m_nRz = m_nRz_Cache[0];
    } 
  else
    {
      m_nPhi = m_nPhi_Cache[1];
      m_nRz = m_nRz_Cache[1];
    }

  //  printf("m_nPhi=%d m_nRz=%d\n",m_nPhi,m_nRz);

  m_waferHalfWidth = 0.5 * m_pixelPitch * static_cast<double> (m_nPhi); 

  if ( m_pixelID->barrel_ec(waferId)==0) m_waferHalfWidth-=0.0363;

  if ( (m_length_nonB_longpixel!= m_length_B_longpixel) && 
       m_pixelID->barrel_ec(waferId)==0 &&
       m_pixelID->layer_disk(waferId)==0 ) 
    { 
      m_isBlayer = true ;
      m_length_longpixel = m_length_B_longpixel; // mm
      m_length_normalpixel = m_length_B_normalpixel;
      m_waferHalfLength =  0.5 * m_B_etaPitch * static_cast<double> (m_nRz);
      if (m_nRz == 192) m_waferHalfWidth +=0.0063;
      /*
#ifdef CLUSTERING_DBG 
      std::cout << "FastPixelClusterization::setPixelPara:" << m_pixelID->print_to_string(waferId) << std::endl;
      std::cout << " B layer " << " normal " <<  m_length_normalpixel << " long " << m_length_longpixel << std::endl;
      std::cout << " B-layer " << " half_width " <<  m_waferHalfWidth << " half length " << m_waferHalfLength<<std::endl;
      std::cout << " B-layer " << " nPhi " <<  m_nPhi << " nRz " << m_nRz << std::endl;
#endif
      */
    } 
  else 
    {
      m_isBlayer = false ;
      m_length_longpixel = m_length_nonB_longpixel; // mm
      m_length_normalpixel = m_length_nonB_normalpixel;
      m_waferHalfLength =  0.5 * m_nonB_etaPitch * static_cast<double> (m_nRz);
      /*
#ifdef CLUSTERING_DBG 
      if (m_pixelID->barrel_ec(waferId)==1) 
	{
	  std::cout << "FastPixelClusterization::setPixelPara:" << m_pixelID->print_to_string(waferId) << std::endl;
	  std::cout << " endcap " << " normal " <<  m_length_normalpixel << " long " << m_length_longpixel << std::endl;
	  std::cout << " endcap " << " half_width " <<  m_waferHalfWidth << " half length "<<m_waferHalfLength<<std::endl;
	  std::cout << " endcap " << " nPhi " <<  m_nPhi << " nRz " << m_nRz << std::endl;
	} 
      else 
	{
	  std::cout << "FastPixelClusterization::setPixelPara:" << m_pixelID->print_to_string(waferId) << std::endl;
	  std::cout << " non-Blayer " << " normal " <<  m_length_normalpixel << " long "<<m_length_longpixel<< std::endl;
	  std::cout << " non-Blayer " << " half_width " <<  m_waferHalfWidth << " half length " << m_waferHalfLength << std::endl;
	  std::cout << " non-Blayer " << " nPhi " <<  m_nPhi << " nRz " << m_nRz << std::endl;
	}
#endif
      */
    }
}

void FastPixelClusterization::clustering() {

std::list<LocalCoordinate>::iterator iter;

#ifdef CLUSTERING_DBG 
  std::cout << "Number of hits in a module:  " << m_pixelHitList.size() << std::endl;
  for (iter=m_pixelHitList.begin(); iter != m_pixelHitList.end(); iter++) {
    std::cout << "phi =  " << (*iter).phi() << " rz = " << (*iter).rz() << std::endl;

  }
#endif 
  
  for(iter=m_pixelHitList.begin();iter!=m_pixelHitList.end();iter++) 
    {
      // Check if the starting hit has been already collected to a cluster
      if(!hit_found((*iter).phi(),(*iter).rz())) 
	{     
	  continue;
	}
      // Initialize cluster parameters 
      initializeClusterPara();
      // Search for other hits of the cluster 
      findCluster((*iter).phi(), (*iter).rz());    
    }
  // Clear the pixel hit list
  m_pixelHitList.clear();
}

void FastPixelClusterization::findCluster(unsigned int phi, unsigned int rz) {

  // Create a vector of Identifiers. Ownership will be given to the PixelCluster
  m_pixelList = new std::vector<Identifier>;

  // Add the hit  
  collectHit(phi, rz);

  // Loop over possible neighbour cells     

  // cell (phi-1, rz)	   
  if(int(phi)-1>=0)
    {
      if ( hit_found(phi-1, rz) ) 
	{
	  collectHit(phi-1, rz);
	  // Collect hits of the cluster in the direction of -phi 
	  if (int(phi)-2 >= 0 )
	    formCluster3(phi-2, rz);
	}
    }
  if(int(phi)-1>=0 )
    {
      if ( hit_found(phi-1, rz+1) ) 
	{
	  collectHit(phi-1, rz+1);
	  // Collect hits of the cluster in the direction of Z 
	  formCluster2(phi-1, rz+2);
	  // Collect hits of the cluster in the direction of -phi
	  if ( int(phi)-2 >= 0 )
	    formCluster3(phi-2, rz+1);
	}
    }
  if(hit_found(phi, rz+1)) 
    {    
      collectHit(phi, rz+1);
      // Collect hits of the cluster in the direction of Z 
      formCluster2(phi, rz+2); 
    }
  if(hit_found(phi+1, rz+1)) 
    {
      collectHit(phi+1, rz+1);
      // Collect hits of the cluster in the direction of phi 
      formCluster1(phi+2, rz+1);
      // Collect hits of the cluster in the direction of Z  
      formCluster2(phi+1, rz+2);
    }
  if(hit_found(phi+1,rz)) 
    {
      collectHit(phi+1, rz);
      // Collect hits of the cluster in the direction of phi 
      formCluster1(phi+2, rz); 
    }

  // Calculate cluster widths 
  double widthRz = get_widthRz(m_rz_min, m_rz_max);       
  double widthPhi = get_widthPhi(m_phi_min, m_phi_max);

  // Calculate the position of cluster center   
  double centrePhi = double(m_sum_phi)/double(m_entries);    
  double centreRz = double(m_sum_rz)/double(m_entries);

  // Create the Pixel Cluster  
  Identifier pixId = m_pixelID->pixel_id(m_element, phi,rz);

  addCluster(pixId, centrePhi, centreRz, widthPhi, widthRz, *m_pixelList,
	     m_phi_max-m_phi_min+1, m_rz_max-m_rz_min+1);

  
  delete  m_pixelList;

#ifdef CLUSTERING_DBG    
    std::cout << "findCluster formed cluster:" 
      // << " id=" <<    waferId
	      << " phi_channel=" << phi
	      << " phi pitch=" << m_pixelPitch
	      << " centrePhi=" << centrePhi
	      << " widthPhi=" << widthPhi   <<std::endl;
    std::cout   << " rz_channel=" << rz
		<< " length_normalpixel " << m_length_normalpixel
		<< " length_longpixel " << m_length_longpixel
		<< " centreRz=" << centreRz
		<< " widthRz=" << widthRz                                 
		<<std::endl;

    const Amg::Vector2D pos =  m_currentCluster->localPosition();
    std::cout << " locT " << pos[0] << " locL " << pos[1] << std::endl;
	
#endif 
  
  // Increase the cluster id
    
  return;
}

void FastPixelClusterization::addCluster(const Identifier& pixId, 
				     double centrePhi, double centreRz, 
				     double widthPhi, double widthRz, 
				     std::vector<Identifier>& rdoList, 
				     unsigned rdoWidthPhi, unsigned rdoWidthRz){

	Amg::Vector2D w1(widthPhi, widthRz);
	Amg::Vector2D dummy(rdoWidthPhi, rdoWidthRz);
	double shift = m_detEl->getLorentzCorrection();
	Amg::Vector2D localpos(centrePhi+shift, centreRz);
	InDet::SiWidth siwidth(dummy, w1);
	Amg::MatrixX* locErrMat = nullptr; //No error matrix
	
	InDet::PixelCluster* cluster = new InDet::PixelCluster(pixId, localpos, rdoList, 1, m_totList,
							       siwidth, m_detEl,locErrMat);
       	m_currentCluster = cluster;
       	m_currentClusterColl->push_back(cluster);
}

void FastPixelClusterization::formCluster1(unsigned int phi, unsigned int rz) {
 
  if (int(rz)-1 >= 0) {
    if ( !hit_found(phi, rz-1) && !hit_found(phi, rz) &&
         !hit_found(phi, rz+1) ) return;
  } else {
    if ( !hit_found(phi, rz) && !hit_found(phi, rz+1) ) return;
  }

  for (int ix=-1; ix<2; ++ix ) {
    if (int(rz)+ix >= 0) {
      if (hit_found(phi, rz+ix)) { 
        collectHit(phi, rz+ix);
        // recursive formation 
        formCluster1(phi+1, rz+ix);
      }
    }
  }  
  return;
}

void FastPixelClusterization::formCluster2(unsigned int phi, unsigned int rz) {

  if ( int(phi) - 1 >= 0) {
    if ( !hit_found(phi-1, rz) && !hit_found(phi, rz) &&
         !hit_found(phi+1, rz) ) return;
  }
  else {
    if ( !hit_found(phi, rz) && !hit_found(phi+1, rz) ) return;
  }

  for (int ix=-1; ix<2; ++ix ) {
    if (int(phi)+ix >= 0) {
      if (hit_found(phi+ix, rz)) {
        collectHit(phi+ix, rz);
        // recursive formation 
        formCluster2(phi+ix, rz+1);
      }
    }
  }
  return;
}

void FastPixelClusterization::formCluster3(unsigned int phi, unsigned int rz) {
  
  if (int(rz)-1 >= 0) {
    if ( !hit_found(phi, rz-1) && !hit_found(phi, rz) &&
         !hit_found(phi, rz+1) ) return;
  }
  else {
    if ( !hit_found(phi, rz) && !hit_found(phi, rz+1) ) return;
  }

  for (int ix=-1; ix < 2; ++ix ) {
    if (int(rz)+ix >= 0) {
      if (hit_found(phi, rz+ix)) { 
        collectHit(phi, rz+ix);
        // recursive formation 
        if ( int(phi)-1 < 0 ) continue;  
        formCluster3(phi-1, rz+ix);
      }
    }
  }
  return;
}

void FastPixelClusterization::collectHit(unsigned int phi, unsigned int rz) {
 
  // Update cluster parameters
  m_sum_rz = m_sum_rz +  get_coordRz(rz);
  m_sum_phi =  m_sum_phi + get_coordPhi(phi);

  if (m_phi_min > phi) m_phi_min = phi;
  if (m_phi_max < phi) m_phi_max = phi;
  
  if (m_rz_min > rz) m_rz_min = rz;
  if (m_rz_max < rz) m_rz_max = rz;

  Identifier pixId = m_pixelID->pixel_id(m_element, phi,rz);
  m_pixelList->push_back(pixId);
  int tot = get_tot(phi, rz);
  m_totList.push_back(tot);
  m_entries++;

#ifdef CLUSTERING_DBG
  std::cout << " phi = " << phi << " rz = " << rz << " tot = "<<tot<<" was collected." << std::endl; 
#endif 

  // Remove hit from hitmap
  if (!hit_found(phi, rz)) {
    std::cout << "FastPixelClusterization: internal inconsistency: reset a pixel without a hit \n";
  } else { 
    clear_hit(phi, rz);
  }
}


std::vector<InDet::PixelClusterCollection *>& FastPixelClusterization::getClusterCollections() {
  return m_clusterCollections;
}
