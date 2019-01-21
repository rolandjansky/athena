/*
  Copyright (C) 2018, 2019 CERN for the benefit of the ATLAS collaboration
*/
#include <vector>
#include <string>
#include <map>
#include "FTK_RecTools/FTK_HashIDTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrigFTKSim/ftk_dcap.h"

#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_ForwardModuleSideDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetPrepRawData/PixelCluster.h"

#include "TMath.h"

using namespace std;

using Amg::Vector3D;

namespace {
  const double ONE_TWELFTH = 1./12.;
}

/////////////////////////////////////////////////////////////////////////////
FTK_HashIDTool::FTK_HashIDTool(const std::string& t, 
			       const std::string& n,
			       const IInterface*  p ): 
  AthAlgTool(t,n,p),
  m_max_tower(64),
  m_pixelId(0),
  m_id_helper(0),
  m_pixelManager(0),
  m_maxD0(0.)
{

  declareProperty("ConstantsDir",m_ConstantsDir="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FTK","directory for constants");
  declareProperty("PatternsVersion",m_PatternsVersion="DataAlignment2017_xm05_ym09_Reb64_v9","Patterns Version");
  declareProperty("Ntowers",m_max_tower,"No. FTK Towers");
  declareProperty("MaxD0",m_maxD0,"Truncate d0 at this maximum value");
}

FTK_HashIDTool::~FTK_HashIDTool()
{ }


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode FTK_HashIDTool::initialize(){


  MsgStream athlog(msgSvc(), name());
  ATH_MSG_INFO("FTK_HashIDTool::initialize()" );

  ATH_MSG_INFO("ConstantsDir: "<<m_ConstantsDir);
  ATH_MSG_INFO("PatternsVersion: "<<m_PatternsVersion);
  ATH_MSG_INFO("Ntowers= " << m_max_tower);

  StatusCode ret=StatusCode::SUCCESS;
  m_moduleFromSector.resize(m_max_tower);
  m_sector8toSector12.resize(m_max_tower);
  for (unsigned int itower=0; itower<m_max_tower; itower++) {
    
    m_moduleFromSector[itower] = new ftk_sectormap();
    unsigned int returnCode = (this)->readModuleIds(itower, *(m_moduleFromSector[itower]));
  if (returnCode) {
      ATH_MSG_WARNING("Error " << returnCode << " loading constants for tower " << itower);
      ret=StatusCode::FAILURE;
      break;
    } 
    
  }  
  if (ret.isSuccess()) {
    StoreGateSvc* detStore;
    ATH_CHECK(service("DetectorStore", detStore));
    
    ATH_CHECK(detStore->retrieve(m_pixelId, "PixelID"));
    ATH_CHECK(detStore->retrieve(m_id_helper, "AtlasID"));
    ATH_CHECK(detStore->retrieve(m_pixelManager));
  }
  return ret; 
}



bool FTK_HashIDTool::findHash(unsigned int hash, bool isSCT, unsigned int& tower, unsigned int& sector, unsigned int& plane) {
  int i = (int) hash;
  sector=tower=plane=0xffffffff;
  bool found=false;

  //  for (unsigned int itower = 0; itower < m_moduleFromSector.size(); itower++) {
  ftk_sectormap* map = m_moduleFromSector[tower];
  if (map) {   
    //      for ( unsigned int isector=0; isector < map->size();isector++) {
    if ((*map)[sector].size() > 0) {
      auto first= (*map)[sector].begin();
      if (isSCT) first+=4;
      auto last = (*map)[sector].end();
      if (!isSCT) last -=8;

      auto p = std::find(first, last, i);
      if ( p!= last ) {
	plane = std::distance(first, p);

      }
    }
  }

  return found;
}


bool haveAuxTracks(const FTK_RawTrackContainer& inputTracks) {
  bool haveAux=false;
  for (auto track : inputTracks) {
    if (track->getIsAuxFormat()) {
      haveAux=true;
      break;
    }
  }
  return haveAux;
}


