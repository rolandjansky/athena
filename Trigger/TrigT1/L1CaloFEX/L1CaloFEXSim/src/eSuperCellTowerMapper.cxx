/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "L1CaloFEXSim/eTowerBuilder.h"
#include "TROOT.h"
#include "TH1.h"
#include "TH1F.h"
#include "TPad.h"
#include "TCanvas.h"
#include "L1CaloFEXSim/eSuperCellTowerMapper.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/errorcheck.h"

//using Athena_test::URNG;
//using Athena_test::randi_seed;
//using Athena_test::randf_seed;

// This is a class which is designed to receive in a list of supercells and a list of eTowers and match them together appropriately.


/* VERY IMPORTANT NOTE.  ARE YOU CONFUSED ABOUT SAMPLING IDs? WANT TO KNOW WHERE ON EARTH THEY COME FROM? FINDING OBSCURE VALUES THAT ARE APPARENTLY MEANINGLESS?

LOOK HERE!: athena/Calorimeter/CaloGeoHelpers/CaloGeoHelpers/CaloSampling.def

*/


namespace LVL1 {

eSuperCellTowerMapper::eSuperCellTowerMapper(const std::string& type,const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent)
{
  declareInterface<IeSuperCellTowerMapper>(this);

}

eSuperCellTowerMapper::~eSuperCellTowerMapper()
{

}

StatusCode eSuperCellTowerMapper::initialize()
{
 
  ATH_CHECK( m_scellsCollectionSGKey.initialize() );
  ATH_CHECK( m_triggerTowerCollectionSGKey.initialize() );

  return StatusCode::SUCCESS;
    
}

StatusCode eSuperCellTowerMapper::AssignTriggerTowerMapper(/*eTowerContainer* my_eTowerContainerRaw*/ std::unique_ptr<eTowerContainer> & my_eTowerContainerRaw){
  
  SG::ReadHandle<xAOD::TriggerTowerContainer> jk_triggerTowerCollection(m_triggerTowerCollectionSGKey/*,ctx*/);
  if(!jk_triggerTowerCollection.isValid()){
    ATH_MSG_FATAL("Could not retrieve jk_triggerTowerCollection " << m_triggerTowerCollectionSGKey.key() );
    return StatusCode::FAILURE;
  }

  for(auto eachTower : *jk_triggerTowerCollection) {
    if(fabs(eachTower->eta())<1.5 && eachTower->sampling()==1) {
      int i_phi = int(eachTower->phi()/(3.14159/32));
      int etaSign{-1};
      int towerID_Modifier{100000};
      if (eachTower->eta() > 0) {
        etaSign = 1;
        towerID_Modifier = 200000;
      }
      int i_eta = int(eachTower->eta() * 10) * etaSign;
      if(i_eta * etaSign == -14) {
        towerID_Modifier = 300000;
      } else if (i_eta * etaSign == 14) {
        towerID_Modifier = 400000;
      }

      int towerid = FindTowerIDForSuperCell(i_eta, i_phi) + towerID_Modifier;
      LVL1::eTower * targetTower; 
      if((targetTower = my_eTowerContainerRaw->findTower(towerid))) {
        if (targetTower->getET_float(4, 0) > 0) {
          ATH_MSG_WARNING("\n==== eSuperCellTowerMapper ============ Hadronic layer energy filled more than once - it will be ignored. (Needs investigation).  Please report this!");
        }
        targetTower->setET(10, int(eachTower->cpET()) * 0.5 * 1000., 4);
      } else {
        ATH_MSG_WARNING("\n==== eSuperCellTowerMapper ============ Tower id is officially unknown - it will be ignored. (Needs investigation).  Please report this!");
      }
    }
  }
  return StatusCode::SUCCESS;
}

void eSuperCellTowerMapper::reset(){
  return;
}

