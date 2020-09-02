/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibDbOperations/CalibT0DbOperations.h"
#include "MuonCalibDbOperations/CalibDbConnection.h"
#include "MuonCalibDbOperations/IConditionsStorage.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MdtCalibData/MdtTubeFitContainer.h"
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
#include "cmath"

namespace MuonCalib {

CalibT0DbOperations::CalibT0DbOperations(CalibDbConnection & db_conn) : m_db_conn(db_conn), m_query(nullptr) {
}

MdtStationT0Container * CalibT0DbOperations::LoadT0Calibration(const NtupleStationId & id, int head_id, std::string & site_name) {
  try {
    //select MDT_TUBE.TUBE_ID, MDT_TUBE.P4 from MDT_TUBE where MDT_TUBE.HEAD_ID = :hid and MDT_TUBE.CHAMBER = :chamb and MDT_TUBE.VALIDFLAG>1
    m_db_conn.OpenTransaction();
    m_query=m_db_conn.GetQuery();
    m_query->setRowCacheSize(432); 
    m_query->addToTableList("MDT_TUBE");
    coral::AttributeList conditionData;
    conditionData.extend<int>("hid");
    conditionData[0].data<int>()=head_id;
//as long as the chamber id is stored as a string, it is more efficient to pass the chamber id as a string in the query
    conditionData.extend<int>("chamb");
    conditionData[1].data<int>()=id.FixedId();
    std::string condition = "MDT_TUBE.HEAD_ID = :hid and MDT_TUBE.CHAMBER = :chamb and MDT_TUBE.VALIDFLAG>1";
    m_query->setCondition( condition, conditionData );
    m_query->addToOutputList( "MDT_TUBE.TUBE_ID", "TUBE_ID" );
    m_query->addToOutputList( "MDT_TUBE.P4", "T0" );
    m_query->addToOutputList( "MDT_TUBE.ADC_1", "ADC_1" );
    coral::ICursor& cursor = m_query->execute();
    MdtStationT0Container *ret=new MdtStationT0Container();
    int count(0);
    while (cursor.next()) {
      const coral::AttributeList & al= cursor.currentRow();
      MuonFixedId fid(al[0].data<int>());
      ret->Sett0(fid.mdtMultilayer(), fid.mdtTubeLayer(), fid.mdtTube(), al[1].data<float>(), al[2].data<float>());
      count++;
    }
    if(count==0) {
      MsgStream log(Athena::getMessageSvc(),"CalibT0DbOperations");
      log<<MSG::WARNING<<"No tubes found for "<<id.regionId()<<" Header "<<site_name<<head_id<<endmsg;
      delete ret;
      return nullptr;
    }
    return ret;
  }//try
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibT0DbOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    return nullptr;
  }
}

