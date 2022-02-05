/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CscRawDataContainerCnv.h"
#include <TString.h> // for Form

StatusCode CscRawDataContainerCnv::initialize() {
	StatusCode sc = CscRawDataContainerCnvBase::initialize();
	if (sc!=StatusCode::SUCCESS) return sc;
  std::cout << "CscRawDataContainerCnv::initialize" << std::endl;
  // pass the IMuonIdHelperSvc to the CscRawDataCnv (needed for p2/p3 to translate between geometrical and positional hashes)
  SmartIF<Muon::IMuonIdHelperSvc> idHelp = serviceLocator()->service<Muon::IMuonIdHelperSvc>( "MuonIdHelperSvc" );
  if (!idHelp.isValid()) throw std::runtime_error(Form("File: %s, Line: %d\nCscRawDataContainerCnv::initialize() - No valid MuonIdHelperSvc present (needed for channel hash conversion)", __FILE__, __LINE__));
  this->m_TPconverter.m_elementCnv.m_elementCnv.setMuonIdHelperSvc(idHelp.get());
  //       ^                ^            ^
  //       |                |            |
  //   TPCnvIDCont     TPCnvVector   CscRawDataCnv_p4
  
  // do the same for p1-p3
  m_converter_p1.m_elementCnv.m_elementCnv.setMuonIdHelperSvc(idHelp.get());
  m_converter_p2.m_elementCnv.m_elementCnv.setMuonIdHelperSvc(idHelp.get());
  m_converter_p3.m_elementCnv.m_elementCnv.setMuonIdHelperSvc(idHelp.get());
  return StatusCode::SUCCESS;
}

CscRawDataContainer* CscRawDataContainerCnv::createTransient()
{
  MsgStream log(msgSvc(), "CscRawDataContainerCnv" );
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << " **** Entered createTransient() "<< endmsg;

  // the use of auto pointers ensures that the persistent object is deleted
  // using the correct persistent type pointer
  
  CscRawDataContainer *            trans_cont = nullptr;
  
  static pool::Guid p4_guid("F94905BF-D474-4453-B9D0-80ECB7BA4041");
  static pool::Guid p3_guid("A77330FB-BD12-4E50-829C-DADFBF556119");
  static pool::Guid p2_guid("19221A0D-4167-4A1C-BE2A-EE335D7C9D5F");
  static pool::Guid p1_guid("3586FE6B-0504-4E78-BD9F-AF839C50F931");
  static pool::Guid p0_guid("D7600810-31BC-4344-A3C6-9C59F47E5551");

  if( compareClassGuid(p4_guid) ) {
    // from this version onwards, the CSC identifier hashes are actually the position of the identifiers in the vector of identifiers
    // thus, no conversion inside CscRawDataCnv_p... is needed anymore
    std::unique_ptr< CscRawDataContainer_p4 > col_vect( poolReadObject< CscRawDataContainer_p4 >() );
    trans_cont = m_TPconverter.createTransient( col_vect.get(), log );
  } else if( compareClassGuid(p3_guid) ) {
    std::unique_ptr< CscRawDataContainer_p3 > col_vect( poolReadObject< CscRawDataContainer_p3 >() );
    trans_cont = m_converter_p3.createTransient( col_vect.get(), log );
  } else if( compareClassGuid(p2_guid) ) {
    std::unique_ptr< CscRawDataContainer_p2 > col_vect( poolReadObject< CscRawDataContainer_p2 >() );
    trans_cont = m_converter_p2.createTransient( col_vect.get(), log );
  } else if( compareClassGuid(p1_guid) ) {
    std::unique_ptr< CscRawDataContainer_p1 > col_vect( poolReadObject< CscRawDataContainer_p1 >() );
    trans_cont = m_converter_p1.createTransient( col_vect.get(), log );
  }
  else if( compareClassGuid(p0_guid) ) {
    // old version from before TP separation
    std::unique_ptr< COLL_vector >	col_vect( this->poolReadObject< COLL_vector >() );
    trans_cont = createTransientFrom_p0( col_vect.get(), log );
  }
  else {
    throw std::runtime_error("Unsupported persistent version of CSC RDO container");
  }
  return trans_cont;
}