FTK_RawTrackContainer* FTK_HashIDTool::processTracks(const FTK_RawTrackContainer& inputTracks, const bool reverseIBL) {

  FTK_RawTrackContainer* outputTracks = new FTK_RawTrackContainer();
  ATH_MSG_DEBUG("FTK_HashIDTool::processTracks Created new FTK_RawTrackContainer at 0x" << std::hex<<outputTracks << std::dec);
  
  unsigned int nAuxTracks=0;
  unsigned int nGoodAuxTracks=0;
  unsigned int itrack=0;
  for (auto track : inputTracks) {
    std::vector<Amg::Vector3D> pointsOnTrack;
    pointsOnTrack.reserve(3);
    if (!track->getIsAuxFormat()) {
      ATH_MSG_DEBUG(" Track " << itrack << " is in FLIC format");
      outputTracks->push_back(new FTK_RawTrack(*track));
    } else {
      ATH_MSG_DEBUG(" Track " << itrack << " is in AUX format - converting");
      nAuxTracks++;
      uint32_t layermap = track->getLayerMap();
      uint32_t idtower = track->getTower();
      uint32_t idsector = track->getSectorID();
      uint32_t layer=0;
      FTK_RawTrack* newTrack = new FTK_RawTrack();
      for( unsigned int ipix = 0; ipix < track->getPixelClusters().size(); ++ipix,++layer){
	if (!track->isMissingPixelLayer(ipix)){
	  FTK_RawPixelCluster clus(track->getPixelWordA(ipix),track->getPixelWordB(ipix),track->getPixelBarcode(ipix));
	  uint32_t id = this->getHashFromAuxSector(idtower, idsector, layer);
	  if (id == 0xffffffff) {
	    ATH_MSG_WARNING(" Failed to get ID for pixel module with tower " << idtower << " sector " << idsector << "  layer " << layer);
	    layermap &= ~(0x1 << layer); // clear bit for layer 
	  } else {
	    ATH_MSG_DEBUG(" pix " << ipix << " Setting module ID 0x" <<std::hex << id << std::dec);
	    //ATH_MSG_DEBUG(" pix " << ipix << " Original   ID 0x" <<std::hex << track->getPixelClusters()[ipix].getModuleID() << std::dec);
	  }
	  clus.setModuleID(id);
	  ATH_MSG_DEBUG(" clus.getModuleID(id) 0x" <<std::hex << clus.getModuleID() << std::dec);
	  newTrack->setPixelCluster(ipix,clus);
	  if (id !=0xffffffff) {
	    ATH_MSG_DEBUG("newTrack->getPixelCluster(ipix).getModuleID() 0x"<<std::hex <<   newTrack->getPixelCluster(ipix).getModuleID() << std::dec);
	    const Amg::Vector3D point = this->getPixelClusterPosition(clus,reverseIBL);
	    pointsOnTrack.push_back(point);
	  }
	}
      }
      for( unsigned int isct = 0; isct < track->getSCTClusters().size(); ++isct,++layer){
	if (!track->isMissingSCTLayer(isct)){
	  uint32_t id = this->getHashFromAuxSector(idtower, idsector, layer);
	  if (id == 0xffffffff) {
	    ATH_MSG_WARNING(" Failed to get ID for SCT module with tower " << idtower << " sector " << idsector << "  layer " << layer);
	    layermap &= ~(0x1 << layer); // clear bit for layer 
	  } else {
	    ATH_MSG_DEBUG(" sct " << isct << " Setting module ID 0x" <<std::hex << id << std::dec << " coord " << track->getSCTClusters()[isct].getHitCoord() );
	    //ATH_MSG_DEBUG(" sct " << isct << " Original   ID 0x" <<std::hex << track->getSCTClusters()[isct].getModuleID() << std::dec);

	  }
	  
	  FTK_RawSCT_Cluster clus(track->getSCTWord(isct),track->getSCTBarcode(isct));
	  clus.setModuleID(id);
	  newTrack->setSCTCluster(layer,clus);
	  ATH_MSG_DEBUG(" NewTrack SCT cluster " << isct << " layer " << layer << " ModuleID 0x" << std::hex << newTrack->getSCTClusters()[isct].getModuleID() << std::dec << " coordinate " << newTrack->getSCTClusters()[isct].getHitCoord());
	}  
      }
      float invpT=0.;
      float d0=0.;
      float z0=0.;
      float phi=0.;
      float cotTheta =0.;

      if (pointsOnTrack.size() < 2) {
	ATH_MSG_DEBUG(" Only " << pointsOnTrack.size() << " valid pixel clusters on track - can't calculate parameters, skipping track");
	delete(newTrack);
	continue;
      } else if ( pointsOnTrack.size() ==2) {
	ATH_MSG_DEBUG(" Only " << pointsOnTrack.size() << " valid pixel clusters on track - using origin as third point for pT estimation");
	Amg::Vector3D origin(0.,0.,0.);
	phi = this->get_phi(origin,pointsOnTrack[0],pointsOnTrack[1]);
	invpT = this->get_invpT(origin,pointsOnTrack[0],pointsOnTrack[1]);
	d0 = 0.;
	z0 = this->get_z0(pointsOnTrack[0],pointsOnTrack[1]);
	cotTheta= this->get_cotTheta(pointsOnTrack[0],pointsOnTrack[1]);
      } else {
	ATH_MSG_DEBUG(" Using 3 pixel points to get track parameters");
	phi = this->get_phi(pointsOnTrack[0],pointsOnTrack[1],pointsOnTrack[2]);
	invpT = this->get_invpT(pointsOnTrack[0],pointsOnTrack[1],pointsOnTrack[2]);
	if (m_maxD0==0.) {
	  d0=0.;
	} else {
	  d0 = this->get_d0(pointsOnTrack[0],pointsOnTrack[1],pointsOnTrack[2]);
	  if (std::fabs(d0)>m_maxD0) d0=(d0>0?m_maxD0:-m_maxD0);
	}
	z0 = this->get_z0(pointsOnTrack[0],pointsOnTrack[2]);
	cotTheta = this->get_cotTheta(pointsOnTrack[0],pointsOnTrack[2]);
      }
      newTrack->setChi2(1.);
      newTrack->setInvPt(invpT);
      newTrack->setPhi(phi);
      newTrack->setD0(d0);
      newTrack->setZ0(z0);
      newTrack->setCotTh(cotTheta);
      newTrack->setLayerMap(layermap);
      newTrack->setSectorID(idsector);
      newTrack->setSectorID(idtower);
      newTrack->setRoadID(track->getRoadID());

      ATH_MSG_DEBUG (itrack << ": AUX      : phi " << phi << " cotTheta " << cotTheta << " invpT " << invpT << " d0 " << d0 << " z0 " << z0);
      //      ATH_MSG_DEBUG (itrack << ": ORIGINAL : phi " << track->getPhi() << " cotTheta " << track->getCotTh() << " invpT " << track->getInvPt() << " d0 " << track->getD0() << " z0 " << track->getZ0());
      float ftkTrkTheta = std::atan2(1.0,newTrack->getCotTh());
      float ftkTrkEta = -std::log(std::tan(ftkTrkTheta/2.));
      float ftkTrkPt=1.e12;
      if (fabs(newTrack->getInvPt()) >= 1.e-12) ftkTrkPt=1./newTrack->getInvPt();
      
      ATH_MSG_DEBUG( itrack << ": pT: " << ftkTrkPt  << " eta: " << ftkTrkEta << " phi: " <<  newTrack->getPhi() << " d0: " << newTrack->getD0() <<
		       " z0: " << newTrack->getZ0()<< " cot " << newTrack->getCotTh() << " theta " << ftkTrkTheta << " invPt " << newTrack->getInvPt() <<
		       " nPix: " << newTrack->getPixelClusters().size() << " nSCT: "<< newTrack->getSCTClusters().size()  );
      ATH_MSG_DEBUG( "     SectorID " << newTrack->getSectorID()   <<   "  RoadID "  << newTrack->getRoadID() << " LayerMap 0x" << std::hex << newTrack->getLayerMap()<<std::dec);
	

      nGoodAuxTracks++;
      outputTracks->push_back(newTrack);
    
    }
    itrack++;
  }
  ATH_MSG_DEBUG("FTK_HashIDTool::processTracks processed " << inputTracks.size() << " tracks of which " << nAuxTracks << " were Aux Tracks " << " of which " << nGoodAuxTracks << " were processed successfully");
  return outputTracks;
}

	
unsigned int FTK_HashIDTool::getHash(unsigned int tower, unsigned int sector,  unsigned int plane) {
  unsigned int hash =  0xffffffff;
  if (plane >= 12) {
    ATH_MSG_ERROR("getHash: Invalid plane " << plane);
    return hash;
  }
  if (m_moduleFromSector[tower]==nullptr) return hash;
  if ((*m_moduleFromSector[tower]).size() <= sector) return hash;
  if (plane >=(*m_moduleFromSector[tower])[sector].size()) return hash;
  return (unsigned int)(*m_moduleFromSector[tower])[sector][plane];
}


