/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// Implementation file for TRT_FillCablingData_DC1 class
//

#include "TRT_FillCablingData_DC1.h"
#include "InDetIdentifier/TRT_ID.h"
#include <iostream>
//#include <math.h>
//#include "GaudiKernel/MsgStream.h"
//#include "TRT_ReadoutGeometry/TRT_DetectorManager.h"

static const InterfaceID IID_ITRT_FillCablingData_DC1
                            ("TRT_FillCablingData_DC1", 1, 0);


  // Constructor
TRT_FillCablingData_DC1::TRT_FillCablingData_DC1
  (const std::string& type, const std::string& name, const IInterface* parent):
     AthAlgTool(type, name, parent)
{
  declareInterface<TRT_FillCablingData_DC1>(this);
}


  // Destructor  
TRT_FillCablingData_DC1::~TRT_FillCablingData_DC1() 
{}


const InterfaceID& TRT_FillCablingData_DC1::interfaceID()
{
  return IID_ITRT_FillCablingData_DC1;
}


  // Initialize
StatusCode TRT_FillCablingData_DC1::initialize()
{
  ATH_MSG_INFO( "TRT_FillCablingData_DC1::initialize()" );
  

  // Get the TRT Helper
  if (detStore()->retrieve(m_id_trt, "TRT_ID").isFailure()) 
  {
    ATH_MSG_FATAL( "Could not get TRT ID helper" );
    return StatusCode::FAILURE;
  }
  m_cntx = m_id_trt->straw_layer_context();

  m_cabling = new TRT_CablingData;

  defineParameters();
//  printParameters();
  defineTables();
  defineCollID();

  ATH_MSG_INFO( "TRT_FillCablingData_DC1::initializiation finished" );

  return StatusCode::SUCCESS;
}


StatusCode TRT_FillCablingData_DC1::finalize()
{
  StatusCode sc = AlgTool::finalize();
  ATH_MSG_INFO( "finalize()" );

  delete m_cabling;

  delete [] m_layerID;
  delete [] m_strawIDInLayer;
  delete [] m_numberOfStrawsInPreviousLayers;

  return sc;
}


TRT_CablingData* TRT_FillCablingData_DC1::fillData()
{
  return m_cabling;
}


