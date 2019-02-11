/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     CaloClusterTimeTool.cxx
PACKAGE:  offline/Calorimeter/CaloClusterCorrection

AUTHORS: D. Hu 
CREATED: Jan 2012 

PURPOSE:  correct time for EM clusters

********************************************************************/

#include "CaloClusterTimeTool.h"
#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"

#include "CaloIdentifier/CaloGain.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
//#include "CaloEvent/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloCondBlobObjs/CaloCondUtils.h"

#include "AthenaKernel/IOVSvcDefs.h"
#include "AsgTools/MsgStream.h"

using xAOD::CaloCluster;


CaloClusterTimeTool::CaloClusterTimeTool (const std::string& type,const std::string& name,const IInterface* parent)
   :  AthAlgTool( type, name, parent ),
      m_LArOnlineIDHelper(nullptr),
      m_calo_id(nullptr)
{
  declareInterface<ICaloClusterTimeTool>(this);
  
  declareProperty("keyFebOffset", m_keyFebOffset); 
  declareProperty("keyRunOffset", m_keyRunOffset); 
  m_runOffsetBlob=0;
  m_lastItFeb=m_febOffsetBlobMap.begin();
}

CaloClusterTimeTool::~CaloClusterTimeTool() {}

StatusCode CaloClusterTimeTool::initialize(){

  ATH_MSG_INFO( "initialize CaloClusterTimeTool " );

  StatusCode sc = StatusCode::SUCCESS;

  sc = AthAlgTool::initialize();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not initialize AthAlgTool for CaloClusterTimeTool " );
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_INFO( "initialize AthAlgTool for CaloClusterTimeTool " );
  }


  // Retrieve online ID helper  
  sc = detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArOnlineIDHelper" );
    return StatusCode::SUCCESS;
  }
  else{
   ATH_MSG_INFO( "LArOnlineIDHelper is retrieved" );
  }

  // Retrieve CaloCell_ID
  sc = detStore()->retrieve(m_calo_id);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve CALO ID");
    return sc;
  }
  else{
   ATH_MSG_INFO("pointer to Calo ID is retrieved");
  }

  ATH_CHECK( m_cablingKey.initialize() );

  // register callback
  if(m_keyFebOffset!=""){
    sc = detStore()->regFcn(&CaloClusterTimeTool::updateFebOffsetMap, this, m_febOffsetColl, m_keyFebOffset);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not register callback" );
      return StatusCode::SUCCESS;
    }
    else{
      ATH_MSG_INFO( "callback is registered" );
    }
  }
  
  if(m_keyRunOffset!=""){
    sc = detStore()->regFcn(&CaloClusterTimeTool::updateRunOffsetMap, this, m_runOffsetColl, m_keyRunOffset);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not register callback" );
      return StatusCode::SUCCESS;
    }
    else{
     ATH_MSG_INFO( "callback is registered" );
    }
  }

  ATH_MSG_INFO( "CaloClusterTimeTool is initialized " );	

  return sc;
}

StatusCode CaloClusterTimeTool::updateFebOffsetMap(IOVSVC_CALLBACK_ARGS_K(/*keys*/)){
  ATH_MSG_DEBUG( "CaloClusterTimeTool updateFebOffsetMap is called" ); 
  std::list<std::string>::const_iterator itr;
  //for(itr=keys.begin(); itr!=keys.end(); ++itr){
  //  msg() << *itr <<"";
  //}
  //msg() << endreq; 

  CondAttrListCollection::const_iterator iColl = m_febOffsetColl->begin(); 
  CondAttrListCollection::const_iterator last = m_febOffsetColl->end(); 
  for(;iColl != last; ++iColl) {
    unsigned int sysId = static_cast<unsigned int>(iColl->first); 

    std::map<unsigned int, const CaloCondBlobFlt*>::iterator iOld = m_febOffsetBlobMap.find(sysId);
    if(iOld != m_febOffsetBlobMap.end()){
      delete iOld->second;
    }

    const coral::Blob& blob = iColl->second["CaloCondBlob16M"].data<coral::Blob>();
    const CaloCondBlobFlt* flt = CaloCondBlobFlt::getInstance(blob);

    m_febOffsetBlobMap[sysId] = flt;
  }   
  
  m_lastItFeb=m_febOffsetBlobMap.begin();
  return StatusCode::SUCCESS; 
}

