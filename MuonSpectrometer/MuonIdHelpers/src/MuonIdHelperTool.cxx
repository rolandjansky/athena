/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"


#include <iostream>

namespace Muon {

  MuonIdHelperTool::MuonIdHelperTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa), m_hasCSC(true), m_hasSTgc(true), m_hasMM(true)
  {
    declareInterface<MuonIdHelperTool>(this);
    declareProperty("HasCSC", m_hasCSC);
    declareProperty("HasSTgc", m_hasSTgc);
    declareProperty("HasMM", m_hasMM);
  }


  MuonIdHelperTool::~MuonIdHelperTool(){}


  StatusCode MuonIdHelperTool::initialize()  {

    if ( detStore()->retrieve( m_mdtIdHelper ).isFailure() ) {
      ATH_MSG_ERROR(" Cannot retrieve MdtIdHelper ");
      return StatusCode::FAILURE;
    }

    if (m_hasCSC) {
        if ( detStore()->retrieve( m_cscIdHelper ).isFailure() ) {
            ATH_MSG_WARNING(" Cannot retrieve CscIdHelper, please consider setting HasCSC property to false in the future when running a layout without CSC chambers");
            m_hasCSC = false;
            m_cscIdHelper = nullptr;
        }
    } else m_cscIdHelper = nullptr;
    if ( detStore()->retrieve( m_rpcIdHelper ).isFailure() ) {
      ATH_MSG_ERROR(" Cannot retrieve RpcIdHelper ");
      return StatusCode::FAILURE;
    }
    if ( detStore()->retrieve( m_tgcIdHelper ).isFailure() ) {
      ATH_MSG_ERROR(" Cannot retrieve TgcIdHelper ");
      return StatusCode::FAILURE;
    }
    if (m_hasSTgc) {
        if (detStore()->retrieve(m_stgcIdHelper).isFailure()) {
            ATH_MSG_WARNING(" Cannot retrieve sTgcIdHelper, please consider setting HasSTgc property to false in the future when running a layout without sTGC chambers");
            m_hasSTgc = false;
            m_stgcIdHelper = nullptr;
        }
    } else m_stgcIdHelper = nullptr;
    if (m_hasMM) {
        if ( detStore()->retrieve( m_mmIdHelper ).isFailure() ) {
            ATH_MSG_WARNING(" Cannot retrieve MmIdHelper, please consider setting HasMM property to false in the future when running a layout without MicroMegas chambers");
            m_hasMM = false;
            m_mmIdHelper = nullptr;
        }
    } else m_mmIdHelper = nullptr;
    
    if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " Technologies: size " << m_mdtIdHelper->technologyNameIndexMax();
    for( int tech=0; tech<=m_mdtIdHelper->technologyNameIndexMax();++tech ){
      std::string name = m_mdtIdHelper->technologyString(tech);
      if( name == "MDT" )  m_technologies.push_back(MuonStationIndex::MDT);
      if( name == "CSC" )  m_technologies.push_back(MuonStationIndex::CSCI);
      if( name == "RPC" )  m_technologies.push_back(MuonStationIndex::RPC);
      if( name == "TGC" )  m_technologies.push_back(MuonStationIndex::TGC);
      if( name == "STGC" ) m_technologies.push_back(MuonStationIndex::STGC);
      if( name == "MM" )   m_technologies.push_back(MuonStationIndex::MM);
      if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << ", " << tech << " " << name;
    }
    if( msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << endmsg;
    unsigned int nstationsNames = m_mdtIdHelper->stationNameIndexMax()+1;
    m_stationNameData.resize(nstationsNames);
    for( int i=0; i <= m_mdtIdHelper->stationNameIndexMax(); ++i){

      std::string name = m_mdtIdHelper->stationNameString(i);

      if(name.compare(MuonIdHelper::BAD_NAME)==0) continue;

      StationNameData& data = m_stationNameData[i];

      data.stationName = name;

      data.isEndcap = m_mdtIdHelper->isEndcap(i);
      data.isSmall  = m_mdtIdHelper->isSmall(i);

      data.chIndex = MuonStationIndex::ChUnknown;
      if( data.isEndcap ){
	if( data.stationName[1] == '1' )       data.chIndex = MuonStationIndex::EML;
	else if( data.stationName[1] == '2' )  data.chIndex = MuonStationIndex::EML;
	else if( data.stationName[1] == '3' )  data.chIndex = MuonStationIndex::EML;
	else if( data.stationName[1] == '4' )  data.chIndex = MuonStationIndex::EIL;

	if( data.stationName[1] == 'O' ){
	  if( data.stationName[2] == 'L' ) data.chIndex = MuonStationIndex::EOL;
	  else                             data.chIndex = MuonStationIndex::EOS;
	}else if( data.stationName[1] == 'M' ){
	  if( data.stationName[2] == 'L' )  data.chIndex = MuonStationIndex::EML;
	  else                              data.chIndex = MuonStationIndex::EMS;
	}else if( data.stationName[1] == 'I' ){
	  if( data.stationName[2] == 'L' ) data.chIndex = MuonStationIndex::EIL;
	  else                             data.chIndex = MuonStationIndex::EIS;
	}else if( data.stationName[1] == 'E' ){
	  if( data.stationName[0] == 'B' )   {
	    data.chIndex = MuonStationIndex::BEE;
	  }else{
	    if( data.stationName[2] == 'L' ) data.chIndex = MuonStationIndex::EEL;
	    else                             data.chIndex = MuonStationIndex::EES;
	  }
	}else if( data.stationName[0] == 'C' ) {
	  if( data.stationName[2] == 'L' ) data.chIndex = MuonStationIndex::CSL;
	  else                             data.chIndex = MuonStationIndex::CSS;
	}
	if( data.stationName[0] == 'S' || data.stationName[0] == 'M') {
	  if( data.isSmall ) data.chIndex = MuonStationIndex::EIS;
	  else               data.chIndex = MuonStationIndex::EIL;
	}

      }else{
	if( data.stationName[1] == 'O' )  {
	  if( data.stationName[2] == 'L' ) data.chIndex = MuonStationIndex::BOL;
	  else                             data.chIndex = MuonStationIndex::BOS;
	}else if( data.stationName[1] == 'M' )   {
	  if( data.stationName[2] == 'L' || data.stationName[2] == 'E' ) data.chIndex = MuonStationIndex::BML;
	  else                             data.chIndex = MuonStationIndex::BMS;
	}else if( data.stationName[1] == 'I' )  {
	  if( data.stationName[2] == 'L' || data.stationName[2] == 'M' || data.stationName[2] == 'R' ) data.chIndex = MuonStationIndex::BIL;
	  else                                                                                         data.chIndex = MuonStationIndex::BIS;
	}
      }

      data.stIndex = MuonStationIndex::toStationIndex(data.chIndex );

      if( msgLvl(MSG::DEBUG) ){
	msg(MSG::DEBUG) << "Adding station " << i << " " << data.stationName << " ";
	if( data.isEndcap ) msg(MSG::DEBUG) << " Endcap, ";
	else                msg(MSG::DEBUG) << " Barrel, ";
	if( data.isSmall )  msg(MSG::DEBUG) << " Small, ";
	else                msg(MSG::DEBUG) << " Large, ";

	msg(MSG::DEBUG) << MuonStationIndex::chName(data.chIndex) << "  " << MuonStationIndex::stName(data.stIndex) << endmsg;
      }
    }
    return StatusCode::SUCCESS;
  }

  StatusCode MuonIdHelperTool::finalize() {
    if( AthAlgTool::finalize().isFailure() ) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }

  int MuonIdHelperTool::gasGap( const Identifier& id ) const {
    if( isRpc(id) ) {
      return m_rpcIdHelper->gasGap(id);

    }else if( isTgc(id) ) {
      return m_tgcIdHelper->gasGap(id);

    }else if( isCsc(id) ){
      return m_cscIdHelper->wireLayer(id);

    }else if( issTgc(id) ){
      return m_stgcIdHelper->gasGap(id);

    }else if( isMM(id) ){
      return m_mmIdHelper->gasGap(id);
    }else{
      return m_mdtIdHelper->channel(id);
    }
    return 1;
  }

  bool MuonIdHelperTool::isMuon( const Identifier& id ) const {
    return m_mdtIdHelper->is_muon(id);
  }

  bool MuonIdHelperTool::isMdt( const Identifier& id ) const {
    if (!m_mdtIdHelper) return false;
    return m_mdtIdHelper->is_mdt(id);
  }

  bool MuonIdHelperTool::isMM( const Identifier& id ) const {
    if (!m_mmIdHelper) return false;
    return m_mmIdHelper->is_mm(id);
  }

  bool MuonIdHelperTool::isCsc( const Identifier& id ) const {
    if (!m_cscIdHelper) return false;
    return m_cscIdHelper->is_csc(id);
  }

  bool MuonIdHelperTool::isRpc( const Identifier& id ) const {
    if (!m_rpcIdHelper) return false;
    return m_rpcIdHelper->is_rpc(id);
  }

  bool MuonIdHelperTool::isTgc( const Identifier& id ) const {
    if (!m_tgcIdHelper) return false;
    return m_tgcIdHelper->is_tgc(id);
  }

  bool MuonIdHelperTool::issTgc( const Identifier& id ) const {
    if (!m_stgcIdHelper) return false;
    return m_stgcIdHelper->is_stgc(id);
  }

  bool MuonIdHelperTool::issMdt( const Identifier& id ) const {
    if (!isMdt(id)) return false;
    else if (m_mdtIdHelper->isEndcap(id)) return false; // there are no sMDTs in the endcaps
    else if (m_mdtIdHelper->isBME(id)) return true; // all BME chambers are sMDTs
    else if (m_mdtIdHelper->isBMG(id)) return true; // all BMG chambers are sMDTs
    else if (!m_rpcIdHelper) return false; // there must be RPCs in any layout
    bool sMdt = false;
    // now, let's check if we are in the inner barrel layer, and if there are RPCs installed
    // if yes, the MDT chambers must be sMDTs
    if (m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id)).find("BI")!=std::string::npos) {
      // now try to retrieve RPC identifier with the same station name/eta/phi and check if it is valid
      bool isValid = false;
      m_rpcIdHelper->elementID(m_mdtIdHelper->stationName(id), m_mdtIdHelper->stationEta(id), m_mdtIdHelper->stationPhi(id), 1, true, &isValid, true); // last 4 arguments are: doubletR, check, isValid, noPrint
      if (isValid) sMdt = true; // there is a BI RPC in the same station, thus, this station was already upgraded and sMDTs are present
    }
    return sMdt;
  }

  bool MuonIdHelperTool::hasHPTDC( const Identifier& id ) const {
    /** NOTE that in Run4, no HPTDCs at all are planned to be present any more,
        so this function should be obsolete from Run4 onwards */
    if (!isMdt(id)) return false; // only sMDT chambers can have HPTDCs
    else if (m_mdtIdHelper->isEndcap(id)) return false; // there are no HPTDCs in the endcaps
    else if (m_mdtIdHelper->isBME(id)) return false; // all BME sMDTs have no HPTDC
    else if (m_mdtIdHelper->isBMG(id)) return true; // all BMG sMDTs have an HPTDC
    // the remaining sMDTs (installed in BI in LS1) all have an HPTDC in Run3
    else if (issMdt(id)) return true;
    return false;
  }

  bool MuonIdHelperTool::measuresPhi( const Identifier& id ) const {
    if( isRpc(id) ) {
      return m_rpcIdHelper->measuresPhi(id);
    }else if( isTgc(id) ) {
      return m_tgcIdHelper->measuresPhi(id);
    }else if( isCsc(id) ){
      return m_cscIdHelper->measuresPhi(id);
    }else if( issTgc(id) ){
      return m_stgcIdHelper->measuresPhi(id);
    }
    // MM and MDTs only measure eta
    return false;
  }


  bool MuonIdHelperTool::isTrigger( const Identifier& id ) const {
    if( isRpc(id) ) return true;
    else if( isTgc(id) ) return true;
    return false;
  }

  bool MuonIdHelperTool::isEndcap( const Identifier& id ) const {
    return m_mdtIdHelper->isEndcap(id);
  }

  bool MuonIdHelperTool::isSmallChamber( const Identifier& id ) const {
    return m_mdtIdHelper->isSmall(id);
  }


  MuonStationIndex::ChIndex MuonIdHelperTool::chamberIndex( const Identifier& id ) const {
    if( !id.is_valid() || !isMuon(id) ){
      if( id.is_valid() ) ATH_MSG_WARNING("chamberIndex: invalid ID " << m_mdtIdHelper->print_to_string(id));
      return MuonStationIndex::ChUnknown;
    }
    return m_stationNameData[m_mdtIdHelper->stationName(id)].chIndex;
  }


  MuonStationIndex::StIndex MuonIdHelperTool::stationIndex( const Identifier& id ) const {
    if( !id.is_valid() || !isMuon(id) ){
      if( id.is_valid() ) ATH_MSG_WARNING("stationIndex: invalid ID " << m_mdtIdHelper->print_to_string(id));
      return MuonStationIndex::StUnknown;
    }
    return m_stationNameData[m_mdtIdHelper->stationName(id)].stIndex;
  }

  MuonStationIndex::PhiIndex MuonIdHelperTool::phiIndex( const Identifier& id ) const {
    if( !id.is_valid() || !isMuon(id) ){
      if( id.is_valid() ) ATH_MSG_WARNING("phiIndex: invalid ID " << m_mdtIdHelper->print_to_string(id));
      return MuonStationIndex::PhiUnknown;
    }
    if( isMdt(id) || isMM(id) ) {
      ATH_MSG_WARNING("phiIndex: not supported for " << toString(id));
      return MuonStationIndex::PhiUnknown;
    }
    MuonStationIndex::PhiIndex index = MuonStationIndex::PhiUnknown;
    MuonStationIndex::StIndex stIndex = stationIndex(id);
    if( stIndex == MuonStationIndex::BM ){
      if( rpcIdHelper().doubletR(id) == 1 )  index = MuonStationIndex::BM1;
      else                                   index = MuonStationIndex::BM2;
    }else if( stIndex == MuonStationIndex::BO ){
      if( rpcIdHelper().doubletR(id) == 1 )  index = MuonStationIndex::BO1;
      else                                   index = MuonStationIndex::BO2;
    }else if( stIndex == MuonStationIndex::EI ){
      if( isCsc(id) )      index = MuonStationIndex::CSC;
      else if( isTgc(id) ) index = MuonStationIndex::T4;
      else if( m_stgcIdHelper->multilayer(id) == 1 ) index = MuonStationIndex::STGC1;
      else index = MuonStationIndex::STGC2;
    }else if( stIndex == MuonStationIndex::EM ){
      std::string chamberName = chamberNameString(id);
      if( chamberName[1] == '1' )      index = MuonStationIndex::T1;
      else if( chamberName[1] == '2' ) index = MuonStationIndex::T2;
      else                             index = MuonStationIndex::T3;
    }
    return index;
  }
  
  MuonStationIndex::DetectorRegionIndex MuonIdHelperTool::regionIndex( const Identifier& id ) const {
    if( isEndcap(id) ) return stationEta(id) < 0 ? MuonStationIndex::EndcapC : MuonStationIndex::EndcapA;
    return MuonStationIndex::Barrel;
  }

  MuonStationIndex::LayerIndex MuonIdHelperTool::layerIndex( const Identifier& id ) const {
    return MuonStationIndex::toLayerIndex(stationIndex(id));
  }

  MuonStationIndex::TechnologyIndex MuonIdHelperTool::technologyIndex( const Identifier& id ) const {
    if( isMdt(id) )  return MuonStationIndex::MDT;
    if( isCsc(id) )  return MuonStationIndex::CSCI;
    if( isTgc(id) )  return MuonStationIndex::TGC;
    if( isRpc(id) )  return MuonStationIndex::RPC;
    if( issTgc(id) ) return MuonStationIndex::STGC;
    if( isMM(id) )   return MuonStationIndex::MM;
    return MuonStationIndex::TechnologyUnknown;//m_technologies[m_mdtIdHelper->technology(id)];
  }

  std::string MuonIdHelperTool::toString( const Identifier& id ) const  {
    std::ostringstream sout;
    if( !id.is_valid() ) return " Invalid Identifier";
    sout << toStringGasGap(id);
    if( isRpc(id) ) {
      sout << (m_rpcIdHelper->measuresPhi(id) ? " phi" : " eta") << " channel " << std::setw(2) << m_rpcIdHelper->channel(id);
    }else if( isTgc(id) ) {
      sout << (m_tgcIdHelper->measuresPhi(id)     ? " phi" : " eta") << " channel " << std::setw(2) << m_tgcIdHelper->channel(id);
    }else if( isCsc(id) ){
      sout << (m_cscIdHelper->measuresPhi(id) ? " phi" : " eta") << " channel " << std::setw(2) << m_cscIdHelper->channel(id);
    }else if( issTgc(id) ){
      int channelType = m_stgcIdHelper->channelType(id);
      if( channelType == 0 ) sout << " pad ";
      else if( channelType == 1 ) sout << " eta ";
      else if( channelType == 2 ) sout << " phi ";
      sout  << " channel " << std::setw(2) << m_stgcIdHelper->channel(id);
    }else if( isMM(id) ){
      sout << " channel " << std::setw(2) << m_mmIdHelper->channel(id);
    }
    return sout.str();
  }

  std::string MuonIdHelperTool::toStringTech( const Identifier& id ) const  {
    std::ostringstream sout;
    if( !id.is_valid() ) return " Invalid Identifier";
    if( isRpc(id) ) {
      sout  << m_rpcIdHelper->technologyString( m_rpcIdHelper->technology(id) );
    }else if( isTgc(id) ) {
      sout  << m_tgcIdHelper->technologyString( m_tgcIdHelper->technology(id) );
    }else if( isCsc(id) ){
      sout  << m_cscIdHelper->technologyString( m_cscIdHelper->technology(id) );
    }else if( issTgc(id) ){
      sout  << m_stgcIdHelper->technologyString( m_stgcIdHelper->technology(id) );
    }else if( isMM(id) ){
      sout  << m_mmIdHelper->technologyString( m_mmIdHelper->technology(id) );
    }else{
      sout   << m_mdtIdHelper->technologyString( m_mdtIdHelper->technology(id) );
    }
    return sout.str();

  }

  std::string MuonIdHelperTool::chamberNameString( const Identifier& id ) const  {
    return m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(id));
  }

  std::string MuonIdHelperTool::toStringStation( const Identifier& id ) const {
    std::ostringstream sout;
    if( !id.is_valid() ) return " Invalid Identifier";
    if( isRpc(id) ) {
      sout  << m_rpcIdHelper->technologyString( m_rpcIdHelper->technology(id) )
	    << " " << m_rpcIdHelper->stationNameString( m_rpcIdHelper->stationName(id) )
	    << " eta " << std::setw(2) << m_rpcIdHelper->stationEta(id)
	    << " phi " << std::setw(2) << m_rpcIdHelper->stationPhi(id);
    }else if( isTgc(id) ) {
      sout  << m_tgcIdHelper->technologyString( m_tgcIdHelper->technology(id) )
	    << " " << m_tgcIdHelper->stationNameString( m_tgcIdHelper->stationName(id) )
	    << " eta " << std::setw(2) << m_tgcIdHelper->stationEta(id)
	    << " phi " << std::setw(2) << m_tgcIdHelper->stationPhi(id);
    }else if( isCsc(id) ){
      sout  << m_cscIdHelper->technologyString( m_cscIdHelper->technology(id) )
	    << " " << m_cscIdHelper->stationNameString( m_cscIdHelper->stationName(id) )
	    << " eta " << std::setw(2) << m_cscIdHelper->stationEta(id)
	    << " phi " << std::setw(2) << m_cscIdHelper->stationPhi(id);
    }else if( isMM(id) ){
      sout  << m_mmIdHelper->technologyString( m_mmIdHelper->technology(id) )
	    << " " << m_mmIdHelper->stationNameString( m_mmIdHelper->stationName(id) )
	    << " eta " << std::setw(2) << m_mmIdHelper->stationEta(id)
	    << " phi " << std::setw(2) << m_mmIdHelper->stationPhi(id);
    }else if( issTgc(id) ){
      sout  << m_stgcIdHelper->technologyString( m_stgcIdHelper->technology(id) )
	    << " " << m_stgcIdHelper->stationNameString( m_stgcIdHelper->stationName(id) )
	    << " eta " << std::setw(2) << m_stgcIdHelper->stationEta(id)
	    << " phi " << std::setw(2) << m_stgcIdHelper->stationPhi(id);
    }else{
      sout   << m_mdtIdHelper->technologyString( m_mdtIdHelper->technology(id) )
	     << " " << m_mdtIdHelper->stationNameString( m_mdtIdHelper->stationName(id) )
	     << " eta " << std::setw(2) << m_mdtIdHelper->stationEta(id)
	     << " phi " << std::setw(2) << m_mdtIdHelper->stationPhi(id);
    }
    return sout.str();
  }

  std::string MuonIdHelperTool::toStringChamber( const Identifier& id ) const  {
    std::ostringstream sout;
    if( !id.is_valid() ) return " Invalid Identifier";
    if( isRpc(id) ) {
      sout  << m_rpcIdHelper->technologyString( m_rpcIdHelper->technology(id) )
	    << " " << m_rpcIdHelper->stationNameString( m_rpcIdHelper->stationName(id) )
	    << " eta " << std::setw(2) << m_rpcIdHelper->stationEta(id)
	    << " phi " << std::setw(2) << m_rpcIdHelper->stationPhi(id)
	    << " dbR " << m_rpcIdHelper->doubletR(id);
    }else if( isTgc(id) ) {
      sout  << m_tgcIdHelper->technologyString( m_tgcIdHelper->technology(id) )
	    << " " << m_tgcIdHelper->stationNameString( m_tgcIdHelper->stationName(id) )
	    << " eta " << std::setw(2) << m_tgcIdHelper->stationEta(id)
	    << " phi " << std::setw(2) << m_tgcIdHelper->stationPhi(id);
    }else if( isCsc(id) ){
      sout  << m_cscIdHelper->technologyString( m_cscIdHelper->technology(id) )
	    << " " << m_cscIdHelper->stationNameString( m_cscIdHelper->stationName(id) )
	    << " eta " << std::setw(2) << m_cscIdHelper->stationEta(id)
	    << " phi " << std::setw(2) << m_cscIdHelper->stationPhi(id);
    }else if( issTgc(id) ){
      sout  << m_stgcIdHelper->technologyString( m_stgcIdHelper->technology(id) )
	    << " " << m_stgcIdHelper->stationNameString( m_stgcIdHelper->stationName(id) )
	    << " eta " << std::setw(2) << m_stgcIdHelper->stationEta(id)
	    << " phi " << std::setw(2) << m_stgcIdHelper->stationPhi(id);
    }else if( isMM(id) ){
      sout  << m_mmIdHelper->technologyString( m_mmIdHelper->technology(id) )
	    << " " << m_mmIdHelper->stationNameString( m_mmIdHelper->stationName(id) )
	    << " eta " << std::setw(2) << m_mmIdHelper->stationEta(id)
	    << " phi " << std::setw(2) << m_mmIdHelper->stationPhi(id);
    }else{
      sout   << m_mdtIdHelper->technologyString( m_mdtIdHelper->technology(id) )
	     << " " << m_mdtIdHelper->stationNameString( m_mdtIdHelper->stationName(id) )
	     << " eta " << std::setw(2) << m_mdtIdHelper->stationEta(id)
	     << " phi " << std::setw(2) << m_mdtIdHelper->stationPhi(id);
    }
    return sout.str();
  }

  std::string MuonIdHelperTool::toStringDetEl( const Identifier& id ) const  {
    std::ostringstream sout;
    if( !id.is_valid() ) return " Invalid Identifier";
    if( isRpc(id) ) {
      sout  << toStringChamber(id)
	    << " dbZ " << m_rpcIdHelper->doubletZ(id)
	    << " dbPhi " << m_rpcIdHelper->doubletPhi(id);
    }else if( isTgc(id) ) {
      sout  << toStringChamber(id);
    }else if( isCsc(id) ){
      sout  << toStringChamber(id)
	    << " chlay " << m_cscIdHelper->chamberLayer(id);
    }else if( isMM(id) ){
      sout  << toStringChamber(id)
	    << " chlay " << m_mmIdHelper->multilayer(id);
    }else if( issTgc(id) ){
      sout  << toStringChamber(id)
	    << " chlay " << m_stgcIdHelper->multilayer(id);
    }else{
      sout << toStringChamber(id)
	   << " ml " << m_mdtIdHelper->multilayer(id);
    }
    return sout.str();
  }

  std::string MuonIdHelperTool::toStringGasGap( const Identifier& id ) const  {
    std::ostringstream sout;
    if( !id.is_valid() ) return " Invalid Identifier";
    if( isRpc(id) ) {
      sout  << toStringDetEl(id)
	    << " gap " << m_rpcIdHelper->gasGap(id);
    }else if( isTgc(id) ) {
      sout  << toStringDetEl(id)
	    << " gap " << m_tgcIdHelper->gasGap(id);
    }else if( isCsc(id) ){
      sout  << toStringDetEl(id)
	    << " lay " << m_cscIdHelper->wireLayer(id);
    }else if( issTgc(id) ){
      sout  << toStringDetEl(id)
	    << " lay " << m_stgcIdHelper->gasGap(id);
    }else if( isMM(id) ){
      sout  << toStringDetEl(id)
	    << " lay " << m_mmIdHelper->gasGap(id);
    }else{
      sout << toStringDetEl(id)
	   << " lay " << m_mdtIdHelper->tubeLayer(id) << " tube " << std::setw(2) << m_mdtIdHelper->channel(id);
    }
    return sout.str();
  }

  Identifier MuonIdHelperTool::chamberId( const Identifier& id) const  {
    Identifier chId;
    // use phi hits on segment
    if( isTgc(id) ){

      chId = m_tgcIdHelper->elementID(id);

    }else if( isRpc(id) ){

      chId = m_rpcIdHelper->elementID(id);

    }else if( isMM(id) ){

      chId = m_mmIdHelper->elementID(id);

    }else if( issTgc(id) ){

      chId = m_stgcIdHelper->elementID(id);

    }else if( isCsc(id) ){

      Identifier elId = m_cscIdHelper->elementID(id);
      chId = m_cscIdHelper->channelID(elId,2,1,1,1);

    }else{

      chId = m_mdtIdHelper->elementID(id);

    }
    return chId;
  }

  Identifier MuonIdHelperTool::detElId( const Identifier& id) const  {


    Identifier detElId;
    // use phi hits on segment
    if( isTgc(id) ){

      detElId = m_tgcIdHelper->elementID(id);

    }else if( isRpc(id) ){

      Identifier elId = m_rpcIdHelper->elementID(id);
      int doubZ = m_rpcIdHelper->doubletZ(id);
      int doubPhi = m_rpcIdHelper->doubletPhi(id);
      detElId = m_rpcIdHelper->channelID(elId,doubZ,doubPhi,1,0,1);

    }else if( isCsc(id) ){
      Identifier elId = m_cscIdHelper->elementID(id);
      detElId = m_cscIdHelper->channelID(elId,2,1,1,1);

    }else if( issTgc(id) ){
      Identifier elId = m_stgcIdHelper->elementID(id);
      detElId = m_stgcIdHelper->channelID(elId,m_stgcIdHelper->multilayer(id),1,1,1);

    }else if( isMM(id) ){
      Identifier elId = m_mmIdHelper->elementID(id);
      detElId = m_mmIdHelper->channelID(elId,m_mmIdHelper->multilayer(id),1,1);

    }else{
      Identifier elId = m_mdtIdHelper->elementID(id);
      detElId = m_mdtIdHelper->channelID(elId,m_mdtIdHelper->multilayer(id),1,1);
    }
    return detElId;
  }

  Identifier MuonIdHelperTool::layerId( const Identifier& id ) const  {
    Identifier layerId;
    // use phi hits on segment
    if( isTgc(id) ){

      Identifier elId = m_tgcIdHelper->elementID(id);
      int gasGap = m_tgcIdHelper->gasGap(id);
      int measuresPhi = m_tgcIdHelper->measuresPhi(id);
      layerId = m_tgcIdHelper->channelID(elId,gasGap,measuresPhi,1);

    }else if( isRpc(id) ){

      Identifier elId = m_rpcIdHelper->elementID(id);
      int doubZ = m_rpcIdHelper->doubletZ(id);
      int doubPhi = m_rpcIdHelper->doubletPhi(id);
      int gasGap = m_rpcIdHelper->gasGap(id);
      int measuresPhi = m_rpcIdHelper->measuresPhi(id);
      layerId = m_rpcIdHelper->channelID(elId,doubZ,doubPhi,gasGap,measuresPhi,1);

    }else if( isCsc(id) ){
      Identifier elId = m_cscIdHelper->elementID(id);
      int chLayer = m_cscIdHelper->chamberLayer(id);
      int wireLayer = m_cscIdHelper->wireLayer(id);
      int measuresPhi = m_cscIdHelper->measuresPhi(id);
      layerId = m_cscIdHelper->channelID(elId,chLayer,wireLayer,measuresPhi,1);

    }else if( isMM(id) ){
      Identifier elId = m_mmIdHelper->elementID(id);
      int chLayer = m_mmIdHelper->multilayer(id);
      int wireLayer = m_mmIdHelper->gasGap(id);
      layerId = m_mmIdHelper->channelID(elId,chLayer,wireLayer,1);

    }else if( issTgc(id) ){
      Identifier elId = m_stgcIdHelper->elementID(id);
      int chLayer = m_stgcIdHelper->multilayer(id);
      int wireLayer = m_stgcIdHelper->gasGap(id);
      layerId = m_stgcIdHelper->channelID(elId,chLayer,wireLayer,m_stgcIdHelper->channelType(id),1);

    }else{
      layerId = id;
    }
    return layerId;
  }

  Identifier MuonIdHelperTool::gasGapId( const Identifier& id ) const  {

    Identifier gasGapId;
    // use phi hits on segment
    if( isTgc(id) ){

      Identifier elId = m_tgcIdHelper->elementID(id);
      int gasGap = m_tgcIdHelper->gasGap(id);
      gasGapId = m_tgcIdHelper->channelID(elId,gasGap,0,1);

    }else if( isRpc(id) ){

      Identifier elId = m_rpcIdHelper->elementID(id);
      int doubZ = m_rpcIdHelper->doubletZ(id);
      int doubPhi = m_rpcIdHelper->doubletPhi(id);
      int gasGap = m_rpcIdHelper->gasGap(id);
      gasGapId = m_rpcIdHelper->channelID(elId,doubZ,doubPhi,gasGap,0,1);

    }else if( isCsc(id) ){
      Identifier elId = m_cscIdHelper->elementID(id);
      int chLayer = m_cscIdHelper->chamberLayer(id);
      int wireLayer = m_cscIdHelper->wireLayer(id);
      gasGapId = m_cscIdHelper->channelID(elId,chLayer,wireLayer,1,1);
    }else if( isMM(id) ){
      Identifier elId = m_mmIdHelper->elementID(id);
      int chLayer = m_mmIdHelper->multilayer(id);
      int wireLayer = m_mmIdHelper->gasGap(id);
      gasGapId = m_mmIdHelper->channelID(elId,chLayer,wireLayer,1);

    }else if( issTgc(id) ){
      Identifier elId = m_stgcIdHelper->elementID(id);
      int chLayer = m_stgcIdHelper->multilayer(id);
      int wireLayer = m_stgcIdHelper->gasGap(id);
      gasGapId = m_stgcIdHelper->channelID(elId,chLayer,wireLayer,1,1);

    }else{
      Identifier elId = m_mdtIdHelper->elementID(id);
      int ml = m_mdtIdHelper->multilayer(id);
      int lay = m_mdtIdHelper->tubeLayer(id);
      gasGapId = m_mdtIdHelper->channelID(elId,ml,lay,1);
    }
    return gasGapId;
  }

  int MuonIdHelperTool::stationPhi( const Identifier& id ) const {
    if( !id.is_valid() ) {
      ATH_MSG_WARNING("stationPhi: invalid ID");
      return 0;
    }
    if( isRpc(id) ) {
      return m_rpcIdHelper->stationPhi(id);
    }else if( isTgc(id) ) {
      return m_tgcIdHelper->stationPhi(id);
    }else if( isMdt(id) ){
      return m_mdtIdHelper->stationPhi(id);
    }else if( isCsc(id) ){
      return m_cscIdHelper->stationPhi(id);
    }else if( issTgc(id) ){
      return m_stgcIdHelper->stationPhi(id);
    }else if( isMM(id) ){
      return m_mmIdHelper->stationPhi(id);
    }
    return 0;
  }

  int MuonIdHelperTool::stationEta( const Identifier& id ) const {
    if( !id.is_valid() ) {
      ATH_MSG_WARNING("stationEta: invalid ID");
      return 0;
    }
    if( isRpc(id) ) {
      return m_rpcIdHelper->stationEta(id);
    }else if( isTgc(id) ) {
      return m_tgcIdHelper->stationEta(id);
    }else if( isMdt(id) ){
      return m_mdtIdHelper->stationEta(id);
    }else if( isCsc(id) ){
      return m_cscIdHelper->stationEta(id);
    }else if( issTgc(id) ){
      return m_stgcIdHelper->stationEta(id);
    }else if( isMM(id) ){
      return m_mmIdHelper->stationEta(id);
    }
    return 0;
  }

  int MuonIdHelperTool::sector( const Identifier& id ) const {
    // TGC has different segmentation, return 0 for the moment
    if( isTgc(id) ) {
      static std::vector<int> tgcSectorMapping;
      if( tgcSectorMapping.empty() ){
        std::vector<int>* mapping = 0;
        StatusCode sc = detStore()->retrieve(mapping,"TGC_SectorMapping");
        if( sc.isFailure() || !mapping ){
          ATH_MSG_WARNING("sector: failed to retrieve TGC sector mapping");
          return 0;
        }
        ATH_MSG_DEBUG("sector: retrieve TGC sector mapping " << mapping->size() );
        tgcSectorMapping = *mapping;
      }
      IdentifierHash hash;
      m_tgcIdHelper->get_module_hash(id,hash);
      if( hash >= tgcSectorMapping.size() ){
        ATH_MSG_WARNING("sector: TGC not yet supported");
        return 0;
      }
      return tgcSectorMapping[hash];
    }
    int sect = 2 * stationPhi( id );
    if( !isSmallChamber( id ) ) --sect;
    return sect;
  }
  bool MuonIdHelperTool::HasCSC() const {
    return m_hasCSC;
  }
  bool MuonIdHelperTool::HasSTgc() const {
    return m_hasSTgc;
  }
  bool MuonIdHelperTool::HasMM() const {
    return m_hasMM;
  }
}
