/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1RawDataHandleBase                //
//                                                            //
//  Description: Base class for RDO handles                   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008 (rewritten January 2009)      //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATAHANDLEBASE_H
#define VP1RAWDATAHANDLEBASE_H

class SoNode;
class SoTransform;
class SoMaterial;
class VP1RawDataCommonData;

#include "VP1RawDataSystems/VP1RawDataCollBase.h"
#include "VP1RawDataSystems/VP1RawDataFlags.h"

#include <QStringList>

#include "GeoPrimitives/GeoPrimitives.h"



class VP1RawDataHandleBase {
public:

  VP1RawDataHandleBase(VP1RawDataCollBase*);
  virtual ~VP1RawDataHandleBase();

  VP1RawDataCollBase* coll() const { return m_coll; }
  VP1RawDataCommonData * common() const { return m_coll->common(); }

  void setVisible(bool);
  bool visible() const { return m_visible; }

  void update3DObjects();
  virtual QStringList clicked(bool verbose) const = 0;//Called when user selects the node (stringlist is displayed in messagebox).

  virtual Amg::Vector3D center();//Default impl. uses the transform

  virtual VP1RawDataFlags::InDetPartsFlags inInDetParts() const { return VP1RawDataFlags::None; }//Override if InDet

  static QString unsignedToHex(unsigned);

  void updateShownOutlines();

protected:

  virtual SoNode * buildShape() = 0;
  virtual SoTransform * buildTransform() = 0;

  // It is illegal to copy/assign a VP1RawDataHandleBase:
  VP1RawDataHandleBase( const VP1RawDataHandleBase & );
  VP1RawDataHandleBase & operator= ( const VP1RawDataHandleBase & );

private:
  class Imp;
  Imp * d;
  //Here for inlining:
  VP1RawDataCollBase * m_coll;
  bool m_visible;

};

#endif
