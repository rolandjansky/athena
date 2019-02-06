/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigOnlineSpacePointTool/FastSCT_Clusterization.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TrkPrepRawData/PrepRawDataCLASS_DEF.h"

//#define CLUSTERING_DBG 

void FastSCT_Clusterization::initializeGeometry(const InDetDD::SCT_DetectorManager* manager){

  ///@todo WARNING - hack needed here for non-const method of SCT_DetectorManager
  m_man = manager;
  // Barrel geometry : get a barrel element (use layer 0, phi 1, eta 1, side 0)
  const InDetDD::SiDetectorElement* element = m_man->getDetectorElement(0, 0, 1, 1, 0);
  m_barrel_pitch  = element->phiPitch();
  m_deltaXlorentz=0.0026;
  m_corrTable[0]= 1.0;
  m_corrTable[1]=-1.0;

#ifdef CLUSTERING_DBG
std::cout << " FastSCT_Clusterization::initializeGeometry"<< std::endl;
std::cout << " barrel: pitch " << m_barrel_pitch  << " lorentz shift " << m_deltaXlorentz << std::endl;
#endif

  // find the geometry of the SCT endcap wafers:
  for (int disk=0; disk<s_ndisk; disk++)
    for (int ieta=0; ieta<s_neta; ieta++){
      // find the forward element. Use phi 0, side 0
      const InDetDD::SiDetectorElement* element = m_man->getDetectorElement(2, disk, 0, ieta, 0);
      if (0==element) continue;
      m_forward_pitch[disk][ieta]=element->phiPitch();
#ifdef CLUSTERING_DBG
      std::cout << " forward disk " << disk << " eta " << ieta
                << " pitch " << m_forward_pitch[disk][ieta]  << std::endl;
#endif
    }

}


void FastSCT_Clusterization::addCluster(){	
// make list of identifiers of all strips in the cluster:
   std::vector<Identifier> datavector;
   for (unsigned istrip=0; istrip<m_current_width; ++istrip){
      unsigned this_strip = m_strips_ascending ? m_first_strip+istrip : m_first_strip-istrip;
#ifdef CLUSTERING_DBG 
      std::cout << "adding hit on strip " << this_strip << "to list of identifiers " << std::endl;
#endif
      Identifier stripId = m_sctID->strip_id(m_element, this_strip);
      datavector.push_back(stripId);
   }

   Identifier stripID(m_sctID->strip_id(m_element, m_first_strip) );

   // position of center of strip relative to center of element.
   int strip = static_cast<int>(m_first_strip+m_last_strip-s_maxstrip);
   double clusterPos = 0.5 * static_cast<double>(strip);
   //double localPhiR = m_current_pitch*clusterPos + m_localXcorrection;

   IdentifierHash hashId = m_sctID->wafer_hash(m_element);
   double shift = m_lorentzAngleTool->getLorentzShift(hashId);
   double localPhiR = m_current_pitch*clusterPos + shift;


#ifdef CLUSTERING_DBG 
   std::cout << "Adding cluster to collection with localpos : " << localPhiR << 
     " pitch " << m_current_pitch << 
     " first strip " << m_first_strip <<
     " last_strip " << m_last_strip <<
     " max strip " << s_maxstrip << 
     " strip " << strip <<
     " clusterPos(strips) " << clusterPos<< 
     " clusterPos(mm) " << localPhiR << std::endl;
#endif
   
   Amg::Vector2D localpos(localPhiR, 0.);
   
   Amg::Vector2D pt(m_current_width, 1);
   InDet::SiWidth width(pt);
	 Amg::MatrixX* locErrMat = nullptr; //No error matrix
   
   InDet::SCT_Cluster* cluster = new InDet::SCT_Cluster(stripID, localpos, datavector, width, m_detEl,locErrMat);
   
   m_currentClusterColl->push_back(cluster);

}

