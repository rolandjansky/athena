/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibDbOperations/CalibRtDbOperations.h"
#include "MuonCalibDbOperations/CalibDbConnection.h"
#include "MuonCalibDbOperations/IConditionsStorage.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"
#include "MdtCalibData/RtFullInfo.h"
#include "MuonCalibMath/SamplePoint.h"
#include "RelationalAccess/IRelationalService.h"
#include "RelationalAccess/IRelationalDomain.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITableDataEditor.h"
#include "RelationalAccess/IAuthenticationService.h"
#include "RelationalAccess/IAuthenticationCredentials.h"
#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/IBulkOperation.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralKernel/Context.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include "sstream"
#include "string"
#include "cmath"

namespace MuonCalib {

CalibRtDbOperations::CalibRtDbOperations(CalibDbConnection & db_conn) : m_db_conn(db_conn), m_rt_id(-1), m_rt_map_t_id(-1), m_head_id(-1), m_region_id(-1), m_n_points(0) {
  m_query=nullptr;
}
	
bool CalibRtDbOperations::LoadRt(const NtupleStationId & id, int head_id, bool validated, const std::string & /*site_name*/, std::vector<SamplePoint> & points, RtFullInfo * full_info) {
  m_head_id=head_id;
  m_region_id=id.FixedId();
  try {
    points.clear();
    m_db_conn.OpenTransaction();
    m_rt_id=-1;
    if(!getRtId(validated, full_info)) {
      MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
      log<<MSG::WARNING<<"Header query failed!"<<endmsg;
      m_db_conn.Rollback();
      return false;
    }
    if (m_rt_id<0) {
      m_db_conn.Rollback();
      return false;
    }
    if(!readRTS(points)) {
      m_db_conn.Rollback();
      return false;
    }
    return true;
  }
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    return false;
  }
}

bool CalibRtDbOperations::WriteUpdateRt(const NtupleStationId & id, int head_id, const std::string & /*site_name*/, const std::vector<SamplePoint> & points, int validflag, const RtFullInfo * full_info) {
  m_prev_algs="";
  m_head_id=head_id;
  m_region_id=id.FixedId();
  try {
    m_db_conn.OpenTransaction();
    if(!getRtId((validflag>1), nullptr)) {
      m_db_conn.Rollback();
      MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
      log<<MSG::WARNING<<"Questing RT header failed!"<<endmsg;
      return false;
    }
    if(m_rt_id>=0) {
      if(!updateFullInfo(full_info, validflag, static_cast<int>(points.size()))) {
        MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
        log<<MSG::WARNING<<"Failed update header"<<endmsg;
        m_db_conn.Rollback();
        return false;
      }
      if(!updateRTSable(points)) {
	m_db_conn.Rollback();
	return false;
      }
			
    } else {
      if(!insertFullInfo(full_info, validflag, static_cast<int>(points.size()))) {
        MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
        log<<MSG::WARNING<<"Failed update header"<<endmsg;
        m_db_conn.Rollback();
        return false;				
      }
      if(!insertRTSable(points)) {
	m_db_conn.Rollback();
	return false;				
      }
				
    }
    m_db_conn.Commit();
    return true;
  }
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    m_db_conn.Rollback();
    return false;
  }
  
}

bool CalibRtDbOperations::SetValidflag(int validflag, bool commit) {
  if(m_rt_id<0) {
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<"No current rt!"<<endmsg;
    return false;
  }
  try {
    m_db_conn.OpenTransaction();
    coral::ITableDataEditor & editor=m_db_conn.GetTableEditor("MDT_RT");
    std::string updateAction = "VALIDFLAG=:val";
    std::string updateCondition = "HEAD_ID=:hid and REGION_ID=:rid and CALIBFLAG=:rtid";
    coral::AttributeList updateData;
    updateData.extend<int>("val");
    updateData[0].data<int>()=validflag;
    updateData.extend<int>("rtid");
    updateData["rtid"].data<int>()=m_rt_id;
    updateData.extend<int>("hid");
    updateData["hid"].data<int>()=m_head_id;
    updateData.extend<int>("rid");
    updateData["rid"].data<int>()=m_region_id;
		
    int n_rows=editor.updateRows( updateAction, updateCondition, updateData );
    if(n_rows!=1)	{
      MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
      log<<MSG::WARNING<<"Invalid number of rows updated in MDT_RT! "<<n_rows<<" instead of 1"<<endmsg;
      m_db_conn.Rollback();
      return false;
    }
    if(commit)
      m_db_conn.Commit();
    return true;
  }
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    m_db_conn.Rollback();
    return false;
  }
}	

