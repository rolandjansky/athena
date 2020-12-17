/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @Class   : VP1TriggerHandleL2
 *
 * @brief   : L2 Trigger Data Handle
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 * @author  : Edward Moyse    <edward.moyse@cern.ch>
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/VP1TriggerHandleL2.h"

//L2 includes
#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFIsolationContainer.h"
#include "TrigMuonEvent/CombinedMuonFeatureContainer.h"
#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"

//Other includes
#include <QString>

//DEFs
const float ZERO_LIMIT = 0.00001;


//Processing L2 data handle item
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerHandleL2::processitem()  
{
  //Vectors Data Structure Init ------------------------------>
  std::vector<QString> headerID(3), barrelID, endcapID, generalID, 
                       barrelData, endcapData, generalData;
  
  //Define Header IDs (thus pre-define vector data structure)
  headerID[0]=QString("Barrel");
  headerID[1]=QString("Endcap");
  headerID[2]=QString("General");
  
  //Clear Export Vector
  m_trigData.clear();
  //----------------------------------------------------------<
  
  //Muon feature iterator
  std::vector<const MuonFeatureDetails*>::const_iterator itMfd;
  //bool off_match = false; never used, code commented out below
  for(itMfd=m_containerL2.begin(); itMfd!=m_containerL2.end(); itMfd++) {
    //Check ID
    int id = (*itMfd)->id();
    if(id!=1) continue; //muFast_Muon
    
    //RoI System
    int systemID = (*itMfd)->roi_system(); //0:barrel, 1:endcap, 2:forward
        
    //Flow
    bool isL1hitThere             = false;
    bool isL1emuOkForTriggerPlane = false;
    
    int nRPC = (*itMfd)->pad_hit_onlineId().size();
    
    int nTGCMidRho = (*itMfd)->tgc_Mid_rho_N();
    int nTGCMidPhi = (*itMfd)->tgc_Mid_phi_N();
    
    float TGCMidRhoChi2 = (*itMfd)->tgc_Mid_rho_chi2();
    float TGCMidPhiChi2 = (*itMfd)->tgc_Mid_phi_chi2();
    
    //Process systems
    if(systemID==0) { //RPC
      float rpc1_z = (*itMfd)->rpc1_z();
      float rpc2_z = (*itMfd)->rpc2_z();
      const float NO_VALUE = 99999;
      if(nRPC!=0) isL1hitThere = true;
      if(fabs(rpc1_z-NO_VALUE) > ZERO_LIMIT && fabs(rpc1_z) > ZERO_LIMIT && fabs(rpc2_z-NO_VALUE) > ZERO_LIMIT && fabs(rpc2_z) > ZERO_LIMIT) {
	     isL1emuOkForTriggerPlane = true;
	    }
      
      //Dump to vectors
      barrelID.push_back("RPC-1: Z");
      barrelData.push_back(QString::number(rpc1_z));
            
      barrelID.push_back("RPC-2: Z");
      barrelData.push_back(QString::number(rpc2_z));
            
      barrelID.push_back("RPC: nHits");
      barrelData.push_back(QString::number(nRPC));
    } else { //TGC
      const float NO_VALUE = -99999;
      if(nTGCMidRho!=0 && nTGCMidPhi!=0) isL1hitThere = true;
      if(fabs(TGCMidRhoChi2 - NO_VALUE) > ZERO_LIMIT && fabs(TGCMidPhiChi2 - NO_VALUE) > ZERO_LIMIT) isL1emuOkForTriggerPlane = true;
      
      //Dump to vectors
      endcapID.push_back("TGC Mid nHits: Rho");
      endcapData.push_back(QString::number(nTGCMidRho));
      
      endcapID.push_back("TGC Mid nHits: Phi");
      endcapData.push_back(QString::number(nTGCMidPhi));
            
      endcapID.push_back("TGC Mid Chi2: Rho");
      endcapData.push_back(QString::number(TGCMidRhoChi2));
      
      endcapID.push_back("TGC Mid Chi2: Phi");
      endcapData.push_back(QString::number(TGCMidPhiChi2));
    }
    
    //Loop over each MDT tube
    std::vector<float> mdt_tube_residual = (*itMfd)->mdt_tube_residual();
    std::vector<float> mdt_tube_r        = (*itMfd)->mdt_tube_r();
    std::vector<float> mdt_tube_z        = (*itMfd)->mdt_tube_z();
    
    //INIT counters
    int n_mdt_hits_inner  = 0;
    int n_mdt_hits_middle = 0;
    int n_mdt_hits_outer  = 0;
    
    //Process systems
    if(systemID==0) { //Barrel
      for(int i_tube=0; i_tube<(int)mdt_tube_residual.size(); i_tube++) {
	      QString tube;
        float res = mdt_tube_residual[i_tube];
        float r = mdt_tube_r[i_tube];
        int imr = 2;
	
	      //Flags
        if     (r<650) { imr=0; }
        else if(r<850) { imr=1; }
        
	//Dump to vectors
        if(imr==0) {
          n_mdt_hits_inner++;
	        tube = (QString::number(i_tube));
          barrelID.push_back(QString("muFast_MDT_Inn_residual_barrel Tube No-")+tube);
          barrelData.push_back(QString::number(res));
          
          /**if(off_match){
	          tube = (QString::number(i_tube));   
	          barrelID.push_back(QString("muFast_MDT_Inn_residual_barrel_OffMatch Tube No-")+tube);
            barrelData.push_back(QString::number(res));
          } **/
        }
        else if(imr==1) {
          n_mdt_hits_middle++;
	        tube = (QString::number(i_tube));
          barrelID.push_back(QString("muFast_MDT_Mid_residual_barrel Tube No-")+tube);
          barrelData.push_back(QString::number(res));
	  
          /**if(off_match){
	          tube = (QString::number(i_tube));
            barrelID.push_back(QString("muFast_MDT_Mid_residual_barrel_OffMatch Tube No-")+tube);
            barrelData.push_back(QString::number(res));
	        } **/
        }
        else if(imr==2) {
          n_mdt_hits_outer++;
	        tube = (QString::number(i_tube));
          barrelID.push_back(QString("muFast_MDT_Out_residual_barrel tube No-")+tube);
          barrelData.push_back(QString::number(res));
          
          /**if(off_match){
            tube = (QString::number(i_tube));
            barrelID.push_back(QString("muFast_MDT_Out_residual_barrel_OffMatch tube No-")+tube);
            barrelData.push_back(QString::number(res));
	        } **/
        }
      }
    }//END: systemID=0
    else{ //Endcap
      for(int i_tube=0; i_tube<(int)mdt_tube_residual.size(); i_tube++) {
	      QString tube;
        float z = mdt_tube_z[i_tube];
        int imr = 2;
	
        if     ( fabs(z) < 10000 ) { imr=0; }
        else if( fabs(z) < 15000 ) { imr=1; }
	
	//Dump to vectors
        if(imr == 0) {
          n_mdt_hits_inner++;
	        tube = (QString::number(i_tube));
          endcapID.push_back(QString("muFast_MDT_Inn_residual_endcap Tube No-")+tube);
          endcapData.push_back(QString::number(TGCMidRhoChi2));
          
          /**if(off_match){
	          tube = (QString::number(i_tube));
            endcapID.push_back(QString("muFast_MDT_Inn_residual_endcap Tube No-")+tube);
            endcapData.push_back(QString::number(TGCMidRhoChi2));
	        } **/
        }
        else if(imr == 1) {
          n_mdt_hits_middle++;
	        tube = (QString::number(i_tube));
          endcapID.push_back(QString("muFast_MDT_Mid_residual_endcap Tube No-")+tube);
          endcapData.push_back(QString::number(TGCMidRhoChi2));
          
          /**if(off_match){
	          tube = (QString::number(i_tube));
            endcapID.push_back(QString("muFast_MDT_Mid_residual_endcap_OffMatch Tube No-")+tube);
            endcapData.push_back(QString::number(TGCMidRhoChi2));
	        } **/
        } else if(imr == 2) {
          n_mdt_hits_outer++;
	        tube = (QString::number(i_tube));
          endcapID.push_back(QString("muFast_MDT_Out_residual_endcap Tube No-" )+tube);
          endcapData.push_back(QString::number(TGCMidRhoChi2));
          
          /**if(off_match){
	          tube = (QString::number(i_tube));
            endcapID.push_back(QString("muFast_MDT_Out_residual_endcap_OffMatch Tube No-")+tube);
            endcapData.push_back(QString::number(TGCMidRhoChi2));
	        } **/
        }
      }
    }//END: Endcap
    
    //MDT flow
    bool isMDThitThereForTriggerPlane = false;
    bool isMDTFitOkForTriggerPlane    = false;
    bool isMDTFitOkFor2Plane          = false;
    
    int nMDT = (*itMfd)->mdt_onlineId().size();
    
    float MDTInnChi2 = (*itMfd)->mdt_Inner_fit_chi();
    float MDTMidChi2 = (*itMfd)->mdt_Middle_fit_chi();
    float MDTOutChi2 = (*itMfd)->mdt_Outer_fit_chi();
    
    const float MDT_CHI2_NO_VALUE = -99999;
    
    if(n_mdt_hits_middle != 0) isMDThitThereForTriggerPlane = true;
    if(fabs(MDTMidChi2-MDT_CHI2_NO_VALUE) > ZERO_LIMIT) isMDTFitOkForTriggerPlane = true;
    if(isMDTFitOkForTriggerPlane && (fabs(MDTInnChi2 - MDT_CHI2_NO_VALUE) > ZERO_LIMIT || fabs(MDTOutChi2 - MDT_CHI2_NO_VALUE) > ZERO_LIMIT) ) {
      isMDTFitOkFor2Plane = true;
    }
    
    //DEFs
    int isL1hitTheret, isL1emuOkForTriggerPlanet, isMDThitTheret, isMDTFitOkForTriggerPlanet, isMDTFitOkFor2Planet;
    
    if(isL1hitThere==true) isL1hitTheret=1;
    else isL1hitTheret=0;
    
    if(isL1emuOkForTriggerPlane==true) isL1emuOkForTriggerPlanet=1;
    else isL1emuOkForTriggerPlanet=0;
    
    if(isMDThitThereForTriggerPlane==true)  isMDThitTheret=1;
    else isMDThitTheret=0;
    
    if(isMDTFitOkForTriggerPlane==true)  isMDTFitOkForTriggerPlanet=1;
    else  isMDTFitOkForTriggerPlanet=0;
    
    if(isMDTFitOkFor2Plane==true)  isMDTFitOkFor2Planet=1;
    else  isMDTFitOkFor2Planet=0;
    
    //Dump to vectors
    generalID.push_back("isL1hitThere");
    generalData.push_back(QString::number(isL1hitTheret));
    
    generalID.push_back("L1emuOkForTriggerPlane");
    generalData.push_back(QString::number(isL1emuOkForTriggerPlanet));
    
    generalID.push_back("isMDThitThere");
    generalData.push_back(QString::number(isMDThitTheret));
    
    generalID.push_back("FitOkForTriggerPlane");
    generalData.push_back(QString::number(isMDTFitOkForTriggerPlanet));
    
    generalID.push_back("FitOkFor2Plane");
    generalData.push_back(QString::number(isMDTFitOkFor2Planet));
    
    //Dump to vectors
    if(systemID==0) { //Barrel
      barrelID.push_back("muFast_RPC_Pad_N" );
      barrelData.push_back(QString::number(nRPC));
            
      barrelID.push_back("muFast_MDT_Inn_fit_chi2_barrel");
      barrelData.push_back(QString::number(MDTInnChi2));
            
      barrelID.push_back("muFast_MDT_Mid_fit_chi2_barrel");
      barrelData.push_back(QString::number(MDTMidChi2));
            
      barrelID.push_back("muFast_MDT_Out_fit_chi2_barrel");
      barrelData.push_back(QString::number(MDTOutChi2));
      
    } else{ //Endcap
      endcapID.push_back("muFast_TGC_Mid_rho_chi2");
      endcapData.push_back(QString::number(TGCMidRhoChi2));
      
      endcapID.push_back("muFast_TGC_Mid_phi_chi2" );
      endcapData.push_back(QString::number(TGCMidPhiChi2));
      
      endcapID.push_back("muFast_MDT_Inn_fit_chi2_endcap");
      endcapData.push_back(QString::number(MDTInnChi2));
            
      endcapID.push_back("FitOkFor2Plane");
      endcapData.push_back(QString::number(MDTMidChi2));
            
      endcapID.push_back("muFast_MDT_Out_fit_chi2_endcap");
      endcapData.push_back(QString::number(MDTOutChi2));
      
      endcapID.push_back("muFast_MDT_N_endcap");
      endcapData.push_back(QString::number(nMDT));
    }
  }
  //END: Looping MuonFeatureDetails
  
  //Export Trigger Data according to pre-def. structure ------>
  if(int(headerID.size())==3) {
    if(int(barrelID.size())==int(barrelData.size()) && int(endcapID.size())==int(endcapData.size()) && int(generalID.size())==int(generalData.size())) {
      if(int(barrelData.size())!=0 || int(endcapData.size())!=0 || int(generalData.size())) {
	m_trigData.push_back(headerID);    //[0]
	
	m_trigData.push_back(barrelID);    //[1]
	m_trigData.push_back(endcapID);    //[2]
	m_trigData.push_back(generalID);   //[3]
	
	m_trigData.push_back(barrelData);  //[4]
	m_trigData.push_back(endcapData);  //[5]
	m_trigData.push_back(generalData); //[6]
	
	return true;
      }//one or more non-empty vectors
      else
	log_warning("processitem(): No Barrel, Endcap or General data found.");
    }//ID-Data size match
    else
      log_error("processitem(): ID-Data size matching failed!");
  }//header size
  else
    log_error("processitem(): incorrect headerID size!");
  return false;
  //----------------------------------------------------------<
}//END: processitem


