/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @Class   : VP1TriggerHandleEF
 *
 * @brief   : EF Trigger Data Handle
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/VP1TriggerHandleEF.h"

//EF includes
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

//Other includes
#include <math.h>

//Trigger data vectors
std::vector<QString> h_type, h_source, h_id; //header data
std::vector<std::vector<QString> > t_data;   //track data

//Data structure settings
//--- Note: When making changes, do adapt vector  ---
//---       loading order in loadTrackContainer() ---
const int HWIDTH = 3; //header width
const int TYPIDX = 0; //h_type index
const int SRCIDX = 1; //h_source index
const int TIDIDX = 2; //h_id index

//MuonEF and MuGirl count
int count_mef=0, count_mg=0;

//Muon Track Info: Instant holders
TrigMuonEFTrack* muonEFTrack;
TrigMuonEFCbTrack* muonEFCbTrack;

//Muon Track Info: Names
std::string muonTrackName[18] =
  {
    "Charge",         //ID: 0
    "iPt",            //ID: 1
    "Theta",          //ID: 2
    "Phi",            //ID: 3
    "m",              //ID: 4
    "chi2",           //ID: 5
    "chi2prob",       //ID: 6
    "posX",           //ID: 7
    "posY",           //ID: 8
    "posZ",           //ID: 9
    "NCscHitsEta",    //ID: 10
    "NCscHitsPhi",    //ID: 11
    "NTgcHitsEta",    //ID: 12
    "NTgcHitsPhi",    //ID: 13
    "NRpcHitsEta",    //ID: 14
    "NRpcHitsPhi",    //ID: 15
    "d0",             //ID: 16
    "z0"              //ID: 17
  };


//Muon Track Info: Data
//_____________________________________________________________________________________________
double VP1Trig::VP1TriggerHandleEF::muonTrackData(QString type, int id)
{
  double data=-101;
  
  if(type=="s_mef" || type=="s_mg" || type=="e_mef" || type=="e_mg") {
    switch(id)
      {
      case 0: data = muonEFTrack->Charge();       break;
      case 1: data = muonEFTrack->iPt();          break;
      case 2: data = muonEFTrack->cotTh();        break;
      case 3: data = muonEFTrack->phi();          break;
      case 4: data = muonEFTrack->m();            break;
      case 5: data = muonEFTrack->chi2();         break;
      case 6: data = muonEFTrack->chi2prob();     break;
      case 7: data = muonEFTrack->posX();         break;
      case 8: data = muonEFTrack->posY();         break;
      case 9: data = muonEFTrack->posZ();         break;
      case 10: data = muonEFTrack->NCscHitsEta(); break;
      case 11: data = muonEFTrack->NCscHitsPhi(); break;
      case 12: data = muonEFTrack->NTgcHitsEta(); break;
      case 13: data = muonEFTrack->NTgcHitsPhi(); break;
      case 14: data = muonEFTrack->NRpcHitsEta(); break;
      case 15: data = muonEFTrack->NRpcHitsPhi(); break;
      case 16: data = muonEFTrack->d0();          break;
      case 17: data = muonEFTrack->z0();          break;
      default: data = -102;
      }
  } else if(type=="c_mef" || type=="c_mg") {
    switch(id)
      {
      case 0: data = muonEFCbTrack->Charge();       break;
      case 1: data = muonEFCbTrack->iPt();          break;
      case 2: data = muonEFCbTrack->cotTh();        break;
      case 3: data = muonEFCbTrack->phi();          break;
      case 4: data = muonEFCbTrack->m();            break;
      case 5: data = muonEFCbTrack->chi2();         break;
      case 6: data = muonEFCbTrack->chi2prob();     break;
      case 7: data = muonEFCbTrack->posX();         break;
      case 8: data = muonEFCbTrack->posY();         break;
      case 9: data = muonEFCbTrack->posZ();         break;
      case 10: data = muonEFCbTrack->NCscHitsEta(); break;
      case 11: data = muonEFCbTrack->NCscHitsPhi(); break;
      case 12: data = muonEFCbTrack->NTgcHitsEta(); break;
      case 13: data = muonEFCbTrack->NTgcHitsPhi(); break;
      case 14: data = muonEFCbTrack->NRpcHitsEta(); break;
      case 15: data = muonEFCbTrack->NRpcHitsPhi(); break;
      case 16: data = muonEFCbTrack->d0();          break;
      case 17: data = muonEFCbTrack->z0();          break;
      default: data = -103;
      }
  }
  else
    log_error("muonTrackData(): invalid type!");
  
  return data;
}//END: muonTrackData


