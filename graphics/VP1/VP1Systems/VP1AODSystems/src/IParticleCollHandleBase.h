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
#include "AODCollHandleBase.h"
#include "IParticleHandleBase.h"

//VP1
#include "VP1Base/VP1Interval.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/VP1StdCollection.h"

//xAOD
#include "xAODBase/IParticle.h" // for xAOD::Type::ObjectType

// Qt
#include <QColor>
#include <QList>

//Misc
#include <iostream>

class AODSysCommonData;
class SoSeparator;
class SoMaterial;
class VP1ExtraSepLayerHelper;

/// Base class for collections holding AOD objects of iParticle type
/// Handles pt etc cuts
/// Local data:
///  - pointer to the xAODcollection
///  - lists of handles 
///  - caches for the pt cuts
class IParticleCollHandleBase : public AODCollHandleBase
{

  Q_OBJECT

public:

  //Each derived class should implement a "static QStringList
  //availableCollections();" method, which the collection widget will
  //use to determine the available collections.

  IParticleCollHandleBase(AODSysCommonData *,
  const QString& name,
  xAOD::Type::ObjectType);
  virtual ~IParticleCollHandleBase();
  
protected:
  /// For extensions specific to this collection 
  virtual void setupSettingsFromControllerSpecific(AODSystemController*) {};
public:

  // Loops
  // Must be called from derived classes when filling in new object handles.
  virtual void hintNumberOfHandlesInEvent(unsigned);
  void addHandle(AODHandleBase*);

  //For iterating over object handles:
  virtual void handleIterationBegin();
  virtual AODHandleBase* getNextHandle(); //Returns 0 when done.
  QList<AODHandleBase*> getHandlesList() const;

  ///////////////////////////////////////////////////////////
  //  For loading the data and resetting after each event: //
  ///////////////////////////////////////////////////////////
  virtual QByteArray persistifiableState() const;//!<Provide specific implementation 
  virtual void setState(const QByteArray&);//!<Provide specific implementation 
  
protected:

  //Override if special cuts. Remember to call base class implementation also for common cuts.
  virtual bool cut(AODHandleBase*);//Return true if should be shown (based on various cuts), false otherwise.

  //Utility (fixme: put in utility class elsewhere).
  template <class T> void cleanupPtrContainer(T&) const;//Delete pointers and calls .clear()
  //   template <class T> void cleanupNodeContainer(T&) const;//unref's pointers and calls .clear()

public:

  virtual bool mayHaveAssociatedObjects() const { return false; }
    
protected:

  virtual QColor  defaultColor() const = 0;//Will be used in assignDefaultMaterial
  
public slots:

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

  VP1Interval m_cut_allowedPtSq;//We store the allowed interval for pt squared - to avoid sqrt's.
  VP1Interval m_cut_allowedEta;
  QList<VP1Interval> m_cut_allowedPhi;
  bool m_cut_pt_allowall;
  bool m_cut_eta_allowall;
  bool m_cut_phi_allowall;
  bool m_cut_etaptphi_allwillfail;
};

#endif




