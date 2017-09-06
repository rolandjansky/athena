/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleCollHandleBase               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

//Local includes
#include "IParticleCollHandleBase.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "IParticleHandleBase.h"
#include "VP1AODSystems/AODSystemController.h"
#include "AODSysCommonData.h"

//VP1 base includes
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

//SoCoin
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"

//Athnea
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

//Qt
#include <QComboBox>
#include <QTreeWidgetItem>
#include <qdatetime.h>
#include <vector>
#include <QString>
#include <sstream>

//____________________________________________________________________
class IParticleCollHandleBase::Imp {
public:
  IParticleCollHandleBase * theclass;
  
  //Vector of iParticle handles:
  std::vector<IParticleHandleBase*> handles;
  // QList of xAOD handles
  QList<AODHandleBase*> handlesList;

  //For iteration:
  std::vector<IParticleHandleBase*>::iterator itHandles;
  std::vector<IParticleHandleBase*>::iterator itHandlesEnd;
  
  // N.B. Material button defined in children.
};



//____________________________________________________________________
IParticleCollHandleBase::IParticleCollHandleBase( AODSysCommonData * cd,
const QString& name,
xAOD::Type::ObjectType type)
  : AODCollHandleBase(cd,name,type), d(new Imp), // Need to add back ObjectType once simple way to create string is added to xAODBase
  m_cut_allowedPtSq(VP1Interval()),
  m_cut_allowedEta(VP1Interval()),
  m_cut_allowedPhi(QList<VP1Interval>()),
  m_cut_pt_allowall(false),
  m_cut_eta_allowall(false),
  m_cut_phi_allowall(false),
  m_cut_etaptphi_allwillfail(true)
{
  d->theclass = this;
}

//____________________________________________________________________
IParticleCollHandleBase::~IParticleCollHandleBase()
{
  messageVerbose("destructor start");
  
  d->handles.clear();
  
  // clean the QList<handle>
  foreach(AODHandleBase* handle, d->handlesList) {
    delete handle;
  }
  
  // delete the Imp instance
  delete d;
  messageVerbose("destructor end");
}

//____________________________________________________________________
void IParticleCollHandleBase::hintNumberOfHandlesInEvent(unsigned n)
{
  d->handles.reserve(n);
}

//____________________________________________________________________
void IParticleCollHandleBase::addHandle(AODHandleBase* ah)
{
  IParticleHandleBase* handle = dynamic_cast<IParticleHandleBase* >(ah);
  if (!handle) {
    message("ERROR - wrong handle type passed to IParticleCollHandleBase::addHandle!");
    return;
  }
  d->handles.push_back(handle); // for the vector<handle>
  d->handlesList << handle; // for the QList<handle>
}

//____________________________________________________________________
void IParticleCollHandleBase::handleIterationBegin()
{
  d->itHandles = d->handles.begin();
  d->itHandlesEnd = d->handles.end();
}

//____________________________________________________________________
AODHandleBase* IParticleCollHandleBase::getNextHandle() {
  if (d->itHandles==d->itHandlesEnd)
  // if (d->itHandles==d->itHandlesEnd || d->itHandles!=d->handles.begin()) // Only use first handle (for debugging)
    return 0;
  else
    return *(d->itHandles++);
}

//____________________________________________________________________
QList<AODHandleBase*> IParticleCollHandleBase::getHandlesList() const
{
  messageVerbose("AODCollHandleBase::getHandlesList()");
  return d->handlesList;
}