MdtTubeFitContainer * CalibT0DbOperations::LoadT0Validation(const NtupleStationId & id, int head_id, std::string &/*site_name*/) {
  try {
    m_db_conn.OpenTransaction();
    m_query=m_db_conn.GetQuery();
    m_query->setRowCacheSize(432); 
    m_query->addToTableList("MDT_TUBE");
    m_query->addToTableList("MDT_TUBE_V");
    coral::AttributeList conditionData;
    conditionData.extend<int>("hid");
    conditionData[0].data<int>()=head_id;
//as long as the chamber id is stored as a string, it is more efficient to pass the chamber id as a string in the query
    std::ostringstream ostr;
    ostr<<id.FixedId();
    conditionData.extend<std::string>("chamb");
    conditionData[1].data<std::string>()=ostr.str();
    std::string condition = "MDT_TUBE.HEAD_ID = :hid and MDT_TUBE.CHAMBER = :chamb  and MDT_TUBE.VALIDFLAG=0 and MDT_TUBE_V.HEAD_ID=:hid and MDT_TUBE_V.TUBE_ID = MDT_TUBE.TUBE_ID and MDT_TUBE_V.CALIBFLAG=MDT_TUBE.CALIBFLAG";
    m_query->setCondition( condition, conditionData );
    m_query->addToOutputList( "MDT_TUBE.TUBE_ID", "TUBE_ID" );
    m_query->addToOutputList( "MDT_TUBE.P4", "T0" );
    m_query->addToOutputList( "MDT_TUBE_V.ENTRIES", "ENTRIES");
    m_query->addToOutputList( "MDT_TUBE_V.CHISQUARE_1", "CHISQUARE_1");
    m_query->addToOutputList( "MDT_TUBE_V.CHISQUARE_2", "CHISQUARE_2");
    for(int i=0; i<7; i++) {
      std::ostringstream ostr;
      if(i==4) {
	ostr<<"MDT_TUBE.P4";
      }	else {
	ostr<<"MDT_TUBE_V.P"<<i;
      }
      std::ostringstream ostr2;
      ostr2<<"P"<<i;
      m_query->addToOutputList( ostr.str(), ostr2.str() );
      if(i>3 && i<7) {
	if(i==4) {
	  m_query->addToOutputList("MDT_TUBE_V.P4_ERR", "P4_ERR");
	} else {
	  ostr<<"_ERR";
	  ostr2<<"_ERR";
	  m_query->addToOutputList( ostr.str(), ostr2.str() );
	}
      }
    }
    m_query->addToOutputList( "MDT_TUBE.NHITS", "NHITS");
    m_query->addToOutputList( "MDT_TUBE.NHITS_ABOVE_ADC_CUT", "NHITS_ABOVE_ADC_CUT");
    m_query->addToOutputList( "MDT_TUBE_V.ALGO_FLAG", "ALGO_FLAG");
    m_query->addToOutputList( "MDT_TUBE_V.TUBE_GROUPING", "TUBE_GROUPING");
    coral::ICursor& cursor = m_query->execute();
    int n_tubes(0), n_layers(0), n_ml(0);
    std::map<MuonFixedId, MdtTubeFitContainer::SingleTubeFit> fits;
    std::map<MuonFixedId, MdtTubeFitContainer::SingleTubeCalib> calibs;
    std::string alg_flg, tb_grp;
    bool str_set(false);
    while (cursor.next()) {
      const coral::AttributeList & al= cursor.currentRow();
      MuonFixedId fid(al["TUBE_ID"].data<int>());
      MdtTubeFitContainer::SingleTubeCalib &calib(calibs[fid]);
      calib.t0=al["P4"].data<float>();
      calib.adcCal=0;
      MdtTubeFitContainer::SingleTubeFit &fit(fits[fid]);
      fit.statistics = al["ENTRIES"].data<int>();
      fit.n_hits=al["NHITS"].data<int>();
      fit.n_hits_above_adc_cut = al["NHITS_ABOVE_ADC_CUT"].data<int>();
      fit.chi2Tdc=0.0;
      fit.chi2TdcEnd=0.0;
      for(int i=0; i<7; i++) {
	std::ostringstream ostr2;
	ostr2<<"P"<<i;
	fit.par[i]=al[ostr2.str()].data<float>();
	if(i>3 && i<7) {
	  ostr2<<"_ERR";
	  fit.cov[i]=al[ostr2.str()].data<float>();
	} else {
	  fit.cov[i]=0.0;
	}
      }
      if(n_tubes<fid.mdtTube())
	n_tubes=fid.mdtTube();
      if(n_layers<fid.mdtTubeLayer())
	n_layers=fid.mdtTubeLayer();
      if(n_ml<fid.mdtMultilayer())
	n_ml=fid.mdtMultilayer();
      if(!str_set) {
	alg_flg=al["ALGO_FLAG"].data<std::string>();
	tb_grp=al["TUBE_GROUPING"].data<std::string>();
	str_set=true;
      }
    }
    //create output class
    MdtTubeFitContainer *ret=new MdtTubeFitContainer(id.regionId(), n_ml, n_layers, n_tubes);
    for(std::map<MuonFixedId, MdtTubeFitContainer::SingleTubeFit>::const_iterator it=fits.begin(); it!=fits.end(); it++) {
      int ml=it->first.mdtMultilayer()-1;
      int ly=it->first.mdtTubeLayer()-1;
      int tb=it->first.mdtTube()-1;
      ret->setFit(ml, ly, tb, it->second);
      ret->setCalib(ml, ly, tb, calibs[it->first]);
    }
    ret->setGroupBy(tb_grp);
    ret->setImplementation(alg_flg);
    return ret;	
  } //try
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibT0DbOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    return nullptr;
  }
}

