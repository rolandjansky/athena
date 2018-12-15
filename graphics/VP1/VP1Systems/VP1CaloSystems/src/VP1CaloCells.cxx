/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1CaloSystems/VP1CaloCells.h"
#include "VP1HEPVis/nodes/SoGenericBox.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Utils/VP1LinAlgUtils.h"

#include <Inventor/nodes/SoSeparator.h>

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TileEvent/TileCell.h"
#include "TileIdentifier/TileHWID.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/TileInfo.h"
#include "CaloConditions/ICaloBadChanTool.h"

#include <stdexcept>
#include <sstream>

// MBTS
#include "CaloIdentifier/TileTBID.h"
#include <Inventor/nodes/SoTransform.h>

// Epsilon for changing MBTS cell dimensions with respect to the volume dimensions
#define MBTS_EPS 10

// ************ Base Class ***************
VP1CaloCell::VP1CaloCell(const CaloCell* caloCell):
  m_caloCell(caloCell)
{
  // Cannot draw object if it has no Calo DDE
  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();
  if (!ddElement)
    throw std::runtime_error("VP1CaloCell: Calo Cell has no Calo DDE");
}

VP1CaloCell::~VP1CaloCell()
{
}

const CaloCell* VP1CaloCell::getCaloCell()
{
  return m_caloCell;
}

Identifier VP1CaloCell::getID()
{
  return m_caloCell->ID();
}

double VP1CaloCell::energyToTransverse(const double& e) const
{
  return m_caloCell->sinTh()*e;
}

bool VP1CaloCell::cutPassed(const VP1CC_GlobalCuts& globalCuts)
{
  // only do this if it's something significant
  if (globalCuts.clipRadius<10e8){
    if (!isInsideClipVolume(globalCuts)) return false;
    // double radius = sqrt(m_caloCell->x()*m_caloCell->x() + m_caloCell->y()*m_caloCell->y() + m_caloCell->z()*m_caloCell->z());
   
  }
  
  // Check side and Eta cut
  bool passed = (m_caloCell->eta()>=0 && globalCuts.sideA) 
    || (m_caloCell->eta()<0 && globalCuts.sideC);
  passed = passed && globalCuts.allowedEta.contains(m_caloCell->eta());
  if(!passed) return false;
  // Check Phi Cut
  passed = false;
  foreach(VP1Interval i, globalCuts.allowedPhi){
    if(i.contains(m_caloCell->phi()) 
       ||i.contains(m_caloCell->phi()+2*M_PI)
       ||i.contains(m_caloCell->phi()-2*M_PI)) {
      passed = true;
      break;
    }
  }
  return passed;
}

bool VP1CaloCell::isInsideClipVolume(const VP1CC_GlobalCuts& globalCuts) {
  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();
  double radius = ddElement->r();
  double z = ddElement->z();
  double radialDistance = sqrt(radius*radius + z*z);
  if (radialDistance < globalCuts.clipRadius ) return true;
  return false;
}

void VP1CaloCell::updateScene(VP1CC_SoNode2CCMap* node2cc,
			      bool useEt,
			      const QPair<bool,double>& scale,
			      bool outline,
			      const VP1CC_GlobalCuts& globalCuts)
{
  if(cutPassed(globalCuts))
    build3DObjects(node2cc,useEt,scale,outline, globalCuts);
  else
    remove3DObjects(node2cc);
}

// ************  LAr  ***************
VP1CC_LAr::VP1CC_LAr(const CaloCell* caloCell):
  VP1CaloCell(caloCell),
  m_hit(0),
  m_helper(0)
{
/** caloCell was already dereferenced at this point by VP1CaloCell, so cannot be null
  if(!caloCell)
    throw std::runtime_error("VP1CC_LAr: 0 pointer to CaloCell");
**/
}

VP1CC_LAr::~VP1CC_LAr()
{
}

void VP1CC_LAr::remove3DObjects(VP1CC_SoNode2CCMap* node2cc)
{
  if(m_hit) {
    node2cc->erase(m_hit);
    m_helper->removeNode(m_hit);
    m_hit = 0;
  }
}

// ************  LArEMB  ***************
VP1CC_LArEMB::VP1CC_LArEMB(const CaloCell* caloCell,
				   const VP1CC_SeparatorMap* separators):
  VP1CC_LAr(caloCell)
{
  // define Separator type depending on Cell energy
  if(!separators)
    throw std::runtime_error("VP1CC_LArEMB: 0 pointer to VP1CC Separator Map");

  VP1CC_SeparatorTypes mySeparatorType;
  if(caloCell->energy()<0)
    mySeparatorType = VP1CC_SepLArEMBNeg;
  else
    mySeparatorType = VP1CC_SepLArEMBPos;

  // get SoSeparator from the map
  VP1CC_SeparatorMap::const_iterator it = separators->find(mySeparatorType);
  if(it ==separators->end())
    throw std::runtime_error("VP1CC_LArEMB: Missing separator helper in the map");

  m_helper = it->second;

  if(!m_helper)
    throw std::runtime_error("VP1CC_LArEMB: 0 pointer to separator helper");
}