unsigned int FTK_HashIDTool::getHashFromAuxSector(unsigned int tower, unsigned int sector,  unsigned int plane) {
  unsigned int hash =  0xffffffff;
  if (plane >= 12) {
    ATH_MSG_ERROR("getHashFromAuxSector: Invalid plane " << plane);
    return hash;
  }
  if (sector>m_sector8toSector12[tower].size()) {
    ATH_MSG_ERROR("getHashFromAuxSector: Invalid sector " << sector << " max " << m_sector8toSector12.size()-1);
    return hash;
  }
  unsigned int remappedSector=m_sector8toSector12[tower][sector];
  if (m_moduleFromSector[tower]==nullptr) return hash;
  if ((*m_moduleFromSector[tower]).size() <= remappedSector) return hash;
  if (plane >=(*m_moduleFromSector[tower])[remappedSector].size()) return hash;
  return (unsigned int)(*m_moduleFromSector[tower])[remappedSector][plane];
}


int FTK_HashIDTool::readModuleIds(unsigned int itower, ftk_sectormap& hashID) {
   int nSector8L,nPlane8L;
   // define which 8L plane goes to which 12L plane
   vector<int> const remapPlanes={1,2,3, 4,6,8,9,10, 0,5,7,11};
   enum {
      ERROR_PATTFILE=1,
      ERROR_CONNFILE=2
   };


   std::stringstream ssPat, ssCon;
   ssPat << m_ConstantsDir<<"/FitConstants/"<<m_PatternsVersion<<"/sectors_raw_8L_reg"<<itower<<".patt.bz2";
   
   std::string pattfilename=ssPat.str();
   ATH_MSG_DEBUG("Sectors file: "<<pattfilename);
   const char * cpattfilename = pattfilename.c_str();
   std::unique_ptr<ftk_dcap::istream> patt_8L = std::unique_ptr<ftk_dcap::istream> (ftk_dcap::open_for_read(cpattfilename));
   if(!patt_8L) {
     ATH_MSG_WARNING("readSectorDefinition failed to open file "<<cpattfilename
		     <<" for reading, skipping");
     return ERROR_PATTFILE;
   }
   ssCon << m_ConstantsDir<<"/FitConstants/"<<m_PatternsVersion<<"/sectors_raw_8L_reg"<<itower<<".conn";
   std::string connfilename=ssCon.str();
   ATH_MSG_DEBUG("Connections file: "<<connfilename);
   const char * charconnfilename = connfilename.c_str();
   ifstream conn(charconnfilename);
   if (!conn) {
     ATH_MSG_WARNING("readSectorDefinition failed to open file "<<charconnfilename
		     <<" for reading, skipping");

     return ERROR_CONNFILE;
   }

   (*patt_8L)>>nSector8L>>nPlane8L;
   // check errors here
   //   (*patt_8L).fail(), nSector8L>0, 0<nPlane8L<remapPlanes.size()
   int lastSector8=-1;
   int lastSector12=-1;
   int error=0;

   unsigned int icount=0;
   while(!((*patt_8L).eof()||conn.eof())) {
      int iSectorPatt,iSectorConn;
      (*patt_8L)>>iSectorPatt;
      conn>>iSectorConn;
      if((*patt_8L).fail()||conn.fail()) {
         // end
         break;
      }
      if((iSectorPatt!=lastSector8+1)||
         (iSectorPatt!=iSectorConn)||
         (iSectorPatt<0)||(iSectorPatt>=nSector8L)) {
         error=ERROR_PATTFILE;
         // file is corrupted
         break;
      }

      m_sector8toSector12[itower].push_back(icount);

      vector<int> moduleData(remapPlanes.size());
      // read 8L module IDs
      for(int k=0;k<nPlane8L;k++) {
         (*patt_8L)>>moduleData[remapPlanes[k]];
      }
      int dummy;
      // skip dummy entries
      (*patt_8L)>>dummy;
      (*patt_8L)>>dummy;
      // read connection data and store 12L module data

      int n12;
      conn>>n12;
      for(int i=0;i<n12;i++) {
         int sector12;
         conn>>dummy>>sector12;
         if(lastSector12+1!=sector12) {
            error=ERROR_CONNFILE;
            break;
         }
         for(unsigned int k=nPlane8L;k<remapPlanes.size();k++) {
            conn>>moduleData[remapPlanes[k]];
         }
	 hashID.push_back(moduleData);
         lastSector12=sector12;
	 icount++;
      }
      if(error) break;

      lastSector8=iSectorPatt;
   }

     

   return error;
}