bool  CalibRtDbOperations::ReadForConditions(std::string /*site_name*/, int head_id, IConditionsStorage & storage) {
  try {
    m_db_conn.OpenTransaction();
    m_query=m_db_conn.GetQuery();
    m_query->addToTableList("MDT_RT");
    m_query->addToTableList("MDT_RT_MAP");
    coral::AttributeList conditionData;
    conditionData.extend<int>("hid");
    conditionData[0].data<int>()=head_id;
    std::string condition = "MDT_RT.HEAD_ID=:hid and MDT_RT_MAP.HEAD_ID=:hid and MDT_RT.REGION_ID = MDT_RT_MAP.REGION_ID and MDT_RT.CALIBFLAG = MDT_RT_MAP.CALIBFLAG and MDT_RT.VALIDFLAG>1";
    m_query->setCondition( condition, conditionData );
    m_query->addToOrderList( "MDT_RT.REGION_ID" );
    m_query->addToOutputList("MDT_RT.REGION_ID", "REGION_ID");
    m_query->addToOutputList("MDT_RT_MAP.POINT_NR", "POINT_NR");
    m_query->addToOutputList("MDT_RT_MAP.T", "T");
    m_query->addToOutputList("MDT_RT_MAP.R", "R");
    m_query->addToOutputList("MDT_RT_MAP.S", "S");
    m_query->setRowCacheSize(100000);
    //execute query
    int last_region_id(-1);
    std::map<int, SamplePoint> points;
    coral::ICursor& cursor = m_query->execute();
    while (cursor.next()) {
      const coral::AttributeList & al= cursor.currentRow();
      int rt_id=static_cast<int>(al["REGION_ID"].data<float>());
      if(last_region_id>0 && rt_id!=last_region_id) {
	if(!storage.StoreRtChamber(last_region_id, points)) {
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<"Storage_failed"<<endmsg;
	  return false;
	}
	points.clear();
      }
      last_region_id=rt_id;
      SamplePoint pt(al["T"].data<float>(), al["R"].data<float>(), al["S"].data<float>());
      points[static_cast<int>(al["POINT_NR"].data<short>())] = pt;
    }
    if(last_region_id>0) {
      if(!storage.StoreRtChamber(last_region_id, points)) {
        MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
        log<<MSG::WARNING<<"Storage_failed"<<endmsg;
        return false;
      }
    }		
    return true;
  }
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    m_db_conn.Rollback();
    return false;
  }	
}

