/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDCollHandleBase.h"
#include "VP1PRDSystems/PRDSysCommonData.h"
#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1PRDSystems/VP1PrepRawDataSystem.h"
#include "VP1PRDSystems/PRDSystemController.h"
#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"

#include "EventPrimitives/EventPrimitives.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoMaterial.h>

#include <QComboBox>

//____________________________________________________________________
class PRDCollHandleBase::Imp {
public:
  PRDCollHandleBase * theclass;

  //Collection definition:
  QString storegate_key;
  PRDDetType::Type detType;

  template <class T>
  bool actualLoad();//Templated according to collection type.

  VP1ExtraSepLayerHelper * sephelper_detail;
  VP1ExtraSepLayerHelper * sephelper_simple;
  SoSeparator * sep_lods;
  std::map<int,std::pair<SoLevelOfDetail*,std::pair<VP1ExtraSepLayerHelper*,VP1ExtraSepLayerHelper*> > > regionindex2lodhelpers;//idx->(sephelperdetailed,sephelpersimple)
  DETAIL generalprddetaillevel;
  void updateDetailSepAttachments();

  SoMaterial * highlightmaterial;
  bool highlightoutliers;
  bool drawerrors;
  bool drawrdos;

  VP1Interval allowedEta;
  QList<VP1Interval> allowedPhi;

  bool ensureLoaded();//first call each event will attempt to load
              //PRDs from storegate. Returns false in case of
              //problems this events. (This is inexpensive to
              //call)

  //Vector of prd handles:
  std::vector<PRDHandleBase*> prdhandles;

  bool etaPhiCut(PRDHandleBase*);

  //Extra widgets:
  QComboBox * comboBox_detailLevel;
};

//____________________________________________________________________
PRDCollHandleBase::PRDCollHandleBase(PRDDetType::Type type,PRDSysCommonData*cd,const QString& key)
  : VP1StdCollection(cd->system(),"PRDCollHandle_"+PRDDetType::typeToString(type)+"_"+key), m_d(new Imp), m_common(cd),m_nshownhandles(0),
  m_colourmethod(ByTechOnly), m_highlightweight(999999.0)
{
  m_d->theclass = this;
  m_d->detType = type;
  m_d->storegate_key = key;
  m_d->highlightmaterial = 0;
  m_d->highlightoutliers = false;
  m_d->drawerrors = false;
  m_d->drawrdos = false;
  m_d->sephelper_detail = 0;
  m_d->sephelper_simple = 0;
  m_d->sep_lods = 0;
  m_d->comboBox_detailLevel = new QComboBox;
  m_d->comboBox_detailLevel->setToolTip("Level of realism in representation"
    " (\"Auto\" switches mode based on distance to camera)");
  m_d->comboBox_detailLevel->addItems(QStringList()<<"Low"<<"Auto"<<"High");//Low==simple, High==detailed. Don't change order.
  m_d->generalprddetaillevel = SIMPLE;
  m_d->comboBox_detailLevel->setCurrentIndex(0);//corresponds to simple.
  connect(m_d->comboBox_detailLevel,SIGNAL(currentIndexChanged(int)),this,SLOT(detailComboBoxItemChanged()));
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));
}

//____________________________________________________________________
void PRDCollHandleBase::init(VP1MaterialButtonBase*)
{
  VP1StdCollection::init();//this call is required

  SoSeparator * sep_detail = new SoSeparator;
  sep_detail->setName("PRDColl_sep_detail");
  SoSeparator * sep_simple = new SoSeparator;
  sep_simple->setName("PRDColl_sep_simple");
  m_d->sep_lods = new SoSeparator;
  m_d->sep_lods->setName("PRDColl_sep_lods");
  sep_detail->ref();
  sep_simple->ref();
  m_d->sep_lods->ref();

  m_d->sephelper_detail = new VP1ExtraSepLayerHelper(sep_detail,128);
  m_d->sephelper_simple = new VP1ExtraSepLayerHelper(sep_simple,128);

  setupSettingsFromController(common()->controller());

  //Setup detail level:
  setGeneralPRDDetailLevel(defaultDetailLevel());
  m_d->updateDetailSepAttachments();

}


