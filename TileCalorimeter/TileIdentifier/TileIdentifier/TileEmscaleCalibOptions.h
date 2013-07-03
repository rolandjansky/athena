/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEIDENTIFIER_TILEEMSCALECALIBOPTIONS_H
#define TILEIDENTIFIER_TILEEMSCALECALIBOPTIONS_H

/**
    @brief Class for encoding Tile calibration options into a bit map
    @author Lukas Pribyl <lukas.pribyl@cern.ch>

    @details In this class one can specify, whether to use the conditions
    data of a given proxy for EM scale calibration or whether to skip the 
    calibration step including this proxy. The resulting bitmap, stored
    as uint32_t is to be used by TileConditions/TileExpertToolEmscale,
    which calibrates the Tilecal ADC amplitude, based on the options,
    stored in the bitmap.
*/

#include <iostream>
#include <vector>
#include <stdint.h>

class TileEmscaleCalibOptions{
 public:

  enum CalBitPos{
    OflCisFitLin,  
    OflCisFitNln,  
    OflLasLin,     
    OflLasNln,     
    OflLasPart,    
    OflCes,        
    OflEms,        
    OflLasLinPisa, 
    OflLasNlnPisa, 
    OflMinbias,    
    OnlCisLin,     
    OnlLasLin,     
    OnlCes,        
    OnlEms,
    MaxBit
  };

  /** @brief Ctor. By default all options are off */
  TileEmscaleCalibOptions();

  /** @brief Ctor. m_options are set to options */
  TileEmscaleCalibOptions(uint32_t options);

  /** @brief Dtor. */
  ~TileEmscaleCalibOptions() {}

  /** @brief Returns the complete bitmap with all EM scale calibration options */
  uint32_t getEmscaleCalibOptions() const;

  /** @brief Dumps the member EM scale calibration options in a human readable form */
  void dumpEmscaleCalibOptions(std::ostringstream& stream) const;

  /** @brief   Returns a bit for a given calibration option 
      @details The value "true" means that the given calibration will be applied
      by the TileConditions/TileExpertToolEmscale. If the value is "false", 
      the calibration step will be skipped
  */
  bool getEmscaleCalibBit(CalBitPos idx) const;  
  /** @brief Sets a given EM scale calibration option to the value given by turnOn */
  void setEmscaleCalibBit(CalBitPos idx, bool turnOn);  

 private:
  uint32_t m_options;
  std::vector<std::string> m_bitToProb;

  void initBitToProb();

};

//
//_____________________________________________________
inline uint32_t
TileEmscaleCalibOptions::getEmscaleCalibOptions() const
{
  return m_options;
}

//
//_____________________________________________________
inline bool 
TileEmscaleCalibOptions::getEmscaleCalibBit(CalBitPos idx) const
{
  if(m_options & (1<<idx)) return true;
  else return false;
}

#endif //TILEIDENTIFIER_TILEEMSCALECALIBOPTIONS_H


