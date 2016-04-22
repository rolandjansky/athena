// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdentifier/LArEM_SuperCell_ID.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Helper class for LArEM offline identifiers for supercells
 */


#ifndef CALOIDENTIFIER_LAREM_SUPERCELL_ID_H
#define CALOIDENTIFIER_LAREM_SUPERCELL_ID_H

#include "CLIDSvc/CLASS_DEF.h"
#include "CaloIdentifier/LArEM_Base_ID.h"
#include "SGTools/BaseInfo.h"


/**
*
* @class LArEM_SuperCell_ID
* @brief Helper class for LArEM offline identifiers for supercells
*  
*  This class provides an interface to decode and generate offline identifiers
*  for the Supercell version of the LAr EM detector subsystem.  <p>
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
* "                0,[2,5]          endcap outer wheel sampling 1
* "                [0,1]            endcap inner wheel sampling 1
* "                [0,1]            endcap outer wheel sampling 2
* "                [0,1]            endcap inner wheel sampling 2
* "                0                endcap outer wheel sampling 3
* 
* 
* eta for barrel   [0,15]         presampler - 0< eta <1.52 - deta is approximately equal to 0.1
* "                [0,55]         sampling 1 region 0  0   < eta < 1.4   - deta = 0.025
* "                [0,2]          sampling 1 region 1  1.4 < eta < 1.475 - deta = 0.025
* "                [0,55]         sampling 2 region 0  0   < eta < 1.4   - deta = 0.025 
* "                0              sampling 2 region 1  1.4 < eta < 1.475 - deta = 0.075
* "                [0,13]         sampling 3 region 0  0   < eta < 1.35  - deta = 0.1
* "
* phi for barrel   [0,63]         barrel presampler   - dphi = 0.1
* "                [0,63]         sampling 1 region 0 - dphi = 0.1
* "                [0,63]         sampling 1 region 1 - dphi = 0.1
* "                [0,63]         sampling 2 region 0 - dphi = 0.1
* "                [0,63]         sampling 2 region 1 - dphi = 0.1
* "                [0,63]         sampling 3 region 0 - dphi = 0.1
* 
* number of cells in barrel :
* presampler :  2048
* sampling 1 :  7552
* sampling 2 :  7296
* sampling 3 :  1792
* total      : 18688
* 
* eta for endcap   [0,2]       presampler  sampling 0 region 0  1.5   < eta < 1.8   - deta = 0.1
* "                0           outer wheel sampling 1 region 0  1.375 < eta < 1.425 - deta = 0.125
* "                [0,11]      outer wheel sampling 1 region 2  1.5   < eta < 1.8   - deta = 0.025
* "                [0,11]      outer wheel sampling 1 region 3  1.8   < eta < 2.0   - deta = 0.1667
* "                [0,15]      outer wheel sampling 1 region 4  2.0   < eta < 2.4   - deta = 0.025
* "                0           outer wheel sampling 1 region 5  2.4   < eta < 2.5   - deta = 0.1
* "                [0,2]       inner wheel sampling 1 region 0  2.5   < eta < 3.1   - deta = 0.2
* "                0           inner wheel sampling 1 region 0  3.1   < eta < 3.2   - deta = 0.1
* "                0           outer wheel sampling 2 region 0  1.375 < eta < 1.425 - deta = 0.05
* "                [0,42]      outer wheel sampling 2 region 1  1.425 < eta < 2.5   - deta = 0.025
* "                [0,2]       inner wheel sampling 2 region 0  2.5   < eta < 3.1   - deta = 0.2
* "                0           inner wheel sampling 2 region 0  3.1   < eta < 3.2   - deta = 0.1
* "                [0,9]       outer wheel sampling 3 region 0  1.5   < eta < 2.5   - deta = 0.1
* 
* phi for endcap   [0,63]      presampler  sampling 0 region 0       - dphi = 0.1
* "                [0,63]      outer wheel sampling 1 regions [0,5]  - dphi = 0.1 
* "                [0,31]      inner wheel sampling 1 region  0      - dphi = 0.2
* "                [0,63]      outer wheel sampling 2 regions [0,1]  - dphi = 0.1
* "                [0,31]      inner wheel sampling 2 region 0       - dphi = 0.2
* "                [0,63]      outer wheel sampling 3 region 0       - dphi = 0.1
* 
* number of cells in endcap :
* presampler  :   384
* Outer wheel:
* sampling 1  :  4864
* sampling 2  :  5632
* sampling 3  :  1280
* total       : 11776
* Inner wheel:
* sampling 1  :   256
* sampling 2  :   256
* total       :   512
* 
* Grand Total : 12672
*
* @author RD Schaffer, Denis Oliveira Damazio, scott snyder
*/



class LArEM_SuperCell_ID : public LArEM_Base_ID
{
public:        
  LArEM_SuperCell_ID(void);    
  ~LArEM_SuperCell_ID(void);

  /** initialization from the identifier dictionary*/
  virtual int  initialize_from_dictionary (const IdDictMgr& dict_mgr);
};

//using the macro below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF( LArEM_SuperCell_ID , 99488227 , 1 )
SG_BASE (LArEM_SuperCell_ID, LArEM_Base_ID);


#endif // CALOIDENTIFIER_LAREM_SUPERCELL_ID_H