//____________________________________________________________________
PRDCollHandleBase::~PRDCollHandleBase()
{
  //Lod sep-helpers:
  std::map<int,std::pair<SoLevelOfDetail*,std::pair<VP1ExtraSepLayerHelper*,VP1ExtraSepLayerHelper*> > >::iterator it, itE = m_d->regionindex2lodhelpers.end();
  for (it = m_d->regionindex2lodhelpers.begin();it!=itE;++it) {
    delete it->second.second.first;
    delete it->second.second.second;
    it->second.first->unref();
  }
  m_d->regionindex2lodhelpers.clear();

  //Cleanup separators:
  if (m_d->sephelper_detail) {
    SoSeparator * sep_detail = m_d->sephelper_detail->topSeparator();
    delete m_d->sephelper_detail;
    sep_detail->unref();
  }
  if (m_d->sephelper_simple) {
    SoSeparator * sep_simple = m_d->sephelper_simple->topSeparator();
    delete m_d->sephelper_simple;
    sep_simple->unref();
  }
  if (m_d->sep_lods)
    m_d->sep_lods->unref();

  cleanupPtrContainer(m_d->prdhandles);

  if (m_d->highlightmaterial)
    m_d->highlightmaterial->unref();

  delete m_d;
}

//____________________________________________________________________
bool PRDCollHandleBase::load() {
  bool safeToLoad(false);
  switch (m_d->detType){
    case PRDDetType::TRT:
    safeToLoad = VP1JobConfigInfo::hasTRTGeometry();
    break;
    case PRDDetType::Pixel:
    safeToLoad = VP1JobConfigInfo::hasPixelGeometry();
    break;
    case PRDDetType::SCT:
    safeToLoad = VP1JobConfigInfo::hasSCTGeometry();
    break;
    case PRDDetType::CSC:
    case PRDDetType::CSCstrip:
    case PRDDetType::RPC:
    case PRDDetType::TGC:
    case PRDDetType::MDT:
    case PRDDetType::MM:
    case PRDDetType::sTGC:
    safeToLoad = VP1JobConfigInfo::hasMuonGeometry();
    break;
    default:
    safeToLoad = false;
  }

  if (!safeToLoad){
    message("Required geometry not enabled in job.");
    return false;
  }

  switch (m_d->detType){
    case PRDDetType::TRT:   return m_d->actualLoad<InDet::TRT_DriftCircleContainer>();
    case PRDDetType::Pixel: return m_d->actualLoad<InDet::PixelClusterContainer>();
    case PRDDetType::SCT:   return m_d->actualLoad<InDet::SCT_ClusterContainer>();
    case PRDDetType::CSC:   return m_d->actualLoad<Muon::CscPrepDataContainer>();
    case PRDDetType::CSCstrip:   return m_d->actualLoad<Muon::CscStripPrepDataContainer>();
    case PRDDetType::RPC:   return m_d->actualLoad<Muon::RpcPrepDataContainer>();
    case PRDDetType::TGC:   return m_d->actualLoad<Muon::TgcPrepDataContainer>();
    case PRDDetType::MDT:   return m_d->actualLoad<Muon::MdtPrepDataContainer>();
    case PRDDetType::MM:    return m_d->actualLoad<Muon::MMPrepDataContainer>();
    case PRDDetType::sTGC:  return m_d->actualLoad<Muon::sTgcPrepDataContainer>();
    //SpacePoints implements their own load.
    default:
    return false;
  }
}