bool CalibRtDbOperations::getRtId(bool validated,  RtFullInfo * full_info) {
//select MDT_RT, ... from MDT_RT where head_id=:hid and site_name=:sn and region_id=:rid and [and validflag>0| and validflag=0]
  m_query=m_db_conn.GetQuery();
  m_query->addToTableList("MDT_RT");
  coral::AttributeList conditionData;
  conditionData.extend<int>("hid");
  conditionData[0].data<int>()=m_head_id;
  conditionData.extend<int>("rid");
  conditionData[1].data<int>()=m_region_id;
  m_prev_algs = "";
  std::string condition = "HEAD_ID=:hid and REGION_ID=:rid";
  if (validated) {
    condition += " and VALIDFLAG>1";
  } else {
    condition += " and VALIDFLAG=0";
  }
  m_query->setCondition( condition, conditionData );	
  m_query->addToOutputList("CALIBFLAG");
  m_query->addToOutputList("BINS");
  m_query->addToOutputList("ALGO_FLAG");
  if(full_info!=NULL) {
    m_query->addToOutputList("N_SEGS");
    m_query->addToOutputList("AVER_ANGLE");
    m_query->addToOutputList("DELTA_AVER_ANGLE");
    m_query->addToOutputList("AVER_SPREAD");
    m_query->addToOutputList("DELTA_AVER_SPREAD");
    m_query->addToOutputList("CONVERGENCE");
    m_query->addToOutputList("B_AVER");
    m_query->addToOutputList("T_AVER");
  }
  coral::ICursor& cursor = m_query->execute();
  int n_rows=0;
  m_rt_id=-9999;
  while (cursor.next())	{
    n_rows++;
    const coral::AttributeList & al= cursor.currentRow();
    m_rt_id = al["CALIBFLAG"].data<short>();
    m_n_points = static_cast<int>(al["BINS"].data<short>());
    m_prev_algs = al["ALGO_FLAG"].data<std::string>();
    if(full_info!=NULL)	{
      full_info->setNumSeg(static_cast<int>(al["N_SEGS"].data<int>()));
      full_info->setMeanAng(al["AVER_ANGLE"] .data<float>());
      full_info->setRmsAng(al["DELTA_AVER_ANGLE"]. data<float>());
      full_info->setMeanPos(al["AVER_SPREAD"]. data<float>());
      full_info->setRmsPos(al["DELTA_AVER_SPREAD"]. data<float>());
      full_info->setConvEst(al["CONVERGENCE"].data<float>());
      full_info->setImplementation(m_prev_algs);
    }
  }
  if(n_rows>1) {
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<"Found "<<n_rows<<" in MDT_RT instead of 0 or 1"<<endmsg;
    return false;
  }
  m_db_conn.DestroyQuery(m_query);
  return true;
}

inline bool CalibRtDbOperations ::readRTS(std::vector<SamplePoint> &points) {
  m_query=m_db_conn.GetQuery();
  m_query->addToTableList("MDT_RT_MAP");
  coral::AttributeList conditionData;	
  conditionData.extend<int>("rtid");
  conditionData[0].data<int>()=m_rt_id;
  conditionData.extend<int>("hid");
  conditionData[1].data<int>()=m_head_id;
  conditionData.extend<int>("rid");
  conditionData[2].data<int>()=m_region_id;
  std::string condition = "HEAD_ID=:hid and REGION_ID=:rid and CALIBFLAG=:rtid";
  m_query->setCondition( condition, conditionData );
  m_query->addToOutputList("MDT_RT_MAP.POINT_NR", "POINT_NR");
  m_query->addToOutputList("MDT_RT_MAP.R", "R");
  m_query->addToOutputList("MDT_RT_MAP.T", "T");
  m_query->addToOutputList("MDT_RT_MAP.S", "S");
  m_query->addToOrderList("MDT_RT_MAP.POINT_NR");
  m_query->setRowCacheSize(100);
  coral::ICursor& cursor = m_query->execute();
  points.clear();
  points.resize(m_n_points);
  int count(0);
  while (cursor.next())	{
    const coral::AttributeList & al= cursor.currentRow();
    int pt_nr(static_cast<int>(al["POINT_NR"].data<short>()));
    if(pt_nr>=m_n_points) {
      MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
      log<<MSG::WARNING<<"Illegal point number "<<pt_nr<<endmsg;
      return false;
    }
    SamplePoint point(al["T"].data<float>(), al["R"].data<float>(), al["S"].data<float>());
    points[pt_nr]=point;
    count++;
  }
  if (count!=m_n_points) {
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<m_n_points<<" points expected, "<<count<<" found!"<<endmsg;
    return false;
  }
  m_db_conn.DestroyQuery(m_query);	
  return true;
}

