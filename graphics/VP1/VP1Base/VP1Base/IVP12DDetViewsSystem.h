/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP12DDetViewsSystem             //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: July 2007                             //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP12DDETVIEWSSYSTEM_H
#define IVP12DDETVIEWSSYSTEM_H

#include "VP1Base/IVP12DSystem.h"

#include <QSet>

class VP1GraphicsItemCollection;

class IVP12DDetViewsSystem : public IVP12DSystem {

  Q_OBJECT

public:

  ////////////////////////////////////////////////////////////////////////
  //////////////  This is the interesting bit for those     //////////////
  //////////////  who are deriving from this class...       //////////////
  ////////////////////////////////////////////////////////////////////////

  IVP12DDetViewsSystem(const QString & name, const QString & information, const QString & contact_info);

  virtual void systemcreate(StoreGateSvc* detstore);//Optional.

  //If you need a controller - create it and return it from a reimplementation of this method (it will only be called once).
  virtual QWidget * buildController();

  //If some items should be visible for the entire run, reimplement:
  virtual void buildPermanentItemCollections( StoreGateSvc* detstore,
					      VP1GraphicsItemCollection* ic_xy,
					      VP1GraphicsItemCollection* ic_rz );

  //Reimplement and add the items for a given event, based on the contents of storegate:
  virtual void buildEventItemCollections( StoreGateSvc *sg,
					  VP1GraphicsItemCollection* ic_xy,
					  VP1GraphicsItemCollection* ic_rz );

  virtual void systemerase();//Optional - for erasing maps etc. with event data.

  virtual void systemuncreate();//Optional.


  //This is the interesting bit for channels using this system:
  QSet<VP1GraphicsItemCollection*> getItemCollections_XY() const;
  QSet<VP1GraphicsItemCollection*> getItemCollections_RZ() const;

  ////////////////////////////////////////////////////////////////////////
  ////////////  The rest of this file is just internal stuff  ////////////
  ////////////           - dont worry about it -              ////////////
  ////////////////////////////////////////////////////////////////////////

  virtual ~IVP12DDetViewsSystem();
private:
  class Imp;
  Imp * m_d;
  void create(StoreGateSvc* detstore);
  void refresh(StoreGateSvc* storegate);
  void erase();
  void uncreate();
};



#endif
