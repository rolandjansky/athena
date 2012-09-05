/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCONDBLOBOBJS_CALOCONDUTILS_H
#define CALOCONDBLOBOBJS_CALOCONDUTILS_H

/** 
    @brief Static class providing utility functions.
    @author Nils Gollub <nils.gollub@cern.ch>
*/

#include <string>
#include <map>

class CaloCondUtils{
 public:

  enum SYSTEM{EMECZNEG = 0,
	      EMBZNEG  = 1,
	      EMBZPOS  = 2,
	      EMECZPOS = 3,
	      HEC      =16,
	      FCAL     =32,
	      TILE     =48};
  
  /** @brief Returns the cool channel id associated with a given system.
      @param sys The SYSTEM id */
  static unsigned int getCoolChannelId(const SYSTEM sys);
  
  /** @brief Returns the number of cells for a given system.
      @param sys The SYSTEM id */
  static unsigned int getNCells(const SYSTEM sys);
  
  /** @brief Returns the full tag string, composed of camelized folder name and tag part.
      @details The tag name conventions are outline at
               https://twiki.cern.ch/twiki/bin/view/Atlas/CoolTagging.
      @param folder The full COOL folder path capitalized, eg. "/CALO/V01/NOISE"
      @param tag The tag part, eg. "XX-YYY" */
  static std::string getFullTag(const std::string& folder, const std::string& tag);

  /** @brief Returns the non-negative gainId to be used with the COOL DB. 
      @details The CaloGain enumertor is negative for Tile Cells. Negative gains
      can not be used as an index. This function provides a non-negative index for each 
      CaloGain
      @param caloGain Input caloGain */
  static unsigned int getDbCaloGain(int caloGain);

  //private:
  //static std::map<SYSTEM,unsigned int> m_nCells;
};
#endif
