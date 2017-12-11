/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// TRT_FillCablingData_TB04 class - class used for filling
//      of TRT Mapping in case of TB 2004
//
#ifndef TRT_FILLCABLINGDATA_TB04_H
#define TRT_FILLCABLINGDATA_TB04_H


/*
 * Base class
 */
#include "AthenaBaseComps/AthAlgTool.h"


#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/IdContext.h"

#include "TRT_CablingData.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h" 
#include "InDetIdentifier/TRT_ID.h"

#include "eformat/SourceIdentifier.h"  // change to new eformat v3


#include <vector>
#include <stdint.h> 


class TRT_FillCablingData_TB04: public AthAlgTool {

public:

    // Constructor
 TRT_FillCablingData_TB04(const std::string& type, const std::string& name,
        const IInterface* parent);

    // Destructor	
  virtual ~TRT_FillCablingData_TB04();

    //AlgTool InterfaceID 
  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();

  virtual StatusCode finalize();

    // Fill the data
  TRT_CablingData* fillData();

    //Define map of collection IDs for all ROBs
  void defineCollID();

    // Get ROBIDs for each Detector Element (Straw Layer)
  std::vector<uint32_t> getRobID(Identifier id);

  void fillCollID(uint32_t rob_id, std::vector<IdentifierHash> & ids);

private:

  const TRT_ID* m_TRTHelper;

  IdContext m_cntx;

  TRT_CablingData* m_cabling;

    // Define parameters
  void defineParameters();

    // Fill Tables with IDs for all straws
  void defineTables();

  int m_numberOfRings;
  int m_numberOfLayersA;
  int m_numberOfLayersB;
  int m_numberOfLayersC;
  //  int m_numberOfStrawsInBarrelROD;
  //  int m_shiftForRightBarrelStraws;
  int m_numberOfIdentifierSectors;

    // Source ID for all robs
  //  std::vector<uint32_t> m_allRobs;

    // Vector for the mapping from phi to ROB Source ID
  std::vector<uint32_t> m_phi_to_source;

    // Identifiers for all straws;
    //   Make 2D because IDs are different depending on which ROD we are
  std::vector< std::vector<Identifier> > m_identfierForAllStraws;
  //  std::vector<Identifier> m_identfierForAllStraws;
  
    // Hashed Identifiers for all straws;
  std::vector< std::vector<IdentifierHash> > m_identfierHashForAllStraws;  
  
    // Identifiers for all Collections for RODs 
  std::vector<std::vector<IdentifierHash> *> m_collID;

  // Private version of number of straws in each layer
  std::vector< std::vector<int> > m_ncol;

  int m_StrawsByModule[3];

};

#endif     //TRT_FILLCABLINGDATA_TB04_H
