/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGFTK_RAWDATA_FTK_RAWSCT_CLUSTER_H
#define TRIGFTK_RAWDATA_FTK_RAWSCT_CLUSTER_H

#include <stdint.h>
#include "CLIDSvc/CLASS_DEF.h"

class FTK_RawSCT_Cluster {
private:
  uint32_t    m_word;
  int         m_layer; 
  signed long m_barcode;

public:

  FTK_RawSCT_Cluster(); 
  FTK_RawSCT_Cluster(int); 
  FTK_RawSCT_Cluster(uint32_t, signed long, int);
  virtual ~FTK_RawSCT_Cluster();

  void setBarcode(signed long barcode){ m_barcode = barcode;}
  void setLayer(int layer){ m_layer = layer;}
  void setWord(uint32_t word){ m_word = word;}
  void setHitCoord(float hit_coord);
  void setHitWidth(unsigned int hit_width);
  void setModuleID(unsigned int module_id);

  uint32_t     getWord()     const { return m_word;}
  int          getLayer()    const { return m_layer;}
  signed long  getBarcode()  const { return m_barcode;}
  float        getHitCoord() const;
  unsigned int getHitWidth() const;
  unsigned int getModuleID() const;
};

CLASS_DEF( FTK_RawSCT_Cluster , 178057777 , 1 )

#endif // TRIGFTK_RAWDATA_FTK_RAWSCT_CLUSTER_H
