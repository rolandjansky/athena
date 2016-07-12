/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/MuonCalibGeometryDumper.h"
#include "MdtTubeGeomertyRow.h"
#include "MdtStationGeometryRow.h"


#include <stdexcept>

// MuonReadoutGeometry //
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"

//geomodel
#include "GeoModelInterfaces/IGeoModelSvc.h"

//root
#include "TFile.h"
#include "TTree.h"
#include "TNamed.h"


namespace MuonCalib {

MuonCalibGeometryDumper::MuonCalibGeometryDumper(const std::string &name, ISvcLocator *pSvcLocator) : AthAlgorithm(name, pSvcLocator){

  m_MDT_ID_helper = std::string("MDTIDHELPER");
  declareProperty("MDTIdHelper", m_MDT_ID_helper);

  m_idToFixedIdToolType = std::string("MuonCalib::IdToFixedIdTool");
  declareProperty("idToFixedIdToolType", m_idToFixedIdToolType);

  m_idToFixedIdToolName = std::string("MuonCalib_IdToFixedIdTool");
  declareProperty("idToFixedIdToolName", m_idToFixedIdToolName);
  
  m_rootFile = "geom.root";
  declareProperty("RootFile", m_rootFile);
		
  //for the sake of coverity
  m_MdtIdHelper=NULL;
  m_detMgr =NULL;
  m_id_tool=NULL;
}  //end MuonCalibGeometryDumper::MuonCalibGeometryDumper

MuonCalibGeometryDumper::~MuonCalibGeometryDumper() {
}

StatusCode MuonCalibGeometryDumper::initialize() {

// MDT ID helper //
  ATH_CHECK( detStore()->retrieve(m_MdtIdHelper, m_MDT_ID_helper) );

// muon detector manager //
  ATH_CHECK( detStore()->retrieve(m_detMgr) );

// muon fixed id tool //
  ATH_CHECK( toolSvc()->retrieveTool(m_idToFixedIdToolType,
			       m_idToFixedIdToolName, m_id_tool) );

  const IGeoModelSvc *geoModel;
  ATH_CHECK( service ("GeoModelSvc", geoModel) );
	
  TFile *output_file = new TFile(m_rootFile.c_str(), "RECREATE");
  TNamed geometry_version("ATLASVersion", geoModel->atlasVersion().c_str());
  geometry_version.Write();
  dump_mdt_geometry();
  output_file->Write();
  return StatusCode::SUCCESS;
}  //end MuonCalibGeometryDumper::initialize

inline bool MuonCalibGeometryDumper::dump_mdt_geometry() {
  MdtTubeGeomertyRow row;
  TTree * mdt_tubes=new TTree("mdt_tubes", "mdt tubes");
  mdt_tubes->Branch("id", &row.tube_id, "id/i");
  mdt_tubes->Branch("pos_x", &row.pos_x, "pos_x/F");
  mdt_tubes->Branch("pos_y", &row.pos_y, "pos_y/F");
  mdt_tubes->Branch("pos_z", &row.pos_z, "pos_z/F");
  MdtStationGeometryRow station_row;
  UInt_t station_id;
  TTree * mdt_station = new TTree ("mdt_station", "mdt station");
  mdt_station->Branch("id", &station_id, "id/i");
  station_row.CreateBranches(mdt_station);
//loop on chambers
  MdtIdHelper::const_id_iterator it     = m_MdtIdHelper->module_begin();
  MdtIdHelper::const_id_iterator it_end = m_MdtIdHelper->module_end();
  for( ; it!=it_end;++it ) {
    std::cout<<"."<<std::flush;
    const MuonGM::MdtReadoutElement *detEl = m_detMgr->getMdtReadoutElement( m_MdtIdHelper->channelID(*it,1,1,1));
    if(!detEl) continue;
    station_row.ReadHepTransform(detEl->AmdbLRSToGlobalTransform());
    MuonFixedId fixed_id(m_id_tool->idToFixedId(*it));
    station_id=fixed_id.mdtChamberId().getIdInt();
    mdt_station->Fill();
    //get number of mls;
    int n_mls=m_MdtIdHelper->numberOfMultilayers(*it);
    //loop on multilayers
    for(int ml=1; ml<=n_mls; ml++) {
      const MuonGM::MdtReadoutElement *detEl_ml = m_detMgr->getMdtReadoutElement(m_MdtIdHelper->channelID(*it,ml ,1,1));
      int n_layers=detEl_ml->getNLayers();
//			if(detEl_ml==NULL) {
//				std::cerr<<"detEl_ml==NULL"<<std::endl;
//				}
      for(int ly=1; ly<=n_layers; ly++)	{
	fillLayer(*it, detEl_ml, row, mdt_tubes, ml, ly);
      }
    }
  }
  return true;
}  //end MuonCalibGeometryDumper::dump_mdt_geometry

inline void MuonCalibGeometryDumper::fillLayer(const Identifier &ch_id,
const MuonGM::MdtReadoutElement *detEl, MdtTubeGeomertyRow &row, TTree *tree, const int &ml, const int &ly) {
//	std::cout<<"fillTubePos for "<<ml<<", "<<ly<<", "<<tb<<std::endl;
  int n_tubes=detEl->getNtubesperlayer();
  for(int tb=1; tb<=n_tubes; tb++) {
    Identifier tid(m_MdtIdHelper->channelID(ch_id ,ml ,ly, tb));
    MuonFixedId fixed_id(m_id_tool->idToFixedId(tid));
    row.tube_id=fixed_id.getIdInt();
    Amg::Vector3D tube_pos_g=detEl->tubePos(ml, ly, tb);
    Amg::Vector3D tube_pos_l=detEl->GlobalToAmdbLRSCoords(tube_pos_g);
    row.pos_x=tube_pos_l.x();
    row.pos_y=tube_pos_l.y();
    row.pos_z=tube_pos_l.z();
    tree->Fill();
  }
}  //end MuonCalibGeometryDumper::fillLayer

} //namespace MuonCalib
