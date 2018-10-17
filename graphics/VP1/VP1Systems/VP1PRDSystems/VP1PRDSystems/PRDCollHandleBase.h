/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLECTIONHANDLEBASE_H
#define PRDCOLLECTIONHANDLEBASE_H

#include "VP1Base/VP1StdCollection.h"
#include "VP1PRDSystems/PRDDetTypes.h"
#include <vector>

class PRDSysCommonData;
class PRDSystemController;
class PRDHandleBase;
class VP1ExtraSepLayerHelper;
class SoSeparator;
class SoMaterial;
namespace Trk { class PrepRawData; }

class PRDCollHandleBase : public VP1StdCollection {

  Q_OBJECT

public:

  PRDCollHandleBase(PRDDetType::Type,PRDSysCommonData*,const QString& key) ;
  virtual void init(VP1MaterialButtonBase* mat=0);//reimplementations must start with a call to this.
  virtual ~PRDCollHandleBase();

  //Called from init(). Should set current cut values and
  //connections with controller to monitor future changes. Reimplement
  //the ..Specific method to setup subsystem specific settings.
  void setupSettingsFromController(PRDSystemController*);
protected:
  virtual void setupSettingsFromControllerSpecific(PRDSystemController*) {};
public:

  ///////////////////////////////////////////////////////////
  //  For loading the data and resetting after each event: //
  ///////////////////////////////////////////////////////////

  virtual bool load();

  PRDSysCommonData * common() const { return m_common; }

  //For use by the handles:
  VP1ExtraSepLayerHelper * sephelperDetailedNodes () const;
  VP1ExtraSepLayerHelper * sephelperSimpleNodes() const;
  SoMaterial * highLightMaterial() const;

  //If highlight outliers is set, other highlight modes (TRT HT, ...) will be ignored.
  //Other highlight modes are indicated by the return value of highLight() in the PRDHandleBase's
  bool highLightOutliers() const;
  double highLightWeight() const { return m_highlightweight; }

  bool drawErrors() const;
  bool drawRDOs() const;


  //Detail level of shown prds:
  enum DETAIL { SIMPLE, DETAILED, AUTO };//AUTO => Use Level of detail (SoLOD nodes) to switch between simple/detailed shapes.
  static QString toString(const DETAIL&);

  void getLODSeparators(int index, VP1ExtraSepLayerHelper*& sephelper_detail,VP1ExtraSepLayerHelper*& sephelper_simple);

  enum COLOURMETHOD { ByTechOnly, BySegment, ByTrack, BySegmentAndTrack };
  static QString toString(const COLOURMETHOD&);
  COLOURMETHOD colourMethod() const { return m_colourmethod; }
  bool colourByTracks() const { return m_colourmethod==ByTrack||m_colourmethod==BySegmentAndTrack; }
  bool colourBySegments() const { return m_colourmethod==BySegment||m_colourmethod==BySegmentAndTrack; }

  DETAIL detailLevel() const;
  bool simpleDetailLevel() const { return detailLevel()==SIMPLE; }
signals:
  void detailLevelChanged();
public slots:
  void setGeneralPRDDetailLevel(DETAIL);
  void setColourMethod(PRDCollHandleBase::COLOURMETHOD);
  void setDrawErrors(bool);
  void setDrawRDOs(bool);
  void setHighLightOutliers(bool);
  void setHighLightWeight(const double&);
  void setAllowedEta(const VP1Interval&);
  void setAllowedPhi(const QList<VP1Interval>&);

private slots:
  void detailComboBoxItemChanged();

protected:
  virtual PRDHandleBase* addPRD(const Trk::PrepRawData*) = 0;//Return pointer to the handle. Base class (this) will own it.

  virtual DETAIL defaultDetailLevel() const { return AUTO; }
  //Use to loop over prds in derived classes:
  void addHandle(PRDHandleBase*);//All handles must be added here (so they can be retrieved by the next methods).
  std::vector<PRDHandleBase*>& getPrdHandles();
  const std::vector<PRDHandleBase*>& getPrdHandles() const;

  virtual void postLoadInitialisation() {}
  virtual bool cut(PRDHandleBase*) = 0;//Return true if should be shown (based on various cuts), false otherwise.
  virtual void eraseEventDataSpecific() {}

  //Utility (fixme: put in utility class elsewhere).
  template <class T> void cleanupPtrContainer(T&) const;//Delete pointers and calls .clear()
  //   template <class T> void cleanupNodeContainer(T&) const;//unref's pointers and calls .clear()

  virtual float lodArea() const { return 500.0f*500.0f; }//Reimplement Override to tune LOD shift for a given collection.

  void recheckCutStatus(PRDHandleBase*);//Call in derived classes for handles that might be effected by a change in cuts.

  //Convenience methods which can be called from derived classes (but specialised procedures might be more optimal)
  void recheckCutStatusOfAllHandles();
  void recheckCutStatusOfAllVisibleHandles();
  void recheckCutStatusOfAllNotVisibleHandles();

public:
  int nShownHandles() { return m_nshownhandles; }

  //For use only by PRDHandleBase::setVisible(..):
  void incrementNShownHandles() { ++m_nshownhandles; }
  void decrementNShownHandles() { --m_nshownhandles; }

protected:
  qint32 provideCollTypeID() const;
  virtual QString provideText() const;
  virtual void assignDefaultMaterial(SoMaterial*) const;
  virtual QColor defaultColor() const = 0;//Will be used in assignDefaultMaterial
  QString provideSection() const;
  QString provideSectionToolTip() const;
  QList<QWidget*> provideExtraWidgetsForGuiRow() const;
  QByteArray extraWidgetsState() const;
  void setExtraWidgetsState(const QByteArray&);
private slots:
  void collVisibilityChanged(bool);
private:

  class Imp;
  Imp * m_d;

  PRDSysCommonData * m_common;
  int m_nshownhandles;
  COLOURMETHOD m_colourmethod;
  double m_highlightweight;
};

//Fixme: Move elsewhere (at least to our cxx!):
template <class T> void PRDCollHandleBase::cleanupPtrContainer(T&t) const
{
  typename T::iterator it(t.begin()), itE(t.end());
  for (;it!=itE;++it)
    delete *it;
  t.clear();
}

#endif
