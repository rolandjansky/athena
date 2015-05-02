/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_LutCam
#define TrigConf_LutCam

#include <string>
#include <inttypes.h>
#include <sys/types.h>
#include <vector>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {
  
   class LutCam : public L1DataBaseclass {
      
   public:

      static const u_int ALL_LUT_SIZE = 49664; // the source of these numbers is the CtpcoreModule.cc (CtpcoreModule::LUT_ADDR_SIZE)
      static const u_int ALL_CAM_SIZE =  8192;
      
      LutCam();
      ~LutCam();

      // Accessors

      // return copy of vector
      const std::vector<uint32_t> & lut() const;
      void setLut( const u_int l[], const int size);

      // return copy of vector
      const std::vector<uint32_t> & cam() const;
      void setCam( const u_int c[], const int size );

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      
   private:
      std::vector<uint32_t> m_lut;
      std::vector<uint32_t> m_cam;
   };
}
#endif 
  