//____________________________________________________________________
void PRDCollHandleBase::collVisibilityChanged(bool vis)
{
  if (verbose())
    messageVerbose("TrackCollHandleBase::collVisibilityChanged => "+str(vis));
  if (vis)
    recheckCutStatusOfAllNotVisibleHandles();//Fixme -> ofallhandles? All must be not visible anyway...
  else
    recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
template <class T>
bool PRDCollHandleBase::Imp::actualLoad()
{
  /////////////////////////////////////////////////////////////////////////
  // Retrieve element container from event store:
  const T* container;
  VP1SGAccessHelper sg_access(theclass->systemBase());
  if(!sg_access.retrieve(container,storegate_key))
    return false;

  typename T::const_iterator element, lastElement(container->end());

  /////////////////////////////////////////////////////////////////////////
  // Sanity check:
  if ( container->size()==0 && container->begin()!=lastElement ) {
    theclass->message("ERROR: container '"+storegate_key+"' has size()==0, but different begin() and end() iterators!");
    return false;
  }

  /////////////////////////////////////////////////////////////////////////
  // In case the container was created incorrectly we must fallback to a different method of getting the prds:
  bool fallback(false);
  const DataHandle<typename T::base_value_type> firstElementD, lastElementD;
  if ( container->size()!=0 && container->begin()==lastElement ) {
    //This is either an empty container, or it is a case of an incorrectly created container which we must get by datahandles.
    theclass->messageDebug("Retrieved empty container. This might be misleading. Now going to attempt to load prds via datahandles instead.");
    if (VP1SGContentsHelper(theclass->systemBase()).getKeys<T>().count()!=1) {
      theclass->messageDebug("But seems that there is not exactly one collection of type "+QString(typeid(T).name())
        +", so we won't attempt that anyway!! Thus we assume there there are simply no prd's.");
    } else {
      if(sg_access.retrieve(firstElementD, lastElementD,true)) {
        if (firstElementD==lastElementD) {
          theclass->messageDebug("No prd's found when accessed by datahandles either. It seems that there really are just no prds in this collection.");
        } else {
          fallback = true;
        }
      } else {
        theclass->messageDebug("Failed retrieval by datahandles. We take that as a sign that there really are just no prds in this collection ");
      }
    }
  }

  theclass->messageVerbose("Loop over actual prd's and create handles");
  //Retrieve prds and create handles:

  int ignoredUnsafeHandle_NoPRD(0);
  int ignoredUnsafeHandle_NoDetElem(0);
  int ignoredUnsafeHandle_NotSane(0);
  typename T::base_value_type::const_iterator prd, prdLast;
  int iprds(0);
  if (!fallback) {
        // element = container->begin();
        //         prd = (*element)->begin();
        //         ++iprds;
        //         PRDHandleBase * handle = theclass->addPRD(*prd);
        //         theclass->addHandle(handle);
        //         theclass->common()->registerPRD2Handle(*prd,handle);  
        //               theclass->systemBase()->updateGUI();
    for ( element = container->begin(); element!=lastElement ; ++element) {
      prd = (*element)->begin(), prdLast = (*element)->end();
      for ( ; prd!=prdLast ; ++prd) {
        ++iprds;
        if (!*prd) {
          ++ignoredUnsafeHandle_NoPRD;
          continue;
        }
        PRDHandleBase * handle = theclass->addPRD(*prd);
        if (handle) {
          if (!handle->isSane()) ignoredUnsafeHandle_NotSane++;
          if (!handle->getPRD()->detectorElement()) {
            ++ignoredUnsafeHandle_NoDetElem;
            delete handle;
          } else {
            theclass->addHandle(handle);
            theclass->common()->registerPRD2Handle(*prd,handle);
          }
        }
      }
      if (!(iprds%100))
        theclass->systemBase()->updateGUI();
    }

  } else {

    for ( const DataHandle<typename T::base_value_type> elementD(firstElementD); elementD!=lastElementD; ++elementD ) {
      prd = (*elementD).begin(), prdLast = (*elementD).end();
      for ( ; prd!=prdLast ; ++prd) {
        ++iprds;
        if (!*prd) {
          ++ignoredUnsafeHandle_NoPRD;
          continue;
        }
        PRDHandleBase * handle = theclass->addPRD(*prd);
        if (handle) {
          if (!handle->isSane()) ignoredUnsafeHandle_NotSane++;

          if (!handle->getPRD()->detectorElement()) {
            ++ignoredUnsafeHandle_NoDetElem;
            delete handle;
          } else {
            theclass->addHandle(handle);
            theclass->common()->registerPRD2Handle(*prd,handle);
          }
        }
        if (!(iprds%100))
          theclass->systemBase()->updateGUI();
      }
    }
  }
  theclass->systemBase()->updateGUI();
  prdhandles.resize(prdhandles.size());

  if (ignoredUnsafeHandle_NoPRD)
    theclass->message("WARNING - ignoring "+str(ignoredUnsafeHandle_NoPRD)+" null prd pointer(s).");
  if (ignoredUnsafeHandle_NoDetElem)
    theclass->message("WARNING - ignoring "+str(ignoredUnsafeHandle_NoDetElem)+" prd pointer(s) with null detector elements.");
  if (ignoredUnsafeHandle_NotSane)
    theclass->message("WARNING - found "+str(ignoredUnsafeHandle_NotSane)+" prd pointer(s) which fail sanity checks (i.e. contain NaNs).");

  //Perform initialisation needed for cuts (e.g. those of the "global" type, such as requiring a number of PRDs in the same detector module):
  theclass->messageVerbose("postLoadInitialisation");
  theclass->postLoadInitialisation();

  theclass->systemBase()->updateGUI();

  //Show the handles that need to be shown:
  theclass->recheckCutStatusOfAllNotVisibleHandles();//Use this method to not get the deselectAll call

  theclass->message("Found "+QString::number(iprds)+" ("+QString::number(theclass->nShownHandles())+" shown) PRDs in container '"+storegate_key+"'");

  return true;

}

//____________________________________________________________________
void PRDCollHandleBase::Imp::updateDetailSepAttachments()
{
  SoSeparator * collsep = theclass->collSep();
  if (!collsep)
    return;
  bool save = collsep->enableNotify(false);
  if (generalprddetaillevel==PRDCollHandleBase::DETAILED) {
    //Detail
    if (collsep->findChild(sephelper_detail->topSeparator())<0)
      collsep->addChild(sephelper_detail->topSeparator());
  } else {
    //No detail
    if (collsep->findChild(sephelper_detail->topSeparator())>-1)
      collsep->removeChild(sephelper_detail->topSeparator());
  }
  if (generalprddetaillevel==PRDCollHandleBase::SIMPLE) {
    //Simple
    if (collsep->findChild(sephelper_simple->topSeparator())<0)
      collsep->addChild(sephelper_simple->topSeparator());
  } else {
    //No simple
    if (collsep->findChild(sephelper_simple->topSeparator())>-1)
      collsep->removeChild(sephelper_simple->topSeparator());
  }
  if (generalprddetaillevel==PRDCollHandleBase::AUTO) {
    //LOD
    if (collsep->findChild(sep_lods)<0)
      collsep->addChild(sep_lods);
  } else {
    //No LOD
    if (collsep->findChild(sep_lods)>-1)
      collsep->removeChild(sep_lods);
  }
  if (save) {
    collsep->enableNotify(true);
    collsep->touch();
  }
}

//____________________________________________________________________
VP1ExtraSepLayerHelper * PRDCollHandleBase::sephelperDetailedNodes() const
{
  return m_d->sephelper_detail;
}

//____________________________________________________________________
VP1ExtraSepLayerHelper * PRDCollHandleBase::sephelperSimpleNodes() const
{
  return m_d->sephelper_simple;
}

//____________________________________________________________________
void PRDCollHandleBase::detailComboBoxItemChanged()
{
  messageVerbose("Collection detail level combo box changed index");
  switch(m_d->comboBox_detailLevel->currentIndex()) {
    case 1:
    setGeneralPRDDetailLevel(AUTO);
    break;
    case 2:
    setGeneralPRDDetailLevel(DETAILED);
    break;
    default:
    case 0:
    setGeneralPRDDetailLevel(SIMPLE);
    break;
  }
}

//____________________________________________________________________
PRDCollHandleBase::DETAIL PRDCollHandleBase::detailLevel() const
{
  return m_d->generalprddetaillevel;
}

//____________________________________________________________________
void PRDCollHandleBase::setGeneralPRDDetailLevel( DETAIL dm )
{
  if (m_d->generalprddetaillevel==dm)
    return;
  m_d->generalprddetaillevel = dm;
  messageVerbose("Detail level changed");
//   common()->system()->deselectAll();

  //Update gui combobox:
  int targetIndex(0);
  switch(m_d->generalprddetaillevel) {
    case AUTO:
    targetIndex = 1;
    break;
    case DETAILED:
    targetIndex = 2;
    break;
    default:
    case SIMPLE:
    targetIndex = 0;
    break;
  }
  if (targetIndex!=m_d->comboBox_detailLevel->currentIndex()) {
    bool save = m_d->comboBox_detailLevel->blockSignals(true);
    m_d->comboBox_detailLevel->setCurrentIndex(targetIndex);
    m_d->comboBox_detailLevel->blockSignals(save);
  }

  //Actual changes to 3D representation:
  m_d->updateDetailSepAttachments();

  detailLevelChanged();

}

//____________________________________________________________________
void PRDCollHandleBase::getLODSeparators(int index, VP1ExtraSepLayerHelper*& sephelper_detail,VP1ExtraSepLayerHelper*& sephelper_simple)
{
  const float complexity = 0.3f;//Fixme: Hardcoded here.
  std::map<int,std::pair<SoLevelOfDetail*,std::pair<VP1ExtraSepLayerHelper*,VP1ExtraSepLayerHelper*> > >::iterator it = m_d->regionindex2lodhelpers.find(index);
  if (it!=m_d->regionindex2lodhelpers.end()) {
    sephelper_detail = it->second.second.first;
    sephelper_simple = it->second.second.second;
    //To try to scale somehow different regions to change at "the same time":
    it->second.first->screenArea.setValue(lodArea()*(sephelper_detail->topSeparator()->getNumChildren()+1)/(complexity+0.5f));
    return;
  }
  SoLevelOfDetail * lod = new SoLevelOfDetail;
  SoSeparator * sep_detail = new SoSeparator;
  SoSeparator * sep_simple = new SoSeparator;
  lod->addChild(sep_detail);
  lod->addChild(sep_simple);
  lod->screenArea.setValue(lodArea()/(complexity+0.5f));
  m_d->sep_lods->addChild(lod);
  sephelper_detail = new VP1ExtraSepLayerHelper(sep_detail);
  sephelper_simple = new VP1ExtraSepLayerHelper(sep_simple);

  lod->ref();
  m_d->regionindex2lodhelpers[index] =
    std::pair<SoLevelOfDetail*,std::pair<VP1ExtraSepLayerHelper*,VP1ExtraSepLayerHelper*> >
    (lod,std::pair<VP1ExtraSepLayerHelper*,VP1ExtraSepLayerHelper*>(sephelper_detail,sephelper_simple));
}

//____________________________________________________________________
void PRDCollHandleBase::addHandle(PRDHandleBase*handle)
{
  if (handle)
    m_d->prdhandles.push_back(handle);
}

//____________________________________________________________________
std::vector<PRDHandleBase*>& PRDCollHandleBase::getPrdHandles()
{
  return m_d->prdhandles;
}

//____________________________________________________________________
const std::vector<PRDHandleBase*>& PRDCollHandleBase::getPrdHandles() const
{
  return m_d->prdhandles;
}

//____________________________________________________________________
void PRDCollHandleBase::recheckCutStatus(PRDHandleBase*handle)
{
  handle->setVisible( visible() && cut(handle) && m_d->etaPhiCut(handle) );
}

//Fixme: A few of the methods of this class should be inlined (requires a few more public data members)

//____________________________________________________________________
void PRDCollHandleBase::setupSettingsFromController(PRDSystemController*controller)
{
  assert(controller);
  largeChangesBegin();

  collSep()->addChild(controller->drawOptions(m_d->detType));

  m_d->highlightmaterial = controller->getHighLightMaterial();
  m_d->highlightmaterial->ref();

  connect(controller,SIGNAL(highLightOutliersChanged(bool)),this,SLOT(setHighLightOutliers(bool)));
  setHighLightOutliers(controller->highLightOutliers());

  connect(controller,SIGNAL(colourMethodChanged(PRDCollHandleBase::COLOURMETHOD)),this,SLOT(setColourMethod(PRDCollHandleBase::COLOURMETHOD)));
  setColourMethod(controller->colourMethod());

  connect(controller,SIGNAL(drawErrorsChanged(bool)),this,SLOT(setDrawErrors(bool)));
  setDrawErrors(controller->drawErrors());

  connect(controller,SIGNAL(drawRDOsChanged(bool)),this,SLOT(setDrawRDOs(bool)));
  setDrawRDOs(controller->drawRDOs());

  connect(controller,SIGNAL(highLightMaterialWeightChanged(const double&)),this,SLOT(setHighLightWeight(const double&)));
  setHighLightWeight(controller->highLightMaterialWeight());

  connect(controller,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),
    this,SLOT(setAllowedEta(const VP1Interval&)));
  setAllowedEta(controller->cutAllowedEta());

  connect(controller,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),
    this,SLOT(setAllowedPhi(const QList<VP1Interval>&)));
  setAllowedPhi(controller->cutAllowedPhi());

  setupSettingsFromControllerSpecific(controller);
  largeChangesEnd();
}


