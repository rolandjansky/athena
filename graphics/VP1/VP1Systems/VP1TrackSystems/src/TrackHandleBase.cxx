/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackHandleBase                   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1TrackSystems/TrackHandleBase.h"
#include "VP1TrackSystems/TrackCollHandleBase.h"
#include "VP1TrackSystems/TrackSystemController.h"
#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1TrackSystems/TrackPropagationHelper.h"
#include "VP1TrackSystems/TrackLODManager.h"
#include "VP1TrackSystems/AssociatedObjectHandleBase.h"
#include "VP1TrackSystems/AscObjSelectionManager.h"
#include "VP1TrackSystems/VP1TrackSanity.h"
#include "VP1TrackSystems/SimBarCode.h"//For unknown pdg. Fixme: Use 0 for unknown pdg!
#include "VP1TrackSystems/AscObj_TSOS.h"
#include "VP1TrackSystems/TrkObjToString.h"
#include "VP1Utils/VP1ParticleData.h"
#include "VP1Utils/VP1LinAlgUtils.h"

#include "VP1PRDSystems/MuonChamberProjectionHelper.h"
#include "VP1PRDSystems/TouchedMuonChamberHelper.h"
#include "VP1GuideLineSystems/InDetProjHelper.h"

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1MaterialButton.h"
#include "VP1Base/VP1QtUtils.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
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

#include "TrkVolumes/Volume.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "GeoPrimitives/AmgStringHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h" 

#include <cassert>



//____________________________________________________________________
class TrackHandleBase::Imp {
public:
  static void convertLineSetToCylinders(SoLineSet*line,SoSeparator*sep, const double& cylradius);
  static double dist(const SbVec3f& p1,const SbVec3f& p2);

  static int ntrackhandles;

  Imp(TrackHandleBase*tc) : theclass(tc),
    chargeinit(false),
    charge(TrackHandleBase::unknown()),
    massinit(false),
    mass(TrackHandleBase::unknown()),
    line(0),
    extraRepresentation(0),
    extrapSurfaces_sep(0),
    extraRepAttached(false),
    points_raw(0), points_propagated(0),
    points_raw_id_projections(0),
    points_raw_muon_projections(0),
    points_propagated_id_projections(0),
    points_propagated_muon_projections(0),
    inittouchedchambers(false),
    randommaterial(0),
    pathInfoLoaded(false),
    pathInfo_TrkTrack(0),
    pathInfo_Points(0),
    label_sep(0),
    shownTSOSParts(TrackCommonFlags::TSOS_NoObjects),
    customColouredTSOSParts(TrackCommonFlags::TSOS_NoObjects),
    tsos_ascobjs(0),
    m_objBrowseTree(0),
    tempMaxPropRadius(0.0){}
  ~Imp() { delete tsos_ascobjs; }
  TrackHandleBase * theclass;

  bool chargeinit;
  double charge;
  bool massinit;
  double mass;

  SoLineSet * line;//This represents the line(s) representing the track and its projections
  SoSeparator * extraRepresentation;//For alternative representations (e.g. tubes for lines)
  SoSeparator* extrapSurfaces_sep;//!< for debugging. FIXME!
  bool extraRepAttached;

  //For efficiency we cache parameterisations of the various track parts:

  //The next two vectors://=0 when not inited, !=0 but empty means previous attempt to init failed.
  std::vector< Amg::Vector3D > * points_raw;
  std::vector<Amg::Vector3D > * points_propagated;
  //The following sets://=0 when not inited, !=0 and ->front().vector.empty() means previous attempt to init failed.
  Amg::SetVectorVector3D * points_raw_id_projections;
  Amg::SetVectorVector3D * points_raw_muon_projections;
  Amg::SetVectorVector3D * points_propagated_id_projections;
  Amg::SetVectorVector3D * points_propagated_muon_projections;

  void ensureInitPointsRaw();
  void ensureInitPointsPropagated();
  void ensureInitPointsProjections_InDet( bool raw );
  void ensureInitPointsProjections_Muon( bool raw );

  void materialChanged();

  void rebuild3DObjects();
  void attach3DObjects();
  void detach3DObjects();

  mutable std::set<GeoPVConstLink> touchedmuonchambers;
  mutable bool inittouchedchambers;

  void addPathToSoLineSetAndSoVertexProperty(const std::vector<Amg::Vector3D >& points,
    int & iver, int& numlines, SoLineSet *, SoVertexProperty * );
  void addPathsToSoLineSetAndSoVertexProperty(const Amg::SetVectorVector3D * paths,
    int & iver, int& numlines,SoLineSet *, SoVertexProperty * );

  SoMaterial * determineMaterial();
  SoMaterial * randommaterial;

  QList<AssociatedObjectHandleBase*> associatedObjects;
  std::map<std::pair<int,double>,AssocObjAttachmentHandle *> attachmentHandles;

  //We cache path info, but it might get invalidated (if switching off simhits for instance):
  bool pathInfoLoaded;
  const Trk::Track * pathInfo_TrkTrack;
  const std::vector< Amg::Vector3D > * pathInfo_Points;
  SoSeparator * label_sep;
  void ensureLoadPathInfo();

  static bool isSane(const Amg::Vector3D& p)
  {
    if (p.x()!=p.x()||fabs(p.x())>1.0*CLHEP::km)
      return false;
    if (p.y()!=p.y()||fabs(p.y())>1.0*CLHEP::km)
      return false;
    if (p.z()!=p.z()||fabs(p.z())>1.0*CLHEP::km)
      return false;
    return true;
  }


  TrackCommonFlags::TSOSPartsFlags shownTSOSParts;
  TrackCommonFlags::TSOSPartsFlags customColouredTSOSParts;
  std::vector<AscObj_TSOS*> * tsos_ascobjs;
  void ensureInitTSOSs();
  AscObj_TSOS* addTSOS(const Trk::TrackStateOnSurface * tsos,unsigned index);

  QTreeWidgetItem* m_objBrowseTree;
  float tempMaxPropRadius;
};


int TrackHandleBase::Imp::ntrackhandles = 0;

//____________________________________________________________________
void TrackHandleBase::Imp::ensureLoadPathInfo()
{
  if (pathInfoLoaded)
    return;
  pathInfoLoaded = true;
  pathInfo_TrkTrack = theclass->provide_pathInfoTrkTrack();
  if (pathInfo_TrkTrack)
    return;
  pathInfo_Points = theclass->provide_pathInfoPoints();
  if (!pathInfo_Points) {
    theclass->collHandle()->systemBase()->message("ERROR: No path information for track in collection "+theclass->collHandle()->name());
  }
}

//____________________________________________________________________
TrackHandleBase::TrackHandleBase(TrackCollHandleBase*ch)
  : m_d(new Imp(this)), m_visible(false), m_collhandle(ch), m_currentmaterial(0)
{
  assert(m_collhandle);
  ++Imp::ntrackhandles;
}

//____________________________________________________________________
TrackHandleBase::~TrackHandleBase()
{
  if (m_d->extraRepresentation)
    common()->unregisterTrack(m_d->extraRepresentation);

  //Fixme: if visible, should we call detach first?
  foreach(AssociatedObjectHandleBase*ao,m_d->associatedObjects)
    delete ao;

  //Delete points the following way since we might use same points for raw and propagated.
  if (m_d->points_propagated != m_d->points_raw)
    delete m_d->points_propagated;
  delete m_d->points_raw;
  if (m_d->points_propagated_id_projections != m_d->points_raw_id_projections)
    delete m_d->points_propagated_id_projections;
  delete m_d->points_raw_id_projections;
  if (m_d->points_propagated_muon_projections != m_d->points_raw_muon_projections)
    delete m_d->points_propagated_muon_projections;
  delete m_d->points_raw_muon_projections;

  clearLine();
  if (m_currentmaterial)
    m_currentmaterial->unref();
  if (m_d->randommaterial)
    m_d->randommaterial->unref();
  if (m_d->label_sep)
    m_d->label_sep->unref();

  std::map<std::pair<int,double>,AssocObjAttachmentHandle *>::iterator it,itE(m_d->attachmentHandles.end());
  for (it = m_d->attachmentHandles.begin();it!=itE;++it)
    delete it->second;

  delete m_d;
  --Imp::ntrackhandles;
}

//____________________________________________________________________
int TrackHandleBase::numberOfInstances()
{
  return Imp::ntrackhandles;
}

//____________________________________________________________________
TrackSysCommonData * TrackHandleBase::common() const
{
  return m_collhandle->common();
}

//____________________________________________________________________
void TrackHandleBase::updateShapes_TSOSWithMeasurements()
{  
  if (!m_d->tsos_ascobjs)
    return;
  std::vector<AscObj_TSOS*>::iterator
    it(m_d->tsos_ascobjs->begin()),
    itE(m_d->tsos_ascobjs->end());
  for (;it!=itE;++it)
    if ((*it)->hasMeasurement())
    (*it)->update3DObjects();
}

//____________________________________________________________________
void TrackHandleBase::updateShapes_TSOSWithErrors()
{
  if (!m_d->tsos_ascobjs)
    return;
  std::vector<AscObj_TSOS*>::iterator
    it(m_d->tsos_ascobjs->begin()),
    itE(m_d->tsos_ascobjs->end());
  for (;it!=itE;++it)
    if ((*it)->hasError())
    (*it)->update3DObjects();
}

//____________________________________________________________________
void TrackHandleBase::updateShapes_TSOSWithMaterialEffects()
{
  if (!m_d->tsos_ascobjs)
    return;
  std::vector<AscObj_TSOS*>::iterator
    it(m_d->tsos_ascobjs->begin()),
    itE(m_d->tsos_ascobjs->end());
  for (;it!=itE;++it)
    if ((*it)->hasMaterialEffect())
    (*it)->update3DObjects();
}

