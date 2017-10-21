/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1VertexCollection               //
//                                                            //
//  Author: Andreas.Wildauer@cern.ch                          //
//          Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//          Ben Zastovnik                                     //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1VertexSystems/VP1VertexCollection.h"
#include "VP1VertexSystems/VP1VertexSystem.h"
#include "VP1VertexSystems/VertexSysController.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1MaterialButton.h"

#include <QStringList>
#include <QComboBox>

#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoMatrixTransform.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/SbMatrix.h>
#include <Inventor/SoPath.h>

#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/VxContainer.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrackLink/ITrackLink.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <iostream>


//____________________________________________________________________
QList<VP1StdCollection*> VP1VertexCollection::createCollections(VertexSysController*controller)
{
  QList<VP1StdCollection*> l;
  foreach (QString key, VP1SGContentsHelper(controller->systemBase()).getKeys<VxContainer>()) {
    VP1VertexCollection * col = new VP1VertexCollection(controller,key);
    col->init();
    l << col;
  }
  return l;
}

//____________________________________________________________________
class VP1VertexCollection::Imp {
public:
  VP1VertexCollection * theclass;
  VertexSysController * controller;
  QString key;

  //  std::map <SoNode *, const Trk::VxCandidate *> nodeToVertexMap;//fixme: obsolete
  void findAssociatedTracks(const Trk::VxCandidate * vtx,
			    QList<const Trk::Track*>& tracks,
			    QList<const Trk::TrackParticleBase*>& trackparticles);//fixme: use

  class VertexHandle {
  public:
    VertexHandle(const Trk::VxCandidate* v , VP1VertexCollection* collHandle) : m_collHandle(collHandle), m_attached(false), m_vertex(v), m_sep(0), m_randommaterial(0) {}
    ~VertexHandle() { if (m_sep) m_sep->unref(); }

    VP1VertexCollection* collHandle() {return m_collHandle;}
    
    void updateShapes(VertexSysController * controller) {
      if (!m_attached) {
        if (m_sep)
          m_sep->removeAllChildren();
        return;
      }
      if (!m_sep) {
        m_sep = new SoSeparator;
        m_sep->ref();
      }
      if (m_sep->getNumChildren()>0)
        m_sep->removeAllChildren();
      SoMaterial* mat = determineMaterial();
      // std::cout<<"This: "<<this<<" Got material at:"<<mat<<", m_randommaterial="<<m_randommaterial<<std::endl;
      m_sep->addChild ( mat );
      actualBuildShape(controller);
    }
    
    SoMaterial * determineMaterial() {
      switch(collHandle()->colourBy()) {
        case VP1VertexCollection::COLOUR_RANDOM:
        {
          if (!m_randommaterial) {
            m_randommaterial = new SoMaterial;
            m_randommaterial->ref();
            rerandomiseRandomMaterial();
          }
          return m_randommaterial;
        }
        case VP1VertexCollection::COLOUR_PERCOLLECTION:
        default:
          return collHandle()->material();
        }
    }

    void ensureAttached(SoSeparator * collSep,VertexSysController * controller) {
      if (m_attached)
        return;
      m_attached = true;
      updateShapes(controller);
      collSep->addChild(m_sep);
    }
    void ensureDetached(SoSeparator * collSep) {
      if (!m_attached)
        return;
      m_attached = false;
      if (m_sep)
        collSep->removeChild(m_sep);
    }

