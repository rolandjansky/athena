///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCollectionCnv.cxx 
// Implementation file for class JetCollectionCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// RecAthenaPool includes
#include "JetCollectionCnv.h"

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"


/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/// Hook ourselves into the infrastructure.
JetCollectionCnv::JetCollectionCnv(ISvcLocator *svcLocator)
  : JetCollectionCnvBase (svcLocator),
    m_msgSvc(msgSvc()),
    m_log(m_msgSvc, "JetCollectionCnv")
{
}

///
/// Create the persistent output given a jet collection.
/// We only write out the JetCollection_tlp1 object - though
/// we might read back others (see below).
///
JetCollection_PERS* 
JetCollectionCnv::createPersistent( JetCollection* transCont ) 
{

  MsgStream msg( msgSvc(), "JetCollectionCnv" );
  msg << MSG::DEBUG << " in createPersistent" << endreq;
  std::vector<std::string> no_constit_colls = m_propTool->get_no_constit_colls();
  m_propTool->set_no_constit(false);
  // Test 1st in collection for its author.
  if(transCont->size() > 0){
    Jet * j = transCont->at(0);
    std::string author = j->jetAuthor();
    // If author match one given by joboptions, don't save constit
    for(size_t i=0;i<no_constit_colls.size();i++){
      if ( author == no_constit_colls[i] ) {
	m_propTool->set_no_constit(true);
	msg << MSG::DEBUG << " NOT saving constit "<< author << endreq; 
	break;
      }
    }
  }

  return m_TPConverter.createPersistent (transCont, m_log);
}

///
/// Read the data in from pool. This isn't trivial as
/// we must support various formats of the data coming in
/// and convert them all to a JetCollection. Schema evolution...
///
JetCollection* JetCollectionCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "JetCollectionCnv" );

  JetCollection *transObj = 0;

  msg << MSG::DEBUG << "  JetCollectionCnv::createTransient() "<< endreq;

  static pool::Guid tr_guid("2FB4D25D-E217-4B8A-B865-180DE73B20E0");
  static pool::Guid pj_guid("E97C0C61-0B3E-401C-B853-A3302168283E");
  static pool::Guid p1_guid("9B8CE942-1457-4392-A98A-A1A464990B9A");
  static pool::Guid p2_guid("0260D9A5-9C75-45FF-AC97-EA220FB11380");
  static pool::Guid p3_guid("8CC411B4-8369-4C61-9B15-8A806DBFBBD1");
  static pool::Guid tlp2_guid("668B769E-E190-4904-BE2C-1677279A9803");
  static pool::Guid tlp5_guid("BEDCAE36-5EFD-4035-A402-2DC4F7A6BF75");
  static pool::Guid tlp6_guid("77DFEE77-16A5-45DD-91FB-FDEEF2B65F7D");

  if ( compareClassGuid(tlp6_guid) ) {
    msg << MSG::DEBUG << "  JetCollectionCnv:  calling tlp6 converter" <<endreq;
    poolReadObject<JetCollection_tlp6> (m_TPConverter);
    return m_TPConverter.createTransient(m_log);

  } else  if ( compareClassGuid(tlp5_guid) ) {
    msg << MSG::DEBUG << "  JetCollectionCnv:  calling tlp5 converter" <<endreq;
    poolReadObject<JetCollection_tlp5> (tlp5_cnv);
    return transObj = tlp5_cnv.createTransient(m_log);

  } else if ( compareClassGuid(tlp2_guid) ) {

    // The T/P seperated object. JetCollection_PERS is not used here
    // as we want to read out specifically JetColleciton_tlp2, not just
    // the lastest guy.

    msg << MSG::DEBUG << "  JetCollectionCnv:  calling tlp2 converter" <<endreq;
    poolReadObject<JetCollection_tlp2> (tlp2_cnv);
    return tlp2_cnv.createTransient(m_log);

//     poolReadObject<JetCollection_tlp2> (m_TPConverter);
//     return transObj = m_TPConverter.createTransient(m_log);

  } else if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<JetCollection>();

  } else if ( compareClassGuid(p1_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<JetCollection_p1> persObj( poolReadObject<JetCollection_p1>() );
    transObj = p1_cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p2_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<JetCollection_p2> persObj( poolReadObject<JetCollection_p2>() );
    transObj = p2_cnv.createTransient( persObj.get(), msg );
    
  } else if ( compareClassGuid(p3_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<JetCollection_p3> persObj( poolReadObject<JetCollection_p3>() );
    transObj = p3_cnv.createTransient( persObj.get(), msg );
    
  } else if ( compareClassGuid(pj_guid) ) {
    msg << MSG::DEBUG << " creating pj_guid "<< endreq;
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<ParticleJetContainer_p1> persObj( poolReadObject<ParticleJetContainer_p1>() );
    transObj = pjp1_cnv.createTransient( persObj.get(), msg );
  } else {
    throw std::runtime_error("Unsupported persistent version of JetCollection");
  }

  return transObj;
}

///
/// initalize - create the hook to the extending converters. This is so we
/// can deal with things like subclasses of JetTagInfoBase from other
/// packages.
///
StatusCode JetCollectionCnv::initialize (void)
{
  StatusCode result = T_AthenaPoolCustomCnv<JetCollection, JetCollection_PERS>::initialize();
  if (!result) {
    return result;
  }

  // TPCnv property tool : 
  IJetTPCnvPropertiesTool * ipropTool;
  StatusCode sc;
  IToolSvc*     p_toolSvc;
  sc = service("ToolSvc", p_toolSvc);
  if( sc.isSuccess() ){
    sc = p_toolSvc->retrieveTool("JetTPCnvPropertiesTool",  ipropTool);
    m_log << MSG::INFO<< " Initialize JetCollectionCnv, iprop tool = "<< ipropTool<< endreq;
    if( sc.isSuccess() ) 
      m_propTool = dynamic_cast<JetTPCnvPropertiesTool*>(ipropTool);
    else {  
      m_log << MSG::ERROR << " Could not retrieve JetTPCnvPropertiesTool !! " <<endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
    m_log << MSG::ERROR << " Could not retrieve ToolSvc !! " <<endreq;
    return StatusCode::FAILURE;
  }
  ///
  /// Load the flavor tagging converter. The hard-coded CLID here is
  /// a run-time linkage to the JetTagInfo converter and collection.
  /// The must be the same, of course, for this to work.
  ///

  IConverter *converter = m_athenaPoolCnvSvc->converter(CLID(50623115));
  if (converter != 0) {
    registerExtendingCnv (converter);
  } else {
    m_log << MSG::FATAL << "Could not load the jet tag info converters for readback!" << endreq;
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}
