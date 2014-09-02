/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SaddleTubsBuilder_H
#define SaddleTubsBuilder_H


class StoreGateSvc;
class IRDBAccessSvc;
class GeoPhysVol;
class IRDBRecordset;
class IMessageSvc;

#include <string>

namespace MuonGM {
	
class SaddleTubsBuilder 
{ 
 public:

  SaddleTubsBuilder(StoreGateSvc  *pDetStore,
		   IRDBAccessSvc *pRDBAccess, std::string geoTag, std::string geoNode);
  ~SaddleTubsBuilder();

  void buildSaddleTubs    ( GeoPhysVol* container );
  bool unavailableData() {return _unavailableData;}
    

 private:
  std::string getMaterial( std::string materialName );

  IRDBAccessSvc* m_pRDBAccess;
  StoreGateSvc*  m_pDetStore;

    bool _unavailableData;
    
  const IRDBRecordset* m_Saddle;
  const IRDBRecordset* m_Saddletuvp;
  const IRDBRecordset* m_Saddletuvs;
  const IRDBRecordset* m_Saddletuvn;
  const IRDBRecordset* m_Saddletuvsbo;
  const IRDBRecordset* m_Saddletuvstu;
  const IRDBRecordset* m_Saddletuvsbr;
  const IRDBRecordset* m_Saddleeboxes;
  const IRDBRecordset* m_Saddleetubes;
  const IRDBRecordset* m_Saddleebreps;
  const IRDBRecordset* m_Saddleebrepsxy;
  
  IMessageSvc*    m_msgSvc;
};

} // namespace MuonGM

#endif
