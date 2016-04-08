/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_PIXELRDOFILLERTOOL_H
#define TRACKD3PDMAKER_PIXELRDOFILLERTOOL_H
 
#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>
 

//#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/PixelRDORawData.h"
//#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/Pixel1RawData.h"  //method 1?

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

class PixelID;

namespace D3PD {



class PixelRDOFillerTool
  : public  D3PD::BlockFillerTool<PixelRDORawData> 
  {
  public:
    
    typedef D3PD::BlockFillerTool<PixelRDORawData> PixelRDOFillerBase;
    PixelRDOFillerTool (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);
 
   virtual StatusCode initialize();
   virtual StatusCode book();
 
   virtual StatusCode fill (const PixelRDORawData& p);
 
 private:
 
   void clearData();

   const PixelID* m_pixelId;

 
   /* NTuple variables **/
   //int *m_nRDO;
   //float *m_timeNanoSec;
   //int *m_isOnTrack; 
   int *m_bec;
   int *m_layer;
   int *m_moduleEta;
   int *m_modulePhi;

   int *m_ToT;
   //int *m_moduleSide;
   //int *m_firstStrip;
   //int *m_nStripsOnWafer;
   //int *m_tbin;
 
  }; // class PixelRDOFillerTool
 
} // namespace D3PD
 
#endif // not TRACKD3PDMAKER_PIXELRDOFILLERTOOL_H

