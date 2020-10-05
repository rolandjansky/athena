/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// TRT_FillCablingData_DC3 class - class used for filling
//      of TRT Mapping in case of full ATLAS with the ROD05
//    P.T. Keener
//
#ifndef TRT_FILLCABLINGDATA_DC3_H
#define TRT_FILLCABLINGDATA_DC3_H

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

#include <map>
#include <set>
#include <vector>
#include <stdint.h> 

class TRT_FillCablingData_DC3: public AthAlgTool {

 public:

   // Constructor
   TRT_FillCablingData_DC3(const std::string& type, const std::string& name,
			       const IInterface* parent);

   // Destructor	
   virtual ~TRT_FillCablingData_DC3();

   //AlgTool InterfaceID 
   static const InterfaceID& interfaceID();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   // Fill the data
   TRT_CablingData* fillData();

   // Get ROBIDs for each Detector Element (Straw Layer)
   std::vector<uint32_t> getRobID(Identifier id);

 private:
   //Define map of collection IDs for all ROBs
   void defineCollID();
   void defineCollID_broken_FDR();

   void fillCollID(uint32_t rob_id, std::vector<IdentifierHash> & ids);
   void fillCollID_broken_FDR(uint32_t rob_id, std::vector<IdentifierHash> & ids);


   const TRT_ID* m_TRTHelper;
   StringProperty m_inputFilename;

   bool m_PartialDetector;
   
   std::set< uint32_t > m_validSourceIds;

   IdContext m_cntx;

   TRT_CablingData* m_cabling;

   // Define parameters
   void defineParameters();

   // Fill Tables with IDs for all straws
   void defineTables();
   void defineTables_broken_FDR();

   bool m_broken_FDR_Cabling;
   bool m_RealData;

   int m_numberOfRings;
   int m_numberOfLayers[3];

   // Identifiers for all straws;
   //   Make 2D because IDs are different depending on which ROD we are
   std::vector< std::vector<Identifier> > m_identfierForAllStraws;
  
   // Hashed Identifiers for all straws;
   std::vector< std::vector<IdentifierHash> > m_identfierHashForAllStraws;  
  
   // Identifiers for all Collections for RODs 
   std::vector<std::vector<IdentifierHash> *> m_collID;

   // Private version of number of straws in each layer
   std::vector< int > m_numberOfStrawsInLayers;
   std::vector< int > m_StrawsPerLayer;

   // Wheel to ROD index for EC
   int m_StrawLayerToRod[160];
   
   // buffer offset to straw maps
   std::map< int, int > m_barrel_a_map;
   std::map< int, int > m_barrel_c_map;

   std::map< int, int >  m_eca_l0_map;
   std::map< int, int >  m_eca_l1_map;
   std::map< int, int >  m_eca_r0_map;
   std::map< int, int >  m_eca_r1_map;

   std::map< int, int >  m_ecc_l0_map;
   std::map< int, int >  m_ecc_l1_map;
   std::map< int, int >  m_ecc_r0_map;
   std::map< int, int >  m_ecc_r1_map;

};

#endif     //TRT_FILLCABLINGDATA_DC3_H