  // works for real supercells from MC
  StatusCode eSuperCellTowerMapper::AssignSuperCellsToTowers(/*eTowerContainer* my_eTowerContainerRaw*/std::unique_ptr<eTowerContainer> & my_eTowerContainerRaw)
{

  bool doPrint = false;

  SG::ReadHandle<CaloCellContainer> jk_scellsCollection(m_scellsCollectionSGKey/*,ctx*/);
  if(!jk_scellsCollection.isValid()){
    ATH_MSG_FATAL("Could not retrieve jk_scellsCollection " << m_scellsCollectionSGKey.key() );
    return StatusCode::FAILURE;
  }


  //const CaloCell_Base_ID* idHelper = caloIdManager->getCaloCell_SuperCell_ID(); // getting the id helper class
  const CaloCell_Base_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_SuperCell_ID") );

  for (CaloCellContainer::const_iterator cell = jk_scellsCollection->begin(); cell != jk_scellsCollection->end(); ++cell){
    const CaloSampling::CaloSample sample = (*cell)->caloDDE()->getSampling();
    const Identifier ID = (*cell)->ID(); // super cell unique ID
    int region = idHelper->region(ID);
    int layer = -1;
    int pos_neg = idHelper->pos_neg(ID);
    int eta_index = idHelper->eta(ID);
    const int phi_index = idHelper->phi(ID);
    float et = (*cell)->energy();
    int prov = (*cell)->provenance();

    /*
  CaloSampling:
    PreSamplerB  0
           EMB1  1
           EMB2  2
           EMB3  3

    PreSamplerE  4
           EME1  5
           EME2  6
           EME3  7

           HEC0  8
           HEC1  9
           HEC2  10
           HEC3  11
    */

    //We need to explicitly avoid +/- 3 pos_neg supercells! These go beyond |eta| == 2.5
    if(abs(pos_neg) == 3){ continue; }

    // LOCAL TO GLOBAL ETA INDEX PATCH - USE A 'TOWER OFFSET' TO MARK THE START OF THE ETA_INDEX COUNTING (e.g. the rounded eta value of the innermost supercell)
    switch(sample){
    case CaloSampling::PreSamplerB: { break; }
    case CaloSampling::EMB1: {
      if(region == 1){ eta_index += 56; }
      break;
    }
    case CaloSampling::EMB2: {
      if(region == 1){ eta_index += 56; }
      break;
    }
    case CaloSampling::EMB3: { break; }
    case CaloSampling::PreSamplerE: { eta_index += 15; break; }
    case CaloSampling::EME1: {
      if(region == 0){ eta_index += 14; }
      else if (region == 1){ /* doesn't exist */ }
      else if (region == 2){ eta_index += 60; }
      else if (region == 3){ eta_index += 108; } //6 supercell region
      else if (region == 4){ eta_index += 80; }
      else if (region == 5){ eta_index += 24; }
      break;
    }
    case CaloSampling::EME2: {
      if(region == 0){ eta_index += 14; }
      else if (region == 1){ eta_index += 57; }
      break;
    }
    case CaloSampling::EME3: { eta_index += 15; break; }
    case CaloSampling::HEC0:
    case CaloSampling::HEC1:
    case CaloSampling::HEC2:
    case CaloSampling::HEC3: { eta_index += 15; break; }
    default: { /*ATH_MSG_DEBUG("Not doing anything since sample = " << sample);*/ break; }
    }


    FindAndConnectTower(my_eTowerContainerRaw,sample,region,layer,pos_neg,eta_index,phi_index,ID,et,prov,doPrint);
  }
  
  return StatusCode::SUCCESS;
  
}


  int eSuperCellTowerMapper::ConnectSuperCellToTower(/*eTowerContainer* my_eTowerContainerRaw*/std::unique_ptr<eTowerContainer> & my_eTowerContainerRaw,int iETower, Identifier ID, int iCell, float et, int layer, bool doenergysplit){

  if(my_eTowerContainerRaw->findTower(iETower)){
    (my_eTowerContainerRaw->findTower(iETower))->setSCID(ID,iCell,et,layer,doenergysplit);
  }

  return 1;

}

