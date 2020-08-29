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
#include "StoreGate/StoreGate.h"

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
 
#include "MdtVsRpcRawDataMonitoring/MdtVsRpcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <sstream>

static const int maxPRD  =   50000;

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
StatusCode MdtVsRpcRawDataValAlg::initialize() {
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_MSG_INFO ( "in initializing MdtVsRpcRawDataValAlg" );
  // MuonDetectorManager from the conditions store
  ATH_CHECK(m_DetectorManagerKey.initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_key_mdt.initialize());
  ATH_CHECK(m_key_rpc.initialize());
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
    std::string generic_path_mdtvsrpcmonitoring = "Muon/MuonRawDataMonitoring/MDTvsRPC" ;
    MonGroup mdtrpc_shift_dqmf( this, generic_path_mdtvsrpcmonitoring+ "/Dqmf", run, ATTRIB_UNMANAGED ) ;
  
    sc = mdtrpc_shift_dqmf.getHist( m_MdtRpcZdiff, "MdtRpcZdifference" ) ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register MdtRpcZdifference hist to MonGroup" );         
    
    sc = mdtrpc_shift_dqmf.getHist( m_MdtNHitsvsRpcNHits, "MdtNHitsvsRpcNHits" ) ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register MdtNHitsvsRpcNHits hist to MonGroup" ); 
  
    //mdt stuff begin     
    int nPrdmdt  = 0;
    Identifier dig_idmdt;
  
    SG::ReadHandle<Muon::MdtPrepDataContainer> mdt_container(m_key_mdt);
    ATH_MSG_DEBUG ("****** mdt->size() : " << mdt_container->size());  

    Muon::MdtPrepDataContainer::const_iterator mdt_containerIt;
    //mdt stuff end
  
    SG::ReadHandle<Muon::RpcPrepDataContainer> rpc_container(m_key_rpc);
    
    ATH_MSG_DEBUG ("****** rpc->size() : " << rpc_container->size());  
  
    Muon::RpcPrepDataContainer::const_iterator containerIt;
  
  
    int nPrd = 0;
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
    m_MdtNHitsvsRpcNHits->Fill(Nhitsrpc,Nhitsmdt);
    
    int N_RpcHitdblPhi1 = 0;
    int N_RpcHitdblPhi2 = 0;

    // MuonDetectorManager from the conditions store
    SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
    const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
    if(MuonDetMgr==nullptr){
      ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
      return StatusCode::FAILURE; 
    } 
    
    for (containerIt = rpc_container->begin() ; containerIt != rpc_container->end() ; ++containerIt) 
      {   
	for (Muon::RpcPrepDataCollection::const_iterator rpcPrd = (*containerIt)->begin(); rpcPrd!=(*containerIt)->end(); ++rpcPrd)
	  {

	    if (nPrd<maxPRD) {

	      Identifier prd_id = (*rpcPrd)->identify();

	      int irpcstationPhi	=   int(m_idHelperSvc->rpcIdHelper().stationPhi (prd_id))  ;		   
	      int irpcstationName	=   int(m_idHelperSvc->rpcIdHelper().stationName(prd_id))  ;		   
	      int irpcstationEta	=   int(m_idHelperSvc->rpcIdHelper().stationEta (prd_id))  ;			   
	      int irpcdoubletR 	=   int(m_idHelperSvc->rpcIdHelper().doubletR   (prd_id))  ;		
	      int irpcmeasuresPhi	=   int(m_idHelperSvc->rpcIdHelper().measuresPhi(prd_id))  ;
	      // only take eta hits
	      if( irpcmeasuresPhi != 0 )continue;
	      int irpcdoubletPhi	 =   int(m_idHelperSvc->rpcIdHelper().doubletPhi(prd_id))  ;
	      int irpcdoubletZ	 =   int(m_idHelperSvc->rpcIdHelper().doubletZ(prd_id))    ;
	      int irpcstrip		 =   int(m_idHelperSvc->rpcIdHelper().strip(prd_id))       ;
	   
	    
      
      
      
	      std::string hardware_name=convertChamberName(irpcstationName,irpcstationEta,irpcstationPhi,type) ;
  	  	  
	      if (selectChambersRange(hardware_name, m_chamberName, 
				      m_idHelperSvc->rpcIdHelper().stationEta(dig_id), m_StationEta,
				      m_idHelperSvc->rpcIdHelper().stationPhi(dig_id), m_StationPhi, m_StationSize) && chambersCosmicSetup(hardware_name,m_cosmicStation)) {	 

		//define layer
                int imdt_multi_near = 0;
		if( (irpcstationName>1) && (irpcstationName<4||irpcstationName==8) ){
		  if(irpcdoubletR==1){layer_name="LowPt";imdt_multi_near=1;}
		  else {layer_name="Pivot";imdt_multi_near=2;}
		}			
		else {layer_name="HighPt";imdt_multi_near=2;
		if(irpcstationName !=4 ) imdt_multi_near=1;
		} 

		//define sector
		int side	     = 'A'     ;
		if(irpcstationEta<0){
		  side = 'C'         ;
		}
		int sector = 2 * irpcstationPhi                              ;
		if(irpcstationName==2 ||  irpcstationName==4 ) sector--  ;
                char sector_char[1000]    ;  
		sprintf(sector_char,"Sector%.2d",sector)                   ;
		sector_name =  sector_char                               ;

		layerSector_name  = sector_name + layer_name  ;

		int NetaStrips = 0 ;
        int ShiftEtaStripsDoubletZ[4];
		for(int idbz=0; idbz!= 3; idbz++){
          ShiftEtaStripsDoubletZ[idbz] = NetaStrips;
          bool isValid=false;
          Identifier id = m_idHelperSvc->rpcIdHelper().channelID(irpcstationName, irpcstationEta,irpcstationPhi,irpcdoubletR,idbz, 1, 1, 1, 1, true, &isValid, true); // last 7 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid, bool noPrint
          if (!isValid) {
            ATH_MSG_DEBUG("Could not find valid Identifier for station="<<irpcstationName<<", eta="<<irpcstationEta<<", phi="<<irpcstationPhi<<", doubletR="<<irpcdoubletR<<", doubletZ="<<idbz<<", continuing...");
            continue;
          }
		  const MuonGM::RpcReadoutElement* rpc = MuonDetMgr->getRpcReadoutElement(id);
		  if(rpc){
		    NetaStrips +=  rpc->NetaStrips();
		  } 
		}


	    
		bool histo_flag=true;
		for (std::vector<std::string>::const_iterator iter=m_layerSector_name_list.begin(); iter!=m_layerSector_name_list.end(); iter++){
		  if ( (sector_name+layerSector_name)==*iter){histo_flag=false;}
		}

		if (histo_flag){
		  // Get the RPC axis range
		
		  /////// NB !!!!!
		  // the eta strip number increases going far away from IP
		  // the phi strip numeber increases going from HV side to RO side
		  float stripzmin   =      0 ;
		  float stripzmax   = -10000 ;
		  for(int ieta=0; ieta!= 17; ieta++){
		    for(int idbz=0; idbz!= 3; idbz++){
              bool isValid=false;
              Identifier id = m_idHelperSvc->rpcIdHelper().channelID(irpcstationName, ieta,irpcstationPhi,irpcdoubletR,idbz, 1, 1, 1, 1, true, &isValid, true); // last 7 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid, bool noPrint
              if (!isValid) {
                ATH_MSG_DEBUG("Could not find valid Identifier for station="<<irpcstationName<<", eta="<<ieta<<", phi="<<irpcstationPhi<<", doubletR="<<irpcdoubletR<<", doubletZ="<<idbz<<", continuing...");
                continue;
              }
		      const MuonGM::RpcReadoutElement* rpc = MuonDetMgr->getRpcReadoutElement(id);
		      if(rpc){
			const Amg::Vector3D r1 = rpc-> globalPosition();
			float pitch = rpc-> StripPitch(0)  ;
		
			float z1 = float ( r1.z() - ( rpc -> NetaStrips() ) * pitch / 2 );
			if ( z1 < stripzmin ) {
			  stripzmin   = z1 ;
			}
			float z2 = float ( r1.z() + ( rpc -> NetaStrips() )* pitch / 2 );
			if ( z2 > stripzmax ) { stripzmax = z2 ; }	      
	      
		      } //check for nullptr
		    } //for loop in idbz 
	      	      
		  } // for loop in etastation
	  
		  // get the MDT axis range
		  float wirezmax     = -10000. ;
		  float wirezmin     = +10000. ;
		  float foundmin     =      0  ;	
		  int stname_index = irpcstationName;  
		  if (irpcstationName == 53) stname_index = MuonGM::MuonDetectorManager::NMdtStatType-2;
		  else stname_index = irpcstationName;
		  for(int eta=0; eta!=17; eta++){ 
		    const MuonGM::MdtReadoutElement* lastdescr = MuonDetMgr->getMdtReadoutElement(stname_index, eta, irpcstationPhi-1, imdt_multi_near-1);
		    if(!lastdescr)continue;
		
		    const Amg::Vector3D lastelc = lastdescr->globalPosition();
		    int NtubesPerLayerlast = lastdescr->getNtubesperlayer();
		    float z =  float(lastelc.z()) + float(NtubesPerLayerlast)/2*29.9;
		
		    if(foundmin==0){ wirezmin   =  float(lastelc.z()) - float(NtubesPerLayerlast)/2*29.9;} 
		    foundmin = 1 ; 
		    if(z>wirezmax)wirezmax=z; 
	      
		  } // loop over eta		     
		
		  m_layerSector_name_list.push_back(sector_name+layerSector_name); 
		  ATH_MSG_DEBUG ( " strip zmin  and zmax"  << stripzmin <<" "<< stripzmax );
	      
		  if (m_mdtvsrpcsectorhist)bookMDTvsRPCsectorHistograms(sector_name, layer_name, stripzmin, stripzmax, wirezmin, wirezmax);
	
		}// if histo_flag
	
		
		// Per Sector
		const MuonDQAHistList& hists1 = m_stationHists.getList( sector_name );
		TH2* mdttubevsrpcetastripsector;
 	      
		//get information from geomodel to book and fill rpc histos with the right max strip number
		const MuonGM::RpcReadoutElement* descriptor =  MuonDetMgr->getRpcReadoutElement(prd_id);
		const Amg::Vector3D stripPos = descriptor->stripPos(prd_id);
	        ATH_MSG_DEBUG ( "rpc coord" << stripPos.z() << stripPos.perp() );
		float irpcstripz = float( stripPos.z() );

		int ShiftPhiStrips	 = descriptor -> NphiStrips()    * (irpcdoubletPhi  -1 ) ;

		int ShiftEtaStrips	 = ShiftEtaStripsDoubletZ[irpcdoubletZ-1] ;

		int ShiftStrips = ShiftEtaStrips;

		//re-define for phi view
		if(irpcmeasuresPhi==1) {
		  ShiftStrips =  ShiftPhiStrips;
		}
	    
		//define sectorlogic		
		int sectorlogic = (sector-1) * 2 ; 
		if(irpcdoubletPhi==1)sectorlogic -= 1     ;
		if(sectorlogic<0)    sectorlogic += 32    ;
		if(side == 'A')      sectorlogic += 32    ;
		
		++nPrd;
		ATH_MSG_DEBUG ( " PRD number  " << nPrd );	    
	    
	    
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
			int imdt_station      =  int(m_idHelperSvc->mdtIdHelper().stationName (dig_idmdt));
			if (imdt_station != irpcstationName) continue;
			int imdt_eta          =  int(m_idHelperSvc->mdtIdHelper().stationEta  (dig_idmdt));
			if (imdt_eta     != irpcstationEta ) continue; 
			int imdt_phi          =  int(m_idHelperSvc->mdtIdHelper().stationPhi  (dig_idmdt));
			if (imdt_phi     != irpcstationPhi ) continue;
			dig_idmdt = (*mdtCollection)->identify();
			int imdt_multi     =  int(m_idHelperSvc->mdtIdHelper().multilayer  (dig_idmdt));
			// only look at near multilayer
			if(imdt_multi  != imdt_multi_near) continue;
			int imdt_adc       =  int((*mdtCollection)->adc());
			//cut on noise
			if( imdt_adc<ncutadc )continue;  
			int imdt_tdc       =  int((*mdtCollection)->tdc());
			int imdt_wire      =  int(m_idHelperSvc->mdtIdHelper().tube        (dig_idmdt));
		    
 		    
			//get mdt information from geomodel to book and fill mdtvsrpc histos with the right min and max range
			if (imdt_station == 53) imdt_station = MuonGM::MuonDetectorManager::NMdtStatType-2;
			const MuonGM::MdtReadoutElement* mdt = MuonDetMgr->getMdtReadoutElement( imdt_station,  imdt_eta+8, imdt_phi-1,  imdt_multi-1);
			int NetaTubes = mdt->getNtubesperlayer() ;			    	      
			const Amg::Vector3D elc =  mdt->globalPosition();
			float imdt_wirez =  float(elc.z());
			float tubeRadius = mdt->innerTubeRadius();
			if(imdt_wirez>=0) {imdt_wirez +=  (float(imdt_wire)-0.5-float(NetaTubes)/2 )* tubeRadius ;}
			else {imdt_wirez =  imdt_wirez -  (float(imdt_wire)-0.5-float(NetaTubes)/2 )* tubeRadius ;}
		
			//fill histos
			if(m_mdtvsrpcchamberhist){
		      
			  bool histo_flag=true;
			  for (std::vector<std::string>::const_iterator iter=m_layer_name_list.begin(); iter!=m_layer_name_list.end(); iter++){
			    if ( (hardware_name+layer_name)==*iter){histo_flag=false;}
			  }
			  if (histo_flag){
			    //booking if necessary 
			    imdt_station = irpcstationName;
			    imdt_eta     = irpcstationEta ;
			    imdt_phi     = irpcstationPhi ;
			    NetaTubes = 0;
			    if (imdt_station == 53) imdt_station = MuonGM::MuonDetectorManager::NMdtStatType-2;
			    const MuonGM::MdtReadoutElement* mdt = MuonDetMgr->getMdtReadoutElement( imdt_station,  imdt_eta+8, imdt_phi-1,  imdt_multi_near-1);
			    if(!mdt)continue; // protection
			    NetaTubes = mdt->getNtubesperlayer();
			    m_layer_name_list.push_back(hardware_name+layer_name); 
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
			double mdt_rpc_dz = imdt_wirez - irpcstripz     ;
			m_MdtRpcZdiff -> Fill ( mdt_rpc_dz ) ;
			
			++nPrdmdt;
			ATH_MSG_DEBUG ( " MdtPrepData number:  " << nPrdmdt );
	      
			if (nPrdmdt > maxPrd-1) {
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
 
	      ATH_MSG_DEBUG ( " RpcPrepData number:  " << nPrd );
 
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
    std::string generic_path_mdtvsrpcmonitoring = "Muon/MuonRawDataMonitoring/MDTvsRPC";
    MonGroup mdtrpc_shift_dqmf( this, generic_path_mdtvsrpcmonitoring+ "/Dqmf", run, ATTRIB_UNMANAGED ) ;
  
    if(newRunFlag()) 
      {
	ATH_MSG_DEBUG ( "MdtVsRpcRawDataValAlg : isNewRun" );
	// Book RAW or ESD capable histos
      
      
	// SHIFTer histograms
	// distribution of difference Zmdt - Zrpc
	float maxdz = 1500 ; // mm
	std::string MdtRpcZdiff_title = "MdtRpcZdifference" ;
	const char* MdtRpcZdiff_title_char = MdtRpcZdiff_title.c_str();
	 
	TH1* MdtRpcZdiff = new TH1I( MdtRpcZdiff_title_char, MdtRpcZdiff_title_char, 100, -maxdz, maxdz ) ;
	sc = mdtrpc_shift_dqmf.regHist( MdtRpcZdiff ) ;
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "MdtRpcZdiff Failed to register histogram " );       
	  return sc;
	  }
	MdtRpcZdiff->GetXaxis()->SetTitle("Z mdt tube - Z rpc strip [mm]" );
	MdtRpcZdiff->GetYaxis()->SetTitle("Counts") ;
	MdtRpcZdiff->SetFillColor(42);
	 
	 
	  ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << MdtRpcZdiff << MdtRpcZdiff_title.c_str() );
	  ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	  ATH_MSG_DEBUG ( "Booked MdtRpcZdifference successfully" );
	  
      
      // correlation number of MDT hits vs RPC hits
	std::string MdtNHitsvsRpcNHits_title = "MdtNHitsvsRpcNHits" ;
	const char* MdtNHitsvsRpcNHits_title_char = MdtNHitsvsRpcNHits_title.c_str();
	 
	TH2* MdtNHitsvsRpcNHits = new TH2I( MdtNHitsvsRpcNHits_title_char, MdtNHitsvsRpcNHits_title_char, 11, -1, 10, 11, -1, 10) ;
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
	 
	  ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << MdtNHitsvsRpcNHits << MdtNHitsvsRpcNHits_title.c_str() );
	  ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	  ATH_MSG_DEBUG ( "Booked MdtNHitsvsRpcNHits successfully" );
	 
     }
  }}//m_doMdtvsRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD 
  return sc;

}
/*----------------------------------------------------------------------------------*/
//input hardware name as produced from convertChamberName()
void MdtVsRpcRawDataValAlg::bookMDTvsRPCHistograms(std::string hardware_name, std::string layer_name, int binz, int binminz, int binmaxz, int binx, int binminx, int binmaxx )
{ 

  StatusCode sc = StatusCode::SUCCESS;  

 
  if( m_doMdtvsRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
  
    // hardware_name = "" ;
  
    binz =  binz / m_mdtvsrpcreducerpcnbins ;
    binx =  binx / m_mdtvsrpcreducemdtnbins ;
  
    //declare a group of histograms
    std::string generic_path_mdtvsrpcmonitoring = "Muon/MuonRawDataMonitoring/MDTvsRPC";
    MonGroup mdtvsrpc_prd_expert( this, generic_path_mdtvsrpcmonitoring+"/Chambers/"+hardware_name, run, ATTRIB_UNMANAGED );   
    MuonDQAHistList& lst = m_stationHists.getList( hardware_name );
    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for mdt vrs rpc 
 
    // mdt vs rpc - doublephi separated 
    std::string mdtvsrpc_dphi1_title       = hardware_name+"_"+layer_name+"_MDTtube_vs_RPCstrip_doublPhi1";
    std::string mdtvsrpc_dphi2_title       = hardware_name+"_"+layer_name+"_MDTtube_vs_RPCstrip_doublPhi2";	
    const char* mdtvsrpc_dphi1_title_char  = mdtvsrpc_dphi1_title.c_str()                                   ;
    const char* mdtvsrpc_dphi2_title_char  = mdtvsrpc_dphi2_title.c_str()                                   ;

    TH2 *mdttubevsrpcetastrip_doublphi1 = new TH2I(mdtvsrpc_dphi1_title_char,mdtvsrpc_dphi1_title_char, binz, binminz, binmaxz, binx, binminx, binmaxx); 		
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
  
    TH2 *mdttubevsrpcetastrip_doublphi2 = new TH2I(mdtvsrpc_dphi2_title_char,mdtvsrpc_dphi2_title_char, binz, binminz, binmaxz, binx, binminx, binmaxx); 		
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
      ATH_MSG_DEBUG ( "RUN : " << run );
    
  
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for mdt tdc for doublphi1 and 2 
    //   doubletphi 1

  
    std::string generic_path_mdttdcdoublphi1 = "/muoncosmics/MDTvsRPC/Chambers/"+hardware_name+"/"+layer_name;
    std::string mdttdcdoublphi1_title = hardware_name+"_"+layer_name+"_MDTtdc_doublPhi1";	
    const char* mdttdcdoublphi1_title_char = mdttdcdoublphi1_title.c_str();

    TH1 *mdttdcdoublphi1=new TH1I(mdttdcdoublphi1_title_char,mdttdcdoublphi1_title_char, TDCNbin/m_mdtvsrpcreducemdttdcnbins, TDCminrange, TDCmaxrange); 		
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
   
      ATH_MSG_DEBUG ( "INSIDE  bookmdttdcdoublphi1Histograms: " << mdttdcdoublphi1 << generic_path_mdttdcdoublphi1.c_str() );
      ATH_MSG_DEBUG ( "RUN : " << run );
    
    //   ---------------  doublphi2 
 
    std::string generic_path_mdttdcdoublphi2 = "/muoncosmics/MDTvsRPC/Chambers/"+hardware_name+"/"+layer_name;
    std::string mdttdcdoublphi2_title = hardware_name+"_"+layer_name+"_MDTtdc_doublPhi2";	
    const char* mdttdcdoublphi2_title_char = mdttdcdoublphi2_title.c_str();

    TH1 *mdttdcdoublphi2=new TH1I(mdttdcdoublphi2_title_char,mdttdcdoublphi2_title_char, TDCNbin/m_mdtvsrpcreducemdttdcnbins, TDCminrange, TDCmaxrange);	
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
   
      ATH_MSG_DEBUG ( "INSIDE  bookmdttdcdoublphi2Histograms: " << mdttdcdoublphi2 << generic_path_mdttdcdoublphi2.c_str() );
      ATH_MSG_DEBUG ( "RUN : " << run );
    
  
  }}//m_doMdtvsRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD   
   	
}

