/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_StrawMap.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TRT_DetectorPerformance/TRT_StrawMap.h"

#include "DataModel/DataVector.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"

#include "PathResolver/PathResolver.h"

//================ Constructor =================================================

InDet::TRT_StrawMap::TRT_StrawMap(const std::string& t,
				  const std::string& n,
				  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_nBarrelStraws(1642), // = 1642;
  m_nAllStraws(5482), // = 5482;
  m_nBarrelChips(104), // = 104;
  m_TRTStrawNeighbourTool("TRT_StrawNeighbourSvc", n)
{
  declareInterface<ITRT_StrawMap>(this);

  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
}

//================ Destructor =================================================

InDet::TRT_StrawMap::~TRT_StrawMap()
{}


//================ Initialisation =================================================

StatusCode InDet::TRT_StrawMap::initialize()
{

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  if (msgLvl(MSG::VERBOSE)) msg() << name() << "InDet::TRT_StrawMap::initialize()" << endreq;

/* only in 15 nightlies

  if ( !(evtStore()) ) {
    if (msgLvl(MSG::ERROR)) msg() << "missing evtStore StoreGate Svc!" << endreq;
    return sc;
  }
  if ( !(detStore()) ) {
    if (msgLvl(MSG::ERROR)) msg() << "missing detStore Svc!" << endreq;
    return sc;
  }
  sc = detStore()->retrieve(m_TRTDetMgr, "TRT");
  if ( sc.isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg() << "Unable to retrieve TRT Detector Manager " << endreq;
    return sc;
  }
*/



  sc = detStore()->retrieve(m_TRTDetMgr, "TRT");
  if ( sc.isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg() << "Unable to retrieve TRT Detector Manager " << endreq;
    return sc;
  }

  // TRT helper
  if (detStore()->retrieve(m_TRTHelper, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  
  // set m_strawToPadMap

  for (int i=0; i<m_nBarrelStraws; i++) m_strawToPadMap[i] = -1; // read strawToChip from the file

  std::string sasa_filename=PathResolver::find_file ("swIndex.txt", "DATAPATH");
  FILE *f = fopen(sasa_filename.c_str(), "r");
  if (f==0) { 
    if (msgLvl(MSG::ERROR)) msg() << "InDet::TRT_StrawMap::initialize() needs file swIndex.txt!!!" << endreq; 
    return sc; 
  }
  char line[300];
  int tmp[5];
  while ( fgets(line, 300, f) != 0 ) {
    if (line[0]=='#') continue;
    sscanf(line, "%d %d %d %d %d\n", tmp, tmp+1, tmp+2, tmp+3, tmp+4);
    m_strawToPadMap[ tmp[0] ] = 2 * tmp[1]; // chip -> pad
  }
  fclose(f);
  for (int i=0; i<m_nBarrelStraws; i++) if (m_strawToPadMap[i]<0||m_strawToPadMap[i]>=m_nBarrelChips*2) { 
    if (msgLvl(MSG::ERROR)) msg() << "InDet::TRT_StrawMap::initialize() m_strawToPadMap[" << i <<"] =  " << m_strawToPadMap[i] << " error (barrel)." << endreq; 
    m_strawToPadMap[i] = 0; // avoid crashing later
  }

//  int endcapChipNumbers[] = {8, 11, 9, 10, 4, 7, 5, 6, 0, 3, 1, 2};
  int endcapChipNumbers[] = {1, 2, 0, 3, 5, 6, 4, 7, 9, 10, 8, 11};
  
  for (int board=0; board<20; board++) // do the same for endcap
    for (int strawLayerNumber=0; strawLayerNumber<8; strawLayerNumber++) 
      for (int strawNumber = 0; strawNumber<24; strawNumber++) {
        int straw = board * 192 + strawNumber * 8 + strawLayerNumber ;
        straw += m_nBarrelStraws;
        int chip = (strawNumber - strawNumber%4) / 4;  // numbering 0-11, see few lines later for hardware numbering consistency
        chip *= 2;
        if (strawLayerNumber>=4) chip += 1; // this gives you chip number in 0-11 range 
        chip = endcapChipNumbers[chip];

        int pad = strawNumber%2; 
 	m_strawToPadMap[ straw ] = 2 * ( chip + board * 12 + m_nBarrelChips ) + pad;
      }
  for (int i=m_nBarrelStraws; i<m_nAllStraws; i++) { //  cross-check
    if ( m_strawToPadMap[i] == -1 ) {
	  if (msgLvl(MSG::ERROR)) msg() << "InDet::TRT_StrawMap::initialize() m_strawToPadMap[" << i <<"] =  " << m_strawToPadMap[i] << " error (endcap)." << endreq;
      m_strawToPadMap[i] = m_nBarrelChips * 2;
	}
  }

  return sc;
}

//================ Finalisation =================================================

StatusCode InDet::TRT_StrawMap::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

//============================================================================================

int InDet::TRT_StrawMap::board(Identifier id) {
  if (id != m_id) setNewIdentifier(id);
  return board();
}

int InDet::TRT_StrawMap::board() {
  return strawToBoard(m_straw);
}

int InDet::TRT_StrawMap::strawToBoard(int straw) {
  return chipToBoard( m_strawToPadMap[straw] / 2 );
}

int InDet::TRT_StrawMap::chipToBoard(int chip) {

  // return logical board index:
  // barrel
  // 0 for Board 1S (has 10 chips)  0 -  9
  // 1 for 1L (11)                 10 - 20
  // 2 for 2S (15)                 21 - 35
  // 3 for 2L, first 9 chips       36 - 44
  // 4 for 2L, second 9 chips      45 - 53
  // 5 for 3S, first 11            54 - 64
  // 6 for 3S, second 12           65 - 76
  // 7 for 3L, first 13            77 - 89 
  // 8 for 3L, second 14           90 - 103
  // endcap
  // 9-28 from smaller |z| to larger |z|

  if (chip >= m_nBarrelChips)  return ( 9 + (chip-m_nBarrelChips) / 12 );
                               // endcap: always 12 chips per board, easy; +9 such that range is 9-28

  static int list[] = {10, 11, 15, 9, 9, 11, 12, 13, 14}; // N chips on each barrel board
  int count = 0;
  for (int i=0; i<9; i++) {
    count += list[i];
    if (chip < count) return i;
  }

  if (msgLvl(MSG::ERROR)) msg() << name() << "InDet::TRT_StrawMap::chipToBoard(), should never come this far! input chip number: " << chip << endreq;
  return 0;
}

int InDet::TRT_StrawMap::chip(Identifier id) {
  if (id != m_id) setNewIdentifier(id);
  return chip();
}

int InDet::TRT_StrawMap::chip() {
  return strawToChip(m_straw, m_reverse);
}

int InDet::TRT_StrawMap::strawToChip(int straw, int side, int phi) {

  int reversed = 0;
  if (abs(side)==2&&(phi<8||(phi>=16&&phi<24))) reversed = 1;
  if (side==-2) reversed = 3-reversed;
  return strawToChip(straw, reversed);
} 

int InDet::TRT_StrawMap::strawToChip(int straw, int reversed) {

  int chip = m_strawToPadMap[straw] / 2;
  if (chip < m_nBarrelChips) return chip; // barrel 

  int chipInBoard = ( chip - m_nBarrelChips ) % 12; // endcap
  chip -= chipInBoard;
  if (reversed%2==1) chipInBoard = 10 + 2 * ( chipInBoard % 2 ) - chipInBoard ; 
  static int reverseSideC[] = {9, 8, 11, 10, 5, 4, 7, 6, 1, 0, 3, 2};  // {10, 11, 8, 9, 6, 7, 4, 5, 2, 3, 0, 1};
  if (reversed>1) chipInBoard = reverseSideC[chipInBoard];
  chip += chipInBoard; 
  return chip;
} 

void InDet::TRT_StrawMap::setNewIdentifier(Identifier id, int *index) {

  int side = m_TRTHelper->barrel_ec(id);
  int phi = m_TRTHelper->phi_module(id);  
  int layerNumber = m_TRTHelper->layer_or_wheel(id);
  int strawLayerNumber = m_TRTHelper->straw_layer(id);
  int strawNumber = m_TRTHelper->straw(id); 
  
  if (index) { index[0] = side; index[1] = phi; index[2] = layerNumber; index[3] = strawLayerNumber; index[4] = strawNumber; }
  
  int straw(0);

  if (abs(side)==1) { // barrel
  
    static int firstCall(1);
    static int numberOfStraws[74];
    if (firstCall) {
      firstCall = 0;
      int strawsPerStrawLayer[] = {14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 17, // layer 0, strawlayers 0-18
				   18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 22, // layer 1, strawLayers 0-23
				   22, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 27 }; // layer 2, strawlayers 0-29
      numberOfStraws[0] = 0;
      for (int i=0; i<73; i++) numberOfStraws[i+1] = numberOfStraws[i] + strawsPerStrawLayer[i] + 1;
    }

    if (layerNumber==1) strawLayerNumber+= 19;
    if (layerNumber==2) strawLayerNumber+= 43;

    straw = ( numberOfStraws[strawLayerNumber+1] - strawNumber -1 );
  
  } else if (abs(side)==2) { // endcap
  
    int board = layerNumber;
    if (layerNumber<6) { //  A-type wheels, two wheels (boards) per "layer"
      board *= 2;
      if (strawLayerNumber>=8) board += 1;
    } else { // shift B wheels accordingly
      board += 6;
    } // board numbered 0-19
    
    straw = board * 192 + strawNumber * 8 + strawLayerNumber % 8;

    int myStraw = straw;    // need to cross-check that it is the same as Jim's code
	
    straw = 0;

    if((layerNumber<6) & (strawLayerNumber>7))
      {
	straw=straw+(384*layerNumber);
	straw=straw+192+(strawLayerNumber%8)+(strawNumber*8);
      }
    else if((layerNumber<6) & (strawLayerNumber<8))
      {
	straw=straw+(384*layerNumber);
	straw=straw + (strawLayerNumber%8) + (strawNumber*8);
      }

    else if((layerNumber>5) & (strawLayerNumber>7))
      {
	straw = straw + 2304 + 192*(layerNumber-6);
	straw = straw + 192 + (strawLayerNumber%8) + (8*strawNumber);
      }
    else if((layerNumber>5) & (strawLayerNumber<8))
      {
	straw = straw + 2304 + 192*(layerNumber-6);
	straw = straw + (strawLayerNumber%8) + (8*strawNumber);
      }
	
    if (straw != myStraw) if (msgLvl(MSG::ERROR)) msg() << "endcap numbering problem " << straw <<  " " << myStraw << endreq;
  
    straw += m_nBarrelStraws; // shift such that they come after the barrel straws
  }

  m_id = id;
  m_straw = straw;
  m_reverse = (abs(side)==2&&(phi<8||(phi>=16&&phi<24))) ? 1 : 0;
  if (side==-2) m_reverse = 3-m_reverse;  // just the other phi sectors are reversed in side C, shift by 2
  
  return;
}


int InDet::TRT_StrawMap::straw(Identifier id) {
  if (id != m_id) setNewIdentifier(id);
  return straw();
}

int InDet::TRT_StrawMap::straw() {
  return m_straw;
}

void InDet::TRT_StrawMap::compareWithTRTStrawNeighbour() {

  if (msgLvl(MSG::VERBOSE)) msg() << "InDet::TRT_StrawMap::compareWithTRTStrawNeighbour: printing out file compareWithTRTStrawNeighbour.txt" << endreq;

  FILE *f = fopen("compareWithTRTStrawNeighbour.txt", "w");

  for (std::vector<Identifier>::const_iterator it = m_TRTHelper->straw_layer_begin(); it != m_TRTHelper->straw_layer_end(); it++  ) {

    int nStrawsInLayer = m_TRTHelper->straw_max(*it);
		  
    for (int i=0; i<=nStrawsInLayer; i++) {
	  
      Identifier id = m_TRTHelper->straw_id(*it, i);
      int index[5];
      this->setNewIdentifier(id, index);
		
      int myChip = this->chip();
      int myReference(-1);

      if (abs(index[0])==2) {
        myReference = ( myChip - m_nBarrelChips ) % 12; // endcap
        if ( index[2]<6 && this->board()%2==0 ) myReference += 20; // A wheels
      } else if (abs(index[0])==1) {
        myReference = myChip;
        if (index[2]==0) myReference +=  1;
        if (index[2]==1) myReference -= 20;
        if (index[2]==2) myReference -= 53;		  
      }

      int chip; 
      m_TRTStrawNeighbourTool->getChip(id, chip);

      //const HepGeom::Point3D<double> &centerRef = m_TRTDetMgr->getElement( id )->center( id );
      const Amg::Vector3D  centerRef = m_TRTDetMgr->getElement( id )->center( id );

      for (int j=0; j<5; j++) fprintf(f, "%2d ", index[j]);
      fprintf(f, " %4d %2d %3d %2d %2d ", this->straw(), this->board(), myChip, myReference, chip);
      fprintf(f, "%f %f %f\n", centerRef.x(), centerRef.y(), centerRef.z());
    }
  }

  fclose(f);
  return;
}