StatusCode CaloClusterTimeTool::updateRunOffsetMap(IOVSVC_CALLBACK_ARGS_K(/*keys*/)){
  ATH_MSG_DEBUG( "CaloClusterTimeTool updateRunOffsetMap is called" );
  std::list<std::string>::const_iterator itr;
  //for(itr=keys.begin(); itr!=keys.end(); ++itr){
  //  msg() << *itr <<"";
  //}
  //msg() << endreq;

  CondAttrListCollection::const_iterator iColl = m_runOffsetColl->begin();
  CondAttrListCollection::const_iterator last = m_runOffsetColl->end();
  for(;iColl != last; ++iColl) {
    unsigned int sysId = static_cast<unsigned int>(iColl->first);

    if(sysId==1){
      if(m_runOffsetBlob){
        delete m_runOffsetBlob;
        m_runOffsetBlob=0; 
      }
      const coral::Blob& blob = iColl->second["CaloCondBlob16M"].data<coral::Blob>();
      const CaloCondBlobFlt* flt = CaloCondBlobFlt::getInstance(blob);
      m_runOffsetBlob = flt;
    }

  }

  return StatusCode::SUCCESS;
}


void CaloClusterTimeTool::makeClusterTimeCorrection(float pv_z, const CaloCluster* cluster,  float& time, float& error, unsigned int& flag)
{

  ATH_MSG_DEBUG( " during CaloClusterTimeTool::makeClusterTimeCorrection  " );
  error=0.;
  flag=0x0;

  if(cluster){

   float emax=-9999.;
   const CaloCell* cell_maxE=0;

   const CaloClusterCellLink* cellLink=cluster->getCellLinks();
   if (cellLink) {
     CaloClusterCellLink::const_iterator cellIter    = cluster->cell_begin();
     CaloClusterCellLink::const_iterator cellIterEnd = cluster->cell_end();
     for (;cellIter!=cellIterEnd;cellIter++) {
      const CaloCell* cell = (*cellIter);
      int sampling = cell->caloDDE()->getSampling();
      if (sampling== CaloCell_ID::EMB2 || sampling== CaloCell_ID::EME2) {
        if ( (cell->provenance() & 0x2000) ) {
          if (cell->e() > emax) {
              emax=cell->e();
              if(emax>5000.) cell_maxE=cell;
          }
        }
      }
     }
   }
   else{
     flag += ( 0x1 << CELL_INFO_MISSING);
     ATH_MSG_WARNING( "cell link is invalid "  );
   }

   if(cell_maxE){
     makeCellTimeCorrection(pv_z, cell_maxE, time, error, flag); 
   }
   else{
     flag += ( 0x1 << CLUS_GOODMIDCELL_MISSING);
     ATH_MSG_WARNING( "no cell found in middle layer with max energy > 5 GeV and good quality "  );
   }
  }
  else{
    flag += ( 0x1 << CLUS_INFO_MISSING);
    ATH_MSG_WARNING( "cluster is empty "  );
  }
}

