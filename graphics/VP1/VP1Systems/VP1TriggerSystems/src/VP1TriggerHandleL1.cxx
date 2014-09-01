/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @Class   : VP1TriggerHandleL1
 *
 * @brief   : L1 Trigger Data Handle
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/VP1TriggerHandleL1.h"

//L1 includes
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigDecisionTool/ChainGroup.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/TDTUtilities.h"

//RoI Info: Names
QString roiInfoName[7] =
  {
    "z:",      //ID: 0
    "eta:",    //ID: 1
    "eta(z+)", //ID: 2
    "eta(z-)", //ID: 3
    "phi:",    //ID: 4
    "RoIid:",  //ID: 5
    "RoIword:" //ID: 6
  };

//RoI ID Vector
std::vector<QString> roi_id; //--- once complete -->|
               //m_trigData <----- added to end <---|

//Tokenize and Store Feature items
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerHandleL1::processFeature(int processId, QString roiId, QString feature)
{
  feature = feature.simplified();
  QStringList featureSeg = feature.split(QRegExp("\\s+"));
  
  if(int(featureSeg.size())==22) { //default: 22 [current TrigDecisionTool]
    std::vector<QString> fdata, fid;
    QStringList fdataitem;
    int idx=0, ri_idx=0;
    
    //**** COLLECT FEATURE ITEM DATA [3.DIM] ***********************************
    for(int fs=0; fs<int(featureSeg.size()); ++fs) {
      if(ri_idx<(int(sizeof roiInfoName/sizeof(QString))-1) && idx==1)
	++ri_idx;
      
      if(roiInfoName[ri_idx]==featureSeg[fs]) {
	if(idx>1 && int(fdataitem.size())>0) {
	  fdata.push_back(fdataitem.join(" "));
	  fdataitem.clear();
	  idx=0;
	}
	++idx;
	continue;
      }
      else {
	if(!featureSeg[fs].contains("[") && !featureSeg[fs].contains("]"))
	  fdataitem << featureSeg[fs];
	++idx;
	
	if(fs==(int(featureSeg.size())-1) && int(fdataitem.size())>0) {
	  fdata.push_back(fdataitem.join(" "));
	  fdataitem.clear();
	}
      }
    }
    
    //**** COLLECT FEATURE ITEM DATA [2.DIM] ***********************************
    //std::cout << "FULL: " << feature.toStdString() << std::endl;
    if(int(sizeof roiInfoName/sizeof(QString))==int(fdata.size())) {
      if(processId==0) {
	for(int v=0; v<int(fdata.size()); ++v) {
	  //std::cout << "SEG: " << roiInfoName[v].toStdString() << " " << fdata[v].toStdString() << std::endl;
	  fid.push_back(roiInfoName[v]);
	}
	m_trigData.push_back(fid);
      }
      m_trigData.push_back(fdata);
      roi_id.push_back(roiId);
      return true;
    }
    else {
      log_error("processFeature(): unexpected vector size: roiInfo["+VP1String::str(int(sizeof roiInfoName/sizeof(QString)))+"], fdata["+VP1String::str(int(featureSeg.size()))+"]!");
      return false;
    }
  }//END: def_22
  else {
    //--- Note: This system depends on the return of the ATLAS TrigDecisionTool. In case ---
    //---       this underlying package has been modified, there are various ways to     ---
    //---       deal with the return more dynamic or adaptive (TODO). In this version    ---
    //---       we however only throw an exception until stable adaptive methods tested. ---
    log_error("processFeature(): TrigDecisionTool returned "+VP1String::str(int(featureSeg.size()))+" feature segment items!");
    return false;
  }
}//END: processFeature


