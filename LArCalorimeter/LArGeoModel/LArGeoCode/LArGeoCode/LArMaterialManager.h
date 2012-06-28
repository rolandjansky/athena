/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _LARMATERIALMANAGER_H_
#define _LARMATERIALMANAGER_H_
//----------------------------------------------------------//
//                                                          //
// class LArMaterialManager  This is a material manager     //
// with special materials for LAR which require geometrical //
// information to calculate because they are blended.       //
// Other materials (non blended, or not blended using       //
// geometrical constants) should go to NovaMaterialManager  //
// in GeoModelService, for the brief duration of its        //
// miserable existence.  From which point onward materials  //
// will be stored in Oracle.                                //
//                                                          //
// Joe Boudreau March 2003                                  //
//                                                          //
// ------------------------                                 //
//                                                          //
// LArMaterialManager class switched to the new interface   //
// of StoredMaterialManager.                                //
// As of now it does need to be subclassed from             //
// AbsMaterialManager and the only thing it has to do is    //
// to build LAr specific materials and store them into      //
// central Stored Material manager                          //
//                                                          //
// V.Tsulaia, Jan-2009                                      //
//                                                          //
//----------------------------------------------------------//

class StoreGateSvc;
class StoredMaterialManager;
class IMessageSvc;
class LArMaterialManager
{
 public:

  // Constructor:
  LArMaterialManager(StoreGateSvc *pDetStore);
  
  // Destructor:
  ~LArMaterialManager();

  // Build LAr specific blended materials
  void buildMaterials();
  
 private:
  
  StoredMaterialManager* m_storedManager;
  IMessageSvc*           m_msgSvc;
};

#endif