//____________________________________________________________________
void PRDCollHandleBase::recheckCutStatusOfAllHandles()
{
  messageVerbose("PRDCollHandleBase::recheckCutStatusOfAllHandles");

  common()->system()->deselectAll();

  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(m_d->prdhandles.begin()),itE(m_d->prdhandles.end());
  int i(0);
  for (;it!=itE;++it) {
    recheckCutStatus(*it);
    if (!(i++%200))
      systemBase()->updateGUI();//since called from ::actualLoad<..>(..)
  }
  largeChangesEnd();
  if (visible()) message("Have "+QString::number(getPrdHandles().size())+" ("+QString::number(nShownHandles())+" shown) PRDs");
}

//____________________________________________________________________
void PRDCollHandleBase::recheckCutStatusOfAllVisibleHandles()
{
  messageVerbose("PRDCollHandleBase::recheckCutStatusOfAllVisibleHandles");
  if (!visible()) return;
  //This method is called when a cut is tightened - thus we better start by deselectAll to avoid weird highlighting issues.
  common()->system()->deselectAll();

  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(m_d->prdhandles.begin()),itE(m_d->prdhandles.end());
  for (;it!=itE;++it) {
    if ((*it)->visible())
      recheckCutStatus(*it);
  }
  largeChangesEnd();
  if (visible()) message("Have "+QString::number(getPrdHandles().size())+" ("+QString::number(nShownHandles())+" shown) PRDs");    
}

