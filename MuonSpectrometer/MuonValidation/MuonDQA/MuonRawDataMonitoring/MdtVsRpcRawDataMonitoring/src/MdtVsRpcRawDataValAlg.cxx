/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MdtVsRpcRawDataValAlg
// Authors:   N. Benekos(Illinois) - G. Chiodini(INFN-Lecce) 
// Aug. 2007
//
// DESCRIPTION:
// Subject: MDTvsRPC-->Offline Muon Data Quality
///////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/StoreGate.h"

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"

#include "Identifier/Identifier.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
 
#include "MdtVsRpcRawDataMonitoring/MdtVsRpcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <sstream>

using namespace std;

static const int maxColl =   1200;
static const int maxPRD  =   50000;
static const int maxClus =   1000;

//mdt stuff
static const int maxPrd =    50000;
static const int ncutadc=       50;

static const int TDCminrange      =    0 ;
static const int TDCmaxrange      = 2000 ;
static const int TDCNbin          =  200 ; 

/////////////////////////////////////////////////////////////////////////////
// *********************************************************************
// Public Methods
// ********************************************************************* 
 

MdtVsRpcRawDataValAlg::MdtVsRpcRawDataValAlg( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent )
{ 

  // Declare the properties 
  declareProperty("DoMdtvsRpcEsd",              m_doMdtvsRpcESD=true);
  declareProperty("MdtvsRpcChamberHist",        m_mdtvsrpcchamberhist=false);  
  declareProperty("MdtvsRpcSectorHist",         m_mdtvsrpcsectorhist=false); 
  declareProperty("MdtvsRpcReduceRpcNbins",     m_mdtvsrpcreducerpcnbins=8);  
  declareProperty("MdtvsRpcReduceMdtNbins",     m_mdtvsrpcreducemdtnbins=8);   
  declareProperty("MdtvsRpcReduceMdtTDCNbins",  m_mdtvsrpcreducemdttdcnbins=10); 
  declareProperty("ChamberName",                m_chamberName="XXX");
  declareProperty("StationSize",                m_StationSize="XXX");
  declareProperty("StationEta",                 m_StationEta=-100);
  declareProperty("StationPhi",                 m_StationPhi=-100);
  declareProperty("LastEvent",                  m_lastEvent=0);
  declareProperty("Sector",                     m_sector=0); 
  declareProperty("CosmicStation",              m_cosmicStation=0);
  declareProperty("Side",                       m_side=0); 
  declareProperty("Clusters",                   m_doClusters = false);  
  declareProperty("ClusterContainer",           m_clusterContainerName = "rpcClusters");
  declareProperty("RpcPrepDataContainer",       m_key_rpc="RPC_Measurements");
  declareProperty("MdtPrepDataContainer",       m_key_mdt="MDT_DriftCircles");
  m_padsId        = 0 ;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  
MdtVsRpcRawDataValAlg::~MdtVsRpcRawDataValAlg()
{
  // fixes fot Memory leak
  if (m_padsId) { 
    delete m_padsId;
    m_padsId = 0; }
  ATH_MSG_INFO ( " deleting MdtVsRpcRawDataValAlg " );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdtVsRpcRawDataValAlg::initialize(){
 
  ATH_MSG_INFO ( "in initializing MdtVsRpcRawDataValAlg" );

  StatusCode sc;

  // Store Gate store
  sc = serviceLocator()->service("StoreGateSvc", m_eventStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( " Cannot get StoreGateSvc " );
    return sc ;
  }
  // retrieve the active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( " Cannot get ActiveStoreSvc " );
    return sc ;
  }

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "DetectorStore service not found !" );
    return StatusCode::FAILURE;
  }   
  
  // Retrieve the MuonDetectorManager  
  sc = detStore->retrieve(m_muonMgr);
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Cannot get MuonDetectorManager from detector store" );
    return StatusCode::FAILURE;
  }  
  else {
    ATH_MSG_DEBUG ( " Found the MuonDetectorManager from detector store. " );
  }

  sc = detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure())
    {
      ATH_MSG_ERROR ( "Can't retrieve RpcIdHelper" );
      return sc;
    }	 
  
  // get RPC cablingSvc
  const IRPCcablingServerSvc* RpcCabGet = 0;
  sc = service("RPCcablingServerSvc", RpcCabGet);
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Could not get RPCcablingServerSvc !" );
    return StatusCode::FAILURE;
  }
 
  sc = RpcCabGet->giveCabling(m_cabling);
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Could not get RPCcablingSvc from the Server !" );
    m_cabling = 0;
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( " Found the RPCcablingSvc. " );    } 


  //mdt stuff
  sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Cannot get MdtIdHelper" );
    return StatusCode::FAILURE;
  }  
  else {
    ATH_MSG_DEBUG ( " Found the MdtIdHelper. " );
  }
 
  std::vector<std::string> hardware_name_list      ;
  std::vector<std::string> layer_name_list         ;
  std::vector<std::string> layerSector_name_list   ;
  hardware_name_list.push_back("XXX")        ;

  rpc_event_inarea=0;
  ManagedMonitorToolBase::initialize().ignore();  //  Ignore the checking code;
  
  imdt_station      =0;
  imdt_eta          =0;
  imdt_phi          =0;
  imdt_multi        =0;
  imdt_layer        =0;
  imdt_wire         =0;
  imdt_wirez        =0;
  imdt_nmaxtubes    =0;
  imdt_multi_near   =0;
  stripzmin         =0;
  stripzmax         =0;
  pitch             =0;
  wirezmax          =0;
  wirezmin          =0;
  foundmin          =0;
  NtubesPerLayerlast=0;
  z                 =0;
  m_nColl           =0;
  m_nPrd            =0;
  m_nClus           =0;
  m_nCollmdt        =0;
  m_nPrdmdt         =0;
  MDTvsRPCNbinz     =0;
  MDTvsRPCNbinx     =0;
  N_RpcHitdblPhi1   =0;
  N_RpcHitdblPhi2   =0;
 
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdtVsRpcRawDataValAlg::fillHistograms()
{
  
  StatusCode sc = StatusCode::SUCCESS;
  ATH_MSG_DEBUG ( "MdtVsRpcRawDataValAlg::MDT-RPC correlation RawData Monitoring Histograms being filled" );

  if( m_doMdtvsRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {

 
    std::string layer_name       ; 
    std::string sector_name      ; 
    std::string layerSector_name ;
  
    //declare a group of histograms
    std::string m_generic_path_mdtvsrpcmonitoring = "Muon/MuonRawDataMonitoring/MDTvsRPC" ;
    MonGroup mdtrpc_shift_dqmf( this, m_generic_path_mdtvsrpcmonitoring+ "/Dqmf", run, ATTRIB_UNMANAGED ) ;
  
    sc = mdtrpc_shift_dqmf.getHist( MdtRpcZdiff, "MdtRpcZdifference" ) ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register MdtRpcZdifference hist to MonGroup" );         
    
    sc = mdtrpc_shift_dqmf.getHist( MdtNHitsvsRpcNHits, "MdtNHitsvsRpcNHits" ) ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register MdtNHitsvsRpcNHits hist to MonGroup" ); 
  
    //mdt stuff begin     
    m_nPrdmdt  = 0;
    Identifier ch_idmdt;
    Identifier dig_idmdt;
  
    const Muon::MdtPrepDataContainer* mdt_container(0);
    sc = (*m_activeStore)->retrieve(mdt_container, m_key_mdt);
    if (sc.isFailure() || 0 == mdt_container) 
      {
	ATH_MSG_ERROR ( " Cannot retrieve MdtPrepDataContainer " << m_key_mdt );
	return sc;
      }
  
    ATH_MSG_DEBUG ("****** mdtContainer->size() : " << mdt_container->size());  

    Muon::MdtPrepDataContainer::const_iterator mdt_containerIt;
    //mdt stuff end
  
    const Muon::RpcPrepDataContainer* rpc_container;
    sc = (*m_activeStore)->retrieve(rpc_container, m_key_rpc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR ( " Cannot retrieve RpcPrepDataContainer " << m_key_rpc );
      return sc;
    }

    
    ATH_MSG_DEBUG ("****** rpc->size() : " << rpc_container->size());  
  
    Muon::RpcPrepDataContainer::const_iterator containerIt;
  
  
    m_nPrd = 0;
    Identifier ch_id;
    Identifier dig_id;
  
    std::string type="RPC";

    float Nhitsrpc = 0 ;
    for (containerIt = rpc_container->begin() ; containerIt != rpc_container->end() ; ++containerIt) 
      {   
	for (Muon::RpcPrepDataCollection::const_iterator rpcPrd = (*containerIt)->begin(); rpcPrd!=(*containerIt)->end(); ++rpcPrd)
	  {  
	    ++Nhitsrpc ;  
    }}
    
    float Nhitsmdt = 0 ;
    Muon::MdtPrepDataContainer::const_iterator MdtcontainerIt ;
    for (MdtcontainerIt = mdt_container->begin() ; MdtcontainerIt != mdt_container->end() ; ++MdtcontainerIt) 
      {
         for (Muon::MdtPrepDataCollection::const_iterator mdtCollection=(*MdtcontainerIt)->begin(); mdtCollection!=(*MdtcontainerIt)->end(); ++mdtCollection ) 
		      {  
	    ++Nhitsmdt ;  
    }}   
    if(Nhitsrpc>0){ Nhitsrpc=log10(Nhitsrpc);} 
    else{ Nhitsrpc=-0.5;}   
    if(Nhitsmdt>0){ Nhitsmdt=log10(Nhitsmdt);} 
    else{ Nhitsmdt=-0.5;}
    MdtNHitsvsRpcNHits->Fill(Nhitsrpc,Nhitsmdt);
    
    N_RpcHitdblPhi1 = 0;
    N_RpcHitdblPhi2 = 0;
    
    // to fix IConversionSvc ptr not set DataProxyÂ : appearing in every event

    //  for (std::vector<Identifier>::const_iterator id_it = m_rpcchambersId->begin();
    //       id_it != m_rpcchambersId->end() ; ++id_it ){	   
    //    containerIt = rpc_container->find(*id_it);

    for (containerIt = rpc_container->begin() ; containerIt != rpc_container->end() ; ++containerIt) 
      {   
	for (Muon::RpcPrepDataCollection::const_iterator rpcPrd = (*containerIt)->begin(); rpcPrd!=(*containerIt)->end(); ++rpcPrd)
	  {

	    if (m_nPrd<maxPRD) {

	      //    if (containerIt!= rpc_container->end() && (*containerIt)->size()>0){      
      
	      //      Identifier prd_id = *id_it;

	      Identifier prd_id = (*rpcPrd)->identify();

	      irpcstationPhi	=   int(m_rpcIdHelper->stationPhi (prd_id))  ;		   
	      irpcstationName	=   int(m_rpcIdHelper->stationName(prd_id))  ;		   
	      irpcstationEta	=   int(m_rpcIdHelper->stationEta (prd_id))  ;			   
	      irpcdoubletR 	=   int(m_rpcIdHelper->doubletR   (prd_id))  ;		
	      irpcmeasuresPhi	=   int(m_rpcIdHelper->measuresPhi(prd_id))  ;
	      // only take eta hits
	      if( irpcmeasuresPhi != 0 )continue;
	      irpcdoubletPhi	 =   int(m_rpcIdHelper->doubletPhi(prd_id))  ;
	      irpcdoubletZ	 =   int(m_rpcIdHelper->doubletZ(prd_id))    ;
	      irpcgasGap	 =   int(m_rpcIdHelper->gasGap(prd_id))      ;
	      irpcstrip		 =   int(m_rpcIdHelper->strip(prd_id))       ;
	   
	      irpctime		 =   double((*rpcPrd)->time())  	     ;		  
	      irpctriggerInfo    =   double((*rpcPrd)->triggerInfo   ())     ;	      
	      irpcambiguityFlag	 =   double((*rpcPrd)->ambiguityFlag ())     ;
	    
      
      
      
	      std::string hardware_name=convertChamberName(irpcstationName,irpcstationEta,irpcstationPhi,type) ;
  	  	  
	      if (selectChambersRange(hardware_name, m_chamberName, 
				      m_rpcIdHelper->stationEta(dig_id), m_StationEta,
				      m_rpcIdHelper->stationPhi(dig_id), m_StationPhi, m_StationSize) && chambersCosmicSetup(hardware_name,m_cosmicStation)) {	 

		//define layer
		if( (irpcstationName>1) && (irpcstationName<4||irpcstationName==8) ){
		  if(irpcdoubletR==1){layer_name="LowPt";imdt_multi_near=1;}
		  else {layer_name="Pivot";imdt_multi_near=2;}
		}			
		else {layer_name="HighPt";imdt_multi_near=2;
		if(irpcstationName !=4 ) imdt_multi_near=1;
		} 

		//define sector
		side	     = 'A'     ;
		EtaStripSign =  1      ; 
		if(irpcstationEta<0){
		  side = 'C'         ;
		  EtaStripSign = -1  ; 
		}
		sector = 2 * irpcstationPhi                              ;
		if(irpcstationName==2 ||  irpcstationName==4 ) sector--  ;
		sprintf(sector_char,"Sector%.2d",sector)                   ;
		sector_name =  sector_char                               ;

		layerSector_name  = sector_name + layer_name  ;

		NetaStrips = 0 ;
		for(int idbz=0; idbz!= 3; idbz++){
        ShiftEtaStripsDoubletZ[idbz] = NetaStrips;
        Identifier id = m_rpcIdHelper->channelID(irpcstationName,irpcstationEta,irpcstationPhi,irpcdoubletR,idbz+1, 1, 1, 1, 1); // last 4 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip
        if (!id.is_valid()) {
          ATH_MSG_DEBUG("Could not find valid Identifier for station="<<irpcstationName<<", eta="<<irpcstationEta<<", phi="<<irpcstationPhi<<", doubletR="<<irpcdoubletR<<", doubletZ="<<idbz+1<<", continuing...");
          continue;
        }
        const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(id);
        if(rpc){
		    NetaStrips +=  rpc->NetaStrips();
		  } 
		}


	    
		histo_flag=true;
		for (std::vector<std::string>::const_iterator iter=layerSector_name_list.begin(); iter!=layerSector_name_list.end(); iter++){
		  if ( (sector_name+layerSector_name)==*iter){histo_flag=false;}
		}

		if (histo_flag){
		  // Get the RPC axis range
		
		  /////// NB !!!!!
		  // the eta strip number increases going far away from IP
		  // the phi strip numeber increases going from HV side to RO side
		  stripzmin   =      0 ;
		  stripzmax   = -10000 ;
      // the RpcIdHelper stationEta ranges from -8 to 8
		  for(int ieta=-8; ieta<9; ieta++){
		    for(int idbz=0; idbz!= 3; idbz++){
          Identifier id = m_rpcIdHelper->channelID(irpcstationName,ieta,irpcstationPhi,irpcdoubletR,idbz+1, 1, 1, 1, 1); // last 4 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip
          if (!id.is_valid()) {
            ATH_MSG_DEBUG("Could not find valid Identifier for station="<<irpcstationName<<", eta="<<ieta<<", phi="<<irpcstationPhi<<", doubletR="<<irpcdoubletR<<", doubletZ="<<idbz+1<<", continuing...");
            continue;
          }
          const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(id);
          if(rpc){

			const Amg::Vector3D r1 = rpc-> globalPosition();
			pitch = rpc-> StripPitch(0)  ;
		
			float z1 = float ( r1.z() - ( rpc -> NetaStrips() ) * pitch / 2 );
			if ( z1 < stripzmin ) {
			  stripzmin   = z1 ;
			}
			float z2 = float ( r1.z() + ( rpc -> NetaStrips() )* pitch / 2 );
			if ( z2 > stripzmax ) { stripzmax = z2 ; }	      
	      
		      } //check if rpc!=NULL
		    } //for loop in idbz 
	      	      
		  } // for loop in etastation
	  
		  // get the MDT axis range
		  wirezmax     = -10000. ;
		  wirezmin     = +10000. ;
		  foundmin     =      0  ;	
			
      int stname_index = irpcstationName;
		  if (irpcstationName == 53) stname_index = MuonGM::MuonDetectorManager::NMdtStatType-2;
		  else stname_index = irpcstationName;
		  for(int eta=0; eta!=17; eta++){ 
		    const MuonGM::MdtReadoutElement* lastdescr = m_muonMgr->getMdtReadoutElement(stname_index, eta, irpcstationPhi-1, imdt_multi_near-1);
		    if(lastdescr==NULL)continue;
		
		    const Amg::Vector3D lastelc = lastdescr->globalPosition();
		    NtubesPerLayerlast = lastdescr->getNtubesperlayer();
		    z =  float(lastelc.z()) + float(NtubesPerLayerlast)/2*29.9;
		
		    if(foundmin==0){ wirezmin   =  float(lastelc.z()) - float(NtubesPerLayerlast)/2*29.9;} 
		    foundmin = 1 ; 
		    if(z>wirezmax)wirezmax=z; 
	      
		  } // loop over eta		     
		
		  layerSector_name_list.push_back(sector_name+layerSector_name); 
		  ATH_MSG_DEBUG ( " strip zmin  and zmax"  << stripzmin <<" "<< stripzmax );
	      
		  if (m_mdtvsrpcsectorhist)bookMDTvsRPCsectorHistograms(sector_name, layer_name, stripzmin, stripzmax, wirezmin, wirezmax);
	
		}// if histo_flag
	
		
		// Per Sector
		const MuonDQAHistList& hists1 = m_stationHists.getList( sector_name );
		TH2* mdttubevsrpcetastripsector;

		//get information from geomodel to book and fill rpc histos with the right max strip number
		const MuonGM::RpcReadoutElement* descriptor =  m_muonMgr->getRpcReadoutElement(prd_id);
		const Amg::Vector3D stripPos = descriptor->stripPos(prd_id);
	        ATH_MSG_DEBUG ( "rpc coord" << stripPos.z() << stripPos.perp() );
		irpcstripz = float( stripPos.z() );

		NphiStrips       = descriptor -> NphiStrips()    *  2                    ;
		ShiftPhiStrips	 = descriptor -> NphiStrips()    * (irpcdoubletPhi  -1 ) ;

		ShiftEtaStrips	 = ShiftEtaStripsDoubletZ[irpcdoubletZ-1] ;

		Nbin        = NetaStrips;
		ShiftStrips = ShiftEtaStrips;

		//re-define for phi view
		if(irpcmeasuresPhi==1) {
		  Nbin = NphiStrips ;		 
		  ShiftStrips =  ShiftPhiStrips;
		}
	    
		//define sectorlogic		
		sectorlogic = (sector-1) * 2 ; 
		if(irpcdoubletPhi==1)sectorlogic -= 1     ;
		if(sectorlogic<0)    sectorlogic += 32    ;
		if(side == 'A')      sectorlogic += 32    ;
		
		++m_nPrd;
		ATH_MSG_DEBUG ( " PRD number  " << m_nPrd );	    
	    
	    
		//////////////loop inside rpc hits
	      
		//////////////loop on mdt begin      	    
		// to fix IConversionSvc ptr not set DataProxy\ufffd: appearing in every event
		//loop in MdtPrepDataContainer
		for (Muon::MdtPrepDataContainer::const_iterator containerMdtIt = mdt_container->begin() ; containerMdtIt != mdt_container->end() ; ++containerMdtIt) {
		  if (containerMdtIt == mdt_container->end() || (*containerMdtIt)->size()==0)continue;  //check if there are counts  
		  ATH_MSG_DEBUG ( "container size? " << (*containerMdtIt)->size() );
		  //check size of the MdtPrepDataContainer 
		  if ((*containerMdtIt)->size()>0) {
		    // loop over hits
		    for (Muon::MdtPrepDataCollection::const_iterator mdtCollection=(*containerMdtIt)->begin(); mdtCollection!=(*containerMdtIt)->end(); ++mdtCollection ) 
		      {
			dig_idmdt = (*mdtCollection)->identify();
			imdt_station      =  int(m_mdtIdHelper->stationName (dig_idmdt));
			if (imdt_station != irpcstationName) continue;
			imdt_eta          =  int(m_mdtIdHelper->stationEta  (dig_idmdt));
			if (imdt_eta     != irpcstationEta ) continue; 
			imdt_phi          =  int(m_mdtIdHelper->stationPhi  (dig_idmdt));
			if (imdt_phi     != irpcstationPhi ) continue;
			dig_idmdt = (*mdtCollection)->identify();
			imdt_multi     =  int(m_mdtIdHelper->multilayer  (dig_idmdt));
			// only look at near multilayer
			if(imdt_multi  != imdt_multi_near) continue;
			imdt_adc       =  int((*mdtCollection)->adc());
			//cut on noise
			if( imdt_adc<ncutadc )continue;  
			imdt_tdc       =  int((*mdtCollection)->tdc());
			imdt_layer     =  int(m_mdtIdHelper->tubeLayer   (dig_idmdt));
			imdt_wire      =  int(m_mdtIdHelper->tube        (dig_idmdt));
			imdt_nmaxtubes =  int(m_mdtIdHelper->tubeMax     (dig_idmdt));
		    
 		    
			//get mdt information from geomodel to book and fill mdtvsrpc histos with the right min and max range
			if (imdt_station == 53) imdt_station = MuonGM::MuonDetectorManager::NMdtStatType-2;
			const MuonGM::MdtReadoutElement* mdt = m_muonMgr->getMdtReadoutElement( imdt_station,  imdt_eta+8, imdt_phi-1,  imdt_multi-1);
			NetaTubes = mdt->getNtubesperlayer() ;			    	      
			const Amg::Vector3D elc =  mdt->globalPosition();
			imdt_wirez =  float(elc.z());
		  
			if(imdt_wirez>=0) {imdt_wirez +=  (float(imdt_wire)-0.5-float(NetaTubes)/2 )* 29.9 ;}
			else {imdt_wirez =  imdt_wirez -  (float(imdt_wire)-0.5-float(NetaTubes)/2 )* 29.9 ;}
		
			//fill histos
			if(m_mdtvsrpcchamberhist){
		      
			  histo_flag=true;
			  for (std::vector<std::string>::const_iterator iter=layer_name_list.begin(); iter!=layer_name_list.end(); iter++){
			    if ( (hardware_name+layer_name)==*iter){histo_flag=false;}
			  }
			  if (histo_flag){
			    //booking if necessary 
			    imdt_station = irpcstationName;
			    imdt_eta     = irpcstationEta ;
			    imdt_phi     = irpcstationPhi ;
			    NetaTubes = 0;
			    if (imdt_station == 53) imdt_station = MuonGM::MuonDetectorManager::NMdtStatType-2;
			    const MuonGM::MdtReadoutElement* mdt = m_muonMgr->getMdtReadoutElement( imdt_station,  imdt_eta+8, imdt_phi-1,  imdt_multi_near-1);
			    if(mdt==NULL)continue; // protection
			    NetaTubes = mdt->getNtubesperlayer();
			    layer_name_list.push_back(hardware_name+layer_name); 
			    if (NetaTubes!=0) bookMDTvsRPCHistograms(hardware_name,layer_name, NetaStrips, 0 , NetaStrips, NetaTubes, 0, NetaTubes);
			  }
			  // Per chamber
			  const MuonDQAHistList& hists = m_stationHists.getList( hardware_name );	
			  // tube vs strip - doublephi separated
			  TH2* mdttubevsrpcetastrip_doublphi1 = hists.getH2(hardware_name+"_"+layer_name+"_MDTtube_vs_RPCstrip_doublPhi1");
			  TH2* mdttubevsrpcetastrip_doublphi2 = hists.getH2(hardware_name+"_"+layer_name+"_MDTtube_vs_RPCstrip_doublPhi2"); 
			  // tdc of mdt - doublephi separated 	
			  TH1* mdttdcdoublphi1 = hists.getH1(hardware_name+"_"+layer_name+"_MDTtdc_doublPhi1");
			  TH1* mdttdcdoublphi2 = hists.getH1(hardware_name+"_"+layer_name+"_MDTtdc_doublPhi2"); 
		        	    
			  // tdc of mdt
			  // tube vs strip doublephi separated
			  if ( irpcdoubletPhi == 1 ) {
			    if ( N_RpcHitdblPhi1==0 ) { if (mdttdcdoublphi1) { mdttdcdoublphi1->Fill(float(imdt_tdc)*25.0/32.0); } }
			    if (mdttubevsrpcetastrip_doublphi1) {mdttubevsrpcetastrip_doublphi1->Fill ( (irpcstrip + ShiftStrips),imdt_wire ); }
			  }	  
			  else {
			    if ( N_RpcHitdblPhi2==0 ) { if (mdttdcdoublphi2) { mdttdcdoublphi2->Fill(float(imdt_tdc)*25.0/32.0); } }
			    if (mdttubevsrpcetastrip_doublphi2) {mdttubevsrpcetastrip_doublphi2->Fill ( (irpcstrip + ShiftStrips),imdt_wire);}
			  }
		        	    
		        	   
			}//end if on mdtvsrpcchamberhist		      
		        if (m_mdtvsrpcsectorhist) {
			 mdttubevsrpcetastripsector = hists1.getH2(sector_name+"_"+layer_name+"_MDTtube_vs_RPCstrip");
			 if (mdttubevsrpcetastripsector) {mdttubevsrpcetastripsector->Fill( (float)irpcstripz, (float)imdt_wirez );}
			 else { ATH_MSG_DEBUG ( "mdttubevsrpcetastripsector not in hist list!" );}
		        }
			// shifter histogram
			mdt_rpc_dz = imdt_wirez - irpcstripz     ;
			MdtRpcZdiff -> Fill ( mdt_rpc_dz ) ;
			
			++m_nPrdmdt;
			ATH_MSG_DEBUG ( " MdtPrepData number:  " << m_nPrdmdt );
	      
			if (m_nPrdmdt > maxPrd-1) {
			  ATH_MSG_WARNING ( "Maximum number of MdtPrepData in the ntuple reached: " << maxPrd );
			  return StatusCode::SUCCESS;
			}  
		      
		      } // loop on MDT Collection

		  }// if (mdt_containerIt size>0)    
		} // loop on MDT Container
		//////////////loop on mdt end		
		if ( irpcdoubletPhi == 1 ) { N_RpcHitdblPhi1++; }
		else                       { N_RpcHitdblPhi2++; }
		
		  
	      }//chamber name selection
 
	      ATH_MSG_DEBUG ( " RpcPrepData number:  " << m_nPrd );
 
	    }//if maxprd rpc 
	    else {
	      ATH_MSG_WARNING ( "More than " << maxPRD << " RPC PrepRawData" );
	    }	  
	  }/// loop on rpc collections    
      }/// loop on rpc containers 
   
  }}//m_doMdtvsRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD   
  return sc;  // statuscode check  
   
}  
 