//____________________________________________________________________
TrackCommonFlags::TSOSPartsFlags TrackHandleBase::shownTSOSParts() const
{
  return m_d->shownTSOSParts;//fixme: inline?
}

//____________________________________________________________________
TrackCommonFlags::TSOSPartsFlags TrackHandleBase::customColouredTSOSParts() const
{
  return m_d->customColouredTSOSParts;//fixme: inline?
}

//____________________________________________________________________
void TrackHandleBase::setShownTSOSParts(TrackCommonFlags::TSOSPartsFlags f)
{
  VP1Msg::messageDebug(QString("TrackHandleBase::setShownTSOSParts to ")+QString::number(f) );

  if (m_d->shownTSOSParts==f)
    return;
  //Figure out changed bits:
  TrackCommonFlags::TSOSPartsFlags change(m_d->shownTSOSParts ^ f);//^ is XOR
  m_d->shownTSOSParts=f;
  if (!m_d->tsos_ascobjs&&(m_d->shownTSOSParts==TrackCommonFlags::TSOS_NoObjects||!visible()))
    return;

  m_d->ensureInitTSOSs();
  //Loop over all TSOS objects. Those with changed parts needs shape
  //and visibility update:
  std::vector<AscObj_TSOS*>::iterator it(m_d->tsos_ascobjs->begin()), itE(m_d->tsos_ascobjs->end());
  for (;it!=itE;++it) {
    if (!((*it)->parts()&change))
      continue;
    const bool vis = (*it)->parts() & m_d->shownTSOSParts;
    if (vis==(*it)->visible()) {
      //Just update shape:
      (*it)->update3DObjects();
      continue;
    }
    //Need both shape and visibility updates.
    if (vis) {
      //update shape and then turn visible:
      (*it)->update3DObjects();
      (*it)->setVisible(vis);
    } else {
      //turn invisible and then update (i.e. invalidate) shape:
      (*it)->setVisible(vis);
      (*it)->update3DObjects();
    }
  }
}

//____________________________________________________________________
void TrackHandleBase::setCustomColouredTSOSParts(TrackCommonFlags::TSOSPartsFlags f)
{
  if (m_d->customColouredTSOSParts==f)
    return;
  //Figure out changed bits:
  TrackCommonFlags::TSOSPartsFlags change(m_d->customColouredTSOSParts ^ f);//^ is XOR
  m_d->customColouredTSOSParts=f;

  if (!m_d->tsos_ascobjs||m_d->shownTSOSParts==TrackCommonFlags::TSOS_NoObjects)
    return;

  TrackCommonFlags::TSOSPartsFlags changedShownParts(change & m_d->shownTSOSParts);

  if (!changedShownParts)
    return;

  //redundant  m_d->ensureInitTSOSs();
  std::vector<AscObj_TSOS*>::iterator it(m_d->tsos_ascobjs->begin()), itE(m_d->tsos_ascobjs->end());
  for (;it!=itE;++it) {
    if ((*it)->parts()&changedShownParts)
      (*it)->update3DObjects();
  }
}

//____________________________________________________________________
void TrackHandleBase::Imp::ensureInitTSOSs()
{
  if (tsos_ascobjs)
    return;
  tsos_ascobjs = new std::vector<AscObj_TSOS*>;
  ensureLoadPathInfo();
  if (!pathInfo_TrkTrack||!pathInfo_TrkTrack->trackParameters()||pathInfo_TrkTrack->trackParameters()->size()==0)
    return;
  VP1TrackSanity * sanity = theclass->common()->trackSanityHelper();

  tsos_ascobjs->reserve(pathInfo_TrkTrack->trackParameters()->size());

  unsigned parindex(0);

  AscObj_TSOS* ascObjNeedDistToNext(0);
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_iter = pathInfo_TrkTrack->trackStateOnSurfaces()->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_end = pathInfo_TrkTrack->trackStateOnSurfaces()->end();
  const Trk::TrackParameters* trackParam(0);
  for (; tsos_iter != tsos_end; ++tsos_iter) {
    trackParam = (*tsos_iter)->trackParameters();
    if (!sanity->isSafe(*tsos_iter)) {
      parindex++;
      continue;
    }
    if (trackParam&&!sanity->isSafe(trackParam)) {
      parindex++;
      continue;
    }
    //Fixme: likewise check that we don't have a bad measurement, material effect, ...
    if (ascObjNeedDistToNext&&trackParam) {
      ascObjNeedDistToNext->setDistToNextPar((trackParam->position()-ascObjNeedDistToNext->approxCenter()).mag());
      ascObjNeedDistToNext = 0;
    }
    VP1Msg::messageVerbose("Adding TSOS at index:"+QString::number(parindex));
    ascObjNeedDistToNext = addTSOS(*tsos_iter,parindex++);
  }

}

//____________________________________________________________________
AscObj_TSOS* TrackHandleBase::Imp::addTSOS(const Trk::TrackStateOnSurface * tsos,unsigned index)
{
  AscObj_TSOS* ao = new AscObj_TSOS(theclass,tsos,index);
  theclass->registerAssocObject(ao);
  tsos_ascobjs->push_back(ao);
  if (ao->parts()&shownTSOSParts)
    ao->setVisible(true);
  return ao;
}

//____________________________________________________________________
void TrackHandleBase::setVisible(bool vis)
{
  QString tmp = (vis==true)?"True":"False";
  QString tmp2 = (m_visible==true)?"True":"False";
  VP1Msg::messageDebug(QString("TrackHandleBase calling setVisible with vis=")+tmp+QString(", and m_visible=")+tmp2 );
  if (vis==m_visible)
    return;

  m_visible=vis;
  if (vis) {
    m_collhandle->incrementNShownHandles();
    if (!touchedMuonChambers().empty()) {
      for (std::set<GeoPVConstLink>::iterator it=touchedMuonChambers().begin(); it!=touchedMuonChambers().end(); ++it)
        common()->touchedMuonChamberHelper()->incrementNumberOfObjectsForPV(*it);
    }
    if (!m_d->line)
      m_d->rebuild3DObjects();//The call to rebuild also fixes attached state.
    else
      m_d->attach3DObjects();
  } else {
    m_collhandle->decrementNShownHandles();
    if (!touchedMuonChambers().empty()) {
      for (std::set<GeoPVConstLink>::iterator it=touchedMuonChambers().begin(); it!=touchedMuonChambers().end(); ++it)
        common()->touchedMuonChamberHelper()->decrementNumberOfObjectsForPV(*it);
    }
    m_d->detach3DObjects();
  }

  bool initTSOS(false);
  if (!m_d->tsos_ascobjs&&vis&&m_d->shownTSOSParts!=TrackCommonFlags::TSOS_NoObjects) {
    m_d->ensureInitTSOSs();
    initTSOS = true;
  }

  if (!initTSOS && m_d->tsos_ascobjs) {
    std::vector<AscObj_TSOS*>::iterator
      it(m_d->tsos_ascobjs->begin()),
      itE(m_d->tsos_ascobjs->end());
    if (m_d->shownTSOSParts!=TrackCommonFlags::TSOS_NoObjects) {
      for (;it!=itE;++it) {
        if ((*it)->visible()!=((*it)->parts() & m_d->shownTSOSParts))
          (*it)->toggleVisible();
      }
    } else {
      for (;it!=itE;++it) {
        if ((*it)->visible())
          (*it)->setVisible(false);
      }
    }
  }

  visibleStateChanged();

  std::map<std::pair<int,double>,AssocObjAttachmentHandle *>::iterator it,itE(m_d->attachmentHandles.end());
  for (it = m_d->attachmentHandles.begin();it!=itE;++it)
    it->second->trackVisibilityChanged();

  //Label

}

//____________________________________________________________________
void TrackHandleBase::update3DObjects( bool invalidatePropagatedPoints, float maxR )
{
  VP1Msg::messageVerbose(QString("TrackHandleBase::update3DObject with maxR set to ")+QString::number(maxR) );
  if (maxR>0.0) {    
    m_d->tempMaxPropRadius=maxR;
  }
  if ( invalidatePropagatedPoints) {
    if (m_d->points_propagated != m_d->points_raw) {
      delete m_d->points_propagated;m_d->points_propagated = 0;
    }
    delete m_d->points_raw;m_d->points_raw = 0;   
    if (m_d->points_propagated_id_projections) { delete m_d->points_propagated_id_projections; m_d->points_propagated_id_projections = 0; }
    if (m_d->points_propagated_muon_projections) { delete m_d->points_propagated_muon_projections; m_d->points_propagated_muon_projections = 0; }
  }
  if (m_visible) {
    m_d->rebuild3DObjects();
  } else {    
    //Simply clear the present 3D objects. They will only be recreated if/when the track becomes visible again.
    clearLine();
  }
  m_d->tempMaxPropRadius=0.0;
}

//____________________________________________________________________
void TrackHandleBase::updateInDetProjections()
{
  //Invalidate points of indet projections if already calculated:
  if (m_d->points_raw_id_projections!=m_d->points_propagated_id_projections)
    delete m_d->points_propagated_id_projections;
  delete m_d->points_raw_id_projections;
  m_d->points_raw_id_projections = 0;
  m_d->points_propagated_id_projections = 0;
  //Rebuild 3D objects:
  update3DObjects();
}

//____________________________________________________________________
void TrackHandleBase::updateMuonProjections()
{
  //Invalidate points of indet projections if already calculated:
  if (m_d->points_raw_muon_projections!=m_d->points_propagated_muon_projections)
    delete m_d->points_propagated_muon_projections;
  delete m_d->points_raw_muon_projections;
  m_d->points_raw_muon_projections = 0;
  m_d->points_propagated_muon_projections = 0;
  //Rebuild 3D objects:
  update3DObjects();
}


