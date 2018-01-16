/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArWFParams2Ntuple.h"

#include "LArRawConditions/LArWFParamsContainer.h"
#include "LArRawConditions/LArWFParamsComplete.h"
#include "LArRawConditions/LArCaliPulseParamsComplete.h"
#include "LArRawConditions/LArDetCellParamsComplete.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"
#include "LArRawConditions/LArTdriftComplete.h"
#include "LArRawConditions/LArOFCBinComplete.h"

LArWFParams2Ntuple::LArWFParams2Ntuple(const std::string& name, ISvcLocator* pSvcLocator)
  : LArCond2NtupleBase(name, pSvcLocator)
{ 
  //declareProperty("UseContainers",m_useContainers=false) ;
  //declareProperty("UseComplete",m_useComplete=false) ;
  declareProperty("DumpCaliPulseParams",m_dumpCaliPulseParams=false);
  declareProperty("DumpDetCellParams"  ,m_dumpDetCellParams=false);
  declareProperty("DumpPhysCaliTdiff"  ,m_dumpPhysCaliTdiff=false);
  declareProperty("DumpTdrift"         ,m_dumpTdrift=false);
  declareProperty("DumpOFCBin"         ,m_dumpOFCBin=false);

  declareProperty("CaliPulseParamsKey",m_caliPulseParamsKey="LArCaliPulseParams");
  declareProperty("DetCellParamsKey"  ,m_detCellParamsKey="LArDetCellParams");
  declareProperty("PhysCaliTDiffKey"  ,m_physCaliTDiffKey="LArPhysCaliTDiff");
  declareProperty("TdriftKey"         ,m_tdriftKey="LArTdrift");
  declareProperty("OFCBinKey"         ,m_ofcBinKey="LArOFCBin");

  m_addFEBTemp=false;
}

LArWFParams2Ntuple::~LArWFParams2Ntuple() 
{}


StatusCode LArWFParams2Ntuple::initialize() {

 m_ntTitle="Parameters";
 m_ntpath="/NTUPLES/FILE1/PARAMS";
 return LArCond2NtupleBase::initialize();
}

StatusCode LArWFParams2Ntuple::stop() {

  //Retrieve objects from DetStore
  const LArCaliPulseParamsComplete* completeCaliPulseParams=0 ;
  const LArDetCellParamsComplete*   completeDetCellParams=0 ;
  const LArPhysCaliTdiffComplete*   completePhysCaliTdiff=0 ;
  const LArTdriftComplete*          completeTdrift=0;
  const LArOFCBinComplete*          completeOFCBin=0;


  if ( m_dumpCaliPulseParams ) { //try retrieving from det store
    ATH_MSG_DEBUG("Retrieving LArCaliPulseParamsComplete from detector store...");
    StatusCode sc = detStore()->retrieve(completeCaliPulseParams,m_caliPulseParamsKey) ;
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Could not retrieve LArCaliPulseParamsComplete from detector store!" ) ;
      m_dumpCaliPulseParams = false ;
    }
  }

  if ( m_dumpDetCellParams ) { //try retrieving from det store
    ATH_MSG_DEBUG("Retrieving LArDetCellParamsComplete from detector store...");
    StatusCode sc = detStore()->retrieve(completeDetCellParams,m_detCellParamsKey);
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Could not retrieve LArDetCellParamsComplete from detector store!" ) ;
      m_dumpDetCellParams = false;
    }
  } 
  

  if ( m_dumpPhysCaliTdiff ) { //try retrieving from det store
    ATH_MSG_DEBUG("Retrieving LArPhysCaliTdiffComplete from detector store...");
    StatusCode sc = detStore()->retrieve(completePhysCaliTdiff,m_physCaliTDiffKey);
    if ( sc.isFailure()) {
      ATH_MSG_ERROR( "Could not retrieve LArPhysCaliTdiffComplete from detector store!" ) ;
      m_dumpPhysCaliTdiff = false ;
    }
  }


  if ( m_dumpTdrift ) { //try retrieving from det store
    ATH_MSG_DEBUG("Retrieving LArTdriftComplete from detector store...");
    StatusCode sc = detStore()->retrieve(completeTdrift,m_tdriftKey) ;
    if ( sc.isFailure()) {
      ATH_MSG_ERROR( "Could not retrieve LArTdriftComplete from detector store!" );
      m_dumpTdrift = false ;
    }
  }

  if ( m_dumpOFCBin ) { //try retrieving from det store
    ATH_MSG_DEBUG("Retrieving LArOFCBinComplete from detector store...");
    StatusCode sc = detStore()->retrieve(completeOFCBin,m_ofcBinKey) ;
    if ( sc.isFailure()) {
      ATH_MSG_ERROR( "Could not retrieve LArOFCBinComplete from detector store!" );
      m_dumpOFCBin= false ;
    }
  }
  
  
  //Book ntuple item
  NTuple::Item<long> gain;
  NTuple::Item<double> Tcal, Fstep, Tdrift, Omega0, Taur, Tdiff,ofcbin;// Tshaper, Amplitude;

  StatusCode sc=m_nt->addItem("gain",gain,-1,2);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'Gain' failed" );
    return StatusCode::FAILURE;
  }


  if ( m_dumpCaliPulseParams ) {
    sc=m_nt->addItem("Tcal",Tcal);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Tcal' failed" );
      return StatusCode::FAILURE;
    }
    sc=m_nt->addItem("Fstep",Fstep);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Fstep' failed" );
      return StatusCode::FAILURE;
    }
  }
  if ( m_dumpTdrift ) {
    sc=m_nt->addItem("Tdrift",Tdrift);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Tdrift' failed" );
      return StatusCode::FAILURE;
    }
  }
  if ( m_dumpPhysCaliTdiff ) {
    sc=m_nt->addItem("Tdiff",Tdiff);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Tdiff' failed" );
      return StatusCode::FAILURE;
    }
  }
  if ( m_dumpDetCellParams ) {
    sc=m_nt->addItem("Omega0",Omega0);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Omega0' failed" );
      return StatusCode::FAILURE;
    }
    sc=m_nt->addItem("Taur",Taur);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Taur' failed" );
      return StatusCode::FAILURE;
    }
  }
  if (m_dumpOFCBin) {
    sc=m_nt->addItem("Bin",ofcbin);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addIdetm 'Bin' failed!" );
    }
  }