/*----------------------------------------------------------------------------------*/
StatusCode MdtVsRpcRawDataValAlg::bookHistogramsRecurrent()
  /*----------------------------------------------------------------------------------*/
{
 
  ATH_MSG_DEBUG ( "MdtVsRpcRawDataValAlg Monitoring Histograms being booked" );
 
  StatusCode sc = StatusCode::SUCCESS; 


  if( m_doMdtvsRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
  
    //declare a group of histograms
    std::string m_generic_path_mdtvsrpcmonitoring = "Muon/MuonRawDataMonitoring/MDTvsRPC";
    MonGroup mdtrpc_shift_dqmf( this, m_generic_path_mdtvsrpcmonitoring+ "/Dqmf", run, ATTRIB_UNMANAGED ) ;

    if(newRunFlag()) 
      {
	ATH_MSG_DEBUG ( "MdtVsRpcRawDataValAlg : isNewRun" );
	// Book RAW or ESD capable histos
      
      
	// SHIFTer histograms
	// distribution of difference Zmdt - Zrpc
	float maxdz = 1500 ; // mm
	std::string m_MdtRpcZdiff_title = "MdtRpcZdifference" ;
	const char* m_MdtRpcZdiff_title_char = m_MdtRpcZdiff_title.c_str();
	 
	TH1* MdtRpcZdiff = new TH1I( m_MdtRpcZdiff_title_char, m_MdtRpcZdiff_title_char, 100, -maxdz, maxdz ) ;
	sc = mdtrpc_shift_dqmf.regHist( MdtRpcZdiff ) ;
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "MdtRpcZdiff Failed to register histogram " );       
	  return sc;
	  }
	MdtRpcZdiff->GetXaxis()->SetTitle("Z mdt tube - Z rpc strip [mm]" );
	MdtRpcZdiff->GetYaxis()->SetTitle("Counts") ;
	MdtRpcZdiff->SetFillColor(42);
	 
	 
	  ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << MdtRpcZdiff << m_MdtRpcZdiff_title.c_str() );
	 // ATH_MSG_DEBUG ( "SHIFT : " << shift );
	  ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	  ATH_MSG_DEBUG ( "Booked MdtRpcZdifference successfully" );
	  
      
      // correlation number of MDT hits vs RPC hits
	std::string m_MdtNHitsvsRpcNHits_title = "MdtNHitsvsRpcNHits" ;
	const char* m_MdtNHitsvsRpcNHits_title_char = m_MdtNHitsvsRpcNHits_title.c_str();
	 
	TH2* MdtNHitsvsRpcNHits = new TH2I( m_MdtNHitsvsRpcNHits_title_char, m_MdtNHitsvsRpcNHits_title_char, 11, -1, 10, 11, -1, 10) ;
	sc = mdtrpc_shift_dqmf.regHist( MdtNHitsvsRpcNHits ) ;
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "MdtNHitsvsRpcNHits Failed to register histogram " );       
	  return sc;
	  }
	MdtNHitsvsRpcNHits->GetYaxis()->SetTitle("Number of MDT hits (10dB)" );
	MdtNHitsvsRpcNHits->GetXaxis()->SetTitle("Number of RPC hits (10dB)" );
	MdtNHitsvsRpcNHits->SetFillColor(42);
	MdtNHitsvsRpcNHits->SetOption("COLZ");
 	MdtNHitsvsRpcNHits->SetMarkerColor(1);  
 	MdtNHitsvsRpcNHits->SetMarkerStyle(21);   
  	MdtNHitsvsRpcNHits->SetMarkerSize(0.2); 	 
	 
	  ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << MdtNHitsvsRpcNHits << m_MdtNHitsvsRpcNHits_title.c_str() );
	 // ATH_MSG_DEBUG ( "SHIFT : " << shift );
	  ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	  ATH_MSG_DEBUG ( "Booked MdtNHitsvsRpcNHits successfully" );
	 
     }
  }}//m_doMdtvsRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD 
  return sc;

}
/*----------------------------------------------------------------------------------*/
//input hardware name as produced from convertChamberName()
void MdtVsRpcRawDataValAlg::bookMDTvsRPCHistograms(std::string m_hardware_name, std::string m_layer_name, int binz, int binminz, int binmaxz, int binx, int binminx, int binmaxx )
{ 

  StatusCode sc = StatusCode::SUCCESS;  

 
  if( m_doMdtvsRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
  
    // m_hardware_name = "" ;
  
    binz =  binz / m_mdtvsrpcreducerpcnbins ;
    binx =  binx / m_mdtvsrpcreducemdtnbins ;
  
    //declare a group of histograms
    std::string m_generic_path_mdtvsrpcmonitoring = "Muon/MuonRawDataMonitoring/MDTvsRPC";
    MonGroup mdtvsrpc_prd_expert( this, m_generic_path_mdtvsrpcmonitoring+"/Chambers/"+m_hardware_name, run, ATTRIB_UNMANAGED );   
    MuonDQAHistList& lst = m_stationHists.getList( m_hardware_name );
    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for mdt vrs rpc 
 
    // mdt vs rpc - doublephi separated 
    std::string m_mdtvsrpc_dphi1_title       = m_hardware_name+"_"+m_layer_name+"_MDTtube_vs_RPCstrip_doublPhi1";
    std::string m_mdtvsrpc_dphi2_title       = m_hardware_name+"_"+m_layer_name+"_MDTtube_vs_RPCstrip_doublPhi2";	
    const char* m_mdtvsrpc_dphi1_title_char  = m_mdtvsrpc_dphi1_title.c_str()                                   ;
    const char* m_mdtvsrpc_dphi2_title_char  = m_mdtvsrpc_dphi2_title.c_str()                                   ;

    TH2 *mdttubevsrpcetastrip_doublphi1 = new TH2I(m_mdtvsrpc_dphi1_title_char,m_mdtvsrpc_dphi1_title_char, binz, binminz, binmaxz, binx, binminx, binmaxx); 		
    lst.addHist(mdttubevsrpcetastrip_doublphi1); 
    //mdtvsrpcexp_prd.regHist(mdttubevsrpcetastrip_doublphi1);
    mdttubevsrpcetastrip_doublphi1->SetOption("COLZ");
    mdttubevsrpcetastrip_doublphi1->SetFillColor(42);  
    mdttubevsrpcetastrip_doublphi1->SetMarkerColor(1);  
    mdttubevsrpcetastrip_doublphi1->SetMarkerStyle(21);   
    mdttubevsrpcetastrip_doublphi1->SetMarkerSize(0.2);  // 1
    mdttubevsrpcetastrip_doublphi1->GetXaxis()->SetTitle("<--- IP        Rpc Eta strip               EC --->"     );
    mdttubevsrpcetastrip_doublphi1->GetYaxis()->SetTitle("<--- IP        Mdt wire number ADC Cut ON  EC --->"     ); 
  
    sc = mdtvsrpc_prd_expert.regHist(mdttubevsrpcetastrip_doublphi1);
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register mdttubevsrpcetastrip_doublphi1 hist to MonGroup" );
  
    TH2 *mdttubevsrpcetastrip_doublphi2 = new TH2I(m_mdtvsrpc_dphi2_title_char,m_mdtvsrpc_dphi2_title_char, binz, binminz, binmaxz, binx, binminx, binmaxx); 		
    lst.addHist(mdttubevsrpcetastrip_doublphi2); 
    //mdtvsrpcexp_prd.regHist(mdttubevsrpcetastrip_doublphi2);
    mdttubevsrpcetastrip_doublphi2->SetOption("COLZ");
    mdttubevsrpcetastrip_doublphi2->SetFillColor(42);  
    mdttubevsrpcetastrip_doublphi2->SetMarkerColor(1);  
    mdttubevsrpcetastrip_doublphi2->SetMarkerStyle(21);   
    mdttubevsrpcetastrip_doublphi2->SetMarkerSize(0.2);  // 1
    mdttubevsrpcetastrip_doublphi2->GetXaxis()->SetTitle("<--- IP        Rpc Eta strip               EC --->"     );
    mdttubevsrpcetastrip_doublphi2->GetYaxis()->SetTitle("<--- IP        Mdt wire number ADC Cut ON  EC --->"     );   
  
    sc = mdtvsrpc_prd_expert.regHist(mdttubevsrpcetastrip_doublphi2);
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register mdttubevsrpcetastrip_doublphi2 hist to MonGroup" );
  
     
      ATH_MSG_DEBUG ( "INSIDE  bookMDTvsRPCHistograms doublPhi 1: " << mdttubevsrpcetastrip_doublphi1 );
      ATH_MSG_DEBUG ( "INSIDE  bookMDTvsRPCHistograms doublPhi 2: " << mdttubevsrpcetastrip_doublphi1 );
      //ATH_MSG_DEBUG ( "SHIFT : " << shift );
      ATH_MSG_DEBUG ( "RUN : " << run );
    
  
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for mdt tdc for doublphi1 and 2 
    //   doubletphi 1

  
    std::string m_generic_path_mdttdcdoublphi1 = "/muoncosmics/MDTvsRPC/Chambers/"+m_hardware_name+"/"+m_layer_name;
    std::string m_mdttdcdoublphi1_title = m_hardware_name+"_"+m_layer_name+"_MDTtdc_doublPhi1";	
    const char* m_mdttdcdoublphi1_title_char = m_mdttdcdoublphi1_title.c_str();

    TH1 *mdttdcdoublphi1=new TH1I(m_mdttdcdoublphi1_title_char,m_mdttdcdoublphi1_title_char, TDCNbin/m_mdtvsrpcreducemdttdcnbins, TDCminrange, TDCmaxrange); 		
    lst.addHist(mdttdcdoublphi1); 
    //mdtvsrpcexp_prd.regHist(mdttdcdoublphi1);
    mdttdcdoublphi1->SetFillColor(42);  
    mdttdcdoublphi1->SetMarkerColor(1);  
    mdttdcdoublphi1->SetMarkerStyle(21);   
    mdttdcdoublphi1->SetMarkerSize(0.2);
    mdttdcdoublphi1->GetXaxis()->SetTitle("Time Spectrum [ns]"     );
    mdttdcdoublphi1->GetYaxis()->SetTitle("Counts"                 ); 
  
    sc = mdtvsrpc_prd_expert.regHist(mdttdcdoublphi1);
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register mdttdcdoublphi1 hist to MonGroup" );
   
      ATH_MSG_DEBUG ( "INSIDE  bookmdttdcdoublphi1Histograms: " << mdttdcdoublphi1 << m_generic_path_mdttdcdoublphi1.c_str() );
      ATH_MSG_DEBUG ( "RUN : " << run );
    
    //   ---------------  doublphi2 
 
    std::string m_generic_path_mdttdcdoublphi2 = "/muoncosmics/MDTvsRPC/Chambers/"+m_hardware_name+"/"+m_layer_name;
    std::string m_mdttdcdoublphi2_title = m_hardware_name+"_"+m_layer_name+"_MDTtdc_doublPhi2";	
    const char* m_mdttdcdoublphi2_title_char = m_mdttdcdoublphi2_title.c_str();

    TH1 *mdttdcdoublphi2=new TH1I(m_mdttdcdoublphi2_title_char,m_mdttdcdoublphi2_title_char, TDCNbin/m_mdtvsrpcreducemdttdcnbins, TDCminrange, TDCmaxrange);	
    lst.addHist(mdttdcdoublphi2); 
    //mdtvsrpcexp_prd.regHist(mdttdcdoublphi2);
    mdttdcdoublphi2->SetFillColor(42);  
    mdttdcdoublphi2->SetMarkerColor(1);  
    mdttdcdoublphi2->SetMarkerStyle(21);   
    mdttdcdoublphi2->SetMarkerSize(0.2);
    mdttdcdoublphi2->GetXaxis()->SetTitle("Time Spectrum [ns]"     );
    mdttdcdoublphi2->GetYaxis()->SetTitle("Counts"                 ); 
  
    sc = mdtvsrpc_prd_expert.regHist(mdttdcdoublphi2);
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register mdttdcdoublphi2 hist to MonGroup" );
   
      ATH_MSG_DEBUG ( "INSIDE  bookmdttdcdoublphi2Histograms: " << mdttdcdoublphi2 << m_generic_path_mdttdcdoublphi2.c_str() );
      ATH_MSG_DEBUG ( "RUN : " << run );
    
  
  }}//m_doMdtvsRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD   
   	
}