//____________________________________________________________________
void TrackHandleBase::Imp::addPathToSoLineSetAndSoVertexProperty(const std::vector<Amg::Vector3D >& points,
  int & iver, int& numlines,
  SoLineSet * line, SoVertexProperty * vertices )
{
  if (points.size()<2)
    return;
  if (!line) {
    theclass->collHandle()->systemBase()->message("TrackHandleBase::Imp::addPathToSoLineSetAndSoVertexProperty (1) ERROR: Unexpected null SoLineSet pointer!");
    return;
  }
  if (!vertices) {
    theclass->collHandle()->systemBase()->message("TrackHandleBase::Imp::addPathToSoLineSetAndSoVertexProperty (1) ERROR: Unexpected null SoVertexPropery pointer!");
    return;
  }
  std::vector<Amg::Vector3D >::const_iterator pointsIt, pointsItEnd(points.end());
  unsigned npointsused(0);


  float maxR2=theclass->common()->controller()->propMaxRadius()>0.0?theclass->common()->controller()->propMaxRadius():std::numeric_limits<float>::max();
  if (tempMaxPropRadius>0.0){    
    maxR2 = tempMaxPropRadius * tempMaxPropRadius;
    theclass->collHandle()->systemBase()->messageVerbose("maxR2 is set to "+VP1Msg::str(maxR2));
  }
  float vertexPlanePhi = (theclass->common()->controller()->vertexProjectionAngle())*M_PI/180;// angle of plane to use for vertex projection
  vertexPlanePhi-=M_PI; // ATLAS range is -PI to PI
  
  // theclass->collHandle()->systemBase()->messageVerbose("TrackHandleBase::Imp::addPathToSoLineSetAndSoVertexProperty - adding "
  //     +VP1Msg::str(points.size())+" points up to R2 of "+VP1Msg::str(maxR2));
  theclass->collHandle()->systemBase()->messageVerbose("Vertex projection is set to "+VP1Msg::str(vertexPlanePhi));
      
  // unsigned int count=0;
  
  //For vertex projection
  Amg::Vector3D tempPoint;
  
  double trkPhi =  theclass->momentum().phi(); // some tracks curve between sections otherwise.
  double newPhi= vertexPlanePhi+M_PI; 
  if (cos(vertexPlanePhi-trkPhi)>0) newPhi=vertexPlanePhi;
  // theclass->collHandle()->systemBase()->messageVerbose("Vertex projection is set to "+VP1Msg::str(vertexPlanePhi)+" trkPhi= "+VP1Msg::str(trkPhi)+" newPhi="+VP1Msg::str(newPhi));
  
  for (pointsIt = points.begin();pointsIt!=pointsItEnd && pointsIt->mag2()<maxR2; ++pointsIt) {
    // theclass->collHandle()->systemBase()->messageVerbose(VP1Msg::str(count++)+": point has perp2="+VP1Msg::str(pointsIt->perp2()));
    if (!isSane(*pointsIt)) {
      theclass->collHandle()->systemBase()->message("WARNING: Ignoring point on track:"+VP1Msg::str( *pointsIt ) );
      continue;
    }
    
    if (theclass->collHandle()->parts() & TrackCommonFlags::VertexProjections){
      tempPoint = *pointsIt;
      Amg::setPhi(tempPoint, newPhi);
      
    // std::cout<<trkPhi<<"\t"<<phi<<"\t"<<r0<<"\t"<<r1<<"\t"<<r2<<"\t"<<r3<<"\t"<<rotatePhi<<"\t"<<tempPoint.getPhi()<<std::endl;

      vertices->vertex.set1Value(iver++,tempPoint.x(),tempPoint.y(),tempPoint.z());
    } else {
      vertices->vertex.set1Value(iver++,pointsIt->x(),pointsIt->y(),pointsIt->z());
    }
    ++npointsused;
  }
  line->numVertices.set1Value(numlines++,npointsused);
}

//____________________________________________________________________
void TrackHandleBase::Imp::addPathsToSoLineSetAndSoVertexProperty(const Amg::SetVectorVector3D * paths,
  int & iver, int& numlines,
  SoLineSet * line, SoVertexProperty * vertices )
{
  if ( !paths || ( paths->size()==1 && paths->begin()->empty()) )
    return;
  if (!line) {
    theclass->collHandle()->systemBase()->message("TrackHandleBase::Imp::addPathToSoLineSetAndSoVertexProperty (2) ERROR: Unexpected null SoLineSet pointer!");
    return;
  }
  if (!vertices) {
    theclass->collHandle()->systemBase()->message("TrackHandleBase::Imp::addPathToSoLineSetAndSoVertexProperty (2) ERROR: Unexpected null SoVertexPropery pointer!");
    return;
  }
  Amg::SetVectorVector3D::const_iterator itProjPart, itProjPartEnd(paths->end());
  for (itProjPart = paths->begin();itProjPart!=itProjPartEnd;++itProjPart)
    addPathToSoLineSetAndSoVertexProperty(*itProjPart,iver,numlines,line,vertices);
}

//____________________________________________________________________
void TrackHandleBase::clearLine()
{
  if (m_d->line) {
    common()->unregisterTrack(m_d->line);
    m_d->line->unref();
    m_d->line=0;
  }
}

//____________________________________________________________________
void TrackHandleBase::registerTrack()
{
  if (m_d->line)
    common()->registerTrack(m_d->line,this);
}


//____________________________________________________________________
void TrackHandleBase::Imp::rebuild3DObjects()
{  
  //Ensure we are always detached while updating.
  if (theclass->m_visible)
    detach3DObjects();

  // Make sure we're not going to redraw old surfaces, for this debug mode.
  if (theclass->common()->trackPropagationHelper()->showExtrapolationSurfaces())
    theclass->common()->trackPropagationHelper()->getExtrapolationSurfaces().clear();

  Trk::IExtrapolator * propagator = theclass->collHandle()->propagator();
  // std::cout<<" propagator "<<propagator<<std::endl;

  if (propagator)
    ensureInitPointsPropagated();
  else
    ensureInitPointsRaw();

  bool id_proj = theclass->collHandle()->parts() & TrackCommonFlags::InDetProjections;
  if (id_proj)
    ensureInitPointsProjections_InDet(!propagator);

  bool muon_proj = theclass->collHandle()->parts() & TrackCommonFlags::MuonProjections;
  if (muon_proj)
    ensureInitPointsProjections_Muon(!propagator);

  std::vector<Amg::Vector3D > * points;
  Amg::SetVectorVector3D *proj_id, *proj_muon;
  if (propagator) {
    points = points_propagated;
    proj_id = points_propagated_id_projections;
    proj_muon = points_propagated_muon_projections;
  } else {
    points = points_raw;
    proj_id = points_raw_id_projections;
    proj_muon = points_raw_muon_projections;
  }

  //Put points into an appropriate vertex property.
  theclass->clearLine(); //Fixme: Since we are just changing shape - no need to delete line and take it
                         //out of the tree first. Just edit lineset properties instead.
  line = new SoLineSet();
  line->ref();
  theclass->registerTrack();

  SoVertexProperty * vertices = new SoVertexProperty();;

  int iver(0), numlines(0);

  std::vector<Amg::Vector3D >::const_iterator  pointsIt, pointsItEnd;

  //Add various parts to these vertices as appropriate:
  if (theclass->collHandle()->parts() & TrackCommonFlags::ActualPath)
    addPathToSoLineSetAndSoVertexProperty(*points,iver,numlines,line,vertices);
  if (id_proj)
    addPathsToSoLineSetAndSoVertexProperty( proj_id, iver, numlines, line, vertices );
  if (muon_proj)
    addPathsToSoLineSetAndSoVertexProperty( proj_muon, iver, numlines, line, vertices );

  line->vertexProperty = vertices;

  //////////////////////////////////////////////////////////////


// Add debugging surfaces. 
  if (theclass->common()->trackPropagationHelper()->showExtrapolationSurfaces()){
    std::vector<Trk::PlaneSurface>& surfaces = theclass->common()->trackPropagationHelper()->getExtrapolationSurfaces();
    std::vector<Trk::PlaneSurface>::const_iterator surfIt=surfaces.begin(), surfEnd=surfaces.end();
    SurfaceToSoNode surfCnv;//fixme: check if need in common()
    extrapSurfaces_sep=new SoSeparator;
    for (;surfIt!=surfEnd;++surfIt){
      SoNode* theSurfSep = surfCnv.translateSurface(*surfIt);
      if (theSurfSep) {
        SoNode * nodeToAdd = theSurfSep;
        extrapSurfaces_sep->addChild(nodeToAdd); 
      }
    }  
  }

// Add labels
  if (!points->empty() && theclass->common()->controller()->doTrackLabels() ){

    // should detach first? This is all fairly suboptimal I'm sure... EJWM.
    if (!label_sep){
      label_sep = new SoSeparator;
      label_sep->ref();
    }
    label_sep->removeAllChildren ();

    SoText2 *labelText = new SoText2;
    labelText->ref();
    QStringList text;
    TrackSystemController::TrackLabelModes labels=theclass->common()->controller()->trackLabels();
    if (labels&TrackSystemController::P){
      text << "P="+VP1Msg::str(theclass->momentum().mag())+" MeV";
    }

    if (labels&TrackSystemController::Pt){
      text << "|Pt|="+VP1Msg::str(theclass->momentum().perp())+" MeV";
    }

    if (labels&TrackSystemController::Pid){
      int pdg = theclass->pdgCode();
      if (pdg) {
        bool ok;
        QString name = VP1ParticleData::particleName(pdg,ok);
        if (ok)
          text << "pdg: "+QString::number(pdg)+" ["+name+"]";
        else
          text << "pdg: "+QString::number(pdg);
      }
    }

    if (labels&TrackSystemController::FitQuality && theclass->getFitQuality()){
      text << TrkObjToString::shortInfo(*theclass->getFitQuality());
    }

    if (labels&TrackSystemController::Hits){
      text <<"Pix["+VP1Msg::str(theclass->getNPixelHits())+"], SCT["+VP1Msg::str(theclass->getNSCTHits())+"], TRT["+VP1Msg::str(theclass->getNTRTHits())+"],"
        <<" MDT["+QString::number(theclass->getNMDTHits())+"], RPC["+QString::number(theclass->getNRPCHits())+"], TGC["+QString::number(theclass->getNTGCHits())+"], CSC["+QString::number(theclass->getNCSCHits())+"]";
    }

      if (labels&TrackSystemController::Direction ){
        Amg::Vector3D mom = theclass->momentum();
        mom /= CLHEP::GeV;
        if (mom.mag2()==0.0) {
          text << "Momentum : 0 (undefined)";
        } else {
          // SoCoin classes don't support Unincode here, apparently.
          // text << VP1Msg::str("(")+QChar(0x03B7)+","+QChar(0x03D5)+VP1Msg::str(")=(")
          //   +VP1Msg::str(mom.pseudoRapidity())+VP1Msg::str(VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() ))+VP1Msg::str(")");
        	double pseudoRapidity = mom.eta();
          text << VP1Msg::str("(eta,phi)=(")
            +VP1Msg::str(pseudoRapidity)+VP1Msg::str(",")+VP1Msg::str(VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() ))+VP1Msg::str(")");
        }
      }

    unsigned int row=0;
    foreach(QString str, text){
      QByteArray array = str.toLatin1();
      labelText->string.set1Value(row++,array.data());
    }

    //Position
    float labelTrackOffset = theclass->common()->controller()->trackLabelTrkOffset();
    unsigned int point=(points->size()-1)*labelTrackOffset;
    Amg::Vector3D labelPos = (*points)[point] ;

//    if (labels&TrackSystemController::PosEndOfTrack)
//      labelPos=    points->back();
//    else if (labels&TrackSystemController::PosBeginOfTrack)
//      labelPos=    points->front ();
//    else if (labels&TrackSystemController::PosMidOfTrack)
//      labelPos=    (*points)[points->size()/2];

    SoTranslation *labelTranslate = new SoTranslation;
    float offScale=10.0;
    int xOffset = theclass->common()->controller()->labelXOffset() ; 
    int yOffset = theclass->common()->controller()->labelYOffset() ; 
    int zOffset = theclass->common()->controller()->labelZOffset() ;
    labelTranslate->translation.setValue(labelPos.x()+(xOffset*offScale),labelPos.y()+(yOffset*offScale),labelPos.z()+(zOffset*offScale)); 

    SoMaterial *sMat = new SoMaterial();
    SoMFColor sColor;
    sColor.setValue(SbColor(0, 0, 1));
    sMat->diffuseColor = sColor;

    label_sep->addChild(labelTranslate);
    label_sep->addChild(labelText);
  }

  //Attach if visible:
  if (theclass->m_visible)
    attach3DObjects();
}