  int eSuperCellTowerMapper::FindAndConnectTower(/*eTowerContainer* my_eTowerContainerRaw*/std::unique_ptr<eTowerContainer> & my_eTowerContainerRaw,CaloSampling::CaloSample sample,const int region, int layer, const int pos_neg, const int eta_index, const int phi_index, Identifier ID, float et, int prov,bool doPrint)
{

  // bool for the special case of 1.8 < eta < 2.0 only in the front layer
  bool doenergysplit = false;
  
  // bool as a flag to enable or disable the connection of supercells to towers according to their location and identities
  bool validcell = true;

  // We tell the eTower which supercell unique ID is in each of it's available 'slots'
  int towereta = -99; // to be filled with tower eta value to help find it
  int towerphi = -99; // to be filled with tower phi value to help find it
  int iETower = -99; // The tower ID which the supercell will be assigned to
  int iCell = -1; // The position within the Tower that the supercell will be assigned to
  int towerID_Modifier = -999999999; // adjustable tower ID modifier to fit tower Id scheme

  /*    
  ** iCell Numbering Scheme **
  
  Let's use the following scheme (from low to high eta regardless of detector side):
  Layer 0:  Cell 0
  Layer 1:  Cell 1, 2, 3, 4
  Layer 2:  Cell 5, 6, 7, 8
  Layer 3:  Cell 9
  Layer 4:  Cell 10 (HEC or TILE, if we have them!)
  */
  

  /*
  ** Structure of the EMB in Supercells **
  
  0 < 1.4 in steps of 0.1 == 14 towers
  1.4 < 1.52 in steps of 0.12  ==> THIS TOWER IS MOVED TO TRANS TOWER REGION
  14 towers total (all by presampler)
  64 towers in phi in total
  
  896 towers in EMB per side
  1792 towers in EMB total  
  */
  
  /*
  ** Structure of the TRANS in Supercells **

  1.4 < 1.5, 1 special tower made from EMB and EME supercells together
  1 tower in eta
  64 towers in phi

  64 towers in TRANS total
  */

  /*
  ** Structure of the EMEC in Supercells **
  
  1.375 < 1.5, funny behaviour, treated as 1 special tower ==> 1 tower === TRANSITION TOWER NOW!
  1.5 < 1.8 standard towers by presampler ==> 3 towers
  1.8 < 2.0 standard towers by back ==> 2 towers
  2.0 < 2.4 standard towers by back ==> 4 towers
  2.4 < 2.5 standard tower by back ==> 1 tower
  2.5 < 3.1, funny behaviour, treated as 3 wide towers ==> 3 towers.   Also wide in phi!  ===> WE DON'T CARE ABOUT |ETA| > 2.5
  3.1 < 3.2 standard tower by back ==> 1 tower.  Also wide in phi!  ===> WE DON'T CARE ABOUT |ETA| > 2.5
  10 towers in eta in total (by different means) (IGNORING ABOVE |ETA| > 2.5)
  64 towers initially in phi (IGNORING ABOVE |ETA| > 2.5)
  
  640 towers in EME per side
  1280 towers in EME total  
  */
  

  /*
  ** Structure of the HEC in Supercells **
  
  1.5 - 2.5, [Region 0] standard towers by presampler ==> layer on top of existing towers
  
  */
  
  /*
  ** Notes on overall structure **
    Total number of expected towers can be as high as: 896+640 = 1536 per side + 64 transition region towers (TRANS) each side = 3200 total <=== This is the correct value!
  */

  /*
  ** Notes on Tower ID Number Scheme **

    Left Barrel IETower = 100000 + X
    Right Barrel IETower = 200000 + X
    Left TRANS IETower = 300000 + X;
    Right TRANS IETower = 400000 + X;
    Left Endcap IETower = 500000 + X
    Right Endcap IETower = 600000 + X
    Left Hadronic Endcap IETower =  11100000 + X --> These are just Layer 5 of Endcap Towers.  They will never be generated as standalone eTowers.
    Right Hadronic Endcap IETower =  22200000 + X --> These are just Layer 5 of Endcap Towers.  They will never be generated as standalone eTowers.

  */


  //----------------------------------------------------------

  // Begin assigning supercells to Towers

  //----------------------------------------------------------

  switch (sample) {
    // E.M. Barrel (EMB)
  case CaloSampling::PreSamplerB: { // DONE
    // Presampler of the EMBarrel.  Supercells are 0.1 x 0.1 unless stated otherwise
    // Region 0 has 14 supercells in 0 < eta < 1.4 and 64 supercells in phi.
    // Region 0 has 1 supercell (0.12 x 0.1) in 1.4 < eta < 1.52 and 64 supercells in phi.  // Documentation describes this as Region 1 but the code does not. We go with Region 0.

    layer = 0; // By definition
    
    towereta = eta_index; // Layer 0 has super cells which are 0.1 x 0.1 - i.e. the full width of a tower - sothe eta_index matches up to the Tower Eta
    towerphi = phi_index; // phi is standard also
    
    if(eta_index == 14){ // special treatment for TRANSITION tower
      if(pos_neg < 0){ towerID_Modifier = 300000; }
      else if(pos_neg > 0){ towerID_Modifier = 400000; }
      iCell = 0; // By definition
    }
    else{ // standard treatment for non-transition towers
      if(pos_neg < 0){ towerID_Modifier = 100000; }
      else if(pos_neg > 0){ towerID_Modifier = 200000; }
      iCell = 0; // By definition
    }

    break;
  }
  case CaloSampling::EMB1: 
  case CaloSampling::EMB2: {
    // Layer 1 of the EMBarrel.  Supercells are 0.025 x 0.1 unless stated otherwise
    // Region 0 has 56 supercells in 0 < eta < 1.4 and 64 supercells in phi.
    // Region 1 has 3 supercells in 1.4 < eta < 1.475 and 64 supercells in phi.
    
    // Layer 2 of the EMBarrel.  Supercells are 0.025 x 0.1 unless stated otherwise
    // Region 0 has 56 supercells in 0 < eta < 1.4 and 64 supercells in phi.
    // Region 1 has 1 supercells (0.075 x 0.1) in 1.4 < eta < 1.475 and 64 supercells in phi.

    if (region == 0) {
      towereta = eta_index / 4; // this divides integers by 4 and truncates away the decimals (rounds DOWN to an integer)
      towerphi = phi_index;
      if(pos_neg < 0){ towerID_Modifier = 100000; }
      else if(pos_neg > 0){ towerID_Modifier = 200000; }
    }
    else if (region == 1){
      towereta = 14; // hardcoded but is correct
      towerphi = phi_index;
      if(pos_neg < 0){ towerID_Modifier = 300000; }
      else if(pos_neg > 0){ towerID_Modifier = 400000; }
    }
    else {
      ATH_MSG_DEBUG("[CaloSampling::EMB1 or CaloSampling::EMB2] -> invalid 'region' value: " << region << " (Under investigation) ");
    }

    switch(sample){
    case CaloSampling::EMB1: { 
      iCell = (eta_index % 4) + 1; 
      layer = 1; 
      break;
    }
    case CaloSampling::EMB2: { 
      if (region == 0) { iCell = (eta_index % 4) + 5; }
      else if (region == 1){ iCell = 5; }
      layer  = 2; 
      break;
    }
    default: {
      ATH_MSG_DEBUG("CaloSampling::EMBX -> invalid sample for assigning iCell value! " << sample << " (Under investigation) ");
      break;
    }
    }

    break;
  }
  case CaloSampling::EMB3: {
    // Layer 3 of the EMBarrel.  Supercells are 0.1 x 0.1 unless stated otherwise
    // Region 0 has 14 supercells in 0 < eta < 1.4 and 64 supercells in phi.
    // Region 1 does not exist
    
    layer = 3; // By definiton
    towereta = eta_index; // Analogous to PreSamplerB
    towerphi = phi_index;
    
    iCell = 9; // By definition;
    
    if(pos_neg < 0){ towerID_Modifier = 100000; }
    else if(pos_neg > 0){ towerID_Modifier = 200000; }
    
    break;
  }
    // E.M. End-cap (EMEC)
  case CaloSampling::PreSamplerE: {
    // Region 1 has 3 supercells in 1.5 < eta < 1.8, and 64 supercells in phi.
    // Supercells are 0.1 x 0.1.
    
    layer = 0;

    towereta = eta_index;
    towerphi = phi_index;
    
    iCell = 0; // By definition
    
    if(pos_neg < 0){ towerID_Modifier = 500000; }
    else if(pos_neg > 0){ towerID_Modifier = 600000; }
    
    break;
  }
  case CaloSampling::EME1: {
    // Layer 1 of the EM End-Cap.  Supercells very frequently change in size.
    // Region 0 has 1 supercell in 1.375 < eta < 1.5, and 64 supercells in phi.  Supercells are 0.125 x 0.1.
    // Region 1 has 12 supercells in 1.5 < eta < 1.8, and 64 supercells in phi.  Supercells are 0.025 x 0.1.
    // Region 2 has 6 (now 12??) supercells in 1.8 < eta < 2.0, and 64 supercells in phi.  Supercells are 0.0333 (0.016667 ???) x 0.1.
    // Region 3 has 16 supercells in 2.0 < eta < 2.4, and 64 supercells in phi.  Supercells are 0.025 x 0.1.
    // Region 4 has 1 supercell in 2.4 < eta < 2.5, and 64 supercells in phi.  Supercells are 0.1 x 0.1.
    // Region 5 has 3 supercells in 2.5 < eta < 3.1, and 32 supercells in phi.  Supercells are 0.2 x 0.2.
    // Region 6 has 1 supercell in 3.1 < eta < 3.2, and 32 supercells in phi.  Supercells are 0.1 x 0.2
    
    // 1.375 < 1.5, funny behaviour, treated as 1 special tower ==> 1 tower
    // 1.5 < 1.8 standard towers by presampler ==> 3 towers
    // 1.8 < 2.0 standard towers by back ==> 2 towers
    // 2.0 < 2.4 standard towers by back ==> 4 towers
    // 2.4 < 2.5 standard tower by back ==> 1 tower
    // 2.5 < 3.1, funny behaviour, treated as 3 wide towers ==> 3 towers
    // 3.1 < 3.2, funny behaviour, treated as 1 tower ==> 1 tower
    // 15 towers in total (by different means)
    
    layer = 1; // By definition
    
    switch (region) {
    case 0: { // special treatment for transition region
      
      towereta = eta_index;
      towerphi = phi_index;

      iCell = 4; // Placement based on advice from Alan Watson
      break;
    }
    case 2: {

      towereta = (eta_index / 4);
      towerphi = phi_index;
      
      iCell = (eta_index % 4) + 1;
      break;
    }
    case 3: {

      // calc ID
      towereta = (eta_index / 6);
      towerphi = phi_index;
      
      iCell = (eta_index % 6) + 1;

      if(iCell == 1){ iCell = 1; doenergysplit = false; }
      else if( iCell == 2 ){ iCell = 1; doenergysplit = true; }
      else if( iCell == 3 ){ iCell = 2; doenergysplit = false; }
      else if( iCell == 4 ){ iCell = 3; doenergysplit = false; }
      else if( iCell == 5 ){ iCell = 3; doenergysplit = true; }
      else if( iCell == 6 ){ iCell = 4; doenergysplit = false; }

      break;

      // OUTDATED CODE LEFT HERE AS A FRIENDLY REMINDER OF SPECIAL STRUCTURE INCASE A FUTURE ISSUE ARISES======================
      // Begin Dima---
      // This is the special region, with 6 supercells per tower
      // Idea here is to divide 2 out of 6 by 2 and add half ET to each of 2 "normal" SC
      //iETower = (eta_index / 6) + 18;

      // These are the cells we will split
      //if (eta_index % 3 == 1) {
      //iCell = (eta_index % 6 < 3 ? 0 : 2);
      //iCell2 = iCell + 1;
      //} else {
      // These ones just need assigning to the correct location
      // So that 0, 2, 3, 5 => 0, 1, 2, 3
      //iCell = (eta_index % 6) / 1.45;
      //}
      // end DIMA---
      // ======================================================================================================================
    }
    case 4: {

      towereta = (eta_index / 4);
      towerphi = phi_index;
      
      iCell = (eta_index % 4) + 1;
      break;
    }
    case 5: {

      towereta = eta_index ;
      towerphi = phi_index;
      
      iCell = 1;
      break;
    }
    default: {
      // This should never fire because we don't go beyond 2.5 
      ATH_MSG_DEBUG("CaloSampling::EME1 -> invalid 'region' value: " << region << " (Under investigation) ");
      break;
    }
      break;
    }

    if(region != 0){
      if(pos_neg < 0){ towerID_Modifier = 500000; }
      else if(pos_neg > 0){ towerID_Modifier = 600000; }
    }
    else if(region == 0){
      // TRANSITION REGION TREATMENT!
      if(pos_neg < 0){ towerID_Modifier = 300000; }
      else if(pos_neg > 0){ towerID_Modifier = 400000; }
    }

    break;
  }
  case CaloSampling::EME2: {
    
    // Layer 2 of the EM End-Cap.  Supercells very frequently change in size.
    // Region 0 has 1 supercell in 1.375 < eta < 1.425, and 64 supercells in phi.  Supercells are 0.05 x 0.1.
    // Region 0 also has 3 supercells in 1.425 < eta < 1.5, and 64 supercells in phi.  Supercells are 0.025 x 0.1
    // Region 1 has 12 supercells in 1.5 < eta < 1.8, and 64 supercells in phi.  Supercells are 0.025 x 0.1.
    // Region 2 has 8 supercells in 1.8 < eta < 2.0, and 64 supercells in phi.  Supercells are 0.025 x 0.1.
    // Region 3 has 16 supercells in 2.0 < eta < 2.4, and 64 supercells in phi.  Supercells are 0.025 x 0.1.
    // Region 4 has 4 supercells in 2.4 < eta < 2.5, and 64 supercells in phi.  Supercells are 0.025 x 0.1.
    // Region 5 has 3 supercells in 2.5 < eta < 3.1, and 32 supercells in phi.  Supercells are 0.2 x 0.2.
    // Region 6 has 1 supercell in 3.1 < eta < 3.2, and 32 supercells in phi.  Supercells are 0.1 x 0.2
    
    layer = 2;
    
    switch (region) {
    case 0: { // special treatment for TRANSITON region

      layer = 3; // change layer label for ET threshold treatment since we are treating this as a layer3 cell - it's an extreme special case cell as part of tne transition region

      towereta = eta_index;
      towerphi = phi_index;

      iCell = 9;

      break;
    }
    case 1: {

      towereta = (eta_index / 4);
      towerphi = phi_index;
      
      iCell = (eta_index % 4) + 5;
     
      break;
    }
    default: {
      ATH_MSG_DEBUG("CaloSampling::EME2 -> invalid 'region' value: " << region << " (Under investigation) ");
      break;
    }
      break;
    }
    
    if(region == 0){
      // TRANSITION REGION TREATMENT! 
      if(pos_neg < 0){ towerID_Modifier = 300000; } 
      else if(pos_neg > 0){ towerID_Modifier = 400000; }
    }
    else {
      if( (eta_index / 4) < 15 ){ 
	if(pos_neg < 0){ towerID_Modifier = 300000; }
	else if(pos_neg > 0){ towerID_Modifier = 400000; }
      }
      else{
	if(pos_neg < 0){ towerID_Modifier = 500000; }
	else if(pos_neg > 0){ towerID_Modifier = 600000; }
      }
    }

    break;
  }
  case CaloSampling::EME3: {
    
    // Layer 3 of the EM End-Cap.  Supercells are 0.1 x 0.1 unless stated otherwise.
    // Region 0 does not exist.
    // Region 1 has 3 supercells in 1.5 < eta < 1.8, and 64 supercells in phi.  Supercells are 0.1 x 0.1.
    // Region 2 has 2 supercells in 1.8 < eta < 2.0, and 64 supercells in phi.  Supercells are 0.1 x 0.1.
    // Region 3 has 4 supercells in 2.0 < eta < 2.4, and 64 supercells in phi.  Supercells are 0.1 x 0.1.
    // Region 4 has 1 supercells in 2.4 < eta < 2.5, and 64 supercells in phi.  Supercells are 0.1 x 0.1.
    // No other Regions exist
    
    layer = 3; // By definition
    
    switch (region) {
    case 0: {

      towereta = eta_index;
      towerphi = phi_index;
      
      iCell = 9; // By definition
      
      break;
    }
    default: {
      ATH_MSG_DEBUG("CaloSampling::EME3 -> invalid 'region' value: " << region << " (Under investigation) ");
      break;
    }
      break;
    }
    
    if(pos_neg < 0){ towerID_Modifier = 500000; }
    else if(pos_neg > 0){ towerID_Modifier = 600000; }

    break;
  }
    // Hadronic End-cap (HEC)
  case CaloSampling::HEC0:
  case CaloSampling::HEC1:
  case CaloSampling::HEC2:
  case CaloSampling::HEC3: {
    
    // All Layers of the Hadronic End-Cap.
    // Region 0 has 10 supercells in 1.5 < eta < 2.5, and 32 supercells in phi.  Supercells are 0.1 x 0.1.
    // Region 1 has 4 supercells in 2.5 < eta < 3.3, and 16 supercells in phi.  Supercells are 0.2 x 0.2.
    
    switch(region){
    case 0: {

      towereta = eta_index;
      towerphi = phi_index;
      
      layer = 4; // By definition
      
      switch (sample){ // only one supercell per layer in all regions for HECX
      case CaloSampling::HEC0: { iCell = 10; break; }
      case CaloSampling::HEC1: { iCell = 11; break; }
      case CaloSampling::HEC2: { iCell = 12; break; }
      case CaloSampling::HEC3: { iCell = 13; break; }
      default: {
	ATH_MSG_DEBUG("CaloSampling::HECX -> invalid sample for assigning iCell value! " << sample << " (Under investigation) ");
	break;
      }
      }
      break;
      
    }
    case 1: {
      validcell = false; // any region 1 HEC cells are actually with eta > 2.5
      //ATH_MSG_DEBUG("CaloSampling::HECX Region 1 invalid as outside of eta range!");
      break;
    }
    default: { break; }
    }
    
    // Tower connection
    if(pos_neg < 0){ towerID_Modifier = 500000; }
    else if(pos_neg > 0){ towerID_Modifier = 600000; }
    
    break;
  }
  case CaloSampling::TileBar0:
  case CaloSampling::TileBar1:
  case CaloSampling::TileBar2: {
    REPORT_MESSAGE_WITH_CONTEXT (MSG::DEBUG, "eSuperCellTowerMapper") << "Supercell is from Tile Barrel - it will be ignored.";
    validcell = false;
    //ATH_MSG_DEBUG("\n==== eSuperCellTowerMapper ============ Supercell is from Tile Barrel - it will be ignored.");
    break;
  }
  case CaloSampling::TileGap1:
  case CaloSampling::TileGap2:
  case CaloSampling::TileGap3: {
    //ATH_MSG_DEBUG("\n==== eSuperCellTowerMapper ============ Supercell is from Tile Gap (ITC and scintillator) - it will be ignored.");
    validcell = false;
    break;
  }
  case CaloSampling::TileExt0:
  case CaloSampling::TileExt1:
  case CaloSampling::TileExt2: {
    //ATH_MSG_DEBUG("\n==== eSuperCellTowerMapper ============ Supercell is from Tile Extended Barrel - it will be ignored.");
    validcell = false;
    break;
  }
  case CaloSampling::FCAL0:
  case CaloSampling::FCAL1:
  case CaloSampling::FCAL2: {
    //ATH_MSG_DEBUG("\n==== eSuperCellTowerMapper ============ Supercell is from Forward EM endcap - it will be ignored.");
    validcell = false;
    break;
  }
  case CaloSampling::MINIFCAL0:
  case CaloSampling::MINIFCAL1:
  case CaloSampling::MINIFCAL2:
  case CaloSampling::MINIFCAL3: {
    //ATH_MSG_DEBUG("\n==== eSuperCellTowerMapper ============ Supercells is from MiniFCAL - it will be ignored.");
    validcell = false;
    break;
  }
  case CaloSampling::Unknown: {
    //ATH_MSG_WARNING("\n==== eSuperCellTowerMapper ============ Supercell sampling is officially unknown - it will be ignored. (Needs investigation).  Please report this!");
    validcell = false;
    break;
  }
  default: {
    ATH_MSG_DEBUG("\n==== eSuperCellTowerMapper ============ Supercell has invalid CaloSampling value: " << sample << " (Needs investigation).  Please report this!");
    validcell = false;
    break;
  }
  }

  if(validcell){
    iETower = FindTowerIDForSuperCell(towereta, towerphi) + towerID_Modifier;
    if(doPrint){
      PrintCellSpec(sample, layer, region, eta_index, phi_index, pos_neg, iETower, iCell, prov, ID, doenergysplit);
    }
    ConnectSuperCellToTower( my_eTowerContainerRaw, iETower, ID, iCell, et, layer, doenergysplit);
  }

  // END ITERATING OVER SUPER CELLS+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ +++++++++++++++++++++++++++++++++++++++++++++
  
  return 1;
  
  }

int eSuperCellTowerMapper::FindTowerIDForSuperCell(int towereta, int towerphi)
{

  return (towerphi + (64 * towereta));

}

