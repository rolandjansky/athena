/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECALIBBLOBOBJS_TILECALIBDRAWERFLT_H
#define TILECALIBBLOBOBJS_TILECALIBDRAWERFLT_H

/** 
    @brief Generic class for storing a number of floats (Flt) for each channel or ADC
    @author Nils Gollub <nils.gollub@cern.ch>
    
    @details This class implements the infrastructure to store a
    (variable) number of floats for each channel or ADC. The number of
    floats to store is encoded in the objSize header variable. The
    object version header variable is used to encode different
    functional forms on the stored data, accessed through the getCalib
    function.
*/

#include "TileCalibBlobObjs/TileCalibDrawerDat.h"
#include "TileCalibBlobObjs/TileCalibType.h"
#include <stdint.h>
#include <vector>
#include "CoralBase/Blob.h"

class TileCalibDrawerFlt : public TileCalibDrawerDat<float>
{
 public:

  /** @brief Dtor */
  virtual ~TileCalibDrawerFlt(){}
  
  /** @brief Returns TileCalibType::FLT. */
  virtual uint16_t getType() const {return TileCalibType::FLT;} 
  
  /** @brief Returns a pointer to a const TileCalibDrawerFlt. */
  static const TileCalibDrawerFlt* getInstance(const coral::Blob& blob);
  
  /** @brief Returns a pointer to a non-const TileCalibDrawerFlt. 
      @param blob Reference to a coral::Blob
      @param def A reference to a DefType object, specifying the inital layout and values
      @param nChans Number of channels to create
      @param objType Type of the (derived class) object
      @param objVers Version of the (derived class) object
      @param author String containing the author user id
      @param comment String containing the actual comment
      @param timeStamp Unix time stamp (if 0, current time is used) */
  static TileCalibDrawerFlt* getInstance(coral::Blob&        blob,
					 const DefType&      def, 
					 uint16_t            nChans,
					 uint16_t            objVers,
					 const std::string&  author="",
					 const std::string&  comment="",
					 uint64_t            timeStamp=0);
  
  /** @brief Returns the calibrated energy for a given channel/ADC and input energy
      @param channel The channel number; If channel number >= getNChans() 
                     it is reset to channel % (maximum number of channels in drawer) 
                     if channel number > (maximum number of channels in drawer)
                     otherwise it is reset to 0 without warning (default policy)   
      @param adc The gain index; if >= getNGains() it is reset to 0 without warning (default policy)   
      @param energy The energy that should be calibrated 
      @param invert If true, the calibration is inverted */
  float getCalib(unsigned int channel, unsigned int adc, float energy, bool invert=false) const;

  /** @brief Returns y and derivative dy for a given x for function blob of type 200.
      @return A bool indicating whether x was within the function range (true) or outside (false)
      @param channel The channel number; If channel number >= getNChans() 
                     it is reset to channel % (maximum number of channels in drawer) 
                     if channel number > (maximum number of channels in drawer)
                     otherwise it is reset to 0 without warning (default policy)   
      @param adc The gain index; if >= getNGains() it is reset to 0 without warning (default policy)   
      @param x Input x value 
      @param y Y value corresponding to x
      @param dy Derivative of the function at x */
  bool getYDY(unsigned int channel, unsigned int adc, float x, float& y, float& dy) const;

  /** @brief Returns y for a given x for function blob of type 200.
      @details This function is created as a helper function for 
      TileCalibDrawer::getYDY and for python use.
      (Python does not like float references) */
  float getY( unsigned int channel, unsigned int adc, float x) const;

  /** @brief Returns y for a given x for function blob of type 200.
      @details This function is inefficient and created for python use only.
      (Python does not like float references) */
  float getDY(unsigned int channel, unsigned int adc, float x) const;


 protected:
  /** @brief Ctor (const). */
  TileCalibDrawerFlt(const coral::Blob& blob);
  /** @brief Ctor (non-const). */
  TileCalibDrawerFlt(coral::Blob& blob);
};


#endif
