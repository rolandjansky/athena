/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "MuonCalibDbOperations/CoolInserter.h"
#include "MuonCalibDbOperations/CalibDbConnection.h"
#include "MuonCalibDbOperations/CalibT0DbOperations.h"
#include "MuonCalibDbOperations/CalibRtDbOperations.h"
#include "MuonCalibDbOperations/CalibHeadOperations.h"
//coral
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoolKernel/ConstRecordAdapter.h"
#include "CoolKernel/FolderSpecification.h"

#include "AthenaKernel/IOVTime.h"

//MuonCalibIdentifier
#include "MuonCalibIdentifier/MuonFixedId.h"

//MuonCalibMath
#include "MuonCalibMath/SamplePoint.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

//c - c++
#include "iostream"
#include "sstream"
#include "exception"
#include <typeinfo>
#include "zlib.h"

namespace MuonCalib {

CoolInserter::CoolInserter(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator),  m_version("v0.0"), p_reg_sel_svc(NULL), m_cool_connect(false), m_t0_created(false), m_rt_created(false) {
  declareProperty("CoolConnectionString", m_cool_connection_string);
  declareProperty("RtVersion", m_version);
  declareProperty("T0Tag", m_tagt0);
  declareProperty("RtTag", m_tagrt);
  declareProperty("IOVStart", m_iov_start);
  declareProperty("IOVEnd", m_iov_end);
  declareProperty("T0Folder", m_t0_folder);
  declareProperty("RtFolder", m_rt_folder);
  declareProperty("T0FolderCompress", m_compressed_t0);
  declareProperty("RtFolderCompress", m_compressed_rt);
  m_n_tubes_chamber=0;
  m_n_tubes_added=0;
  m_iovt_start=0;
  m_iovt_end=0;
  m_aver_t0=0;
  m_aver_adc=0;
}

StatusCode CoolInserter::initialize() {
//check jo
  if(m_tagt0.size() != m_t0_folder.size() || m_tagrt.size() != 	m_rt_folder.size()) {
    ATH_MSG_FATAL("Configuration error: Number of folders and tags do not match!");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_idHelperSvc.retrieve());
  
  ATH_CHECK(m_DetectorManagerKey.initialize());
  
  //get region selection service
  ATH_CHECK(service("RegionSelectionSvc", p_reg_sel_svc));

  //connect to cool database
  try {
    m_db = m_app.databaseService().openDatabase(m_cool_connection_string ,false );
  }
  catch ( cool::DatabaseDoesNotExist  & e ) {
    ATH_MSG_INFO("Creating new database.");
    try {
      m_db = m_app.databaseService().createDatabase(m_cool_connection_string);
    }
    catch(cool::Exception & e) {
      ATH_MSG_FATAL("Cannot create database and datbasae does not exist!");
      return StatusCode::FAILURE;
    }
  }
  m_cool_connect=true;
  if(m_t0_folder.size()) {
    ATH_MSG_INFO("Replication into t0 folders: ");
    for(unsigned int i=0; i<m_t0_folder.size(); i++) {
      ATH_MSG_INFO(m_t0_folder[i] << "(" << m_tagt0[i] << ") ");
    }
  }
  if(m_rt_folder.size()) {
    ATH_MSG_INFO("Replication into rt folders: ");
    for(unsigned int i=0; i<m_rt_folder.size(); i++) {
      ATH_MSG_INFO(m_rt_folder[i] << "(" << m_tagrt[i] << ") ");
    }
  }
  //retrieve calibration sources
  ATH_CHECK(m_calibration_sources.retrieve());

   //get iov
  for(unsigned int i=0; i<m_calibration_sources.size(); i++) {
    int iov_start(-1), iov_end(-1);
    m_calibration_sources[i]->SetIOV(iov_start, iov_end);
    if(m_iov_start<0 && iov_start>=0)
      m_iov_start = iov_start;
    if(m_iov_end==-1 && iov_end!=-1)
      m_iov_end = iov_end;
  }
  if(m_iov_start<0 || m_iov_end==-1) {
    ATH_MSG_FATAL("IOV has to be set, if no calibration source provides it!");
    return StatusCode::FAILURE;
  }
  IOVTime start(m_iov_start, 0);
  m_iovt_start = start.re_time();
  if (m_iov_end<-1) {
    IOVTime end(IOVTime::MAXTIMESTAMP);
    m_iovt_end = end.timestamp();
  } else {
    IOVTime end(m_iov_end, 0);
    m_iovt_end = end.re_time();
  }
  ATH_MSG_INFO("IOV is "<<m_iov_start<<" to "<<m_iov_end);
  //fill compressed flags
  if(m_compressed_t0.size() < m_t0_folder.size())	
    for(unsigned int i=m_compressed_t0.size(); i<m_t0_folder.size(); i++)
      m_compressed_t0.push_back(false);
  if(m_compressed_rt.size() < m_rt_folder.size())	
    for(unsigned int i=m_compressed_rt.size(); i<m_rt_folder.size(); i++)
      m_compressed_rt.push_back(false);
  return StatusCode::SUCCESS;
}
		
StatusCode CoolInserter::finalize() {
  ATH_MSG_INFO("finalizing ");
  try {
    for(unsigned int i=0; i<m_mdtt0_fld.size(); i++)
      m_mdtt0_fld[i]->flushStorageBuffer();	
    for(unsigned int i=0; i<m_mdtrt_fld.size(); i++)
      m_mdtrt_fld[i]->flushStorageBuffer();
  }
  catch(std::exception & e) {
    ATH_MSG_FATAL("Exception in finalize: " << e.what());
    return StatusCode::FAILURE;
  }	
  return StatusCode::SUCCESS;	
}

StatusCode CoolInserter::execute() {
  for(unsigned int i=0; i<m_calibration_sources.size(); i++) {
    ATH_MSG_INFO("Now running " <<m_calibration_sources[i]);
    if (!m_calibration_sources[i]->InstertCalibration(this, static_cast<bool>(m_t0_folder.size()), static_cast<bool>(m_rt_folder.size()))) {
      ATH_MSG_FATAL("Calibration Source " << m_calibration_sources[i].name() << " failed!");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

bool CoolInserter::StartT0Chamber(const NtupleStationId & sid) {
  m_n_tubes_added=0;
  m_sid=sid;
  m_sid.SetMultilayer(0);

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
  if(MuonDetMgr==nullptr){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
    return false; 
  } 

  if (!m_sid.InitializeGeometry(m_idHelperSvc->mdtIdHelper(), MuonDetMgr))
    return false;
//get number of tubes
  int max_nly(-1);
  int max_ntb(-1);
  for(int ml=0; ml<m_sid.NMultilayers(); ml++) {
    if( max_nly < m_sid.NLayers(ml) )
      max_nly=m_sid.NLayers(ml);
    if( max_ntb < m_sid.NTubes(ml) )
      max_ntb=m_sid.NTubes(ml);
  }
  m_n_tubes_chamber=max_nly * max_ntb * m_sid.NMultilayers();
  m_data_string.str("");
  m_data_string.clear();
  m_data_string << "T0" << m_sid.regionId() << "," << m_version<< ",1," <<m_n_tubes_chamber<<std::endl;
  m_aver_t0=0;
  m_aver_adc=0;
  return true;
}

bool CoolInserter::AppendT0(float t0, int validflag, float adc0) {
  if(m_n_tubes_added >= m_n_tubes_chamber) {
    return false;
  }
  m_data_string<<t0<<","<<validflag<<","<<adc0<<",";
  m_n_tubes_added++;
  m_aver_t0+=t0;
  m_aver_adc+=adc0;
  return true;
}

bool CoolInserter::StoreT0Chamber(const NtupleStationId & id,  const std::string & file, unsigned int creation_flags) {
  MuonFixedId fid(id.FixedId());	
//generate fixed if for chamber
  if(!p_reg_sel_svc->isInRegion(fid)) {
    return true;
  }
//do not fill double
  if(m_t0_filled.find(id) != m_t0_filled.end()) {
    return true;
  }
  m_t0_filled.insert(id);
//check number of added tubes
  if (m_n_tubes_added < m_n_tubes_chamber) {
    ATH_MSG_WARNING("Filling missing tubes with average for chamber" << m_sid.regionId());
    m_aver_t0/=m_n_tubes_added;
    m_aver_adc/=m_n_tubes_added;
    for(int i=m_n_tubes_added; i<m_n_tubes_chamber; i++) {
      AppendT0(m_aver_t0, 5, m_aver_adc);
    }
  }
//cool 
  if(m_cool_connect && !m_t0_created) {
    for(unsigned int i=0; i<m_t0_folder.size(); i++) {
      if(!m_db->existsFolder(m_t0_folder[i])) {
	if(!create_folder(i, false)) {
	  ATH_MSG_FATAL("Cannot create folder '" << m_t0_folder[i] <<"'!");
	  return false;
	}
      }
      m_mdtt0_fld.push_back(m_db->getFolder(m_t0_folder[i]));
      m_mdtt0_fld[i]->setupStorageBuffer();
			
      cool::RecordSpecification mdtt0_rec =(m_mdtt0_fld[i]->payloadSpecification());
      bool compressed(false);
      if(m_compressed_t0.size()>i)
	compressed=m_compressed_t0[i];
      m_mdtt0_cool_row[compressed] = cool::Record(mdtt0_rec).attributeList();
    }
    m_t0_created = true;
  }
  if(m_t0_created) {
    for(std::map<bool, coral::AttributeList>::iterator it=m_mdtt0_cool_row.begin(); it!=m_mdtt0_cool_row.end(); it++) {
      it->second["file"].data<cool::String4k>() = file;
      it->second["tech"].data<int>() = creation_flags;
      if(it->first) {
	uLongf dest_len = compressBound(m_data_string.str().size());
	coral::Blob &blob( it->second["data"].data<coral::Blob>());
	blob.resize(dest_len);
	Bytef * p=static_cast<Bytef *>(blob.startingAddress());		
			
	compress(p, &dest_len, reinterpret_cast<const Bytef *>(m_data_string.str().c_str()), m_data_string.str().size());
	blob.resize(dest_len);
      }	else {
	it->second["data"].data<cool::String16M>() = m_data_string.str();
      }
      for(unsigned int i=0; i<m_mdtt0_fld.size(); i++) {
	cool::ConstRecordAdapter record (m_mdtt0_fld[i]->payloadSpecification(),
					 m_mdtt0_cool_row[m_compressed_t0[i]]);
	m_mdtt0_fld[i]->storeObject( m_iovt_start,
				     m_iovt_end,
				     record,
				     id.FixedId(),
				     m_tagt0[i]);
      }
    }
  }
  return true;
}

bool CoolInserter::StoreRtChamber(const NtupleStationId & id, const std::map<int, SamplePoint> & points, const std::string & file, unsigned int creation_flags) {
//generate fixed if for chamber
  MuonFixedId fid(id.FixedId());
  if(!p_reg_sel_svc->isInRegion(fid))	{
    return true;
  }
//check for duplicates
  NtupleStationId	id_cp(id);
  bool found[3];
  for(int i=0; i<3; i++) {
    id_cp.SetMultilayer(i);
    found[i]=(m_rt_filled.find(id_cp) != m_rt_filled.end());
  }
  if (id.GetMl()==0) {
    if(found[0] || (found[1] && found[2]))
      return true;
    if(found[1]) {
      id_cp.SetMultilayer(2);
    }
    if(found[2]) {
      id_cp.SetMultilayer(1);
    }
    if(!found[1] && !found[2]) {
      id_cp.SetMultilayer(0);
    }
  }
  if (id.GetMl()!=0){
    if(found[0] || found[id.GetMl()])
      return true;
  }
  m_rt_filled.insert(id_cp);
  if(m_cool_connect && !m_rt_created)	{
    for(unsigned int  i=0; i<m_rt_folder.size(); i++)	{
      
      if(!m_db->existsFolder(m_rt_folder[i]))	{
	if(!create_folder(i, true)) {
	  ATH_MSG_FATAL("Cannot create folder '" << m_rt_folder[i] <<"'!");
	  return false;
	}
      }
      m_mdtrt_fld.push_back(m_db->getFolder (m_rt_folder[i]));
      m_mdtrt_fld[i]->setupStorageBuffer();
      
      cool::RecordSpecification mdtrt_rec =(m_mdtrt_fld[i]->payloadSpecification());
      bool compressed(false);
      if(m_compressed_rt.size()>i)
	compressed=m_compressed_rt[i];
      m_mdtrt_cool_row[compressed] = cool::Record(mdtrt_rec).attributeList();
    }
    m_rt_created = true;
  }
  std::ostringstream data_column;
  data_column<<id_cp.FixedId()<<","<<points.size()<<std::endl;
  for(std::map<int, SamplePoint>::const_iterator it=points.begin(); it!=points.end(); it++) {
    data_column<<it->second.x2() << "," << it->second.x1() << "," << it->second.error() << ",";
  }
  if(m_rt_created) {
    for(std::map<bool, coral::AttributeList>::iterator it=m_mdtrt_cool_row.begin(); it!=m_mdtrt_cool_row.end(); it++) {
      it->second["tech"].data<int>() = creation_flags;
      it->second["file"].data<cool::String4k>() = file;
      if(it->first) {
	uLongf dest_len = compressBound(data_column.str().size());
	coral::Blob &blob(it->second["data"].data<coral::Blob>());
	blob.resize(dest_len);
	Bytef * p=static_cast<Bytef *>(blob.startingAddress());
	
	compress(p, &dest_len, reinterpret_cast<const Bytef *>(data_column.str().c_str()), data_column.str().size());
	blob.resize(dest_len);
      } else {
	it->second["data"].data<cool::String16M>() = data_column.str();
      }
    }//for(std::map<bool, coral::AttributeList> ...
    for(unsigned int i=0; i<m_mdtrt_fld.size(); i++) {
      cool::ConstRecordAdapter record (m_mdtrt_fld[i]->payloadSpecification(),
				       m_mdtrt_cool_row[m_compressed_rt[i]]);
      m_mdtrt_fld[i]->storeObject( m_iovt_start,
				   m_iovt_end,
				   record,
				   id_cp.FixedId(),
				   m_tagrt[i]);
    }
  }
  return true;
}

inline bool CoolInserter::create_folder(const unsigned int & fldr_nr, const bool &is_rt) {
  std::string folder;
  std::string description;
  if (is_rt) {
    folder = m_rt_folder[fldr_nr];
    description = std::string("<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName><key>")+folder+"</key>";
  } else {
    folder = m_t0_folder[fldr_nr];
    description = std::string("<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName><key>")+folder+"</key>";
  }
		
  bool compressed(false);
  if (is_rt) {
    if(m_compressed_rt.size()>fldr_nr)
      if (m_compressed_rt[fldr_nr])
	compressed=true;
  } else {
    if(m_compressed_t0.size()>fldr_nr)
      if (m_compressed_t0[fldr_nr])
	compressed=true;
  }	
  cool::RecordSpecification spec;
  spec.extend("tech",cool::StorageType::Int32);
  if(compressed) {
    spec.extend("file",cool::StorageType::String255);
    spec.extend("data",cool::StorageType::Blob64k);
  } else {
    spec.extend("file",cool::StorageType::String4k);
    if(is_rt) {
      spec.extend("data", cool::StorageType::String4k);
    } else {
      spec.extend("data", cool::StorageType::String16M);
    }
  }
  cool::FolderSpecification fspec (cool::FolderVersioning::MULTI_VERSION,
				   spec);
  m_db->createFolder(folder, fspec, description, true);
  return true;
}

} //namespace MuonCalib