//____________________________________________________________________
void PRDCollHandleBase::recheckCutStatusOfAllNotVisibleHandles()
{
  messageVerbose("PRDCollHandleBase::recheckCutStatusOfAllNotVisibleHandles");

  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(m_d->prdhandles.begin()),itE(m_d->prdhandles.end());
  for (;it!=itE;++it) {
    if (!(*it)->visible())
      recheckCutStatus(*it);
  }
  largeChangesEnd();
  if (visible()) message("Have "+QString::number(getPrdHandles().size())+" ("+QString::number(nShownHandles())+" shown) PRDs");
}


//____________________________________________________________________
bool PRDCollHandleBase::highLightOutliers() const
{
  return m_d->highlightoutliers;
}

//____________________________________________________________________
bool PRDCollHandleBase::drawErrors() const
{
  return m_d->drawerrors;
}

//____________________________________________________________________
bool PRDCollHandleBase::drawRDOs() const
{
  return m_d->drawrdos;
}

//____________________________________________________________________
SoMaterial * PRDCollHandleBase::highLightMaterial() const
{
  return  m_d->highlightmaterial;
}

//____________________________________________________________________
void PRDCollHandleBase::setHighLightOutliers(bool b)
{
  if (m_d->highlightoutliers==b)
    return;
  m_d->highlightoutliers = b;
  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(m_d->prdhandles.begin()),itE(m_d->prdhandles.end());
  for (;it!=itE;++it) {
    (*it)->updateMaterial();
    //Fixme: Improve performance by only calling updateMaterial() on
    //those that are outliers on at least one track.
  }
  largeChangesEnd();
}