bool CalibT0DbOperations::WriteT0Chamber(const NtupleStationId & id, const MdtTubeFitContainer * t0, std::vector<int> & validation_flag, int head_id, const std::string & site_name) {
  try {
    m_db_conn.OpenTransaction();
    if(!checkTubesPresent(head_id, site_name, id, validation_flag)) {
      MsgStream log(Athena::getMessageSvc(),"CalibT0DbOperations");
      log<<MSG::WARNING<<"Trying to insert data for chamber " << id.regionId()<<" header "<<head_id<<site_name<<"  which already exists!"<<endmsg;
      m_db_conn.Rollback();
      return false;
    }
    coral::ITableDataEditor *editor[] = {&m_db_conn.GetTableEditor(std::string("MDT_TUBE")), &m_db_conn.GetTableEditor(std::string("MDT_TUBE_V")), &m_db_conn.GetTableEditor(std::string("MDT_TUBE_C"))};
    //insert new tubes			
    if (validation_flag.size()==0) {
      if(!insertTubes(site_name, head_id, id, t0, validation_flag, editor)) {
	m_db_conn.Rollback();
	return false;
      }
      m_db_conn.Commit();
      return true;
    }
    //check if all tubes ar enew or if all tubes a validated good
    bool all_new(true), all_good(true);
    for(std::vector<int>::const_iterator it =  validation_flag.begin(); it!=validation_flag.end(); it++) {
      if ((*it)!=0) all_new=false;
      if ((*it)!=3) all_good=false;
      if (!all_new && !all_good) break;
    }
    //insert new tubes
    if(all_new)	{
      if(!insertTubes(site_name, head_id, id, t0, validation_flag, editor)) {
	m_db_conn.Rollback();
	return false;
      }
      m_db_conn.Commit();
      return true;
    }
    //all tubes are good. Update the whole chamber in one go
    if(all_good) {
      if(!setValidationFlag(site_name, head_id, id, 0, 3, *editor[0])) {
	m_db_conn.Rollback();
	return false;
      }
      m_db_conn.Commit();
      return true;
    }
    //update flag for failed and dummy tubes - insert for failed
    std::vector<coral::AttributeList> rowBuffer(3);
    bool row_buffer_initialized(false);
    unsigned int nml=t0->numMultilayers();
    unsigned int nly=t0->numLayers();
    unsigned int ntb=t0->numTubes();
    MuonFixedId fixId(0);
    fixId.setTechnology(0);
    fixId.setStationName(id.GetStation());
    fixId.setStationEta(id.GetEta());
    fixId.setStationPhi(id.GetPhi());
    for(unsigned int ml=0; ml<nml; ml++) {
      for(unsigned int ly=0; ly<nly; ly++) {
	for(unsigned int tb=0; tb<ntb; tb++) {
	  int tb_index=tb + ntb*ly + ntb*nly*ml;
	  if(validation_flag[tb_index]==3) continue;
	  fixId.setMdtTube(tb+1);
	  fixId.setMdtTubeLayer(ly+1);
	  fixId.setMdtMultilayer(ml+1);
	  //new tubes or tubes that are validated bad are inserted
	  int rowsUpdated;
	  if (validation_flag[tb_index]==5)
	    rowsUpdated=setValidFlag(site_name, head_id, fixId.getIdInt(), 5, *editor[0]);
	  else
	    rowsUpdated=setValidFlag(site_name, head_id, fixId.getIdInt(), 1, *editor[0]);
	  
	  if(rowsUpdated!=1){
      MsgStream log(Athena::getMessageSvc(),"CalibT0DbOperations");
      log<<MSG::WARNING<<id.regionId()<<" tb="<<tb<<" ly="<<ly<<" ml="<<ml<<" head_id="<<head_id<<": "<<rowsUpdated<<" Rows to be updated! This is wrong! Check database!"<<endmsg;
	    m_db_conn.Rollback();
	    return false;
	  }
	  if(validation_flag[tb_index]!=5) {
	    //initialize row buffers
	    if(!row_buffer_initialized)	{
	      initRowBuffer(rowBuffer, id, head_id, site_name,  t0);
	      row_buffer_initialized=true;
	    }
	    //fill row buffers
	    fillRowBuffer(rowBuffer, t0, ml, ly, tb, fixId);
	    rowBuffer[0]["VALIDFLAG"].data<int>() =validation_flag[tb_index];
	    //loop over 3 t0 tables - get seqid from 1st and insert it into 2nd and 3rd.
	    for(unsigned int i=0; i<3; i++) {
	      rowBuffer[i]["CALIBFLAG"].data<int>() = static_cast<int>(validation_flag[tb_index] > 3);
	      editor[i]->insertRow( rowBuffer[i] );
	    }
	  }
	}
      }
    }
	//now all tubes whit validflag == 0 are good update in one go
    if(!setValidationFlag(site_name, head_id, id, 0, 3, *editor[0])) {
      m_db_conn.Rollback();
      return false;
    }
    m_db_conn.Commit();
    return true;
  }//try
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibT0DbOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    m_db_conn.Rollback();
    return false;
  }
}