//____________________________________________________________________
void TrackHandleBase::Imp::materialChanged()
{
  theclass->currentMaterialChanged();
  std::map<std::pair<int,double>,AssocObjAttachmentHandle *>::iterator it,itE(attachmentHandles.end());
  for (it = attachmentHandles.begin();it!=itE;++it)
    it->second->trackMaterialChanged();
}

//____________________________________________________________________
double TrackHandleBase::Imp::dist(const SbVec3f& p1,const SbVec3f& p2)//Fixme: to linalgs..
{
  float x1,x2,y1,y2,z1,z2;
  p1.getValue(x1,y1,z1);
  p2.getValue(x2,y2,z2);
  return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1));
}

//____________________________________________________________________
void TrackHandleBase::Imp::convertLineSetToCylinders(SoLineSet*line,SoSeparator*sep,const double& cylradius)
{
  //fixme:  belongs in VP1Utils
  SoVertexProperty *vertices = static_cast<SoVertexProperty *>(line->vertexProperty.getValue());
  if (!vertices)
    return;
  int ivert(0);
  SbMatrix  lastTransf;
  lastTransf.makeIdentity();

  for (int iline=0;iline<line->numVertices.getNum();++iline) {
    int nvert=line->numVertices[iline];
    if (nvert<2) {
      ivert += nvert;
      continue;
    }
    for (int jvert= ivert; jvert<ivert+nvert-1;++jvert) {
      SbVec3f p1 = vertices->vertex[jvert].getValue();
      SbVec3f p2 = vertices->vertex[jvert+1].getValue();
      SoCylinder * cyl = new SoCylinder;
      cyl->radius = cylradius;
      const double l(dist(p1,p2));
      cyl->height = l;
      //First translate (0,l/2,0), then rotate (0,1,0) into (p2-p1), then translate p1.:
      SbMatrix m;
      m.setTranslate(SbVec3f(0,0.5*l,0));
      SbVec3f v(p2); v -= p1;
      SbRotation rot(SbVec3f(0,1,0),v);
      SbMatrix m2; m2.setRotate(rot);
      m.multRight(m2);
      SbMatrix m3;
      m3.setTranslate(p1);
      m.multRight(m3);
      //m is the transform we need in front of our cylinder. However, we need to first add the inverse of all previous transforms.
      SbMatrix mat(m);
      mat.multRight(lastTransf.inverse());
      SoMatrixTransform * mt = new SoMatrixTransform;
      mt->matrix.setValue(mat);
      sep->addChild(mt);
      sep->addChild(cyl);
      lastTransf = m;
    }
    ivert += nvert;
  }
}

//____________________________________________________________________
void TrackHandleBase::Imp::attach3DObjects()
{
  // std::cout<<"TrackHandleBase::Imp::attach3DObjects() - 1"<<std::endl;

  if (!theclass->m_currentmaterial) {
    theclass->m_currentmaterial = determineMaterial();
    theclass->m_currentmaterial->ref();
    materialChanged();
  }
  if (line && theclass->m_collhandle->sephelper()) {
    // std::cout<<"TrackHandleBase::Imp::attach3DObjects() - 2"<<std::endl;
    theclass->m_collhandle->sephelper()->addNodeUnderMaterial(line,theclass->m_currentmaterial);
    double tube_r(theclass->m_collhandle->trackTubeRadius());
    if (tube_r) {
      if (extraRepresentation) {
        extraRepresentation->removeAllChildren();
      } else {
        extraRepresentation = new SoSeparator;
        extraRepresentation->ref();
        theclass->common()->registerTrack(extraRepresentation,theclass);
      }
      convertLineSetToCylinders(line,extraRepresentation,tube_r);
      if (!extraRepAttached) {
        theclass->m_collhandle->sephelper()->addNodeUnderMaterial(extraRepresentation,theclass->m_currentmaterial);
        extraRepAttached=true;
      }
    }
  }
  if (label_sep && theclass->common()->textSep() && theclass->common()->controller()->doTrackLabels())
    theclass->common()->textSep()->addChild(label_sep); 

  if (extrapSurfaces_sep) theclass->m_collhandle->sephelper()->addNodeUnderMaterial(extrapSurfaces_sep,theclass->m_currentmaterial); 
}

//____________________________________________________________________
void TrackHandleBase::Imp::detach3DObjects()
{
  if (!theclass->m_currentmaterial)
    return;//Can never have been attached!
  if (line && theclass->m_collhandle->sephelper()) {
    theclass->m_collhandle->sephelper()->removeNodeUnderMaterial(line,theclass->m_currentmaterial);
    if (extraRepresentation&&extraRepAttached) {
      theclass->m_collhandle->sephelper()->removeNodeUnderMaterial(extraRepresentation,theclass->m_currentmaterial);
      extraRepAttached=false;
    }
  }
  if (label_sep && theclass->common()->textSep()) 
    theclass->common()->textSep()->removeChild(label_sep);

  if (extrapSurfaces_sep) 
    theclass->m_collhandle->sephelper()->removeNodeUnderMaterial(extrapSurfaces_sep,theclass->m_currentmaterial); 
}

//____________________________________________________________________
void TrackHandleBase::updateMaterial()
{
  //We need to change the current material. This means we need to
  //clear the cache. Only if we are visible do we need to do a
  //detach->clear material->attach cycle to trigger attachment under new material.

  if (!m_currentmaterial)
    return;//We have no material cached and is thus not attached either.
  if (m_visible) {
    //See if the material changed. If it did, detach, update the material, attach again.
    SoMaterial * newmat = m_d->determineMaterial();
    newmat->ref();
    if (newmat!=m_currentmaterial) {
      m_d->detach3DObjects();
      m_currentmaterial->unref();
      m_currentmaterial = newmat;
      m_d->attach3DObjects();
      m_d->materialChanged();
    } else {
      newmat->unref();
    }
  } else {
    //Just clear material.
    m_currentmaterial->unref();
    m_currentmaterial = 0;
    m_d->materialChanged();
  }
}