//____________________________________________________________________
bool IParticleCollHandleBase::cut(AODHandleBase* ahand)
{
  messageVerbose("IParticleCollHandleBase::cut()");

  IParticleHandleBase* handle = dynamic_cast<IParticleHandleBase*>(ahand);
  
  if (!handle) {
    message("ERROR! IParticleCollHandleBase::cut() - passed in a handle of the wrong type! ");
    return false;
  }
  
  if (m_cut_etaptphi_allwillfail)
    return false;

  messageVerbose("m_cut_pt_allowall: " + QString::number(m_cut_pt_allowall) + " - m_cut_eta_allowall: " + QString::number(m_cut_eta_allowall) + " - m_cut_phi_allowall: " + QString::number(m_cut_phi_allowall) );

  if (!m_cut_pt_allowall||!m_cut_eta_allowall||!m_cut_phi_allowall)
  {
    messageVerbose("evaluating cut...");

    messageVerbose("m_cut_allowedEta: " + m_cut_allowedEta.toString() + " - excludeInterval: " + QString::number(m_cut_allowedEta.excludeInterval()) );
       

    //    Trk::GlobalMomentum mom(handle->momentum());
    Amg::Vector3D mom(handle->momentum());
    // std::cout<<"mom mag2: "<<mom.mag2()<<std::endl;

    // convention is that if interval is real and negative, then P cut rather than pT
    bool isPCut = (!m_cut_allowedPtSq.noLowerBound() && m_cut_allowedPtSq.lower()<=0)
      || (!m_cut_allowedPtSq.noUpperBound() && m_cut_allowedPtSq.upper()<=0);

    messageVerbose("mom.eta(): " + QString::number(mom.eta()) );

    if (mom.mag2()!=0.0) {

      // *** PT CUT ***
      if (!m_cut_pt_allowall&&!isPCut && !m_cut_allowedPtSq.contains(mom.perp2()))
        return false;
      if (!m_cut_pt_allowall&& isPCut && !m_cut_allowedPtSq.contains(-mom.mag2()))
        return false;

      // *** ETA CUT ***
      //      if (!m_cut_eta_allowall&&!m_cut_allowedEta.contains(mom.pseudoRapidity())){
      if (!m_cut_eta_allowall && !m_cut_allowedEta.contains(mom.eta())){ // migration to Amg (Eigen)
        messageVerbose("eta cut not passed...");
        return false;
      }

      // *** PHI CUT ***
      if (!m_cut_phi_allowall) {
        double phi = VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() );
        messageVerbose("value oh handle's phi: " + QString::number(phi));
        bool ok(false);
        foreach (VP1Interval phirange, m_cut_allowedPhi) {
          if (phirange.contains(phi)||phirange.contains(phi+2*M_PI)||phirange.contains(phi-2*M_PI)) {
            ok = true;
            break;
          }
        }
        if (!ok)
          return false;
      }
    }
    messageVerbose("IParticleCollHandleBase::cut() - mom.mag2() == 0.0!!!");

  }

  messageVerbose("IParticleCollHandleBase::cut() - Returning true!");
  return true;
}

