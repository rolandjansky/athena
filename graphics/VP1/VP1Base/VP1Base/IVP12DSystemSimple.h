/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class IVP12DSystemSimple               //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef IVP12DSYSTEMSIMPLE_H
#define IVP12DSYSTEMSIMPLE_H

#include "VP1Base/IVP12DSystem.h"

class VP1GraphicsItemCollection;

class IVP12DSystemSimple : public IVP12DSystem {

  Q_OBJECT

public:

  ////////////////////////////////////////////////////////////////////////
  //////////////  This is the interesting bit for those     //////////////
  //////////////  who are deriving from this class...       //////////////
  ////////////////////////////////////////////////////////////////////////

  IVP12DSystemSimple(const QString & name, const QString & information, const QString & contact_info);

  virtual void systemcreate(StoreGateSvc* detstore);//Optional.

  //If you need a controller - create it and return it from a reimplementation of this method (it will only be called once).
  virtual QWidget * buildController();

  //If some items should be visible for the entire run, reimplement:
  virtual void buildPermanentItemCollection(StoreGateSvc* detstore,VP1GraphicsItemCollection*);

  //Reimplement and add the items for a given event, based on the contents of storegate:
  virtual void buildEventItemCollection(StoreGateSvc *sg, VP1GraphicsItemCollection*);

  void updateGUI();//Call this occasionally in buildEventSceneGraph(...) to avoid GUI freezing.

  virtual void systemerase();//Optional - for erasing maps etc. with event data.

  virtual void systemuncreate();//Optional.

  ////////////////////////////////////////////////////////////////////////
  ////////////  The rest of this file is just internal stuff  ////////////
  ////////////           - dont worry about it -              ////////////
  ////////////////////////////////////////////////////////////////////////

  virtual ~IVP12DSystemSimple();
private:
  IVP12DSystemSimple( const IVP12DSystemSimple & );
  IVP12DSystemSimple & operator= ( const IVP12DSystemSimple & );
  class Imp;
  Imp * m_d;
  void create(StoreGateSvc* detstore);
  void refresh(StoreGateSvc* storegate);
  void erase();
  void uncreate();
};

/////////// Inlines /////////////
inline void IVP12DSystemSimple::updateGUI() { IVP12DSystem::updateGUI(); }


#endif