inline bool CalibRtDbOperations ::updateFullInfo(const RtFullInfo * full_info, const int & validflag, const int & n_points) {
  coral::ITableDataEditor & editor=m_db_conn.GetTableEditor("MDT_RT");
  std::string updateAction = "N_SEGS = :N_SEGS, AVER_ANGLE = :AVER_ANGLE, DELTA_AVER_ANGLE = :DELTA_AVER_ANGLE, AVER_SPREAD = :AVER_SPREAD, DELTA_AVER_SPREAD = :DELTA_AVER_SPREAD, CONVERGENCE = :CONVERGENCE, VALIDFLAG=:VALIDFLAG, BINS=:BINS, ALGO_FLAG=:ALGO_FLAG";
  std::string updateCondition = "HEAD_ID=:hid and REGION_ID=:rid and CALIBFLAG=:rtid";
  coral::AttributeList updateData;
  fullInfoInitData(full_info, validflag, updateData, n_points, false);
  updateData.extend<int>("rtid");
  updateData["rtid"].data<int>()=m_rt_id;
  updateData.extend<int>("hid");
  updateData["hid"].data<int>()=m_head_id;
  updateData.extend<int>("rid");
  updateData["rid"].data<int>()=m_region_id;
  if(editor.updateRows( updateAction, updateCondition, updateData )!=1)	{
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<"Invalid number of rows updated in MDT_RT!"<<endmsg;
    return false;
  }
  return true;
}

inline bool CalibRtDbOperations ::updateRTSable(const std::vector<SamplePoint> & points) {
  coral::ITableDataEditor & editor=m_db_conn.GetTableEditor("MDT_RT_MAP");
//check if there are now less points than before
  if(static_cast<int>(points.size())<m_n_points) {
    std::string deleteCondition = "HEAD_ID = :hid and REGION_ID=:rid and CALIBFLAG=:rtid and POINT_NR>=:max_point";
    coral::AttributeList deleteData;
    deleteData.extend<int>( "hid" );
    deleteData[0].data<int>() = m_head_id;
    deleteData.extend<int>( "rid" );
    deleteData[1].data<int>() = m_region_id;
    deleteData.extend<int>( "rtid" );
    deleteData[2].data<int>() = m_rt_id;
    deleteData.extend<int>( "max_point" );
    deleteData[3].data<int>() = static_cast<int>(points.size());
    long rowsDeleted = editor.deleteRows( deleteCondition, deleteData );
    if(rowsDeleted != m_n_points - static_cast<int>(points.size())) {
      MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
      log<<MSG::WARNING<<rowsDeleted<<" Points removed instead of "<< m_n_points - static_cast<int>(points.size())<<"! Rolling back!"<<endmsg;
      return false;
    }
  }
  coral::AttributeList table_data;
  rtTableInitData(table_data);
  table_data["CALIBFLAG"].data<int>()=m_rt_id;
  table_data["HEAD_ID"].data<int>()=m_head_id;
  table_data["REGION_ID"].data<int>()=m_region_id;
  for(int i=0; i<static_cast<int>(points.size()); i++) {
    table_data["POINT_NR"].data<int>() = i;
    table_data["T"].data<float>() = points[i].x1();		
    table_data["R"].data<float>() = points[i].x2();
    table_data["S"].data<float>() = points[i].error();
    if(i<m_n_points) {
      std::string updateAction = "R=:R, T=:T, S=:S";
      std::string updateCondition = "CALIBFLAG=:CALIBFLAG and POINT_NR=:POINT_NR and HEAD_ID=:HEAD_ID and REGION_ID=:REGION_ID";
      if(editor.updateRows( updateAction, updateCondition,  table_data)!=1) {
        MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
        log<<MSG::WARNING<<"Wrong number of points updated!"<<endmsg;
        return false;
      }
    } else {
      editor.insertRow(table_data);
    }
  }
  return true;
}

inline bool CalibRtDbOperations ::insertFullInfo(const RtFullInfo * full_info, const int & validflag, const int & n_points) {
  coral::ITableDataEditor & editor=m_db_conn.GetTableEditor("MDT_RT");
  coral::AttributeList rowBuffer;
  fullInfoInitData(full_info, validflag, rowBuffer, n_points,true);
  editor.insertRow( rowBuffer );
  return getRtId((validflag>1), NULL);
}

