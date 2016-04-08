/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP12DGEOMETRYSYSTEM_H
#define VP12DGEOMETRYSYSTEM_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP12DGeometrySystem                          //
//                                                                     //
//  Author: Brock Greman <bgreman@gmail.com>                           //
//                                                                     //
//  Initial version: August 2007                                       //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP12DDetViewsSystem.h"

class VP12DGeometrySystem : public IVP12DDetViewsSystem {

  Q_OBJECT

public:
  VP12DGeometrySystem();
  ~VP12DGeometrySystem();
  void systemcreate(StoreGateSvc*);
  void buildPermanentItemCollections(StoreGateSvc* detstore,VP1GraphicsItemCollection*, VP1GraphicsItemCollection*);
  //QWidget * buildController();  //Will implement a more specific controller

 private:
  VP12DGeometrySystem( const VP12DGeometrySystem & );
  VP12DGeometrySystem & operator= ( const VP12DGeometrySystem & );

  class Imp;
  Imp *d;

};

#endif