//____________________________________________________________________
void PRDCollHandleBase::setDrawErrors(bool b)
{
  if (m_d->drawerrors==b)
    return;
  m_d->drawerrors = b;
  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(m_d->prdhandles.begin()),itE(m_d->prdhandles.end());
  for (;it!=itE;++it) {
    (*it)->update3DObjects();
    //Fixme: Improve performance by only calling updateMaterial() on
    //those that are outliers on at least one track.
  }
  largeChangesEnd();
}

//____________________________________________________________________
void PRDCollHandleBase::setDrawRDOs(bool b)
{
  if (m_d->drawrdos==b)
    return;
  m_d->drawrdos = b;
  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(m_d->prdhandles.begin()),itE(m_d->prdhandles.end());
  for (;it!=itE;++it) {
    (*it)->update3DObjects();
    //Fixme: Improve performance by only calling updateMaterial() on
    //those that are outliers on at least one track.
  }
  largeChangesEnd();
}


//____________________________________________________________________
void PRDCollHandleBase::setHighLightWeight(const double& hlw)
{
  if (m_highlightweight == hlw)
    return;
  m_highlightweight = hlw;
  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(m_d->prdhandles.begin()),itE(m_d->prdhandles.end());
  for (;it!=itE;++it) {
    if (m_d->highlightoutliers || (*it)->highLight())
      (*it)->updateMaterial();
    //Fixme: We can improve performance here by investigating whether
    //the handle actually is an outlier for at least one track, or if
    //highlighting is at all applicable for the collection (maybe it is turned off).
  }
  largeChangesEnd();
}