inline bool  CalibRtDbOperations ::insertRTSable(const std::vector<SamplePoint> & points) {
  coral::ITableDataEditor & editor=m_db_conn.GetTableEditor("MDT_RT_MAP");
  coral::AttributeList table_data;
  rtTableInitData(table_data);
  table_data["CALIBFLAG"].data<int>()=m_rt_id;
  table_data["HEAD_ID"].data<int>()=m_head_id;
  table_data["REGION_ID"].data<int>()=m_region_id;
  coral::IBulkOperation *bulk_inserter=editor.bulkInsert( table_data, 100);
//use trigger to fill in MDT_RT_ID
  for(int i=0; i<static_cast<int>(points.size()); i++) {
    table_data["POINT_NR"].data<int>()=i;
    table_data["T"].data<float>()=points[i].x1();
    table_data["R"].data<float>()=points[i].x2();
    table_data["S"].data<float>()=points[i].error();
    bulk_inserter->processNextIteration();
  }
  bulk_inserter->flush();
  delete bulk_inserter;
  return true;
}

inline void CalibRtDbOperations::fullInfoInitData( const RtFullInfo * full_info, const int & validflag, coral::AttributeList  &updateData, const int & n_bins, const bool & for_insert) {
  if(for_insert) {
    updateData.extend<int>("REGION_ID");
    updateData["REGION_ID"].data<int>()=m_region_id;
    updateData.extend<int>("HEAD_ID");
    updateData["HEAD_ID"].data<int>()=m_head_id;
    updateData.extend<int>("LOWRUN");
    updateData["LOWRUN"].data<int>()=0;
    updateData.extend<int>("UPRUN");
    updateData["UPRUN"].data<int>()=1;
    updateData.extend<std::string>("HISTOGRAM");
    updateData["HISTOGRAM"].data<std::string>()="DUMMY";
  }
  if(m_prev_algs!="") {
    m_prev_algs+=":";
  }
  if(full_info->implementation()=="RtCalibrationIntegration") {
    m_prev_algs+="Int";
  } else if (full_info->implementation()=="RtCalibrationCruved") {
    m_prev_algs+="Cur";
  } else if (full_info->implementation()=="RtCalibrationAnalytic") {
    m_prev_algs+="Ana";
  } else if (full_info->implementation()=="RtCalibrationAnalyticExt") {
    m_prev_algs+="AnaExt";
  } else if (full_info->implementation()=="Validation")	{
    m_prev_algs+="Val";
  } else {
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<"Unknown implementation '"<<full_info->implementation()<<"'!"<<endmsg;
    m_prev_algs+="UU";
  }
  if(m_prev_algs.size()>25) {
    MsgStream log(Athena::getMessageSvc(),"CalibRtDbOperations");
    log<<MSG::WARNING<<"m_prev_algs exceeds size: "<<m_prev_algs<<endmsg;
    m_prev_algs="..";
  }
  updateData.extend<std::string>("ALGO_FLAG");
  updateData["ALGO_FLAG"].data<std::string>()=m_prev_algs;
  updateData.extend<int>("N_SEGS");
  updateData["N_SEGS"].data<int>()=full_info->numSeg();
  updateData.extend<float>("AVER_ANGLE");
  updateData["AVER_ANGLE"].data<float>()=full_info->meanAng();
  if(std::abs(full_info->meanAng()) > 9.9999) {
    updateData["AVER_ANGLE"].data<float>() = ((full_info->meanAng() < 0)? -1 : 1) * 9.9999;
  }
  updateData.extend<float>("DELTA_AVER_ANGLE");
  updateData["DELTA_AVER_ANGLE"].data<float>()=full_info->rmsAng();
  if(std::abs(full_info->rmsAng()) > 9.9999) {
    updateData["DELTA_AVER_ANGLE"].data<float>() = ((full_info->rmsAng() < 0)? -1 : 1) * 9.9999;
  }
  updateData.extend<float>("AVER_SPREAD");
  updateData["AVER_SPREAD"].data<float>()=full_info->meanPos();
  if(std::abs(full_info->meanPos()) > 9.9999) {
    updateData["AVER_SPREAD"].data<float>() = ((full_info->meanPos() < 0)? -1 : 1) * 9.9999;
  }
  updateData.extend<float>("DELTA_AVER_SPREAD");
  updateData["DELTA_AVER_SPREAD"].data<float>()=full_info->rmsPos();
  if(std::abs(full_info->rmsPos()) > 9.9999) {
    updateData["DELTA_AVER_SPREAD"].data<float>() = ((full_info->rmsPos() < 0)? -1 : 1) * 9.9999;
  }
  updateData.extend<float>("CONVERGENCE");
  updateData["CONVERGENCE"].data<float>()=full_info->convEst();
  if(std::abs(full_info->convEst()) > 999.999) {
    updateData["CONVERGENCE"].data<float>() = ((full_info->convEst() < 0)? -1 : 1) * 999.999;
  }
  if(for_insert) {
    updateData.extend<float>("B_AVER");
    updateData["B_AVER"].data<float>()=0.;
    updateData.extend<float>("T_AVER");
    updateData["T_AVER"].data<float>()=0.;
    updateData.extend<float>("HV");
    updateData["HV"].data<float>()=0.;
    updateData.extend<float>("CURR");
    updateData["CURR"].data<float>()=0.;
    updateData.extend<float>("GAS");
    updateData["GAS"].data<float>()=0.;
    updateData.extend<int>("CALIBFLAG");
    updateData["CALIBFLAG"].data<int>() = static_cast<int>(validflag>3);
  }
  updateData.extend<int>("VALIDFLAG");
  updateData["VALIDFLAG"].data<int>()=validflag;
  updateData.extend<short>("BINS");
  updateData["BINS"].data<short>()=n_bins;
}

