/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AODHandleBase                   //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "AODHandleBase.h"
#include "VP1AODSystems/AODSystemController.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "VP1Utils/VP1ParticleData.h"
#include "VP1Utils/VP1LinAlgUtils.h"

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/VP1QtUtils.h"

#include "VP1Utils/SurfaceToSoNode.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/SbRotation.h>
#include <Inventor/SbMatrix.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoText2.h>

#include "GeoPrimitives/AmgStringHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h" 

#include <cassert>

#include "AODCollHandleBase.h"
#include "AODSysCommonData.h"

//#include "xAODBase/IParticle.h"


//____________________________________________________________________
class AODHandleBase::Imp {
public:
  static double dist(const SbVec3f& p1,const SbVec3f& p2);

  static int nobjhandles;

  Imp(AODHandleBase*tc) : theclass(tc),
  m_objBrowseTree(0){}
  ~Imp() { }
  AODHandleBase * theclass;

  //For efficiency we cache parameterisations of the various track parts:

  QTreeWidgetItem* m_objBrowseTree;
};


//____________________________________________________________________
int AODHandleBase::Imp::nobjhandles = 0;

//____________________________________________________________________
AODHandleBase::AODHandleBase(AODCollHandleBase*ch)
  : d(new Imp(this)), m_visible(false),m_collhandle(ch),m_currentmaterial(0)
{
  ++Imp::nobjhandles;
}

//____________________________________________________________________
AODHandleBase::~AODHandleBase()
{
  delete d;
  --Imp::nobjhandles;
}

//____________________________________________________________________
int AODHandleBase::numberOfInstances()
{
  return Imp::nobjhandles;
}

//____________________________________________________________________
AODSysCommonData * AODHandleBase::common() const
{
  return m_collhandle->common();
}

//____________________________________________________________________
void AODHandleBase::setVisible(bool vis)
{
  QString tmp = (vis==true)?"True":"False";
  QString tmp2 = (m_visible==true)?"True":"False";
  VP1Msg::messageDebug(QString("AODHandleBase calling setVisible with vis=")+tmp+QString(", and m_visible=")+tmp2 );
  if (vis==m_visible)
    return;
 
  m_visible=vis;
  if (vis) {
    m_collhandle->incrementNShownHandles();
 
    if (!has3DObjects())
      rebuild3DObjects();//The call to rebuild also fixes attached state.
    else
      attach3DObjects();
  } else {
    m_collhandle->decrementNShownHandles();
    detach3DObjects();
  }

  visibleStateChanged();
}


//____________________________________________________________________
void AODHandleBase::update3DObjects()
{
  if (m_visible) {
    rebuild3DObjects();
  } else {    
    //Simply clear the present 3D objects. They will only be recreated if/when the track/jet becomes visible again.
    clear3DObjects();
  }
}


//____________________________________________________________________
void AODHandleBase::rebuild3DObjects()
{  
  QString tmp2 = (m_visible==true)?"True":"False";
  VP1Msg::messageDebug("AODHandleBase::rebuild3DObjects() - with m_visible="+tmp2 );
  
  //Ensure we are always detached while updating.
  if (m_visible) {
    detach3DObjects();
  }

  clear3DObjects();
  nodes(); // rebuilds the 3D shapes

  //Attach if visible:
  if (m_visible) {
    attach3DObjects();
  }
}

//____________________________________________________________________
void AODHandleBase::attach3DObjects()
{
  VP1Msg::messageVerbose("AODHandleBase::attach3DObjects");

  if (!m_currentmaterial) {
    m_currentmaterial = determineMaterial();
    m_currentmaterial->ref();
    currentMaterialChanged();
  }
  if (has3DObjects() && m_collhandle->sepHelper()) {
    VP1Msg::messageVerbose("Adding nodes to sep helper...");
  
    collHandle()->common()->registerHandle(this); 

    // debug
    // std::cout<<"m_collhandle->sepHelper() = " << m_collhandle->sepHelper()<<std::endl;
    SoNode* tmp = nodes();
    // std::cout << "tmp nodes = "<< tmp << std::endl;

    //		m_collhandle->sepHelper()->addNodeUnderMaterial( nodes(), m_currentmaterial );
    m_collhandle->sepHelper()->addNodeUnderMaterial( tmp, m_currentmaterial );

  } else {
    if (!has3DObjects()) {
      VP1Msg::message("AODHandleBase::attach3DObjects() - No 3D objects to attach!");
    }
    if (!m_collhandle->sepHelper()) VP1Msg::message("AODHandleBase::attach3DObjects() - No sep helper!");
  }
  // if (label_sep && theclass->common()->textSep() && theclass->common()->controller()->doTrackLabels())
  //   theclass->common()->textSep()->addChild(label_sep);
  //
}

