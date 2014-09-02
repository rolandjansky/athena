/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SaddleBrepsBuilder_H
#define SaddleBrepsBuilder_H


class StoreGateSvc;
class IRDBAccessSvc;
class GeoPhysVol;
class IRDBRecordset;
class IMessageSvc;

#include <string>

namespace MuonGM {
// General Constants
//  const int maxDim = 9;
//    const int nFeet = 9;
//      const int nVertex = 9;
//        const double ANGIPL = 0.3927;
	
class SaddleBrepsBuilder
{ 
 public:

  SaddleBrepsBuilder(StoreGateSvc  *pDetStore,
		   IRDBAccessSvc *pRDBAccess, std::string geoTag, std::string geoNode);
  ~SaddleBrepsBuilder();
    
  bool unavailableData() {return _unavailableData;}
  void buildSaddleBreps    ( GeoPhysVol* container );

 private:
  std::string getMaterial( std::string materialName );

  IRDBAccessSvc* m_pRDBAccess;
  StoreGateSvc*  m_pDetStore;

    bool _unavailableData;

    
  const IRDBRecordset* m_Saddle;
  const IRDBRecordset* m_Saddlebrvp;
  const IRDBRecordset* m_Saddlebrvbp;
  const IRDBRecordset* m_Saddlebrvn;
//  const IRDBRecordset* m_Fbrvsbo;
  const IRDBRecordset* m_Saddlebrvstu;
//  const IRDBRecordset* m_Fbrvsbr;
//  const IRDBRecordset* m_Feboxes;
  const IRDBRecordset* m_Saddleetubes;
//  const IRDBRecordset* m_Febreps;
//  const IRDBRecordset* m_Febrepsxy;
  
  IMessageSvc*    m_msgSvc;
};

} // namespace MuonGM

#endif