//   sc=m_nt->addItem("Tshaper",m_Tshaper);
//   if (sc!=StatusCode::SUCCESS) {
//     ATH_MSG_ERROR( "addItem 'Tshaper' failed" );
//     return StatusCode::FAILURE;
//   }
//   sc=m_nt->addItem("Amplitude",m_Amplitude);
//   if (sc!=StatusCode::SUCCESS) {
//     ATH_MSG_ERROR( "addItem 'Amplitude' failed" );
//     return StatusCode::FAILURE;
//   }

  //=======================================================================================================
  // dump WFparams from smaller complete structures 
  // LArCaliPulseParamsComplete , LArDetCellParamsComplete , LArPhysCaliTdiffComplete , LArTdriftComplete
  //=======================================================================================================




  for(long igain=CaloGain::LARHIGHGAIN; igain<CaloGain::LARNGAIN; igain++) {
   std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
   for(; itOnId!=itOnIdEnd;++itOnId){
     const HWIdentifier hwid = *itOnId;
     
     //Initialize variables to 'errorcode'
     float vTcal=ILArCaliPulseParams::ERRORCODE;
     float vFstep=ILArCaliPulseParams::ERRORCODE;
     float vTdrift=ILArCaliPulseParams::ERRORCODE; 
     float vOmega0=ILArCaliPulseParams::ERRORCODE; 
     float vTaur=ILArCaliPulseParams::ERRORCODE;
     float vTdiff=ILArCaliPulseParams::ERRORCODE;
     float vBin=ILArCaliPulseParams::ERRORCODE;
     //Tshaper=ILArCaliPulseParams::ERRORCODE; 
     //Amplitude=ILArCaliPulseParams::ERRORCODE; 
     

     bool haveData=false;
     if ( m_dumpCaliPulseParams ) {
       const LArCaliPulseParamsP& params=completeCaliPulseParams->get(hwid,igain);
       vTcal=params.m_Tcal;
       vFstep=params.m_Fstep;
       if (!haveData && (vTcal>1+ILArCaliPulseParams::ERRORCODE || vFstep>1+ILArCaliPulseParams::ERRORCODE)) haveData=true;
     }

     if (m_dumpDetCellParams ) {
       const LArDetCellParamsP& params=completeDetCellParams->get(hwid,igain);
       vOmega0=params.m_Omega0;
       vTaur=params.m_Taur;
       if (!haveData && (vOmega0>1+ILArCaliPulseParams::ERRORCODE || vTaur>1+ILArCaliPulseParams::ERRORCODE)) haveData=true;
     }
     
     if (m_dumpTdrift ) {
       const LArTdriftP& params=completeTdrift->get(hwid,gain);
       if (!params.isEmpty()) {
	 vTdrift=params.m_Tdrift;
	 haveData=true;
       }
     }

     if ( m_dumpPhysCaliTdiff ) {
	const LArPhysCaliTdiffP& params= completePhysCaliTdiff->get(hwid,igain);
	if (!params.isEmpty()) {
	  vTdiff=params.m_Tdiff;
	  if (!m_dumpTdrift && !m_dumpDetCellParams && !m_dumpCaliPulseParams) haveData=true;
	}
     }

     if ( m_dumpOFCBin ) {
       const LArOFCBinP & bin = completeOFCBin->bin(hwid,igain);
       if (!bin.isEmpty()) {
	 vBin=bin.m_bin;
	 haveData=true;
       }
     }

     if (haveData) {
       bool connected=fillFromIdentifier(hwid);  
       if (!connected) {
	 //std::cout << "Disconnected: Tdrift:" << vTdrift << " Tdiff:" << vTdiff << " Taur" << vTaur << " Omega0" << vOmega0 << " Fstep" << vFstep << " Tcal" << vTcal << std::endl;
	 continue;
       }
       if ( m_dumpCaliPulseParams ) {
	 Tcal=vTcal;
	 Fstep=vFstep;
       }
       if  (m_dumpDetCellParams ) {
	 Taur=vTaur;
	 Omega0=vOmega0;
       }

       if (m_dumpPhysCaliTdiff) Tdiff=vTdiff;

       if (m_dumpTdrift ) Tdrift=vTdrift; 

       if (m_dumpOFCBin) ofcbin=vBin;

       gain=igain;
       

       sc=ntupleSvc()->writeRecord(m_nt);
       if (sc!=StatusCode::SUCCESS) {
	 ATH_MSG_ERROR( "writeRecord failed" );
	 return StatusCode::FAILURE;
       }
     }//end if have data

   }//end loop over identifiers

  }//end loop over gains
  return StatusCode::SUCCESS;
}// end stop-method