//Query TrackContainer and via loadMuonTrack() store track info to corresponding vectors
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerHandleEF::loadTrackContainer()
{
  //VARs
  int mt=0;          //muon flag: type id
  QString type="_";  //muon flag: type name
  int trackNo=0;
  
  //Resets
  h_type.clear();
  h_source.clear();
  h_id.clear();
  t_data.clear();
  m_trigData.clear();
  count_mef=0;
  count_mg=0;
  
  if(m_containerEF->hasTrack()) { //Muon in RoI
    const TrigMuonEFInfoTrackContainer *tc = m_containerEF->TrackContainer(); //TrackContainer: more than one muon in RoI
    TrigMuonEFInfoTrackContainer::const_iterator TrackItr;
    
    for(TrackItr = tc->begin(); TrackItr!=tc->end(); TrackItr++) {
      TrigMuonEFInfoTrack* muonInfo = (*TrackItr);
      mt = int(muonInfo->MuonType()); //<1> MuonEF, <2> MuGirl
      
      //Safety check
      if(mt!=1 && mt!=2) {
	log_error("loadTrackContainer(): invalid muon type flag found: "+VP1String::str(mt));
	return false;
      }
      
      //-----------------------------------------------------------------------
      muonEFTrack = muonInfo->SpectrometerTrack();
      if(mt==1)      type="s_mef"; //-> (S)pec. (M)uon(EF)
      else if(mt==2) type="s_mg";  //-> (S)pec. (M)u(G)irl
      
      if(muonEFTrack)
	loadMuonTrack(trackNo++,type);
      else
	log_info("loadTrackContainer(): no SpectrometerTrack found");
      
      //-----------------------------------------------------------------------
      muonEFTrack = muonInfo->ExtrapolatedTrack();
      if(mt==1)      type="e_mef"; //-> (E)xtra. (M)uon(EF)
      else if(mt==2) type="e_mg";  //-> (E)xtra. (M)u(G)irl
      
      if(muonEFTrack)
	loadMuonTrack(trackNo++,type);
      else
	log_info("loadTrackContainer(): no ExtrapolatedTrack found");
      
      //-----------------------------------------------------------------------
      muonEFCbTrack = muonInfo->CombinedTrack();
      if(mt==1)      type="c_mef"; //-> (C)omb. (M)uon(EF)
      else if(mt==2) type="c_mg";  //-> (C)omb. (M)u(G)irl
      
      if(muonEFCbTrack)
	loadMuonTrack(trackNo++,type);
      else
	log_info("loadTrackContainer(): no CombinedTrack found");
    }//loop
    
    //ASSIGN DATA STRUCTURE AND EXPORT TO EF TRIGGER HANDLE DATA VECTOR ------->
    /*************************************************************
    <   HEADER   >   < TRACK DATA   ... >
     |     |    |      |      |
    TYPE SOURCE ID    DATA - DATA - ...

    TYPE:   MuonEF, MuGirl
            [y-y correspondence to SOURCE]
    SOURCE: SpectrometerTrack, ExtrapolatedTrack, CombinedTrack
            [y-x correspondence to DATA]
    ID:     Charge, iPT, etc.
            [y-y correspondence to DATA]
    DATA:   values corresponding to ID
    *************************************************************/    
    if(int(h_type.size())!=0 && int(h_source.size())!=0 && int(h_id.size())!=0 && int(t_data.size())!=0 && int(m_trigData.size())==0) {
      if(int(h_type.size())==int(h_source.size()) && int(h_source.size())==int(t_data.size())) {
	m_trigData.push_back(h_type);
	m_trigData.push_back(h_source);
	m_trigData.push_back(h_id);
	for(int q=0; q<int(t_data.size()); ++q)
	  m_trigData.push_back(t_data[q]);
	return true;
      }//size match
    }//data available
    //-------------------------------------------------------------------------<
    return false;
  }//hasTrack
  else
    log_warning("loadTrackContainer(): no tracks found in EF container.");
  return false;
}//END: loadTrackContainer