VP1CC_LArEMB::~VP1CC_LArEMB()
{
}

void VP1CC_LArEMB::build3DObjects(VP1CC_SoNode2CCMap* node2cc,
				  bool useEt,
				  const QPair<bool,double>& scale,
				  bool outline,
          const VP1CC_GlobalCuts& globalCuts)
{
  bool createNewHit = false;
  if(!m_hit){
    m_hit = new SoGenericBox();
    createNewHit = true;
  }

  // Build new Hit object
  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();

  // auxiliary variables
  double eta = ddElement->eta();
  double deta = ddElement->deta()*0.5;
  double phi = ddElement->phi();
  double dphi = ddElement->dphi()*0.5;
  double radius = ddElement->r();
  double energy = (useEt ? energyToTransverse(m_caloCell->energy()) : m_caloCell->energy());

  double z = std::fabs(ddElement->z());
  double radialDistance = sqrt(radius*radius + z*z);

  double depth = cellDepth(scale,energy);
  depth = std::min(depth, std::fabs(globalCuts.clipRadius - radialDistance ) ) ;

  // std::cout<<"z "<<ddElement->z()<<"\t radialDistance "<<radialDistance<<"\t clipRadius "<<globalCuts.clipRadius<<"\t depth "<<depth<<std::endl;

  m_hit->setParametersForBarrelEtaPhiCell( eta-deta,eta+deta,phi-dphi,phi+dphi,
					   depth, radius, 0.9, 0.9 );
  m_hit->drawEdgeLines = outline;

  if(createNewHit) {
    m_helper->addNode(m_hit);
    (*node2cc)[m_hit] = this;
  }
}

std::vector<std::string> VP1CC_LArEMB::ToString(const CaloCell_ID*, const std::string& extrainfos)
{
  std::vector<std::string> result;
  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();
  std::ostringstream msg, msg1, msg2, msg3;

  std::string stars("***");
  result.push_back(stars);

  msg << "LAr EMB. ID =  " << m_caloCell->ID().getString() << " " << extrainfos;

  msg1 << " Eta = " << ddElement->eta()
       << " Phi = " << ddElement->phi();

  msg2 << "Energy = " << m_caloCell->energy() << " (Mev)"
       << " Gain " << m_caloCell->gain(); 

  msg3  << "Time = " << m_caloCell->time()
	<< " Quality = " << m_caloCell->quality()
	<< " Provenance = " << m_caloCell->provenance(); 

  result.push_back(msg.str());
  result.push_back(msg1.str());
  result.push_back(msg2.str());
  result.push_back(msg3.str());

  result.push_back(stars);
  return result;
}

// ************  LArEMECHEC  ***************
VP1CC_LArEMECHEC::VP1CC_LArEMECHEC(const CaloCell* caloCell,
					   const CaloCell_ID*  calo_id,
					   const VP1CC_SeparatorMap* separators):
  VP1CC_LAr(caloCell)
{
  if(!separators)
    throw std::runtime_error("VP1CC_LArEMECHEC: 0 pointer to VP1CC Separator Map");

  bool isEMEC = false;
  if(calo_id->is_em_endcap(caloCell->ID()))
    isEMEC = true;
  else if(calo_id->is_hec(caloCell->ID()))
    isEMEC = false;
  else
    throw std::runtime_error("VP1CC_LArEMECHEC: Calo Cell is neither EMEC nor HEC!");

  // define Separator type depending on Cell energy
  VP1CC_SeparatorTypes mySeparatorType;
  if(caloCell->energy()<0) {
    if(isEMEC)
      mySeparatorType = VP1CC_SepLArEMECNeg;
    else
      mySeparatorType  = VP1CC_SepLArHECNeg;
  } else {
    if(isEMEC)
      mySeparatorType = VP1CC_SepLArEMECPos;
    else
      mySeparatorType = VP1CC_SepLArHECPos;
  }

  // get SoSeparator from the map
  VP1CC_SeparatorMap::const_iterator it = separators->find(mySeparatorType);
  if(it ==separators->end())
    throw std::runtime_error("VP1CC_LArEMECHEC: Missing separator in the map");

  m_helper = it->second;

  if(!m_helper)
    throw std::runtime_error("VP1CC_LArEMECHEC: 0 pointer to the separator helper");
}