//Processes L2 data item with storage in QTreeWidgetItem and returns processing success/failure
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerHandleL2::processQTrigItem()
{
  //Process Handle item
  bool status = processitem();
  
  if(status) {
    //VARs
    m_qtrigData = new QTreeWidgetItem(0);
    QTreeWidgetItem* dataL2item;
    int x_id=0, x_data=0, diff=0, avail=0;
    
    //DEFs
    QString sep = " = ";
    
    if(int(m_trigData.size())>1 && int(m_trigData[0].size())!=0 && int(m_trigData.size())==(int(m_trigData[0].size())*2)+1) {
      
      diff=int(m_trigData[0].size());
      for(int x=1; x<=int(m_trigData[0].size()); ++x) {
	x_id=x;
	x_data=x_id+diff;
	if((x_data<int(m_trigData.size())) && (int(m_trigData[x_id].size())==int(m_trigData[x_data].size())) && (int(m_trigData[x_data].size())>0)) {
	  //New Item
	  dataL2item = new QTreeWidgetItem(m_qtrigData);
	  
	  //Set ID
	  dataL2item->setText(0, m_trigData[0][x-1]);
	  
	  //Load Data
	  QList<QTreeWidgetItem *> dataList;
	  for(int i=0; i<int(m_trigData[x_id].size()); ++i)
	    dataList.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(m_trigData[x_id][i]+sep+m_trigData[x_data][i])));
	  dataL2item->insertChildren(0, dataList);
	  //y loop
	  ++avail;
	}//avail. range
      }//x loop
      
      //Data available
      if(avail>0) m_qtrigstatus=true;
      else m_qtrigstatus=false;
    }//vector check
    else {
      log_error("processQTrigItem(): L2 data not available!");
      m_qtrigstatus=false;
    }
  }
  else
    m_qtrigstatus=false;
  
  return m_qtrigstatus;
}//END: processQTrigItem


//Processes L2 data item with storage in Vector and returns processing success/failure
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerHandleL2::processVTrigItem()
{
  //Process Handle item
  bool status = processitem();

  if(status)
    m_vtrigstatus=true;
  else
    m_vtrigstatus=false;

  return m_vtrigstatus;
}//END: processVTrigItem

/*
std::vector<QString>::iterator x_trigData;
std::vector<std::vector<QString> >::iterator y_trigData;

for(y_trigData=m_trigData.begin(); y_trigData!=m_trigData.end(); ++y_trigData) {
  for(x_trigData=y_trigData->begin(); x_trigData!=y_trigData->end(); ++x_trigData) {
    std::cout << (*x_trigData).toStdString() << "  ";
  }
  std::cout << std::endl;
 }
*/