//____________________________________________________________________
void TrackHandleBase::Imp::ensureInitPointsRaw()
{
  // theclass->collHandle()->systemBase()->message("ensureInitPointsRaw start" );

  if (points_raw)
    return;
  points_raw = new std::vector<Amg::Vector3D >;

  //////////////////////////////////////////////////////////////
  //             Get information about this path              //
  //////////////////////////////////////////////////////////////

  ensureLoadPathInfo();

  if (pathInfo_TrkTrack) {
    const VP1TrackSanity * sanity = theclass->common()->trackSanityHelper();
    Amg::Vector3D * firstmomentum(0);
    //Amg::Vector3D vect3d{0.,0.,0.};
    if (pathInfo_TrkTrack->trackParameters())
      points_raw->reserve(pathInfo_TrkTrack->trackParameters()->size());
    bool unsafeparts(false);
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_iter = pathInfo_TrkTrack->trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_end = pathInfo_TrkTrack->trackStateOnSurfaces()->end();
    const Trk::TrackParameters* trackParam;
    for (; tsos_iter != tsos_end; ++tsos_iter) {
      if (!sanity->isSafe(*tsos_iter)) {
        unsafeparts = true;
        continue;
      }
      if (theclass->collHandle()->ignoreMEOTinProp() && (*tsos_iter)->materialEffectsOnTrack())
        continue;
      trackParam = (*tsos_iter)->trackParameters();
      if (trackParam) {
        if (!sanity->isSafe(trackParam)) {
          unsafeparts = true;
          continue;
        }
        trackParam->position();//test
        points_raw->push_back( trackParam->position() );
        if (!firstmomentum) {
        	//vect3d = 
        	firstmomentum = new Amg::Vector3D(trackParam->momentum());
        }
      }
    }
    if (unsafeparts)
      theclass->collHandle()->systemBase()->message("WARNING: Ignored unsafe parts of track!");
    if (points_raw->size()==1) {
      if (!firstmomentum) {
        theclass->collHandle()->systemBase()->message(" TrackHandleBase ERROR: Unexpected null firstmomentum!");
        firstmomentum = new Amg::Vector3D(0,0,0);
      }
      if (firstmomentum->mag()==0.0) {
        theclass->collHandle()->systemBase()->message("TrackHandleBase ERROR: Only point on track had zero momentum. Can't add second point.");
        points_raw->clear();
      } else {
        VP1Msg::messageVerbose("TrackHandleBase: Adding second point in direction of initial momentum.");
        points_raw->push_back(points_raw->front()+(*firstmomentum)*(100.0/firstmomentum->mag()));
      }
    } else if (points_raw->empty()) {
      theclass->collHandle()->systemBase()->message("TrackHandleBase ERROR: No points on track.");
    }
    
    //std:: cout << "firstmomentum: " << firstmomentum << std::endl;
    delete firstmomentum;
    firstmomentum = 0;
    return;
  }
  if (pathInfo_Points)
    std::copy(pathInfo_Points->begin(), pathInfo_Points->end(), std::back_inserter(*points_raw));
  if (points_raw->size()<2) {
    theclass->collHandle()->systemBase()->message("TrackHandleBase ERROR: Less than two points on track provided.");
    points_raw->clear();
  } else {
    VP1Msg::messageVerbose( "TrackHandleBase: Created raw data "
      + QString::number(points_raw->size())
      + " points for track.");
  }
}

//____________________________________________________________________
void TrackHandleBase::Imp::ensureInitPointsPropagated()
{
  if (points_propagated)
    return;

  ensureLoadPathInfo();

  if (pathInfo_TrkTrack) {
    points_propagated = new std::vector<Amg::Vector3D >();
    bool ok (false);
    if (theclass->hasCharge()&&theclass->charge()!=0.0)
      ok = theclass->common()->trackPropagationHelper()->makePointsCharged(*points_propagated,pathInfo_TrkTrack,
    		  theclass->collHandle()->propagator(),
    		  theclass->extrapolationParticleHypothesis(),
    		  !theclass->collHandle()->ignoreMEOTinProp(),
    		  theclass->collHandle()->extendTracks() ? theclass->common()->controller()->extrapolateToThisVolume() : 0 );
    else
      ok = theclass->common()->trackPropagationHelper()->makePointsNeutral(*points_propagated,pathInfo_TrkTrack);

    if (!ok) {
      delete points_propagated;
      ensureInitPointsRaw();
      points_propagated = points_raw;
    }
  } else {
    //We just use the raw points:
    ensureInitPointsRaw();
    points_propagated = points_raw;
  }
}

//____________________________________________________________________
void TrackHandleBase::Imp::ensureInitPointsProjections_InDet( bool raw )
{
  if (raw?points_raw_id_projections:points_propagated_id_projections)
    return;

  raw ? ensureInitPointsRaw() : ensureInitPointsPropagated();

  //First check if we use same points for raw/propagated modes:
  if (points_propagated==points_raw) {
    if (raw) {
      if (points_propagated_id_projections) {
        points_raw_id_projections = points_propagated_id_projections;
        return;
      }
    } else {
      if (points_raw_id_projections) {
        points_propagated_id_projections = points_raw_id_projections;
        return;
      }
    }
  }

  //Time for the hard work:
  std::vector<Amg::Vector3D > * points=0;
  Amg::SetVectorVector3D* projections=0;
  if (raw) {
    points_raw_id_projections = new Amg::SetVectorVector3D;
    points = points_raw;
    projections = points_raw_id_projections;
  } else {
    points_propagated_id_projections = new Amg::SetVectorVector3D;
    points = points_propagated;
    projections = points_propagated_id_projections;
  }

  if ( !points || points->empty() || points->size()<2 ) {
    projections->clear();
    projections->insert(std::vector<Amg::Vector3D >());//error signature
    return;
  }

  bool oneok(false);
  if (theclass->common()->indetProjHelper_Pixel()) {
    theclass->common()->indetProjHelper_Pixel()->projectPath(*points,*projections);
    oneok = true;
  }
  if (theclass->common()->indetProjHelper_SCT()) {
    theclass->common()->indetProjHelper_SCT()->projectPath(*points,*projections);
    oneok = true;
  }
  if (theclass->common()->indetProjHelper_TRT()) {
    theclass->common()->indetProjHelper_TRT()->projectPath(*points,*projections);
    oneok = true;
  }

  if (!oneok) {
    projections->clear();
    projections->insert(std::vector<Amg::Vector3D >());//error signature
  }
}

//____________________________________________________________________
void TrackHandleBase::Imp::ensureInitPointsProjections_Muon( bool raw )
{
  if (raw?points_raw_muon_projections:points_propagated_muon_projections)
    return;

  raw ? ensureInitPointsRaw() : ensureInitPointsPropagated() ;

  //First check if we use same points for raw/propagated modes:
  if (points_propagated==points_raw) {
    if (raw) {
      if (points_propagated_muon_projections) {
        points_raw_muon_projections = points_propagated_muon_projections;
        return;
      }
    } else {
      if (points_raw_muon_projections) {
        points_propagated_muon_projections = points_raw_muon_projections;
        return;
      }
    }
  }

  //Time for the hard work:
  std::vector<Amg::Vector3D > * points;
  Amg::SetVectorVector3D * projections;
  if (raw) {
    points_raw_muon_projections = new Amg::SetVectorVector3D;
    points = points_raw;
    projections = points_raw_muon_projections;
  } else {
    points_propagated_muon_projections = new Amg::SetVectorVector3D;
    points = points_propagated;
    projections = points_propagated_muon_projections;
  }

  MuonChamberProjectionHelper * projhelper = theclass->common()->muonChamberProjectionHelper();
  if ( !VP1JobConfigInfo::hasMuonGeometry() || !projhelper || !points || points->empty() ) {
    projections->insert(std::vector<Amg::Vector3D >());//error signature
    return;
  }

  //Temporary variables:
  Amg::Vector3D firstEndWall_pointA, firstEndWall_pointB;
  Amg::Vector3D secondEndWall_pointA, secondEndWall_pointB;
  bool outsidechamber;
  std::vector<Amg::Vector3D > proj1, proj2;

  //For each (MDT) chamber we try to project all line segments on "points" to the ends of that chamber.
  for (std::set<GeoPVConstLink>::iterator it=theclass->touchedMuonChambers().begin(); it!=theclass->touchedMuonChambers().end(); ++it){

    //For now, only project to end of MDT chambers:
    if (!projhelper->isKnownMDTChamber(*it))
      continue;

    proj1.clear();
    proj2.clear();

    // now loop over the stored points
    std::vector<Amg::Vector3D >::const_iterator  pointsIt=points->begin(), pointsItEnd=points->end()-1;
    for (;pointsIt!=pointsItEnd; pointsIt++) {

      // Do projections
      bool ok = projhelper->projectAndConstrainLineSegmentToMDTChamberEndWalls( *it, *pointsIt, *(pointsIt+1),
        firstEndWall_pointA, firstEndWall_pointB,
        secondEndWall_pointA, secondEndWall_pointB,
        outsidechamber );

      if (!ok) {
        theclass->collHandle()->systemBase()->message("TrackHandleBase Error: "
          "Problems with projectAndConstrainLineSegmentToMDTChamberEndWalls(..)");
  //  projections->clear();
  //  projections->push_back(std::vector<Amg::Vector3D >);//error signature
        continue;
      }

      if (!outsidechamber){
        if ( proj1.empty() ) {
          proj1.push_back(firstEndWall_pointA); proj1.push_back(firstEndWall_pointB);
        } else {
          if ( proj1[proj1.size()-1] == firstEndWall_pointA ) {
            proj1.push_back(firstEndWall_pointB);//Keep adding to line part
          } else {
      //Start new line.
            projections->insert(proj1); proj1.clear();
            proj1.push_back(firstEndWall_pointA); proj1.push_back(firstEndWall_pointB);
          }
        }
        if ( proj2.empty() ) {
          proj2.push_back(secondEndWall_pointA); proj2.push_back(secondEndWall_pointB);
        } else {
        // unsure about this change...coverity 16206, sroe
        // if ( proj2[proj2.size()-1] == firstEndWall_pointA ) {
          if ( proj2[proj2.size()-1] == secondEndWall_pointA ) {
            proj2.push_back(secondEndWall_pointB);//Keep adding to line part
          } else {
      //Start new line.
            projections->insert(proj2); proj2.clear();
            proj2.push_back(secondEndWall_pointA); proj2.push_back(secondEndWall_pointB);
          }
        }
      }
      //proj2 fixme
    }
    if ( !proj1.empty() ) {
      projections->insert(proj1);
    }
    if ( !proj2.empty() ) {
      projections->insert(proj2);
    }
  }
}