void TRT_FillCablingData_DC1::defineParameters()
{
  m_numberOfWheelsA = 6;
  int numberOfWheelsB = 8;
  int numberOfWheelsC = 4;

  m_numberOfWheelsAB = m_numberOfWheelsA + numberOfWheelsB;
  m_numberOfWheelsABC = m_numberOfWheelsAB + numberOfWheelsC;

  m_numberOfStrawPlanesAC = 16;
  m_numberOfStrawPlanesB = 8;

  m_numberOfPlanesAB = m_numberOfWheelsA * m_numberOfStrawPlanesAC +
    numberOfWheelsB * m_numberOfStrawPlanesB;

  m_totalNumberOfPlanes = m_numberOfPlanesAB + numberOfWheelsC *
    m_numberOfStrawPlanesAC;

  m_numberOfIdentifierSectors = 32;
  m_numberOfEndCapPhiSectors = 96;

  m_numberOfPhiSectorsInIDSector = m_numberOfEndCapPhiSectors /
    m_numberOfIdentifierSectors;

  int numberOfStrawsInPlaneAB = 768;
  int numberOfStrawsInPlaneC = 576;

  m_numberOfStrawsInPhiSectorAB = numberOfStrawsInPlaneAB /
    m_numberOfEndCapPhiSectors;
  m_numberOfStrawsInPhiSectorC = numberOfStrawsInPlaneC /
    m_numberOfEndCapPhiSectors;

  m_totalNumberOfStrawsInPhiSectorsAB = m_numberOfStrawsInPhiSectorAB *
    m_numberOfPlanesAB;

  m_numberOfStrawsAInRod = numberOfStrawsInPlaneAB / m_numberOfEndCapPhiSectors *
    m_numberOfStrawPlanesAC * m_numberOfWheelsA;
  m_numberOfStrawsABInRod = m_numberOfStrawsAInRod + numberOfStrawsInPlaneAB /
    m_numberOfEndCapPhiSectors * m_numberOfStrawPlanesB * numberOfWheelsB;

  m_numberOfStrawsInWheelAForRod = numberOfStrawsInPlaneAB /
    m_numberOfEndCapPhiSectors * m_numberOfStrawPlanesAC;
  m_numberOfStrawsInWheelBForRod = numberOfStrawsInPlaneAB /
    m_numberOfEndCapPhiSectors * m_numberOfStrawPlanesB;
  m_numberOfStrawsInWheelCForRod = numberOfStrawsInPlaneC /
    m_numberOfEndCapPhiSectors * m_numberOfStrawPlanesAC;

  m_numberOfRings = 3;

  m_numberOfLayersA = 19;
  m_numberOfLayersB = 24;
  m_numberOfLayersC = 30;

  m_numberOfLayersAB = m_numberOfLayersA + m_numberOfLayersB;

  m_numberOfLayersInPhiSector = m_numberOfLayersAB + m_numberOfLayersC;

  m_numberOfStrawsA = 329;
  int numberOfStrawsB = 520;
  int numberOfStrawsC = 793;

  m_numberOfStrawsAB = m_numberOfStrawsA + numberOfStrawsB;

  m_numberOfStrawsInPhiSector = m_numberOfStrawsAB + numberOfStrawsC;

  m_numberOfStrawsInROD = 1664;
  m_numberOfStrawsInBarrelROD = 1642;

  m_shiftForLeftEndCap = 0;
  m_shiftForLeftBarrel = m_totalNumberOfPlanes * m_numberOfIdentifierSectors;
  m_shiftForRightBarrel = m_shiftForLeftBarrel + m_numberOfLayersInPhiSector *
    m_numberOfIdentifierSectors;
  m_shiftForRightEndCap = m_shiftForRightBarrel + m_numberOfLayersInPhiSector *
    m_numberOfIdentifierSectors;
    
  m_shiftForLeftEndCapStraws = 0;
  m_shiftForLeftBarrelStraws = m_numberOfEndCapPhiSectors * m_numberOfStrawsInROD;
  m_shiftForRightBarrelStraws = m_shiftForLeftBarrelStraws + 
    m_numberOfIdentifierSectors * m_numberOfStrawsInBarrelROD;
  m_shiftForRightEndCapStraws = m_shiftForRightBarrelStraws +
    m_numberOfIdentifierSectors * m_numberOfStrawsInBarrelROD;


  int numberOfStrawsInLayersA[] = {15, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18,
    18, 18, 18, 18, 19, 19, 19, 18};

  int numberOfStrawsInLayersB[] = {19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21,
    22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 23};

  int numberOfStrawsInLayersC[] = {23, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26,
    26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 28};

  m_layerID = new int[m_numberOfStrawsInPhiSector];
  m_strawIDInLayer = new int[m_numberOfStrawsInPhiSector];

  int currentLayerID = 0;
  int firstStrawInLayer = 0;
  int firstStrawInNextLayer = numberOfStrawsInLayersA[currentLayerID];

  int i;
  for (i = 0; i < m_numberOfStrawsInPhiSector; ++i)
  {
    if (i < m_numberOfStrawsA)
    {
      if (i == firstStrawInNextLayer)
      {
        ++currentLayerID;
        firstStrawInLayer = firstStrawInNextLayer;
        firstStrawInNextLayer += numberOfStrawsInLayersA[currentLayerID];
      }
      m_layerID[i] = currentLayerID;
      m_strawIDInLayer[i] = i - firstStrawInLayer;
    }
    else if (i < m_numberOfStrawsAB)
    {
      if (i == m_numberOfStrawsA || i == firstStrawInNextLayer)
      {
        ++currentLayerID;
        firstStrawInLayer = firstStrawInNextLayer;
        firstStrawInNextLayer += numberOfStrawsInLayersB[currentLayerID -
          m_numberOfLayersA];
      }
      m_layerID[i] = currentLayerID;
      m_strawIDInLayer[i] = i - firstStrawInLayer;
    }
    else
    {
      if (i == m_numberOfStrawsAB || i == firstStrawInNextLayer)
      {
        ++currentLayerID;
        firstStrawInLayer = firstStrawInNextLayer;
        firstStrawInNextLayer += numberOfStrawsInLayersC[currentLayerID -
          m_numberOfLayersAB];
      }
      m_layerID[i] = currentLayerID;
      m_strawIDInLayer[i] = i - firstStrawInLayer;
    }
  }

  m_numberOfStrawsInPreviousLayers = new int[m_numberOfLayersInPhiSector];

  int numberOfStrawsInPreviousLayer;
  m_numberOfStrawsInPreviousLayers[0] = 0;
  for (i = 1; i < m_numberOfLayersInPhiSector; ++i)
  {
    if (i <= m_numberOfLayersA)
      numberOfStrawsInPreviousLayer = numberOfStrawsInLayersA[i - 1];
    else if (i <= m_numberOfLayersAB)
      numberOfStrawsInPreviousLayer =
        numberOfStrawsInLayersB[i - m_numberOfLayersA - 1];
    else
      numberOfStrawsInPreviousLayer =
        numberOfStrawsInLayersC[i - m_numberOfLayersAB - 1];
    m_numberOfStrawsInPreviousLayers[i] =
      m_numberOfStrawsInPreviousLayers[i - 1] + numberOfStrawsInPreviousLayer;
  }

}

