/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDHandleBase.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1PRDSystems/PRDCollHandleBase.h"
#include "VP1PRDSystems/VP1PrepRawDataSystem.h"
#include "VP1PRDSystems/PRDTrackSegmentHelper.h"
#include "VP1PRDSystems/TouchedMuonChamberHelper.h"

#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1SoMaterialMixer.h"
#include "VP1Base/VP1Msg.h"

#include "VP1Utils/VP1LinAlgUtils.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>

#include "TrkSurfaces/Surface.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include <vector>
#include <algorithm>
#include <sstream>

//____________________________________________________________________
class PRDHandleBase::Imp {
public:
  PRDCollHandleBase* colhandle;
  void rebuild3DObjects(PRDHandleBase*);
  void attach3DObjects(PRDHandleBase*);
  void detach3DObjects();

  static Amg::Vector3D positionPRD(const Trk::PrepRawData* prd);

  //Child 0 is the transform, then follows the node representing the shapes.
  SoSeparator * sepDetailed;
  SoSeparator * sepSimple;
  SoMaterial * currentmaterial;
  VP1ExtraSepLayerHelper* sephelper_lod_detail;
  VP1ExtraSepLayerHelper* sephelper_lod_simple;

  SoMaterial * determineMaterial(PRDHandleBase*);
};

//____________________________________________________________________
SoSeparator* PRDHandleBase::sepSimple() const
{
  return m_d->sepSimple;
}

//____________________________________________________________________
SoSeparator* PRDHandleBase::sepDetailed() const
{
  return m_d->sepDetailed;
}

//____________________________________________________________________
PRDHandleBase::PRDHandleBase(PRDCollHandleBase*colhandle)
  : m_d(new Imp), m_common(colhandle->common()),m_visible(false)
{
  m_d->colhandle = colhandle;
  m_d->sepDetailed = 0;
  m_d->sepSimple = 0;
  m_d->currentmaterial = 0;
  m_d->sephelper_lod_detail = 0;
  m_d->sephelper_lod_simple = 0;
}


//____________________________________________________________________
PRDHandleBase::~PRDHandleBase()
{
  if (m_d->sepDetailed)
    m_d->sepDetailed->unref();
  if (m_d->sepSimple)
    m_d->sepSimple->unref();
  if (m_d->currentmaterial)
    m_d->currentmaterial->unref();

  delete m_d;
}

//____________________________________________________________________
void PRDHandleBase::Imp::rebuild3DObjects(PRDHandleBase*theclass)
{  
  //Fixme:
  SoTransform * transform(0);
  //If visible and already build: Detach separators (and ensure we got the sephelpers):
  if (theclass->m_visible) {
    if (!sephelper_lod_detail) {
      int idx = theclass->regionIndex();
      colhandle->getLODSeparators(idx, sephelper_lod_detail,sephelper_lod_simple);
    }
    detach3DObjects();
  }

  //Ensure separators are built and with no children:
  if (sepDetailed) {
    //We might get transform from this one:
    if (sepDetailed->getNumChildren()>0) {      
      transform = static_cast<SoTransform*>(sepDetailed->getChild(0));
      transform->ref();
    }
    sepDetailed->removeAllChildren();
  } else {
    sepDetailed = new SoSeparator;
    sepDetailed->ref();
    //     sepDetailed->renderCaching.setValue(SoSeparator::ON);
    //     sepDetailed->boundingBoxCaching.setValue(SoSeparator::ON);
  }
  if (sepSimple) {
    sepSimple->removeAllChildren();
  } else {
    sepSimple = new SoSeparator;
    //     sepSimple->renderCaching.setValue(SoSeparator::ON);
    //     sepSimple->boundingBoxCaching.setValue(SoSeparator::ON);
    sepSimple->ref();
  }
  //Attach transforms:
  if (!transform) {    
    transform = theclass->createTransform();
    transform->ref();
    theclass->common()->registerTransform2Handle(transform,theclass);
  }
  sepDetailed->addChild(transform);//For efficiency it is important that this happens BEFORE buildShapes(..) is called.
  sepSimple->addChild(transform);
  transform->unref();

  SoNode* shape_simple(0), * shape_detailed(0);
  theclass->buildShapes(shape_simple, shape_detailed);
  if (!shape_simple)
    shape_simple = theclass->common()->nodeManager()->getShapeNode_Point();//Add single point as a stop-gap measure.
  if (!shape_detailed)
    shape_detailed = shape_simple;//Could not create detailed shape for some reason. Use simple shape instead.

  sepSimple->addChild(shape_simple);
  sepDetailed->addChild(shape_detailed);

  //Attach if visible:
  if (theclass->m_visible)
    attach3DObjects(theclass);
}