VP1CC_LArEMECHEC::~VP1CC_LArEMECHEC()
{
}

std::vector<std::string> VP1CC_LArEMECHEC::ToString(const CaloCell_ID*  calo_id, const std::string& extrainfos)
{
  std::vector<std::string> result;
  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();

  std::ostringstream msg, msg1, msg2, msg3;

  std::string stars("***");
  result.push_back(stars);

  if(calo_id->is_em_endcap(m_caloCell->ID()))
    msg << "LAr EMEC. ID =  " << m_caloCell->ID().getString();
  else
    msg << "LAr HEC. ID =  " << m_caloCell->ID().getString();
  msg << " " << extrainfos;

  msg1 << " Eta = " << ddElement->eta()
       << " Phi = " << ddElement->phi();

  msg2 << "Energy = " << m_caloCell->energy() << " (Mev)"
       << " Gain " << m_caloCell->gain(); 

  msg3  << "Time = " << m_caloCell->time()
	<< " Quality = " << m_caloCell->quality() 
	<< " Provenance = " << m_caloCell->provenance();

  result.push_back(msg.str());
  result.push_back(msg1.str());
  result.push_back(msg2.str());
  result.push_back(msg3.str());

  result.push_back(stars);
  return result;
}

void VP1CC_LArEMECHEC::build3DObjects(VP1CC_SoNode2CCMap* node2cc,
				      bool useEt,
				      const QPair<bool,double>& scale,
				      bool outline,
          const VP1CC_GlobalCuts& globalCuts)
{
  bool createNewHit = false;
  if(!m_hit){
    m_hit = new SoGenericBox();
    createNewHit = true;
  }

  // Build new Hit object
  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();

  // auxiliary variables
  double eta = ddElement->eta();
  double deta = ddElement->deta()*0.5;
  double phi = ddElement->phi();
  double dphi = ddElement->dphi()*0.5;
  double z  = ddElement->z();
  double energy = (useEt ? energyToTransverse(m_caloCell->energy()) : m_caloCell->energy());

  double depth = cellDepth(scale,energy);
  depth = std::min(depth, std::fabs(globalCuts.clipRadius - z));

  m_hit->setParametersForEndCapEtaPhiCell( eta-deta,eta+deta,phi-dphi,phi+dphi,
					   depth, z, 0.9, 0.9 );
  m_hit->drawEdgeLines = outline;

  if(createNewHit) {
    m_helper->addNode(m_hit);
    (*node2cc)[m_hit] = this;
  }
}

// ************  LArFCAL  ***************
VP1CC_LArFCAL::VP1CC_LArFCAL(const CaloCell* caloCell,
				     const VP1CC_SeparatorMap* separators):
  VP1CC_LAr(caloCell)
{
  if(!separators)
    throw std::runtime_error("VP1CC_LArFCAL: 0 pointer to VP1CC Separator Map");

  // define Separator type depending on Cell energy
  VP1CC_SeparatorTypes mySeparatorType;

  if(m_caloCell->energy()<0)
    mySeparatorType = VP1CC_SepLArFCALNeg;
  else
    mySeparatorType = VP1CC_SepLArFCALPos;

  VP1CC_SeparatorMap::const_iterator it = separators->find(mySeparatorType);
  if(it ==separators->end())
    throw std::runtime_error("VP1CC_LArFCAL: Missing separator in the map");

  m_helper = it->second;

  if(!m_helper)
    throw std::runtime_error("VP1CC_LArFCAL: Wrong 0 pointer to the separator helper");
}

VP1CC_LArFCAL::~VP1CC_LArFCAL()
{
}

std::vector<std::string> VP1CC_LArFCAL::ToString(const CaloCell_ID*, const std::string& extrainfos )
{
  std::vector<std::string> result;
  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();
  std::ostringstream msg, msg1, msg2, msg3;

  std::string stars("***");
  result.push_back(stars);

  msg << "LAr FCAL. ID =  " << m_caloCell->ID().getString() << " " << extrainfos;

  msg1 << " Eta = " << ddElement->eta()
       << " Phi = " << ddElement->phi();

  msg2 << " Energy = " << m_caloCell->energy() << " (Mev)"
       << " Gain " << m_caloCell->gain(); 

  msg3  << "Time = " << m_caloCell->time()
	<< " Quality = " << m_caloCell->quality()
	<< " Provenance = " << m_caloCell->provenance(); 

  result.push_back(msg.str());
  result.push_back(msg1.str());
  result.push_back(msg2.str());
  result.push_back(msg3.str());

  result.push_back(stars);
  return result;
}