void TRT_FillCablingData_DC1::printParameters() const
{

   ATH_MSG_DEBUG( " " );
   ATH_MSG_DEBUG( "***** TRT_IdentifierConversion::printParameters *****" );


  ATH_MSG_DEBUG( "End-cap parameters:" );
  ATH_MSG_DEBUG( "  numberOfWheelsA=" << m_numberOfWheelsA );
  ATH_MSG_DEBUG( "  numberOfWheelsAB=" << m_numberOfWheelsAB );
  ATH_MSG_DEBUG( "  numberOfWheelsABC=" << m_numberOfWheelsABC );
  ATH_MSG_DEBUG( "  numberOfPlanesAB=" << m_numberOfPlanesAB );
  ATH_MSG_DEBUG( "  totalNumberOfPlanes=" << m_totalNumberOfPlanes 
      );
  ATH_MSG_DEBUG( "  numberOfPhiSectorsInIDSector=" << m_numberOfPhiSectorsInIDSector );
  ATH_MSG_DEBUG( "  numberOfStrawsInPhiSectorAB=" << m_numberOfStrawsInPhiSectorAB );
  ATH_MSG_DEBUG( "  numberOfStrawsInPhiSectorC=" << m_numberOfStrawsInPhiSectorC );
  ATH_MSG_DEBUG( "  totalNumberOfStrawsInPhiSectorsAB=" << m_totalNumberOfStrawsInPhiSectorsAB );
  ATH_MSG_DEBUG( "  numberOfStrawsAInRod=" << m_numberOfStrawsAInRod 
      );
  ATH_MSG_DEBUG( "  numberOfStrawsABInRod=" << m_numberOfStrawsABInRod 
      );
  ATH_MSG_DEBUG( "  numberOfStrawsInWheelAForRod=" << m_numberOfStrawsInWheelAForRod );
  ATH_MSG_DEBUG( "  numberOfStrawsInWheelBForRod=" << m_numberOfStrawsInWheelBForRod );
  ATH_MSG_DEBUG( "  numberOfStrawsInWheelCForRod=" << m_numberOfStrawsInWheelCForRod );

  ATH_MSG_DEBUG( "Barrel parameters:" ); 
  ATH_MSG_DEBUG( "  numberOfRings=" << m_numberOfRings );
  ATH_MSG_DEBUG( "  numberOfStrawsA=" << m_numberOfStrawsA );
  ATH_MSG_DEBUG( "  numberOfStrawsAB=" << m_numberOfStrawsAB );
  ATH_MSG_DEBUG( "  numberOfLayersA=" << m_numberOfLayersA );
  ATH_MSG_DEBUG( "  numberOfLayersB=" << m_numberOfLayersB );
  ATH_MSG_DEBUG( "  numberOfLayersC=" << m_numberOfLayersC );
  ATH_MSG_DEBUG( "  numberOfLayersAB=" << m_numberOfLayersAB );
  ATH_MSG_DEBUG( "  numberOfLayersInPhiSector=" << m_numberOfLayersInPhiSector );
  ATH_MSG_DEBUG( "  numberOfStrawsInPhiSector=" << m_numberOfStrawsInPhiSector );

  ATH_MSG_DEBUG( "  numberOfStrawsInROD=" << m_numberOfStrawsInROD 
      );
  ATH_MSG_DEBUG( "  numberOfStrawsInBarrelROD=" << m_numberOfStrawsInBarrelROD );
  ATH_MSG_DEBUG( "  numberOfIdentifierSector=" << m_numberOfIdentifierSectors );
  ATH_MSG_DEBUG( "  numberOfEndCapPhiSectors=" << m_numberOfEndCapPhiSectors );

  int i;
  for (i = 0; i < m_numberOfStrawsInPhiSector; i++)
    ATH_MSG_DEBUG( "  layerID[" << i << "]=" << m_layerID[i] );
  ATH_MSG_DEBUG( " " );

  for (i = 0; i < m_numberOfStrawsInPhiSector; i++)
    ATH_MSG_DEBUG( "  strawIDInLayer[" << i << "]=" << m_strawIDInLayer[i] );
  ATH_MSG_DEBUG( " " );

  for (i = 0; i < m_numberOfLayersInPhiSector; i++)
    ATH_MSG_DEBUG( "  numberOfStrawsInPreviousLayers[" << i << "]=" << m_numberOfStrawsInPreviousLayers[i] );
  ATH_MSG_DEBUG( " " );


  ATH_MSG_DEBUG( "Shifts for subdetectors:" ); 
  ATH_MSG_DEBUG( "  shiftForLeftEndCap=" << m_shiftForLeftEndCap );
  ATH_MSG_DEBUG( "  shiftForLeftBarrel=" << m_shiftForLeftBarrel );
  ATH_MSG_DEBUG( "  shiftForRightBarrel=" << m_shiftForRightBarrel ); 
  ATH_MSG_DEBUG( "  shiftForRightEndCap=" << m_shiftForRightEndCap );
  ATH_MSG_DEBUG( "  shiftForLeftEndCapStraws=" << m_shiftForLeftEndCapStraws );
  ATH_MSG_DEBUG( "  shiftForLeftBarrelStraws=" << m_shiftForLeftBarrelStraws );
  ATH_MSG_DEBUG( "  shiftForRightBarrelStraws=" << m_shiftForRightBarrelStraws );
  ATH_MSG_DEBUG( "  shiftForRightEndCapStraws=" << m_shiftForRightEndCapStraws );

}