// //____________________________________________________________________
// inline void PRDHandleBase::registerTransform(SoTransform*transform)//FIXME: Pass-through not needed
// {
//   //Use this pass-through method to get around a friend-problem
//   //FIXME  common()->system()->registerTransform2Handle(transform,this);
// }

//____________________________________________________________________
void PRDHandleBase::updateMaterial()
{
  //We need to change the current material. This means we need to
  //clear the cache. Only if we are visible do we need to do a
  //detach->clear material->attach cycle to trigger attachment under new material.

  if (!m_d->currentmaterial)
    return;//We have no material cached and is thus not attached either.
  if (m_visible) {
    //See if the material changed. If it did, detach, update the material, attach again.
    SoMaterial * newmat = m_d->determineMaterial(this);
    newmat->ref();
    if (newmat!=m_d->currentmaterial) {
      m_d->detach3DObjects();
      m_d->currentmaterial->unref();
      m_d->currentmaterial = newmat;
      m_d->attach3DObjects(this);
    } else {
      newmat->unref();
    }
  } else {
    //Just clear material.
    m_d->currentmaterial->unref();
    m_d->currentmaterial = 0;
  }
}

//____________________________________________________________________
void PRDHandleBase::Imp::attach3DObjects(PRDHandleBase*theclass)
{
  if (!currentmaterial) {
    currentmaterial = determineMaterial(theclass);
    currentmaterial->ref();
  }
  if (sepDetailed) {
    if (colhandle->sephelperDetailedNodes())
      colhandle->sephelperDetailedNodes()->addNodeUnderMaterial(sepDetailed,currentmaterial);
    if (sephelper_lod_detail)
      sephelper_lod_detail->addNodeUnderMaterial(sepDetailed,currentmaterial);
  }
  if (sepSimple) {
    if (colhandle->sephelperSimpleNodes())
      colhandle->sephelperSimpleNodes()->addNodeUnderMaterial(sepSimple,currentmaterial);
    if (sephelper_lod_simple)
      sephelper_lod_simple->addNodeUnderMaterial(sepSimple,currentmaterial);
  }
}

//____________________________________________________________________
void PRDHandleBase::Imp::detach3DObjects()
{
  if (!currentmaterial)
    return;//Can never have been attached!
  if (sepDetailed) {
    if (colhandle->sephelperDetailedNodes())
      colhandle->sephelperDetailedNodes()->removeNodeUnderMaterial(sepDetailed,currentmaterial);
    if (sephelper_lod_detail)
      sephelper_lod_detail->removeNodeUnderMaterial(sepDetailed,currentmaterial);
  }
  if (sepSimple) {
    if (colhandle->sephelperSimpleNodes())
      colhandle->sephelperSimpleNodes()->removeNodeUnderMaterial(sepSimple,currentmaterial);
    if (sephelper_lod_simple)
      sephelper_lod_simple->removeNodeUnderMaterial(sepSimple,currentmaterial);
  }
}

//____________________________________________________________________
PRDCollHandleBase * PRDHandleBase::collHandle() const
{
  return m_d->colhandle;
}

//____________________________________________________________________
void PRDHandleBase::setVisible(bool vis)
{
  if (vis==m_visible)
    return;
  //std::cout<<"Changing visible status from "<<m_visible<<" to "<<vis<<" for: "<<*getPRD()<<std::endl;
  m_visible=vis;
  if (vis) {
    m_d->colhandle->incrementNShownHandles();
    if (inMuonChamber())
      common()->touchedMuonChamberHelper()->incrementNumberOfObjectsForPV(parentMuonChamberPV());
    if (!m_d->sepSimple||!m_d->sepDetailed)
      m_d->rebuild3DObjects(this);//The call to rebuild also fixes attached state.
    else
      m_d->attach3DObjects(this);
  } else {
    m_d->colhandle->decrementNShownHandles();
    if (inMuonChamber())
      common()->touchedMuonChamberHelper()->decrementNumberOfObjectsForPV(parentMuonChamberPV());
    m_d->detach3DObjects();
  }
}

//____________________________________________________________________
void PRDHandleBase::update3DObjects() {
  //Fixme: If selected we really need to redo selection updates!!!
  if (m_visible) {
    m_d->rebuild3DObjects(this);
  } else {
    //Simply clear the present 3D objects. They will only be recreated if/when the prd becomes visible again.
    if (m_d->sepDetailed) {
      m_d->sepDetailed->unref();
      m_d->sepDetailed = 0;
    }
    if (m_d->sepSimple) {
      m_d->sepSimple->unref();
      m_d->sepSimple = 0;
    }
  }

}

//____________________________________________________________________
QStringList PRDHandleBase::clicked() const
{
  const Trk::PrepRawData * prd = getPRD();
  if (!prd)
    return QStringList("Null PRD");
  std::ostringstream os;
  os << *(prd);
  return QString(os.str().c_str()).split("\n");
}


