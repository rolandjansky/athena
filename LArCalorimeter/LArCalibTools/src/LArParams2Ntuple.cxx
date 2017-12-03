/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArParams2Ntuple.h"
#include "CaloIdentifier/CaloGain.h"

using namespace LArParamsProperties ;

const unsigned LArParams2Ntuple::m_nClasses = LArParamsProperties::END_OF_LIST ;

LArParams2Ntuple::LArParams2Ntuple(const std::string& name, ISvcLocator* pSvcLocator)
 : LArCond2NtupleBase(name, pSvcLocator),
   m_completeCaliPulseParams(0), m_completeDetCellParams(0), m_completePhysCaliTdiff(0),
   m_completeTdrift(0), m_completeMphysOverMcal(0), m_completeRinj(0), m_completeTshaper(0),
   m_completeEMEC_Cphi(0), m_completeEMEC_HValpha(0), m_completeEMEC_HVbeta(0),
   m_completeCableLength(0), m_completeCableAttenuation(0), m_calibCaliPulseParams(0),
   m_completeOFCBin(0)
{ 
  //declareProperty("DumpAllOnlineChannels",m_dumpAllOnlineChannels=std::string("")) ;
  declareProperty("DetStoreSuffix",m_suffix=std::string("")) ;
  declareProperty("AllChannels2Ntuple",m_allChannels2Ntuple=false) ;
  declareProperty("UseAbstractInterface",m_useAbstractInterface=false) ;
  declareProperty("NtupleName",m_ntName="PARAMS");
  m_keylist.clear() ;
  declareProperty("KeyList",m_keylist);
  m_detStoreJo.clear();
  declareProperty("DBKeysList",m_detStoreJo);

  m_classNames = LArParamsProperties::ClassNames() ;

  // set default keywords for detector store retrieval

  m_detStoreKeys.resize(m_nClasses) ;
  for ( unsigned i=0 ; i<m_nClasses ; i++ ) 
    m_detStoreKeys[i] = LArParamsProperties::keyword( m_classNames[i] ) ;

}

LArParams2Ntuple::~LArParams2Ntuple() 
{}

StatusCode LArParams2Ntuple::initialize() {

  m_ntTitle=m_ntName;
  m_ntpath=std::string("/NTUPLES/FILE1/")+m_ntName;

  
  if ( m_classNames.size() != m_nClasses ) { // should never happen! but just to be sure...
    msg(MSG::FATAL) << "List of class names does not match foreseen number of classes, cannot go on!" << endmsg ;
    return StatusCode::FAILURE ;
  }
 
  for ( unsigned i=0 ; i<m_keylist.size() ; i++ ) {
    ATH_MSG_DEBUG("examinimg key " << m_keylist[i] << "...");
    unsigned idx = LArParamsProperties::getClassIndex(m_keylist[i]) ;
    ATH_MSG_DEBUG("... class index " << idx);
    if ( idx >= m_nClasses ) {
      ATH_MSG_DEBUG("key " << m_keylist[i] << " does not correspond to any foreseen class");
      return StatusCode::FAILURE ;
    } else {
      msg(MSG::INFO) << "will dump " << m_keylist[i] << " to Ntuple" << endmsg ;
      m_dumpFlags.set(idx) ;
    }
  }

  if ( m_useAbstractInterface ) {
    msg(MSG::INFO) << "All parameters will be accessed through abstract interface" << endmsg ;
    if ( ! m_allChannels2Ntuple ) {
      msg(MSG::WARNING) << "This will force dumping to Ntuple all foreseen online channels!" << endmsg ;
      m_allChannels2Ntuple = true ;
    }
  } else {
    msg(MSG::INFO) << "All parameters will be accessed through their Complete/VsCalib class" << endmsg ;
  }
  if ( m_allChannels2Ntuple ) {
    msg(MSG::INFO) << "All foreseen online channels will be written to Ntuple" << endmsg ;
  } else { 
    msg(MSG::INFO) << "Only channels with sensible parameters will be written to Ntuple (default)" << endmsg ;
  }

  return LArCond2NtupleBase::initialize();
} 
  

