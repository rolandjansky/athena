/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class AssociatedObjectHandleBase          //
//                                                            //
//  Description: Handle for objects associated with tracks,   //
//               such as measurements, surfaces, etc.         //
//               It might provide just one object, or it      //
//               might alternatively be split into multiple   //
//               components (parts)                           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef ASSOCIATEDOBJECTHANDLEBASE_H
#define ASSOCIATEDOBJECTHANDLEBASE_H


class TrackHandleBase;
class SoTransform;
class SoNode;
class SoSeparator;
class TrackSysCommonData;
class AssocObjAttachmentHandle;
#include <QStringList>

class AssociatedObjectHandleBase {
public:

  static int numberOfInstances();

  AssociatedObjectHandleBase(TrackHandleBase*);//The handle will register itself
                                               //with the trackhandle, which will
                                               //also take care of deleting it
                                               //before moving to next event.
  virtual QStringList clicked() = 0;
  //  virtual SoNode * getNodeForPart( int part ) const = 0;
  virtual unsigned nParts() const { return 1; }

  TrackSysCommonData * common() const;

  enum PICKSTYLE { UNPICKABLE, ALL, COMPONENTS };
  void setPickable(PICKSTYLE);
  PICKSTYLE pickStyle() const { return m_pickStyle; }
  virtual void setVisible(bool);
  bool visible() const { return m_visible; }
  void toggleVisible() { setVisible(!m_visible); }

  TrackHandleBase* trackHandle() const { return m_trackHandle; }
  void update3DObjects();//Call when settings changed affecting the shapes.

  //Return current shapes (0 if not built at the moment):
  SoSeparator* shapeSimple() const;
  SoSeparator* shapeDetailed() const;

  virtual bool initiatesOwnZooms() const { return false; }

protected:

  virtual void buildShapes(SoSeparator*&shape_simple, SoSeparator*&shape_detailed) = 0;
  //The returned SoSeparators should return all necessary transforms,
  //and must optionally also include any necessary SoMaterial nodes
  //(otherwise everything will get the track colour).

  //The next two are for LevelOfDetail. Probably return per detector type/station id?
  virtual int regionIndex() const { return 0; };
  virtual double lodCrossOverValue() const { return 50*50; };

private:

  // It is illegal to copy/assign an AssociatedObjectHandleBase:
  AssociatedObjectHandleBase( const AssociatedObjectHandleBase & );
  AssociatedObjectHandleBase & operator= ( const AssociatedObjectHandleBase & );

  class Imp;
  Imp * m_d;

  TrackHandleBase* m_trackHandle;
  bool m_visible;
  PICKSTYLE m_pickStyle;
  void registerShapes(SoSeparator*simple,SoSeparator*detailed);
  void unregisterShapes(SoSeparator*simple,SoSeparator*detailed);
  AssocObjAttachmentHandle * getAttachmentHandle();

protected:
  friend class TrackHandleBase;
  virtual ~AssociatedObjectHandleBase();//The TrackHandleBase will delete it.

};

#endif
