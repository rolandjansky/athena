/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Build the Calorimeter Saddle
 -----------------------------------------
 Copyright (C) 2007 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: CalorimeterSaddleBuilder.h 459783 2011-09-21 15:00:41Z dellacqu $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_CALORIMETERSADDLEBUILDER_H
# define MUONGEOMODEL_CALORIMETERSADDLEBUILDER_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class StoreGateSvc;
class IRDBAccessSvc;
class GeoPhysVol;
class IRDBRecordset;
class IMessageSvc;

#include <string>

namespace MuonGM {

class SaddleBoxesBuilder;
class SaddleBrepsBuilder;
class SaddleTubsBuilder;


class CalorimeterSaddleBuilder
{
 public:
    CalorimeterSaddleBuilder( StoreGateSvc  *pDetStore,
                              IRDBAccessSvc *pRDBAccess, 
                              std::string    geoTag, std::string geoNode );
    ~CalorimeterSaddleBuilder(){};
    
    void build( GeoPhysVol* container  );

private:

  IRDBAccessSvc* m_pRDBAccess;
  StoreGateSvc*  m_pDetStore;

  IMessageSvc*    m_msgSvc;

  SaddleBoxesBuilder* m_saddleBoxesBuilder;
  SaddleBrepsBuilder* m_saddleBrepsBuilder;
  SaddleTubsBuilder* m_saddleTubsBuilder;
    
    
  //const IRDBRecordset* m_Jdsh;
    
    
};
    
    
}


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // MUONGEOMODEL_CALORIMETERSADDLEBUILDER_H
