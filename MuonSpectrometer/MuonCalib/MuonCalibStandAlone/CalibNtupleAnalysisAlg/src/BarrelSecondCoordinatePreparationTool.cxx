/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 23.01.2008, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF THE METHODS DEFINED IN THE CLASS ::
//::      BarrelSecondCoordinatePreparationTool         ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// standard C++ //
#include <iostream>
#include <fstream>

// MuonCalib //
#include "CalibNtupleAnalysisAlg/BarrelSecondCoordinatePreparationTool.h"

//MuonReadoutGeometry
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

//MuonCalibITools
#include "MuonCalibITools/IIdToFixedIdTool.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
//::::::::::::::::::::::::
//:: NAMESPACE SETTINGS ::
//::::::::::::::::::::::::

using namespace std;
using namespace MuonCalib;

//*****************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::
BarrelSecondCoordinatePreparationTool::BarrelSecondCoordinatePreparationTool(
				    const std::string &t,
                                    const std::string &n,
                                    const IInterface *p) : AthAlgTool(t, n, p) {

///////////////////////
// DECLARE INTERFACE //
///////////////////////
  declareInterface< CalibSegmentPreparationTool >(this);
}

//*****************************************************************************

////////////////
// DESTRUCTOR //
////////////////
BarrelSecondCoordinatePreparationTool::~BarrelSecondCoordinatePreparationTool(void) {
}

//*****************************************************************************

///////////////////////
// METHOD initialize //
///////////////////////
StatusCode BarrelSecondCoordinatePreparationTool::initialize(void) {

/////////////
// OBJECTS //
/////////////

/////////////
// MESSAGE //
/////////////
  ATH_MSG_INFO( "Initializing tool..." );

// muon detector manager //
  ATH_CHECK( detStore()->retrieve(m_detMgr) );

// muon fixed id tool //
  ATH_CHECK( toolSvc()->retrieveTool("MuonCalib::IdToFixedIdTool",
			       "MuonCalib_IdToFixedIdTool", m_id_tool) );

  write_rpc_hits = false;

  if((m_rpcHitsFile=fopen("RpcHits.txt","w"))==NULL) {
    ATH_MSG_INFO( "can't open file" );
  }

  return StatusCode::SUCCESS;
}  //end BarrelSecondCoordinatePreparationTool

//*****************************************************************************

/////////////////////
// METHOD finalize //
/////////////////////
StatusCode BarrelSecondCoordinatePreparationTool::finalize(void) {

/////////////
// OBJECTS //
/////////////

/////////////
// MESSAGE //
/////////////
  ATH_MSG_INFO( "Finalizing tool..." );
  fclose(m_rpcHitsFile);
  
  return StatusCode::SUCCESS;
}  //end BarrelSecondCoordinatePreparationTool::finalize

//*****************************************************************************

////////////////////////////
// METHOD prepareSegments //
////////////////////////////
void BarrelSecondCoordinatePreparationTool::prepareSegments(
  const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments) {

/////////////
// OBJECTS //
/////////////
  std::vector<MuonCalibRawRpcHit*>::const_iterator raw_RPC_begin_it(
    event->rawHitCollection()->rawRpcHitCollectionBegin());
  std::vector<MuonCalibRawRpcHit*>::const_iterator raw_RPC_end_it(
    event->rawHitCollection()->rawRpcHitCollectionEnd());

///////////////////////////////////////////////////////////////////////////////
// LOOP OVER THE SEGMENTS AND PASS THE BARREL SEGMENTS TO THE 2ND COORDINATE //
// RECONSTRUCTION TOOL                                                       //
///////////////////////////////////////////////////////////////////////////////
  std::set<NtupleStationId> to_delete;
  for (std::map<NtupleStationId, MuonCalibSegment *> :: iterator it=
	 segments.begin(); it!= segments.end(); it++) {

// ignore end-cap chambers //
    if (it->second->mdtHOT()[0]->identify().stationNameString()[0]!='B') {
      continue;
    }

// COLLECT RAW RPC HITS
    vector<MuonCalibRawRpcHit *> raw_hits;
    for (vector<MuonCalibRawRpcHit *>::const_iterator it1=raw_RPC_begin_it; it1!=raw_RPC_end_it; it1++) {
      raw_hits.push_back(*it1);
    }

    if(write_rpc_hits) fprintf(m_rpcHitsFile,"START %i %s %i %i\n", 
			       event->eventInfo().eventNumber(),
			       it->second->mdtHOT()[0]->identify().stationNameString().c_str(),
			       it->second->mdtHOT()[0]->identify().phi(),
			       it->second->mdtHOT()[0]->identify().eta());

// reconstruct the second coordinate //
    if (!handleRPChits(*(it->second), raw_hits)) {
      //			cout<<"RPC FIT FAILED!"<<endl;
      to_delete.insert(it->first);
    }

  }

  for(std::set<NtupleStationId> :: const_iterator it=to_delete.begin(); it!=to_delete.end(); it++) {
    segments.erase(segments.find(*it));
  }	

  return;
}  //end BarrelSecondCoordinatePreparationTool::prepareSegments

