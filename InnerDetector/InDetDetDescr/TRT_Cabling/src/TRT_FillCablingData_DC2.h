/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// TRT_FillCablingData_DC2 class - class used for filling
//      of TRT Mapping in case of DC2
//    A.Zalite
//
#ifndef TRT_FILLCABLINGDATA_DC2_H
#define TRT_FILLCABLINGDATA_DC2_H

/*
 * Base class
 */
#include "AthenaBaseComps/AthAlgTool.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/IdContext.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_CablingData.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "eformat/SourceIdentifier.h"  // change to new eformat v3
#include <stdint.h>
#include <vector>


class TRT_FillCablingData_DC2: public AthAlgTool
{
public:
    // Constructor   
  TRT_FillCablingData_DC2(const std::string& type, const std::string& name,
    const IInterface* parent);

    // Destructor	
  virtual ~TRT_FillCablingData_DC2();

    // AlgTool InterfaceID
  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();

  virtual StatusCode finalize();

    // Fill the data
  TRT_CablingData* fillData();

    // Get ROBIDs for each Detector Element (straw layer)
  std::vector<uint32_t> getRobID(Identifier& id) const;

    // Get ROD  for each straw
  uint32_t getRodID(Identifier& id) const;

    // Get Straw and ROD ID
  void getStrawRodID(Identifier& id, uint32_t& rod_id, uint32_t& straw_number)
    const;
  
    // Define Identifier for each straw from BS
  //Identifier defineIdentifier(const EventFormat::SubDetector& subdetector,
    //const int& rod, const int& straw) const; 
  Identifier defineIdentifier(const eformat::SubDetector& subdetector,
    const int& rod, const int& straw) const;

    // Get Identifiers of Collections for the given ROBID
  void fillCollID(uint32_t robid,  std::vector<IdentifierHash> & ids);

    // Get parameters
  void get_parameters(int& numberOfStrawsInROD,
    int& numberOfStrawsInBarrelROD,
    int& shiftForLeftEndCapStraws,
    int& shiftForLeftBarrelStraws,
    int& shiftForRightBarrelStraws,
    int& shiftForRightEndCapStraws,
    int& numberOfEndCapPhiSectors,
    int& numberOfIdentifierSectors);
  
private:

  void defineParameters();
  void printParameters() const;
  void defineTables();
  
    // Define Identifiers of Collections for the given ROBID
  void defineCollID();

  int m_numberOfWheelsA;
  int m_numberOfWheelsAB;
  int m_numberOfWheelsABC;
  int m_numberOfStrawsAInRod;
  int m_numberOfStrawsABInRod;
  int m_numberOfStrawsInWheelAForRod;
  int m_numberOfStrawsInWheelBForRod;
  int m_numberOfStrawsInWheelCForRod;

  int m_numberOfPlanesAB;
  int m_totalNumberOfPlanes;
  int m_numberOfPhiSectorsInIDSector;
  int m_numberOfStrawsInPhiSectorAB;
  int m_numberOfStrawsInPhiSectorC;
  int m_totalNumberOfStrawsInPhiSectorsAB;

  int m_numberOfRings;
  int m_numberOfStrawsA;
  int m_numberOfStrawsAB;
  int m_numberOfLayersA;
  int m_numberOfLayersB;
  int m_numberOfLayersC;
  int m_numberOfLayersAB;

  int m_numberOfLayersInPhiSector;
  int m_numberOfStrawsInPhiSector;

  int m_numberOfStrawsInROD;
  int m_numberOfStrawsInBarrelROD;
  
  int m_numberOfIdentifierSectors;
  int m_numberOfEndCapPhiSectors;
  
  int m_shiftForLeftEndCap;
  int m_shiftForLeftBarrel;
  int m_shiftForRightBarrel;
  int m_shiftForRightEndCap;
  
  int m_shiftForLeftEndCapStraws;
  int m_shiftForLeftBarrelStraws;
  int m_shiftForRightBarrelStraws;
  int m_shiftForRightEndCapStraws;  

  int m_numberOfStrawPlanesAC;
  int m_numberOfStrawPlanesB;

  int* m_layerID;
  int* m_strawIDInLayer;
  int* m_numberOfStrawsInPreviousLayers;

  const TRT_ID* m_id_trt;
  IdContext m_cntx;

  TRT_CablingData* m_cabling;
};

#endif  // TRT_FILLCABLINGDATA_DC2_H