//____________________________________________________________________
void PRDCollHandleBase::setColourMethod(PRDCollHandleBase::COLOURMETHOD cm)
{
  if (m_colourmethod==cm)
    return;
  m_colourmethod = cm;

  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(m_d->prdhandles.begin()),itE(m_d->prdhandles.end());
  for (;it!=itE;++it) {
    (*it)->updateMaterial();
    //Fixme: Improve performance by only calling on those that are on tracks/segments as relevant.
  }
  largeChangesEnd();

}

//____________________________________________________________________
QString PRDCollHandleBase::toString(const PRDCollHandleBase::DETAIL& d)
{
  switch (d) {
    case SIMPLE: return "SIMPLE";
    case DETAILED: return "DETAILED";
    case AUTO: return "AUTO";
    default: return "Unknown (ERROR)";
  }
}

//____________________________________________________________________
QString PRDCollHandleBase::toString(const COLOURMETHOD& cm)
{
  switch (cm) {
    case ByTechOnly: return "ByTechOnly";
    case BySegment: return "BySegment";
    case ByTrack: return "ByTrack";
    case BySegmentAndTrack: return "BySegmentAndTrack";
    default: return "Unknown (ERROR)";
  }
}

//____________________________________________________________________
qint32 PRDCollHandleBase::provideCollTypeID() const
{
  return PRDDetType::typeToInt(m_d->detType);
}

//____________________________________________________________________
QString PRDCollHandleBase::provideText() const
{
  return m_d->storegate_key;
}

