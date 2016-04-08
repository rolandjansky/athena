/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_SCTRDOFILLERTOOL_H
#define TRACKD3PDMAKER_SCTRDOFILLERTOOL_H
 
#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>
 

//#include "InDetRawData/SCT_RDO_Collection.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/SCT_RDORawData.h"
#include "InDetRawData/SCT3_RawData.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"

class SCT_ID;

namespace D3PD {



class SCTRDOFillerTool
  : public  D3PD::BlockFillerTool<SCT_RDORawData> 
  {
  public:
    
    typedef D3PD::BlockFillerTool<SCT_RDORawData> SCTRDOFillerBase;
    SCTRDOFillerTool (const std::string& type,
                     const std::string& name,
                     const IInterface* parent);
 
   virtual StatusCode initialize();
   virtual StatusCode book();
 
   virtual StatusCode fill (const SCT_RDORawData& p);
 
 private:
 
   void clearData();

   const SCT_ID* m_sctId;

 
   /* NTuple variables **/
   //int *m_nRDO;
   //float *m_timeNanoSec;
   //int *m_isOnTrack; 
   int *m_bec;
   int *m_layer;
   int *m_moduleEta;
   int *m_modulePhi;
   int *m_moduleSide;
   int *m_firstStrip;
   int *m_nStripsOnWafer;
   int *m_tbin;
 
  }; // class SCTRDOFillerTool
 
} // namespace D3PD
 
#endif // not TRACKD3PDMAKER_SCTRDOFILLERTOOL_H