//____________________________________________________________________
Amg::Vector3D PRDHandleBase::Imp::positionPRD(const Trk::PrepRawData* prd)
{
  if (!prd)
    return Amg::Vector3D(0,0,0);
  if (!prd||!prd->detectorElement())
    return Amg::Vector3D(0.0,0.0,0.0);
  const Trk::Surface& theSurface = prd->detectorElement()->surface(prd->identify());

  const Amg::Vector3D* hitpos = theSurface.localToGlobal(prd->localPosition());
  if (!hitpos)
    return Amg::Vector3D(0.0,0.0,0.0);

  const Amg::Vector3D pos(*hitpos);
  delete hitpos;
  return pos;
}

//____________________________________________________________________
Amg::Vector3D PRDHandleBase::positionPRD() const
{
  return Imp::positionPRD(getPRD());
}

//____________________________________________________________________
Amg::Vector3D PRDHandleBase::positionSecondPRD() const
{
  return Imp::positionPRD(getSecondPRD());
}


//____________________________________________________________________
Amg::Vector3D PRDHandleBase::center() const
{
  return positionPRD();
}

//____________________________________________________________________
Amg::Transform3D PRDHandleBase::getTransform_CLHEP() const
{  
  if (m_d->sepDetailed&&m_d->sepDetailed->getNumChildren()>0) {
    //For efficiency/consistency we simply copy the transform from the
    //inventor transformation (only loose slight precision in the
    //double->float->double conversions):
    SoTransform * transform = static_cast<SoTransform*>(m_d->sepDetailed->getChild(0));
    const SbVec3f so_translation(transform->translation.getValue());
    float tx, ty, tz;
    transform->translation.getValue().getValue(tx,ty,tz);
    SbVec3f so_rotaxis;
    float so_rotangle;
    transform->rotation.getValue().getValue(so_rotaxis, so_rotangle);
    float rx, ry, rz;
    so_rotaxis.getValue(rx,ry,rz);
    Amg::Vector3D axis(rx,ry,rz);
//    Amg::AngleAxis3D angleAxis = Amg::getRotation3DfromAngleAxis( so_rotangle, axis );
//    Amg::Rotation3D rot;
//    rot = angleAxis;
    Amg::Rotation3D rot = Amg::getRotation3DfromAngleAxis( so_rotangle, axis );
    return Amg::Translation3D(tx,ty,tz) * rot;
  }
  std::cout<<"VP1 PRDHandleBase Warning: getTransform_CLHEP called at inopportune moment!!"<<std::endl;

  //NB: Some code duplicated here and in the createTransform() method.

  const Trk::PrepRawData * prd = getPRD();
  if (!prd) {
    return Amg::Transform3D::Identity();
  }
  const Trk::Surface& theSurface = prd->detectorElement()->surface(prd->identify());
  const Amg::Vector3D* theHitGPos;
  if ( transformUsesSurfacePositionOnly() ) {
    // for tubes, should use position of center of tube (if drawing full tube)
    theHitGPos= new Amg::Vector3D (theSurface.center());
  } else {
    // for clusters or short tubes, use position of hit.
    theHitGPos = theSurface.localToGlobal(prd->localPosition()); //theSurface 'new s' a Vector3D and returns pointer
  }

//  CLHEP::Hep3Vector t(theSurface.transform().getTranslation());
//  return HepGeom::Transform3D( HepGeom::Translate3D(theHitGPos->x()-t.x(),theHitGPos->y()-t.y(),theHitGPos->z()-t.z()) * (theSurface.transform()) );
  Amg::Vector3D t;
  t = Amg::getTranslationVectorFromTransform(theSurface.transform());
  Amg::Translation3D transl = Amg::Translation3D(theHitGPos->x()-t.x(), theHitGPos->y()-t.y(), theHitGPos->z()-t.z());
  Amg::Transform3D transf = transl * (theSurface.transform());
  delete theHitGPos;
  return transf;
}

//____________________________________________________________________
SoTransform * PRDHandleBase::createTransform() const
{  
  //NB: Some code duplicated here and in the getTransform_CLHEP() method.
  const Trk::PrepRawData * prd = getPRD();
  if (!prd) {
    std::cerr<<"PRDHandleBase::createTransform() No prd!"<<std::endl;
    return new SoTransform;
  }
  const Trk::Surface& theSurface = prd->detectorElement()->surface(prd->identify());
  SoTransform * theHitTransform = VP1LinAlgUtils::toSoTransform(theSurface.transform());

  const Amg::Vector3D* theHitGPos;
  if ( transformUsesSurfacePositionOnly() ) {
    // for tubes, should use position of center of tube (if drawing full tube)
    theHitGPos= new Amg::Vector3D (theSurface.center());
  } else {
    // for strips, clusters or short tubes, use position of hit.
    theHitGPos = theSurface.localToGlobal(prd->localPosition());
  }
  if ((*theHitGPos)[0]!=(*theHitGPos)[0] || (*theHitGPos)[1]!=(*theHitGPos)[1] || (*theHitGPos)[2]!=(*theHitGPos)[2]){
    std::cerr<<"PRDHandleBase::createTransform() NaN in globalposition"<<std::endl;
  }
  theHitTransform->translation.setValue((*theHitGPos)[0], (*theHitGPos)[1], (*theHitGPos)[2]);
  delete theHitGPos;
  return theHitTransform;
}