    const Trk::VxCandidate * vertex() const { return m_vertex; }
    SoSeparator * sep() const { return m_sep; }
    bool attached() const { return m_attached; }
    VertexCommonFlags::ReconVertexTypeFlag type() {
      switch(m_vertex->vertexType()) {
      case Trk::PriVtx: return VertexCommonFlags::RVT_Primary;
      case Trk::SecVtx: return VertexCommonFlags::RVT_Secondary;
      case Trk::PileUp: return VertexCommonFlags::RVT_Pileup;
      case Trk::ConvVtx: return VertexCommonFlags::RVT_Conversion;
      case Trk::KinkVtx: return VertexCommonFlags::RVT_Kink;
      case Trk::V0Lambda:
      case Trk::V0LambdaBar:
      case Trk::V0KShort:
      case Trk::V0Vtx : return VertexCommonFlags::RVT_V0;
      default:
      return VertexCommonFlags::RVT_Other;
      }
    }
    void rerandomiseRandomMaterial(){
      // std::cout<<"rerandomiseRandomMaterial"<<std::endl;
      if ( !m_randommaterial )//We will anyway rerandomize it when we need it
        return;
      
      double r2 = 0.3*0.3;
      unsigned i(0);
      double r,g,b;
      bool ok;
      while (true) {
        r = (rand() / static_cast<double>(RAND_MAX));
        g = (rand() / static_cast<double>(RAND_MAX));
        b = (rand() / static_cast<double>(RAND_MAX));
        ok = true;
        //For now we make sure that we avoid black and red. This should be updated from bgd and highlight col automatically! (fixme).
        // -> and we should probably also make sure that tracks close in (eta,phi) are well separated in colour-space.
        if ( (r-1.0)*(r-1.0)+g*g+b*b < r2*0.5 )//avoid red (distance)
          ok = false;
        else if ( r*r/(r*r+g*g+b*b) > 0.8 )//avoid red (angle)
          ok = false;
        else if ( r*r+g*g+b*b < r2*2.0 )//avoid black
          ok = false;
        if (ok)
          break;
        ++i;
        if (i>50 ) {
          r2 *= 0.99;//To avoid problem in case we add too many forbidden spheres.
          if (i>1000) {
            //Just a safety
            collHandle()->systemBase()->message("TrackHandleBase::rerandomiseRandomMaterial Warning: Random colour could"
                         " not be selected such as to satisfy all separation criteria");
            break;
          }
        }
      }
      // std::cout<<"rerandomiseRandomMaterial - setting to"<<r<<","<<g<<","<<b<<std::endl;
      
      VP1MaterialButton::setMaterialParameters(m_randommaterial,r,g,b,
                 0.15, /*brightness*/
                 0.0 /*transparency*/);
    }
    
  private:
    VP1VertexCollection* m_collHandle;
    bool m_attached;
    const Trk::VxCandidate * m_vertex;
    SoSeparator* m_sep;
    SoMaterial* m_randommaterial;
    void actualBuildShape(VertexSysController * controller) {
    	Amg::Vector3D pos = m_vertex->recVertex().position();
    	double r = controller->reconVertexRepresentation();
    	if (r==0) {
    		SoPointSet       * points    = new SoPointSet;
    		SoVertexProperty * vertices = new SoVertexProperty;
    		vertices->vertex.set1Value(0,pos.x(), pos.y(), pos.z());
    		points->numPoints=1;
    		points->vertexProperty.setValue(vertices);
    		m_sep->addChild(points);
    		return;
    	}

      SoTranslation * translation = new SoTranslation;
      translation->translation.setValue ( pos.x(), pos.y(), pos.z() );
      m_sep->addChild ( translation );

      if (r>0) {
        //Transform sphere into an ellipsoid using the error matrix:
//        const Trk::CovarianceMatrix& errMat = m_vertex->recVertex().errorPosition().covariance();
        const AmgSymMatrix(3)& errMat = m_vertex->recVertex().covariancePosition(); // see https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/VxVertex/trunk/VxVertex/RecVertex.h

        //FIXME: std::min hack for now!!
        double a(errMat(0,0)), b(errMat(0,1)), c(errMat(0,2)),
                                    d(errMat(1,1)), e(errMat(1,2)),
                                               f(errMat(2,2));
        double det = a*(d*f-e*e) + 2*b*c*e - d*c*c-f*b*b;
        if (det>0) {
          double sixthrootofdet = exp(log(det)/6.0);
          double invdet = 1.0/sixthrootofdet;
          a *= invdet;
          b *= invdet;
          c *= invdet;
          d *= invdet;
          e *= invdet;
          f *= invdet;
          SbMatrix sbMat(a,b,c,0,
             b,d,e,0,
             c,e,f,0,
             0,0,0,1);
          SoMatrixTransform * matTrans = new SoMatrixTransform();
          matTrans->matrix.setValue(sbMat);
          m_sep->addChild (matTrans);
        } else {
        //fixme: warn
        }
      }

      SoSphere * sphere = new SoSphere;
      sphere->radius = fabs(r);
      m_sep->addChild ( sphere );
    }
  };
  bool cut(VertexHandle* vh,VertexSysController * controller ) {
    return vh->type() & controller->reconCutAllowedTypes();//nb: cache allowed types val?
  }