void VP1CC_LArFCAL::build3DObjects( VP1CC_SoNode2CCMap* node2cc,
				    bool useEt,
				    const QPair<bool,double>& scale,
				    bool outline,
          const VP1CC_GlobalCuts& globalCuts)
{
  bool createNewHit = false;
  if(!m_hit) {
    m_hit = new SoGenericBox();
    createNewHit = true;
  }

  // Build new Hit object
  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();

  // auxiliary variables
  double x  = ddElement->x();
  double dx = ddElement->dx()*0.5;
  double y  = ddElement->y();
  double dy = ddElement->dy()*0.5;
  double z  = ddElement->z();
  double dz = ddElement->dz()*0.5;
  double energy = (useEt ? energyToTransverse(m_caloCell->energy()) : m_caloCell->energy());

  double halfdepth(0.5*cellDepth(scale,energy));
  halfdepth = std::min(halfdepth, std::fabs(0.5 * (globalCuts.clipRadius - std::fabs(z))));
  // std::cout<<"cellDepth: "<<0.5*cellDepth(scale,energy)<<"\t halfdepth: "<<halfdepth<<"\t z "<<z<<"\t dz "<<dz<<std::endl;
  if(z>0){
    m_hit->setParametersForBox( dx*0.9, dy*0.9, halfdepth,x,y,z-dz+halfdepth);    
  } else {
    m_hit->setParametersForBox( dx*0.9, dy*0.9, halfdepth,x,y,z+dz-halfdepth);
  }
  m_hit->drawEdgeLines = outline;

  if(createNewHit) {
    m_helper->addNode(m_hit);
    (*node2cc)[m_hit] = this;
  }
}

VP1CC_Tile::VP1CC_Tile(const CaloCell* caloCell,
			       const TileID* tile_id):
  VP1CaloCell(caloCell),
  m_tileID(tile_id)
{
}

VP1CC_Tile::~VP1CC_Tile()
{
}

// ************  Tile  ***************
VP1CC_TileBarEc::VP1CC_TileBarEc(const CaloCell* caloCell,
					 const TileID* tile_id,
					 const VP1CC_SeparatorMap* separators):
  VP1CC_Tile(caloCell,tile_id),
  m_hitUp(0),
  m_hitDown(0)
{
/**
//caloCell was already dereferenced in the initialiser list, so this is redundant: you've already crashed.
  if(!caloCell)
    throw std::runtime_error("VP1CC_TileBarEc: 0 pointer to CaloCell");
**/
  if(!separators)
    throw std::runtime_error("VP1CC_TileBarEc: 0 pointer to VP1CC Separator Map");

  // The CaloCell has to be of type TileCell
  const TileCell* tile_cell = dynamic_cast<const TileCell*>(caloCell);
  if(tile_cell==0)
    throw std::runtime_error("VP1CC_TileBarEc: CaloCell is not TileCell");

  // define Separator types (Up/Down) depending on Cell energy
  VP1CC_SeparatorTypes mySeparatorTypeUp;
  VP1CC_SeparatorTypes mySeparatorTypeDown;

  double energy1 = tile_cell->ene1();
  double energy2 = tile_cell->ene2();

  if(caloCell->energy()<0)
  {
    if(energy1>=0)
      mySeparatorTypeDown = VP1CC_SepTileNegativePos;
    else
      mySeparatorTypeDown = VP1CC_SepTileNegativeDown;

    if(energy2>=0)
      mySeparatorTypeUp = VP1CC_SepTileNegativePos;
    else
      mySeparatorTypeUp = VP1CC_SepTileNegativeUp;
  }
  else
  {
    if(energy1<=0)
      mySeparatorTypeDown = VP1CC_SepTilePositiveNeg;
    else
      mySeparatorTypeDown = VP1CC_SepTilePositiveDown;

    if(energy2<=0)
      mySeparatorTypeUp = VP1CC_SepTilePositiveNeg;
    else
      mySeparatorTypeUp = VP1CC_SepTilePositiveUp;
  }

  VP1CC_SeparatorMap::const_iterator it = separators->find(mySeparatorTypeDown);
  if(it ==separators->end())
    throw std::runtime_error("VP1CC_TileBarEc: Missing separator helper in the map");

  m_helperDown = it->second;

  it = separators->find(mySeparatorTypeUp);
  if(it ==separators->end())
    throw std::runtime_error("VP1CC_TileBarEc: Missing separator helper in the map");

  m_helperUp = it->second;

  if(!m_helperDown || !m_helperUp)
    throw std::runtime_error("VP1CC_TileBarEc: 0 pointer to one of separator helpers" );
}

VP1CC_TileBarEc::~VP1CC_TileBarEc()
{
}

