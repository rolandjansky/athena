/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CalibRtDbOperations_h
#define CalibRtDbOperations_h

//c - c++
#include "vector"
#include "string"

namespace coral {
  class IQuery;
  class AttributeList;
}

namespace MuonCalib {

class CalibDbConnection;
class NtupleStationId;
class RtFullInfo;
class SamplePoint;
class IConditionsStorage;


class CalibRtDbOperations {
 public:
//=====================constructor - destructor=================================
  CalibRtDbOperations(CalibDbConnection & db_conn);
  virtual ~CalibRtDbOperations()=default;
//=====================publlic member functions=================================
  //load rt do not read full info, if full_info=NULL
  bool LoadRt(const NtupleStationId & id, int head_id, bool validated, const std::string & /*site_name*/, std::vector<SamplePoint> & points, RtFullInfo * full_info);
  //write/update rt
  bool WriteUpdateRt(const NtupleStationId & id, int head_id, const std::string & /*site_name*/, const std::vector<SamplePoint> & points, int validflag, const RtFullInfo * full_info);
  //set validation flag - last requested rt-id is used as key
  bool SetValidflag(int validflag, bool commit=true);
  //read for cconditions db insert
  bool ReadForConditions(std::string /*site_name*/, int head_id, IConditionsStorage & storage);	
 private:
//=====================private data members====================================
  //db connection
  CalibDbConnection & m_db_conn;
  coral::IQuery * m_query;
  //current mdt-rt-id
  int m_rt_id, m_rt_map_t_id, m_head_id, m_region_id;
  std::string m_prev_algs;
  int m_n_points;
  //red mdt-rt table
  bool getRtId(bool validated,  RtFullInfo * full_info);
  //read r/t/s-table
  inline bool readRTS(std::vector<SamplePoint> &points);
  //update MDT_RT-Table
  inline bool updateFullInfo(const RtFullInfo * full_info, const int & validflag, const int & n_points);
  //update r-t-s-table
  inline bool updateRTSable(const std::vector<SamplePoint> & points);
  //insert into MDT_RT table
  inline bool insertFullInfo(const RtFullInfo * full_info, const int & validflag, const int & n_points);	
  //insert line into r/t/s table
  inline bool insertRTSable(const std::vector<SamplePoint> & points);
  //initialize data buffer for MDT_RT table
  inline void fullInfoInitData( const RtFullInfo * full_info, const int & validflag, coral::AttributeList  &updateData, const int & n_bins, const bool & for_insert);
  //initialize data buffer for MDT_RT_MAP_. Table
  inline void rtTableInitData(coral::AttributeList & rowBuffer);
  //get rt map t id	
  inline bool getRTMapTId();
  //prevent double inserts
  inline bool checkRtPresent(const int & head_id, const std::string & /*site_name*/, const NtupleStationId & id, const int & validflag);
};

} //namespace MuonCalib

#endif
