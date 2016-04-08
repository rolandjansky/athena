/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

namespace InDet {


  class PixelIDLayerComp{
  public:
    PixelIDLayerComp(): m_pixelId(0) {}
    PixelIDLayerComp(const PixelID* pixelId) : m_pixelId(pixelId) {}
    bool operator() (Identifier id1, Identifier id2){

      bool id1_is_pixel = m_pixelId->is_pixel(id1);
      bool id2_is_pixel = m_pixelId->is_pixel(id2);
      if(!id1_is_pixel && id2_is_pixel) return false;
      if(id1_is_pixel && !id2_is_pixel) return true;

      // FIX ME:
      // this situation isn't well defined, unfortunately.
      // But in principle we should never be in this situation, right?
      if(!id1_is_pixel && !id2_is_pixel){
	// ATH_MSG_WARNING("Passed a non-pixel ID into the pixel ID map!");
	return false;
      }

      if(!m_pixelId->is_pixel(id1) || !m_pixelId->is_pixel(id2))
	return (&id1 < &id2);
      
      bool id1_is_barrel = m_pixelId->is_barrel(id1);
      bool id2_is_barrel = m_pixelId->is_barrel(id2);
      if((id1_is_barrel && id2_is_barrel) ||
	 (!id1_is_barrel && !id2_is_barrel)){
	// they're both in the barrel or both in the endcap, 
	// so check the layer
	return (m_pixelId->layer_disk(id1) < m_pixelId->layer_disk(id2));
      }

      // one is in the barrel and one is in the endcap
      if(!id1_is_barrel && id2_is_barrel){
	return false;
      }
      
      //remaining possibility is if(id1_is_barrel && !id2_is_barrel), remove 'if' statement to make coverity happy, otherwise complains about logically dead code
      return true;
      
      
    }

    void SetPixelIDSvc(PixelID* pixelId) { m_pixelId = pixelId; }
  private:
    const PixelID * m_pixelId;
  };

}