//____________________________________________________________________
SoMaterial * PRDHandleBase::Imp::determineMaterial(PRDHandleBase*theclass)
{
  double hlw = colhandle->highLightWeight();
  //Get most frequent special case (i.e. no highlighting ) out of the
  //way first for efficiency:
  if (colhandle->colourMethod()!=PRDCollHandleBase::ByTechOnly) {
    std::map<SoMaterial*,double> materials;
      //-> Retrieve tracks/segments.
      const PRDTrackSegmentHelper::TracksAndSegments * ts = theclass->common()->trackAndSegmentHelper()->tracksAndSegments(theclass->getPRD());
      if (ts) {
	//-> Add all materials from tracks as appropriate
	if (colhandle->colourByTracks()) {
	  for (unsigned i = 0; i < ts->tracks.size(); ++i) {
	    SoMaterial * mat = theclass->common()->trackAndSegmentHelper()->trackMaterial(ts->tracks[i]);
	    if (mat)
	      materials[mat]=1.0;
	    else
	      std::cout<< "PRDHandleBase ERROR: Did not find track material!"<<std::endl;
	  }
	}
	//-> Add all materials from segments as appropriate
	if (colhandle->colourBySegments()) {
	  for (unsigned i = 0; i < ts->segments.size(); ++i) {
	    SoMaterial * mat = theclass->common()->trackAndSegmentHelper()->segmentMaterial(ts->segments[i]);
	    if (mat)
	      materials[mat]=1.0;
	    else
	      std::cout<< "PRDHandleBase ERROR: Did not find segment material (2)!"<<std::endl;
	  }
	}
	if (hlw>0.0&&colhandle->highLightOutliers()) {
	  //Add outliers (if colourbytracks) + a highlightmaterial, all with appropriate weights.
	  if (colhandle->colourByTracks()&&!ts->tracks_outliers.empty()) {
	    if (hlw>999.0) {
	      materials[colhandle->highLightMaterial()] = ts->tracks_outliers.size();
	    } else {
	      for (unsigned i = 0; i < ts->tracks_outliers.size(); ++i) {
		SoMaterial * mat = theclass->common()->trackAndSegmentHelper()->trackMaterial(ts->tracks_outliers[i]);
		if (mat)
		  materials[mat]=1.0/(hlw+1.0);
		else
		  std::cout<< "PRDHandleBase ERROR: Did not find track material (3)!"<<std::endl;
	      }
 	      materials[colhandle->highLightMaterial()] = ts->tracks_outliers.size()*hlw/(hlw+1.0);
	    }
	  }
	} else {
	  //Just add materials from outliers - and see if we need highlighting also.
	  if (colhandle->colourByTracks()) {
	    for (unsigned i = 0; i < ts->tracks_outliers.size(); ++i) {
	      SoMaterial * mat = theclass->common()->trackAndSegmentHelper()->trackMaterial(ts->tracks_outliers[i]);
	      if (mat)
		materials[mat]=1.0;
		else
		  std::cout<< "PRDHandleBase ERROR: Did not find track material (4)!"<<std::endl;
	    }
	  }
	  if (hlw>0.0&&!colhandle->highLightOutliers()&&theclass->highLight()) {
	    unsigned ntrackssegments= (colhandle->colourByTracks()?ts->tracks.size()+ts->tracks_outliers.size():0)
                                      +(colhandle->colourBySegments()?ts->segments.size():0);
	    if (ntrackssegments)
	      materials[colhandle->highLightMaterial()] = hlw*ntrackssegments;
	  }
	}
	if (!materials.empty())
	  return theclass->common()->materialMixer()->getMixedMaterial(materials);
      }
  }


  //OK, just use the tech colour - and possibly some non-outlier-highlighting.
  if (hlw>0.0&&!colhandle->highLightOutliers()&&theclass->highLight()) {
    if (hlw>999.0)
      return colhandle->highLightMaterial();
    return theclass->common()->materialMixer()->getMixedMaterial(colhandle->highLightMaterial(),hlw,colhandle->material(),1.0);
  } else {
    return colhandle->material();
  }
}