//____________________________________________________________________
void IParticleCollHandleBase::setCutAllowedPt(const VP1Interval& allowedpt)
{
  messageVerbose("IParticleCollHandleBase::setCutAllowedPt()");

  if (!allowedpt.isSane())
    return;

  //why do we need to do this? Why does it matter if it's negative?
  //double minptsq = allowedpt.lower() <= 0.0 ? -std::numeric_limits<double>::infinity() : allowedpt.lower()*allowedpt.lower();
  //VP1Interval allowedptsq(minptsq,allowedpt.upper()*allowedpt.upper());

  //Modified code from EJWM
  double signMin = allowedpt.lower()<0.0 ? -1.0 : 1.0;
  double signMax = allowedpt.upper()<0.0 ? -1.0 : 1.0;
  double minptsq = allowedpt.lower() == -std::numeric_limits<double>::infinity() ? -std::numeric_limits<double>::infinity() : signMin*(allowedpt.lower()*allowedpt.lower());
  double maxptsq = allowedpt.upper() == std::numeric_limits<double>::infinity() ? std::numeric_limits<double>::infinity()   : signMax*(allowedpt.upper()*allowedpt.upper());

  VP1Interval allowedptsq(minptsq,maxptsq);
  // std::cout<<"allowedptsq:isAllR"<<allowedptsq.isAllR()<<std::endl;
  // std::cout<<"allowedptsq.isEmpty()"<<allowedptsq.isEmpty()<<std::endl;


  if (m_cut_allowedPtSq==allowedptsq)
    return;
  m_cut_pt_allowall = allowedptsq.isAllR();
  m_cut_etaptphi_allwillfail = allowedptsq.isEmpty() || m_cut_allowedEta.isEmpty() || m_cut_allowedPhi.isEmpty();

  if (!m_cut_allowedPtSq.contains(allowedptsq)&&!allowedptsq.contains(m_cut_allowedPtSq)) {
    m_cut_allowedPtSq = allowedptsq;
    recheckCutStatusOfAllHandles();
    return;
  }
  bool relaxcut = allowedptsq.contains(m_cut_allowedPtSq);
  m_cut_allowedPtSq = allowedptsq;
  if (relaxcut)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void IParticleCollHandleBase::setCutAllowedEta(const VP1Interval& allowedEta)
{
  messageVerbose("signal received in setCutAllowedEta ("+allowedEta.toString()+")");
  if (!allowedEta.isSane())
    return;

  if (m_cut_allowedEta==allowedEta)
    return;
  m_cut_eta_allowall = allowedEta.isAllR();
  m_cut_etaptphi_allwillfail = allowedEta.isEmpty() || m_cut_allowedPtSq.isEmpty() || m_cut_allowedPhi.isEmpty();

  if (!m_cut_allowedEta.contains(allowedEta)&&!allowedEta.contains(m_cut_allowedEta)) {
    m_cut_allowedEta = allowedEta;
    recheckCutStatusOfAllHandles();
    return;
  }
  bool relaxcut = allowedEta.contains(m_cut_allowedEta);
  m_cut_allowedEta = allowedEta;
  if (relaxcut)
    recheckCutStatusOfAllNotVisibleHandles();
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void IParticleCollHandleBase::setCutAllowedPhi(const QList<VP1Interval>& allowedPhi)
{
  messageVerbose("IParticleCollHandleBase::setCutAllowedPhi() - signal received in setCutAllowedPhi.");

  if (m_cut_allowedPhi==allowedPhi)
    return;
  m_cut_allowedPhi = allowedPhi;

  m_cut_phi_allowall = m_cut_allowedPhi.count()==1 && m_cut_allowedPhi.at(0).isAllR();
  m_cut_etaptphi_allwillfail = m_cut_allowedPhi.isEmpty() || m_cut_allowedPtSq.isEmpty() || m_cut_allowedEta.isEmpty();

  if (m_cut_phi_allowall) {
    recheckCutStatusOfAllNotVisibleHandles();
    return;
  }
  if (m_cut_etaptphi_allwillfail) {
    recheckCutStatusOfAllVisibleHandles();
    return;
  }
  recheckCutStatusOfAllHandles();
  return;
}


//____________________________________________________________________
void IParticleCollHandleBase::setState(const QByteArray&state)
{
  VP1Deserialise des(state);
  des.disableUnrestoredChecks();
  if (des.version()!=0&&des.version()!=1) {
    messageDebug("Warning: Ignoring state with wrong version");
    return;
  }
  bool vis = des.restoreBool();

  QByteArray matState = des.restoreByteArray();
  // d->matButton->restoreFromState(matState);
  QByteArray extraWidgetState = des.version()>=1 ? des.restoreByteArray() : QByteArray();
  setVisible(vis);

  if (extraWidgetState!=QByteArray())
    setExtraWidgetsState(extraWidgetState);
}

//____________________________________________________________________
QByteArray IParticleCollHandleBase::persistifiableState() const
{
  messageDebug("IParticleCollHandleBase::persistifiableState() - start...");

  // if (!d->matButton) {
  //   message("ERROR: persistifiableState() called before init()");
  //   return QByteArray();
  // }
  VP1Serialise serialise(1/*version*/);
  serialise.disableUnsavedChecks();

  // SAVE THE CHECKED/UNCHECKED STATUS OF THE COLLECTION
  serialise.save(visible());

  // SAVE THE MATERIAL BUTTON
  // Q_ASSERT(d->matButton&&"Did you forget to call init() on this VP1StdCollection?");
  // serialise.save(d->matButton->saveState());

  // SAVE THE EXTRA-STATES
  serialise.save(extraWidgetsState());//version 1+

  messageDebug("IParticleCollHandleBase::persistifiableState() - end.");
  return serialise.result();
}