void CaloClusterTimeTool::makeCellTimeCorrection(float pv_z,const CaloCell* cell, float & time, float & error, unsigned int& flag){

    ATH_MSG_DEBUG( " during CaloClusterTimeTool::makeCellTimeCorrection  " );

    time = cell->time();
    error=0.;
    float e_GEV = cell->e()*(1./1000.) ;
    CaloGain::CaloGain  gain= cell->gain();
    unsigned int dbGain = CaloCondUtils::getDbCaloGain(gain);
   
    if(dbGain==0) flag += ( 0x1 << HIGH_GAIN_SET); 
    if(dbGain==1) flag += ( 0x1 << MEDIUM_GAIN_SET); 
    if(dbGain==2) flag += ( 0x1 << LOW_GAIN_SET); 

    Identifier cell_ID  = cell->ID(); 
    const unsigned int cell_ID_value = cell_ID.get_compact();
    IdentifierHash cellHash = m_calo_id->calo_cell_hash(cell_ID);
    int sub_Calo(0);
    unsigned int sub_Hash = m_calo_id->subcalo_cell_hash(cellHash, sub_Calo);
    unsigned int subHash_calo=0;
    unsigned int sysId = 0;

    //Calo       sysId      Offset     
    //EMEC NEG     0          0              
    //EMB NEG      1         31872
    //EMB POS      2         31872 + 54784 = 86656
    //EMEC POS     3         31872 + 54784 + 54784 =141440
    //HEC          16         0
    //FCAL         32         0
    //TILE         48         0 
    if (sub_Calo==CaloCell_ID::LAREM) {
       if (sub_Hash >= 141440) {
         subHash_calo=sub_Hash-141440;
         sysId=3;
       }
       else if (sub_Hash>=86656) {
         subHash_calo=sub_Hash-86656;
         sysId=2;
       }
       else if (sub_Hash>=31872) {
         subHash_calo=sub_Hash-31872;
         sysId=1;
       }
       else{
         subHash_calo=sub_Hash;
         sysId=0;
       }
    }// end if LAr EM
    else{
       ATH_MSG_WARNING( "channel is not in EMEC or EMB no correction" );
       flag += ( 0x1 << CELL_NO_EM); 
       return;
    }

    if(m_lastItFeb!=m_febOffsetBlobMap.end()){
     if(m_lastItFeb->first!=sysId){
       m_lastItFeb=m_febOffsetBlobMap.find(sysId);
     }
    }
    else m_lastItFeb=m_febOffsetBlobMap.find(sysId);

/*
    if(sub_Calo==CaloCell_ID::LAREM) { subHash_calo=sub_Hash; }
    else if(sub_Calo==CaloCell_ID::LARHEC) { subHash_calo=sub_Hash+173312; }
    else if(sub_Calo==CaloCell_ID::LARFCAL) { subHash_calo=sub_Hash+173312+5632; }
    else if(sub_Calo==CaloCell_ID::TILE) { subHash_calo=sub_Hash+173312+5632+3524; }
*/

    const CaloDetDescrElement* caloDDEl=  cell->caloDDE();
    Identifier cell_ID_1  = caloDDEl->identify(); 
    const unsigned int cell_ID_1_value = cell_ID_1.get_compact();
    float cell_x = caloDDEl->x(); // or x_raw()
    float cell_y = caloDDEl->y(); // or y_raw() 
    float cell_z = caloDDEl->z(); // or z_raw() 
    
    float febchaOff = 0;
    float errOff[2]={0};
    float eneOff[15]={0};
    float runOff = 0; 
    float runEntry = 0; 

    if(m_lastItFeb!=m_febOffsetBlobMap.end()){  
     const CaloCondBlobFlt* const flt_feb = m_lastItFeb->second;
     febchaOff = flt_feb->getData(subHash_calo, dbGain, 0);
     errOff[0] = flt_feb->getData(subHash_calo, dbGain, 1);
     errOff[1] = flt_feb->getData(subHash_calo, dbGain, 2);
     eneOff[0] = flt_feb->getData(subHash_calo, dbGain, 3); 
     eneOff[1] = flt_feb->getData(subHash_calo, dbGain, 4); 
     eneOff[2] = flt_feb->getData(subHash_calo, dbGain, 5); 
     eneOff[3] = flt_feb->getData(subHash_calo, dbGain, 6); 
     eneOff[4] = flt_feb->getData(subHash_calo, dbGain, 7); 
     eneOff[5] = flt_feb->getData(subHash_calo, dbGain, 8); 
     eneOff[6] = flt_feb->getData(subHash_calo, dbGain, 9); 
     eneOff[7] = flt_feb->getData(subHash_calo, dbGain, 10); 
     eneOff[8] = flt_feb->getData(subHash_calo, dbGain, 11); 
     eneOff[9] = flt_feb->getData(subHash_calo, dbGain, 12); 
     eneOff[10] = flt_feb->getData(subHash_calo, dbGain, 13);
     eneOff[11] = flt_feb->getData(subHash_calo, dbGain, 14);
     eneOff[12] = flt_feb->getData(subHash_calo, dbGain, 15);
     eneOff[13] = flt_feb->getData(subHash_calo, dbGain, 16);
     eneOff[14] = flt_feb->getData(subHash_calo, dbGain, 17);
    }
    else {
      ATH_MSG_WARNING("no sysId="<<sysId<<" find in m_febOffsetBlobMap");
      flag += ( 0x1 << DB_ALL_MISSING); 
    }

    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
    const LArOnOffIdMapping* cabling{*cablingHdl};
    if(!cabling) {
       ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
       return;
    }
    //if(run_number < 177986 || run_number > 191933) flag += ( 0x1 << RUN_NO_OUT_OF_RANGE);
    IdentifierHash cell_onHash = caloDDEl->onl1();
    HWIdentifier cell_onID = m_LArOnlineIDHelper->channel_Id(cell_onHash) ;
    HWIdentifier cell_onID_1  = cabling->createSignalChannelID(cell_ID);
    HWIdentifier feb_onID = m_LArOnlineIDHelper->feb_Id(cell_onID_1);
    HWIdentifier ft_onID = m_LArOnlineIDHelper->feedthrough_Id(cell_onID_1);
    bool ifValidId = m_LArOnlineIDHelper->isValidId(ft_onID);
    ATH_MSG_DEBUG("ifValidId="<<ifValidId );
    if(!ifValidId) {
      ATH_MSG_WARNING( "ft_onID is not valid" );
      flag += ( 0x1 << CELL_FTONID_INVALID); 
      return;
    }
    const unsigned int cell_onHash_value = cell_onHash;
    const unsigned int ft_onID_value = ft_onID.get_compact();
    const unsigned int feb_onID_value = feb_onID.get_compact();
    const unsigned int cell_onID_value = cell_onID.get_compact();
    const unsigned int cell_onID_1_value = cell_onID_1.get_compact();
    int feedthrough = m_LArOnlineIDHelper->feedthrough(ft_onID);  // [0,31] ==barrel A/C 
                                                                 // 00,01,04,07,08,11,12,13,14,17,18,19,20,23,24  = EMEC Standard crates 
                                                                 // 02,09,15,21           = EMEC Special crates
                                                                 // 03,10,16,22           = HEC crates
                                                                 // 06                    = FCAL crate 
    int pos_neg = m_LArOnlineIDHelper->pos_neg(ft_onID);  // A:0; C:1
    int barrel_ec = m_LArOnlineIDHelper->barrel_ec(ft_onID); // Barrel:0; EC:1
    int index = feedthrough;
    if(!barrel_ec){ // barrel
       if(pos_neg) index = feedthrough; //barrel C  [0,31]
       else  index = 32 + feedthrough;  // barrel A  [32,63]
    }
    else { //EC 
      if(pos_neg) index = 64 + feedthrough; // EMEC C [64,88]
      else  index = 25 + 64 + feedthrough; // EMEC A [89,113]
    }

    if(m_runOffsetBlob){
      runOff = m_runOffsetBlob->getData(index, dbGain, 0);
      runEntry = m_runOffsetBlob->getData(index, dbGain, 1);
    }
    else {
      ATH_MSG_WARNING("m_runOffsetBlob is empty"); 
      flag += ( 0x1 << DB_RUN_MISSING); 
    }

    if (msgLvl(MSG::DEBUG)){
      ATH_MSG_DEBUG("cell_ID_value="<<cell_ID_value<<"; cell_ID_1_value="<<cell_ID_1_value <<"; cellHash="<<cellHash<<"; sub_Hash="<<sub_Hash<<"; subHash_calo="<<subHash_calo <<"; gain=" << dbGain);
      ATH_MSG_DEBUG("febchaOff="<<febchaOff);
      ATH_MSG_DEBUG("errOff[0]="<<errOff[0]);
      ATH_MSG_DEBUG("errOff[1]="<<errOff[1]);
      ATH_MSG_DEBUG("eneOff[0]="<<eneOff[0]);
      ATH_MSG_DEBUG("eneOff[1]="<<eneOff[1]);
      ATH_MSG_DEBUG("eneOff[2]="<<eneOff[2]);
      ATH_MSG_DEBUG("eneOff[3]="<<eneOff[3]);
      ATH_MSG_DEBUG("eneOff[4]="<<eneOff[4]);
      ATH_MSG_DEBUG("eneOff[5]="<<eneOff[5]);
      ATH_MSG_DEBUG("eneOff[6]="<<eneOff[6]);
      ATH_MSG_DEBUG("eneOff[7]="<<eneOff[7]);
      ATH_MSG_DEBUG("eneOff[8]="<<eneOff[8]);
      ATH_MSG_DEBUG("eneOff[9]="<<eneOff[9]);
      ATH_MSG_DEBUG("eneOff[10]="<<eneOff[10]);
      ATH_MSG_DEBUG("eneOff[11]="<<eneOff[11]);
      ATH_MSG_DEBUG("eneOff[12]="<<eneOff[12]);
      ATH_MSG_DEBUG("eneOff[13]="<<eneOff[13]);
      ATH_MSG_DEBUG("eneOff[14]="<<eneOff[14]);
      ATH_MSG_DEBUG("cell_onHash_value="<<cell_onHash_value <<"cell_onID_value="<<cell_onID_value<<";cell_onID_1_value= "<<cell_onID_1_value<<";feb_onID_value"<<feb_onID_value<<";ft_onID_value="<<ft_onID_value<<";feedthrough="<<feedthrough<<";pos_neg="<<pos_neg<<";barrel_ec="<<barrel_ec<<";index="<<index<<"; gain="<< dbGain);
      ATH_MSG_DEBUG("runOff="<<runOff);
      ATH_MSG_DEBUG("runEntry="<<runEntry);
    }

    if(runOff==0.) flag += ( 0x1 << RUN_NO_CORRECTION_ZERO);
    if(runEntry<50) flag += ( 0x1 << RUN_NO_CORRECTION_LOW_STATS);

    // pass1 correction : HFEC depended run correction
    time = time - runOff;

    // pass2,3 correction : feb correction, channel correction
    time = time - febchaOff;

    // pass4 correction: slot(FEB) depended energy correction
    time = time - energyCorr(eneOff,15, e_GEV);
    if(e_GEV<5.0 || e_GEV>250.0) flag += ( 0x1 << ENERGY_CORRECTION_FAILED);

    // pass5 correction: primary vertex correction
    time = time - pvCorr(cell_x, cell_y, cell_z, pv_z);

    error = errorCompute(errOff, 2, e_GEV);  
}

