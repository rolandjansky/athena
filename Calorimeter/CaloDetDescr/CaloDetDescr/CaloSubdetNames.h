/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloSubdetNames.h
 *
 * @brief CaloSubdetNames declaration
 *
 * @author Claire Adam-Bourdarios
 *
 * $Id: CaloSubdetNames.h,v 1.4 2007-07-08 18:43:07 tsulaia Exp $
 */

#ifndef CALODETDESCR_CALOSUBDETNAMES_H
# define CALODETDESCR_CALOSUBDETNAMES_H


#include <vector>
#include "Identifier/Identifier.h"

/** @class CaloSubdetNames

   @brief This class contains the ENUM needed to label the calorimeter pieces
   which be mooved independently, including the Cryostats, and mapping
   with the corresponding identifiers and strings when possible.

   Claire Adam-Bourdarios, sept 2005
  
*/
class CaloSubdetNames
{
 public:
  /** @brief LAr Alignable volumes
   */
  enum ALIGNVOL { LARCRYO_B = 0, LARCRYO_EC_POS = 1, LARCRYO_EC_NEG = 2,
		  PRESAMPLER_B_POS = 3, PRESAMPLER_B_NEG = 4,
		  EMB_POS = 5, EMB_NEG = 6,
		  PRESAMPLER_EC_POS = 7, PRESAMPLER_EC_NEG = 8,
		  EMEC_POS = 9, EMEC_NEG = 10, 
		  HEC1_POS = 11, HEC1_NEG = 12, 
		  HEC2_POS = 13, HEC2_NEG = 14,
		  FCAL1_POS = 15, FCAL1_NEG = 16, 
		  FCAL2_POS = 17, FCAL2_NEG = 18,
		  FCAL3_POS = 19, FCAL3_NEG = 20,
		  SOLENOID = 21, UNKNOWN = 22 };
  /** @brief standard constructor
   */
  CaloSubdetNames();
  
  /** @brief destructor
   */
  virtual ~CaloSubdetNames();

  /** @brief Matching between names and enum
   */  
  std::string alignVolName(CaloSubdetNames::ALIGNVOL alvol);
  /** @brief Matching between names and enum
   */  
  CaloSubdetNames::ALIGNVOL alignVolEnum(std::string reconame);
 

 private:
  /** @brief match names and enum
   */
  void match_names();

  /** @brief vector of names
   */
  std::vector <std::string>                    m_reconame;
  /** @brief vector of ALIGNVOL enum values
   */ 
  std::vector <CaloSubdetNames::ALIGNVOL>  m_alvol;
};

#endif // CALODETDESCR_CALOSUBDETNAMES_H