void FastSCT_Clusterization::setupNewElement(const Identifier elementId, 
                                             const IdentifierHash hashId, 
                                             const unsigned int sctStrip,
                                             const InDetDD::SiDetectorElement* detElement){
  
  m_currentClusterColl = new InDet::SCT_ClusterCollection(hashId);
  m_currentClusterColl->setIdentifier(elementId);
  m_detEl=detElement;
  m_current_width=1;
  m_first_strip = sctStrip;
  m_last_strip = sctStrip;
  m_strips_ascending=true;
  m_deltaXlorentz= m_lorentzAngleTool->getLorentzShift(hashId);
  // find geometry of this element:
  if (m_sctID->is_barrel(elementId) ){
    m_current_pitch = m_barrel_pitch;
    int iside=m_sctID->side(elementId);
    m_localXcorrection = m_deltaXlorentz*m_corrTable[iside];
  }else{
    int disk_of_element = m_sctID->layer_disk(elementId);
    int eta_of_element = m_sctID->eta_module(elementId);
    m_current_pitch = m_forward_pitch[disk_of_element][eta_of_element];
    m_localXcorrection = 0;
  }

#ifdef CLUSTERING_DBG 
  std::cout << "  FastSCT_Clusterization::setupNewElement " << 
    m_sctID->print_to_string(elementId) << std::endl;
  std::cout << "  FastSCT_Clusterization::setupNewElement " << " eta_module " << " pitch " << m_current_pitch << " strip " << sctStrip << std::endl;
#endif 

}

void FastSCT_Clusterization:: addHit( const Identifier elementId, const IdentifierHash
                                      hashId, const unsigned int sctStrip,
                                      const InDetDD::SiDetectorElement* detElement) {
  if (m_firstWord) {
    m_firstWord = false;
#ifdef CLUSTERING_DBG 
    std::cout << "first word" << std::endl;
#endif

    m_element = elementId;
    setupNewElement(elementId, hashId, sctStrip, detElement);
    
#ifdef CLUSTERING_DBG 
      std::cout << "done first hit" << std::endl;
#endif
    return;
  }

  if (m_element != elementId) {
// New wafer
#ifdef CLUSTERING_DBG 
    std::cout << "adding clusters from previous wafer " << std::endl;
#endif
    // Add the last cluster from previous wafer to the collection
    addCluster();
    // Add the cluster collection to the collection list    
    m_clusterCollections.push_back(m_currentClusterColl); 
#ifdef CLUSTERING_DBG   
  std::cout << "set up new element, strip = " << sctStrip << std::endl; 
#endif 

    // set up new wafer
    m_element = elementId;
    setupNewElement(elementId, hashId, sctStrip, detElement);

  } else {
#ifdef CLUSTERING_DBG 
      std::cout << "same wafer strip = " << sctStrip << std::endl; 
#endif
// Same wafer.
// find if this strip is adjacent to last one:
    int diff = sctStrip-m_last_strip;

// check if ascending order.
    if (diff==1){
      m_strips_ascending=true;
      m_current_width++;
      m_last_strip=sctStrip;
#ifdef CLUSTERING_DBG 
      std::cout << "extending cluster to width=" << m_current_width << std::endl;
#endif
   } else if (diff==-1) {
      m_strips_ascending=false;
      m_current_width++;
      m_last_strip=sctStrip;
#ifdef CLUSTERING_DBG 
      std::cout << "extending cluster to width=" << m_current_width << std::endl;
#endif
    } else {
// new cluster:
      addCluster();
#ifdef CLUSTERING_DBG 
      std::cout << "first strip in new cluster" << m_current_width << std::endl;
#endif
      m_current_width=1;
      m_last_strip=sctStrip;
      m_first_strip=sctStrip;
    }
  }
}

void FastSCT_Clusterization::finishHits() {
  addCluster();
  // Add the cluster collection to the collection list    
  m_clusterCollections.push_back(m_currentClusterColl); 
}

std::vector<const InDet::SCT_ClusterCollection *>& FastSCT_Clusterization::getClusterCollections() {
  return m_clusterCollections;
}