//____________________________________________________________________
void PRDCollHandleBase::assignDefaultMaterial(SoMaterial*m) const
{
  VP1QtInventorUtils::setMatColor( m, defaultColor(), 0.18/*brightness*/ );
}

//____________________________________________________________________
QString PRDCollHandleBase::provideSection() const
{
  switch (m_d->detType) {
    case PRDDetType::Pixel:
    case PRDDetType::SCT:
    case PRDDetType::TRT:
    return "Inner Detector PRDs";
    case PRDDetType::SpacePoints:
    return "ID Space Points";
    case PRDDetType::CSC:
    case PRDDetType::CSCstrip:
    case PRDDetType::RPC:
    case PRDDetType::TGC:
    case PRDDetType::MDT:
    case PRDDetType::MM:
    case PRDDetType::sTGC:
    return "Muon Spectrometer PRDs";
    default: return "Unknown Section";
  }
}

//____________________________________________________________________
QString PRDCollHandleBase::provideSectionToolTip() const
{
  switch (m_d->detType) {
    case PRDDetType::Pixel:
    case PRDDetType::SCT:
    case PRDDetType::TRT:
    return "Inner Detector PRD collections in event";
    case PRDDetType::SpacePoints:
    return "Space Point collections in event";
    case PRDDetType::CSC:
    case PRDDetType::CSCstrip:
    case PRDDetType::RPC:
    case PRDDetType::TGC:
    case PRDDetType::MDT:
    return "Muon Spectrometer PRD collections in event";
    default: return "Error: PRDDetType not recognised.";
  }
}

//____________________________________________________________________
QList<QWidget*> PRDCollHandleBase::provideExtraWidgetsForGuiRow() const
{
  return QList<QWidget*>() << m_d->comboBox_detailLevel;
}

//____________________________________________________________________
QByteArray PRDCollHandleBase::extraWidgetsState() const
{
  VP1Serialise serialise(0/*version*/,systemBase());
  serialise.save(m_d->comboBox_detailLevel);
  serialise.disableUnsavedChecks();
  return serialise.result();
}

//____________________________________________________________________
void PRDCollHandleBase::setExtraWidgetsState(const QByteArray& ba)
{
  VP1Deserialise state(ba, systemBase());
  if (state.version()!=0)
    return;//just ignore silently... i guess we ought to warn?
  state.restore(m_d->comboBox_detailLevel);
  state.disableUnrestoredChecks();
  detailComboBoxItemChanged();
}

//____________________________________________________________________
void PRDCollHandleBase::setAllowedEta(const VP1Interval& e)
{
  if (m_d->allowedEta==e)
    return;
  bool relaxed(e.contains(m_d->allowedEta));
  bool tightened(m_d->allowedEta.contains(e));
  m_d->allowedEta=e;
  if (relaxed)
    recheckCutStatusOfAllNotVisibleHandles();
  else if (tightened)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllHandles();
}

//____________________________________________________________________
void PRDCollHandleBase::setAllowedPhi(const QList<VP1Interval>& l)
{
  if (m_d->allowedPhi==l)
    return;
  m_d->allowedPhi=l;
  recheckCutStatusOfAllHandles();
}

//____________________________________________________________________
bool PRDCollHandleBase::Imp::etaPhiCut(PRDHandleBase* handle)
{
  bool allPhiAllowed = allowedPhi.count()==1&&allowedPhi.at(0).isAllR();
  bool allEtaAllowed = allowedEta.isAllR();
  if (allEtaAllowed&&allPhiAllowed)
    return true;
  if (allowedPhi.isEmpty()||allowedEta.isEmpty())
    return false;
  Amg::Vector3D p = handle->center();
  if (!allEtaAllowed) {
    if (!allowedEta.contains(p.eta()))
      return false;
  }
  if (!allPhiAllowed) {
    double phi(p.phi());
    foreach(VP1Interval i,allowedPhi) {
      if (i.contains(phi)||i.contains(phi+2*M_PI)||i.contains(phi-2*M_PI))
        return true;
    }
    return false;
  }
  return true;
}