float CaloClusterTimeTool::energyCorr(float eneOff[], int num, float energy){
 ATH_MSG_DEBUG("inside energyCorr");
 if(num < 15){
   ATH_MSG_ERROR( "less than 15 parameters are passed to energyCorr" );
   return 0;
 }  

 float ene_use=0.;
 float tOff=0.; 

 if(eneOff[0]==eneOff[8]){
   if(energy<eneOff[0]) ene_use=eneOff[0];
   else if(energy>eneOff[1]) ene_use=eneOff[1]; 
   else ene_use=energy;

   tOff=eneOff[2]+
        eneOff[3]*ene_use+
        eneOff[4]*ene_use*ene_use+
        eneOff[5]*ene_use*ene_use*ene_use+
        eneOff[6]*ene_use*ene_use*ene_use*ene_use+
        eneOff[7]*ene_use*ene_use*ene_use*ene_use*ene_use;
 }
 else {
  if(energy>=eneOff[8]){
    if(energy>eneOff[1]) ene_use=eneOff[1];
    else ene_use=energy;

    tOff=eneOff[2]+
         eneOff[3]*ene_use+
         eneOff[4]*ene_use*ene_use+        
         eneOff[5]*ene_use*ene_use*ene_use+
         eneOff[6]*ene_use*ene_use*ene_use*ene_use+
         eneOff[7]*ene_use*ene_use*ene_use*ene_use*ene_use;
  }
  else{
    if(energy<eneOff[0]) ene_use=eneOff[0];
    else ene_use=energy;

    tOff=eneOff[9]+
         eneOff[10]*ene_use+
         eneOff[11]*ene_use*ene_use+        
         eneOff[12]*ene_use*ene_use*ene_use+
         eneOff[13]*ene_use*ene_use*ene_use*ene_use;
  }
 }
 
 return tOff; 
} 

float CaloClusterTimeTool::pvCorr(float cellX, float cellY, float cellZ, float pvZ){
  ATH_MSG_DEBUG("inside pvCorr");
  float r_cen = sqrt(cellX*cellX + cellY*cellY + cellZ*cellZ);
  float r_pv = sqrt(cellX*cellX + cellY*cellY + (cellZ-pvZ)*(cellZ-pvZ));
  float tOff = (r_pv-r_cen)*(0.01/3.0);

  return tOff;
}

float CaloClusterTimeTool::errorCompute(float errOff[], int num, float energy){
 ATH_MSG_DEBUG("inside errorCompute");
 if(num < 2){
   ATH_MSG_ERROR( "less than 2 parameters are passed to errorCompute" );
   return 0;
 }

 const float relerr = errOff[0]/energy;
 float error = sqrt( relerr*relerr  + errOff[1]*errOff[1] );

 return error;
}