//Load Muon Track data to corresponding vectors
//_____________________________________________________________________________________________
void VP1Trig::VP1TriggerHandleEF::loadMuonTrack(int trackNo, QString type)
{
  //PROCESS TRACK DATA AND STORE IN TEMP. VECTOR ---------------------------->
  std::vector<QString> data;
  
  for(int i=0; i < int(sizeof muonTrackName/sizeof(std::string)); ++i) {
    if(trackNo==0) h_id.push_back(QString::fromStdString(muonTrackName[i]));
    if(muonTrackName[i]=="Theta") {
      double cotTh = muonTrackData(type,i);
      if(cotTh!=0) data.push_back(QString::number((M_PI/2)-atan(1/cotTh)));
      else data.push_back(QString::number((M_PI/2)-atan(10^10)));
    }
    else {
      data.push_back(QString::number(muonTrackData(type,i)));
    }
  }
  //-------------------------------------------------------------------------<
  
  //LOAD HEADER TYPE, SOURCE AND DATA TO TEMP. VECTORS ---------------------->
  if(int(data.size())!=0) {
    if(type=="s_mef") {      //SpectrometerTrack - MuonEF
      //Header Update
      h_type.push_back("MuonEF");
      h_source.push_back("SpectrometerTrack");
      //Track Update
      t_data.push_back(data);
      ++count_mef;
    }
    else if(type=="s_mg") {  //SpectrometerTrack - MuGirl
      //Header Update
      h_type.push_back("MuGirl");
      h_source.push_back("SpectrometerTrack");
      //Track Update
      t_data.push_back(data);
      ++count_mg;
    }
    else if(type=="e_mef") { //ExtrapolatedTrack - MuonEF
      //Header Update
      h_type.push_back("MuonEF");
      h_source.push_back("ExtrapolatedTrack");
      //Track Update
      t_data.push_back(data);
      ++count_mef;
    }
    else if(type=="e_mg") {  //ExtrapolatedTrack - MuGirl
      //Header Update
      h_type.push_back("MuGirl");
      h_source.push_back("ExtrapolatedTrack");
      //Track Update
      t_data.push_back(data);
      ++count_mg;
    }
    else if(type=="c_mef") { //CombinedTrack - MuonEF
      //Header Update
      h_type.push_back("MuonEF");
      h_source.push_back("CombinedTrack");
      //Track Update
      t_data.push_back(data);
      ++count_mef;
    }
    else if(type=="c_mg") {  //CombinedTrack - MuGirl
      //Header Update
      h_type.push_back("MuGirl");
      h_source.push_back("CombinedTrack");
      //Track Update
      t_data.push_back(data);
      ++count_mg;
    }
    else
      log_error("loadMuonTrack(): invalid type!");
  }//data available
  else
    log_error("loadMuonTrack(): no data available!");
  //-------------------------------------------------------------------------<
}//END: loadMuonTrack