//Processing L1 data handle item
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerHandleL1::processitem()
{
  Trig::FeatureContainer::combination_const_iterator cIt; //Feature iterator
  int combId=0, processId=0;
  QString feature, roiId;
  
  //Clear ROI vectors
  m_trigData.clear();
  roi_id.clear();
  
  //--- Note: For single muon objects (use combinations only for 2 or more objects) ---
  //e.g.: std::vector< Feature<TrigRoiDescriptor> > initRois = m_containerL1->get<TrigRoiDescriptor>("initialRoI");
  
  using namespace Trig;
  for(cIt = m_containerL1.getCombinations().begin(); cIt != m_containerL1.getCombinations().end(); ++cIt) {
    
    //IMPORT: all possible features of TrigRoiDescriptor
    std::vector< Feature<TrigRoiDescriptor> > forID            = cIt->get<TrigRoiDescriptor>("forID");
    std::vector< Feature<TrigRoiDescriptor> > forMS            = cIt->get<TrigRoiDescriptor>("forMS");
    std::vector< Feature<TrigRoiDescriptor> > initialRoI       = cIt->get<TrigRoiDescriptor>("initialRoI");
    std::vector< Feature<TrigRoiDescriptor> > secondaryRoI_L2  = cIt->get<TrigRoiDescriptor>("secondaryRoI_L2");
    std::vector< Feature<TrigRoiDescriptor> > secondaryRoI_EF  = cIt->get<TrigRoiDescriptor>("secondaryRoI_EF");
    std::vector< Feature<TrigRoiDescriptor> > T2TauFinal       = cIt->get<TrigRoiDescriptor>("T2TauFinal");
    std::vector< Feature<TrigRoiDescriptor> > TrigT2CaloEgamma = cIt->get<TrigRoiDescriptor>("TrigT2CaloEgamma");
    std::vector< Feature<TrigRoiDescriptor> > TrigT2CaloJet    = cIt->get<TrigRoiDescriptor>("TrigT2CaloJet");
    std::vector< Feature<TrigRoiDescriptor> > TrigT2CosmicJet  = cIt->get<TrigRoiDescriptor>("TrigT2CosmicJet");
    std::vector< Feature<TrigRoiDescriptor> > TrigT2CaloTau    = cIt->get<TrigRoiDescriptor>("TrigT2CaloTau");
    
    //EXPORT: Feature vector contents >>
    Feature<TrigRoiDescriptor> d;
    //********************
    if( !forID.empty() ){
      for(int dItr=0; dItr<int(forID.size()); ++dItr) {
	d = forID[dItr];	
	feature = QString::fromStdString(::str(*d.cptr()));
	roiId = QString("ROI_ID_")+QString::number(combId)+QString(":")+QString::number(dItr);
	if(!feature.isEmpty()) {
	  bool fstatus = processFeature(processId, roiId, feature); ++processId;
	  if(!fstatus)
	    log_error("processitem(): feature processing failed for: "+roiId+" > "+qstr(::str(*d.cptr())));
	}
      }//loop
    }//END: forID
    
    //********************
    if( !forMS.empty() ){
      for(int dItr=0; dItr<int(forMS.size()); ++dItr) {
	d = forMS[dItr];
	feature = QString::fromStdString(::str(*d.cptr()));
        roiId = QString("ROI_MS_")+QString::number(combId)+QString(":")+QString::number(dItr);
        if(!feature.isEmpty()) {
          bool fstatus = processFeature(processId, roiId, feature); ++processId;
	  if(!fstatus)
            log_error("processitem(): feature processing failed for: "+roiId+" > "+qstr(::str(*d.cptr())));
        }
      }//loop
    }//END: forMS
    
    //********************
    if( !initialRoI.empty() ){
      for(int dItr=0; dItr<int(initialRoI.size()); ++dItr) {
	d = initialRoI[dItr];
	feature = QString::fromStdString(::str(*d.cptr()));
        roiId = QString("ROI_Initial_")+QString::number(combId)+QString(":")+QString::number(dItr);
        if(!feature.isEmpty()) {
          bool fstatus = processFeature(processId, roiId, feature); ++processId;
	  if(!fstatus)
            log_error("processitem(): feature processing failed for: "+roiId+" > "+qstr(::str(*d.cptr())));
        }
      }//loop
    }//END: initialRoI
    
    //********************
    if( !secondaryRoI_L2.empty() ){
      for(int dItr=0; dItr<int(secondaryRoI_L2.size()); ++dItr) {
	d = secondaryRoI_L2[dItr];
	feature = QString::fromStdString(::str(*d.cptr()));
        roiId = QString("ROI_SecondaryL2_")+QString::number(combId)+QString(":")+QString::number(dItr);
        if(!feature.isEmpty()) {
          bool fstatus = processFeature(processId, roiId, feature); ++processId;
	  if(!fstatus)
            log_error("processitem(): feature processing failed for: "+roiId+" > "+qstr(::str(*d.cptr())));
        }
      }//loop
    }//END: secondaryRoI_L2
    
    //********************
    if( !secondaryRoI_EF.empty() ){
      for(int dItr=0; dItr<int(secondaryRoI_EF.size()); ++dItr) {
	d = secondaryRoI_EF[dItr];
	feature = QString::fromStdString(::str(*d.cptr()));
        roiId = QString("ROI_SecondaryEF_")+QString::number(combId)+QString(":")+QString::number(dItr);
        if(!feature.isEmpty()) {
          bool fstatus = processFeature(processId, roiId, feature); ++processId;
	  if(!fstatus)
            log_error("processitem(): feature processing failed for: "+roiId+" > "+qstr(::str(*d.cptr())));
        }
      }//loop
    }//END: secondaryRoI_EF
    
    //********************
    if( !T2TauFinal.empty() ){
      for(int dItr=0; dItr<int(T2TauFinal.size()); ++dItr) {
	d = T2TauFinal[dItr];
	feature = QString::fromStdString(::str(*d.cptr()));
        roiId = QString("ROI_T2TauFinal_")+QString::number(combId)+QString(":")+QString::number(dItr);
        if(!feature.isEmpty()) {
          bool fstatus = processFeature(processId, roiId, feature); ++processId;
	  if(!fstatus)
            log_error("processitem(): feature processing failed for: "+roiId+" > "+qstr(::str(*d.cptr())));
        }
      }//loop
    }//END: T2TauFinal
    
    //********************
    if( !TrigT2CaloEgamma.empty() ){
      for(int dItr=0; dItr<int(TrigT2CaloEgamma.size()); ++dItr) {
	d = TrigT2CaloEgamma[dItr];
	feature = QString::fromStdString(::str(*d.cptr()));
        roiId = QString("ROI_TrigT2CaloEgamma_")+QString::number(combId)+QString(":")+QString::number(dItr);
        if(!feature.isEmpty()) {
          bool fstatus = processFeature(processId, roiId, feature); ++processId;
	  if(!fstatus)
            log_error("processitem(): feature processing failed for: "+roiId+" > "+qstr(::str(*d.cptr())));
        }
      }//loop
    }//END: TrigT2CaloEgamma
    
    //********************
    if( !TrigT2CaloJet.empty() ){
      for(int dItr=0; dItr<int(TrigT2CaloJet.size()); ++dItr) {
	d = TrigT2CaloJet[dItr];
	feature = QString::fromStdString(::str(*d.cptr()));
        roiId = QString("ROI_TrigT2CaloJet_")+QString::number(combId)+QString(":")+QString::number(dItr);
        if(!feature.isEmpty()) {
          bool fstatus = processFeature(processId, roiId, feature); ++processId;
	  if(!fstatus)
            log_error("processitem(): feature processing failed for: "+roiId+" > "+qstr(::str(*d.cptr())));
        }
      }//loop
    }//END: TrigT2CaloJet
    
    //********************
    if( !TrigT2CosmicJet.empty() ){
      for(int dItr=0; dItr<int(TrigT2CosmicJet.size()); ++dItr) {
	d = TrigT2CosmicJet[dItr];
	feature = QString::fromStdString(::str(*d.cptr()));
        roiId = QString("ROI_TrigT2CosmicJet_")+QString::number(combId)+QString(":")+QString::number(dItr);
        if(!feature.isEmpty()) {
          bool fstatus = processFeature(processId, roiId, feature); ++processId;
	  if(!fstatus)
            log_error("processitem(): feature processing failed for: "+roiId+" > "+qstr(::str(*d.cptr())));
        }
      }//loop
    }//END: TrigT2CosmicJet
    
    //********************
    if( !TrigT2CaloTau.empty() ){
      for(int dItr=0; dItr<int(TrigT2CaloTau.size()); ++dItr) {
	d = TrigT2CaloTau[dItr];
	feature = QString::fromStdString(::str(*d.cptr()));
        roiId = QString("ROI_TrigT2CaloTau_")+QString::number(combId)+QString(":")+QString::number(dItr);
        if(!feature.isEmpty()) {
          bool fstatus = processFeature(processId, roiId, feature); ++processId;
	  if(!fstatus)
            log_error("processitem(): feature processing failed for: "+roiId+" > "+qstr(::str(*d.cptr())));
        }
      }//loop
    }//END: TrigT2CaloTau
    
    ++combId;
  }//END: loop over chain group items
  
  //EXPORT TRIGGER DATA -------------------------------------------------------------------------------->
  if(m_trigData.size()!=0 && m_trigData[0].size()!=0 && roi_id.size()!=0) {
    //std::cout << "dataSize>" << int(m_trigData.size()) << " idSize>" << int(roi_id.size()) << std::endl;
    if((int(m_trigData.size())-1)==int(roi_id.size())) {
      m_trigData.push_back(roi_id);                       //Append ROI IDs to ROI data (incl. fid, fdata)
      return true;
    }
    else {
      log_error("processitem(): ROI data and ID entries do not match!");
      return false;
    }
  }
  else
    log_error("processitem(): ROI vectors empty: cannot append IDs!");
  return false;
  //----------------------------------------------------------------------------------------------------<
}//END: processitem