void TRT_FillCablingData_DC1::defineTables()
{
  int straw;
  Identifier Id;
  Identifier IdLayer;
  IdentifierHash hashId;
 
    // Loop over All RODs and All Straws in them
  //EventFormat::SubDetector det_id = EventFormat::TRT_ENDCAP_A_SIDE;
  eformat::SubDetector det_id = eformat::TRT_ENDCAP_A_SIDE;
 
  for(int phi=0; phi<32;++phi)
  {
    for (int i = 0; i < 3; ++i)
    {
      for (straw = 0; straw < 1664; ++straw)
      {
        Id = defineIdentifier(det_id, phi * 3 + i, straw); 
        m_cabling->set_identifierForAllStraws(Id);	
	IdLayer = m_id_trt->layer_id(Id);
	if ( !m_id_trt->get_hash(IdLayer, hashId, &m_cntx) )
	   m_cabling->set_identifierHashForAllStraws(hashId);
	else
	   ATH_MSG_DEBUG( "defineTables: unable to get hash for IdLayer " << m_id_trt->show_to_string(IdLayer) );

	//std::cout << " Creating straw ID " << m_id_trt->show_to_string(IdLayer) 
	//	    << " in Collection " << hashId << std::endl;
      }
    }
  }

  //det_id = EventFormat::TRT_BARREL_A_SIDE;
  det_id = eformat::TRT_BARREL_A_SIDE;
  for(int phi=0;phi<32;++phi)
  {
    for (straw = 0; straw < 1642; ++straw)
    {
      Id = defineIdentifier(det_id, phi, straw);
      m_cabling->set_identifierForAllStraws(Id);
      IdLayer = m_id_trt->layer_id(Id);
      if ( !m_id_trt->get_hash(IdLayer, hashId, &m_cntx) )
	 m_cabling->set_identifierHashForAllStraws(hashId);
      else
	 ATH_MSG_DEBUG( "defineTables: unable to get hash for IdLayer " << m_id_trt->show_to_string(IdLayer) );
    }
  }

  //det_id = EventFormat::TRT_BARREL_C_SIDE;
  det_id = eformat::TRT_BARREL_C_SIDE;
  for(int phi=0;phi<32;++phi)
  {
    for (straw = 0; straw < 1642; ++straw)
    {
      Id = defineIdentifier(det_id, phi, straw);
      m_cabling->set_identifierForAllStraws(Id); 
      IdLayer = m_id_trt->layer_id(Id);
      if ( !m_id_trt->get_hash(IdLayer, hashId, &m_cntx) )
	 m_cabling->set_identifierHashForAllStraws(hashId);
      else
	 ATH_MSG_DEBUG( "defineTables: unable to get hash for IdLayer " << m_id_trt->show_to_string(IdLayer) );
    }     
  } 

  //det_id = EventFormat::TRT_ENDCAP_C_SIDE;
  det_id = eformat::TRT_ENDCAP_C_SIDE;
  for(int phi=0;phi<32;++phi)
  {
    for (int i = 0; i < 3; ++i)
    {
      for (straw = 0; straw < 1664; ++straw)
      {
        Id = defineIdentifier(det_id, phi * 3 + i, straw);
        m_cabling->set_identifierForAllStraws(Id);
	IdLayer = m_id_trt->layer_id(Id);
        if ( !m_id_trt->get_hash(IdLayer, hashId, &m_cntx) )
	   m_cabling->set_identifierHashForAllStraws(hashId);
	else
	   ATH_MSG_DEBUG( "defineTables: unable to get hash for IdLayer " << m_id_trt->show_to_string(IdLayer) );
      }	
    }
  }
  
//  ATH_MSG_DEBUG( " m_identifierForAllStraws size " << m_identifierForAllStraws.size() );

    // Initialize m_allRobs
  //EventFormat::ModuleType type = EventFormat::ROD_TYPE;
  //det_id = EventFormat::TRT_ENDCAP_A_SIDE;
  det_id = eformat::TRT_ENDCAP_A_SIDE;

  for(int phi=0; phi<32;++phi)
  {
    for (int i = 0; i < 3; ++i)
    {
      //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) (phi * 3 + i));
      eformat::helper::SourceIdentifier sid(det_id,(uint8_t) (phi * 3 + i));
      m_cabling->add_allRobs(sid.code());
    }
  }

  //det_id = EventFormat::TRT_ENDCAP_C_SIDE;
  det_id = eformat::TRT_ENDCAP_C_SIDE;
  for(int phi=0;phi<32;++phi)
  {
    for (int i = 0; i < 3; ++i)
    {
      //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) (phi * 3 + i));
      eformat::helper::SourceIdentifier sid(det_id,(uint8_t) (phi * 3 + i));
      m_cabling->add_allRobs(sid.code());
    }
  }

  //det_id = EventFormat::TRT_BARREL_A_SIDE;
  det_id = eformat::TRT_BARREL_A_SIDE;
  for(int phi=0;phi<32;++phi)
  {
      //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) ( phi ));
      eformat::helper::SourceIdentifier sid(det_id,(uint8_t) ( phi ));
      m_cabling->add_allRobs(sid.code());
  }

  //det_id = EventFormat::TRT_BARREL_C_SIDE;
  det_id = eformat::TRT_BARREL_C_SIDE;
  for(int phi=0;phi<32;++phi)
  {
      //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) ( phi ));
      eformat::helper::SourceIdentifier sid(det_id,(uint8_t) ( phi ));
      m_cabling->add_allRobs(sid.code());
  }
}