  void recheckCut(VertexHandle* vh) {
    if (cut(vh,controller))
      vh->ensureAttached(theclass->collSep(),controller);
    else
      vh->ensureDetached(theclass->collSep());
  }

  QList<VertexHandle*> vertices;
  //Extra widgets:
  QComboBox * comboBox_colourby;
  static QString comboBoxEntry_ColourByCollection() { return "Uniform"; }
  static QString comboBoxEntry_ColourByRandom() { return "Random"; }
  
  SoMaterial * m_randommaterial;
};


//____________________________________________________________________
VP1VertexCollection::VP1VertexCollection(VertexSysController*controller,const QString& key)
  : VP1StdCollection(controller->systemBase(),"VP1VertexCollection_"+key), m_d(new Imp),
  m_colourby(COLOUR_PERCOLLECTION)
{
  m_d->theclass = this;
  m_d->controller = controller;
  m_d->key = key;
  m_d->comboBox_colourby = new QComboBox;
  connect(controller,SIGNAL(reconVertexRepresentationChanged(const double&)),this,SLOT(updateAllShapes()));
  connect(controller,SIGNAL(reconCutAllowedTypesChanged(VertexCommonFlags::ReconVertexTypeFlags)),this,SLOT(recheckAllCuts()));
  connect(this,SIGNAL(tracksFromVertexChanged(QList<std::pair<const SoMaterial*,QList<const Trk::Track*> > >&)),
          dynamic_cast<VP1VertexSystem*>(controller->systemBase()),SLOT(updateVertexToTracks(QList<std::pair<const SoMaterial*,QList<const Trk::Track*> > >&))); // FIXME - only for data!

}

//____________________________________________________________________
VP1VertexCollection::~VP1VertexCollection()
{
  foreach(Imp::VertexHandle* vh,m_d->vertices)
    delete vh;
    
  // delete m_d->comboBox_colourby; DONE by baseclass
  delete m_d;
}

void VP1VertexCollection::init(VP1MaterialButtonBase*)
{
  VP1StdCollection::init();//this call is required

  // setupSettingsFromController(common()->controller());

  //Setup colour by:
  m_d->comboBox_colourby->setToolTip("Determine how vertices from this collection are coloured.");
  QStringList l;
  l << Imp::comboBoxEntry_ColourByCollection();
  l << Imp::comboBoxEntry_ColourByRandom();
  m_d->comboBox_colourby->addItems(l);
  m_d->comboBox_colourby->setCurrentIndex(0);//corresponds to per collection.
  connect(m_d->comboBox_colourby,SIGNAL(currentIndexChanged(int)),
          this,SLOT(colourByComboBoxItemChanged()));
  
  connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));
  setColourBy(defaultColourBy());
  
  connect(m_d->controller,SIGNAL(rerandomise()),this,SLOT(rerandomiseRandomVertexColours()));
  
}

void VP1VertexCollection::setColourBy( VP1VertexCollection::COLOURBY cb )
{
  messageVerbose("setColourBy called");
  if (m_colourby==cb)
    return;
  messageVerbose("setColourBy ==> Changed");
  m_colourby=cb;

  //Update gui combobox:
  QString targetText;
  switch(cb) {
  case COLOUR_RANDOM:
    targetText = Imp::comboBoxEntry_ColourByRandom();
    break;
  default:
  case COLOUR_PERCOLLECTION:
    targetText = Imp::comboBoxEntry_ColourByCollection();
    break;
  }
  if (targetText!=m_d->comboBox_colourby->currentText()) {
    int i = m_d->comboBox_colourby->findText(targetText);
    if (i>=0&&i<m_d->comboBox_colourby->count()) {
      bool save = m_d->comboBox_colourby->blockSignals(true);
      m_d->comboBox_colourby->setCurrentIndex(i);
      m_d->comboBox_colourby->blockSignals(save);
    } else {
      message("ERROR: Problems finding correct text in combo box");
    }
  }

  // //Actual material updates (could do this more efficiently by just updating material, but shouldn't matter for vertices)
  updateAllShapes();  
  
  // Now work out what colours to send to the track system
  QList<const Trk::Track*> tracks;
  QList<const Trk::TrackParticleBase*> trackparticles;
  QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > > colAndTracks;
  foreach(Imp::VertexHandle* vh,m_d->vertices){
    m_d->findAssociatedTracks(vh->vertex(),tracks,trackparticles);
    const SoMaterial* mat = vh->attached()?vh->determineMaterial():0; // store either the material, or 0 for hidden vertices
    colAndTracks.append(std::pair<const SoMaterial*, QList< const Trk::Track*> >(mat, tracks));
  }
  emit tracksFromVertexChanged(colAndTracks);
  
}

