/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArRegionSelector_LArRegionSelectorTable_h
#define LArRegionSelector_LArRegionSelectorTable_h

#include "RegionSelector/IRegionLUT_Creator.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "Identifier/Identifier.h" 
#include "Identifier/HWIdentifier.h" 
#include "CaloIdentifier/LArID_Exception.h" 

#include <string>

class RegionSelectorLUT;
class CaloTTDescrManager;
class CaloLVL1_ID;
class LArRoI_Map;

/////////////////////////////////////////////////////////////////////////////

/**
 @brief This service contains code to generate the tables of element extents 
 and ROBids used by the region selector. 
 
 The code loops over each LAr TT
 and fills the class RegionSelectorLUT which includes for each element: <br>
 hash Id, <br>
 sampling (EM or had), <br>
 eta min, <br>
 eta max, <br>
 phi min, <br>
 phi max, <br>
 rob Ids  <br>

 Once filled, the tables are stored in StoreGate.

 @author
 Initial implementation (May 2004) by C. Bourdarios, making use of code fragments from LArRawUtils (H. Ma) <br>
 Major update (Feb. 2006) by F. Ledroit 
*/


class LArRegionSelectorTable : public AthAlgTool, virtual public IRegionLUT_Creator
 {

public:

   /** constructor */
  LArRegionSelectorTable (const std::string&, 
			  const std::string&,
			  const IInterface*);
  /** destructor */
  virtual ~LArRegionSelectorTable();


  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  /** returns the lookup table (RegionSelectorLUT  object)
      @param subSyst. there are 4 sub-systems: <br>
      "EM", "HEC", "FCALEM", "FCALHAD "
  */ 
  virtual const RegionSelectorLUT * getLUT(std::string subSyst) const;

private:

  void fillMaps(); 

  StatusCode recordMap(RegionSelectorLUT* larRSlut, std::string larKey); 


  /** Service  property: optionnaly print all lookup tables to ascii files.<br>
      Only used for debug purposes, as the tables are meant to be retrieved from StoreGate */
  bool m_printTable;

  const CaloTTDescrManager* m_ttman; 
  const CaloLVL1_ID*        m_TT_ID; 
  const LArRoI_Map*         m_roiMap;

  /** pointer to lookup table for sub-system EM */
  RegionSelectorLUT * m_emLUT;
  /** pointer to lookup table for sub-system HEC */
  RegionSelectorLUT * m_hecLUT;
  /** pointer to lookup table for sub-system FCALEM */
  RegionSelectorLUT * m_fcalemLUT;
  /** pointer to lookup table for sub-system FCALHAD */
  RegionSelectorLUT * m_fcalhadLUT;

  /** Name of ascii file containing EM layer 0*/
  std::string m_roiFileNameEM_0;
  /** Name of ascii file containing EM layer 1*/
  std::string m_roiFileNameEM_1;
  /** Name of ascii file containing EM layer 2*/
  std::string m_roiFileNameEM_2;
  /** Name of ascii file containing EM layer 3*/
  std::string m_roiFileNameEM_3;
  /** Name of ascii file containing HEC layer 0*/
  std::string m_roiFileNameHEC_0;
  /** Name of ascii file containing HEC layer 1*/
  std::string m_roiFileNameHEC_1;
  /** Name of ascii file containingHEC layer 2*/
  std::string m_roiFileNameHEC_2;
  /** Name of ascii file containing HEC layer 3*/
  std::string m_roiFileNameHEC_3;
  /** Name of ascii file containing FCAL module 1 (EM layer)*/
  std::string m_roiFileNameFCALem_0;
  /** Name of ascii file containing FCAL module 2 (1st HAD layer)*/
  std::string m_roiFileNameFCALhad_0;
  /** Name of ascii file containing FCAL module 3 (2nd HAD layer)*/
  std::string m_roiFileNameFCALhad_1;

};

#endif // LArRegionSelector_LArRegionSelectorTable_h