/*----------------------------------------------------------------------------------*/
void MdtVsRpcRawDataValAlg::bookMDTvsRPCsectorHistograms(std::string m_sector_name, std::string m_layer_name, float stripzmin, float stripzmax,float wirezmin, float wirezmax )
{ 
 
  StatusCode sc = StatusCode::SUCCESS;   
 
  if( m_doMdtvsRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
  		  
    //declare a group of histograms
    std::string m_generic_path_mdtvsrpcmonitoring = "Muon/MuonRawDataMonitoring/MDTvsRPC";
    MonGroup mdtvsrpc_prd_shift( this, m_generic_path_mdtvsrpcmonitoring+"/Sectors/"+m_sector_name, run, ATTRIB_UNMANAGED );   
    MuonDQAHistList& lst = m_stationHists.getList(m_sector_name);
    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for mdt vrs rpc 
    std::string m_generic_path_mdtvsrpcsector = m_generic_path_mdtvsrpcmonitoring+"/Sectors/"+m_sector_name+m_layer_name;
    std::string m_mdtvsrpcsector_title = m_sector_name+"_"+m_layer_name+"_MDTtube_vs_RPCstrip";	 
    const char* m_mdtvsrpcsector_title_char = m_mdtvsrpcsector_title.c_str();
  
    MDTvsRPCNbinx = int( (-stripzmin+stripzmax) / 30 / m_mdtvsrpcreducemdtnbins)  ;
    MDTvsRPCNbinz = int( (-wirezmin +wirezmax ) / 30 / m_mdtvsrpcreducerpcnbins)  ;
   
    TH2 *mdtvsrpcsector=new TH2I(m_mdtvsrpcsector_title_char,m_mdtvsrpcsector_title_char, MDTvsRPCNbinz, stripzmin, stripzmax , MDTvsRPCNbinx, wirezmin, wirezmax);    		
    lst.addHist(mdtvsrpcsector); 
  
    mdtvsrpcsector->SetOption("COLZ");
    mdtvsrpcsector->SetFillColor(42);  
    mdtvsrpcsector->SetMarkerColor(1);  
    mdtvsrpcsector->SetMarkerStyle(21);   
    //mdtvsrpcsector->SetMarkerSize(0.2);
    mdtvsrpcsector->GetXaxis()->SetTitle("<--- Side C                Rpc Eta strip z [mm]        Side A --->"	);
    mdtvsrpcsector->GetYaxis()->SetTitle("<--- Side C                Mdt wire z [mm]	       Side A --->"	);
  
      ATH_MSG_DEBUG ( "INSIDE  bookMDTvsRPCsectorHistograms: " << mdtvsrpcsector << m_generic_path_mdtvsrpcsector.c_str() );
      ATH_MSG_DEBUG ( "RUN : " << run );
     
    sc = mdtvsrpc_prd_shift.regHist(mdtvsrpcsector);  		     	
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register mdtvsrpcsector hist to MonGroup" );

  }}//m_doMdtvsRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD 
  
}

/*----------------------------------------------------------------------------------*/
StatusCode MdtVsRpcRawDataValAlg::procHistograms()
{
  ATH_MSG_DEBUG ( "MdtVsRpcRawDataValAlg finalize()" );
  return StatusCode::SUCCESS; 
}
 
//======================================================================================//
/**  finalize */
//======================================================================================//
StatusCode MdtVsRpcRawDataValAlg::finalize() 
{ 

  StatusCode sc = ManagedMonitorToolBase::finalize();
  if(!sc.isSuccess()) return sc;


  ATH_MSG_DEBUG ( "MdtVsRpcRawDataValAlg::finalize() " );

  
  return sc;
}
