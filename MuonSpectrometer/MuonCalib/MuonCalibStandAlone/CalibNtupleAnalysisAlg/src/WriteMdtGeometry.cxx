/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/WriteMdtGeometry.h"

//coral
#include "RelationalAccess/IRelationalService.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/ISessionProxy.h"
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
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralKernel/Context.h"

#include <stdexcept>

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"

namespace MuonCalib {

WriteMdtGeometry::WriteMdtGeometry(const std::string &name, ISvcLocator *pSvcLocator) : 
    AthAlgorithm(name, pSvcLocator),
    m_context( &coral::Context::instance() ),
    m_idToFixedIdToolType("MuonCalib::IdToFixedIdTool"),
    m_idToFixedIdToolName("MuonCalib_IdToFixedIdTool"),
    m_session(nullptr),
    m_detMgr(nullptr),
    m_id_tool(nullptr) {

  declareProperty("idToFixedIdToolType", m_idToFixedIdToolType);
  declareProperty("idToFixedIdToolName", m_idToFixedIdToolName);
  declareProperty("ConnectionString", m_connectionString);
  declareProperty("WorkingSchema", m_WorkingSchema);

}  //end WriteMdtGeometry::WriteMdtGeometry

StatusCode WriteMdtGeometry::initialize() {
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(detStore()->retrieve(m_detMgr));
  ATH_CHECK(toolSvc()->retrieveTool(m_idToFixedIdToolType, m_idToFixedIdToolName, m_id_tool));
	
  loadServices();
	
  try {
    OpenConnection();
    coral::ITableDataEditor &editor = m_session->nominalSchema().tableHandle("MDT_CHAMBER").dataEditor();
    if (!fill_db(editor)) {
      CloseConnection(false);
      return StatusCode::FAILURE;
    }
    CloseConnection(true);	
  }
  catch ( coral::SchemaException &e )  {
    ATH_MSG_ERROR( "Schema exception : " << e.what() );
    CloseConnection(false);	
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Initialisation done!" );
  return StatusCode::SUCCESS;
}  //end WriteMdtGeometry::initialize

void WriteMdtGeometry::loadServices() {
  coral::IHandle<coral::IConnectionService> lookSvcH = m_context->query<coral::IConnectionService>();
  if (!lookSvcH.isValid()) {
    m_context->loadComponent( "CORAL/Services/ConnectionService" );
     lookSvcH = m_context->query<coral::IConnectionService>();
  }
  if (!lookSvcH.isValid()) {
     throw std::runtime_error( "Could not locate the connection service" );
  }
  m_context->loadComponent( "CORAL/Services/XMLAuthenticationService" );
  m_context->loadComponent( "CORAL/Services/RelationalService" );
}  //end WriteMdtGeometry::loadServices

inline bool WriteMdtGeometry::fill_db(coral::ITableDataEditor &editor) {
  ATH_MSG_INFO( "Filling db" );
  coral::AttributeList rowBuffer;
  rowBuffer.extend<int>("CHAMBER");
  rowBuffer.extend<int>("LY");
  rowBuffer.extend<int>("ML");
  rowBuffer.extend<int>("N_TUBES");
  rowBuffer.extend<float>("LOC_Y");
  rowBuffer.extend<float>("LOC_Z");
  rowBuffer.extend<float>("Y_SPACING");
  //loop on chambers
  MdtIdHelper::const_id_iterator it     = m_idHelperSvc->mdtIdHelper().module_begin();
  MdtIdHelper::const_id_iterator it_end = m_idHelperSvc->mdtIdHelper().module_end();
  for( ; it!=it_end;++it ) {
    const MuonGM::MdtReadoutElement *detEl = m_detMgr->getMdtReadoutElement( m_idHelperSvc->mdtIdHelper().channelID(*it,1,1,1));
    if(!detEl) continue;
    //get number of mls;
    int n_mls=m_idHelperSvc->mdtIdHelper().numberOfMultilayers(*it);
    //fixed id
    MuonFixedId fixed_id(m_id_tool->idToFixedId(*it));
    rowBuffer["CHAMBER"].data<int>() = fixed_id.mdtChamberId().getIdInt();
    //loop on multilayers
    for(int ml=1; ml<=n_mls; ml++) {
      rowBuffer["ML"].data<int>()=ml;
      const MuonGM::MdtReadoutElement *detEl_ml = m_detMgr->getMdtReadoutElement(m_idHelperSvc->mdtIdHelper().channelID(*it,ml ,1,1));
      int n_layers=detEl_ml->getNLayers();
      int n_tubes=detEl_ml->getNtubesperlayer();
      rowBuffer["N_TUBES"].data<int>()=n_tubes;
      for(int ly=1; ly<=n_layers; ly++)	{
	rowBuffer["LY"].data<int>()=ly;
	fillLayer(detEl_ml, rowBuffer, ml, ly);
	editor.insertRow(rowBuffer);
      }
    }
  }
  return true;
}  //end WriteMdtGeometry::fill_db

inline void WriteMdtGeometry::fillLayer(const MuonGM::MdtReadoutElement *detEl, coral::AttributeList &rowBuffer, const int &ml, const int &ly) {
  Amg::Vector3D TubePos1 = detEl->GlobalToAmdbLRSCoords(detEl->tubePos(ml,ly,1));
  Amg::Vector3D TubePos2 = detEl->GlobalToAmdbLRSCoords(detEl->tubePos(ml,ly,2));
  rowBuffer["LOC_Y"].data<float>()=TubePos1.y();
  rowBuffer["LOC_Z"].data<float>()=TubePos1.z();
  rowBuffer["Y_SPACING"].data<float>()=TubePos2.y() - TubePos1.y();
}  //end WriteMdtGeometry::fillLayer

void WriteMdtGeometry::OpenConnection() {
  coral::ConnectionService connSvc;
  m_session = connSvc.connect( m_connectionString );
}

void WriteMdtGeometry::CloseConnection(bool commit) {
  if(m_session != NULL)	{
    if(commit)
      m_session->transaction().commit();
    else
      m_session->transaction().rollback();
  }
  m_session=NULL;
}

coral::IRelationalDomain& WriteMdtGeometry::domain( const std::string &connectionString ) {
  coral::IHandle<coral::IRelationalService> relationalService= m_context->query<coral::IRelationalService>();
  if ( !relationalService.isValid() ) {
    throw std::runtime_error( "Could not locate the relational service" );
  }
  if(connectionString.find("sqlite")!=0) {
    coral::IHandle<coral::IAuthenticationService> authenticationService= m_context->query<coral::IAuthenticationService>();
    if ( !authenticationService.isValid() ) {
      throw std::runtime_error( "Could not locate the authentication service" );
    }
    
    const coral::IAuthenticationCredentials &credentials = authenticationService->credentials( connectionString );
    m_username = credentials.valueForItem( "user" );
    m_password = credentials.valueForItem( "password" );
  } else {
    m_username="";
    m_password="";
  }
  return relationalService->domainForConnection( connectionString );
}  //end WriteMdtGeometry::domain

} //namespace MuonCalib
