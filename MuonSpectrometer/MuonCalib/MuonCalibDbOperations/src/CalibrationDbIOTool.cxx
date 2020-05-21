/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MuonCalibDbOperations/CalibrationDbIOTool.h"
#include "MuonCalibDbOperations/CalibDbConnection.h"
#include "MuonCalibDbOperations/CalibT0DbOperations.h"
#include "MuonCalibDbOperations/CalibRtDbOperations.h"
#include "MuonCalibDbOperations/CalibHeadOperations.h"
//gaudi
#include "GaudiKernel/MsgStream.h"

//MdtCalibUtils
#include "MdtCalibUtils/TubeDataFromFile.h"
#include "MdtCalibUtils/RtDataFromFile.h"

//MdtCalibData
#include "MdtCalibData/MdtTubeFitContainer.h"
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/RtChebyshev.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/RtResolutionLookUp.h"
#include "MdtCalibData/RtSpline.h"
#include "MdtCalibData/RtResolutionFromPoints.h"
#include "MdtCalibData/RtFromPoints.h"

//MdtCalibRt
#include "MdtCalibRt/RtCalibrationOutput.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"

//MuonCalibMath
#include "MuonCalibMath/SamplePoint.h"

//c - c++
#include "string"
#include "iostream"
#include "fstream"
#include <sys/types.h>
#include <dirent.h>

#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"


namespace MuonCalib {

CalibrationDbIOTool::CalibrationDbIOTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t, n, p), m_db_ConnectionString(""), m_db_WorkingSchema(""), m_sitename("") {
  declareInterface< CalibrationIOTool >(this) ;
  declareProperty("ConnectionString", m_db_ConnectionString);
  declareProperty("WriterConnectionString", m_writer_connection_string);
  declareProperty("WorkingSchema", m_db_WorkingSchema);
  m_headid=-1;
  declareProperty("HeadId", m_headid);
  declareProperty("SiteName", m_sitename);
  m_max_rt_points=100;
  declareProperty("MaxRtPoints", m_max_rt_points);
  m_use_validated_t0=true;
  declareProperty("UseValidaedT0", m_use_validated_t0);
  m_use_validated_t0=false;
  declareProperty("UseValidaedRt", m_use_validated_rt);
  for (int i=0; i<2;i++) {
    m_data_connections[i]=NULL;
  }
  declareProperty("ReaderAccount", m_reader_account);
  declareProperty("ReaderPassword", m_reader_password);
  declareProperty("WriterAccount", m_writer_account);
  declareProperty("WriterPassword", m_writer_password);
}