std::vector<std::string> VP1CC_TileBarEc::ToString(const CaloCell_ID*, const std::string& extrainfos)
{
  std::vector<std::string> result;
  const TileCell* tile_cell = dynamic_cast<const TileCell*>(m_caloCell);
  if (not tile_cell) return result;
  Identifier cellid = m_caloCell->ID();

  double total_energy = m_caloCell->energy();
  double total_time = m_caloCell->time();
  double pmt_energy1 = tile_cell->ene1();
  double pmt_time1 = tile_cell->time1();
  double pmt_energy2 = tile_cell->ene2();
  double pmt_time2 = tile_cell->time2();

  std::ostringstream msg, msg1, msg2, msg3, msg4;
  msg  << "Cell: " << id2name(cellid) << " " << extrainfos;
  msg1 << "Energy = " << total_energy << " (Mev)";
  msg2 << "Time = " << total_time;
  msg3 << " PMT1 Energy = " << pmt_energy1 << " (Mev)"
       << " PMT1 Time = " << pmt_time1;
  msg4 << " PMT2 Energy = " << pmt_energy2 << " (Mev)"
       << " PMT2 Time = " << pmt_time2;

  std::string stars("***");
  result.push_back(stars);

  result.push_back(msg.str());
  result.push_back(msg1.str());
  result.push_back(msg2.str());
  result.push_back(msg3.str());
  result.push_back(msg4.str());

  result.push_back(stars);
  return result;
}

int VP1CC_TileBarEc::GetFragChannel(const TileHWID* tile_hw_id,
					bool up,
					int& frag,
					int& channel)
{
  HWIdentifier hwId;
  IdContext context = tile_hw_id->channel_context();

  int result = 0;

  if(up)
    result = tile_hw_id->get_id(m_caloCell->caloDDE()->onl2(),hwId,&context);
  else
    result = tile_hw_id->get_id(m_caloCell->caloDDE()->onl1(),hwId,&context);

  if(result!=0)
    return result;

  frag = tile_hw_id->frag(hwId);
  channel = tile_hw_id->channel(hwId);

  return 0;
}

void VP1CC_TileBarEc::build3DObjects(VP1CC_SoNode2CCMap* node2cc,
				     bool useEt,
				     const QPair<bool,double>& scale,
				     bool outline,
          const VP1CC_GlobalCuts& globalCuts)
{
  bool createNewHit = false;
  if(!m_hitUp){
    m_hitUp = new SoGenericBox();
    m_hitDown = new SoGenericBox();
    createNewHit = true;
  }

  const TileCell* tile_cell = dynamic_cast<const TileCell*>(m_caloCell);
  if (not tile_cell) return;
  double energy1 = (useEt ? energyToTransverse(tile_cell->ene1()) : tile_cell->ene1());
  double energy2 = (useEt ? energyToTransverse(tile_cell->ene2()) : tile_cell->ene2());

  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();

  double eta = ddElement->eta();
  double deta = ddElement->deta()*0.5;
  double phi = ddElement->phi();
  double dphi = ddElement->dphi()*0.5;
  double radius = ddElement->r();
  double dradius = ddElement->dr()*0.5;

  double z = ddElement->z();
  double minR = radius - dradius;
  double radialDistance = sqrt(minR*minR + z*z);


  double depth1 = cellDepth(scale,energy1);
  depth1 = std::min(depth1, std::fabs(globalCuts.clipRadius - radialDistance));
  double depth2 = cellDepth(scale,energy2);
  depth2 = std::min(depth2, std::fabs(globalCuts.clipRadius - radialDistance));
  // std::cout<<"VP1CC_TileBarEc clipRadius: "<<globalCuts.clipRadius<<"\t radius "<<radius<<"\t depth1 "
 //           <<cellDepth(scale,energy1)<<"\t depth2 "<<cellDepth(scale,energy2)<<"\t cd1 "<<depth1<<"\t cd1 "<<depth1<<std::endl;
 
  //double etaMin, double etaMax,
  // double phiMin, double phiMax,
  // double cellDepth, double cellDistance,
  // double etasqueezefact, double phisqueezefact 
  m_hitUp->setParametersForBarrelEtaPhiCell(eta,eta+deta,phi-dphi,phi+dphi,
					    depth1,radius-dradius,0.9,0.9);
  m_hitUp->drawEdgeLines = outline;
  m_hitDown->setParametersForBarrelEtaPhiCell(eta-deta,eta,phi-dphi,phi+dphi,
					      depth2,radius-dradius,0.9,0.9);
  m_hitDown->drawEdgeLines = outline;

  if(createNewHit) {
    (*node2cc)[m_hitUp] = this;
    (*node2cc)[m_hitDown] = this;
    m_helperUp->addNode(m_hitUp);
    m_helperDown->addNode(m_hitDown);
  }
}