//::::::::::::::::::::::::::
//:: METHOD handleRPChits ::
//::::::::::::::::::::::::::
bool BarrelSecondCoordinatePreparationTool::handleRPChits(MuonCalibSegment & MDT_segment,
				vector<MuonCalibRawRpcHit *> & raw_hits) {
  //Segment parameters
  MuonFixedId seg_ID(MDT_segment.mdtHOT()[0]->identify());

  Amg::Vector3D seg_pos(MDT_segment.position());
  Amg::Vector3D seg_dir(MDT_segment.direction());

  Amg::Transform3D Segment2Global(MDT_segment.localToGlobal());
  Amg::Transform3D Global2Segment((MDT_segment.localToGlobal()).inverse());
  
  if(write_rpc_hits) {
    Amg::Vector3D old_point = Amg::Vector3D(seg_pos.x(), 0.0, seg_pos.z());
    Amg::Vector3D old_vector = Amg::Vector3D(seg_dir.x()/seg_dir.z(), 0, 1.0);
    old_point = Segment2Global*old_point;
    old_vector = Segment2Global.linear()*old_vector;

    fprintf(m_rpcHitsFile,"old_line: x=%11.3f y=%11.3f a=%10.5f\n",
	    old_point.x(), old_point.y(), old_vector.x()/old_vector.y());
  }

  int seg_sector_ind; //hardware phi-index
  if(seg_ID.stationNameString()[2]=='L' || seg_ID.stationNameString()[2]=='R' || 
     seg_ID.stationNameString()[2]=='M') {
    seg_sector_ind = 2*seg_ID.phi() - 1;
  } else {
    seg_sector_ind = 2*seg_ID.phi();
  }

  seg_pos[0]=0.;
  if(std::isnan(seg_dir.y())) return false;
  seg_dir[0]=0.;
  seg_dir = seg_dir.unit();

  ///////////////////
  //Handle RPC hits//
  ///////////////////
  int old_name(0);
  int old_phi(0);
  int old_eta(0);
  int old_R(0);
  int num_total(0);
  int num_current(0);

  vector<CLHEP::HepVector> RPChits;
  vector<int> in_seg_sector;
  vector<int> num_hits_same_layer;

  vector<MuonCalibRawRpcHit *>::iterator raw_it = raw_hits.begin();
  while (raw_it != raw_hits.end()) {
    bool bad_hit = false;
    MuonFixedId ID((*raw_it)->identify());
    if(ID.rpcMeasuresPhi() != 1) bad_hit = true;

    int sector_ind; //hardware phi-index
    if(ID.stationNameString()[2]=='L' || ID.stationNameString()[2]=='R' || 
       ID.stationNameString()[2]=='M') {
      sector_ind = 2*ID.phi() - 1;
    } else {
      sector_ind = 2*ID.phi();
    }

    int in_sector(2);//0-in the same sector; 1-in the adjacent sector; 2-other sectors
    if(sector_ind==seg_sector_ind) in_sector=0;
    if(abs(sector_ind-seg_sector_ind)==1) in_sector=1;
    if(abs(sector_ind-seg_sector_ind)==15) in_sector=1;

    //Suppression of noise in sector 14
//		if(ID.stationName()==11 && ID.phi()==7 && ID.eta()==0) bad_hit = true;
//		if(ID.stationName()==10 && ID.phi()==7 && ID.eta()==-1) bad_hit = true;
//		if(ID.stationName()==11 && ID.phi()==7 && ID.eta()==-1) in_sector=3;
//		if(ID.stationName()==10 && ID.phi()==7 && ID.eta()==1) in_sector=3;
//		if(ID.stationName()==10 && ID.phi()==7 && ID.eta()==-2) in_sector=3;

    //RPC hits positions
    Amg::Vector3D glb_pos((*raw_it)->globalPosition());
    Amg::Vector3D hit_pos(Global2Segment*glb_pos);

    CLHEP::HepVector p_x(3,0);
    p_x[0] = hit_pos.x();
    p_x[1] = hit_pos.z();
    p_x[2] = ((*raw_it)->width())/3.464;

    //r-phi pattern
    double diff;
    diff = hit_pos.y() - (seg_pos.y() + (hit_pos.z()-seg_pos.z())*seg_dir.y()/seg_dir.z());
    if(!(fabs(diff)<((*raw_it)->length()+400.0))) bad_hit = true;

    if(write_rpc_hits){
      if(ID.rpcMeasuresPhi()==1){
	fprintf(m_rpcHitsFile,"%i %i %i x=%11.3f y=%11.3f z=%11.3f %i\n",
		ID.stationName(), ID.phi(), ID.eta(), glb_pos.x(), 
		glb_pos.y(), glb_pos.z(), int(bad_hit));
      }
    }

    if(bad_hit) {
      raw_it=raw_hits.erase(raw_it);
    } else {
      raw_it++;
      RPChits.push_back(p_x);
      in_seg_sector.push_back(in_sector);
      //calculate number of hits in the same RPC layer (for downweighting)
      num_hits_same_layer.push_back(1);
      if(ID.stationName()==old_name && ID.phi()==old_phi && ID.eta()==old_eta &&
	 ID.rpcDoubletR()==old_R) {
	num_current++;
      } else {
	if(num_total) {
	  num_current++;
	  for(int m=(num_total-num_current); m<num_total; m++) {
	    num_hits_same_layer[m] = num_current;
	  }
	}
	num_current=0;
	old_name = ID.stationName();
	old_phi = ID.phi();
	old_eta = ID.eta();
	old_R = ID.rpcDoubletR();
      }
      num_total++;
    }
  }
  
  if(num_total) {
    num_current++;
    for(int m=(num_total-num_current); m<num_total; m++) {
      num_hits_same_layer[m] = num_current;
    }
  }
  
  bool x_fit = false;
  CLHEP::HepVector alph(2,0);
  double ang_err(100.0);
  if(!rpcFit(RPChits, in_seg_sector, num_hits_same_layer, 25.0, alph, ang_err) 
     && ang_err<0.05) x_fit=true;
  
  if(!x_fit) return false;
  
  // update second coordinate for MDT hits
  for (unsigned int l=0; l<MDT_segment.mdtHOT().size(); l++) {
    MuonFixedId ID(MDT_segment.mdtHOT()[l]->identify());
    Identifier atl_id = m_id_tool->fixedIdToId(ID);
    const MuonGM::MdtReadoutElement *MdtRoEl = m_detMgr->getMdtReadoutElement(atl_id);
    double tube_length(MdtRoEl->tubeLength(atl_id));
    
    Amg::Vector3D loc_position(MDT_segment.mdtHOT()[l]->localPosition());
    double x_loc = alph[0] + alph[1]*loc_position.z();
    if(fabs(x_loc)>(0.5*tube_length + 300.0)) {
//			cout<<"HUGE!"<<endl;
      return false;
    }
    if(fabs(x_loc) > 0.5*tube_length) {
//			cout<<"LARGE!"<<endl;
      x_loc = 0.5*(fabs(x_loc)/x_loc)*tube_length;
    }
//cout<<"x_old="<<loc_position.x();

    loc_position[0]=(x_loc);
//cout<<" x_new="<<loc_position.x()<<" z="<<loc_position.z()<<endl;
    MDT_segment.mdtHOT()[l]->setLocalPos(loc_position);
    MDT_segment.mdtHOT()[l]->setGlobalPos(Segment2Global*loc_position);
    //set signal propagatino time
    MDT_segment.mdtHOT()[l]->setDriftTime( MDT_segment.mdtHOT()[l]->driftTime() + MDT_segment.mdtHOT()[l]->propagationTime());
    MDT_segment.mdtHOT()[l]->setDistanceToReadout(MdtRoEl-> distanceFromRO( MDT_segment.mdtHOT()[l]->globalPosition(), atl_id));
    MDT_segment.mdtHOT()[l]->setPropagationTime( (MDT_segment.mdtHOT()[l]->distanceToReadout() - tube_length/2.0) * 0.0033356409519815205);
    MDT_segment.mdtHOT()[l]->setDriftTime( MDT_segment.mdtHOT()[l]->driftTime() - MDT_segment.mdtHOT()[l]->propagationTime());
    
  }

  // update second coordinate for MDT segment
  seg_pos[0]=(alph[0] + alph[1]*seg_pos.z());

  seg_dir[0]=(alph[1]);
  seg_dir[1]=(seg_dir.y()/seg_dir.z());
  seg_dir[2]=1.0;

  MDT_segment.set(MDT_segment.chi2(), seg_pos, seg_dir);

  if(write_rpc_hits) {
    Amg::Vector3D fit_point = Amg::Vector3D(alph[0], 0, 0);
    Amg::Vector3D fit_vector = Amg::Vector3D(alph[1], 0, 1.0);
    fit_point = Segment2Global*fit_point;
    fit_vector = Segment2Global*fit_vector;

    fprintf(m_rpcHitsFile,"fit_line: x=%11.3f y=%11.3f a=%10.5f\n",
	    fit_point.x(), fit_point.y(), fit_vector.x()/fit_vector.y());
  }

  return true;
}  //end BarrelSecondCoordinatePreparationTool::handleRPChits