//____________________________________________________________________
void TrackHandleBase::registerTouchedMuonChamber( const GeoPVConstLink& chamberPV ) const
{
  m_d->touchedmuonchambers.insert(chamberPV);
}

//____________________________________________________________________
const std::set<GeoPVConstLink>& TrackHandleBase::touchedMuonChambers() const
{
  if (!m_d->inittouchedchambers) {
    if (VP1JobConfigInfo::hasMuonGeometry())
      ensureTouchedMuonChambersInitialised();
    m_d->inittouchedchambers=true;
  }
  return m_d->touchedmuonchambers;
}

//____________________________________________________________________
SoMaterial * TrackHandleBase::Imp::determineMaterial()
{
  // theclass->collHandle()->systemBase()->message("determineMaterial with material = "+QString::number(static_cast<unsigned int>(theclass->collHandle()->colourBy()))); //too verbose. EJWM.

  int pdgcode(0);
  switch(theclass->collHandle()->colourBy()) {
    case TrackCollHandleBase::COLOUR_BYPID:
    pdgcode = theclass->pdgCode();
    return theclass->common()->controller()->getMaterialForPDGCode(pdgcode == SimBarCode::unknownPDG ? 0 : pdgcode);
    case TrackCollHandleBase::COLOUR_RANDOM:
    if (!randommaterial) {
      randommaterial = new SoMaterial;
      randommaterial->ref();
      theclass->rerandomiseRandomMaterial();
    }
    return randommaterial;
    case TrackCollHandleBase::COLOUR_CHARGE:
    return theclass->common()->controller()->getMaterialForCharge(theclass->hasCharge()?theclass->charge():0.0);
    case TrackCollHandleBase::COLOUR_MOMENTUM:
    return theclass->common()->controller()->getMaterialForMomentum(theclass->momentum().mag());
    case TrackCollHandleBase::COLOUR_DISTANCE:
    { 
    // AscObjSelectionManager* selManager=  theclass->common()->ascObjSelectionManager();
    //      TrackHandleBase* handle = 0; 
    //      if ( selManager ) {  
    //        QList<AssociatedObjectHandleBase*> selection = selManager->currentSelection();
    //        if (!selection.empty()) handle = selection[0]->trackHandle(); // Take first at the moment, but should loop and colour by all. FIXME!
    //        else theclass->collHandle()->systemBase()->message("Empty selection!");
    //      } else {
    //        theclass->collHandle()->systemBase()->message("No AscObjSelectionManager");
    //      }

      TrackHandleBase* handle = theclass->common()->lastSelectedTrackHandle();
      if (handle==0) {
      //theclass->collHandle()->systemBase()->message("No previously selected track.");
        return theclass->collHandle()->material(); // use collection colouring
      }

      Amg::Vector3D selectedTrackMom = handle->momentum();
      Amg::Vector3D thisTrackMom     = theclass->momentum();

      float phiDistance  =  sqrt ( pow( selectedTrackMom.phi() - thisTrackMom.phi(),2) );
      float etaDistance  =  sqrt ( pow( selectedTrackMom.eta() - thisTrackMom.eta(),2) );

    //theclass->collHandle()->systemBase()->message("Distance "+QString::number(distance));
      float colScale=std::max(0.0, std::min(1.0,phiDistance/(M_PI))); // means that min scale is reached 0.5 of total possible distance away.
      float brightness= std::max(0.2, 1.0-(etaDistance/5.0) );
    //theclass->collHandle()->systemBase()->message("Distance "+QString::number(distance)+"\t brightness "+QString::number(brightness));

      SoMaterial* mat = new SoMaterial;
      mat->ref();

    // get colour of collection.
      const SbColor& col=theclass->collHandle()->material()->diffuseColor[0];
      float r,g,b; 
      col.getValue(r,g,b);

      double r3,g3,b3;    
      if (colScale>0.01) {

      //Use QColor to get HSL
        QColor tempCol = QColor::fromRgbF( r,g,b );
        double h,s,v;
        tempCol.getHsvF(&h,&s,&v);

      //get opposite hue for farthest away points.
        h+=0.5;
        if (h>1.0) h-=1.0;
        tempCol.setHsvF(h,s,v);
        double r2,g2,b2;
        tempCol.getRgbF(&r2,&g2,&b2);

      // closest will have collection colour - far away will have opposite colour
        r3 = r+(r2-r)*colScale;
        b3 = b+(b2-b)*colScale;
        g3 = g+(g2-g)*colScale;

      } else {
      // too close - take default values
        r3=r;b3=b;g3=g;
      }    

      VP1MaterialButton::setMaterialParameters(mat,r3*brightness,g3*brightness,b3*brightness,
        theclass->collHandle()->collMaterialBrightness(),
        theclass->collHandle()->collMaterialTransparency());

      return mat;
    }
    case TrackCollHandleBase::COLOUR_VERTEX:
    {
      SoMaterial* mat = theclass->common()->system()->materialFromVertex(theclass);
      if (mat) 
        return mat;
      else 
        return theclass->collHandle()->material();
    }
    case TrackCollHandleBase::COLOUR_PERCOLLECTION:
    default:
    return theclass->collHandle()->material();
  }
}

//____________________________________________________________________
void TrackHandleBase::rerandomiseRandomMaterial()
{
  if ( !m_d->randommaterial )//We will anyway rerandomize it when we need it
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
        m_d->theclass->collHandle()->systemBase()->message("TrackHandleBase::rerandomiseRandomMaterial Warning: Random colour could"
          " not be selected such as to satisfy all separation criterias");
        break;
      }
    }
  }
  VP1MaterialButton::setMaterialParameters(m_d->randommaterial,r,g,b,
    collHandle()->collMaterialBrightness(),
    collHandle()->collMaterialTransparency());
}

//____________________________________________________________________
void TrackHandleBase::updateRandomColourTransparencyAndBrightness()
{
  if ( !m_d->randommaterial )//We will anyway set up correctly when needed.
    return;
  float r,g,b;
  m_d->randommaterial->diffuseColor[0].getValue(r,g,b);
  VP1MaterialButton::setMaterialParameters(m_d->randommaterial,r,g,b,
    collHandle()->collMaterialBrightness(),
    collHandle()->collMaterialTransparency());
}


//____________________________________________________________________
void TrackHandleBase::registerAssocObject(AssociatedObjectHandleBase* ao)
{
  if (VP1Msg::verbose()) {
    if (m_d->associatedObjects.contains(ao)) {
      m_d->theclass->collHandle()->systemBase()->message("TrackHandleBase::registerAssocObject ERROR: object already registered!");
      return;
    }
  }
  //Assume ownership of this associated object:
  if (!ao) {
    m_d->theclass->collHandle()->systemBase()->message("TrackHandleBase::registerAssocObject ERROR: object is null!");
    return;
  }

  m_d->associatedObjects << ao;
}


//____________________________________________________________________
AssocObjAttachmentHandle * TrackHandleBase::getAttachmentHandle(int regionIndex, const double&crossoverval)
{
  std::map<std::pair<int,double>,AssocObjAttachmentHandle *>::iterator
    it = m_d->attachmentHandles.find(std::make_pair(regionIndex,crossoverval));
  if (it!=m_d->attachmentHandles.end())
    return it->second;
  AssocObjAttachmentHandle *attachHandle
    = new AssocObjAttachmentHandle(common()->trackLODManager()->getLODHandle(regionIndex,crossoverval),this);
  m_d->attachmentHandles[std::make_pair(regionIndex,crossoverval)] = attachHandle;
  return attachHandle;
}

//____________________________________________________________________
double TrackHandleBase::calculateCharge() const
{
  //look up from pdgCode();//fixme: 0 or -999 means unknown??
  int pdg = pdgCode();
  if (pdg) {
    bool ok;
    double c = VP1ParticleData::particleCharge(pdg,ok);
    if (ok) {
      //       if (VP1Msg::verbose())
      //  VP1Msg::messageVerbose("TrackHandleBase: Determined charge from pdg code "+VP1Msg::str(pdg)+": "+VP1Msg::str(c));
      return c;
    }
  }

  //Attempt to look at the first trackparameter.
  const Trk::Track * trk = provide_pathInfoTrkTrack();
  if (trk&&!trk->trackParameters()->empty()) {
    double c = (*(trk->trackParameters()->begin()))->charge();
    //     if (VP1Msg::verbose())
    //       VP1Msg::messageVerbose("TrackHandleBase: Determined charge from first track parameter: "+VP1Msg::str(c));
    return c;
  }
  if (VP1Msg::verbose())
    VP1Msg::messageVerbose("Failed to determine charge.");

  return unknown();
}

//____________________________________________________________________
double TrackHandleBase::charge() const
{
  if (m_d->chargeinit)
    return m_d->charge;
  m_d->chargeinit = true;
  m_d->charge = calculateCharge();
  return m_d->charge;
}

