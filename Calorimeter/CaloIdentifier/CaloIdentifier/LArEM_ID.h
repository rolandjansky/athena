/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LAREM_ID_H
#define LAREM_ID_H

#include "CLIDSvc/CLASS_DEF.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/IdentifierHash.h"
#include "IdDict/IdDictFieldImplementation.h"
#include "IdDict/IdDictDefs.h"

#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArEM_Base_ID.h"
#include "SGTools/BaseInfo.h"

#include <vector>
#include <algorithm>

/**
*
* @class LArEM_ID
* @brief Helper class for LArEM offline identifiers
*  
*  This class provides an interface to decode and generate offline identifiers
*  for the LAr EM detector subsystem.  <p>
* 
* Definition and range of values for the elements of the identifier are: <p>
* <pre>
*             Connected channels :
*             ------------------
* element           range              meaning
* -------           -----              -------
* 
* barrel_ec        +/-1             positive/negative barrel - A/C side or P/M half barrel
* "                +/-2             positive/negative endcap outer wheel - A/C side 
* "                +/-3             positive/negative endcap inner wheel - A/C side
* 
* sampling         0                both presamplers
* "                [1,3]            barrel and endcap outer wheel 
* "                [1,2]            endcap inner wheel
* 
* region           0                both presamplers
* "                [0,1]            barrel sampling 1 and 2 
* "                0                barrel sampling 3
* "
* "                [0,5]            endcap outer wheel sampling 1
* "                0                endcap inner wheel sampling 1
* "                [0,1]            endcap outer wheel sampling 2
* "                0                endcap inner wheel sampling 2
* "                0                endcap outer wheel sampling 3
* 
* 
* eta for barrel   [0,60]         presampler - 0< eta <1.52 - deta is approximately equal to 0.025
* "                [0,447]        sampling 1 region 0  0   < eta < 1.4   - deta = 0.025/8 
* "                [0,2]          sampling 1 region 1  1.4 < eta < 1.475 - deta = 0.025
* "                [0,55]         sampling 2 region 0  0   < eta < 1.4   - deta = 0.025 
* "                0              sampling 2 region 1  1.4 < eta < 1.475 - deta = 0.075
* "                [0,26]         sampling 3 region 0  0   < eta < 1.35  - deta = 0.050 
* "
* phi for barrel   [0,63]         barrel presampler   - dphi = 0.1
* "                [0,63]         sampling 1 region 0 - dphi = 0.1
* "                [0,255]        sampling 1 region 1 - dphi = 0.025
* "                [0,255]        sampling 2 region 0 - dphi = 0.025
* "                [0,255]        sampling 2 region 1 - dphi = 0.025
* "                [0,255]        sampling 3 region 0 - dphi = 0.025
* 
* number of cells in barrel :
* presampler :  7808
* sampling 1 : 58752
* sampling 2 : 29184
* sampling 3 : 13824
* total      :109568
* 
* eta for endcap   [0,11]      presampler  sampling 0 region 0  1.5   < eta < 1.8   - deta = 0.025
* "                0           outer wheel sampling 1 region 0  1.375 < eta < 1.425 - deta = 0.05  
* "                [0,2]       outer wheel sampling 1 region 1  1.425 < eta < 1.5   - deta = 0.025
* "                [0,95]      outer wheel sampling 1 region 2  1.5   < eta < 1.8   - deta = 0.025/8
* "                [0,47]      outer wheel sampling 1 region 3  1.8   < eta < 2.0   - deta = 0.025/6
* "                [0,63]      outer wheel sampling 1 region 4  2.0   < eta < 2.4   - deta = 0.025/4 
* "                [0,3]       outer wheel sampling 1 region 5  2.4   < eta < 2.5   - deta = 0.025
* "                [0,6]       inner wheel sampling 1 region 0  2.5   < eta < 3.2   - deta = 0.1
* "                0           outer wheel sampling 2 region 0  1.375 < eta < 1.425 - deta = 0.05
* "                [0,42]      outer wheel sampling 2 region 1  1.425 < eta < 2.5   - deta = 0.025
* "                [0,6]       inner wheel sampling 2 region 0  2.5   < eta < 3.2   - deta = 0.1
* "                [0,19]      outer wheel sampling 3 region 0  1.5   < eta < 2.5   - deta = 0.05 
* 
* phi for endcap   [0,63]      presampler  sampling 0 region 0       - dphi = 0.1
* "                [0,63]      outer wheel sampling 1 regions [0,5]  - dphi = 0.1 
* "                [0,63]      inner wheel sampling 1 region  0      - dphi = 0.1 
* "                [0,255]     outer wheel sampling 2 regions [0,1]  - dphi = 0.025
* "                [0,63]      inner wheel sampling 2 region 0       - dphi = 0.1
* "                [0,255]     outer wheel sampling 3 region 0       - dphi = 0.025
* 
* number of cells in endcap :
* presampler  :  1536
* Outer wheel:
* sampling 1  : 27648
* sampling 2  : 22528
* sampling 3  : 10240
* total       : 60416
* Inner wheel:
* sampling 1  :   896
* sampling 2  :   896
* total       :  1792
* 
* Grand Total : 63744
*
* </pre>
* @author started by RD Schaffer
* @author maintained by Fabienne Ledroit
*/

class Range;

class LArEM_ID : public LArEM_Base_ID
{
public:        

  typedef Identifier::size_type  size_type ;

  LArEM_ID(void);    
  ~LArEM_ID(void);


  
  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);
};

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( LArEM_ID , 163583365 , 1 )
SG_BASE (LArEM_ID, LArEM_Base_ID);


#endif // LAREM_ID_H