bool CalibT0DbOperations::ReadForConditions(std::string /*site_name*/, int head_id, IConditionsStorage & storage) {
  try {
    //prepare query
    m_db_conn.OpenTransaction();
    m_query=m_db_conn.GetQuery();
    m_query->addToTableList("MDT_TUBE");
    m_query->addToOutputList("MDT_TUBE.CHAMBER", "CHAMBER");
    m_query->addToOutputList("MDT_TUBE.TUBE_ID", "TUBE_ID");
    m_query->setRowCacheSize(10000); 	
    const std::vector<std::string> &rows_tube_table(storage.RowsFromMdtTubeTable());
    for(std::vector<std::string>::const_iterator it=rows_tube_table.begin(); it!=rows_tube_table.end(); it++) {
      if(*it=="CHAMBER" || *it=="TUBE_ID") continue;
      m_query->addToOutputList(std::string("MDT_TUBE.") + *it, *it);
    }
    const std::vector<std::string> &rows_tube_v_table(storage.RowsFromMdtTubeVTable());
    if(rows_tube_v_table.size() != 0) {
      m_query->addToTableList("MDT_TUBE_V");
      for(std::vector<std::string>::const_iterator it=rows_tube_v_table.begin(); it!=rows_tube_v_table.end(); it++) {
	m_query->addToOutputList( std::string("MDT_TUBE_V.") + *it, *it);
      }
    }
    const std::vector<std::string> &rows_tube_c_table(storage.RowsFromMdtTubeCTable());
    if(rows_tube_c_table.size() != 0) {
      m_query->addToTableList("MDT_TUBE_C");
      for(std::vector<std::string>::const_iterator it=rows_tube_c_table.begin(); it!=rows_tube_c_table.end(); it++) {
	m_query->addToOutputList( std::string("MDT_TUBE_C.") + *it, *it);
      }
    }
    coral::AttributeList conditionData;
    conditionData.extend<int>("HEAD_ID");
    conditionData["HEAD_ID"].data<int>()=head_id;
    std::string condition = "MDT_TUBE.HEAD_ID = :HEAD_ID and MDT_TUBE.VALIDFLAG>1 ";
    if (rows_tube_v_table.size() != 0)
      condition = condition + " and MDT_TUBE.HEAD_ID = MDT_TUBE_V.HEAD_ID and MDT_TUBE.TUBE_ID=MDT_TUBE_V.TUBE_ID and MDT_TUBE.CALIBFLAG=MDT_TUBE_V.CALIBFLAG";
    if (rows_tube_c_table.size() != 0)
      condition = condition + " and MDT_TUBE.HEAD_ID = MDT_TUBE_C.HEAD_ID and MDT_TUBE.TUBE_ID=MDT_TUBE_C.TUBE_ID and MDT_TUBE.CALIBFLAG=MDT_TUBE_C.CALIBFLAG";
    m_query->setCondition( condition, conditionData );
    m_query->addToOrderList( "MDT_TUBE.CHAMBER" );
    //execute query
    int old_chamber_id(-1);
    std::map<IConditionsStorage::TubeId, coral::AttributeList> rows_of_a_chamber;
    coral::ICursor& cursor = m_query->execute();
    while (cursor.next()) {
      const coral::AttributeList & al= cursor.currentRow();
      //if we have a new chamber - call callback
      if(old_chamber_id != al["CHAMBER"].data<int>() && rows_of_a_chamber.size() != 0) {
	if(!storage.StoreT0Chamber(old_chamber_id, rows_of_a_chamber)) {
	  return false;
	}
	rows_of_a_chamber.clear();
      }
      //store row
      IConditionsStorage::TubeId id(al["TUBE_ID"].data<int>());
      rows_of_a_chamber[id]=al;
      old_chamber_id=al["CHAMBER"].data<int>();
    }
    if(!storage.StoreT0Chamber(old_chamber_id, rows_of_a_chamber)) {
      return false;
    }
    return true;	
  }
  catch( coral::SchemaException& e ) {
    MsgStream log(Athena::getMessageSvc(),"CalibT0DbOperations");
    log<<MSG::WARNING<<"Schema exception : " << e.what()<<endmsg;
    return false;
  }
}

