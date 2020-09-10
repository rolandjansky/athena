/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <cmath>

#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"  
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "RpcRawDataMonitoring/RpcGlobalUtilities.h"
 
namespace RpcGM 
{
std::vector<int>  RpcStripShift(const MuonGM::MuonDetectorManager* muonMgr, const RpcIdHelper& rpcIdHelper, Identifier prdcoll_id, int  irpctriggerInfo) 
{
   
  int irpcstationPhi	 =   int(rpcIdHelper.stationPhi(prdcoll_id))   ;
  int irpcstationName	 =   int(rpcIdHelper.stationName(prdcoll_id))  ;	     
  int irpcstationEta	 =   int(rpcIdHelper.stationEta(prdcoll_id))   ;			     
  int irpcdoubletR  	 =   int(rpcIdHelper.doubletR(prdcoll_id))	  ;		     
  int irpcdoubletZ  	 =   int(rpcIdHelper.doubletZ(prdcoll_id))	  ;
  int irpcdoubletPhi	 =   int(rpcIdHelper.doubletPhi(prdcoll_id))   ;
  int irpcgasGap	 =   int(rpcIdHelper.gasGap(prdcoll_id))	  ;
  int irpcmeasuresPhi	 =   int(rpcIdHelper.measuresPhi(prdcoll_id))  ;
  int irpcstrip		 =   int(rpcIdHelper.strip(prdcoll_id))	  ;
   
  //get information from geomodel to book and fill rpc histos with the right max strip number
  
  const MuonGM::RpcReadoutElement* descriptor = muonMgr->getRpcReadoutElement(prdcoll_id);

  std::vector<int>  rpcstriptot  ;
  
  int NphiStrips	    = descriptor -> NphiStrips()* 2		     ;
  int ShiftPhiStrips        = descriptor -> NphiStrips()*(irpcdoubletPhi-1)  ;
  int NetaStrips	    = 0						     ;
  int ShiftEtaStrips        = 0						     ;
  int ShiftStrips	    = 0						     ;
  int NetaStripsTot         = 0						     ;
  int NetaStripsTotSideA    = 0						     ;
  int NetaStripsTotSideC    = 0						     ;
  int ShiftEtaStripsTot     = 0						     ;
  int Nbin  	            = 0						     ;
  int EtaStripSign          = 0						     ;
  int SectorLogic	    = 0						     ;
  int Side  	            = 0						     ;
  int PanelIndex	    = 0						     ;
  int Settore	            = 0						     ;
  int PlaneTipo	            = 0						     ;
  int strip_dbindex         = 0						     ;
  int ShiftPhiTot_db        = 0						     ;
  int NphiStripsTotSideA    = 0					             ;
  int NphiStripsTotSideC    = 0					             ;
  int NetaPanelsTot         = 0					             ;
  int ShiftEtaPanelsTot     = 0					             ;
  int NetaPanelsTotSideA    = 0					             ;
  int NetaPanelsTotSideC    = 0					             ;
  int ShiftStripPhiAtlas    = 0					             ;
  
  //1***
  //evaluate sector logic
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME
  SectorLogic  = (irpcstationPhi - 1) * 4 ; 
  SectorLogic +=  irpcdoubletPhi          ;
  Settore      = (irpcstationPhi -1) * 2  ;
  if( (irpcstationName==3||irpcstationName==5||irpcstationName>7) && (irpcstationName<11) ){
    SectorLogic+=2;
    Settore++; 
  }
  SectorLogic   = SectorLogic - 2  ; 
  Settore = Settore +1 ;
  if(SectorLogic==-1)SectorLogic=31;
  if(irpcstationEta>0){Side = 1     ;SectorLogic+=32;}
  
  //evaluate plane type
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME
  if( irpcstationName==2||irpcstationName==3||irpcstationName==8||irpcstationName==53 ){
    if(irpcdoubletR==1){
      PlaneTipo=0;
    }
    else {PlaneTipo=1;}
  }			
  else {PlaneTipo=2;}
  //Extension feet Pivot
  if(irpcdoubletR==2)PlaneTipo=1;
  //BML7 assigned to pivot 
  if( irpcstationName==2 && ( (std::abs(irpcstationEta)==7)||(irpcstationPhi==7&&std::abs(irpcstationEta)==6) ) )PlaneTipo=1;
  
  //evaluate strip shift
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME
  /////// NB !!!!!
  // the eta strip number increases going far away from IP
  // the phi strip number increases going from HV side to RO side
   

      int krpcstationName    = irpcstationName ;
      int krpcstationNameMin = irpcstationName ;
      int krpcstationNameMax = irpcstationName ;
      if((irpcstationName==2||irpcstationName==53)&&irpcstationPhi==7){ // sector 13 with BME
         krpcstationNameMin = 1; 
         krpcstationNameMax = 2;	 
      }  
      if(irpcstationName>=9&&irpcstationName<=10&&irpcdoubletR==1){ // feet extension
         krpcstationNameMin =  9; 
         krpcstationNameMax = 10; 
      }  
      if(irpcstationName>=8&&irpcstationName<=10&&irpcdoubletR==2){ // feet extension
         krpcstationNameMin =  8; 
         krpcstationNameMax = 10; 
      }    
   
      for(int jrpcstationName=krpcstationNameMin; jrpcstationName!=krpcstationNameMax+1; jrpcstationName++){
       if (jrpcstationName>10 && jrpcstationName!=53) continue; 
       krpcstationName = jrpcstationName ;
       if(krpcstationName==1)krpcstationName = 53 ; //BME     
    
    
    for(int keta=0; keta!= 9; keta++){
      int ieta = keta ;
      if(irpcstationEta<0&&ieta==0)continue     ; 
      if(irpcstationEta<0&&ieta >0)ieta = - keta;// 

        int krpcdoubletR=irpcdoubletR;
	
	if(PlaneTipo==0){
	 if(krpcstationName==2&&std::abs(ieta)==7         )continue;
	 if(krpcstationName==2&&std::abs(ieta)==6&&irpcstationPhi==7)continue;
	 if(krpcstationName==4||krpcstationName==5||krpcstationName==9||krpcstationName==10)continue;  
        }
	else if(PlaneTipo==1){
	 if(krpcstationName==2&&std::abs(ieta)==7         ){krpcdoubletR=1;}
	 else if(krpcstationName==2&&std::abs(ieta)==6&&irpcstationPhi==7){krpcdoubletR=1;}
	 else{ krpcdoubletR=2;}
        }
	else if(PlaneTipo==2){
	 if(krpcstationName==2||krpcstationName==3||krpcstationName==8||krpcstationName==53)continue;
        }
        
	for(int idbz=1; idbz!= 4; idbz++){
      Identifier rpcId = rpcIdHelper.channelID(krpcstationName, ieta, irpcstationPhi, krpcdoubletR, idbz, 1, 1, 1, 1); // last 4 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip
      if (!rpcId.is_valid()) continue;
    	const MuonGM::RpcReadoutElement* rpc = muonMgr->getRpcReadoutElement(rpcId);
    	if(rpc){	
		
	  if ( idbz != rpc->getDoubletZ() ) continue ;
	  
    	  if( ieta==irpcstationEta ){if(krpcstationName == irpcstationName ){
    	    if( idbz==irpcdoubletZ ){ 
	     ShiftEtaStrips    = NetaStrips;
	     if(irpcstationEta<0){ 
	      ShiftEtaStripsTot = NetaStripsTotSideC  ;  
	      ShiftPhiTot_db    = NphiStripsTotSideC  ;
	      ShiftEtaPanelsTot = NetaPanelsTotSideC  ;
	     }
	     else{ 
	      ShiftEtaStripsTot = NetaStripsTotSideA  ;  
	      ShiftPhiTot_db    = NphiStripsTotSideA  ;
	      ShiftEtaPanelsTot = NetaPanelsTotSideA  ;
	     }
	    }
    	    NetaStrips    +=  rpc->NetaStrips()  ;
    	  }}
	  
    	  NetaStripsTot       +=  rpc->NetaStrips()  ;
    	  NetaPanelsTot       ++                     ;
	  if(irpcstationEta<0){
	   NetaStripsTotSideC  +=  rpc->NetaStrips()  ;
	   NphiStripsTotSideC  +=  rpc->NphiStrips()  ;
    	   NetaPanelsTotSideC  ++                     ;
	  }
	  else{ 
	   NetaStripsTotSideA  +=  rpc->NetaStrips()  ;
	   NphiStripsTotSideA  +=  rpc->NphiStrips()  ;
    	   NetaPanelsTotSideA  ++                     ;	

	  }
    	
	  
        } //check if rpc is nullptr
      } //for loop in idbz
     } //for loop in krpcstationName
    } // for loop in etastation     
  
  Nbin	      = NetaStrips     ;
  ShiftStrips = ShiftEtaStrips ;
  //re-define for phi view
  if(irpcmeasuresPhi==1) {
    Nbin = NphiStrips ;	   
    ShiftStrips =  ShiftPhiStrips;
  }
     
  EtaStripSign =  1	                   ; 
  if(irpcstationEta<0)  EtaStripSign = -1  ; 
  		
  if(irpcdoubletR==1){
   if     (irpcstationName==2||irpcstationName==53 ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 )        ;}
   else if(irpcstationName==3                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) + 2*64 ;}
   else if(irpcstationName==8                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) + 2*64 ;}
   else if(irpcstationName==4		           ){ShiftStripPhiAtlas = (2*64+80*2) * ( irpcstationPhi -1 )	     ;}
   else{ShiftStripPhiAtlas = (2*64+80*2) * ( irpcstationPhi -1 ) + 2*80 ;} 
   //BML7 assigned to pivot 
   if( irpcstationName==2 && std::abs(irpcstationEta)==7){
    ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 );
    if(irpcstationPhi==8)ShiftStripPhiAtlas +=4*16;
   }
   if( irpcstationName==2 && irpcstationPhi==7&&std::abs(irpcstationEta)==6 ){
    ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) +2*16      ;
   }
  } 		
  if(irpcdoubletR==2){
   if(irpcstationPhi<=6){   
    if     (irpcstationName==2                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 )        ;}
    else if(irpcstationName==3                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) + 2*64 ;} 
    else if(irpcstationName>=8&&irpcstationName<=10 ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) + 2*64 ;}                    
   }
   else if(irpcstationPhi==7){   
    if     (irpcstationName==2||irpcstationName==53 ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 )+2*16     ;}
    else if(irpcstationName>=8&&irpcstationName<=10 ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 )+2*16+2*64;}
   }
   else if(irpcstationPhi==8){   
    if     (irpcstationName==2                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) +4*16      ;}
    else if(irpcstationName==3                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) +4*16+2*64 ;}
   }
  }
  //2***
  // cool db strip index
  
  if(irpcmeasuresPhi==0) {
    strip_dbindex = ( ShiftEtaStripsTot + irpcstrip ) * EtaStripSign ;
  }
  else {
    if ( irpcstationEta<0 ) { strip_dbindex= - NetaStripsTotSideC - ShiftPhiTot_db - irpcstrip ;}
    else { strip_dbindex = NetaStripsTotSideA + ShiftPhiTot_db + irpcstrip; }
  } 
  if ( irpcstationEta>=0 ) strip_dbindex = strip_dbindex -1;
  
  //3***
  //bin panel number for summary plots 
  int irpcstationName_index = irpcdoubletR -1 ; // 0 LowPt, 1 Pivot, 2 HighPt
  if ( irpcstationName !=2 && irpcstationName !=3 && irpcstationName !=8 && irpcstationName !=53) irpcstationName_index += 2 ;  
  

  PanelIndex = irpcmeasuresPhi + (irpcgasGap-1)*2 + (irpcdoubletPhi-1)*4 + (irpcdoubletZ-1)*8 + (irpcstationName_index)*24 
    + ( ( std::abs(irpcstationEta) ) )*72 ;
  // exception station name=53, assume irpcstationEta = 0 ;
  if(irpcstationName ==53)
  PanelIndex = irpcmeasuresPhi + (irpcgasGap-1)*2 + (irpcdoubletPhi-1)*4 + (irpcdoubletZ-1)*8 + (irpcstationName_index)*24;
  // exception for special chambers sectors 12 and 14:
  if ( irpcstationName==10 ) { 
    // convention: BOG <-> doubletZ=3 <-> (3-1)*8=16
    PanelIndex = irpcmeasuresPhi + (irpcgasGap-1)*2 + (irpcdoubletPhi-1)*4 + 16 + (irpcstationName_index)*24 
      + ( ( std::abs(irpcstationEta) ) )*72 ;
  }
  if ( (irpcdoubletR==2) && (irpcstationName==9 || irpcstationName==10) ) {
    // convention: chambers of RPC upgrade -> eta = eta + 7
    PanelIndex = PanelIndex + 7*72 ;
  }
  
  if(irpcstationEta<0) PanelIndex = -PanelIndex ; 
  
  PanelIndex+= 1000*irpctriggerInfo	;



  //4***
  //panel and tower consecutive index

  int tower_dbindex   = 0 ;  
  int panel_dbindex   = 0 ; 
   
  int lastname        = 0 ;
  int laststationEta  = 0 ; 
  int lastdoubletZ    = 0 ; 
   
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME
 
  for(int iphi = 1; iphi != irpcstationPhi+1; iphi++ ){ 
    
    lastname    = 10 ;    
    if(iphi==irpcstationPhi){
      lastname    = irpcstationName ;
      if(lastname==53)lastname=1;
    }
    
    for(int kname=      1; kname!= lastname+1 ; kname++){
      int iname = kname ;
      if (kname == 1) iname= 53;
      if (iname>10 && iname!=53) continue; 
      if( iname==6 || iname==7 ) continue; 
       ;  

      laststationEta =  8;   
      if(iname==irpcstationName&&iphi==irpcstationPhi){
	laststationEta    = std::abs(irpcstationEta) ;
      }
   
      for(int ieta = 0; ieta != laststationEta+1; ieta++ ){
        if(ieta == 0 && irpcstationEta<0)continue;
	int keta = ieta ;
	if(irpcstationEta<0){
	  keta=-ieta;
	} 


       //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME

        int ir  =  irpcdoubletR;  
        
	if(PlaneTipo==0){
	 if(iname==2&&ieta==7         )continue;
	 if(iname==2&&ieta==6&&iphi==7)continue;
	 if(iname==4||iname==5||iname==9||iname==10)continue;  
        }
	else if(PlaneTipo==1){
	 if(irpcstationName==2&&std::abs(irpcstationEta)==7  	         )ir=2;
	 if(irpcstationName==2&&std::abs(irpcstationEta)==6&&irpcstationPhi==7)ir=2;
	 if(iname==2&&ieta==7         )ir=1;
	 if(iname==2&&ieta==6&&iphi==7)ir=1;
        }
	else if(PlaneTipo==2){
	 if(iname==2||iname==3||iname==8||iname==53)continue;
        }
	
	
	
	lastdoubletZ   = 3 ;  
	if(iname==irpcstationName&&iphi==irpcstationPhi&&keta==irpcstationEta){
	  lastdoubletZ      = irpcdoubletZ   ;
	}    
 
	for(int iz   =      1; iz   !=   lastdoubletZ+1; iz++	){ 

    Identifier rpcId = rpcIdHelper.channelID(iname, keta, iphi, ir, iz, 1, 1, 1, 1); // last 4 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip
    if (!rpcId.is_valid()) continue;
	  const MuonGM::RpcReadoutElement* rpc = muonMgr->getRpcReadoutElement(rpcId);
    
	  if(!rpc)continue;
	  
	  panel_dbindex++;
	  
	  if(iz==1)tower_dbindex++;
	  
	}}}}
				    	  
  rpcstriptot.push_back(NphiStrips	      );  // 0
  rpcstriptot.push_back(ShiftPhiStrips	      );  // 1
  rpcstriptot.push_back(NetaStrips	      );
  rpcstriptot.push_back(ShiftEtaStrips	      );
  rpcstriptot.push_back(ShiftStrips	      );
  rpcstriptot.push_back(NetaStripsTot	      );
  rpcstriptot.push_back(NetaStripsTotSideA    );
  rpcstriptot.push_back(NetaStripsTotSideC    );
  rpcstriptot.push_back(ShiftEtaStripsTot     );
  rpcstriptot.push_back(Nbin		      );
  rpcstriptot.push_back(EtaStripSign	      );
  rpcstriptot.push_back(SectorLogic	      );
 
  rpcstriptot.push_back(Side		      );
  rpcstriptot.push_back(PanelIndex	      );   //13
  rpcstriptot.push_back(Settore 	      );
  rpcstriptot.push_back(PlaneTipo	      );   //15
  rpcstriptot.push_back(strip_dbindex	      );   //16
  rpcstriptot.push_back(NphiStripsTotSideA    );
  rpcstriptot.push_back(NphiStripsTotSideC    ); 
  rpcstriptot.push_back(NetaPanelsTot         ); 
  rpcstriptot.push_back(ShiftEtaPanelsTot     ); 
  rpcstriptot.push_back(NetaPanelsTotSideC    );
  rpcstriptot.push_back(NetaPanelsTotSideA    );   //22 
  rpcstriptot.push_back(panel_dbindex         );
  rpcstriptot.push_back(tower_dbindex         );   //24
  rpcstriptot.push_back(ShiftStripPhiAtlas    );   //25
  
  return  rpcstriptot ;

}
std::vector<std::string>    RpcLayerSectorSideName(const RpcIdHelper& rpcIdHelper, Identifier prdcoll_id, int  irpctriggerInfo)
{
  
  
  std::string SideSector    ;
  int  sector               ;
  char sector_char[1000]    ; 
  char dblZ_char[100]       ;
  char dblPhi_char[100]     ;
  
 
  std::vector<std::string>  layersectorside_name ;
   
  
  std::string sector_name               ;   
  std::string layer_name		   ;
  std::string layer_name_ee_pp		   ;
  std::string layer_name_e_p		   ;
  std::string layertodraw1_name 	   ;
  std::string layertodraw2_name 	   ;
  std::string layervslayer_name 	   ;
  std::string layer0_name		   ;
  std::string layer1_name		   ;
  std::string layer2_name		   ;
  std::string layerPhivsEta_name	   ;
  std::string layerPhivsEtaSector_name	   ;
  std::string layeronly_name               ;
  std::string HVorROsideleft  = "HV side"  ;
  std::string HVorROsideright = "RO side"  ;
  std::string layer_name_panel             ;
  std::string sector_dphi_layer            ; 
  	       
 
  int irpcstationPhi	=   int(rpcIdHelper.stationPhi(prdcoll_id))   ;	      
  int irpcstationName	=   int(rpcIdHelper.stationName(prdcoll_id))  ;	      
  int irpcstationEta	=   int(rpcIdHelper.stationEta(prdcoll_id))   ;		      
  int irpcdoubletR  	=   int(rpcIdHelper.doubletR(prdcoll_id))	 ;	      
  int irpcdoubletZ  	=   int(rpcIdHelper.doubletZ(prdcoll_id))	 ;
  int irpcdoubletPhi	=   int(rpcIdHelper.doubletPhi(prdcoll_id))   ;
  int irpcgasGap	=   int(rpcIdHelper.gasGap(prdcoll_id))	 ;
  int irpcmeasuresPhi	=   int(rpcIdHelper.measuresPhi(prdcoll_id))  ;
  
  sprintf(dblZ_char    ,"_dblZ%d"    ,irpcdoubletZ  );
  sprintf(dblPhi_char  ,"_dblPhi%d"  ,irpcdoubletPhi);
  	    
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME
  if( irpcstationName==2 || irpcstationName==3 || irpcstationName==8 || irpcstationName==53 ){
    if(irpcdoubletR==1){
      layer_name="LowPt";
    }
    else {layer_name="Pivot";}
  }			
  else {layer_name="HighPt";}
  if(irpcdoubletR==2)layer_name="Pivot";
  //BML7 assigned to pivot 
  if( irpcstationName==2 && ( (std::abs(irpcstationEta)==7)||(irpcstationPhi==7&&std::abs(irpcstationEta)==6) ) )layer_name="Pivot";
	    
  if(irpcstationName==3||irpcstationName==5||irpcstationName==8||irpcstationName==9||irpcstationName==10){
    HVorROsideleft  = "RO side" ;
    HVorROsideright = "HV side" ;
  }
  
  layer_name_ee_pp = layer_name ;    
  //define if rpc hits from gasgap=0,1 or trigger hits
  if(irpctriggerInfo==0){
    if(irpcgasGap==1){
      layer_name+="0";layertodraw1_name="GasGap1";layertodraw2_name="GasGap2";
    }
    else {layer_name+="1";}	  
  }
  else if(irpctriggerInfo==100){
    layer_name="HighPt_TriggerFromLowPt";
    layertodraw1_name="TriggerIn";
    layertodraw2_name="TriggerOut LowPt";
  }
  else if(irpctriggerInfo==106){
    layer_name="HighPt_TriggerOut"; 
    layertodraw1_name="TriggerOut";
    layertodraw2_name="GasGap1or2 Pivot";
  }
  else if(irpctriggerInfo==6||irpctriggerInfo==106){
    layer_name="LowPt_TriggerOut"; 
    layertodraw1_name="TriggerOut";
    layertodraw2_name="GasGap1or2 Pivot";
  }
  else {
    layer_name+="_NotKnown"; ;
  }
            
  layeronly_name    = layer_name ;
  layer_name_e_p    = layer_name ;
  layer_name_panel  = layeronly_name + dblPhi_char + dblZ_char;
  sector_dphi_layer = layer_name + dblPhi_char;
  // sector_dphi_layer.insert(8, "_") ;
  
  if(irpcmeasuresPhi==0){
    layer_name+=dblPhi_char;
    layer_name+="_Eta";
    layer_name_e_p+="_Eta";
    layer_name_panel+="_Eta";
    layer_name_ee_pp+=dblPhi_char;
    layer_name_ee_pp+="_Eta";
    
    ////////////////
    
    if(irpctriggerInfo==0){
      layervslayer_name = layer_name_ee_pp+"2vsEta1" ;	  
    }
    else if(irpctriggerInfo==100){
      layervslayer_name = layer_name_ee_pp+"HPtTrInvsLPtTrOut" ;
    }
    else if(irpctriggerInfo==  6){
      layervslayer_name = layer_name_ee_pp+"vsLPtTrOut" ;
    }
    else if(irpctriggerInfo==106){
      layervslayer_name = layer_name_ee_pp+"vsHPtTrOut" ;
    }
    else {
      layervslayer_name = layer_name_ee_pp+"_NotKnown"; ;
    }
    
    ////////////////
    
    
    layer0_name="<--- IP  Rpc Eta strip  EC --->"	     ;
    layer1_name="<--- IP  Rpc Eta strip  "+layertodraw1_name+"  EC --->";
    layer2_name="<--- IP  Rpc Eta strip  "+layertodraw2_name+"  EC --->";	       
  }
  else{
    layer0_name=HVorROsideleft+"  Rpc Phi strip  "+ HVorROsideright    ;
    layer_name+=dblZ_char ;
    layer_name+="_Phi"    ;
    layer_name_e_p+="_Phi";
    layer_name_panel+="_Phi";
    layer_name_ee_pp+=dblZ_char;
    layer_name_ee_pp+="_Phi"   ;
    layerPhivsEta_name	 = layer_name_e_p    + "vsEta"		       ;
    
    ////////////////
    
    if(irpctriggerInfo==0){
      layervslayer_name = layer_name_ee_pp+"2vsPhi1" ;	  
    }
    else if(irpctriggerInfo==100){
      layervslayer_name = layer_name_ee_pp+"HighPtTrInvsLowPtTrOut" ;
    }
    else if(irpctriggerInfo==  6){
      layervslayer_name = layer_name_ee_pp+"LowPtvsLowPtTrOut" ;
    }
    else if(irpctriggerInfo==106){
      layervslayer_name = layer_name_ee_pp+"HighPtvsHighPtTrOut" ;
    }
    else {
      layervslayer_name = layer_name_ee_pp+"_NotKnown"; ;
    }
    
    ////////////////
    
    layerPhivsEtaSector_name  = sector_name + layer_name_e_p + "vsEta" ;
    layer1_name="<--- DBL_PHI0 Rpc Phi strip  "+layertodraw1_name+"  DBL_PHI1 --->";
    layer2_name="<--- DBL_PHI0 Rpc Phi strip  "+layertodraw2_name+"  DBL_PHI1 --->";	    
  }
	   

  	       
  //sector
  if(irpcstationEta>0){
    SideSector = "A"	 ; 
  }
  else{
    SideSector = "C"	 ; 
  }
	    
  sector = 2 * irpcstationPhi 			             ;
  if(irpcstationName==2 ||  irpcstationName==4 ) sector--    ;
  sprintf(sector_char,"Sector%.2d",sector)		     ;
  SideSector =  sector_char  			             ;
  
  sector_dphi_layer = SideSector + "_" + sector_dphi_layer   ;	   
	   
  layersectorside_name.push_back(layer_name		 ); // 0	  
  layersectorside_name.push_back(layertodraw1_name	 );	  
  layersectorside_name.push_back(layertodraw2_name	 );	  
  layersectorside_name.push_back(layervslayer_name	 );	  
  layersectorside_name.push_back(layer0_name		 );	  
  layersectorside_name.push_back(layer1_name		 );	  
  layersectorside_name.push_back(layer2_name		 );	  
  layersectorside_name.push_back(layerPhivsEta_name	 );	  
  layersectorside_name.push_back(layerPhivsEtaSector_name);	  
  layersectorside_name.push_back(SideSector              );	  
  layersectorside_name.push_back(layeronly_name          );
  layersectorside_name.push_back(layer_name_panel        );	
  layersectorside_name.push_back(sector_dphi_layer       );  //12
  
  return  layersectorside_name;

} 
}
