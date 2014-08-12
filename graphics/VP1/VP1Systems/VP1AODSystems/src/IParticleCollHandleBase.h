/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class IParticleCollHandleBase             //
//                                                            //
//  Description: Base class for collection handles            //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1AODSYSTEMS_IPARTICLECOLLHANDLEBASE_H
#define VP1AODSYSTEMS_IPARTICLECOLLHANDLEBASE_H

//Local
#include "VP1AODSystems/AODSystemController.h"

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
class IParticleHandleBase;
class SoSeparator;
class SoMaterial;
class VP1ExtraSepLayerHelper;
class TrackSysCommonData;

namespace Trk {
  class Track;
  class Segment;
  class IExtrapolator;
}

class IParticleCollHandleBase : public VP1StdCollection
{

  Q_OBJECT

public:

  //Each derived class should implement a "static QStringList
  //availableCollections();" method, which the collection widget will
  //use to determine the available collections.

  IParticleCollHandleBase(AODSysCommonData *,
          const QString& name,
		  xAOD::Type::ObjectType);
  virtual void init(VP1MaterialButtonBase* matBut=0) =0;//reimplementations must start with a call to VP1StdCollection::init.
  virtual ~IParticleCollHandleBase();

   //Called just after creation. Should set current cut values and
  //connections with controller to monitor future changes. Reimplement
  //the ..Specific method to setup subsystem specific settings.
  void setupSettingsFromController(AODSystemController*);
protected:
	/// For extensions specific to this collection 
  virtual void setupSettingsFromControllerSpecific(AODSystemController*) {};
public:

  ///////////////////////////////////////////////////////////
  //  For loading the data and resetting after each event: //
  ///////////////////////////////////////////////////////////

  //For use by the handles:
  QString name() const;
  AODSysCommonData * common() const { return m_commonData; }
  VP1ExtraSepLayerHelper * sephelper() const { return m_sephelper; }//For attaching/detaching. FIXME: TO BASE!!
  
  virtual QByteArray persistifiableState() const;//!<Provide specific implementation 
  virtual void setState(const QByteArray&);//!<Provide specific implementation 
  
protected:

  //Must be called from derived classes when filling in new track handles.
  void hintNumberOfHandlesInEvent(unsigned);
  void addHandle(IParticleHandleBase*);

  //For iterating over track handles:
  void handleIterationBegin();
  IParticleHandleBase* getNextHandle(); //Returns 0 when done.

  //Override if special cuts. Remember to call base class implementation also for common cuts.
  virtual bool cut(IParticleHandleBase*);//Return true if should be shown (based on various cuts), false otherwise.

  //Utility (fixme: put in utility class elsewhere).
  template <class T> void cleanupPtrContainer(T&) const;//Delete pointers and calls .clear()
  //   template <class T> void cleanupNodeContainer(T&) const;//unref's pointers and calls .clear()

  void recheckCutStatus(IParticleHandleBase*);//Call in derived classes for handles that might be effected by a change in cuts.

  //Convenience methods which can be called from derived classes or
  //the trackcollwidget (but specialised procedures might be more
  //optimal)
public:
  void recheckCutStatusOfAllVisibleHandles();
  void recheckCutStatusOfAllNotVisibleHandles();
  void recheckCutStatusOfAllHandles();
  void update3DObjectsOfAllHandles();
  void updateMaterialOfAllHandles();

  QList<IParticleHandleBase*> getHandlesList() const;
  int nShownHandles() { return m_nshownhandles; }

  //For use only by IParticleHandleBase::setVisible(..):
  void incrementNShownHandles() { ++m_nshownhandles; }
  void decrementNShownHandles() { --m_nshownhandles; }

  virtual bool mayHaveAssociatedObjects() const { return false; }
  
  enum COLOURBY { COLOUR_PERCOLLECTION, COLOUR_RANDOM, COLOUR_BYPID, COLOUR_MOMENTUM, COLOUR_CHARGE, COLOUR_DISTANCE, COLOUR_VERTEX };
  // static QString toString(const COLOURBY&);
  // 
  // COLOURBY colourBy() const { return m_colourby; }

  virtual COLOURBY defaultColourBy () const { return COLOUR_PERCOLLECTION; }
  virtual bool     allowColourByPID() const { return false; }
  virtual bool     allowColourByMomentum() const { return false; }
  virtual bool     allowColourByCharge() const { return false; }
  virtual bool     allowColourByVertex() const { return false;  }
  
protected:

  qint32          provideCollTypeID() const;
  virtual QString provideSection() const;
  virtual QString provideSectionToolTip() const;
  virtual QString provideText() const { return name(); };//FIXME
  virtual void    assignDefaultMaterial(SoMaterial*) const;
  virtual QColor  defaultColor() const = 0;//Will be used in assignDefaultMaterial
  QList<QWidget*> provideExtraWidgetsForGuiRow() const;
  QByteArray      extraWidgetsState() const;
  void            setExtraWidgetsState(const QByteArray&);

private slots:
  void colourByComboBoxItemChanged();
  void collVisibilityChanged(bool);
  
public slots:
  void rerandomiseRandomTrackColours();

  // setters
  void setCutAllowedPt(const VP1Interval&);
  void setCutAllowedEta(const VP1Interval&);
  void setCutAllowedPhi(const QList<VP1Interval>&);

  //getters
  VP1Interval getCutAllowedPt() {return m_cut_allowedPtSq; };
  VP1Interval getCutAllowedEta() {return m_cut_allowedEta; };
  QList<VP1Interval> getCutAllowedPhi() {return m_cut_allowedPhi; };
  bool getPtAllowall() {return m_cut_pt_allowall; };
  bool getEtaAllowall() {return m_cut_eta_allowall; };
  bool getPhiAllowall() {return m_cut_phi_allowall; };


private:

  class Imp;
  Imp * d;

  int m_nshownhandles;
  xAOD::Type::ObjectType m_type;
  AODSysCommonData * m_commonData;
  VP1ExtraSepLayerHelper * m_sephelper;

  VP1Interval m_cut_allowedPtSq;//We store the allowed interval for pt squared - to avoid sqrt's.
  VP1Interval m_cut_allowedEta;
  QList<VP1Interval> m_cut_allowedPhi;
  bool m_cut_pt_allowall;
  bool m_cut_eta_allowall;
  bool m_cut_phi_allowall;
  bool m_cut_etaptphi_allwillfail;
  void fillObjectBrowser();
  void updateObjectBrowserVisibilityCounts();
};

//Fixme: Move elsewhere:
template <class T> void IParticleCollHandleBase::cleanupPtrContainer(T&t) const
{
  typename T::iterator it(t.begin()), itE(t.end());
  for (;it!=itE;++it)
    delete *it;
  t.clear();
}

#endif