StatusCode LArParams2Ntuple::stop() {

  
 // ntuple handling:
  //------------------
  NTuple::Item<long> ntcellIndex;
  NTuple::Item<long> ntflag, ntgain;

  // .................... CaliPulseParams:
  NTuple::Item<double> ntTcal, ntFstep, ntOffset, ntdTimeCal ;
  NTuple::Item<long> ntnCB ;
  // .................... DetCellParams:
  NTuple::Item<double> ntOmega0, ntTaur ;
  // .................... PhysCaliTdiff, Tdrift:
  NTuple::Item<double> ntTdiff,  ntTdrift ;
  // .................... MphysOverMcal:
  NTuple::Item<double> ntMphysOverMcal ;
  // .................... Rinj, Tshaper:
  NTuple::Item<double> ntRinj, ntTshaper ;
  // .................... EMEC stuff:
  NTuple::Item<double> ntEMEC_Cphi, ntEMEC_HValpha, ntEMEC_HVbeta ;
  // .................... Cable stuff:
  NTuple::Item<double> ntCableLength, ntCableAttenuation ;
  //                      OFCBin
  NTuple::Item<long> ntbin;

  StatusCode sc=m_nt->addItem("cellIndex",ntcellIndex,0,2000);
  if (sc!=StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "addItem 'Cell Index' failed" << endmsg;
    return StatusCode::FAILURE;
  }
  sc=m_nt->addItem("gain",ntgain,-1,2);
  if (sc!=StatusCode::SUCCESS) {
    msg(MSG::ERROR) << "addItem 'Gain' failed" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( m_dumpFlags[CaliPulseParamsComplete] || m_dumpFlags[CaliPulseParamsVsCalib] ) {
    sc=m_nt->addItem("Tcal",ntTcal);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Tcal' failed" << endmsg;
      return StatusCode::FAILURE;
    }
    sc=m_nt->addItem("Fstep",ntFstep);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Fstep' failed" << endmsg;
      return StatusCode::FAILURE;
    }
    sc=m_nt->addItem("Offset",ntOffset);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Offset' failed" << endmsg;
      return StatusCode::FAILURE;
    }
    sc=m_nt->addItem("dTimeCal",ntdTimeCal);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'dTimeCal' failed" << endmsg;
      return StatusCode::FAILURE;
    }
    sc=m_nt->addItem("nCB",ntnCB);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'nCB' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[DetCellParamsComplete] ) {
    sc=m_nt->addItem("Omega0",ntOmega0);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Omega0' failed" << endmsg;
      return StatusCode::FAILURE;
    }
    sc=m_nt->addItem("Taur",ntTaur);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Taur' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[PhysCaliTdiffComplete] ) {
    sc=m_nt->addItem("Tdiff",ntTdiff);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Tdiff' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[TdriftComplete] ) {
    sc=m_nt->addItem("Tdrift",ntTdrift);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Tdrift' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[MphysOverMcalComplete] ) {
    sc=m_nt->addItem("MphysOverMcal",ntMphysOverMcal);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'MphysOverMcal' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[RinjComplete] ) {
    sc=m_nt->addItem("Rinj",ntRinj);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Rinj' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[TshaperComplete] ) {
    sc=m_nt->addItem("Tshaper",ntTshaper);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'Tshaper' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[EMEC_CphiComplete] ) {
    sc=m_nt->addItem("EMEC_Cphi",ntEMEC_Cphi);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'EMEC_Cphi' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[EMEC_HValphaComplete] ) {
    sc=m_nt->addItem("EMEC_HValpha",ntEMEC_HValpha);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'EMEC_HValpha' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[EMEC_HVbetaComplete] ) {
    sc=m_nt->addItem("EMEC_HVbeta",ntEMEC_HVbeta);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'EMEC_HVbeta' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[CableLengthComplete] ) {
    sc=m_nt->addItem("CableLength",ntCableLength);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'CableLength' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[CableAttenuationComplete] ) {
    sc=m_nt->addItem("CableAttenuation",ntCableAttenuation);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'CableAttenuation' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_dumpFlags[OFCBinComplete] ) {
    sc=m_nt->addItem("OFCBin",ntbin);
    if (sc!=StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "addItem 'OFCBin' failed" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  //=======================================================================================================
  // dump smaller complete structures to a common Ntuple
  //=======================================================================================================

  //
  // first, scan all complete data structures to collect a map of channels
  //

  int nGains = (int)CaloGain::LARNGAIN ;

  m_dump_flags_map.resize(nGains) ;  // map of channels to be dumped; resized to host 3 gains

  if ( m_allChannels2Ntuple ) {

    // *** all readout channels will go into the ntuple ***

    if ( m_dumpFlags[CaliPulseParamsComplete] ) {
      sc = retrieveFromDetStore(m_completeCaliPulseParams) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(CaliPulseParamsComplete) ;
    }
    if ( m_dumpFlags[DetCellParamsComplete] ) {
      sc = retrieveFromDetStore(m_completeDetCellParams) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(DetCellParamsComplete) ;
    }
    if ( m_dumpFlags[PhysCaliTdiffComplete] ) {
      sc = retrieveFromDetStore(m_completePhysCaliTdiff) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(PhysCaliTdiffComplete) ;
    }
    if ( m_dumpFlags[TdriftComplete] ) {
      sc = retrieveFromDetStore(m_completeTdrift) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(TdriftComplete) ;
    }
    if ( m_dumpFlags[MphysOverMcalComplete] ) {
      sc = retrieveFromDetStore(m_completeMphysOverMcal) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(MphysOverMcalComplete) ;
    }
    if ( m_dumpFlags[RinjComplete] ) {
      sc = retrieveFromDetStore(m_completeRinj) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(RinjComplete) ;
    }
    if ( m_dumpFlags[TshaperComplete] ) {
      sc = retrieveFromDetStore(m_completeTshaper) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(TshaperComplete) ;
    }
    if ( m_dumpFlags[EMEC_CphiComplete] ) {
      sc = retrieveFromDetStore(m_completeEMEC_Cphi) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(EMEC_CphiComplete) ;
    }
    if ( m_dumpFlags[EMEC_HValphaComplete] ) {
      sc = retrieveFromDetStore(m_completeEMEC_HValpha) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(EMEC_HValphaComplete) ;
    }
    if ( m_dumpFlags[EMEC_HVbetaComplete] ) {
      sc = retrieveFromDetStore(m_completeEMEC_HVbeta) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(EMEC_HVbetaComplete) ;
    }
    if ( m_dumpFlags[CableLengthComplete] ) {
      sc = retrieveFromDetStore(m_completeCableLength) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(CableLengthComplete) ;
    }
    if ( m_dumpFlags[CableAttenuationComplete] ) {
      sc = retrieveFromDetStore(m_completeCableAttenuation) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(CableAttenuationComplete) ;
    }
    if ( m_dumpFlags[CaliPulseParamsVsCalib] ) {
      sc = retrieveFromDetStore(m_calibCaliPulseParams) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(CaliPulseParamsVsCalib) ;
    }
    if ( m_dumpFlags[OFCBinComplete] ) {
      sc = retrieveFromDetStore(m_completeOFCBin) ;
      if ( sc.isFailure() )  m_dumpFlags.clear(OFCBinComplete) ;
    }

    std::vector<HWIdentifier>::const_iterator chIt    =  m_onlineId->channel_begin() ;
    std::vector<HWIdentifier>::const_iterator chItEnd =  m_onlineId->channel_end() ;
    for ( ; chIt != chItEnd ; chIt++ ) {
      HWIdentifier chid = *chIt ;
      for ( unsigned gain=0 ; (int)gain<nGains ; ++gain ) {
	DumpFlags & flags = m_dump_flags_map[gain][chid] ;
	flags = m_dumpFlags ;
      }
    }
    if ( ! m_isSC ) {
    std::vector<HWIdentifier>::const_iterator chCalibIt    =  ((LArOnlineID*)m_onlineId)->calib_channel_begin() ;
    std::vector<HWIdentifier>::const_iterator chCalibItEnd =  ((LArOnlineID*)m_onlineId)->calib_channel_end() ;
    for ( ; chCalibIt != chCalibItEnd ; chCalibIt++ ) {
       HWIdentifier chid = *chCalibIt ;
      for ( unsigned gain=0 ; (int)gain<nGains ; ++gain ) {
	DumpFlags & flags = m_dump_flags_map[gain][chid] ;
	flags = m_dumpFlags ;
      }
    }
    }

  } else {

    // *** scan all complete data structures to collect a map of valid channels ***

    for ( unsigned i=0 ; i<m_nClasses ; i++ ) {
      if ( ! m_dumpFlags[i] )  continue ;
      ATH_MSG_DEBUG(m_classNames[i] << " was selected to be dumped...");
      
      switch ( i ) {
      case CaliPulseParamsComplete: {
	sc = scanReadoutChannels(m_completeCaliPulseParams) ; 
	break ;
      }
      case DetCellParamsComplete: {
	sc = scanReadoutChannels(m_completeDetCellParams) ;
	break ;
      }
      case PhysCaliTdiffComplete: {
	sc = scanReadoutChannels(m_completePhysCaliTdiff) ; 
	break ;
      }
      case TdriftComplete: {
	sc = scanReadoutChannels(m_completeTdrift) ;
	break ;
      }
      case MphysOverMcalComplete: {
	sc = scanReadoutChannels(m_completeMphysOverMcal) ;
	break ;
      }
      case RinjComplete: {
	sc = scanReadoutChannels(m_completeRinj) ;
	break ;
      }
      case TshaperComplete: {
	sc = scanReadoutChannels(m_completeTshaper) ;
	break ;
      }
      case EMEC_CphiComplete: {
	sc = scanReadoutChannels(m_completeEMEC_Cphi) ;
	break ;
      }
      case EMEC_HValphaComplete: {
	sc = scanReadoutChannels(m_completeEMEC_HValpha) ;
	break ;
      }
      case EMEC_HVbetaComplete: {
	sc = scanReadoutChannels(m_completeEMEC_HVbeta) ;
	break ;
      }
      case CableLengthComplete: {
	sc = scanReadoutChannels(m_completeCableLength) ;
	break ;
      }
      case CableAttenuationComplete: {
	sc = scanReadoutChannels(m_completeCableAttenuation) ;
	break ;
      }
      case CaliPulseParamsVsCalib: {
	sc = scanCalibChannels(m_calibCaliPulseParams) ; 
	break ;
      }
      case OFCBinComplete: {
	sc = scanReadoutChannels(m_completeOFCBin) ; 
	break ;
      }
      }  // end switch 

    } // end of the channels scan

  }

  //
  // ... then fill the ntuple
  //

  for ( unsigned gain=0 ; (int)gain<nGains ; gain++) {

    std::map< HWIdentifier,DumpFlags >::const_iterator map_it   = m_dump_flags_map[gain].begin() ;
    std::map< HWIdentifier,DumpFlags >::const_iterator map_it_e = m_dump_flags_map[gain].end() ;

    for ( ; map_it!=map_it_e ; map_it++ ) {

      HWIdentifier chid = HWIdentifier(map_it->first) ;

      const std::vector<bool> & flags = (map_it->second).flags() ;
      if ( flags.size() <= 0 ) continue ;
      if ( flags.size() < m_nClasses ) {  // should never happen...
	msg(MSG::WARNING) << "... flags vector shorter than " << m_nClasses << ": " << flags.size() << endmsg ;
	continue ;
      }

      fillFromIdentifier(chid); //Fill common values by base-class

      if ( flags[CaliPulseParamsComplete] ) {
	ntTcal     = m_completeCaliPulseParams->Tcal(chid,gain) ;
	ntFstep    = m_completeCaliPulseParams->Fstep(chid,gain) ;
	ntOffset   = m_completeCaliPulseParams->Offset(chid,gain) ;
	ntdTimeCal = m_completeCaliPulseParams->dTimeCal(chid,gain) ;
	ntnCB      = m_completeCaliPulseParams->nCB(chid,gain) ;

      }
      if ( flags[DetCellParamsComplete] ) {
	ntOmega0 = m_completeDetCellParams->Omega0(chid,gain) ;
	ntTaur   = m_completeDetCellParams->Taur(chid,gain) ;
      }
      if ( flags[PhysCaliTdiffComplete] ) {
	ntTdiff  = m_completePhysCaliTdiff->Tdiff(chid,gain) ;
      }
      if ( flags[TdriftComplete] ) {
	ntTdrift = m_completeTdrift->Tdrift(chid) ;
      }
      if ( flags[MphysOverMcalComplete] ) {
	ntMphysOverMcal = m_completeMphysOverMcal->MphysOverMcal(chid,gain) ;
      }
      if ( flags[RinjComplete] ) {
	ntRinj = m_completeRinj->Rinj(chid) ;
      }
      if ( flags[TshaperComplete] ) {
	ntTshaper = m_completeTshaper->Tshaper(chid) ;
      }
      if ( flags[EMEC_CphiComplete] ) {
	ntEMEC_Cphi = m_completeEMEC_Cphi->EMEC_Cphi(chid) ;
      }
      if ( flags[EMEC_HValphaComplete] ) {
	ntEMEC_HValpha = m_completeEMEC_HValpha->EMEC_HValpha(chid) ;
      }
      if ( flags[EMEC_HVbetaComplete] ) {
	ntEMEC_HVbeta = m_completeEMEC_HVbeta->EMEC_HVbeta(chid) ;
      }
      if ( flags[CableLengthComplete] ) {
	ntCableLength = m_completeCableLength->CableLength(chid) ;
      }
      if ( flags[CableAttenuationComplete] ) {
	ntCableAttenuation = m_completeCableAttenuation->CableAttenuation(chid) ;
      }

      if ( flags[CaliPulseParamsVsCalib] ) {
	ntTcal     = m_calibCaliPulseParams->Tcal(chid,gain) ;
	ntFstep    = m_calibCaliPulseParams->Fstep(chid,gain) ;
	ntOffset   = m_calibCaliPulseParams->Offset(chid,gain) ;
	ntdTimeCal = m_calibCaliPulseParams->dTimeCal(chid,gain) ;
	ntnCB      = m_calibCaliPulseParams->nCB(chid,gain) ;
      }

      if ( flags[OFCBinComplete] ) {
	ntbin  = m_completeOFCBin->bin(chid,gain) ;
      }
      sc=ntupleSvc()->writeRecord(m_nt);
      if (sc!=StatusCode::SUCCESS) {
	msg(MSG::ERROR) << "writeRecord failed" << endmsg;
	return StatusCode::FAILURE;
      }      
      ATH_MSG_DEBUG("... record written to ntuple");
    }  // end channel loop
  }  // end gain loop

ATH_MSG_INFO("LArParams2Ntuple has finished.");
  return StatusCode::SUCCESS;
}// end stop-method.
   

template < class DATA > 
StatusCode LArParams2Ntuple::scanReadoutChannels( const DATA*& data_object ) {

  unsigned classIndex = LArParamsProperties::getClassIndex(data_object) ;
  bool     useGain    = LArParamsProperties::isGainDependent(data_object) ;

  if ( classIndex >= m_nClasses ) {
    msg(MSG::ERROR) << "Class index " << classIndex << " beyond number of known classes: " << m_nClasses << endmsg ;
    return StatusCode::FAILURE ;
  }

  std::string detStoreKey;
  if (m_detStoreJo.size() > 0 && m_detStoreJo[classIndex] != "") {
    detStoreKey = m_detStoreJo[classIndex];
    ATH_MSG_VERBOSE(classIndex<<" detStoreKey = "<<detStoreKey);
  } else {
    detStoreKey =  m_detStoreKeys[classIndex] + m_suffix ;
  }

  std::string dataName    =  m_classNames[classIndex] ;

  if ( dataName.substr(dataName.length()-8,8) != std::string("Complete") ) {
    msg(MSG::ERROR) << "Function scanReadoutChannels cannot be called for data class " << dataName << endmsg ;
    return StatusCode::FAILURE ;
  }

  ATH_MSG_VERBOSE("Trying to retrieve " << dataName << " from detector store with key="<< detStoreKey << " ...");

  StatusCode sc = m_detStore->retrieve(data_object,detStoreKey) ;
  if ( sc == StatusCode::FAILURE ) {
    ATH_MSG_VERBOSE("... failed! Trying without key ...");
    StatusCode sc = m_detStore->retrieve(data_object) ;
  }

  if ( sc == StatusCode::FAILURE ) {
    msg(MSG::ERROR) << "Could not retrieve " << dataName << " from detector store!" << endmsg ;
    return sc ;
  } 

  // loop through the data object and keep memory of valid items

  ATH_MSG_VERBOSE("... " << dataName << " retrieved");

  unsigned nGains = (int)CaloGain::LARNGAIN ;
  if ( ! useGain ) nGains = 1 ;  // data are gain-independent

  for ( unsigned gain=0 ; gain<nGains ; gain++) {
    ATH_MSG_VERBOSE("... parsing gain " << gain);
    typename DATA::ConstConditionsMapIterator it   = data_object->begin(gain) ;
    typename DATA::ConstConditionsMapIterator it_e = data_object->end(gain) ;
    for ( ; it!=it_e ; it++ ) {
      HWIdentifier chid =  m_onlineId->channel_Id( HWIdentifier(it.getFebId()) , it.getChannel() ) ;
      ATH_MSG_VERBOSE("    ... channel " << std::hex << chid.get_compact()<< std::dec);
      if ( LArParamsProperties::isValid( data_object->get(chid,gain) ) ) {
	if ( useGain ) {
	  DumpFlags & flags = m_dump_flags_map[gain][chid] ;
	  flags.set(classIndex) ;
	} else {
	  for ( unsigned g=0 ; g<3 ; g++ ) {
	    DumpFlags & flags = m_dump_flags_map[g][chid] ;
	    flags.set(classIndex) ;
	  }
	}
      } else {
	ATH_MSG_VERBOSE("        (looks empty!)");
      }  // endif isValid
    }  // end loop over channels
  }  // end loop over gains
  return sc ;

} 

template < class DATA > 
StatusCode LArParams2Ntuple::scanCalibChannels( const DATA*& data_object ) {

  unsigned classIndex = LArParamsProperties::getClassIndex(data_object) ;
  //bool     useGain    = LArParamsProperties::isGainDependent(data_object) ;
  if ( classIndex >= m_nClasses ) {
    msg(MSG::ERROR) << "Class index " << classIndex << " beyond number of known classes: " << m_nClasses << endmsg ;
    return StatusCode::FAILURE ;
  }

  std::string detStoreKey =  m_detStoreKeys[classIndex] + m_suffix ;
  std::string dataName    =  m_classNames[classIndex] ;

  if ( dataName.substr(dataName.length()-7,7) != std::string("VsCalib") ) {
    msg(MSG::ERROR) << "Function scanCalibChannels cannot be called for data class " << dataName << endmsg ;
    return StatusCode::FAILURE ;
  }

  ATH_MSG_VERBOSE("Trying to retrieve " << dataName << " from detector store with key="<< detStoreKey << " ...");

  StatusCode sc = m_detStore->retrieve(data_object,detStoreKey) ;
  if ( sc == StatusCode::FAILURE ) {
    ATH_MSG_VERBOSE("... failed! Trying without key ...");
    StatusCode sc = m_detStore->retrieve(data_object) ;
  }

  if ( sc == StatusCode::FAILURE ) {
    msg(MSG::ERROR) << "Could not retrieve " << dataName << " from detector store!" << endmsg ;
    return sc ;
  } 

  // loop through the data object and keep memory of valid items

  ATH_MSG_VERBOSE("... " << dataName << " retrieved");
  ATH_MSG_DEBUG(dataName << " successfully retrieved!");
  // data_object->initialize() ;

  typename DATA::Const_CB_It cb_it   = data_object->begin() ;
  typename DATA::Const_CB_It cb_it_e = data_object->end() ;
  for ( ; cb_it!=cb_it_e ; cb_it++ ) { // loop through calibration boards
    const typename DATA::CB_Tvec_pair & cb_params_set = *cb_it ;
    const typename DATA::CB_Id &        cb_identifier = cb_params_set.first ;
    const typename DATA::T_vector &     cb_params     = cb_params_set.second ;
    HWIdentifier cb_HWid(cb_identifier) ;
    unsigned nchan = cb_params.size() ;
    if (msgLvl(MSG::VERBOSE)) {
      unsigned b_ec =  m_onlineId->barrel_ec(cb_HWid) ;
      unsigned p_n  =  m_onlineId->pos_neg(cb_HWid) ;
      unsigned ft   =  m_onlineId->feedthrough(cb_HWid) ;
      unsigned slot =  m_onlineId->slot(cb_HWid) ;
      msg(MSG::VERBOSE) << "    ... B/EC=" << b_ec << " P/N=" << p_n << " FT=" << ft << " slot=" << slot 
			<< " nchan=" << nchan << endmsg ;
    }//end if verbose

    for ( unsigned ichan=0 ; ichan<nchan ; ichan++ ) { // loop through channels in a CB
      if ( !m_isSC ) {
      HWIdentifier chid =  ((LArOnlineID*)m_onlineId)->calib_channel_Id(cb_HWid , ichan) ;
      ATH_MSG_VERBOSE("    ... calib channel " << chid);
      if ( LArParamsProperties::isValid( data_object->get(chid) ) ) {
	for ( unsigned g=0 ; g<3 ; g++ ) {
	  DumpFlags & flags = m_dump_flags_map[g][chid] ;
	  flags.set(classIndex) ;
	}
      }
      } // end of m_isSC
    }

  }  // end loop over calib boards
  return sc ;

} 

template < class DATA > 
StatusCode LArParams2Ntuple::retrieveFromDetStore( const DATA*& data_object ) {
  //
  // data_object must be a concrete object (Complete or VsCalib)
  //
  unsigned classIndex = LArParamsProperties::getClassIndex(data_object) ;
  //bool     useGain    = LArParamsProperties::isGainDependent(data_object) ;
  if ( classIndex >= m_nClasses ) {
    msg(MSG::ERROR) << "Class index " << classIndex << " beyond number of known classes: " << m_nClasses << endmsg ;
    return StatusCode::FAILURE ;
  }
  std::string detStoreKey =  m_detStoreKeys[classIndex] + m_suffix ;
  std::string dataName    =  m_classNames[classIndex] ;
  if ( m_useAbstractInterface ) {

    ATH_MSG_VERBOSE("Trying to retrieve " << dataName << " from detector store through abstract interface I" << detStoreKey << " ...");
    StatusCode sc = retrieveAbstractInterface(data_object) ;
    if ( sc == StatusCode::FAILURE ) {
      msg(MSG::WARNING) << "Could not retrieve " << dataName << " from detector store!" << endmsg ;
      return sc ;
    }

  } else {

    ATH_MSG_VERBOSE("Trying to retrieve "<<dataName<<" from detector store with key="<<detStoreKey<<" ...");
    StatusCode sc = m_detStore->retrieve(data_object,detStoreKey) ;
    if ( sc == StatusCode::FAILURE ) {
      ATH_MSG_VERBOSE("... failed! Trying without key ...");
      StatusCode sc = m_detStore->retrieve(data_object) ;
    }
    if ( sc == StatusCode::FAILURE ) {
      msg(MSG::WARNING) << "Could not retrieve " << dataName << " from detector store!" << endmsg ;
      return sc ;
    } 

  }
  ATH_MSG_INFO(dataName << " retrieved successfully from det store");
  return StatusCode::SUCCESS ;
}


inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArCaliPulseParamsComplete*& data_object) {
  const ILArCaliPulseParams* abstract_object ;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArCaliPulseParamsComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArCaliPulseParamsVsCalib*& data_object) {
  const ILArCaliPulseParams* abstract_object ;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArCaliPulseParamsVsCalib*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArDetCellParamsComplete*& data_object) {
  const ILArDetCellParams* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArDetCellParamsComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArPhysCaliTdiffComplete*& data_object) {
  const ILArPhysCaliTdiff* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArPhysCaliTdiffComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArTdriftComplete*& data_object) {
  const ILArTdrift* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArTdriftComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArMphysOverMcalComplete*& data_object) {
  const ILArMphysOverMcal* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArMphysOverMcalComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArRinjComplete*& data_object) {
  const ILArRinj* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArRinjComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArTshaperComplete*& data_object) {
  const ILArTshaper* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArTshaperComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArEMEC_CphiComplete*& data_object) {
  const ILArEMEC_Cphi* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArEMEC_CphiComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArEMEC_HValphaComplete*& data_object) {
  const ILArEMEC_HValpha* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArEMEC_HValphaComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArEMEC_HVbetaComplete*& data_object) {
  const ILArEMEC_HVbeta* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArEMEC_HVbetaComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArCableLengthComplete*& data_object) {
  const ILArCableLength* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArCableLengthComplete*>(abstract_object) ;
  return sc ;
}
inline StatusCode LArParams2Ntuple::retrieveAbstractInterface(const LArCableAttenuationComplete*& data_object) {
  const ILArCableAttenuation* abstract_object = nullptr;
  StatusCode sc = m_detStore->retrieve(abstract_object) ;
  data_object = dynamic_cast<const LArCableAttenuationComplete*>(abstract_object) ;
  return sc ;
}