bool VP1CC_TileBarEc::isInsideClipVolume(const VP1CC_GlobalCuts& globalCuts) {
  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();
  double z = ddElement->z();
  double radius = ddElement->r();
  double dradius = ddElement->dr()*0.5;
  double minR = radius - dradius;
  
  double radialDistance = sqrt(minR*minR + z*z);
  
  if ( radialDistance > globalCuts.clipRadius ) return false;
  return true;
}


void VP1CC_TileBarEc::remove3DObjects(VP1CC_SoNode2CCMap* node2cc)
{
  if(m_hitUp) {
    // Remove entries from node2cc map
    node2cc->erase(m_hitUp);
    node2cc->erase(m_hitDown);

    // Drop the hit from the scene and delete hit object
    m_helperUp->removeNode(m_hitUp);
    m_helperDown->removeNode(m_hitDown);

    m_hitUp = 0;
    m_hitDown = 0;
  }
}

std::string VP1CC_TileBarEc::id2name(Identifier& id)
{
  std::string name("");

  // Barrel/EC
  if(m_tileID->is_tile_barrel(id))
    name += std::string("LB");
  else
    name += std::string("EB");

  // Pos/Neg
  if(m_tileID->side(id)==1)
    name += std::string("A");
  else
    name += std::string("C");

  // Module number
  std::ostringstream modnum;
  modnum << m_tileID->module(id) + 1;
  name += (modnum.str() + std::string(" "));

  // Sample
  int sample = m_tileID->sample(id);
  switch(sample)
  {
  case 0:
    {
      name += std::string("A");
      break;
    }
  case 1:
    {
      if(m_tileID->is_tile_barrel(id))
      {
	name += std::string("B");
	if(m_tileID->tower(id)<9)
	  name += std::string("C");
      }
      else
      {
	if(m_tileID->section(id)==2)
	  name += std::string("B");
	else
	  name += std::string("C");
      }
      break;
    }
  case 2:
    {
      name += std::string("D");
      break;
    }
  default:
    break;
  }

  // Tower
  std::ostringstream townum;
  if(m_tileID->sample(id)==2)
    townum << m_tileID->tower(id)/2;
  else
    townum << m_tileID->tower(id)+1;

  name += townum.str();
  return name;
}

// ************  TileCrack  ***************
VP1CC_TileCrack::VP1CC_TileCrack(const CaloCell* caloCell,const TileID* tile_id,
  const VP1CC_SeparatorMap* separators):VP1CC_Tile(caloCell,tile_id), m_hit(0){
  /**
  VP1CC_Tile has already dereferenced caloCell at this point, so it can't be zero anyway
  if(!caloCell)
    throw std::runtime_error("VP1CC_TileCrack: 0 pointer to CaloCell");
  **/
  if(!separators)
    throw std::runtime_error("VP1CC_TileCrack: 0 pointer to VP1CC Separator Map");

  // define Separator type depending on Cell energy
  VP1CC_SeparatorTypes mySeparatorType;

  if(caloCell->energy()<0)
    mySeparatorType = VP1CC_SepTileNegativeDown;
  else
    mySeparatorType = VP1CC_SepTilePositiveDown;

  VP1CC_SeparatorMap::const_iterator it = separators->find(mySeparatorType);
  if(it ==separators->end())
    throw std::runtime_error("VP1CC_TileCrack: Missing separator in the map");

  m_helper = it->second;

  if(!m_helper)
    throw std::runtime_error("VP1CC_TileCrack: 0 pointer to the separator");
}

VP1CC_TileCrack::~VP1CC_TileCrack()
{
}

std::vector<std::string> VP1CC_TileCrack::ToString(const CaloCell_ID*  calo_id, const std::string& extrainfos)
{
  std::vector<std::string> result;
  std::ostringstream msg, msg1;

  std::string stars("***");
  result.push_back(stars);
  result.push_back(std::string("TileCrack Calo Cell"));

  // Create cell name (string)
  std::string cellName("EB");
  if(calo_id->side(m_caloCell->ID()) > 0)
    cellName += std::string("A");
  else
    cellName += std::string("C");

  // Module number
  std::ostringstream modnum;
  modnum << calo_id->module(m_caloCell->ID()) + 1;
  cellName += (modnum.str() + std::string(" E"));

  switch(calo_id->tower(m_caloCell->ID()))
  {
  case 10:
    {
      cellName += std::string("1");
      break;
    }
  case 11:
    {
      cellName += std::string("2");
      break;
    }
  case 13:
    {
      cellName += std::string("3");
      break;
    }
  case 15:
    {
      cellName += std::string("4");
      break;
    }
  default:
    break;
  }

  msg  << "Cell: " << cellName << " " << extrainfos;

  msg1 << " Energy = " << m_caloCell->energy() << " (Mev)"
       << " Time = " << m_caloCell->time();
  result.push_back(msg.str());
  result.push_back(msg1.str());

  result.push_back(stars);
  return result;
}

