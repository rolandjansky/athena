/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// TRT_FillCablingData_SR1_ECC class - class used for filling
//      of TRT Mapping in case of SR1 Cosmics
//    P.T. Keener
//
#ifndef TRT_FILLCABLINGDATA_SR1_ECC_H
#define TRT_FILLCABLINGDATA_SR1_ECC_H


/*
 * Base class
 */
#include "AthenaBaseComps/AthAlgTool.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/IdContext.h"

#include "TRT_CablingData.h"
#include "TRT_ReadoutGeometry/TRT_DetectorManager.h" 
#include "InDetIdentifier/TRT_ID.h"

#include "eformat/SourceIdentifier.h"  // change to new eformat v3

#include "TRT_FillCablingData_Types.h"

#include <map>
#include <vector>
#include <stdint.h> 



class TRT_FillCablingData_SR1_ECC: public AthAlgTool {

public:

    // Constructor
 TRT_FillCablingData_SR1_ECC(const std::string& type, const std::string& name,
        const IInterface* parent);

    // Destructor	
  virtual ~TRT_FillCablingData_SR1_ECC();

    //AlgTool InterfaceID 
  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();

  virtual StatusCode finalize();

    // Fill the data
  TRT_CablingData* fillData();

    //Define map of collection IDs for all ROBs
  void defineCollID();

    // Get ROBIDs for each Detector Element (Straw Layer)
  std::vector<uint32_t> getRobID(Identifier id) const;

  void fillCollID(uint32_t rob_id, std::vector<IdentifierHash> & ids);

private:

  const TRT_ID* m_TRTHelper;
  IdContext m_cntx;

  TRT_CablingData* m_cabling = nullptr;

    // Define parameters
  void defineParameters();

    // Fill Tables with IDs for all straws
  void defineTables();

  int m_numberOfRings = 0;
  int m_numberOfLayersA = 0;
  int m_numberOfLayersB = 0;
  int m_numberOfLayersC = 0;
  //  int m_numberOfStrawsInBarrelROD;
  //  int m_shiftForRightBarrelStraws;
  int m_numberOfIdentifierSectors = 0;


    // Source ID for all robs
  //  std::vector<uint32_t> m_allRobs;

    // Vector for the mapping from phi to ROB Source ID
  std::map<int, std::vector<uint32_t> > m_phi_to_source;

    // Identifiers for all straws;
    //   Make 2D because IDs are different depending on which ROD we are
  std::vector< std::vector<Identifier> > m_identfierForAllStraws;
  //  std::vector<Identifier> m_identfierForAllStraws;
  
    // Hashed Identifiers for all straws;
  std::vector< std::vector<IdentifierHash> > m_identfierHashForAllStraws;  
  
    // Identifiers for all Collections for RODs 
  std::vector<std::vector<IdentifierHash> *> m_collID;

  // Private version of number of straws in each layer
  std::vector< int > m_numberOfStrawsInLayers;

  // buffer offset to straw maps per ROD connector
  std::map< int, int > m_1BS_1BL_map;
  std::map< int, int > m_2BS_2BL1_map;
  std::map< int, int > m_2BL2_3BL1_map;
  std::map< int, int > m_3BL2_map;
  std::map< int, int > m_3BS_map;

  std::map< int, int > m_1FS_1FL_map;
  std::map< int, int > m_2FS_2FL1_map;
  std::map< int, int > m_2FL2_3FL1_map;
  std::map< int, int > m_3FL2_map;
  std::map< int, int > m_3FS_map;

  std::map< int, int > m_ec_1l_map;
  std::map< int, int > m_ec_2l_map;
  std::map< int, int > m_ec_3l_map;

  std::map< int, int > m_ec_1r_map;
  std::map< int, int > m_ec_2r_map;
  std::map< int, int > m_ec_3r_map;

  //  int m_NumberOfMapFiles;


  GlobalCableMap m_CableMap;

};

#endif     //TRT_FILLCABLINGDATA_SR1_H
