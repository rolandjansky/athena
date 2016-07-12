/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CalibNtupleAnalysisAlg/GeoDraw.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "MdtStationGeometryRow.h"

#include "TTree.h"
#include "TFile.h"
#include "TDirectory.h"
#include "MdtStationGeometryRow.h"
#include "iostream"

namespace MuonCalib {

inline unsigned int get_station_id(unsigned int &tube_id) {
  MuonFixedId id(tube_id);
  return id.mdtChamberId().getIdInt();
}

class GeoDraw_Data {
public:
  std::map<unsigned int, HepGeom::Point3D<double> > tube_positions;
  std::map<unsigned int, HepGeom::Transform3D> local_to_global;
  std::map<unsigned int, HepGeom::Transform3D> global_to_local;
};

GeoDraw_Data *GeoDraw::s_data = NULL;

void GeoDraw::Load(const char *filename) {
  if(s_data) delete s_data;
  s_data = new GeoDraw_Data();
  TDirectory *lastdir=gDirectory;
  TFile inf(filename);
  TTree *mdt_tubes=dynamic_cast<TTree *>(inf.Get("mdt_tubes"));
  if(!mdt_tubes) return;
  UInt_t id;
  Float_t pos_x, pos_y, pos_z;
  mdt_tubes->SetBranchAddress("id", &id);
  mdt_tubes->SetBranchAddress("pos_x", &pos_x);
  mdt_tubes->SetBranchAddress("pos_y", &pos_y);
  mdt_tubes->SetBranchAddress("pos_z", &pos_z);
  TNamed *ATLASVersion = dynamic_cast<TNamed *>(inf.Get("ATLASVersion"));
  if (ATLASVersion)
    std::cout<<"Reading geometry "<<ATLASVersion->GetTitle()<<std::endl;
  std::cout<<"Loading "<<mdt_tubes->GetEntries()<<" tubes"<<std::endl;
  for(int i=0; i<mdt_tubes->GetEntries(); i++) {
    mdt_tubes->GetEntry(i);
    s_data->tube_positions[id].setX(pos_x);
    s_data->tube_positions[id].setY(pos_y);
    s_data->tube_positions[id].setZ(pos_z);
  }
		
  TTree *mdt_station=dynamic_cast<TTree *>(inf.Get("mdt_station"));
  if (!mdt_station) return;
  MdtStationGeometryRow row;
  row.SetBranchAddress(mdt_station);
  mdt_station->SetBranchAddress("id", &id);
  std::cout<<"Loading "<<mdt_station->GetEntries()<<" stations"<<std::endl;
  for(int i=0; i<mdt_station->GetEntries(); i++) {
    mdt_station->GetEntry(i);
    s_data->local_to_global[id] = row.GetTransform();
    s_data->global_to_local[id] = s_data->local_to_global[i].inverse();
  }
  lastdir->cd();
}  //end GeoDraw::Load

double GeoDraw::PosX(unsigned int tube_id) {
  std::map<unsigned int, HepGeom::Point3D<double> >::const_iterator it(s_data->tube_positions.find(tube_id));
  if(it==s_data->tube_positions.end()) {
    std::cout<<"Tube not found"<<std::endl;
    return 9e9;
  }
  return it->second.x();
}
	
double GeoDraw::PosY(unsigned int tube_id) {
  std::map<unsigned int, HepGeom::Point3D<double> >::const_iterator it(s_data->tube_positions.find(tube_id));
  if(it==s_data->tube_positions.end()) {
    std::cout<<"Tube not found"<<std::endl;
    return 9e9;
  }
  return it->second.y();
}
	
double GeoDraw::PosZ(unsigned int tube_id) {
  std::map<unsigned int, HepGeom::Point3D<double> >::const_iterator it(s_data->tube_positions.find(tube_id));
  if(it==s_data->tube_positions.end()) {
    std::cout<<"Tube not found"<<std::endl;
    return 9e9;
  }
  return it->second.z();
}
	
double GeoDraw::GPosX(unsigned int tube_id) {
  unsigned int station_id(get_station_id(tube_id));
  std::map<unsigned int, HepGeom::Transform3D>::const_iterator tr_it(s_data->local_to_global.find(station_id));
  if(tr_it==s_data->local_to_global.end()) return -9e9;
  std::map<unsigned int, HepGeom::Point3D<double> >::const_iterator it(s_data->tube_positions.find(tube_id));
  if(it==s_data->tube_positions.end()) return 9e9;
  return (tr_it->second * it->second).x();
}

double GeoDraw::GPosY(unsigned int tube_id) {
  unsigned int station_id(get_station_id(tube_id));
  std::map<unsigned int, HepGeom::Transform3D>::const_iterator tr_it(s_data->local_to_global.find(station_id));
  if(tr_it==s_data->local_to_global.end()) return -9e9;
  std::map<unsigned int, HepGeom::Point3D<double> 
    >::const_iterator it(s_data->tube_positions.find(tube_id));
  if(it==s_data->tube_positions.end()) return 9e9;
  return (tr_it->second * it->second).y();
}

double GeoDraw::GPosZ(unsigned int tube_id) {
  unsigned int station_id(get_station_id(tube_id));
  std::map<unsigned int, HepGeom::Transform3D>::const_iterator tr_it(s_data->local_to_global.find(station_id));
  if(tr_it==s_data->local_to_global.end()) return -9e9;
  std::map<unsigned int, HepGeom::Point3D<double> >::const_iterator it(s_data->tube_positions.find(tube_id));
  if(it==s_data->tube_positions.end()) return 9e9;
  return (tr_it->second * it->second).z();
}

double GeoDraw::LocalToGlobalX(unsigned int id, double x, double y, double z) {
  unsigned int station_id(get_station_id(id));
  std::map<unsigned int, HepGeom::Transform3D>::const_iterator tr_it(s_data->local_to_global.find(station_id));
  if(tr_it==s_data->local_to_global.end()) return -9e9;
  HepGeom::Point3D<double> p(x, y, z);
  return (tr_it->second * p).x();
}
  
double GeoDraw::LocalToGlobalY(unsigned int id, double x, double y, double z) {
  unsigned int station_id(get_station_id(id));
  std::map<unsigned int, HepGeom::Transform3D>::const_iterator tr_it(s_data->local_to_global.find(station_id));
  if(tr_it==s_data->local_to_global.end()) return -9e9;
  HepGeom::Point3D<double> p(x, y, z);
  return (tr_it->second * p).y();
}
	
double GeoDraw::LocalToGlobalZ(unsigned int id, double x, double y, double z) {
  unsigned int station_id(get_station_id(id));
  std::map<unsigned int, HepGeom::Transform3D>::const_iterator tr_it(s_data->local_to_global.find(station_id));
  if(tr_it==s_data->local_to_global.end()) return -9e9;
  HepGeom::Point3D<double> p(x, y, z);
  return (tr_it->second * p).z();
}

double GeoDraw::GlobalToLocalX(unsigned int id, double x, double y, double z) {
  unsigned int station_id(get_station_id(id));
  std::map<unsigned int, HepGeom::Transform3D>::const_iterator tr_it(s_data->global_to_local.find(station_id));
  if(tr_it==s_data->global_to_local.end()) return -9e9;
  HepGeom::Point3D<double> p(x, y, z);
  return (tr_it->second * p).x();
}
	
double GeoDraw::GlobalToLocalY(unsigned int id, double x, double y, double z) {
  unsigned int station_id(get_station_id(id));
  std::map<unsigned int, HepGeom::Transform3D>::const_iterator tr_it(s_data->global_to_local.find(station_id));
  if(tr_it==s_data->global_to_local.end()) return -9e9;
  HepGeom::Point3D<double> p(x, y, z);
  return (tr_it->second * p).y();
}
	
double GeoDraw::GlobalToLocalZ(unsigned int id, double x, double y, double z) {
  unsigned int station_id(get_station_id(id));
  std::map<unsigned int, HepGeom::Transform3D>::const_iterator tr_it(s_data->global_to_local.find(station_id));
  if(tr_it==s_data->global_to_local.end()) return -9e9;
  HepGeom::Point3D<double> p(x, y, z);
  return (tr_it->second * p).z();
}

}  //namespace MuonCalib