/*----------------------------------------------------------------------------------*/
void MdtVsRpcRawDataValAlg::bookMDTvsRPCsectorHistograms(std::string sector_name, std::string layer_name, float stripzmin, float stripzmax,float wirezmin, float wirezmax )
{ 
 
  StatusCode sc = StatusCode::SUCCESS;   
 
  if( m_doMdtvsRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) {
  		  
    //declare a group of histograms
    std::string generic_path_mdtvsrpcmonitoring = "Muon/MuonRawDataMonitoring/MDTvsRPC";
    MonGroup mdtvsrpc_prd_shift( this, generic_path_mdtvsrpcmonitoring+"/Sectors/"+sector_name, run, ATTRIB_UNMANAGED );   
    MuonDQAHistList& lst = m_stationHists.getList(sector_name);
    
    ////////////////////////////////////////////////////////////////////////////////////// 
    //histo path for mdt vrs rpc 
    std::string generic_path_mdtvsrpcsector = generic_path_mdtvsrpcmonitoring+"/Sectors/"+sector_name+layer_name;
    std::string mdtvsrpcsector_title = sector_name+"_"+layer_name+"_MDTtube_vs_RPCstrip";	 
    const char* mdtvsrpcsector_title_char = mdtvsrpcsector_title.c_str();
  
    int MDTvsRPCNbinx = int( (-stripzmin+stripzmax) / 30 / m_mdtvsrpcreducemdtnbins)  ;
    int MDTvsRPCNbinz = int( (-wirezmin +wirezmax ) / 30 / m_mdtvsrpcreducerpcnbins)  ;
   
    TH2 *mdtvsrpcsector=new TH2I(mdtvsrpcsector_title_char,mdtvsrpcsector_title_char, MDTvsRPCNbinz, stripzmin, stripzmax , MDTvsRPCNbinx, wirezmin, wirezmax);    		
    lst.addHist(mdtvsrpcsector); 
  
    mdtvsrpcsector->SetOption("COLZ");
    mdtvsrpcsector->SetFillColor(42);  
    mdtvsrpcsector->SetMarkerColor(1);  
    mdtvsrpcsector->SetMarkerStyle(21);   
    mdtvsrpcsector->GetXaxis()->SetTitle("<--- Side C                Rpc Eta strip z [mm]        Side A --->"	);
    mdtvsrpcsector->GetYaxis()->SetTitle("<--- Side C                Mdt wire z [mm]	       Side A --->"	);
  
      ATH_MSG_DEBUG ( "INSIDE  bookMDTvsRPCsectorHistograms: " << mdtvsrpcsector << generic_path_mdtvsrpcsector.c_str() );
      ATH_MSG_DEBUG ( "RUN : " << run );
     
    sc = mdtvsrpc_prd_shift.regHist(mdtvsrpcsector);  		     	
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register mdtvsrpcsector hist to MonGroup" );

  }}//m_doMdtvsRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD 
  
}