//____________________________________________________________________
QList<QWidget*> VP1VertexCollection::provideExtraWidgetsForGuiRow() const
{
  return QList<QWidget*>() << m_d->comboBox_colourby;

}

//____________________________________________________________________
void VP1VertexCollection::colourByComboBoxItemChanged()
{
  messageVerbose("Collection detail level combo box changed index");
  if (m_d->comboBox_colourby->currentText()==Imp::comboBoxEntry_ColourByRandom())
    setColourBy(COLOUR_RANDOM);
  else
    setColourBy(COLOUR_PERCOLLECTION);
}

//____________________________________________________________________
QString VP1VertexCollection::provideText() const
{
  return m_d->key;
}

//____________________________________________________________________
void VP1VertexCollection::Imp::findAssociatedTracks(const Trk::VxCandidate * vtx,
						    QList<const Trk::Track*>& tracks,
						    QList<const Trk::TrackParticleBase*>& trackparticles)
{
  tracks.clear();
  trackparticles.clear();
  if (!vtx)
    return;

  const std::vector<Trk::VxTrackAtVertex*> * tmpVxTAVtx = vtx->vxTrackAtVertex();
  for ( std::vector<Trk::VxTrackAtVertex*>::const_iterator itr = tmpVxTAVtx->begin(); itr != tmpVxTAVtx->end(); itr++ ) {
    Trk::ITrackLink * link = *itr ? (*itr)->trackOrParticleLink() : 0;
    if (!link) continue;
    
    //Attempt to get Trk::TrackParticleBase:

    Trk::LinkToTrackParticleBase* linkToTrackParticle = dynamic_cast<Trk::LinkToTrackParticleBase*>(link);
    const Trk::TrackParticleBase * tp((linkToTrackParticle && linkToTrackParticle->isValid())?linkToTrackParticle->cachedElement():0);

    //Attempt to get Trk::Track - directly or from the track particle:
    const Trk::Track* trk(0);
    if (tp) {
      trk = tp->originalTrack();
    } else {
      Trk::LinkToTrack* linkToTrack = dynamic_cast<Trk::LinkToTrack*>(link);
      if (linkToTrack && linkToTrack->isValid())
        trk = linkToTrack->cachedElement();
    }

    //Store:
    if (tp)
      trackparticles << tp;
    if (trk)
      tracks << trk;
  }

}

//____________________________________________________________________
void VP1VertexCollection::assignDefaultMaterial(SoMaterial*m) const
{
  VP1MaterialButton::setMaterialParameters( m, 0.66667/*red*/, 1.0/*green*/, 0.0/*blue*/, 0.15 /*brightness*/ );
}

//____________________________________________________________________
void VP1VertexCollection::recheckAllCuts()
{
  largeChangesBegin();
  QList<const Trk::Track*> tracks;
  QList<const Trk::TrackParticleBase*> trackparticles;
  QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > > colAndTracks;
  foreach(Imp::VertexHandle* vh,m_d->vertices){
    m_d->recheckCut(vh);
    m_d->findAssociatedTracks(vh->vertex(),tracks,trackparticles);
    const SoMaterial* mat = vh->attached()?vh->determineMaterial():0; // store either the material, or 0 for hidden vertices
    colAndTracks.append(std::pair<const SoMaterial*, QList< const Trk::Track*> >(mat, tracks));
  }
  emit tracksFromVertexChanged(colAndTracks);
  largeChangesEnd();
}

//____________________________________________________________________
void VP1VertexCollection::updateAllShapes()
{
  messageVerbose("VP1VertexCollection::updateAllShapes()");
  
  largeChangesBegin();
  foreach(Imp::VertexHandle* vh,m_d->vertices)
    vh->updateShapes(m_d->controller);
  largeChangesEnd();
}

