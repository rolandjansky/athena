/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
// ================================================
// InDetAlignFillSiCluster
// ================================================
//
// InDetAlignFillSiCluster.cxx
// Source file for InDetAlignFillSiCluster
//
// Carlos Escobar, started 08/03/2008
//
// AthAlgTool to fill silicon cluster information in a ntuple

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "TrkEventPrimitives/ParamDefs.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include "InDetAlignGenTools/InDetAlignFillSiCluster.h"

#include <string>

static const int maxclusters = 24000;
static const int maxclsize = 10;

//=====================================================================
// InDetAlignFillSiCluster()
//=====================================================================
InDetAlignFillSiCluster::InDetAlignFillSiCluster(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_ntupleSvc(nullptr),
    m_pixelid(nullptr),
    m_sctID(nullptr),
    m_Pixel_clcontainer(nullptr),
    m_Sct_clcontainer(nullptr)
{
  declareInterface<IInDetAlignFillSiCluster>(this);
  declareProperty("Pixel_SiClusterContainerName", m_Pixel_SiClustersName="PixelClusters");
  declareProperty("SCT_SiClusterContainerName",   m_Sct_SiClustersName="SCT_Clusters");

  // Ntuple
  declareProperty("NtupleName", m_ntupleName="/NTUPLES/GLOBFILE");
}


//=====================================================================
// ~FillSiCluster()
//=====================================================================
InDetAlignFillSiCluster::~InDetAlignFillSiCluster() {}


//=====================================================================
// initialize()
//===================================================================== 
StatusCode InDetAlignFillSiCluster::initialize() {

  ATH_MSG_DEBUG("In Initialize() of FillSiCluster()");
  
  // ID Helper
  
  if (detStore()->retrieve(m_sctID, "SCT_ID").isFailure()){
    msg(MSG::FATAL) << "Could not get SCT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SCT ID is : "<< m_sctID <<endmsg ;
  
  if (detStore()->retrieve(m_pixelid, "PixelID").isFailure()){
    msg(MSG::FATAL) << "Could not get PIXEL ID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pixel ID is : " << m_pixelid << endmsg;
  
  // retrieve the NTuple Service
  if (StatusCode::SUCCESS != service("NTupleSvc", m_ntupleSvc)) {
    ATH_MSG_FATAL ("NTupleSvc service not found!");
    return StatusCode::FAILURE;
  }
  
  bookNtuple();

  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialize() of FillSiCluster successful" << endmsg;
  return StatusCode::SUCCESS;
}


//=====================================================================
// finalize()
//===================================================================== 
StatusCode InDetAlignFillSiCluster::finalize() {
  
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "Finalize() of FillSiCluster" << endmsg;
    msg(MSG::DEBUG) << "________________________________________________________" << endmsg;
    msg(MSG::DEBUG) << endmsg;
    msg(MSG::DEBUG) << " InDetAlignFillSiCluster Summary: " << endmsg;
    msg(MSG::DEBUG) << "________________________________________________________" << endmsg;
    msg(MSG::DEBUG) << endmsg;   
  }

  return StatusCode::SUCCESS;
}


//=====================================================================
// FillSiCluster()
//===================================================================== 
StatusCode InDetAlignFillSiCluster::FillSiCluster() {

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In FillSiCluster()" << endmsg;

  StatusCode sc;

  if(RetrieveSCTSiClusters() != StatusCode::FAILURE)
    FillSCTSiNtuple();
  
  if(RetrievePixelSiClusters() != StatusCode::FAILURE)
    FillPixelSiNtuple();
    
  std::string nt0id = m_ntupleName + "/SiCluster";
  sc = m_ntupleSvc->writeRecord(nt0id);
  if (sc.isFailure()) if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Could not write " << nt0id << "!" << endmsg;

  return StatusCode::SUCCESS;
}


//=====================================================================
// bookNtuple()
//===================================================================== 
void InDetAlignFillSiCluster::bookNtuple() {

  ATH_MSG_DEBUG ("Booking Trk::Track Info...");
    
  NTupleFilePtr file1(m_ntupleSvc, m_ntupleName);
  std::string nt0id    = m_ntupleName + "/SiCluster";
  std::string comments = "Silicon Cluster Information";

  StatusCode sc;

  NTuplePtr nt0(m_ntupleSvc, nt0id);
  if (nt0) ATH_MSG_DEBUG ("Ntuple is already booked");
  else {
    ATH_MSG_DEBUG ("Attempting to book general ntuple");
    nt0 = m_ntupleSvc->book(nt0id,CLID_ColumnWiseTuple,comments);
      
    if (nt0) {

      // ---------------------------------------------------------------------
      // Pixel Clusters
      sc = nt0->addItem("pixel_clus_nclusters", m_pixel_nclusters, 0, maxclusters);
      sc = nt0->addItem("pixel_clus_clx",       m_pixel_nclusters, m_pixel_clx);
      sc = nt0->addItem("pixel_clus_cly",       m_pixel_nclusters, m_pixel_cly);
      sc = nt0->addItem("pixel_clus_clz",       m_pixel_nclusters, m_pixel_clz);
      sc = nt0->addItem("pixel_clus_LocX",      m_pixel_nclusters, m_pixel_LocX);
      sc = nt0->addItem("pixel_clus_LocY",      m_pixel_nclusters, m_pixel_LocY);
      sc = nt0->addItem("pixel_clus_groupsize", m_pixel_nclusters, m_pixel_groupsize);
      sc = nt0->addItem("pixel_clus_layer",     m_pixel_nclusters, m_pixel_layer);
      sc = nt0->addItem("pixel_clus_phi",       m_pixel_nclusters, m_pixel_phi);
      sc = nt0->addItem("pixel/pixel_clus_row",m_pixel_nclusters, m_pixel_clrow, maxclsize);
      sc = nt0->addItem("pixel/pixel_clus_col",m_pixel_nclusters, m_pixel_clcol, maxclsize);
      m_pixel_nclusters = 0;
      // ---------------------------------------------------------------------

      // ----------------------------------------------------------------------
      // SCT Clusters
      sc = nt0->addItem("sct_clus_nclusters", m_sct_nclusters, 0, maxclusters);
      sc = nt0->addItem("sct_clus_x",         m_sct_nclusters, m_sct_clx);
      sc = nt0->addItem("sct_clus_y",         m_sct_nclusters, m_sct_cly);
      sc = nt0->addItem("sct_clus_z",         m_sct_nclusters, m_sct_clz);
      sc = nt0->addItem("sct_clus_groupsize", m_sct_nclusters, m_sct_groupsize);
      sc = nt0->addItem("sct_clus_layer",     m_sct_nclusters, m_sct_layer);
      sc = nt0->addItem("sct_clus_eta",       m_sct_nclusters, m_sct_eta);
      sc = nt0->addItem("sct_clus_phi",       m_sct_nclusters, m_sct_phi);
      sc = nt0->addItem("sct_clus_side",      m_sct_nclusters, m_sct_side);
      m_sct_nclusters = 0;
      // ----------------------------------------------------------------------
    
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ntuple " << nt0id << " has been booked successfully! " << endmsg;
    }
    else
      ATH_MSG_ERROR ("Error booking ntuple");
  }

}


//=====================================================================
// RetrieveSCTSiClusters()
//=====================================================================
StatusCode InDetAlignFillSiCluster::RetrieveSCTSiClusters() {

  ATH_MSG_DEBUG ("In RetrieveSCTSiClusters()");
  StatusCode sc;
  
  // get clusters from TDS
  sc = evtStore()->retrieve(m_Sct_clcontainer, m_Sct_SiClustersName);
  if (sc.isFailure()  || !m_Sct_clcontainer){
    ATH_MSG_DEBUG ("SCT Cluster container for SCT not found");
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG ("SCT Cluster container for SCT found");
  
  return sc;
}


//=====================================================================
// RetrievePixelSiClusters()
//=====================================================================
StatusCode InDetAlignFillSiCluster::RetrievePixelSiClusters() {

  ATH_MSG_DEBUG ("In RetrievePixelSiClusters()");
  StatusCode sc;

  // get pixel clusters from TDS
  sc = evtStore()->retrieve(m_Pixel_clcontainer, m_Pixel_SiClustersName);
  if (sc.isFailure()  || !m_Pixel_clcontainer){
    ATH_MSG_DEBUG ("Pixel Cluster container for Pixels not found");
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG ("Pixel Cluster container for Pixels found");
    
  return sc;
}

//=====================================================================
// FillSCTSiNtuple()
//=====================================================================
void InDetAlignFillSiCluster::FillSCTSiNtuple() {

  ATH_MSG_DEBUG ("In FillSCTSiNtuple()");

  m_sct_nclusters = 0;

  // loop over SCT clusters collections
  for(SCT_ClusterContainer::const_iterator it=m_Sct_clcontainer->begin();
      it!=m_Sct_clcontainer->end(); ++it) {

    const InDet::SCT_ClusterCollection *colNext=&(**it);
    if (!colNext) continue;
    
    // loop over Clusters
    DataVector<InDet::SCT_Cluster>::const_iterator p_clus;
    for(p_clus=colNext->begin(); p_clus!=colNext->end(); ++p_clus) {

      Identifier clId = (*p_clus)->identify();
      const InDet::SCT_Cluster& cluster = **p_clus;
      int GroupSize = cluster.rdoList().size();
      
      if ( m_sct_nclusters < maxclusters ) {
	m_sct_clx[m_sct_nclusters] = cluster.globalPosition().x();
	m_sct_cly[m_sct_nclusters] = cluster.globalPosition().y();
	m_sct_clz[m_sct_nclusters] = cluster.globalPosition().z();
	m_sct_layer[m_sct_nclusters] = m_sctID->layer_disk(clId);
	m_sct_eta[m_sct_nclusters] = m_sctID->eta_module(clId);
	m_sct_phi[m_sct_nclusters] = m_sctID->phi_module(clId);
	m_sct_side[m_sct_nclusters] = m_sctID->side(clId);
	m_sct_groupsize[m_sct_nclusters] = GroupSize;

	// Cluster Information
	if (msgLvl(MSG::DEBUG)) {
	  msg(MSG::DEBUG) << "SCT Cluster: ["
		<< m_sct_layer[m_sct_nclusters] << "/"
		<< m_sct_eta[m_sct_nclusters] << "/"
		<< m_sct_phi[m_sct_nclusters] << "/"
		<< m_sct_side[m_sct_nclusters] << "] - ("
		<< m_sct_clx[m_sct_nclusters] << ","
		<< m_sct_cly[m_sct_nclusters] << ","
		<< m_sct_clz[m_sct_nclusters] << ")";
	}

	if (GroupSize>1) {
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " strips: (" << m_sctID->strip(clId) - GroupSize
		<< "," << m_sctID->strip(clId)
		<< ") - GroupSize: " << GroupSize
		<< endmsg;
	}
	else {
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " strip: (" << m_sctID->strip(clId)
		<< ") - GroupSize: " << GroupSize
		<< endmsg;
	}

      }
      m_sct_nclusters++;
    }
    // if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "number of clusters: " << m_sct_nclusters << endmsg;
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SCT Clusters: " << m_sct_nclusters << endmsg;
  return;
}

//=====================================================================
// FillPixelSiNtuple()
//=====================================================================
void InDetAlignFillSiCluster::FillPixelSiNtuple() {

  ATH_MSG_DEBUG ("In FillPixelSiNtuple()");

  m_pixel_nclusters = 0;

  // loop over Pixel clusters collections
  for(PixelClusterContainer::const_iterator it=m_Pixel_clcontainer->begin(); 
      it!=m_Pixel_clcontainer->end(); ++it) {

    const InDet::PixelClusterCollection *colNext=&(**it);
    if (!colNext) continue;

    // loop over Clusters
    DataVector<InDet::PixelCluster>::const_iterator p_clus;
    for(p_clus=colNext->begin(); p_clus!=colNext->end(); ++p_clus) {
      Identifier clId = (*p_clus)->identify();

      const InDet::PixelCluster& cluster = **p_clus;

      int GroupSize = cluster.rdoList().size();

      if (m_pixel_nclusters < maxclusters ){
        m_pixel_LocX[m_pixel_nclusters] = cluster.localPosition().x();
        m_pixel_LocY[m_pixel_nclusters] = cluster.localPosition().y();
        m_pixel_layer[m_pixel_nclusters] = m_pixelid->layer_disk(clId);
        m_pixel_phi[m_pixel_nclusters] = m_pixelid->phi_module(clId);
        m_pixel_groupsize[m_pixel_nclusters] = GroupSize;
        m_pixel_clx[m_pixel_nclusters] = cluster.globalPosition().x();
        m_pixel_cly[m_pixel_nclusters] = cluster.globalPosition().y();
        m_pixel_clz[m_pixel_nclusters] = cluster.globalPosition().z();
	const std::vector<Identifier>& rdoList =cluster.rdoList(); 
	std::vector<Identifier>::const_iterator nextRDO; 
	int i(0); 
	for(nextRDO=rdoList.begin(); nextRDO !=rdoList.end(); ++nextRDO){ 
	  Identifier rdoId = (*nextRDO); 
	  if(i<maxclsize){
	    m_pixel_clrow[m_pixel_nclusters][i] = m_pixelid->phi_index(rdoId); 
	    m_pixel_clcol[m_pixel_nclusters][i] = m_pixelid->eta_index(rdoId);
	  }
	  ++i; 
	}

	// Cluster Information
	if (msgLvl(MSG::DEBUG)) {
	  msg(MSG::DEBUG) << "Pixel Cluster (Local Pos): ["
		<< m_pixel_layer[m_pixel_nclusters] << "/"
		<< m_pixel_phi[m_pixel_nclusters] << "] - ("
		<< m_pixel_LocX[m_pixel_nclusters] << ","
		<< m_pixel_LocY[m_pixel_nclusters] << ")"
		<< " Groupsize: " << GroupSize << endmsg;
	}

      }
      m_pixel_nclusters++;
    }
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pixel Clusters: " << m_pixel_nclusters << endmsg;
  
  return;
}
