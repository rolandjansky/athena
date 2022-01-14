/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gSuperCellTowerMapper - supercells and Tile tower assignment to gTowers
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include "L1CaloFEXSim/gSuperCellTowerMapper.h"
#include "L1CaloFEXSim/gTowerBuilder.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/errorcheck.h"


// This class is designed to receive a list of supercells and a list of gTowers and to match them together appropriately.


namespace LVL1 {

gSuperCellTowerMapper::gSuperCellTowerMapper(const std::string& type,const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent)
{
  declareInterface<IgSuperCellTowerMapper>(this);

}

gSuperCellTowerMapper::~gSuperCellTowerMapper()
{
}

StatusCode gSuperCellTowerMapper::initialize()
{

  ATH_CHECK( m_scellsCollectionSGKey.initialize() );
  ATH_CHECK( m_triggerTowerCollectionSGKey.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode gSuperCellTowerMapper::AssignTriggerTowerMapper(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw) const {

  static constexpr float delta_phi = M_PI/32;
  static constexpr float delta_eta = 0.1;

  SG::ReadHandle<xAOD::TriggerTowerContainer> triggerTowerCollection(m_triggerTowerCollectionSGKey/*,ctx*/);
  if(!triggerTowerCollection.isValid()){
    ATH_MSG_FATAL("Could not retrieve triggerTowerCollection " << m_triggerTowerCollectionSGKey.key() );
    return StatusCode::FAILURE;
  }


  for(auto eachTower : *triggerTowerCollection) {
    if(fabs(eachTower->eta())<1.5 && eachTower->sampling()==1) {
      int tile_iphi = int(eachTower->phi()/delta_phi);
      int tower_iphi = tile_iphi/2;
      int tile_ieta = int(fabs(eachTower->eta())/delta_eta);
      int tower_ieta = tile_ieta/2;
      int nphi = 32;
      int etaSign{-1};
      int towerID_Modifier{100000};

      if (eachTower->eta() > 0) {
        etaSign = 1;
        towerID_Modifier = 200000;
      }

      if(tower_ieta * etaSign == -7) {
        towerID_Modifier = 300000;
      }

      else if (tower_ieta * etaSign == 7) {
        towerID_Modifier = 400000;
      }
      int towerid = FindTowerIDForSuperCell(tower_ieta, tower_iphi, nphi) + towerID_Modifier;

      LVL1::gTower * targetTower;
      if((targetTower = my_gTowerContainerRaw->findTower(towerid)))
      {
        // Set the ET to the gTower, with layer 1 to specify this comes from the HAD
        targetTower->addET(static_cast<int>(eachTower->jepET()) * 1000., 1);
      } else
      {
        ATH_MSG_WARNING("\n==== gSuperCellTowerMapper ============ Tower id is officially unknown - it will be ignored. (Needs investigation).  Please report this!");
      }
    }
  }
  return StatusCode::SUCCESS;
}

void gSuperCellTowerMapper::reset(){
  return;
}

// works for real supercells from MC
StatusCode gSuperCellTowerMapper::AssignSuperCellsToTowers(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw) const
{
  bool doPrint = true;

  SG::ReadHandle<CaloCellContainer> scellsCollection(m_scellsCollectionSGKey/*,ctx*/);
  if(!scellsCollection.isValid()){
    ATH_MSG_FATAL("Could not retrieve scellsCollection " << m_scellsCollectionSGKey.key() );
    return StatusCode::FAILURE;
  }


  const CaloCell_Base_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_SuperCell_ID") );

  for (const auto& cell : * scellsCollection){

    const CaloSampling::CaloSample sample = (cell)->caloDDE()->getSampling();
    const Identifier ID = (cell)->ID(); // super cell unique ID
    int region = idHelper->region(ID);
    int pos_neg = idHelper->pos_neg(ID);
    int eta_index = idHelper->eta(ID);
    const int phi_index = idHelper->phi(ID);
    float et = (cell)->energy()/cosh((cell)->eta());
    int prov = (cell)->provenance();


    // I removed the LOCAL TO GLOBAL ETA INDEX PATCH for gFEX
    // Since in any case the SC assignment won't be regular, the eta and phi bins are combined directly in the FindAndConnectTower
    FindAndConnectTower(my_gTowerContainerRaw,sample,region,pos_neg,eta_index,phi_index,ID,et,prov,doPrint);

    
  }

  for (auto gTower : *my_gTowerContainerRaw)
  {
    gTower->setET();

  }


  return StatusCode::SUCCESS;

}


void gSuperCellTowerMapper::ConnectSuperCellToTower(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw, int iGTower, Identifier ID, float et) const {

  LVL1::gTower * tmpTower = my_gTowerContainerRaw->findTower(iGTower);

  if(tmpTower){
    tmpTower->setSCID(ID);
    tmpTower->addET(et, 0); // layer is always 0 (EM) for SuperCells
 
  }

}

int gSuperCellTowerMapper::FindAndConnectTower(std::unique_ptr<gTowerContainer> & my_gTowerContainerRaw,CaloSampling::CaloSample sample,const int region, const int pos_neg, const int eta_index, const int phi_index, Identifier ID, float et, int prov,bool doPrint) const
{

  bool validcell = true;

  // We tell the gTower which supercell unique ID is in each tower
  int nphi = -1; // to be filled with number of phi bins
  int towereta = -99; // to be filled with tower eta value to help find it
  int towerphi = -99; // to be filled with tower phi value to help find it
  int iGTower = -99; // The tower ID which the supercell will be assigned to
  int towerID_Modifier = -999999999; // adjustable tower ID modifier to fit tower Id scheme


  //----------------------------------------------------------

  // Begin assigning supercells to Towers

  //----------------------------------------------------------

  switch (sample) {
      // E.M. Barrel (EMB)
    case CaloSampling::PreSamplerB: {
      // The following divides integers by 2 and truncates away the decimals (rounds DOWN to an integer)
      towereta = eta_index / 2; // Layer 0 has super cells which are 0.1 x 0.1 - gTower is 0.2 x 0.2
      towerphi = phi_index / 2; // phi is 32 for gTowers

      nphi = 32;

      if(eta_index == 14){ // special treatment for the tower where EMB and EMEC overlap (for gFEX: Eta Range 1.4-1.6)
        if(pos_neg < 0){ towerID_Modifier = 300000; }
        else if(pos_neg > 0){ towerID_Modifier = 400000; }
      }
      else{ // standard treatment for non-transition towers
        if(pos_neg < 0){ towerID_Modifier = 100000; }
        else if(pos_neg > 0){ towerID_Modifier = 200000; }
      }

      break;
    }
    case CaloSampling::EMB1:
    case CaloSampling::EMB2: {

      nphi = 32;

      if (region == 0) {
        towereta = eta_index / 8;
        towerphi = phi_index / 2;
        if(pos_neg < 0){ towerID_Modifier = 100000; }
        else if(pos_neg > 0){ towerID_Modifier = 200000; }
      }
      else if (region == 1){
        towereta = 7;
        towerphi = phi_index/2;
        if(pos_neg < 0){ towerID_Modifier = 300000; }
        else if(pos_neg > 0){ towerID_Modifier = 400000; }
      }
      else {
        ATH_MSG_DEBUG("[CaloSampling::EMB1 or CaloSampling::EMB2] -> invalid 'region' value: " << region << " (Under investigation) ");
      }

      switch(sample){
      case CaloSampling::EMB1: {
        break;
      }
      case CaloSampling::EMB2: {
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

      nphi = 32;
      towereta = eta_index/2; // Analogous to PreSamplerB
      towerphi = phi_index/2;


      if(pos_neg < 0){ towerID_Modifier = 100000; }
      else if(pos_neg > 0){ towerID_Modifier = 200000; }

      break;
    }
      // E.M. End-cap (EMEC)
    case CaloSampling::PreSamplerE: {

      nphi = 32;
      towereta = 7 + std::ceil(eta_index / 2.); //This is a special case, as there are only 3 cells in eta 1.56 (gFEX bin 7), 1.66 (gFEX bin 8), 1.76 (gFEX bin 8)
      towerphi = phi_index / 2; // phi is 32 for gTowers


      if (towereta == 7){
        if(pos_neg < 0){ towerID_Modifier = 300000; }
        else if(pos_neg > 0){ towerID_Modifier = 400000; }
      }
      else if (towereta != 7){
        if(pos_neg < 0){ towerID_Modifier = 500000; }
        else if(pos_neg > 0){ towerID_Modifier = 600000; }
      }

      break;
    }
    case CaloSampling::EME1: {

      nphi = 32;

      switch (region) {
      case 0: {

        towereta = 7;
        towerphi = phi_index/2;

        break;
      }
      case 2: {

        if (eta_index < 4) {towereta = 7;}
        else {towereta = 8;}
        towerphi = phi_index/2;

        break;
      }
      case 3: {

        // calc ID
        towereta = 9;
        towerphi = phi_index/2;
        break;

      }
      case 4: {

        towereta = 10 + (eta_index / 8);
        towerphi = phi_index/2;

        break;
      }
      case 5: {

        towereta = 11 ;
        towerphi = phi_index/2;

        break;
      }
      default: {
        ATH_MSG_DEBUG("CaloSampling::EME1 -> invalid 'region' value: " << region << " (Under investigation) ");
        break;
      }
      break;
      }

      if (towereta == 7){
        if(pos_neg < 0){ towerID_Modifier = 300000; }
        else if(pos_neg > 0){ towerID_Modifier = 400000; }
      }
      else if (towereta != 7){
        if(pos_neg < 0){ towerID_Modifier = 500000; }
        else if(pos_neg > 0){ towerID_Modifier = 600000; }
      }

      break;
    }
    case CaloSampling::EME2: {

      nphi = 32;

      switch (region) {
      case 0: { 

        if(abs(pos_neg) == 2){
          towereta = 7;
          towerphi = phi_index/2;
        }
        else if(abs(pos_neg) == 3){
          if (eta_index == 0){
            towereta = 12;
            towerphi = phi_index;
          }
          else if (eta_index == 1){
            towereta = 13;
            towerphi = phi_index/2;
            nphi = 16;
          }
          else if (eta_index == 2){
            towereta = 14;
            towerphi = phi_index/2;
            nphi = 16;
          }
        }
        break;
      }
      case 1: {
        if(abs(pos_neg) == 2){
          towerphi = phi_index/2;
          if (eta_index < 7) {towereta = 7;}
          else if (eta_index < 15) {towereta = 8;}
          else if (eta_index < 23) {towereta = 9;}
          else if (eta_index < 31) {towereta = 10;}
          else if (eta_index < 43) {towereta = 11;}
        }
        else if(abs(pos_neg) == 3){
          towerphi = phi_index/2;
          towereta = 14;
          nphi = 16;
        }
        break;
      }

      default: {
        ATH_MSG_DEBUG("CaloSampling::EME2 -> invalid 'region' value: " << region << " (Under investigation) ");
        break;
      }
        break;
      }

      if (towereta == 7){
        if(pos_neg < 0){ towerID_Modifier = 300000; }
        else if(pos_neg > 0){ towerID_Modifier = 400000; }
      }
      else if ((towereta == 13) || (towereta == 14)){
        if(pos_neg < 0){ towerID_Modifier = 700000; }
        else if(pos_neg > 0){ towerID_Modifier = 800000; }
      }
      else {//else if (towereta < 13)
        if(pos_neg < 0){ towerID_Modifier = 500000; }
        else if(pos_neg > 0){ towerID_Modifier = 600000; }
      }

      break;
    }
    case CaloSampling::EME3: {

      nphi = 32;
      switch (region) {
      case 0: {
        if(abs(pos_neg) == 2){
          towerphi = phi_index/2;
          if (eta_index == 0){
            towereta = 7;
          }
          else if(eta_index < 3){
            towereta = 8;
          }
          else if(eta_index < 5){
            towereta = 9;
          }
          else if(eta_index < 7){
            towereta = 10;
          }
          else if(eta_index < 10){
            towereta = 11;
          }

        }
        else if(abs(pos_neg) == 3){
          if (eta_index == 0){
            towereta = 12;
            towerphi = phi_index;
          }
          else if (eta_index == 1){
            towereta = 13;
            towerphi = phi_index/2;
            nphi = 16;
          }
          else if (eta_index == 2){
            towereta = 14;
            towerphi = phi_index/2;
            nphi = 16;
          }
        }
        break;
      }
      case 1: { // we only have the case: abs(pos_neg) = 3
        towerphi = phi_index/2;
        towereta = 14;
        nphi = 16;
        break;
      }
      default: {
        ATH_MSG_DEBUG("CaloSampling::EME3 -> invalid 'region' value: " << region << " (Under investigation) ");
        break;
      }
        break;
      }

      if (towereta == 7){
        if(pos_neg < 0){ towerID_Modifier = 300000; }
        else if(pos_neg > 0){ towerID_Modifier = 400000; }
      }
      else if (towereta < 13){
        if(pos_neg < 0){ towerID_Modifier = 500000; }
        else if(pos_neg > 0){ towerID_Modifier = 600000; }
      }
      else { //if towereta = 13, 14
        if(pos_neg < 0){ towerID_Modifier = 700000; }
        else if(pos_neg > 0){ towerID_Modifier = 800000; }
      }


      break;
    }
      // Hadronic End-cap (HEC)
    case CaloSampling::HEC0:{
      nphi = 32;
      switch(region){
      case 0: {
        if(abs(pos_neg) == 2){
          towerphi = phi_index/2;
          if (eta_index == 0){
            towereta = 7;
          }
          else if(eta_index < 3){
            towereta = 8;
          }
          else if(eta_index < 5){
            towereta = 9;
          }
          else if(eta_index < 7){
            towereta = 10;
          }
          else if(eta_index < 10){
            towereta = 11;
          }
        }
        break;

      }

      case 1: {
        if(abs(pos_neg) == 2){
          if (eta_index == 0){
            towereta = 12;
            towerphi = phi_index;
            nphi = 32;
          }
          else if(eta_index == 1){
            towereta = 13;
            towerphi = phi_index/2;
            nphi = 16;
          }
          else if(eta_index == 2){
            towereta = 14;
            towerphi = phi_index/2;
            nphi = 16;
          }
          else if(eta_index == 3){
            towereta = 15;
            towerphi = phi_index/2;
            nphi = 16;
          }
        }
        break;

      }

      default: {
        ATH_MSG_DEBUG("CaloSampling::HEC0 -> invalid 'region' value: " << region << " (Under investigation) ");
        break;
      }

      }

      // Tower connection
      if (towereta == 7){
        nphi = 32;
        if(pos_neg < 0){ towerID_Modifier = 300000; }
        else if(pos_neg > 0){ towerID_Modifier = 400000; }
      }
      else if (towereta < 13){
        nphi = 32;
        if(pos_neg < 0){ towerID_Modifier = 500000; }
        else if(pos_neg > 0){ towerID_Modifier = 600000; }
      }
      else if ((towereta == 13) || (towereta == 14)){
        nphi = 16;
        if(pos_neg < 0){ towerID_Modifier = 700000; }
        else if(pos_neg > 0){ towerID_Modifier = 800000; }
      }
      else if (towereta == 15){
        nphi = 16;
        if(pos_neg < 0){ towerID_Modifier = 900000; }
        else if(pos_neg > 0){ towerID_Modifier = 1000000; }
      }
      break;
    }
    case CaloSampling::TileBar0:
    case CaloSampling::TileBar1:
    case CaloSampling::TileBar2: {
      REPORT_MESSAGE_WITH_CONTEXT (MSG::DEBUG, "gSuperCellTowerMapper") << "Supercell is from Tile Barrel - it will be ignored.";
      validcell = false;
      ATH_MSG_DEBUG("\n==== gSuperCellTowerMapper ============ Supercell is from Tile Barrel - it will be ignored.");
      break;
    }
    case CaloSampling::TileGap1:
    case CaloSampling::TileGap2:
    case CaloSampling::TileGap3: {
      ATH_MSG_DEBUG("\n==== gSuperCellTowerMapper ============ Supercell is from Tile Gap (ITC and scintillator) - it will be ignored.");
      validcell = false;
      break;
    }
    case CaloSampling::TileExt0:
    case CaloSampling::TileExt1:
    case CaloSampling::TileExt2: {
      ATH_MSG_DEBUG("\n==== gSuperCellTowerMapper ============ Supercell is from Tile Extended Barrel - it will be ignored.");
      validcell = false;
      break;
    }
    case CaloSampling::FCAL0:{
      nphi = 16;
      towerphi = phi_index;
      if (eta_index == 0){
        towereta = 14;
      }
      else if(eta_index < 6){
        towereta = 15;
      }
      else if(eta_index < 8){
        towereta = 16;
      }
      else if(eta_index < 10){
        towereta = 17;
        }
      else if(eta_index < 12){
        towereta = 18;
      }

      if (towereta == 14){
        if(pos_neg < 0){ towerID_Modifier = 700000; }
        else if(pos_neg > 0){ towerID_Modifier = 800000; }
      }
      else{
        if(pos_neg < 0){ towerID_Modifier = 900000; }
        else if(pos_neg > 0){ towerID_Modifier = 1000000; }
      }

      break;
    }
    case CaloSampling::FCAL1:{
      nphi = 16;
      towerphi = phi_index;
      if (eta_index < 5){
        towereta = 15;
      }
      else if(eta_index < 6){
        towereta = 16;
      }
      else if(eta_index < 7){
        towereta = 17;
      }
      else if(eta_index < 8){
        towereta = 18;
      }

      if(pos_neg < 0){ towerID_Modifier = 900000; }
      else if(pos_neg > 0){ towerID_Modifier = 1000000; }

      break;
    }
    case CaloSampling::FCAL2:{
      nphi = 16;
      towerphi = phi_index;
      if (eta_index < 2){
        towereta = 15;
      }
      else if(eta_index < 3){
        towereta = 16;
      }
      else if(eta_index < 4){
        towereta = 17;
      }

      if(pos_neg < 0){ towerID_Modifier = 900000; }
      else if(pos_neg > 0){ towerID_Modifier = 1000000; }
      break;
    }


    case CaloSampling::MINIFCAL0:
    case CaloSampling::MINIFCAL1:
    case CaloSampling::MINIFCAL2:
    case CaloSampling::MINIFCAL3: {
      ATH_MSG_DEBUG("\n==== gSuperCellTowerMapper ============ Supercells is from MiniFCAL - it will be ignored.");
      validcell = false;
      break;
    }
    case CaloSampling::Unknown: {
      ATH_MSG_WARNING("\n==== gSuperCellTowerMapper ============ Supercell sampling is officially unknown - it will be ignored. (Needs investigation).  Please report this!");
      validcell = false;
      break;
    }
    default: {
      ATH_MSG_DEBUG("\n==== gSuperCellTowerMapper ============ Supercell has invalid CaloSampling value: " << sample << " (Needs investigation).  Please report this!");
      validcell = false;
      break;
    }
  }

  if(validcell){
    iGTower = FindTowerIDForSuperCell(towereta, towerphi, nphi) + towerID_Modifier;
    if(doPrint){
      PrintCellSpec(sample, region, eta_index, phi_index, pos_neg, towereta, towerphi, nphi, towerID_Modifier, iGTower, prov, ID);
    }
    ConnectSuperCellToTower(my_gTowerContainerRaw, iGTower, ID, et);
  }

  // END ITERATING OVER SUPER CELLS+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ +++++++++++++++++++++++++++++++++++++++++++++

  return 1;

}

int gSuperCellTowerMapper::FindTowerIDForSuperCell(int towereta, int towerphi, int nphi) const
{

  return (towerphi + (nphi * towereta));

}

  void gSuperCellTowerMapper::PrintCellSpec(const CaloSampling::CaloSample sample, const int region,
                                            const int eta_index, const int phi_index, const int pos_neg,
                                            int towereta, int towerphi, int nphi, int towerID_Modifier,
                                            int iGTower, int prov, Identifier ID) const
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
  case CaloSampling::TileBar0: { sampleName = "TileBar0"; break; }
  case CaloSampling::TileBar1: { sampleName = "TileBar1"; break; }
  case CaloSampling::TileBar2: { sampleName = "TileBar2"; break; }
  case CaloSampling::TileGap1: { sampleName = "TileGap1"; break; }
  case CaloSampling::TileGap2: { sampleName = "TileGap2"; break; }
  case CaloSampling::TileGap3: { sampleName = "TileGap3"; break; }
  case CaloSampling::TileExt0: { sampleName = "TileExt0"; break; }
  case CaloSampling::TileExt1: { sampleName = "TileExt1"; break; }
  case CaloSampling::TileExt2: { sampleName = "TileExt2"; break; }
  case CaloSampling::FCAL0: { sampleName = "FCAL0"; break; }
  case CaloSampling::FCAL1: { sampleName = "FCAL1"; break; }
  case CaloSampling::FCAL2: { sampleName = "FCAL2"; break; }

  default: {
    ATH_MSG_DEBUG("\n==== gSuperCellTowerMapper ============ Supercell has invalid CaloSampling value: " << sample << " (Needs investigation).  Please report this!");
    break;
  }
  }

  ATH_MSG_DEBUG("ASSIGNED CELL:::  CASE: " << sampleName
		        << "\tSample:           " << sample
		        << "\tRegion:           " << region
		        << "\tEta_Index:        " << eta_index
		        << "\tPhi_Index:        " << phi_index
		        << "\tPosNeg:           " << pos_neg
            << "\tgTower Eta:       " << towereta
            << "\tgTower Phi:       " << towerphi
            << "\tgTower nphi:      " << nphi
            << "\tgTower modifier:  " << towerID_Modifier
		        << "\tiGTower:          " << iGTower
		        << "\tProvenance:       " << prov
		        << "\tID:               " << ID
            << " ");

  return;
}


} // end of LVL1 namespace