/*
void LArParams2Ntuple::dumpChannels() 
{
  MsgStream log(msgSvc(), name());
  if ( m_dumpAllOnlineChannels == std::string("") )  return ;
  //
  FILE* f = fopen( m_dumpAllOnlineChannels.c_str() , "w" ) ;
  if ( f == NULL ) {
    msg(MSG::WARNING) << "Could not open file " << m_dumpAllOnlineChannels << endmsg ;
    msg(MSG::WARNING) << "... channels dump cannot be done!" << endmsg ;
    return ;
  }
  std::string subdet , comment , descr ;
  //unsigned FT, slot, channel, b_ec, p_n ;
 
  std::vector<HWIdentifier>::const_iterator chIt    =  m_onlineId->channel_begin() ;
  std::vector<HWIdentifier>::const_iterator chItEnd =  m_onlineId->channel_end() ;
  msg(MSG::INFO) << "Dumping online read-out channels to file " << m_dumpAllOnlineChannels << endmsg; 
  for ( ; chIt != chItEnd ; chIt++ ) {
    writeToFile(*chIt,f) ;
  }
  std::vector<HWIdentifier>::const_iterator chCalibIt    =  m_onlineId->calib_channel_begin() ;
  std::vector<HWIdentifier>::const_iterator chCalibItEnd =  m_onlineId->calib_channel_end() ;
  msg(MSG::INFO) << "Dumping online calib channels to file " << m_dumpAllOnlineChannels << endmsg; 
  for ( ; chCalibIt != chCalibItEnd ; chCalibIt++ ) {
    writeToFile(*chCalibIt,f) ;
  }
  fclose(f) ;
}

void LArParams2Ntuple::writeToFile(const HWIdentifier & ch, FILE* f) {
  unsigned b_ec    =  m_onlineId->barrel_ec(ch) ;
  unsigned p_n     =  m_onlineId->pos_neg(ch) ;
  unsigned FT      =  m_onlineId->feedthrough(ch) ;
  unsigned slot    =  m_onlineId->slot(ch) ;
  unsigned channel =  m_onlineId->channel(ch) ;
  unsigned layer, region, ieta, iphi ;
  std::string descr   =  m_onlineId->show_to_string(ch) ;
  std::string subdet = "     " ;
  if (  m_onlineId->isEmBarrelOnline(ch) )   subdet = "EMB " ;
  if (  m_onlineId->isEmEndcapOnline(ch) )   subdet = "EMEC" ;
  if (  m_onlineId->isHecOnline(ch) )        subdet = "HEC " ;
  if (  m_onlineId->isFcalOnline(ch) )       subdet = "FCAL" ;
  std::string comment = "          " ;
  bool offlineOK = false ;
  layer = region = ieta = iphi = 99999 ;
  if (  m_onlineId->isCalibration(ch) ) {
    comment = "Calib Line" ;
  } else {
    int calibLine ;
    try {
      Identifier id = m_larCablingSvc->cnvToIdentifier(ch);
      if ( m_emId->is_lar_em(id) ) {
	layer  = m_emId->sampling(id) ;
	region = m_emId->region(id) ;
	ieta   = m_emId->eta(id) ;
	iphi   = m_emId->phi(id) ;
	offlineOK = true ;
      }
      const std::vector<HWIdentifier>& calibLineV = m_larCablingSvc->calibSlotLine(ch);
      std::vector<HWIdentifier>::const_iterator calibLineIt = calibLineV.begin();
      if ( calibLineIt != calibLineV.end() ) {
	calibLine =  m_onlineId->channel(*calibLineIt);
      } else {
	calibLine = -2 ;  // says not possible to find calib line
      }
    } catch ( LArID_Exception & except ) {
      calibLine = -2 ;
    }
    char comment_cstr[11] ;
    sprintf(comment_cstr,"pulse=%3.3d",calibLine) ;
    comment = std::string(comment_cstr) ;
    if ( offlineOK ) {
      char offline_cstr[30] ;
      sprintf(offline_cstr," L=%1d R=%1d eta=%3.3d phi=%3.3d",layer,region,ieta,iphi) ;
      comment += std::string(offline_cstr) ;
    }
  }
  if ( p_n == 0 ) {
    subdet = "-" + subdet ;
  } else { 
    subdet = "+" + subdet ;
  }
  fprintf(f, "HWId=%11.11d B/E=%2.2d P/N=%2.2d FT=%2.2d sl=%2.2d ch=%3.3d  det=%5s %50s %50s\n", 
	  ch.get_identifier32().get_compact(), b_ec, p_n, FT, slot, channel, 
          subdet.c_str(), comment.c_str(), descr.c_str() ) ;
}
*/
