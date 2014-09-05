/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTK_RAWDATA_FTK_RAWPIXELCLUSTER_H
#define TRIGFTK_RAWDATA_FTK_RAWPIXELCLUSTER_H

#include <stdint.h>
#include "CLIDSvc/CLASS_DEF.h"

class FTK_RawPixelCluster {
private:
  
  uint32_t m_word_a;
  uint32_t m_word_b;
  int m_layer;
  signed long m_barcode;

public:
  FTK_RawPixelCluster(); 
  FTK_RawPixelCluster(int); 
  FTK_RawPixelCluster( uint32_t,  uint32_t, int);
  virtual ~FTK_RawPixelCluster();

  void setWordA(uint32_t word_a){ m_word_a = word_a;}
  void setWordB(uint32_t word_b){ m_word_b = word_b;}
  void setLayer(int layer){ m_layer = layer;}
  void setColWidth(unsigned int col_width);
  void setRowWidth(unsigned int row_width);
  void setColCoord(float col_coord);
  void setRowCoord(float row_coord);
  void setBarcode(signed long barcode){ m_barcode = barcode;}
  void setModuleID(unsigned int module_id);

  uint32_t    getWordA() const { return m_word_a;}
  uint32_t    getWordB() const { return m_word_b;}
  int         getLayer() const { return m_layer;}
  int         getColWidth() const;
  int         getRowWidth() const;
  float       getColCoord() const;
  float       getRowCoord() const;
  signed long getBarcode() const {return m_barcode;}
  unsigned int getModuleID() const;
};

CLASS_DEF( FTK_RawPixelCluster , 121694371 , 1 )

#endif // TRIGFTK_RAWDATA_FTK_RAWPIXELCLUSTER_H