int VP1CC_TileCrack::GetFragChannel( const TileHWID* tile_hw_id,
				     bool /*up*/,//TK: Not used?
				     int& frag,
				     int& channel )
{
  HWIdentifier hwId;
  IdContext context = tile_hw_id->channel_context();

  int result = tile_hw_id->get_id(m_caloCell->caloDDE()->onl1(),hwId,&context);

  if(result!=0)
    return result;

  frag = tile_hw_id->frag(hwId);
  channel = tile_hw_id->channel(hwId);

  return 0;
}

void VP1CC_TileCrack::build3DObjects(VP1CC_SoNode2CCMap* node2cc,
				     bool useEt,
				     const QPair<bool,double>& scale,
				     bool outline,
             const VP1CC_GlobalCuts& globalCuts)
{
  bool createNewHit = false;
  if(!m_hit) {
    m_hit = new SoGenericBox();
    createNewHit = true;
  }

  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();
  double eta = ddElement->eta();
  double deta = ddElement->deta()*0.5;
  double phi = ddElement->phi();
  double dphi = ddElement->dphi()*0.5;
  double z = ddElement->z();
  double dz = ddElement->dz()*0.5;
  double energy = (useEt ? energyToTransverse(m_caloCell->energy()) : m_caloCell->energy());

  double radius = ddElement->r();
  double minZ = std::fabs(z)-dz;
  double radialDistance = sqrt(radius*radius + minZ*minZ);

  double depth = cellDepth(scale,energy);
  depth = std::min(depth, std::fabs(globalCuts.clipRadius - radialDistance));
  // std::cout<<"z "<<z<<"\t dz "<<dz<<"\t r "<<ddElement->r()<<"\t clipRadius"<<globalCuts.clipRadius<<"\t depth "<<depth<<"\t cellDepth "<<cellDepth(scale,energy)<<std::endl;
  
  // std::cout<<"VP1CC_TileCrack clipRadius: "<<globalCuts.clipRadius<<"\t radialDistance "<<radialDistance<<"\t depth "
  //          <<cellDepth(scale,energy)<<"\t cd1 "<<depth<<std::endl;
  
      
  // double etaMin, double etaMax, double phiMin, double phiMax,
  // double cellDepth, double cellDistance, double etasqueezefact, double phisqueezefact 
  m_hit->setParametersForEndCapEtaPhiCell( eta-deta,eta+deta,phi-dphi,phi+dphi,
					   depth,z-dz,0.9,0.9);
  m_hit->drawEdgeLines = outline;

  if(createNewHit) {
    (*node2cc)[m_hit] = this;
    m_helper->addNode(m_hit);
  }
}

bool VP1CC_TileCrack::isInsideClipVolume(const VP1CC_GlobalCuts& globalCuts) {
  const CaloDetDescrElement* ddElement = m_caloCell->caloDDE();
  double z = ddElement->z();
  double dz = ddElement->dz()*0.5;
  double radius = ddElement->r();
  double minZ = std::fabs(z)-dz;
  double radialDistance = sqrt(radius*radius + minZ*minZ);
  
  if ( radialDistance > globalCuts.clipRadius ) return false;
  return true;
}


void VP1CC_TileCrack::remove3DObjects(VP1CC_SoNode2CCMap* node2cc)
{
  if(m_hit) {
    node2cc->erase(m_hit);
    m_helper->removeNode(m_hit);
    m_hit = 0;
  }
}

// ---------------------------------------------------
//
//             *** *** MBTS *** ***
//
// ---------------------------------------------------

VP1Mbts::VP1Mbts(const TileCell*  cell,
                const TileTBID*  idhelper,                                              
                SoSeparator*     separator,                                             
                bool             run2Geo):                                              
  m_cell(cell),
  m_idhelper(idhelper),
  m_separator(separator),                                                               
  m_run2Geo(run2Geo)                                                                    
{
}

VP1Mbts::~VP1Mbts()
{
}