  void eSuperCellTowerMapper::PrintCellSpec(const CaloSampling::CaloSample sample, int layer, const int region, const int eta_index, const int phi_index, const int pos_neg, int iETower, int iCell, int prov, Identifier ID ,bool doenergysplit)
{
  
  std::string sampleName = "";
  
  switch (sample) {
  case CaloSampling::PreSamplerB: { sampleName = "PreSamplerB"; break; }
  case CaloSampling::EMB1: { sampleName = "EMB1"; break; }
  case CaloSampling::EMB2: { sampleName = "EMB2"; break; }
  case CaloSampling::EMB3: { sampleName = "EMB3"; break; }
  case CaloSampling::PreSamplerE: { sampleName = "PreSamplerE"; break; }
  case CaloSampling::EME1: { sampleName = "EME1"; break; }
  case CaloSampling::EME2: { sampleName = "EME2"; break; }
  case CaloSampling::EME3: { sampleName = "EME3"; break; }
  case CaloSampling::HEC0: { sampleName = "HEC0"; break; }
  case CaloSampling::HEC1: { sampleName = "HEC1"; break; }
  case CaloSampling::HEC2: { sampleName = "HEC2"; break; }
  case CaloSampling::HEC3: { sampleName = "HEC3"; break; }
  default: {
    ATH_MSG_DEBUG("\n==== eSuperCellTowerMapper ============ Supercell has invalid CaloSampling value: " << sample << " (Needs investigation).  Please report this!");
    break;
  }
  }
  
  if(true){
    ATH_MSG_DEBUG("ASSIGNED CELL:::  CASE: " << sampleName
	      << "\tSample: " << sample
	      << "\tLayer: " << layer
	      << "\tRegion: " << region
	      << "\tEta_Index: " << eta_index
	      << "\tPhi_Index: " << phi_index
	      << "\tPosNeg: " << pos_neg
	      << "\tiETower: " << iETower
	      << "\tiCell: " << iCell
	      << "\tDoEnergySplit: " << doenergysplit
	      << "\tProvenance: " << prov
	      << "\tID: " << ID
		  << " ");
  }

  return;
}


} // end of LVL1 namespace