//____________________________________________________________________
void AODHandleBase::detach3DObjects()
{
  VP1Msg::messageVerbose("AODHandleBase::detach3DObjects");
	
  if (!m_currentmaterial)
    return;//Can never have been attached!
  if (has3DObjects() && m_collhandle->sepHelper()) {
    m_collhandle->sepHelper()->removeNodeUnderMaterial(nodes(),m_currentmaterial);
  }
  
  collHandle()->common()->deregisterHandle(this);
  
  // if (label_sep && theclass->common()->textSep()) 
  //   theclass->common()->textSep()->removeChild(label_sep);
}

//____________________________________________________________________
void AODHandleBase::updateMaterial()
{
  VP1Msg::messageVerbose("AODHandleBase::updateMaterial()");
	
  //We need to change the current material. This means we need to
  //clear the cache. Only if we are visible do we need to do a
  //detach->clear material->attach cycle to trigger attachment under new material.

  if (!m_currentmaterial)
    return;//We have no material cached and is thus not attached either.
  if (m_visible) {
    //See if the material changed. If it did, detach, update the material, attach again.
    SoMaterial * newmat = determineMaterial();
    newmat->ref();
    if (newmat!=m_currentmaterial) {
      detach3DObjects();
      m_currentmaterial->unref();
      m_currentmaterial = newmat;
      attach3DObjects();
      currentMaterialChanged();
    } else {
      newmat->unref();
    }
  } else {
    //Just clear material.
    m_currentmaterial->unref();
    m_currentmaterial = 0;
    currentMaterialChanged();
  }
}

////____________________________________________________________________
//double AODHandleBase::mass() const
//{
//  return 0.0;
//}


//____________________________________________________________________
QStringList AODHandleBase::baseInfo() const
{
  VP1Msg::messageDebug("AODHandleBase::baseInfo()");

  QStringList l;


  //  Amg::Vector3D mom = momentum()/1000;
  //  if (mom.mag2()==0.0) {
  //    l << "Momentum : 0 (undefined)";
  //  } else {
  //    l << "Momentum [GeV]: "+VP1Msg::str(mom);
  //    l << "|Pt|/|P| [GeV]: "+VP1Msg::str(mom.perp())+" / " + VP1Msg::str(mom.mag());
  //    l << VP1Msg::str("(")+QChar(0x03B7)+","+QChar(0x03D5)+VP1Msg::str(")=(")
  //        +VP1Msg::str(mom.eta())+VP1Msg::str(",")+VP1Msg::str(VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() ))+VP1Msg::str(")");
  //
  //    l << "Eta: "+VP1Msg::str(mom.eta());
  //    l << "Phi: "+VP1Msg::str(VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() ));
  //  }
  // l<<"Hit summary: Pix["+VP1Msg::str(getNPixelHits())+"], SCT["+VP1Msg::str(getNSCTHits())+"], TRT["+VP1Msg::str(getNTRTHits())
  //   +"], MDT["+QString::number(getNMDTHits())+"], RPC["+QString::number(getNRPCHits())+"], TGC["+QString::number(getNTGCHits())+"], CSC["+QString::number(getNCSCHits())+"].";
  // 
  // return l;
  // int pdg = pdgCode();
  // if (pdg) {
  //   bool ok;
  //   QString name = VP1ParticleData::particleName(pdg,ok);
  //   if (ok)
  //     l << "pdg: "+QString::number(pdg)+" ["+name+"]";
  //   else
  //     l << "pdg: "+QString::number(pdg);
  // }

  if (l.length() == 0)
    l << "baseInfo : (undefined)";

  return l;
}


//____________________________________________________________________
void AODHandleBase::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) {

  VP1Msg::messageDebug("AODHandleBase::fillObjectBrowser()");

  if (d->m_objBrowseTree){
    VP1Msg::message("ERROR: AODHandleBase::fillObjectBrowser - already have m_objBrowseTree. Aborting.");
    return;
  }
	
  d->m_objBrowseTree = new QTreeWidgetItem();

  QString l = shortInfo();

  d->m_objBrowseTree->setText(0, type()+QString(QString::number(listOfItems.size())) );
  d->m_objBrowseTree->setText(1, l );    

  if (!visible()) {
    d->m_objBrowseTree->setFlags(0); // not selectable, not enabled
  }
  listOfItems << browserTreeItem();
}

//____________________________________________________________________
QTreeWidgetItem* AODHandleBase::browserTreeItem() const {
  return d->m_objBrowseTree;
}

//____________________________________________________________________
void AODHandleBase::visibleStateChanged()
{  
  if ( !browserTreeItem()) {
    VP1Msg::messageVerbose("visibleStateChanged: No m_objBrowseTree!");
    return;
  }
    
  if (!visible()) {
    browserTreeItem()->setFlags(0); // not selectable, not enabled
  } else {
    browserTreeItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); //  selectable,  enabled
  }  
  QFont itemFont = browserTreeItem()->font(0);
  itemFont.setStrikeOut(!visible());
  browserTreeItem()->setFont(0, itemFont);
  browserTreeItem()->setFont(1, itemFont);
}

