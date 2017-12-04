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
JetCollectionCnv::createPersistent( JetCollection* ) 
{
   MsgStream msg( msgSvc(), "JetCollectionCnv" );
   msg << MSG::ERROR << "createPersistent() is obsolete" << endmsg;
   return nullptr;
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

  msg << MSG::DEBUG << "  JetCollectionCnv::createTransient() "<< endmsg;

  static pool::Guid tr_guid("2FB4D25D-E217-4B8A-B865-180DE73B20E0");
  static pool::Guid pj_guid("E97C0C61-0B3E-401C-B853-A3302168283E");
  static pool::Guid p1_guid("9B8CE942-1457-4392-A98A-A1A464990B9A");
  static pool::Guid p2_guid("0260D9A5-9C75-45FF-AC97-EA220FB11380");
  static pool::Guid p3_guid("8CC411B4-8369-4C61-9B15-8A806DBFBBD1");
  static pool::Guid tlp2_guid("668B769E-E190-4904-BE2C-1677279A9803");
  static pool::Guid tlp5_guid("BEDCAE36-5EFD-4035-A402-2DC4F7A6BF75");
  static pool::Guid tlp6_guid("77DFEE77-16A5-45DD-91FB-FDEEF2B65F7D");

  if ( compareClassGuid(tlp6_guid) ) {
    msg << MSG::DEBUG << "  JetCollectionCnv:  calling tlp6 converter" <<endmsg;
    poolReadObject<JetCollection_tlp6> (m_TPConverter);
    return m_TPConverter.createTransient(m_log);

  } else  if ( compareClassGuid(tlp5_guid) ) {
    msg << MSG::DEBUG << "  JetCollectionCnv:  calling tlp5 converter" <<endmsg;
    poolReadObject<JetCollection_tlp5> (m_tlp5_cnv);
    return transObj = m_tlp5_cnv.createTransient(m_log);

  } else if ( compareClassGuid(tlp2_guid) ) {

    // The T/P seperated object. JetCollection_PERS is not used here
    // as we want to read out specifically JetColleciton_tlp2, not just
    // the lastest guy.

    msg << MSG::DEBUG << "  JetCollectionCnv:  calling tlp2 converter" <<endmsg;
    poolReadObject<JetCollection_tlp2> (m_tlp2_cnv);
    return m_tlp2_cnv.createTransient(m_log);

//     poolReadObject<JetCollection_tlp2> (m_TPConverter);
//     return transObj = m_TPConverter.createTransient(m_log);

  } else if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<JetCollection>();

  } else if ( compareClassGuid(p1_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<JetCollection_p1> persObj( poolReadObject<JetCollection_p1>() );
    transObj = m_p1_cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p2_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<JetCollection_p2> persObj( poolReadObject<JetCollection_p2>() );
    transObj = m_p2_cnv.createTransient( persObj.get(), msg );
    
  } else if ( compareClassGuid(p3_guid) ) {

    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<JetCollection_p3> persObj( poolReadObject<JetCollection_p3>() );
    transObj = m_p3_cnv.createTransient( persObj.get(), msg );
    
  } else if ( compareClassGuid(pj_guid) ) {
    msg << MSG::DEBUG << " creating pj_guid "<< endmsg;
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<ParticleJetContainer_p1> persObj( poolReadObject<ParticleJetContainer_p1>() );
    transObj = m_pjp1_cnv.createTransient( persObj.get(), msg );
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

  ///
  /// Load the flavor tagging converter. The hard-coded CLID here is
  /// a run-time linkage to the JetTagInfo converter and collection.
  /// The must be the same, of course, for this to work.
  ///

  IConverter *converter = m_athenaPoolCnvSvc->converter(CLID(50623115));
  if (converter != 0) {
    registerExtendingCnv (converter);
  } else {
    m_log << MSG::FATAL << "Could not load the jet tag info converters for readback!" << endmsg;
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}