inline void CalibT0DbOperations::initRowBuffer(std::vector<coral::AttributeList> & rowBuffer, const NtupleStationId &id, const int & head_id, const std::string & /*site_name*/, const MdtTubeFitContainer * t0) {
  for(unsigned int i=0; i<3; i++) {
    rowBuffer[i].extend<int>("TUBE_ID");
    rowBuffer[i].extend<int>("HEAD_ID");
    rowBuffer[i]["HEAD_ID"].data<int>()=head_id;
    rowBuffer[i].extend<int>("CALIBFLAG");
  }
  rowBuffer[0].extend<int>("CHAMBER");
  rowBuffer[0]["CHAMBER"].data<int>()=id.FixedId();
  rowBuffer[0].extend<int>("VALIDFLAG");
  rowBuffer[1].extend<int>("ENTRIES");
  rowBuffer[1].extend<float>("CHISQUARE_1");
  rowBuffer[1].extend<float>("CHISQUARE_2");
  for(int i=0; i<8; i++) {
    std::ostringstream ostr2;
    ostr2<<"P"<<i;
    if(i==4)
      rowBuffer[0].extend<float>(ostr2.str());
    else
      rowBuffer[1].extend<float>(ostr2.str());
    ostr2<<"_ERR";
    if(i>3 && i<7)
      rowBuffer[1].extend<float>(ostr2.str());
    else
      rowBuffer[2].extend<float>(ostr2.str());
  }
  for(int i=0; i<4; i++) {
    std::ostringstream ostr2;
    ostr2<<"ADC_"<<i;
    //!!!!!!ADC_0 should be moved to MDT_TUBE
    rowBuffer[0].extend<float>(ostr2.str());
    ostr2<<"_ERR";
    rowBuffer[2].extend<float>(ostr2.str());
  }
  rowBuffer[2].extend<float>("ADC_CHISQUARE");	
  rowBuffer[0].extend<int>("NHITS");
  rowBuffer[0].extend<int>("NHITS_ABOVE_ADC_CUT");
  rowBuffer[1].extend<std::string>("ALGO_FLAG");
  if(t0->implementation()=="T0CalibrationClassic") {
    rowBuffer[1]["ALGO_FLAG"].data<std::string>()="T0Classic";
  } else if(t0->implementation()=="T0CalibrationMT") { 
    rowBuffer[1]["ALGO_FLAG"].data<std::string>()="T0MT";
  } else {
    rowBuffer[1]["ALGO_FLAG"].data<std::string>()="Unknown";
  }
  rowBuffer[1].extend<std::string>("TUBE_GROUPING");
  rowBuffer[1]["TUBE_GROUPING"].data<std::string>() = "UNKNOWN";
}