//____________________________________________________________________
double TrackHandleBase::mass() const
{
  if (m_d->massinit)
    return m_d->mass;
  m_d->massinit = true;
  int pdg = pdgCode();
  if (pdg) {
    bool ok;
    double m = VP1ParticleData::particleMass(pdg,ok);
    if (ok)
      m_d->mass = m;
  }
  return m_d->mass;
}

//____________________________________________________________________
Trk::ParticleHypothesis TrackHandleBase::extrapolationParticleHypothesis() const
{
  switch(abs(pdgCode())) {
    case 211: return Trk::pion;
    case 2212: return Trk::proton;
    case 11: return Trk::electron;
    case 13: return Trk::muon;
    case 321://k+
    case 323://k*+
    return Trk::kaon;
    case 0:
    default:
    return (hasCharge()&&charge()==0.0) ? Trk::nonInteracting : getNMuonHits()>0?Trk::muon:Trk::pion;
  }
}

//____________________________________________________________________
QStringList TrackHandleBase::baseInfo() const
{
  QStringList l;
  Amg::Vector3D mom = momentum();
  mom /= CLHEP::GeV;
  if (mom.mag2()==0.0) {
    l << "Momentum : 0 (undefined)";
  } else {
    l << "Momentum [GeV]: "+VP1Msg::str(mom);
    l << "|Pt|/|P| [GeV]: "+VP1Msg::str(mom.perp())+" / " + VP1Msg::str(mom.mag());
    l << VP1Msg::str("(")+QChar(0x03B7)+","+QChar(0x03D5)+VP1Msg::str(")=(")
        +VP1Msg::str(mom.eta())+VP1Msg::str(",")+VP1Msg::str(VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() ))+VP1Msg::str(")");
    
    l << "Eta: "+VP1Msg::str(mom.eta());
    l << "Phi: "+VP1Msg::str(VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() ));
  }
  l<<"Hit summary: Pix["+VP1Msg::str(getNPixelHits())+"], SCT["+VP1Msg::str(getNSCTHits())+"], TRT["+VP1Msg::str(getNTRTHits())
    +"], MDT["+QString::number(getNMDTHits())+"], RPC["+QString::number(getNRPCHits())+"], TGC["+QString::number(getNTGCHits())+"], CSC["+QString::number(getNCSCHits())+"].";

  return l;
  /** coverity 17186: this code is unreachable
  int pdg = pdgCode();
  if (pdg) {
    bool ok;
    QString name = VP1ParticleData::particleName(pdg,ok);
    if (ok)
      l << "pdg: "+QString::number(pdg)+" ["+name+"]";
    else
      l << "pdg: "+QString::number(pdg);
  }

  return l;
  **/
}

//____________________________________________________________________
const Amg::Vector3D * TrackHandleBase::startPoint() const
{
  m_d->ensureLoadPathInfo();
  if (m_d->pathInfo_TrkTrack) {
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_iter = m_d->pathInfo_TrkTrack->trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_end = m_d->pathInfo_TrkTrack->trackStateOnSurfaces()->end();
    for (; tsos_iter != tsos_end; ++tsos_iter) {
      if (common()->trackSanityHelper()->isSafe(*tsos_iter)) {
        const Trk::TrackParameters* trackParam = (*tsos_iter)->trackParameters();
        if (common()->trackSanityHelper()->isSafe(trackParam))
          return &(trackParam->position());
      }
    }
  } else if (m_d->pathInfo_Points&&!m_d->pathInfo_Points->empty()) {
    return &(m_d->pathInfo_Points->at(0));
  }
  return 0;
}

//____________________________________________________________________
const Amg::Vector3D * TrackHandleBase::endPoint() const
{
  m_d->ensureLoadPathInfo();
  if (m_d->pathInfo_TrkTrack) {
    DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator tsos_iter = m_d->pathInfo_TrkTrack->trackStateOnSurfaces()->rend();
    DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator tsos_end = m_d->pathInfo_TrkTrack->trackStateOnSurfaces()->rbegin();
    for (; tsos_iter != tsos_end; ++tsos_iter) {
      if (common()->trackSanityHelper()->isSafe(*tsos_iter)) {
        const Trk::TrackParameters* trackParam = (*tsos_iter)->trackParameters();
        if (common()->trackSanityHelper()->isSafe(trackParam))
          return &(trackParam->position());
      }
    }
  } else if (m_d->pathInfo_Points&&!m_d->pathInfo_Points->empty()) {
    return &(m_d->pathInfo_Points->back());
  }
  return 0;
}

bool TrackHandleBase::isIDTrack() const 
{
  const Amg::Vector3D * start = startPoint();
  if (!start) return false;
  return start->perp()<1100 &&fabs( start->perp())<3500;  
}


//____________________________________________________________________
QList<AssociatedObjectHandleBase*> TrackHandleBase::getAllAscObjHandles() const
{
  return m_d->associatedObjects;
}

std::vector< Amg::Vector3D > * TrackHandleBase::hackGetPointsPropagated()
{
  m_d->ensureInitPointsPropagated();
  return m_d->points_propagated;
}

//____________________________________________________________________
class AssocObjAttachmentHandle::Imp {
public:
  Imp(TrackLODHandle *tlh,TrackHandleBase * th)
    : trackhandle(th),
    trackmat(0),
    lodHandle(tlh),
    septrack_simple(0),
    septrack_detailed(0),
    pickStyleChildIdx(-1),
    attached(false) {}
  TrackHandleBase * trackhandle;
  SoMaterial* trackmat;
  TrackLODHandle *lodHandle;
  SoSeparator * septrack_simple;
  SoSeparator * septrack_detailed;
  void ensureInit();
  static SoPickStyle * pickStyle;
  int pickStyleChildIdx;
  bool attached;
  void ensureAttached()
  {
    if (attached)
      return;
    attached=true;
    if (!septrack_detailed)
      return;//We attach in ensureInit in this case!
    lodHandle->addNodes(septrack_simple,septrack_detailed);
  }
  void ensureDetached()
  {
    if (!attached)
      return;
    attached=false;
    if (!septrack_detailed)
      return;
    lodHandle->removeNodes(septrack_simple,septrack_detailed);
  }
  static SoMaterial * dummymaterial;
};
SoPickStyle * AssocObjAttachmentHandle::Imp::pickStyle = 0;
SoMaterial * AssocObjAttachmentHandle::Imp::dummymaterial = 0;

//____________________________________________________________________
void AssocObjAttachmentHandle::trackVisibilityChanged()
{
  bool b = m_d->trackhandle->visible();
  if (b&&!m_d->trackhandle->currentMaterial())
    VP1Msg::message("ERROR: track is visible but has not current material!!");
  if (b)
    m_d->ensureAttached();
  else
    m_d->ensureDetached();
}

//____________________________________________________________________
AssocObjAttachmentHandle::AssocObjAttachmentHandle(TrackLODHandle * tlh,TrackHandleBase * th)
  : m_d ( new Imp(tlh,th) )
{
  if (!tlh)
    VP1Msg::message("AssocObjAttachmentHandle ERROR: constructed with null LOD handle!");
  if (!Imp::dummymaterial) {
    Imp::dummymaterial = new SoMaterial;
    Imp::dummymaterial->ref();
  }
}

//____________________________________________________________________
AssocObjAttachmentHandle::~AssocObjAttachmentHandle()
{
  if (VP1Msg::verbose()) {
    if ((m_d->septrack_detailed && m_d->pickStyleChildIdx != m_d->septrack_detailed->findChild(Imp::pickStyle))
    ||(m_d->septrack_simple&&m_d->pickStyleChildIdx != m_d->septrack_simple->findChild(Imp::pickStyle))) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::setPickableStateOfNodes detected wrong pickStyleChildIdx");
      return;
    }
  }

  if (Imp::pickStyle) {
    Imp::pickStyle->unref();
    if ( Imp::pickStyle->getRefCount()==1 ) {
      Imp::pickStyle->unref();
      Imp::pickStyle = 0;
    }
  }

  if (m_d->septrack_simple)
    m_d->septrack_simple->unref();
  if (m_d->septrack_detailed)
    m_d->septrack_detailed->unref();
  if (m_d->trackmat)
    m_d->trackmat->unref();
  delete m_d;
}

//____________________________________________________________________
void AssocObjAttachmentHandle::trackMaterialChanged()
{
  SoMaterial * m = m_d->trackhandle->currentMaterial();
  if (m_d->trackmat==m)
    return;
  if (!m) {
    if (m_d->trackhandle->visible())
      VP1Msg::message("ERROR: track is visible but has no current material!!");
    m = Imp::dummymaterial;
  }
  m->ref();
  if (m_d->trackmat) {
    if ( m_d->septrack_simple && m_d->septrack_simple->findChild(m_d->trackmat) > -1 )
      m_d->septrack_simple->replaceChild(m_d->trackmat,m);
    if ( m_d->septrack_detailed && m_d->septrack_detailed->findChild(m_d->trackmat) > -1 )
      m_d->septrack_detailed->replaceChild(m_d->trackmat,m);
    m_d->trackmat->unref();
  }
  m_d->trackmat = m;
}

//____________________________________________________________________
void AssocObjAttachmentHandle::Imp::ensureInit()
{
  if (septrack_simple)
    return;
  septrack_simple = new SoSeparator;
  septrack_detailed = new SoSeparator;
  septrack_simple->ref();
  septrack_detailed->ref();

  if (!pickStyle) {
    pickStyle = new SoPickStyle;
    pickStyle->style=SoPickStyle::UNPICKABLE;
    pickStyle->ref();
  }
  pickStyle->ref();
  if (attached) {
    attached = false;
    ensureAttached();
  }
}