inline void CalibRtDbOperations::rtTableInitData(coral::AttributeList & rowBuffer) {
  rowBuffer.extend<int>("HEAD_ID");
  rowBuffer.extend<int>("REGION_ID");
  rowBuffer.extend<int>("CALIBFLAG");
  rowBuffer.extend<int>("POINT_NR");
  rowBuffer.extend<float>("T");
  rowBuffer.extend<float>("R");
  rowBuffer.extend<float>("S");
}

inline bool CalibRtDbOperations::checkRtPresent(const int & head_id, const std::string & /*site_name*/, const NtupleStationId & id, const int & validflag) {
  m_query=m_db_conn.GetQuery();
//select count(MDT_RT.MDT_RT_ID) as  N_TUBES, MDT_RT.VALIDFLAG as VALIDFLAG from MDT_RT where MDT_RT.HEAD_ID = :hid and MDT_RT.REGION_ID = :chamb and SITE_NAME=:sn group by MDT_RT.VALIDFLAG;
  m_query->addToTableList("MDT_RT");	
  coral::AttributeList conditionData;
  conditionData.extend<int>("hid");
  conditionData[0].data<int>()=head_id;
//as long as the chamber id is stored as a string, it is more efficient to pass the chamber id as a string in the query
  std::ostringstream ostr;
  ostr<<id.FixedId();
  conditionData.extend<std::string>("chamb");
  conditionData[1].data<std::string>()=ostr.str();
  std::string condition = "MDT_RT.HEAD_ID = :hid and MDT_RT.REGION_ID = :chamb";
  m_query->setCondition( condition, conditionData );
  m_query->addToOutputList( "count(MDT_RT.CALIBFLAG)", "N_RTS");
  m_query->addToOutputList( "MDT_RT.VALIDFLAG", "VALIDFLAG" );
  m_query->groupBy("MDT_RT.VALIDFLAG");
  coral::ICursor& cursor = m_query->execute();
  bool has_unvalidated(false), has_validated(false);
  while (cursor.next())	{
    const coral::AttributeList & al= cursor.currentRow();
    if(al["VALIDFLAG"].data<short>()>0) {
      if(al["N_RTS"].data<double>()>0) {
	has_validated=true;
      }
    } else {
      if(al["N_RTS"].data<double>()>0) {
	has_unvalidated=true;
      }
    }
  }
  if (validflag==0 && has_unvalidated) return false;
  if (validflag>0 && has_validated) return false;
  return true;
}

}//namespace MuonCalib