//Processes EF data item with storage in QTreeWidgetItem and returns processing success/failure
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerHandleEF::processQTrigItem()
{
  //Query TrackContainer and store 
  //data to corresponding vectors
  bool status = loadTrackContainer();
  
  if(status) {
    //VARs
    m_qtrigData = new QTreeWidgetItem(0);
    m_qtrigData->setText(0, QString(m_trigID));
    int avail=0;
    
    //DEFs
    QString sep = " = ";
    int TWIDTH = int(m_trigData.size())-HWIDTH;
    
    if(int(m_trigData.size())>HWIDTH) {
      if(int(m_trigData[TYPIDX].size())==int(m_trigData[SRCIDX].size()) && int(m_trigData[SRCIDX].size())==TWIDTH) {
	
	//### chainIDs ### >>>
	if(!(m_chainIDs.isEmpty())) {
	  QTreeWidgetItem* chain = new QTreeWidgetItem(m_qtrigData);
          chain->setText(0, QString("Passed Triggers"));
	  QList<QTreeWidgetItem *> chainItems;
	  for(int i=0; i < int(m_chainIDs.size()); ++i)
	    chainItems.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(m_chainIDs[i])));
	  chain->insertChildren(0, chainItems);
	}//chain data available
	
	//### EF ### >>>>
	if(count_mef!=0) {
	  QTreeWidgetItem* eftree = new QTreeWidgetItem(m_qtrigData);
	  eftree->setText(0, QString("Muon Type 1 -> EF"));
	  
	  for(int i=0; i<TWIDTH; ++i) {
	    if(m_trigData[TYPIDX][i].contains("MuonEF") && m_trigData[SRCIDX][i].contains("Spectrometer")) {
	      QTreeWidgetItem* eftree_s = new QTreeWidgetItem(eftree);
	      eftree_s->setText(0, QString("Spectrometer Track-")+QString::number(i+1));
	      QList<QTreeWidgetItem *> efitem_s;
	      for(int j=0; j < int(m_trigData[i+HWIDTH].size()); ++j) {
		efitem_s.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(m_trigData[TIDIDX][j]+sep+m_trigData[i+HWIDTH][j])));
	      }
	      eftree_s->insertChildren(0, efitem_s);
	    }
	  }//loop
	  
	  for(int i=0; i<TWIDTH; ++i) {
	    if(m_trigData[TYPIDX][i].contains("MuonEF") && m_trigData[SRCIDX][i].contains("Extrapolated")) {
	      QTreeWidgetItem* eftree_e = new QTreeWidgetItem(eftree);
	      eftree_e->setText(0, QString("Extrapolated Track-")+QString::number(i+1));
	      QList<QTreeWidgetItem *> efitem_e;
	      for(int j=0; j < int(m_trigData[i+HWIDTH].size()); ++j) {
		efitem_e.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(m_trigData[TIDIDX][j]+sep+m_trigData[i+HWIDTH][j])));
	      }
	      eftree_e->insertChildren(0, efitem_e);
	    }
	  }//loop
	  
	  for(int i=0; i<TWIDTH; ++i) {
	    if(m_trigData[TYPIDX][i].contains("MuonEF") && m_trigData[SRCIDX][i].contains("Combined")) {
	      QTreeWidgetItem* eftree_c = new QTreeWidgetItem(eftree);
	      eftree_c->setText(0, QString("Combined Track-")+QString::number(i+1));
	      QList<QTreeWidgetItem *> efitem_c;
	      for(int j=0; j < int(m_trigData[i+HWIDTH].size()); ++j) {
		efitem_c.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(m_trigData[TIDIDX][j]+sep+m_trigData[i+HWIDTH][j])));
	      }
	      eftree_c->insertChildren(0, efitem_c);
	    }
	  }//loop
	  ++avail;
	}//mef data available
	
	//### MuonGirl ### >>>
	if(count_mg!=0) {
	  QTreeWidgetItem* mgtree = new QTreeWidgetItem(m_qtrigData);
	  mgtree->setText(0, QString("Muon Type 2 -> MuGirl"));
	  
	  for(int i=0; i<TWIDTH; ++i) {
	    if(m_trigData[TYPIDX][i].contains("MuGirl") && m_trigData[SRCIDX][i].contains("Spectrometer")) {
	      QTreeWidgetItem* mgtree_s = new QTreeWidgetItem(mgtree);
	      mgtree_s->setText(0, QString("Spectrometer Track-")+QString::number(i+1));
	      QList<QTreeWidgetItem *> mgitem_s;
	      for(int j=0; j < int(m_trigData[i+HWIDTH].size()); ++j) {
		mgitem_s.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(m_trigData[TIDIDX][j]+sep+m_trigData[i+HWIDTH][j])));
	      }
	      mgtree_s->insertChildren(0, mgitem_s);
	    }
	  }//loop
	  
	  for(int i=0; i<TWIDTH; ++i) {
	    if(m_trigData[TYPIDX][i].contains("MuGirl") && m_trigData[SRCIDX][i].contains("Extrapolated")) {
	      QTreeWidgetItem* mgtree_e = new QTreeWidgetItem(mgtree);
	      mgtree_e->setText(0, QString("Extrapolated Track-")+QString::number(i+1));
	      QList<QTreeWidgetItem *> mgitem_e;
	      for(int j=0; j < int(m_trigData[i+HWIDTH].size()); ++j) {
		mgitem_e.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(m_trigData[TIDIDX][j]+sep+m_trigData[i+HWIDTH][j])));
	      }
	      mgtree_e->insertChildren(0, mgitem_e);
	    }
	  }//loop
	  
	  for(int i=0; i<TWIDTH; ++i) {
	    if(m_trigData[TYPIDX][i].contains("MuGirl") && m_trigData[SRCIDX][i].contains("Combined")) {
	      QTreeWidgetItem* mgtree_c = new QTreeWidgetItem(mgtree);
	      mgtree_c->setText(0, QString("Combined Track-")+QString::number(i+1));
	      QList<QTreeWidgetItem *> mgitem_c;
	      for(int j=0; j < int(m_trigData[i+HWIDTH].size()); ++j) {
		mgitem_c.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(m_trigData[TIDIDX][j]+sep+m_trigData[i+HWIDTH][j])));
	      }
	      mgtree_c->insertChildren(0, mgitem_c);
	    }
	  }//loop
	  ++avail;
	}// mg data available
	
      }//size match check
    }//simple size check
    
    //Data available
    if(avail>0) m_qtrigstatus=true;
    else {
      log_error("processQTrigItem(): EF data not available!");
      m_qtrigstatus=false;
    }
    
  }//status
  else
    m_qtrigstatus=false;
  
  return m_qtrigstatus;
}//END: processQTrigItem


//Processes EF data item with storage in Vector and returns processing success/failure
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerHandleEF::processVTrigItem()
{
  //Query TrackContainer and store
  //data to corresponding vectors
  bool status = loadTrackContainer();
  
  if(status) {
    if(int(m_trigData.size())>3 && int(m_trigData[0].size())==int(m_trigData[1].size()) && int(m_trigData[1].size())==(int(m_trigData.size())-3))
      m_vtrigstatus=true;
  }
  else
    m_vtrigstatus=false;
  
  return m_vtrigstatus;
}//END: processVTrigItem