//____________________________________________________________________
bool VP1VertexCollection::load()
{
  const VxContainer* vxContainer;
  if (!VP1SGAccessHelper(systemBase()).retrieve(vxContainer, m_d->key))
    return false;

  int i(0);
  for ( VxContainer::const_iterator vtxItr=vxContainer->begin(); vtxItr != vxContainer->end() ; vtxItr++ ) {
    const Trk::VxCandidate* theVxCandidate = *vtxItr;
    if (!theVxCandidate)
      continue;
    m_d->vertices << new Imp::VertexHandle(theVxCandidate, this);
    if (!(i++%20))
      systemBase()->updateGUI();
  }

  recheckAllCuts();
  //TODO: Option in controller for colouring first vertex in the
  //collection (the first is the identified primary vertex), with a
  //distinct colour.

  return true;
}

//____________________________________________________________________
QStringList VP1VertexCollection::infoOnClicked(SoPath* pickedPath)
{
  static_cast<IVP13DSystem*>(systemBase())->deselectAll();

  //Get Trk::VxCandidate pointer associated with the clicked node:
  while (pickedPath->getLength()>3&&pickedPath->getNodeFromTail(0)->getTypeId()!=SoSeparator::getClassTypeId())
    pickedPath->pop();
  if (pickedPath->getNodeFromTail(0)->getTypeId()!=SoSeparator::getClassTypeId())
    return QStringList() << "ERROR: Could not get vertex information for picked Node (1)";
  SoSeparator * pickedSep = static_cast<SoSeparator *>(pickedPath->getNodeFromTail(0));

  Imp::VertexHandle*vertexHandle(0);
  foreach (Imp::VertexHandle*vh,m_d->vertices) {
    if (vh->sep()==pickedSep) {
      vertexHandle = vh;
      break;
    }
  }
  if (!vertexHandle)
    return QStringList() << "ERROR: Could not get vertex information for picked Node (1)";
  const Trk::VxCandidate * vtx = vertexHandle->vertex();

  QStringList l;
  if (m_d->controller->printInfoOnClick()) {

    //Make output:
    l <<"Reconstructed vertex from collection "+text()+":" ;
    l << "--Position [CLHEP::mm]: ("+str(vtx->recVertex().position().x()/CLHEP::mm)+", "+
      str(vtx->recVertex().position().y()/CLHEP::mm)+", "+str(vtx->recVertex().position().z()/CLHEP::mm)+")";

    //Track associations. (in the future we will use them to tell the
    //track system to e.g. colour tracks by vertex - for now just print the number of those):
    QList<const Trk::Track*> tracks;
    QList<const Trk::TrackParticleBase*> trackparticles;
    m_d->findAssociatedTracks(vtx,tracks,trackparticles);
    l << "--#Tracks used: "+str(vtx->vxTrackAtVertex()->size());
    if (m_d->controller->printVerboseInfoOnClick()) {
      l << "--#Trk::Track's: "+str(tracks.count());
      l << "--#Trk::TrackParticleBase's: "+str(trackparticles.count());
      l <<"======== Dump ========";
      std::ostringstream s;
      vtx->dump(s);
      l << QString(s.str().c_str()).split('\n');
      l <<"======================";
    }
  }

  if (m_d->controller->zoomOnClick()) {
    std::set<SoCamera*> cameras = static_cast<IVP13DSystem*>(systemBase())->getCameraList();
    std::set<SoCamera*>::iterator it,itE = cameras.end();
    for (it=cameras.begin();it!=itE;++it)
      VP1CameraHelper::animatedZoomToPath(*it,collSep(),pickedPath,2.0,1.0);
  }

  return l;
}

//____________________________________________________________________
void VP1VertexCollection::collVisibilityChanged(bool vis)
{
  if (verbose())
    messageVerbose("VP1VertexCollection::collVisibilityChanged => "+str(vis));
  if (vis) {
    recheckAllCuts();
  } else {
    QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > > colAndTracks;
    emit tracksFromVertexChanged(colAndTracks);
  }
} 

//____________________________________________________________________
void VP1VertexCollection::rerandomiseRandomVertexColours()
{
  if (!isLoaded())
    return;
  messageVerbose("rerandomiseRandomVertexColours start");
  largeChangesBegin();
  foreach(Imp::VertexHandle* vh,m_d->vertices)
    vh->rerandomiseRandomMaterial();
  largeChangesEnd();
  messageVerbose("rerandomiseRandomVertexColours end");
}