bool VP1Mbts::UpdateScene(VP1CC_MbtsScinInfoMap* drawinfo,
			  VP1CC_SoNode2MbtsMap* node2mbts,
			  double energy,
			  bool outline, 
        double clipRadius)
{
  // Draw object only when cell energy is above threshold
  if(m_cell->energy() < energy)
    return false;
  
  // FIXME!
  // std::cout<<"clipRadius "<<clipRadius<<std::endl;
  if (clipRadius < 350 )
    return false;

  // Decode cell identifier in order to find draw info - shape and transformation
  Identifier id = m_cell->ID();

  int type = m_idhelper->type(id);       // -1 neg, 1 pos
  int module = m_idhelper->module(id);   // 0-7 scintillator copy number
  int channel = m_idhelper->channel(id); // 0 scin1, 1 scin2

  if(drawinfo->find(channel) ==drawinfo->end())
    throw std::runtime_error("Unable to find scintillator global info");

  VP1CC_MbtsScinInfo* scinInfo = (*drawinfo)[channel];

  // Get the transform
  SoTransform* scinXF = 0;
  if(type == -1) {
    if(scinInfo->cTransforms.find(module)==scinInfo->cTransforms.end())
      throw std::runtime_error("Unable to find global transform for the scintillator");
    scinXF = VP1LinAlgUtils::toSoTransform(scinInfo->cTransforms[module]);
  }
  else if( type == 1) {
    if(scinInfo->aTransforms.find(module)==scinInfo->aTransforms.end())
      throw std::runtime_error("Unable to find global transform for the scintillator");
    scinXF = VP1LinAlgUtils::toSoTransform(scinInfo->aTransforms[module]);
  } else
    return false;

  // 3D object - Trd
  SoGenericBox* scinTrd = new SoGenericBox();
  scinTrd->setParametersForTrd(scinInfo->dx1 + MBTS_EPS,
                               scinInfo->dx2 + MBTS_EPS,
                               scinInfo->dy1 - MBTS_EPS,
                               scinInfo->dy2 - MBTS_EPS,
                               scinInfo->dz  - MBTS_EPS);
  scinTrd->drawEdgeLines = outline;

  // Now add the object to the scene
  SoSeparator* scinSep = new SoSeparator;
  scinSep->addChild(scinXF);
  scinSep->addChild(scinTrd);
  m_separator->addChild(scinSep);

  // Keep the pointer to the 3D object in the map
  (*node2mbts)[scinTrd] = this;

  // ________________________________________________________________________________________
  // RUN2: if channel=1 draw two scintillators instead of one                               
  if(m_run2Geo && channel==1) {                                                             
    SoTransform* scinXF1 = 0;                                                               
    if(type == -1) {                                                                        
      if(scinInfo->cTransforms.find(module+1)==scinInfo->cTransforms.end())               
       throw std::runtime_error("Unable to find global transform for the scintillator");     
      scinXF1 = VP1LinAlgUtils::toSoTransform(scinInfo->cTransforms[module+1]);           
    }                                                                                        
    else if( type == 1) {                                                                   
      if(scinInfo->aTransforms.find(module+1)==scinInfo->aTransforms.end())               
       throw std::runtime_error("Unable to find global transform for the scintillator");     
      scinXF1 = VP1LinAlgUtils::toSoTransform(scinInfo->aTransforms[module+1]);           
    }                                                                                        
    SoGenericBox* scinTrd1 = new SoGenericBox();                                            
    scinTrd1->setParametersForTrd(scinInfo->dx1 + MBTS_EPS,                                
                                  scinInfo->dx2 + MBTS_EPS,                                 
                                  scinInfo->dy1 - MBTS_EPS,                                 
                                  scinInfo->dy2 - MBTS_EPS,                                 
                                  scinInfo->dz  - MBTS_EPS);                                
    scinTrd1->drawEdgeLines = outline;                                                     
    // Now add the object to the scene                                                       
    SoSeparator* scinSep1 = new SoSeparator();                                              
    scinSep1->addChild(scinXF1);                                                           
    scinSep1->addChild(scinTrd1);                                                          
    m_separator->addChild(scinSep1);                                                        
                                                                                             
    // Keep the pointer to the 3D object in the map                                          
    (*node2mbts)[scinTrd1] = this;                                                         
  }                                                                                          
  // ________________________________________________________________________________________
  // RUN2: if channel=1 draw two scintillators instead of one                               
                                                                                             
  return true;
}

std::vector<std::string> VP1Mbts::ToString()
{
  std::vector<std::string> result;

  // Decode ID
  Identifier id = m_cell->ID();

  int type = m_idhelper->type(id);       // -1 neg, 1 pos
  int module = m_idhelper->module(id);   // 0-7 scintillator copy number
  int channel = m_idhelper->channel(id); // 0 scin1, 1 scin2


  std::ostringstream msg, msg1, msg2, msg3;
  msg  << " MBTS. Side = " << type << ". Scintillator Type = " << channel
       << ", Num = " << module;
  msg1 << " Energy = " << m_cell->energy();
  msg2 << " Time = " << m_cell->time();

  std::string stars("***");
  result.push_back(stars);

  result.push_back(msg.str());
  result.push_back(msg1.str());
  result.push_back(msg2.str());

  result.push_back(stars);
  return result;
}