StatusCode CalibrationDbIOTool::initialize() {
  MsgStream log(msgSvc(), name());

  StatusCode sc= serviceLocator()->service("RegionSelectionSvc", p_reg_sel_svc);
  if(!sc.isSuccess()) {
    log << MSG::ERROR <<"Cannot retrieve RegionSelectionSvc!" <<endmsg;
    return sc;
  }
  m_region_ids=p_reg_sel_svc->GetStationsInRegions();
  log<< MSG::INFO << " CalibrationDbIOTool::initialize() - number of selected regions: "<<m_region_ids.size()<<endmsg;
	    
  log<< MSG::INFO << "open connection" <<endmsg;
  m_connection = new CalibDbConnection(m_db_ConnectionString, m_db_WorkingSchema);
  log<< MSG::INFO << "." <<endmsg;
  if(m_reader_account!="") {
    m_connection->SetLogin(m_reader_account, m_reader_password);
  }
  if (!m_connection->OpenConnection()) {
    log<< MSG::FATAL << "Cannot open connection to database!" <<endmsg;
    return StatusCode::FAILURE;
  }
  log<< MSG::INFO << ".." <<endmsg;
  m_head_ops = new CalibHeadOperations(*m_connection);
  if (m_headid<0) {
    m_headid=m_head_ops->GetLatestHeadId();
    if (m_headid<0) {
      log<< MSG::FATAL << "Cannot get latest head_id for site "<< m_sitename<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CalibrationDbIOTool::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO <<"CalibrationDbIOTool finalize"<<endmsg;

  try {
    if(m_head_ops!=NULL) delete m_head_ops;
    if(m_connection != NULL) delete m_connection;

    for(int i=0; i<2; i++) {
      if (m_data_connections[i]!=NULL) delete m_data_connections[i];
    }
  } catch(std::exception & e) {
    log << MSG::FATAL << "Exception in finalize: " << e.what() <<endmsg;
    return StatusCode::FAILURE;
  }	
  
  return StatusCode::SUCCESS;
}

StatusCode CalibrationDbIOTool::WriteT0(MdtTubeFitContainer* t0, const NtupleStationId & it, int /*iov_start*/, int /*iov_end*/) {
  MsgStream log(msgSvc(), name());
  log<<MSG::INFO<<"Writing to database"<<endmsg;
  if(!m_connection->OpenConnection()) {
    log << MSG::FATAL <<"Open Connection failed!"<<endmsg;
    return StatusCode::FAILURE;
  } else {
    if(get_connection(1)==NULL) {
      return StatusCode::FAILURE;
    }
    m_data_connections[1]->OpenConnection();
    CalibT0DbOperations t0_op(*m_data_connections[1]);
    std::vector<int> val(0);
    if(!t0_op.WriteT0Chamber(it, t0, val, m_headid, m_sitename)) {
      log << MSG::FATAL <<"Writing t0 failed!"<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}
    
StatusCode  CalibrationDbIOTool::LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &t0s, int /*iov_id*/) {
  MsgStream log(msgSvc(), name()); 
  if(!m_connection->OpenConnection()) {
    log << MSG::FATAL <<"Open Connection failed!"<<endmsg;
    return StatusCode::FAILURE;
  } else {
    if(!get_connection(0)) {
      return StatusCode::FAILURE;
    }
    m_data_connections[0]->OpenConnection();
    CalibT0DbOperations t0_op(*m_data_connections[0]);
    t0s.clear();
 
    //loop on all the ids in the selected calibration region
    for (std::vector<MuonCalib::NtupleStationId>::iterator it=m_region_ids.begin(); it!=m_region_ids.end(); it++) {
      log << MSG::INFO << "Reading t0s for region " << it->regionId() << endmsg;
      MdtStationT0Container * t0;
      t0=t0_op.LoadT0Calibration(*it, m_headid, m_sitename);
      if(t0!=NULL)
	t0s[*it]=t0;
    }
  }
  return StatusCode::SUCCESS;   
} 

StatusCode CalibrationDbIOTool::WriteRt(const RtCalibrationOutput *rt_relation, const IRtResolution * resolution, const NtupleStationId & station_id, int /*iov_start*/, int /*iov_end*/, bool /*real_rt*/, bool /*real resolution*/) {
  MsgStream log(msgSvc(), name()); 
  std::vector<SamplePoint> points;
//fill rt poits
  fillRtPoints(rt_relation->rt(), points);
  //fill resolution points
  fillResPoints(resolution, points);
//fill multilayer rt difference as pseudo-point
  if(rt_relation->rt()->HasTmaxDiff()) {
    SamplePoint point(rt_relation->rt()->GetTmaxDiff(), -99.9, 0.0);
    points.push_back(point);
  }
  if(!m_connection->OpenConnection()) {
    log << MSG::FATAL <<"Open Connection failed!"<<endmsg;
    return StatusCode::FAILURE;
  } else {
    if(get_connection(1)==NULL) {
      return StatusCode::FAILURE;
    }
    m_data_connections[1]->OpenConnection();
    CalibRtDbOperations rt_op(*m_data_connections[1]);
    if(!rt_op.WriteUpdateRt(station_id, m_headid, m_sitename, points, 0, rt_relation->fullInfo())) {
      log << MSG::FATAL <<"Writing rt failed!"<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CalibrationDbIOTool::LoadRt(std::map<NtupleStationId, IRtRelation *> & rts, std::map<NtupleStationId, IRtResolution *> &res, int /*iov_id*/) {
  MsgStream log(msgSvc(), name()); 
  if(!m_connection->OpenConnection()) {
    log << MSG::FATAL <<"Open Connection failed!"<<endmsg;
    return StatusCode::FAILURE;
  } else {
    if(!get_connection(0)) {
      return StatusCode::FAILURE;
    }
    m_data_connections[0]->OpenConnection();
    CalibRtDbOperations rt_op(*m_data_connections[0]);
    //loop on all the ids in the selected calibration region
    RtFromPoints rt_from_points;
    RtResolutionFromPoints res_from_points;
    for (std::vector<MuonCalib::NtupleStationId>::iterator it=m_region_ids.begin(); it!=m_region_ids.end(); it++) {
      std::vector<SamplePoint> in_points;
      RtFullInfo full_info;
      if(!rt_op.LoadRt(*it, m_headid, m_use_validated_rt, m_sitename, in_points, &full_info))
	continue;
      log << MSG::INFO << "Reading rts for region " << it->regionId() << endmsg;
      std::vector<SamplePoint> outpoints;
      float tmax_diff(-9e9);
      for (unsigned int k=0; k<in_points.size(); k++) {
	if (in_points[k].x2()<-99) {
	  tmax_diff=in_points[k].x1();
	  continue;
	}
	SamplePoint outpoint(in_points[k].x1(), in_points[k].x2(), 1.0);
	outpoints.push_back(outpoint);
      }
      rts[*it]= new RtRelationLookUp(rt_from_points.getRtRelationLookUp(outpoints));
      if(tmax_diff>-8e8) {
	rts[*it]->SetTmaxDiff(tmax_diff);
      }
      outpoints.clear();
      for (unsigned int k=0; k<in_points.size(); k++) {
	if (in_points[k].x2()<-99) {
	  continue;
	}		
	SamplePoint outpoint(in_points[k].x1(), in_points[k].error(), 1.0);
	outpoints.push_back(outpoint);
      }
      res[*it] = new RtResolutionLookUp(res_from_points.getRtResolutionLookUp(outpoints));
    }
  }		
  return StatusCode::SUCCESS;   
}

inline void CalibrationDbIOTool::fillRtPoints( const IRtRelation*  rt, std::vector<SamplePoint> & points) {
  MsgStream log(msgSvc(), name()); 
  points.clear();
//for rt relations based on support points, write points
  const RtRelationLookUp *rt_lookup=dynamic_cast<const RtRelationLookUp *>(rt);
  const CalibFunc::ParVec & rt_param = rt->parameters();
  if(rt_lookup && static_cast<int>(rt_lookup->nPar()-2)<=m_max_rt_points) {
    double t_min(rt_param[0]);
    double bin_size = rt_param[1];
    unsigned int nb_points(rt_lookup->nPar()-2);
    for (unsigned int k=0; k<nb_points; k++) {
      double radius(rt_param[k+2]);
      if(std::isnan(radius)) {
	log<<MSG::WARNING<<"Skipping NAN"<<endmsg;
	continue;
      }	
      SamplePoint point(t_min+bin_size*k, rt_param[k+2], -1);
      points.push_back(point);
    }
    return;
  }
  const RtSpline *rt_spline=dynamic_cast<const RtSpline *>(rt);
  if(rt_spline && static_cast<int>(rt_param.size()/2)<=m_max_rt_points)	{
    for(unsigned int i=0; i<rt_param.size(); i+=2) {
      SamplePoint point(rt_param[i], rt_param[i+1], -1);
      points.push_back(point);
    }
    return;
  }
//for all others, or if the number of support points is too high, calculate new support points
  for(int i=0; i<m_max_rt_points; i++) {
    double t=rt->tLower() + i*((rt->tUpper() - rt->tLower())/(m_max_rt_points - 1));
    SamplePoint point(t, rt->radius(t), -1.0);
  }
}

inline void CalibrationDbIOTool::fillResPoints(const IRtResolution *rt_resolution, std::vector<SamplePoint> &  points) {
  for(unsigned int i=0; i<points.size(); i++) {
    points[i].set_error(rt_resolution->resolution(points[i].x1()));
  }
}

CalibDbConnection * CalibrationDbIOTool::get_connection(int write) {
  if(m_data_connections[write]!=0) {
    return m_data_connections[write];
  }
  m_data_connections[write] = m_head_ops->GetDataConnection(m_headid, static_cast<bool>(write), m_writer_connection_string, m_writer_account, m_writer_password);
  return m_data_connections[write];
}

} ///namespace MuonCalib