/////////////////////////////////////////////
// Fit by RPC hits                         //
/////////////////////////////////////////////
int BarrelSecondCoordinatePreparationTool::rpcFit(vector<CLHEP::HepVector> &RPC_hits,
				vector<int> in_sect, vector<int> num_same, double max_r,
				CLHEP::HepVector &tr_par, double &angle_err) {
  int ierr(0); // Status of Matrix Inversion

  if(RPC_hits.size()<2) return -1;

  CLHEP::HepMatrix Gmm = CLHEP::HepMatrix(2,2,0);
  CLHEP::HepVector bth = CLHEP::HepVector(2,0);
  for(unsigned int j=0;j<RPC_hits.size();j++) {
    CLHEP::HepVector dlt = CLHEP::HepVector(2,0);
    dlt[0] = 1.0;
    dlt[1] = RPC_hits[j][1];
    if(RPC_hits[j][2]<=0) RPC_hits[j][2]=10000.0;
    double wght = 1.0/(RPC_hits[j][2]*RPC_hits[j][2]);
    wght = wght/(double(num_same[j]));
    if(in_sect[j]==1) wght = wght/4.0;
    if(in_sect[j]==2) wght = wght/25.0;
    if(in_sect[j]==3) wght = wght/100.0;
    Gmm += wght*dlt*dlt.T();
    bth += wght*RPC_hits[j][0]*dlt;
  }
  
  Gmm.invert(ierr);
  if(ierr) {
    ATH_MSG_ERROR( "Gmm matrix inversion ERROR:"<<ierr<<" Can't invert matrix" );
    return -1;
  }

  tr_par = Gmm*bth;
  
  double tmp_cut(800.0);
  int count(0);

  while(count!=2) {
    bth[0]=0.0;
    bth[1]=0.0;
    Gmm[0][0]=0.0;
    Gmm[0][1]=0.0;
    Gmm[1][0]=0.0;
    Gmm[1][1]=0.0;

    for(unsigned int j=0;j<RPC_hits.size();j++) {
      double res = (tr_par[0] + tr_par[1]*RPC_hits[j][1] - RPC_hits[j][0])/RPC_hits[j][2];
      
      double c_norm = 1.345;
      double addweight;
      if(fabs(res)<c_norm) addweight=1;
      else addweight=c_norm/fabs(res);
      
      CLHEP::HepVector dlt = CLHEP::HepVector(2,0);
      dlt[0] = 1.0;
      dlt[1] = RPC_hits[j][1];
      if(RPC_hits[j][2]<=0) RPC_hits[j][2]=10000.0;
      double wght = 1.0/(RPC_hits[j][2]*RPC_hits[j][2]);
      wght = wght/(double(num_same[j]));
      wght = wght*addweight;
      if(in_sect[j]==1) wght = wght/4.0;
      if(in_sect[j]==2) wght = wght/25.0;
      if(in_sect[j]==3) wght = wght/100.0;
      Gmm += wght*dlt*dlt.T();
      bth += wght*RPC_hits[j][0]*dlt;
    }

    Gmm.invert(ierr);
    if(ierr) {
      ATH_MSG_ERROR( "Gmm matrix inversion ERROR:"<<ierr<<" Can't invert matrix" );
      return -1;
    }
    tr_par = Gmm*bth;

    for(unsigned int j=0;j<RPC_hits.size();j++) {
      double res = (tr_par[0] + tr_par[1]*RPC_hits[j][1] - RPC_hits[j][0])/RPC_hits[j][2];
      if(fabs(res)>3.0*tmp_cut && fabs(res)>max_r) {
	RPC_hits[j][2]=100000.0;
      }
    }
    if(tmp_cut<=max_r) {
      count++;
    }
    tmp_cut = 0.5*tmp_cut;
  }

  if(Gmm[1][1]<0) return -2;
  angle_err = sqrt(Gmm[1][1]);
  
  return 0;
}  //end BarrelSecondCoordinatePreparationTool::rpcFit
