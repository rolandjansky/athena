/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VertexCollHandle                    //
//                                                            //
//  Description: class for Vertex collection handles          //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1AODSYSTEMS_VERTEXCOLLHANDLEBASE_H
#define VP1AODSYSTEMS_VERTEXCOLLHANDLEBASE_H

//Local
#include "VP1AODSystems/AODSystemController.h"
#include "AODCollHandleBase.h"

//VP1
#include "VP1Base/VP1Interval.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/VP1StdCollection.h"

//xAOD
#include "xAODBase/IParticle.h"

// Qt
#include <QtGui/QColor>
#include <QList>

//Misc
#include <iostream>

class AODSysCommonData;
class VertexHandle;
class SoSeparator;
class SoMaterial;
class VP1ExtraSepLayerHelper;
class VertexCollectionSettingsButton;

class VertexCollHandle : public AODCollHandleBase
{

  Q_OBJECT

public:

  //Each derived class should implement a "static QStringList
  //availableCollections();" method, which the collection widget will
  //use to determine the available collections.

  VertexCollHandle(AODSysCommonData *,
          const QString& name,
		      xAOD::Type::ObjectType);
          
  virtual void init(VP1MaterialButtonBase* matBut=0);//reimplementations must start with a call to VP1StdCollection::init.
  virtual ~VertexCollHandle();

protected:
	/// For extensions specific to this collection 
  virtual void setupSettingsFromControllerSpecific(AODSystemController*);
  
  
public:

  // The button is responsible for the settings of the vertex collection, e.g. colouring, vertex size etc.
  const VertexCollectionSettingsButton& collSettingsButton() const;

  ///////////////////////////////////////////////////////////
  //  For loading the data and resetting after each event: //
  ///////////////////////////////////////////////////////////
  static QStringList availableCollections(IVP1System*);//For the collection widget.
  
  virtual QByteArray persistifiableState() const;//!<Provide specific implementation 
  virtual void setState(const QByteArray&);//!<Provide specific implementation 
  
protected:

  /// Actually load the EDM objects
  virtual bool load();

  //Must be called from derived classes when filling in new track handles.
  void hintNumberOfHandlesInEvent(unsigned);
  void addHandle(AODHandleBase*);

  //For iterating over track handles:
  void handleIterationBegin();
  AODHandleBase* getNextHandle(); //Returns 0 when done.
  QList<AODHandleBase*> getHandlesList() const;

  //Override if special cuts. Remember to call base class implementation also for common cuts.
  virtual bool cut(AODHandleBase*);//Return true if should be shown (based on various cuts), false otherwise.

  //Utility (fixme: put in utility class elsewhere).
  template <class T> void cleanupPtrContainer(T&) const;//Delete pointers and calls .clear()
  //   template <class T> void cleanupNodeContainer(T&) const;//unref's pointers and calls .clear()

  void recheckCutStatus(VertexHandle*);//Call in derived classes for handles that might be effected by a change in cuts.

  //Convenience methods which can be called from derived classes or
  //the trackcollwidget (but specialised procedures might be more
  //optimal)

  virtual bool mayHaveAssociatedObjects() const { return false; }
  
protected:
  virtual QColor defaultColor() const { return QColor::fromRgbF(1.0f, 0.5f, 0.5f); } // light red


private slots:
	void resetCachedValuesCuts();
  
public slots:
  // void rerandomiseRandomVertexColours();


  // setters
  void setCutAllowedY(const VP1Interval&);
  void setCutAllowedZ(const VP1Interval&);
  void setCutAllowedR(const VP1Interval&);
  void setVertexSize(int);

  //getters
  VP1Interval getCutAllowedY() {return m_cut_allowedY; };
  VP1Interval getCutAllowedZ() {return m_cut_allowedZ; };
  VP1Interval getCutAllowedR() {return m_cut_allowedR; };
  bool        getYAllowall() {return m_cut_y_allowall; };
  bool        getZAllowall() {return m_cut_z_allowall; };
  bool        getRAllowall() {return m_cut_r_allowall; };

private:

  class Imp;
  Imp * d;

  int m_nshownhandles;

  VP1Interval m_cut_allowedY;
  VP1Interval m_cut_allowedR;
  VP1Interval m_cut_allowedZ;
  bool m_cut_y_allowall;
  bool m_cut_r_allowall;
  bool m_cut_z_allowall;
  bool m_cut_ry_allwillfail;
  bool m_cut_rz_allwillfail;
};

//Fixme: Move elsewhere:
template <class T> void VertexCollHandle::cleanupPtrContainer(T&t) const
{
  typename T::iterator it(t.begin()), itE(t.end());
  for (;it!=itE;++it)
    delete *it;
  t.clear();
}

#endif