//____________________________________________________________________
void AssocObjAttachmentHandle::attachNodes( SoNode*simple, SoNode*detailed, bool unpickable )
{
  if (!m_d->trackhandle->visible())
    return;
  m_d->ensureInit();
  if (!m_d->trackmat) {
    SoMaterial * m = m_d->trackhandle->currentMaterial();
    if (!m)
      m = Imp::dummymaterial;//For now we attach a dummy material.
    m_d->trackmat=m;
    m->ref();
    m_d->septrack_simple->addChild(m);
    m_d->septrack_detailed->addChild(m);
  }

  if (VP1Msg::verbose()) {
    if (m_d->pickStyleChildIdx != m_d->septrack_detailed->findChild(Imp::pickStyle)
    ||m_d->pickStyleChildIdx != m_d->septrack_simple->findChild(Imp::pickStyle)) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::attachNodes detected wrong pickStyleChildIdx");
      return;
    }
    if (m_d->septrack_simple->findChild(simple)>-1||m_d->septrack_detailed->findChild(detailed)>-1) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::attachNodes Already attached!");
      return;
    }
  }
  if (unpickable) {
    if (m_d->pickStyleChildIdx==-1) {
      m_d->pickStyleChildIdx = m_d->septrack_simple->getNumChildren();
      m_d->septrack_simple->addChild(Imp::pickStyle);
      m_d->septrack_detailed->addChild(Imp::pickStyle);
    }
    m_d->septrack_simple->addChild(simple);
    m_d->septrack_detailed->addChild(detailed);
  } else {
    if (m_d->pickStyleChildIdx>-1) {
      m_d->septrack_simple->insertChild(simple,m_d->pickStyleChildIdx);
      m_d->septrack_detailed->insertChild(detailed,m_d->pickStyleChildIdx);
      ++(m_d->pickStyleChildIdx);
    } else {
      m_d->septrack_simple->addChild(simple);
      m_d->septrack_detailed->addChild(detailed);
    }
  }

  //Should we update the overall attachment status?:
  if (m_d->trackhandle->visible() && m_d->septrack_simple->getNumChildren() == 2 + (m_d->pickStyleChildIdx==-1?0:1)) {
    m_d->ensureAttached();
  }
}

//____________________________________________________________________
void AssocObjAttachmentHandle::setPickableStateOfNodes( SoNode*simple, SoNode*detailed, bool unpickable )
{
  if (VP1Msg::verbose()) {
    VP1Msg::messageVerbose("AssocObjAttachmentHandle::setPickableStateOfNodes called with unpickable = "
      +QString(unpickable?"true":"false"));
    if (!simple||!detailed) {
      VP1Msg::messageVerbose("AssocObjAttachmentHandle::setPickableStateOfNodes ERROR: Called with null pointers!");
      return;
    }
  }
  m_d->ensureInit();
  int isimple = m_d->septrack_simple->findChild(simple);
  if (VP1Msg::verbose()) {
    if (m_d->septrack_simple->getNumChildren()!=m_d->septrack_detailed->getNumChildren()) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::setPickableStateOfNodes septrack_simple->getNumChildren()"
        "!=m_d->septrack_detailed->getNumChildren().");
      return;
    }
    int idetailed = m_d->septrack_detailed->findChild(detailed);
    if (idetailed!=isimple) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::setPickableStateOfNodes"
        " called with simple and detailed nodes that are not at same child idx!");
      return;
    }
    if (m_d->pickStyleChildIdx != m_d->septrack_detailed->findChild(Imp::pickStyle)
    ||m_d->pickStyleChildIdx != m_d->septrack_simple->findChild(Imp::pickStyle)) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::setPickableStateOfNodes detected wrong pickStyleChildIdx");
      return;
    }
  }
  if (isimple<0) {
    VP1Msg::messageDebug( "WARNING: AssocObjAttachmentHandle::setPickableStateOfNodes"
      " called with nodes that are not currently children. Calling attachNodes(..)." );
    attachNodes(simple, detailed, unpickable);
    return;
  }
  if (unpickable == (m_d->pickStyleChildIdx>-1&&isimple>m_d->pickStyleChildIdx)) {
    VP1Msg::messageDebug("WARNING: AssocObjAttachmentHandle::setPickableStateOfNodes"
      " already in correct state.");
    return;
  }

  simple->ref();//To avoid deletion upon removeChild calls.
  detailed->ref();
  if (unpickable) {
    m_d->septrack_simple->removeChild(simple);
    m_d->septrack_detailed->removeChild(detailed);
    if (m_d->pickStyleChildIdx==-1) {
      m_d->pickStyleChildIdx = m_d->septrack_simple->getNumChildren();
      m_d->septrack_simple->addChild(Imp::pickStyle);
      m_d->septrack_detailed->addChild(Imp::pickStyle);
    } else {
      --(m_d->pickStyleChildIdx);
    }
    m_d->septrack_simple->addChild(simple);
    m_d->septrack_detailed->addChild(detailed);
  } else {
    if (m_d->pickStyleChildIdx==-1) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::setPickableStateOfNodes Inconsistent logic");
      simple->unref();
      detailed->unref();
      return;
    }
    if (m_d->pickStyleChildIdx==isimple-1&&isimple==m_d->septrack_simple->getNumChildren()-1) {
      //pickStyle object no longer needed:
      VP1Msg::messageVerbose("AssocObjAttachmentHandle::setPickableStateOfNodes detaching pickstyle");
      m_d->septrack_simple->removeChild(Imp::pickStyle);
      m_d->septrack_detailed->removeChild(Imp::pickStyle);
      m_d->pickStyleChildIdx = -1;
    } else {
      //Move children left of the pickstyle object.
      m_d->septrack_simple->removeChild(simple);
      m_d->septrack_detailed->removeChild(detailed);
      m_d->septrack_simple->insertChild(simple,m_d->pickStyleChildIdx);
      m_d->septrack_detailed->insertChild(detailed,m_d->pickStyleChildIdx);
      ++(m_d->pickStyleChildIdx);
    }
  }
  simple->unref();
  detailed->unref();
}

//____________________________________________________________________
void AssocObjAttachmentHandle::detachNodes( SoNode*simple, SoNode*detailed )
{
  if (!m_d->trackmat)
    return;
  int isimple = m_d->septrack_simple->findChild(simple);
  if (VP1Msg::verbose()) {
    if (!m_d->septrack_simple) {
      //       VP1Msg::message("ERROR: AssocObjAttachmentHandle::detachNodes Not initialised previously!.");
      m_d->ensureInit();
    }
    if (!simple||!detailed) {
      VP1Msg::messageVerbose("AssocObjAttachmentHandle::detach ERROR: Called with null pointers!");
      return;
    }
    if (m_d->septrack_simple->getNumChildren()!=m_d->septrack_detailed->getNumChildren()) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::detachNodes septrack_simple->getNumChildren()"
        "!=m_d->septrack_detailed->getNumChildren().");
      return;
    }
    if (m_d->pickStyleChildIdx != m_d->septrack_detailed->findChild(Imp::pickStyle)
    ||m_d->pickStyleChildIdx != m_d->septrack_simple->findChild(Imp::pickStyle)) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::detachNodes detected wrong pickStyleChildIdx");
      return;
    }
    if (isimple==-1) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::detachNodes Not previously attached!");
      return;
    }
    if (m_d->septrack_detailed->findChild(detailed)!=isimple) {
      VP1Msg::message("ERROR: AssocObjAttachmentHandle::setPickableStateOfNodes"
        " called with simple and detailed nodes that are not at same child idx!");
      return;
    }
  }
  m_d->septrack_simple->removeChild(simple);
  m_d->septrack_detailed->removeChild(detailed);
  if ( m_d->pickStyleChildIdx!=-1
    && isimple>m_d->pickStyleChildIdx
  && m_d->pickStyleChildIdx==m_d->septrack_simple->getNumChildren()-1 ) {
    VP1Msg::messageVerbose("AssocObjAttachmentHandle::detachNodes detaching pickstyle");
    m_d->septrack_simple->removeChild(Imp::pickStyle);
    m_d->septrack_detailed->removeChild(Imp::pickStyle);
    m_d->pickStyleChildIdx = -1;
  }

  //Should we update the overall attachment status?:
  if (m_d->trackhandle->visible() && m_d->septrack_simple->getNumChildren() == 1 + (m_d->pickStyleChildIdx==-1?0:1))
    m_d->ensureDetached();
}

//____________________________________________________________________
QList<AssociatedObjectHandleBase*> TrackHandleBase::getVisibleMeasurements() const
{
  QList<AssociatedObjectHandleBase*> l;
  if (!m_d->tsos_ascobjs) return l;
  std::vector<AscObj_TSOS*>::iterator
    it(m_d->tsos_ascobjs->begin()),
    itE(m_d->tsos_ascobjs->end());
  for (;it!=itE;++it)
    if ((*it)->hasMeasurement()&&(*it)->visible())
    l << (*it);
  return l;
}

void TrackHandleBase::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) {
  VP1Msg::messageVerbose("TrackHandleBase::fillObjectBrowser");

  assert(m_d->m_objBrowseTree==0);
  m_d->m_objBrowseTree = new QTreeWidgetItem();

  QString l = shortInfo();
  m_d->m_objBrowseTree->setText(0, type()+QString(QString::number(listOfItems.size())) );
  m_d->m_objBrowseTree->setText(1, l );    

  if (!visible()) {
    m_d->m_objBrowseTree->setFlags(0); // not selectable, not enabled
  }
  listOfItems << browserTreeItem();
}

QTreeWidgetItem* TrackHandleBase::browserTreeItem() const {return m_d->m_objBrowseTree;}

//____________________________________________________________________
void TrackHandleBase::visibleStateChanged()
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