//Processes L1 data item with storage in QTreeWidgetItem and returns processing success/failure
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerHandleL1::processQTrigItem()
{  
  //Process Handle item
  bool status = processitem();
  m_qtrigitem = new QTreeWidgetItem(0);
  
  if(status) {
    //Safety Check: TrigID / content count match
    if(!(int(m_trigData[int(m_trigData.size())-1].size()) == int(m_trigData.size())-2)) {
      log_error("processQTrigItem(): mismatch in TrigID / data item count: TrigID size: "+VP1String::str(int(m_trigData[int(m_trigData.size())-1].size()))+" and TrigID data cout: "+VP1String::str(int(m_trigData.size())-2));
      m_qtrigstatus=false;
    }
    else {
      QTreeWidgetItem* trigId;
      QString dataItem;
      
      m_qtrigitem->setText(0, QString("TE: ")+m_trigID);
      
      for(int x=1; x<int(m_trigData.size()-1); ++x) {
	trigId = new QTreeWidgetItem(m_qtrigitem);
	trigId->setText(0, m_trigData[int(m_trigData.size())-1][x-1]);
	
	QList<QTreeWidgetItem *> data;
	for(int y=0; y<int(m_trigData[x].size()); ++y) {
	  if(int(m_trigData[0].size())==int(m_trigData[x].size())) {
	    dataItem = m_trigData[0][y]+QString(" ")+m_trigData[x][y];
	    data.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(dataItem)));
	  }
	  else
	    log_error("processQTrigItem(): mismatch in feature ID to data item count!");
	}//y loop
	trigId->insertChildren(0, data);
      }//x loop
      m_qtrigstatus=true;
    }//count ok
  }//status ok
  else
    m_qtrigstatus=false;
  
  return m_qtrigstatus;
}//END: processQTrigItem


//Processes L1 data item with storage in Vectors and returns processing success/failure
//_____________________________________________________________________________________________
bool VP1Trig::VP1TriggerHandleL1::processVTrigItem()
{
  //Process Handle item
  bool status = processitem();
  
  if(status)
    m_vtrigstatus=true;
  else
    m_vtrigstatus=false;
  
  return m_vtrigstatus;
}//END: processVTrigItem