inline void CalibT0DbOperations::fillRowBuffer(std::vector<coral::AttributeList> & rowBuffer, const MdtTubeFitContainer * t0, const int & ml, const int &ly, const int &tb, const MuonFixedId &fixId) {
  for(unsigned int i=0; i<3; i++) {
    rowBuffer[i]["TUBE_ID"].data<int>()=fixId.getIdInt();
  }
  rowBuffer[1]["ENTRIES"].data<int>()=t0->getFit(ml,ly,tb)->statistics;
  rowBuffer[1]["CHISQUARE_1"].data<float>()=t0->getFit(ml,ly,tb)->chi2Tdc;
  rowBuffer[1]["CHISQUARE_2"].data<float>() = t0->getFit(ml,ly,tb)->chi2TdcEnd;
  rowBuffer[1]["TUBE_GROUPING"].data<std::string>() = t0->getFit(ml,ly,tb)->group_by;
  for(int i=0; i<8; i++) {
    double val=t0->getFit(ml,ly,tb)->par[i];
    if(std::isnan(val)) val=-99999.;
    std::ostringstream ostr2;
    ostr2<<"P"<<i;
    if(i==4)
      rowBuffer[0][ostr2.str()].data<float>()=val;
    else
      rowBuffer[1][ostr2.str()].data<float>()=val;
    ostr2<<"_ERR";
    val=t0->getFit(ml,ly,tb)->cov[i];
    if(std::isnan(val)) val=-99999.;
    if(i>3 && i<7)
      rowBuffer[1][ostr2.str()].data<float>()=val;
    else
      rowBuffer[2][ostr2.str()].data<float>()=val;
  }
  for(int i=0; i<4; i++) {
    std::ostringstream ostr2;
    ostr2<<"ADC_"<<i;
    rowBuffer[0][ostr2.str()].data<float>() = t0->getFit(ml,ly,tb)->adc_par[i];
    ostr2<<"_ERR";
    rowBuffer[2][ostr2.str()].data<float>() = t0->getFit(ml,ly,tb)->adc_err[i];
  }
  rowBuffer[2]["ADC_CHISQUARE"].data<float>()=t0->getFit(ml,ly,tb)->adc_chi2;
  rowBuffer[0]["NHITS"].data<int>()=t0->getFit(ml,ly,tb)->n_hits;
  if(t0->getFit(ml,ly,tb)->n_hits > 999999)
    rowBuffer[0]["NHITS"].data<int>()=999999;
  rowBuffer[0]["NHITS_ABOVE_ADC_CUT"].data<int>() = t0->getFit(ml,ly,tb)-> n_hits_above_adc_cut;
  if(t0->getFit(ml,ly,tb)-> n_hits_above_adc_cut > 999999)
    rowBuffer[0]["NHITS_ABOVE_ADC_CUT"].data<int>() = 999999;
}

bool CalibT0DbOperations::setValidFlag(const std::string &/*site_name*/, const int & head_id, const int & tube_id, const int & new_validflag, coral::ITableDataEditor &editor) {
  std::string updateAction = "validflag = :newval";
  std::string updateCondition = "head_id=:hid and tube_id=:tid and validflag=0";
  coral::AttributeList updateData;
  updateData.extend<int>("hid");
  updateData[0].data<int>()=head_id;
  updateData.extend<int>("tid");
  updateData[1].data<int>()=tube_id;
  //	updateData.extend<std::string>("sn");
  //	updateData[2].data<std::string>()=site_name;
  updateData.extend<int>("newval");
  updateData[2].data<int>()=new_validflag;
  long rowsUpdated = editor.updateRows( updateAction, updateCondition, updateData );
  return rowsUpdated;
}

bool  CalibT0DbOperations::insertTubes(const std::string & site_name, int head_id, const NtupleStationId & id, const MdtTubeFitContainer * t0, const std::vector<int> & validation_flag, coral::ITableDataEditor * editor[]) {
  std::vector<coral::AttributeList> rowBuffer(3);
  std::vector<coral::IBulkOperation *> bulk_inserter(3);
  bool row_buffer_initialized(false);
  unsigned int nml=t0->numMultilayers();
  unsigned int nly=t0->numLayers();
  unsigned int ntb=t0->numTubes();
//do not insert completely dead chambers
  bool all_dead(true);	
  for(unsigned int ml=0; ml<nml; ml++) {
    for(unsigned int ly=0; ly<nly; ly++) {
      for(unsigned int tb=0; tb<ntb; tb++) {
	if(t0->getFit(ml,ly,tb)->statistics > 0) {
	  all_dead=false;
	  break;
	}
      }
    }
  }
  if(all_dead) {
    MsgStream log(Athena::getMessageSvc(),"CalibT0DbOperations");
    log<<MSG::WARNING<<"Refusing to insert completely dead chamber"<<endmsg;
    return false;
  }
  MuonFixedId fixId(0);
  fixId.setTechnology(0);
  fixId.setStationName(id.GetStation());
  fixId.setStationEta(id.GetEta());
  fixId.setStationPhi(id.GetPhi());
  for(unsigned int ml=0; ml<nml; ml++) {
    for(unsigned int ly=0; ly<nly; ly++) {
      for(unsigned int tb=0; tb<ntb; tb++) {
	int tb_index=tb + ntb*ly + ntb*nly*ml;
	fixId.setMdtTube(tb+1);
	fixId.setMdtTubeLayer(ly+1);
	fixId.setMdtMultilayer(ml+1);
	int v_flag=0;
	if (static_cast<int>(validation_flag.size())>tb_index) {
	  v_flag=validation_flag[tb_index];
	}
	if(!row_buffer_initialized) {
	  initRowBuffer(rowBuffer, id, head_id, site_name,  t0);
	  for (int i=0; i<3; i++) {
	    bulk_inserter[i]=editor[i]->bulkInsert( rowBuffer[i], 432);
	  }
	  row_buffer_initialized=true;
	}
	fillRowBuffer(rowBuffer, t0, ml, ly, tb, fixId);
	rowBuffer[0]["VALIDFLAG"].data<int>() =v_flag;
	for(unsigned int i=0; i<3; i++)	{
	  rowBuffer[i]["CALIBFLAG"].data<int>() = static_cast<int>(v_flag>3);
	  bulk_inserter[i]->processNextIteration();

	}
      }
    }
  }
  for(unsigned int i=0; i<3; i++) {
    bulk_inserter[i]->flush();
    delete bulk_inserter[i];
  }
  return true;
}