void TRT_FillCablingData_DC1::defineCollID()
{
  ATH_MSG_DEBUG( " TRT_IdentifierConversionTool::defineCollID " );
   
    // initialize allRobs
  //EventFormat::ModuleType type = EventFormat::ROD_TYPE;
  //EventFormat::SubDetector  det_id = EventFormat::TRT_ENDCAP_A_SIDE;
  eformat::SubDetector  det_id = eformat::TRT_ENDCAP_A_SIDE;
  for(int phi=0; phi<32;++phi)
  {
    for (int i = 0; i < 3; ++i)
    {
      //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) (phi * 3 + i));
      eformat::helper::SourceIdentifier sid(det_id,(uint8_t) (phi * 3 + i));
      std::vector<IdentifierHash> * vect = new std::vector<IdentifierHash>();
      fillCollID(sid.code(), *vect);
      m_cabling->add_collID(sid.code(), vect);            
    }
  }

  //det_id = EventFormat::TRT_BARREL_A_SIDE;
  det_id = eformat::TRT_BARREL_A_SIDE;
  for(int phi=0;phi<32;++phi)
  {
      //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) ( phi ));
      eformat::helper::SourceIdentifier sid(det_id,(uint8_t) ( phi ));
      std::vector<IdentifierHash> * vect = new std::vector<IdentifierHash>();
      fillCollID(sid.code(), *vect);
      m_cabling->add_collID(sid.code(), vect);
  }

  //det_id = EventFormat::TRT_BARREL_C_SIDE;
  det_id = eformat::TRT_BARREL_C_SIDE;
  for(int phi=0;phi<32;++phi)
  {
      //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) ( phi ));
      eformat::helper::SourceIdentifier sid(det_id,(uint8_t) ( phi ));
      std::vector<IdentifierHash> * vect = new std::vector<IdentifierHash>();
      fillCollID(sid.code(), *vect);
      m_cabling->add_collID(sid.code(), vect);
  }
  
  //det_id = EventFormat::TRT_ENDCAP_C_SIDE;
  det_id = eformat::TRT_ENDCAP_C_SIDE;
  for(int phi=0;phi<32;++phi)
  {
    for (int i = 0; i < 3; ++i)
    {
      //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) (phi * 3 + i));
      eformat::helper::SourceIdentifier sid(det_id,(uint8_t) (phi * 3 + i));
      std::vector<IdentifierHash> * vect = new std::vector<IdentifierHash>();
      fillCollID(sid.code(), *vect);
      m_cabling->add_collID(sid.code(), vect);
    }
  }
  
  ATH_MSG_DEBUG( " TRT_FillCablingData_DC1::defineCollID finished " );
}  
  
  
void TRT_FillCablingData_DC1::fillCollID(uint32_t rob_id, 
  std::vector<IdentifierHash> & ids)
{
  int id_barrel_ec, id_phi_module;
  IdContext slContext = m_id_trt->straw_layer_context();
  IdentifierHash idHash;

  //EventFormat::SubDetector det_id;
  eformat::SubDetector det_id;
  uint8_t module;

  eformat::helper::SourceIdentifier id (rob_id);
  det_id = id.subdetector_id();
  module = id.module_id();

  //if (det_id == EventFormat::TRT_ENDCAP_A_SIDE)
  if (det_id == eformat::TRT_ENDCAP_A_SIDE)
  {
    id_barrel_ec = -2;
    id_phi_module = ((int) module) / 3;
    for (int wheel = 0; wheel < m_numberOfWheelsABC; ++wheel)
    {
      if (wheel < m_numberOfWheelsA)
      {
	for (int layer = 0; layer < m_numberOfStrawPlanesAC; ++layer) 
	{
	      
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, wheel,
					     layer);
//	  std::cout << " fillCollID: Identifier id " << (std::string)id << std::endl; 				     
//	  std::cout << " fillCollID: IdentifierHash " << m_id_trt->get_hash(id, idHash, &slContext) << std::endl; 			     
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	  {	    
	    ids.push_back(idHash);
	  }
	  else  
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
      else if (wheel < m_numberOfWheelsAB)
      {
        for (int layer = 0; layer < m_numberOfStrawPlanesB; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, wheel, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
      else
      {
        for (int layer = 0; layer < m_numberOfStrawPlanesAC; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, wheel, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
    }
  }
  //else if (det_id == EventFormat::TRT_BARREL_A_SIDE)
  else if (det_id == eformat::TRT_BARREL_A_SIDE)
  {
    id_barrel_ec = -1;
    id_phi_module = (int) module;
    for (int ring = 0; ring < m_numberOfRings; ++ring)
    {
      if (ring == 0)
      {
        for (int layer = 0; layer < m_numberOfLayersA; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, ring, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
      else if (ring == 1)
      {
        for (int layer = 0; layer < m_numberOfLayersB; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, ring, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else  
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
      else
      {
        for (int layer = 0; layer < m_numberOfLayersC; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, ring, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else 
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
    }
  }
  //else if (det_id == EventFormat::TRT_BARREL_C_SIDE)
  else if (det_id == eformat::TRT_BARREL_C_SIDE)
  {
    id_barrel_ec = 1;
    id_phi_module = (int) module;
    for (int ring = 0; ring < m_numberOfRings; ++ring)
    {
      if (ring == 0)
      {
        for (int layer = 0; layer < m_numberOfLayersA; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, ring, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
      else if (ring == 1)
      {
        for (int layer = 0; layer < m_numberOfLayersB; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, ring, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
      else
      {
        for (int layer = 0; layer < m_numberOfLayersC; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, ring, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else
	    ATH_MSG_DEBUG( "fillCollID: unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
    }
  }
  //else if (det_id == EventFormat::TRT_ENDCAP_C_SIDE)
  else if (det_id == eformat::TRT_ENDCAP_C_SIDE)
  {
    id_barrel_ec = 2;
    id_phi_module = ((int) module) / 3;
    for (int wheel = 0; wheel < m_numberOfWheelsABC; ++wheel)
    {
      if (wheel < m_numberOfWheelsA)
      {
        for (int layer = 0; layer < m_numberOfStrawPlanesAC; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, wheel, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
      else if (wheel < m_numberOfWheelsAB)
      {
        for (int layer = 0; layer < m_numberOfStrawPlanesB; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, wheel, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
      else
      {
        for (int layer = 0; layer < m_numberOfStrawPlanesAC; ++layer) 
	{
	  Identifier id = m_id_trt->layer_id(id_barrel_ec, id_phi_module, wheel, layer);
	  if (!m_id_trt->get_hash(id, idHash, &slContext)) 
	    ids.push_back(idHash);
	  else
	    ATH_MSG_DEBUG( "fillCollID(): unable to get hash for id " << m_id_trt->show_to_string(id) );
	}
      }
    }
  }
}

  
  // Define Identifier for each straw from BS
Identifier TRT_FillCablingData_DC1::defineIdentifier
  //(const EventFormat::SubDetector& subdetector, const int& intRod,
  (const eformat::SubDetector& subdetector, const int& intRod,
  const int& straw) const
{
  int id_barrel_ec, id_layer_or_wheel, id_phi_module, id_straw_layer, id_straw;

//  if ((subdetector == EventFormat::TRT_BARREL_A_SIDE) ||
  if ((subdetector == eformat::TRT_BARREL_A_SIDE) ||
//    (subdetector == EventFormat::TRT_BARREL_C_SIDE)) // Barrel
      (subdetector == eformat::TRT_BARREL_C_SIDE)) // Barrel
  {
//    if (subdetector == EventFormat::TRT_BARREL_A_SIDE)
    if (subdetector == eformat::TRT_BARREL_A_SIDE)
      id_barrel_ec = -1;
    else
      id_barrel_ec = 1;

    id_phi_module = intRod;

    if (straw < m_numberOfStrawsA)  // Module A
    {
      id_layer_or_wheel = 0;
      id_straw_layer = m_layerID[straw];
      id_straw = m_strawIDInLayer[straw];
    }
    else if (straw < m_numberOfStrawsAB)  // Module B
    {
      id_layer_or_wheel = 1;
      id_straw_layer = m_layerID[straw] - m_numberOfLayersA;
      id_straw = m_strawIDInLayer[straw];
    }
    else  // Module C
    {
      id_layer_or_wheel = 2;
      id_straw_layer = m_layerID[straw] - m_numberOfLayersAB;
      id_straw = m_strawIDInLayer[straw];
    }
  }
  else  // Endcaps
  {
    //if (subdetector == EventFormat::TRT_ENDCAP_A_SIDE)
    if (subdetector == eformat::TRT_ENDCAP_A_SIDE)
      id_barrel_ec = -2;
    else
      id_barrel_ec = 2;

    id_phi_module = intRod / m_numberOfPhiSectorsInIDSector;
    int rodInIDSector = intRod % m_numberOfPhiSectorsInIDSector;

    if (straw < m_numberOfStrawsAInRod)  // Wheels A
    {
      id_layer_or_wheel = straw / m_numberOfStrawsInWheelAForRod;
      int strawInWheelA = straw % m_numberOfStrawsInWheelAForRod;
      id_straw_layer = strawInWheelA / m_numberOfStrawsInPhiSectorAB;
      id_straw = strawInWheelA % m_numberOfStrawsInPhiSectorAB +
        m_numberOfStrawsInPhiSectorAB * rodInIDSector;
    }
    else if (straw < m_numberOfStrawsABInRod)  // Wheels B
    {
      int strawInWheelsB = straw - m_numberOfStrawsAInRod;
      id_layer_or_wheel = strawInWheelsB / m_numberOfStrawsInWheelBForRod +
        m_numberOfWheelsA;
      int strawInWheelB = strawInWheelsB % m_numberOfStrawsInWheelBForRod;
      id_straw_layer = strawInWheelB / m_numberOfStrawsInPhiSectorAB;
      id_straw = strawInWheelB % m_numberOfStrawsInPhiSectorAB +
        m_numberOfStrawsInPhiSectorAB * rodInIDSector;
    }
    else  // Wheels C
    {
      int strawInWheelsC = straw - m_numberOfStrawsABInRod;
      id_layer_or_wheel = strawInWheelsC / m_numberOfStrawsInWheelCForRod +
        m_numberOfWheelsAB;
      int strawInWheelC = strawInWheelsC % m_numberOfStrawsInWheelCForRod;
      id_straw_layer = strawInWheelC / m_numberOfStrawsInPhiSectorC;
      id_straw = strawInWheelC % m_numberOfStrawsInPhiSectorC +
        m_numberOfStrawsInPhiSectorC * rodInIDSector;
    }
  }

  return m_id_trt->straw_id(id_barrel_ec, id_phi_module, id_layer_or_wheel,
    id_straw_layer, id_straw);
} 


  // Get ROBIDs for each Detector Element (Straw Layer)
std::vector<uint32_t> TRT_FillCablingData_DC1::getRobID(Identifier& id)
  const
{
  std::vector<uint32_t> v;

  //EventFormat::ModuleType type = EventFormat::ROB_TYPE;
  //EventFormat::SubDetector det_id;
  eformat::SubDetector det_id;

  int id_barrel_ec = m_id_trt->barrel_ec(id);
  int id_phi_module = m_id_trt->phi_module(id);

  if (id_barrel_ec == -2)
  {
    //det_id = EventFormat::TRT_ENDCAP_A_SIDE;
    det_id = eformat::TRT_ENDCAP_A_SIDE;
    for (int i = 0; i < 3; ++i)
    {
      //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) (id_phi_module * 3 + i));
      eformat::helper::SourceIdentifier sid(det_id,(uint8_t) (id_phi_module * 3 + i));
      v.push_back(sid.code());
    }
  }
  else if (id_barrel_ec == -1)
  {
    //det_id = EventFormat::TRT_BARREL_A_SIDE;
    det_id = eformat::TRT_BARREL_A_SIDE;
    //eformat::helper::SourceIdentifier sid(det_id, type, (uint8_t) id_phi_module);
    eformat::helper::SourceIdentifier sid(det_id, (uint8_t) id_phi_module);
    v.push_back(sid.code());
  }
  else if (id_barrel_ec == 1)
  {
    //det_id = EventFormat::TRT_BARREL_C_SIDE;
    det_id = eformat::TRT_BARREL_C_SIDE;
    //eformat::helper::SourceIdentifier sid(det_id, type, (uint8_t) id_phi_module);
    eformat::helper::SourceIdentifier sid(det_id, (uint8_t) id_phi_module);
    v.push_back(sid.code());
  }
  else if (id_barrel_ec == 2)
  {
    //det_id = EventFormat::TRT_ENDCAP_C_SIDE;
    det_id = eformat::TRT_ENDCAP_C_SIDE;
    for (int i = 0; i < 3; ++i)
    {
      //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) (id_phi_module * 3 + i));
      eformat::helper::SourceIdentifier sid(det_id,(uint8_t) (id_phi_module * 3 + i));
      v.push_back(sid.code());
    }
  }

  return v;
}

  // Get ROD  for each Straw
uint32_t TRT_FillCablingData_DC1::getRodID(Identifier& id) const
{

  uint32_t rod_id=0;
  int rod_num;

  //EventFormat::ModuleType type = EventFormat::ROD_TYPE;
  //EventFormat::SubDetector det_id;
  eformat::SubDetector det_id;

  int id_barrel_ec = m_id_trt->barrel_ec(id);
  int id_phi_module = m_id_trt->phi_module(id);
  int id_layer_or_wheel = m_id_trt->layer_or_wheel(id);
  int id_straw = m_id_trt->straw(id);

  if (id_barrel_ec == -2)
  {
    //det_id = EventFormat::TRT_ENDCAP_A_SIDE;
    det_id = eformat::TRT_ENDCAP_A_SIDE;
    if (id_layer_or_wheel < m_numberOfWheelsAB)
      rod_num =  id_straw / m_numberOfStrawsInPhiSectorAB;
    else
      rod_num =  id_straw / m_numberOfStrawsInPhiSectorC;
    //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) (id_phi_module * 3 + rod_num));
    eformat::helper::SourceIdentifier sid(det_id,(uint8_t) (id_phi_module * 3 + rod_num));
    rod_id = sid.code();
  }
  else if (id_barrel_ec == -1)
  {
    //det_id = EventFormat::TRT_BARREL_A_SIDE;
    det_id = eformat::TRT_BARREL_A_SIDE;
    //eformat::helper::SourceIdentifier sid(det_id, type, (uint8_t) (id_phi_module));
    eformat::helper::SourceIdentifier sid(det_id, (uint8_t) (id_phi_module));
    rod_id = sid.code();
  }
  else if (id_barrel_ec == 1)
  {
    //det_id = EventFormat::TRT_BARREL_C_SIDE;
    det_id = eformat::TRT_BARREL_C_SIDE;
    //eformat::helper::SourceIdentifier sid(det_id, type, (uint8_t) (id_phi_module));
    eformat::helper::SourceIdentifier sid(det_id, (uint8_t) (id_phi_module));
    rod_id = sid.code();
  }
  else if (id_barrel_ec == 2)
  {
    //det_id = EventFormat::TRT_ENDCAP_C_SIDE;
    det_id = eformat::TRT_ENDCAP_C_SIDE;
    if (id_layer_or_wheel < m_numberOfWheelsAB)
      rod_num =  id_straw / m_numberOfStrawsInPhiSectorAB;
    else
      rod_num =  id_straw / m_numberOfStrawsInPhiSectorC;
    //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) (id_phi_module * 3 + rod_num));
    eformat::helper::SourceIdentifier sid(det_id,(uint8_t) (id_phi_module * 3 + rod_num));
    rod_id = sid.code();
  }

  return rod_id;
}


  // Get Straw and ROD ID
void TRT_FillCablingData_DC1::getStrawRodID(Identifier& id, 
  uint32_t& rod_id, uint32_t& straw_number) const
{

  int rod_num, straw, layer;

  //EventFormat::ModuleType type = EventFormat::ROD_TYPE;
  //EventFormat::SubDetector det_id;
  eformat::SubDetector det_id;  

  int id_barrel_ec = m_id_trt->barrel_ec(id);
  int id_phi_module = m_id_trt->phi_module(id);
  int id_layer_or_wheel = m_id_trt->layer_or_wheel(id);
  int id_straw_layer = m_id_trt->straw_layer(id);
  int id_straw = m_id_trt->straw(id);

  if (id_barrel_ec == -2)
  {
    //det_id = EventFormat::TRT_ENDCAP_A_SIDE;
    det_id = eformat::TRT_ENDCAP_A_SIDE;
    if (id_layer_or_wheel < m_numberOfWheelsAB)
      rod_num =  id_straw / m_numberOfStrawsInPhiSectorAB;
    else
      rod_num =  id_straw / m_numberOfStrawsInPhiSectorC;
    //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) (id_phi_module * 3 + rod_num));
    eformat::helper::SourceIdentifier sid(det_id,(uint8_t) (id_phi_module * 3 + rod_num));
    rod_id = sid.code();
  }
  else if (id_barrel_ec == -1)
  {
    //det_id = EventFormat::TRT_BARREL_A_SIDE;
    det_id = eformat::TRT_BARREL_A_SIDE;
    //eformat::helper::SourceIdentifier sid(det_id, type, (uint8_t) (id_phi_module));
    eformat::helper::SourceIdentifier sid(det_id, (uint8_t) (id_phi_module));
    rod_id = sid.code();
  }
  else if (id_barrel_ec == 1)
  {
    //det_id = EventFormat::TRT_BARREL_C_SIDE;
    det_id = eformat::TRT_BARREL_C_SIDE;
    //eformat::helper::SourceIdentifier sid(det_id, type, (uint8_t) (id_phi_module));
    eformat::helper::SourceIdentifier sid(det_id, (uint8_t) (id_phi_module));
    rod_id = sid.code();
  }
  else if (id_barrel_ec == 2)
  {
    //det_id = EventFormat::TRT_ENDCAP_C_SIDE;
    det_id = eformat::TRT_ENDCAP_C_SIDE;
    if (id_layer_or_wheel < m_numberOfWheelsAB)
      rod_num =  id_straw / m_numberOfStrawsInPhiSectorAB;
    else
      rod_num =  id_straw / m_numberOfStrawsInPhiSectorC;
    //eformat::helper::SourceIdentifier sid(det_id, type,(uint8_t) (id_phi_module * 3 + rod_num));
    eformat::helper::SourceIdentifier sid(det_id,(uint8_t) (id_phi_module * 3 + rod_num));
    rod_id = sid.code();
  }

  if ((id_barrel_ec == -2) || (id_barrel_ec == 2))  // Endcaps
  {
    if (id_layer_or_wheel < m_numberOfWheelsA)
      straw = id_layer_or_wheel * m_numberOfStrawsInWheelAForRod +
        id_straw_layer * m_numberOfStrawsInPhiSectorAB +
        id_straw % m_numberOfStrawsInPhiSectorAB;
    else if (id_layer_or_wheel < m_numberOfWheelsAB)
      straw = m_numberOfStrawsAInRod + (id_layer_or_wheel - m_numberOfWheelsA) *
        m_numberOfStrawsInWheelBForRod +
        id_straw_layer * m_numberOfStrawsInPhiSectorAB +
        id_straw % m_numberOfStrawsInPhiSectorAB;
    else
      straw = m_numberOfStrawsABInRod +
        (id_layer_or_wheel - m_numberOfWheelsAB) *
        m_numberOfStrawsInWheelCForRod +
        id_straw_layer * m_numberOfStrawsInPhiSectorC +
        id_straw % m_numberOfStrawsInPhiSectorC;
  }
  else  // Barrel
  {
    if (id_layer_or_wheel == 0)
      layer = id_straw_layer;
    else if (id_layer_or_wheel == 1)
      layer = m_numberOfLayersA + id_straw_layer;
    else
      layer = m_numberOfLayersAB + id_straw_layer;
    straw = m_numberOfStrawsInPreviousLayers[layer] + id_straw;
  }

  straw_number = (uint32_t) straw;
}

void TRT_FillCablingData_DC1::get_parameters(int& numberOfStrawsInROD,
    int& numberOfStrawsInBarrelROD,
    int& shiftForLeftEndCapStraws,
    int& shiftForLeftBarrelStraws,
    int& shiftForRightBarrelStraws,
    int& shiftForRightEndCapStraws,
    int& numberOfEndCapPhiSectors,
    int& numberOfIdentifierSectors)
{
  numberOfStrawsInROD = m_numberOfStrawsInROD;
  numberOfStrawsInBarrelROD = m_numberOfStrawsInBarrelROD;
  shiftForLeftEndCapStraws = m_shiftForLeftEndCapStraws;
  shiftForLeftBarrelStraws = m_shiftForLeftBarrelStraws;
  shiftForRightBarrelStraws = m_shiftForRightBarrelStraws;
  shiftForRightEndCapStraws = m_shiftForRightEndCapStraws;
  numberOfEndCapPhiSectors = m_numberOfEndCapPhiSectors;
  numberOfIdentifierSectors = m_numberOfIdentifierSectors;
} 
  