const Amg::Vector3D FTK_HashIDTool::getPixelClusterPosition(const FTK_RawPixelCluster& raw_pixel_cluster, const bool reverseIBL) {
  IdentifierHash hash = raw_pixel_cluster.getModuleID();
  Identifier wafer_id = m_pixelId->wafer_id(hash); // Need to set up this tool
  const InDetDD::SiDetectorElement* pDE = m_pixelManager->getDetectorElement(hash);

  ATH_MSG_VERBOSE( "FTK_HashIDTool::getPixelClusterPosition: Pixel FTKHit hashID 0x" << std::hex << hash << std::dec << " " << m_id_helper->print_to_string(pDE->identify()));

  const InDetDD::SiDetectorElement* pixelDetectorElement = m_pixelManager->getDetectorElement(hash);
  const InDetDD::PixelModuleDesign* design
    (dynamic_cast<const InDetDD::PixelModuleDesign*>(&pixelDetectorElement->design()));

  ATH_MSG_VERBOSE( "FTK_HashIDTool::getPixelClusterPosition: raw FTK cluster position: " <<
      " Row(phi): " <<  raw_pixel_cluster.getRowCoord() << " Col(eta): " << raw_pixel_cluster.getColCoord() <<
      " RowWidth: " << raw_pixel_cluster.getRowWidth() << " ColWidth: " << raw_pixel_cluster.getColWidth());

  unsigned int layer = m_pixelId->layer_disk(wafer_id);
  bool isBarrel = (m_pixelId->barrel_ec(wafer_id)==0);


  if (isBarrel) {
    if (layer==0) {
      ATH_MSG_VERBOSE( "FTK_HashIDTool::getPixelClusterPosition: IBL ");
    } else {
      ATH_MSG_VERBOSE( "FTK_HashIDTool::getPixelClusterPosition: Pixel Barrel layer  " << layer);
    }
  } else {
    ATH_MSG_VERBOSE( "FTK_HashIDTool::getPixelClusterPosition: Pixel Endcap layer  " << layer);
  }
  ATH_MSG_VERBOSE( "FTK_HashIDTool::getPixelClusterPosition: Module rows= " << design->rows() << " phiPitch= " << design->phiPitch() << " width= " << design->width() );
  ATH_MSG_VERBOSE( "FTK_HashIDTool::getPixelClusterPosition: columns = " << design->columns() << " etaPitch= " << design->etaPitch() <<  " length " << design->length());

  int rawLocalPhiCoord;
  
  if (reverseIBL && isBarrel && layer==0) {
    rawLocalPhiCoord = 2680 -  raw_pixel_cluster.getRowCoord(); //335*8=2680
  } else {
    rawLocalPhiCoord = raw_pixel_cluster.getRowCoord();
  }

  int rawLocalEtaCoord= raw_pixel_cluster.getColCoord();

  const InDetDD::SiCellId cornerCell(0, 0);
  const InDetDD::SiLocalPosition localPositionOfCornerCell = design->localPositionOfCell(cornerCell);
  const double phi0 = localPositionOfCornerCell.xPhi();
  const double eta0 = localPositionOfCornerCell.xEta();

  ATH_MSG_VERBOSE( "FTK_HashIDTool::getPixelClusterPosition: local position of pixel at (0,0) is "<<  phi0 << ",  " << eta0);



  // ftk row coordinates: units of 6.25 microns, origin is centre of zeroth pixel
  // ftk column coordinates: units of 25 microns, origin is edge of the zeroth coordinate

  double phiPos = ((double) rawLocalPhiCoord) * 6.25e-3 + phi0; // rawLocalPhiCoord=0 is the centre of the zeroth pixel

  if (!isBarrel) phiPos-=0.025; // correcttion for the fact that rawLocalPhiCoord=0 seems to be the edge of the endcap pixel, not the centre!
  
  double etaPos=0.;
  if (isBarrel && layer==0) {
    etaPos = ((double) rawLocalEtaCoord) * 25.0e-3 + eta0 - 0.25; // rawLocalEtaCoord=0 is the edge (-0.25mm) of the zeroth IBL pixel.
  } else {
    etaPos = ((double) rawLocalEtaCoord) * 25.0e-3 + eta0 - 0.3; // rawLocalEtaCoord=0 is the edge (-0.3mm) of the zeroth pixel.
  }

  ATH_MSG_VERBOSE( "FTK_HashIDTool::getPixelClusterPosition: Cluster position phiPos, etaPos "<<  phiPos << ", " << etaPos);

  if ( etaPos >  design->length()/2.) {
    ATH_MSG_DEBUG( "FTK_HashIDTool::getPixelClusterPosition: Illegal position: Setting Cluster eta position to module length/2.");
    etaPos = design->length()/2.;
  } else if (  etaPos <  -design->length()/2. ) {
    etaPos = -design->length()/2.;
    ATH_MSG_DEBUG( "FTK_HashIDTool::getPixelClusterPosition: Illegal position: Setting Cluster eta position to -(module length/2.)");
  }
  if ( phiPos >  design->width()/2.) {
    phiPos = design->width()/2.;
    ATH_MSG_DEBUG( "FTK_HashIDTool::getPixelClusterPosition: Illegal position: Setting Cluster phi position to module width/2.");
  } else if (  phiPos <  -design->width()/2. ) {
    phiPos = -design->width()/2.;
    ATH_MSG_DEBUG( "FTK_HashIDTool::getPixelClusterPosition: Illegal position: Setting Cluster phi position to -(module width/2.)");
  }
  const InDetDD::SiLocalPosition cellLocalPosition(etaPos,phiPos);

  const InDetDD::SiCellId  cell = design->cellIdOfPosition(cellLocalPosition);

  int phi_index = cell.phiIndex();
  int eta_index = cell.etaIndex();
  ATH_MSG_VERBOSE( "FTK_HashIDTool::getPixelClusterPosition: Cell created with phiIndex " << phi_index << " eta_index " <<   eta_index << " expected: phiIndex " << (int) ((phiPos-phi0)/design->phiPitch()) << " eta index " << (int) ((etaPos-eta0)/design->etaPitch()));


  Identifier pixel_id = m_pixelId->pixel_id(wafer_id, phi_index, eta_index);


  int phiWidth    = std::max(raw_pixel_cluster.getRowWidth(),1u);
  int etaWidth    = std::max(raw_pixel_cluster.getColWidth(),1u);

  int colMin = (int)(eta_index-0.5*etaWidth);
  int colMax = colMin+etaWidth;

  int rowMin = (int)(phi_index-0.5*phiWidth);
  int rowMax = rowMin+phiWidth;


  double etaW = design->widthFromColumnRange(colMin, colMax-1);
  double phiW = design->widthFromRowRange(rowMin, rowMax-1);


  ATH_MSG_VERBOSE("phiW = design->widthFromRowRange("<<rowMin<<","<<rowMax-1<<") = " << phiW);
  ATH_MSG_VERBOSE("etaW = design->widthFromRowRange("<<colMin<<","<<colMax-1<<") = " << etaW);

  InDet::SiWidth siWidth(Amg::Vector2D(phiWidth,etaWidth),Amg::Vector2D(phiW,etaW));

  Amg::Vector2D position(phiPos,etaPos);

  ATH_MSG_VERBOSE("FTK_HashIDTool::getPixelClusterPosition: local coordinates phiPos, etaPos"<<  phiPos << ", " << etaPos);
  ATH_MSG_VERBOSE("FTK_HashIDTool::getPixelClusterPosition: FTK cluster phiwidth " << phiWidth << " etawidth " <<  etaWidth << " siWidth.phiR() " << siWidth.phiR() << " siWidth.z() " << siWidth.z());

  std::vector<Identifier> rdoList;
  rdoList.push_back(pixel_id);

  Amg::MatrixX* cov = new Amg::MatrixX(2,2);
  cov->setZero();

  (*cov)(0,0) = siWidth.phiR()*siWidth.phiR()*ONE_TWELFTH;
  (*cov)(1,1) = siWidth.z()*siWidth.z()* ONE_TWELFTH;

  ATH_MSG_VERBOSE("FTK_HashIDTool::getPixelClusterPosition: Setting defaulterrors (*cov)(0,0) " <<  (*cov)(0,0) << " (*cov)(1,1)" << (*cov)(1,1) );

  InDet::PixelCluster pixel_cluster(pixel_id, position, rdoList, siWidth,pixelDetectorElement, cov);

  Amg::Vector3D pos (pixel_cluster.globalPosition().x(),pixel_cluster.globalPosition().y(), pixel_cluster.globalPosition().z());

  return pos;
}