bool CalibT0DbOperations::setValidationFlag(const std::string & /*site_name*/, int head_id, const NtupleStationId & id, int from, int to, coral::ITableDataEditor & editor) {
  std::string updateAction = "validflag = :newval";
  std::string updateCondition = "head_id=:hid and chamber=:cham and validflag=:old_val";
  coral::AttributeList updateData;
  updateData.extend<int>("hid");
  updateData[0].data<int>()=head_id;
  updateData.extend<std::string>("cham");
  std::ostringstream ostr;
  ostr<<id.FixedId();
  updateData[1].data<std::string>()=ostr.str();
  updateData.extend<int>("newval");
  updateData[2].data<int>()=to;
  updateData.extend<int>("old_val");
  updateData[3].data<int>()=from;
  long rowsUpdated = editor.updateRows( updateAction, updateCondition, updateData );
  return (rowsUpdated<=432);
}
	
inline bool CalibT0DbOperations::checkTubesPresent(const int & head_id, const std::string &/*site_name*/, const NtupleStationId & id, const std::vector<int> & validflag) {
  m_query=m_db_conn.GetQuery();
//select count(MDT_TUBE.TUBE_ID) as  N_TUBES, MDT_TUBE.VALIDFLAG as VALIDFLAG from MDT_TUBE where MDT_TUBE.HEAD_ID = :hid and MDT_TUBE.CHAMBER = :chamb and SITE_NAME=:sn group by MDT_TUBE.VALIDFLAG;
  m_query->addToTableList("MDT_TUBE");	
  coral::AttributeList conditionData;
  conditionData.extend<int>("hid");
  conditionData[0].data<int>()=head_id;
//as long as the chamber id is stored as a string, it is more efficient to pass the chamber id as a string in the query
  std::ostringstream ostr;
  ostr<<id.FixedId();
  conditionData.extend<std::string>("chamb");
  conditionData[1].data<std::string>()=ostr.str();
  std::string condition = "MDT_TUBE.HEAD_ID = :hid and MDT_TUBE.CHAMBER = :chamb";
  m_query->setCondition( condition, conditionData );
  m_query->addToOutputList( "count(MDT_TUBE.TUBE_ID)", "N_TUBES");
  m_query->addToOutputList( "MDT_TUBE.VALIDFLAG", "VALIDFLAG" );
  m_query->groupBy("MDT_TUBE.VALIDFLAG");
  coral::ICursor& cursor = m_query->execute();
  bool has_unvalidated(false), has_validated(false);
  while (cursor.next())	{
    const coral::AttributeList & al= cursor.currentRow();
    if(al["VALIDFLAG"].data<short>()>0)	{
      if(al["N_TUBES"].data<double>()>0){
	has_validated=true;
      }
    } else {
      if(al["N_TUBES"].data<double>()>0) {
	has_unvalidated=true;
      }
    }
  }
//check what has to be done
  if(validflag.size()==0)
    return !has_unvalidated;
  for(std::vector<int>::const_iterator it=validflag.begin(); it!=validflag.end(); it++)	{
    if((*it) > 0 && has_validated)
      return false;
    if((*it) == 0 && has_unvalidated)
      return false;
  }
  return true;
}

} //namespace MuonCalib
