/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class AscObj_TSOS                       //
//                                                            //
//  Author: Troels Kofoed Jacobsen                            //
//          Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: June 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

//Surface from materialeffects.

//Troels todo:
//  projections (not right away)

//Thomas todo:
//  print truth info (get rid of truth points)
//  Notify when path changes.
//
//  Fixme: shared objects? Or cache scene parts for faster updating??

#include "VP1TrackSystems/AscObj_TSOS.h"
#include "VP1TrackSystems/VP1TrackSystem.h"
#include "VP1TrackSystems/TrackSysCommonData.h"
#include "VP1TrackSystems/TrackHandleBase.h"
#include "VP1TrackSystems/TrackSystemController.h"
#include "VP1TrackSystems/AscObjSelectionManager.h"
#include "VP1TrackSystems/TrkObjToString.h"

#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Utils/SurfaceToSoNode.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Utils/VP1ErrorUtils.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1HEPVis/nodes/SoTransparency.h"

#include "AtlasDetDescr/AtlasDetectorID.h"

#include "TrkEventPrimitives/DriftCircleSide.h"
#include "TrkEventPrimitives/DriftCircleStatus.h"


#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkTrack/TrackStateOnSurface.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSphere.h>

#include <set>

#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

//____________________________________________________________________
AscObj_TSOS::AscObj_TSOS( TrackHandleBase *track,
              const Trk::TrackStateOnSurface * tsos,
              unsigned indexOfPointOnTrack )
  : AssociatedObjectHandleBase(track), m_tsos(tsos),
    m_parts(TrackCommonFlags::TSOS_NoObjects),
    m_indexOfPointOnTrack(indexOfPointOnTrack), 
    m_distToNextPar(-1), 
    m_objBrowseTree(0)
{
  SoTransparency::initClass();

  const Trk::Surface * surf(0);
  
  if (m_tsos->type(Trk::TrackStateOnSurface::Hole)) m_parts |= TrackCommonFlags::TSOS_Hole;
  
  if (m_tsos->trackParameters()) {
    m_parts |= TrackCommonFlags::TSOS_TrackPars;
    if (dynamic_cast<const Trk::TrackParameters *>(m_tsos->trackParameters())) {
      if (m_tsos->type(Trk::TrackStateOnSurface::Perigee)) {
        m_parts |= TrackCommonFlags::TSOS_TrackParsErrorsPerigee;
      }
      else {
        m_parts |= TrackCommonFlags::TSOS_TrackParsErrorsNotPerigee;
      }
    }
    surf = &(m_tsos->trackParameters()->associatedSurface());
  }
  
  if (measurement()) {
    m_parts |= TrackCommonFlags::TSOS_MeasError;
    if (rioOnTrack()) {
      if (m_tsos->type(Trk::TrackStateOnSurface::Outlier))
        m_parts |= TrackCommonFlags::TSOS_MeasRioOnTrackOutlier;
      else
        m_parts |= TrackCommonFlags::TSOS_MeasRioOnTrackNotOutlier;
    } else if (competingRIOsOnTrack()) {
      if (m_tsos->type(Trk::TrackStateOnSurface::Outlier))
        m_parts |= TrackCommonFlags::TSOS_MeasCompetingRioOnTrackOutlier;
      else
        m_parts |= TrackCommonFlags::TSOS_MeasCompetingRioOnTrackNotOutlier;
    } else
      VP1Msg::messageDebug("Warning: Ignoring measurement which is not a (competing) rio on track.");
    if (!surf) surf = &(measurement()->associatedSurface());
  }
  const Trk::MaterialEffectsBase* matEff = m_tsos->materialEffectsOnTrack();
  if (matEff) {
    const Trk::MaterialEffectsOnTrack * meot = dynamic_cast<const Trk::MaterialEffectsOnTrack * >(matEff);
    if (meot) {
      if (meot->energyLoss()&&meot->energyLoss()->deltaE()!=0)
        m_parts |= TrackCommonFlags::TSOS_MaterialEffects;
      else
        m_parts |= TrackCommonFlags::TSOS_MaterialEffectsWithNoDeltaE;
    } else {
      if (dynamic_cast<const Trk::EstimatedBremOnTrack * >(matEff))
        VP1Msg::messageDebug("Warning: Ignoring EstimatedBremOnTrack.");
      else
        VP1Msg::messageDebug("Warning: Ignoring material effect which is not MaterialEffectsOnTrack.");
    }
    if (!surf)
      surf = &(matEff->associatedSurface());
  }
  if (surf) {
    if (surf->associatedDetectorElement())
      m_parts |= TrackCommonFlags::TSOS_SurfacesDetElem;
    else
      m_parts |= TrackCommonFlags::TSOS_SurfacesCustom;
    
  }
}

//____________________________________________________________________
void AscObj_TSOS::setDistToNextPar(const double& d)
{
  //We assume this is called right after the constructor - so no need
  //to update 3D objects.
  m_distToNextPar = d;
}

//____________________________________________________________________
Amg::Vector3D AscObj_TSOS::approxCenter() const
{
  if (hasParameters())
    return m_tsos->trackParameters()->position();
  if (hasSurface())
    return surface()->center();
  VP1Msg::message("AscObj_TSOS::approxCenter() WARNING: Failed to determine"
          " position from either params or surface");
  return Amg::Vector3D(0,0,0);
}

//____________________________________________________________________
int AscObj_TSOS::regionIndex() const
{
	Amg::Vector3D c = approxCenter();
  static const double l=30.0*CLHEP::cm;
  return       static_cast<int>(c.z()/l)
         +1000*static_cast<int>(c.y()/l)
      +1000000*static_cast<int>(c.x()/l);
}

//____________________________________________________________________
const Trk::MeasurementBase * AscObj_TSOS::measurement() const
{
  return m_tsos->measurementOnTrack();
}

//____________________________________________________________________
SoTranslation* AscObj_TSOS::getZTranslationTube( const Trk::Surface * theSurface,
                         const double& maxTrans ) const
{
  const Amg::Vector3D* origo = theSurface->localToGlobal(Amg::Vector2D(0,0));
  const Amg::Vector3D* unitz = theSurface->localToGlobal(Amg::Vector2D(0,1));

  std::vector< Amg::Vector3D > * points = trackHandle()->hackGetPointsPropagated();//FIXME

  double min = std::numeric_limits<double>::max();
  double smin=0;
  size_t imin=0;
  //First we find the point in PointsPropageted closest to the line.
  for ( size_t i = 0; i<points->size(); i++)
  {
    double s;
    double dist = VP1LinAlgUtils::distPointLine2( (*points)[i], *origo, *unitz, s );
    if (dist < min)
    {
      min = dist;
      imin = i;
    }
  }

  if (points->size()>1 ){
    // FIXME - don't think we should ever really need this test. EJWM.
    
    //Then we get the parameters on the forward and backward lines giving the points with
    //the shortest distances.
    //We then use the line which has the parameter closest to 0.5 which means that it
    //is inside the part of the line between this point and the next/previous
    double sp,tp,sm,tm;
    if (imin+1 < points->size() && imin >= 1)
    {
      VP1LinAlgUtils::distLineLineParam((*points).at(imin),(*points).at(imin+1),*origo,*unitz,tp,sp);
      VP1LinAlgUtils::distLineLineParam((*points).at(imin-1),(*points).at(imin),*origo,*unitz,tm,sm);
      smin = fabs(tm - 0.5) < fabs(tp - 0.5) ? sm : sp;
    } else if (imin+1 >= points->size() && imin >= 1 )
    {
      VP1LinAlgUtils::distLineLineParam((*points).at(imin-1),(*points).at(imin),*origo,*unitz,tm,sm);
      smin = sm;
    } else
    {
      VP1LinAlgUtils::distLineLineParam((*points).at(imin),(*points).at(imin+1),*origo,*unitz,tp,sp);
      smin = sp;
    }
  } else {
    // probably something is wrong!
    VP1Msg::message("AscObj_TSOS::getZTranslationTube: <2 points returned by trackHandle!");
  }

  //Ensure that we do not get out of bounds, preserve sign of translation.
  if (fabs(smin) > maxTrans)
  {
    double sign = smin > 0 ? 1.0 : -1.0;
    smin = sign*maxTrans;
  }

  SoTranslation * trans = new SoTranslation;
  trans->translation.setValue(0,0,smin);

  return trans;
}

double AscObj_TSOS::deviationFromMeasurement(const bool& absolute)
{
  double rioValue = 0;
  double paramValue = 0;
  double sigma = 1;

  const Trk::RIO_OnTrack* rio = rioOnTrack();
  if (!rio && competingRIOsOnTrack()!=0 ) {
      // must be crot
      rio =  &(competingRIOsOnTrack()->rioOnTrack(competingRIOsOnTrack()->indexOfMaxAssignProb () ));
  } else {
      VP1Msg::message(" No ROT or cROT in AscObj_TSOS::deviationFromMeasurement(const bool& absolute)!");
      return 0.0;
  }
  Identifier id = rio->identify();
  const AtlasDetectorID *  idhelper = VP1DetInfo::atlasIDHelper();
  //FIXME: should we check for idhelper before?
  const bool isTRT = idhelper && idhelper->is_indet(id) && idhelper->is_trt(id);
  const bool isMDT = idhelper && idhelper->is_muon(id)  && idhelper->is_mdt(id);
  if ( idhelper && ( isTRT||isMDT ) )
  {
    //Value from the measurement
    rioValue = fabs(rio->localParameters().get(Trk::driftRadius));

    //Value from the track
    const Trk::TrackParameters* trackParams = m_tsos->trackParameters();
    const Trk::TrackParameters * meas = dynamic_cast<const Trk::TrackParameters *>(trackParams);
    const Trk::AtaStraightLine * atas = dynamic_cast<const Trk::AtaStraightLine *>(meas);
    if (not atas) return std::nan("");
    const Amg::Vector2D& localposMeas = atas->localPosition();
    paramValue = fabs(localposMeas[Trk::locR]);

    if (!absolute)
    {
      const AmgSymMatrix(5) covmat = *(meas->covariance());
      sigma = sqrt( covmat(0,0) );
    }
  }

  return absolute ? fabs( rioValue - paramValue ) : fabs( rioValue - paramValue )/sigma;
}

void AscObj_TSOS::addDeviationFromMeasurementInfoToShapes( SoSeparator*&shape_simple, SoSeparator*&shape_detailed)
{
  const Trk::RIO_OnTrack* rio = rioOnTrack();
  Identifier id = rio->identify();
  const AtlasDetectorID *  idhelper = VP1DetInfo::atlasIDHelper();
  //FIXME: should we check for idhelper before?
  const bool isTRT = idhelper && idhelper->is_indet(id) && idhelper->is_trt(id);
  const bool isMDT = idhelper && idhelper->is_muon(id) && idhelper->is_mdt(id);
  if ( idhelper && ( isTRT||isMDT ) )
  {
    const Trk::Surface& theSurface = rio->associatedSurface();
    const Amg::Vector3D* origo = theSurface.localToGlobal(Amg::Vector2D(0,0));
    const Amg::Vector3D* unitz = theSurface.localToGlobal(Amg::Vector2D(0,1));
    const Amg::Vector3D& point = m_tsos->trackParameters()->position(); //FIXME: use the one from rio when it is fixed.
    //const Amg::Vector3D& point = rio->globalPosition();
    //this is currently returning a position on the z-axis which is wrong. Re-enable this feature
    //when this is fixed

    //Get the point 'pointMeas' on the measurement z-axis which is closest to the point 'point'
    double s;
    VP1LinAlgUtils::distPointLineParam( point, *origo, *unitz, s );
    const Amg::Vector3D pointMeas = *origo + s*(*unitz - *origo);
    delete origo;
    delete unitz;

    SoLineSet * line = new SoLineSet();
    SoVertexProperty * vertices = new SoVertexProperty();
    vertices->vertex.set1Value(0,point.x(),point.y(),point.z());
    vertices->vertex.set1Value(1,pointMeas.x(),pointMeas.y(),pointMeas.z());
    line->numVertices = 2;
    line->vertexProperty = vertices;

    //colour according to side() (left=red, right=green)
    if (isTRT)
    {
      const InDet::TRT_DriftCircleOnTrack * dcot = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>(rio);
      if (not dcot) return;
      if (dcot->side()!=Trk::NONE)
      {
        SoSeparator * sep = new SoSeparator;
        SoMaterial * mat = new SoMaterial;
        if (dcot->side()==Trk::LEFT)
          mat->diffuseColor.setValue(1,0,0);
        else //(dcot->side()==Trk::RIGHT)
          mat->diffuseColor.setValue(0,1,0);
        sep->addChild(mat);
        sep->addChild(line);
        shape_detailed->addChild(sep);
        return;
      }
    } else if (isMDT){
      const Muon::MdtDriftCircleOnTrack * dcot = dynamic_cast<const Muon::MdtDriftCircleOnTrack *>(rio);
      if (not dcot) return;
      if (dcot->side()!=Trk::NONE)
      {
        SoSeparator * sep = new SoSeparator;
        SoMaterial * mat = new SoMaterial;
        if (dcot->side()==Trk::LEFT)
          mat->diffuseColor.setValue(1,0,0);
        else //(dcot->side()==Trk::RIGHT)
          mat->diffuseColor.setValue(0,1,0);
        sep->addChild(mat);
        sep->addChild(line);
        shape_detailed->addChild(sep);
        return;
      }
    }

    ensureInitSeps(shape_simple,shape_detailed);
    shape_detailed->addChild(line);
  }
}

//____________________________________________________________________
void AscObj_TSOS::addTrackParamInfoToShapes( SoSeparator*&shape_simple,
                         SoSeparator*&shape_detailed,
                         bool showPars, bool showParsErrors, bool showSurfaces)
{
  const Trk::TrackParameters* trackParams = m_tsos->trackParameters();
  Q_ASSERT(trackParams);
  ensureInitSeps(shape_simple,shape_detailed);

  Amg::Vector3D p1(trackParams->position());//point

  if (showPars) {

//	  Trk::GlobalMomentum u(trackParams->momentum().unit());//mom direction
	  Amg::Vector3D u(trackParams->momentum().unit());//mom direction

    double length = 5*CLHEP::cm;
    if (m_distToNextPar>0)
      length = std::min(m_distToNextPar*0.75,length);
    Amg::Vector3D p2 = p1+length*u;

    //Line:
    SoLineSet * line = new SoLineSet();
    SoVertexProperty * vertices = new SoVertexProperty();
    vertices->vertex.set1Value(0,p1.x(),p1.y(),p1.z());
    vertices->vertex.set1Value(1,p2.x(),p2.y(),p2.z());
    line->numVertices.set1Value(0,2);

    //Point:
    SoPointSet * points = new SoPointSet;
    SoVertexProperty * vertices2 = new SoVertexProperty;
    vertices2->vertex.set1Value(0,p1.x(),p1.y(),p1.z());
    points->numPoints=1;
    line->vertexProperty = vertices;
    points->vertexProperty = vertices2;

    bool isHole= m_parts & TrackCommonFlags::TSOS_Hole;
    if (trackHandle()->customColouredTSOSParts()&TrackCommonFlags::TSOS_TrackPars 
      || (isHole&&(trackHandle()->customColouredTSOSParts() & TrackCommonFlags::TSOS_Hole) ) ) {
      SoMaterial * mat = isHole?common()->controller()->customMatHoleParameters():common()->controller()->customMatParameters();
      SoSeparator * sep = new SoSeparator;
      sep->addChild(mat);
      sep->addChild(line);
      sep->addChild(points);
      shape_simple->addChild(sep);
      shape_detailed->addChild(sep);
    } else {
      shape_simple->addChild(line);
      shape_simple->addChild(points);
      shape_detailed->addChild(line);
      shape_detailed->addChild(points);
    }
  }

  if ( showParsErrors ){    
    SoTransform* theHitTransform = VP1LinAlgUtils::toSoTransform(surface()->transform()); // FIXME - remove if working in local coords
    shape_detailed->addChild(theHitTransform);
    // Need to cast in order to get error Matrix
    if (trackParams->covariance()) // Only draw error if point is a measurement
    {
      // fixme! This is really ugly - can't be cast to a base class? MeasuredTrackParameters doesn't know LP.
      const Trk::AtaStraightLine * atas = dynamic_cast<const Trk::AtaStraightLine *>(trackParams);
      const Trk::Perigee * per = dynamic_cast<const Trk::Perigee *>(trackParams);
      const Trk::AtaPlane * aap = dynamic_cast<const Trk::AtaPlane *>(trackParams);
      const Trk::AtaDisc * aad = dynamic_cast<const Trk::AtaDisc *>(trackParams);
      if (per||atas||aap||aad){
        const Amg::Vector2D& localPos = atas ? atas->localPosition() : (per ? per->localPosition() : (aap ? aap->localPosition() : aad->localPosition()));
//        addErrors(*(trackParams->associatedSurface()), meas->localErrorMatrix().covariance(), localPos, p1, showSurfaces, shape_simple, shape_detailed, false, false); //FIXME - last parameter false when working in correct local coords
        addErrors(trackParams->associatedSurface(), *trackParams->covariance(), localPos, p1, showSurfaces, shape_simple, shape_detailed, false, false); //FIXME - last parameter false when working in correct local coords
      } else {
        VP1Msg::messageVerbose("Not a supported parameter for errors: ");
        // std::cout<<(*trackParams)<<std::endl;
      }
    } 
  }
}

void AscObj_TSOS::addErrors(const Trk::Surface& theSurface, const AmgSymMatrix(5)& tmpCovMat,
    const Amg::Vector2D& localPos, const Amg::Vector3D& p1, bool showSurfaces, SoSeparator* shape_simple, SoSeparator* shape_detailed, bool force1D, bool applyLocalTrans )
{
  VP1Msg::messageVerbose("addErrors");  
  if (applyLocalTrans) VP1Msg::messageVerbose("applyLocalTrans");  
  
      //Fixme: get from collhandle:!!
  const bool settingsDRAWCYLINDER = common()->controller()->parTubeErrorsDrawCylinders();
  const bool settingsDRAWZERRCIRCLES = settingsDRAWCYLINDER;
  const double settingsSIGMASCALE = common()->controller()->nStdDevForParamErrors();

  SoSeparator * errSimple = new SoSeparator;
  SoSeparator * errDetailed = new SoSeparator;

    //Possibly a custom colour
  if (trackHandle()->customColouredTSOSParts()&TrackCommonFlags::TSOS_AnyParsErrors) {
    // FIXME - what about meas errors?
    SoMaterial * mat = common()->controller()->customMatParameterErrors();
    errSimple->addChild( mat );
    errDetailed->addChild( mat );
  }
  // SoTransparency * transparent = new SoTransparency;
  // transparent->transparency.setValue(0.5);
  // errSimple->addChild( transparent );
  // errDetailed->addChild( transparent );
  
  const int npoints = common()->controller()->numberOfPointsOnCircles();
      // Check for type of associated surface
      // const Trk::Surface * theSurface = trackParams->associatedSurface();
  const Trk::CylinderSurface     * cylSurf = dynamic_cast<const Trk::CylinderSurface *>(&theSurface);
  const Trk::DiscSurface         * disSurf = dynamic_cast<const Trk::DiscSurface *>(&theSurface);
  const Trk::PerigeeSurface      * perSurf = dynamic_cast<const Trk::PerigeeSurface *>(&theSurface);
  const Trk::PlaneSurface        * plaSurf = dynamic_cast<const Trk::PlaneSurface *>(&theSurface);
  const Trk::StraightLineSurface * strSurf = dynamic_cast<const Trk::StraightLineSurface *>(&theSurface);
  if ( plaSurf ) /* Plane Surface */
  {  
    // Shift from Surface centre to correct position
    if (applyLocalTrans) {
      SoTranslation * theTransform = new SoTranslation;
      const Amg::Vector2D* locPosTmp = surface()->globalToLocal(p1);
      if (locPosTmp) {
        theTransform->translation.setValue(locPosTmp->x(),locPosTmp->y(),0.0); 
        // std::cout<<"applyLocalTrans & Offset=("<<locPosTmp->x()<<","<<locPosTmp->y()<<std::endl;
        errSimple->addChild(theTransform);
        errDetailed->addChild(theTransform);
        delete locPosTmp;
      } else {
        VP1Msg::message("AscObj_TSOS::addErrors - failed to get tmp position");  
      }
    }
    
    VP1Msg::messageVerbose("Plane surf error of size:"+QString::number(tmpCovMat.rows()));
    if (force1D) VP1Msg::messageVerbose("Forcing 1D");  
    VP1ErrorUtils::errorAtPlaneSurface(errSimple, errDetailed, tmpCovMat, &theSurface, p1, settingsSIGMASCALE, npoints, showSurfaces, force1D, false );
    ensureInitSeps(shape_simple,shape_detailed);
    shape_simple->addChild( errSimple );
    shape_detailed->addChild( errDetailed );
  } else if ( strSurf || perSurf) {
  /* Straight Line Surface or perigee surface*/
    // Shift from Surface centre to correct position
    if (applyLocalTrans){
      SoTranslation * theTransform = getZTranslationTube(&theSurface,10000.0);
      errSimple->addChild(theTransform);
      errDetailed->addChild(theTransform);
    }
    VP1ErrorUtils::errorAtStraightLineSurface(errSimple, errDetailed, tmpCovMat, localPos, &theSurface, settingsSIGMASCALE,  settingsDRAWZERRCIRCLES, settingsDRAWCYLINDER,  npoints, force1D, false);
    ensureInitSeps(shape_simple,shape_detailed);
    shape_simple->addChild(   errSimple );
    shape_detailed->addChild( errDetailed );
  } else if ( cylSurf ) {
    VP1Msg::message("WARNING: Visualisation of errors on Cylinder Surface not implemented");
  } else if ( disSurf ) {
    VP1Msg::message("WARNING: Visualisation of errors on Disc Surface not implemented");
  }  
  VP1Msg::messageVerbose("leaving addErrors");  
  
  return;
}

//____________________________________________________________________
void AscObj_TSOS::addSurfaceToShapes( SoSeparator*&shape_simple, SoSeparator*&shape_detailed)
{
  // static int ignoreTubes=-1;
  // if (ignoreTubes==-1)
  //   ignoreTubes = VP1QtUtils::environmentVariableIsOn("VP1_TRKSYS_NEVERSHOWTUBESURFACES") ? 1 : 0;
  static int ignoreECModules=-1;
  if (ignoreECModules==-1)
    ignoreECModules = VP1QtUtils::environmentVariableIsOn("VP1_TRKSYS_NEVERSHOW_ENDCAPMODULES") ? 1 : 0;

  if (common()->controller()->hideTubeSurfaces() &&dynamic_cast<const Trk::StraightLineSurface*>(surface()))
    return;

  if (ignoreECModules) {
    const Trk::PlaneSurface* ps = dynamic_cast<const Trk::PlaneSurface*>(surface());
    if (ps) {
      Amg::Vector3D z(0.0,0.0,1.0);
      double angle_z_normal = Amg::angle(z, ps->normal());
      double abscostheta = fabs(cos(angle_z_normal));
      if (abscostheta>0.707) return;
    }
  }

  SurfaceToSoNode surfCnv;//fixme: check if need in common()
  SoNode* theSurfSepSimple = surfCnv.translateSurface(*(surface()),true);
  SoNode* theSurfSep = surfCnv.translateSurface(*(surface()));
  if (theSurfSep) {
    SoNode * nodeToAddSimple = theSurfSepSimple;
    SoNode * nodeToAdd = theSurfSep;
    ensureInitSeps(shape_simple,shape_detailed);
    if (trackHandle()->customColouredTSOSParts()&TrackCommonFlags::TSOS_AnySurface) {
      SoMaterial * mat = common()->controller()->customMatSurfaces();
      if (theSurfSep->getTypeId().isDerivedFrom(SoSeparator::getClassTypeId())) {
        static_cast<SoSeparator*>(theSurfSepSimple)->insertChild(mat,0);
        static_cast<SoSeparator*>(theSurfSep)->insertChild(mat,0);
      } else {
        SoSeparator * sepSimple = new SoSeparator;
        sepSimple->addChild(mat);
        sepSimple->addChild(theSurfSepSimple);
        nodeToAddSimple = sepSimple;
        SoSeparator * sep = new SoSeparator;
        sep->addChild(mat);
        sep->addChild(theSurfSep);
        nodeToAdd = sep;
      }
    }
//    shape_simple->addChild(nodeToAdd); //todo: check this. I changed nodeToAdd to nodeToAddSimple
    shape_simple->addChild(nodeToAddSimple);
    shape_detailed->addChild(nodeToAdd);
  } else {
    VP1Msg::message("AscObj_TSOS::addSurfaceToShapes WARNING: Got null shape.");
  }
}

//____________________________________________________________________
void AscObj_TSOS::addMaterialEffectsToShapes( SoSeparator*&shape_simple, SoSeparator*&shape_detailed)
{
  const double settingsForceOnTrack = false;

  ensureInitSeps(shape_simple,shape_detailed);
  SoSeparator * matSepSimple = new SoSeparator;
  SoSeparator * matSepDetailed = new SoSeparator;

  //1) Transformation.

  if (m_tsos->trackParameters()) {
    //Put on position of track parameter
    Amg::Vector3D p1(m_tsos->trackParameters()->position());//point
    SoTranslation * theTransform = new SoTranslation;
    theTransform->translation.setValue(p1.x(),p1.y(),p1.z());
    matSepSimple->addChild(theTransform);
    matSepDetailed->addChild(theTransform);
  } else {
    const Trk::Surface * surf = surface();
    if (!surf) {
      VP1Msg::message("AscObj_TSOS::addMaterialEffectsToShapes WARNING: Has neither surface or parameter to get position!");
    } else {
      if (settingsForceOnTrack) {
        VP1Msg::message("AscObj_TSOS::addMaterialEffectsToShapes WARNING: settingsForceOnTrack not implemented");
        
    //Take position from surface and somehow constrain it onto the line.
    //(Fixme... same kind of treatment as in the "short tubes"... how?).
      } else {
    //Take center of surface if present.
    SoTransform * theTransform = VP1LinAlgUtils::toSoTransform(surf->transform());
    matSepSimple->addChild(theTransform);
    matSepDetailed->addChild(theTransform);
      }
    }
  }
  //2) Possibly a custom colour

  if (trackHandle()->customColouredTSOSParts()&TrackCommonFlags::TSOS_AnyMaterialEffects) {
    SoMaterial * mat = common()->controller()->customMatMaterialEffects();
    matSepSimple->addChild(mat);
    matSepDetailed->addChild(mat);
  }

  //3) The shape

  SoPointSet * point = common()->singlePoint();
  matSepSimple->addChild(point);

  if (m_parts & TrackCommonFlags::TSOS_MaterialEffectsWithNoDeltaE) {
    matSepDetailed->addChild(point);
  } else {
    const Trk::MaterialEffectsBase* matEff = m_tsos->materialEffectsOnTrack();
    const Trk::MaterialEffectsOnTrack* matEffOnTrk = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(matEff);
    if (matEffOnTrk){
      const double absDeltaE = fabs(matEffOnTrk->energyLoss()->deltaE());
      const double radius(absDeltaE > 1*CLHEP::eV ? 5.0*exp(log(absDeltaE/CLHEP::MeV)/3.0) : 0);//\propto cube root
      //TK: radius used to be: 5.0*sqrt(absDE), but we want sphere volume \propto deltaE
      const double scale = common()->controller()->materialEffectsOnTrackScale();
      if (scale * radius > 0.1) {
        SoSphere * meotSphere = new SoSphere;
        meotSphere->radius.setValue( scale * radius  );
        matSepDetailed->addChild(meotSphere);
      } else {
        matSepDetailed->addChild(point);
      }
    }
  }

  //Attach:
  shape_simple->addChild(matSepSimple);
  shape_detailed->addChild(matSepDetailed);
}

//____________________________________________________________________
void AscObj_TSOS::addRIO_OnTrackInfoToShapes( SoSeparator*&shape_simple,
                          SoSeparator*&shape_detailed,
                const Trk::RIO_OnTrack* rio, bool blockDrawingOfGP)
{
  // VP1Msg::messageVerbose("AscObj_TSOS::addRIO_OnTrackInfoToShapes() start");
  
  if (!rio) { return;} 
//  Identifier id = rio->identify();

  ensureInitSeps(shape_simple,shape_detailed);

  TrackCommonFlags::TSOSPartsFlags f(trackHandle()->shownTSOSParts()&m_parts);
  const bool showMeas(f & TrackCommonFlags::TSOS_AnyMeasurement);
  const bool showMeasErrors(f & TrackCommonFlags::TSOS_MeasError);

  TrkObjToString::MeasurementType type = m_objToType.type(rio);
  // std::cout<<">>>>>>>>>>>     addRIO_OnTrackInfoToShapes: Have measurement of type: "<<static_cast<unsigned int>(type)<<" & showRioOnTrack="<<showMeas<<std::endl;
  if (showMeas){
    // std::pair<SoSeparator*,  std::vector<SoNode*> > detailed_nodes, simple_nodes; 
    // handle colouring
      // VP1Msg::message(m_objToType.name(*rio));

    if (m_parts & trackHandle()->customColouredTSOSParts()&TrackCommonFlags::TSOS_AnyMeasRioOnTrack) {
      SoMaterial * mat;
      if (m_parts & trackHandle()->customColouredTSOSParts()&TrackCommonFlags::TSOS_MeasRioOnTrackNotOutlier)
        mat = common()->controller()->customMatMeasurements();
      else
        mat = common()->controller()->customMatMeasurementsOutliers();
      shape_simple->addChild(mat);
      shape_detailed->addChild(mat);
    }

    bool isStrip = (type==TrkObjToString::RPC || type==TrkObjToString::TGC || type==TrkObjToString::CSC || type==TrkObjToString::SCT|| type==TrkObjToString::Pixel);
    bool isDC = (type==TrkObjToString::TRT || type==TrkObjToString::MDT);
    if (isStrip)
      m_hitToSoNode.buildStripShapes(*rio,shape_simple,shape_detailed,blockDrawingOfGP); // transforms to localPos (i.e. assumes in surface frame)
    else {
      if (isDC) {
        float zShift=0.0;
        if (isShortMeasurement()){
          SoTranslation * theTransform = getZTranslationTube(&(rio->associatedSurface ()),10000.0);
          float x,y; // dummy
          theTransform->translation.getValue().getValue(x,y,zShift);
          shape_simple->addChild(theTransform);
          shape_detailed->addChild(theTransform);
        }
        m_hitToSoNode.buildTubeShapes(*rio,shape_simple,shape_detailed,blockDrawingOfGP,isShortMeasurement()?common()->controller()->measurementsShorttubesScale():0.0, false);
        if (isShortMeasurement()){
          SoTranslation * theTransform = new SoTranslation;
          theTransform->translation.setValue(0,0,-zShift);
          shape_simple->addChild(theTransform);
          shape_detailed->addChild(theTransform);
        }
      }
    }
  }

//Errors
  if (showMeasErrors){
    const Trk::LocalParameters & locParams = rio->localParameters () ;
//    VP1Msg::message( "Trying to draw measurement errors for ROT with id="+QString::number(rio->identify().get_compact())+" locparams have dim="+QString::number(locParams.dimension())+", locellmat="+QString::number(rio->localErrorMatrix().covariance().num_row()));
    VP1Msg::message( "Trying to draw measurement errors for ROT with id="+QString::number(rio->identify().get_compact())+" locparams have dim="+QString::number(locParams.dimension())+", locellmat="+QString::number(rio->localCovariance().rows()));

    Amg::Vector2D  localPos;
    /*
     * see:
     * Tracking/TrkEvent/TrkEventPrimitives/trunk/TrkEventPrimitives/LocalParameters.h
     * Tracking/TrkEvent/TrkEventPrimitives/trunk/src/LocalParameters.cxx
     */
    if (locParams.contains(Trk::loc1)) {
    	localPos( locParams.get(Trk::loc1), locParams.contains(Trk::loc2) ? locParams.get(Trk::loc2) : 0 );
    } else{
      VP1Msg::message( "WARNING: locParams.contains(Trk::loc1) is FALSE!!");
    }

    bool force1D=false;
    if (locParams.dimension ()!=rio->localCovariance().rows()){
      VP1Msg::message( "WARNING: ROT has inconsistent dimensions between cov matrix and local parameters. Forcing to 1D.");
      force1D=true;
    }
    force1D=true; //FIXME!
    
    // last parameter sets whether we do a local shift. 
    addErrors(rio->associatedSurface(), rio->localCovariance(), localPos, rio->globalPosition(), false,
              shape_simple, shape_detailed, force1D, true);
    VP1Msg::message( "Done with measurement errors");
  }
}

//____________________________________________________________________
void AscObj_TSOS::addCompetingRIO_OnTrackInfoToShapes( SoSeparator*&shape_simple,
                          SoSeparator*&shape_detailed)
{
  // std::cout<<"addCompetingRIO_OnTrackInfoToShapes"<<std::endl;
  const Trk::CompetingRIOsOnTrack * crio = competingRIOsOnTrack();

  ensureInitSeps(shape_simple,shape_detailed);

  //Possibly a custom colour
  if (m_parts & trackHandle()->customColouredTSOSParts()&TrackCommonFlags::TSOS_MeasCompetingRioOnTrackNotOutlier) {
    SoMaterial * mat = common()->controller()->customMatMeasurements();
    shape_simple->addChild(mat);
    shape_detailed->addChild(mat);
  } else if (m_parts & trackHandle()->customColouredTSOSParts()&TrackCommonFlags::TSOS_MeasCompetingRioOnTrackOutlier) {
    SoMaterial * mat = common()->controller()->customMatMeasurementsOutliers();
    shape_simple->addChild(mat);
    shape_detailed->addChild(mat);
  } 

  if (common()->controller()->drawMeasGlobalPositions()){
    // std::cout<<"cROT - showing globalpositions."<<std::endl;
    
    // if (surface()!=crio->rioOnTrack(crio->indexOfMaxAssignProb ())->associatedSurface())
    //   VP1Msg::message("AscObj_TSOS::addCompetingRIO_OnTrackInfoToShapes WARNING: cROT surface doesn't match one from surface()") ;
//Draw cross marking global position of the rio on track
    SoTranslation * theTransform = new SoTranslation;
    Amg::Vector3D offset = crio->globalPosition() - crio->associatedSurface().center();
    // std::cout<<"globalPOS=("<<crio->globalPosition().x()<<","<<crio->globalPosition().y()<<","<<crio->globalPosition().z()<<")"<<std::endl;
    // std::cout<<"associatedSurf centre =("<<crio->associatedSurface().center().x()<<","<<crio->associatedSurface().center().y()<<","<<crio->associatedSurface().center().z()<<")"<<std::endl;
    // std::cout<<"offset=("<<offset.x()<<","<<offset.y()<<","<<offset.z()<<")"<<std::endl;
    // theTransform->translation.setValue( offset.x(), offset.y(), offset.z());
    
    
    Amg::Vector3D locPos = (crio->associatedSurface().transform().rotation().inverse())*(offset);
    theTransform->translation.setValue( locPos.x(), locPos.y(), locPos.z());
    // std::cout<<"the GP Transform=("<<locPos.x()<<","<<locPos.y()<<","<<locPos.z()<<")"<<std::endl;
    
    // double xoff= crio->localParameters().get(Trk::loc1);
    // double yoff= crio->localParameters().get(Trk::loc2);
    // double zoff= globalpos.z()-surface().center().z();
    // theTransform->translation.setValue(xoff,yoff,0.0);
    // SoTranslation * theTransformBack = new SoTranslation;
    // theTransformBack->translation.setValue(-locPos.x(), -locPos.y(), -locPos.z());
    // std::cout<<"theTransformBack="<<Amg::toString(*theTransformBack)<<std::endl;
    
    SoSeparator * gpSep = new SoSeparator;
    
    gpSep->addChild(theTransform); 
    gpSep->addChild(common()->nodeManager().getShapeNode_Cross(50)); 
    shape_detailed->addChild(gpSep);   
  }

  //Draw all contained rio on tracks
  TrackCommonFlags::TSOSPartsFlags f(trackHandle()->shownTSOSParts()&m_parts);
  const bool showcRioOnTrack(f & TrackCommonFlags::TSOS_AnyMeasurement);
  
  if (showcRioOnTrack){
    
    // FIXME - shouldn't need to use so many seps, but having problems with transformations.
    SoSeparator * rotSepSimple = new SoSeparator;
    SoSeparator * rotSepDetailed = new SoSeparator;
    
    // std::cout<<"addCompetingRIO_OnTrackInfoToShapes: About to draw most probable ROT"<<std::endl;
    unsigned int maxProb = crio->indexOfMaxAssignProb ();
    // std::cout<<"Drawing maxProb ["<<maxProb<<"] strip "<<std::endl;
    addRIO_OnTrackInfoToShapes(rotSepSimple,rotSepDetailed, &(crio->rioOnTrack(maxProb)), true);
    shape_detailed->addChild(rotSepDetailed); 
    shape_simple->addChild(rotSepSimple); 
    
    // std::cout<<"addCompetingRIO_OnTrackInfoToShapes: Done drawing most probable"<<std::endl;

    SoTransparency * transparent = new SoTransparency;
    transparent->transparency.setValue(0.5);
    shape_detailed->addChild( transparent );

    const Trk::RIO_OnTrack* rot = &(crio->rioOnTrack(maxProb));
    //if (rot==0 || rot == NULL) { VP1Msg::message("WARNING: cROT max prob ROT is NULL. Aborting."); return;}
    if (!rot) { VP1Msg::message("WARNING: cROT has empty max prob ROT. Aborting."); return;}
    // Now, last position drawn was that of strip pos for most probable ROT. So take this as basis.
    if (!rot->detectorElement()) { VP1Msg::message("WARNING: most prob ROT from cROT has empty DE. Aborting."); return;}
    
    const MuonGM::TgcReadoutElement* tgcDetEl =
      dynamic_cast<const MuonGM::TgcReadoutElement*>(rot->detectorElement());
    const MuonGM::RpcReadoutElement* rpcDetEl =
      dynamic_cast<const MuonGM::RpcReadoutElement*>(rot->detectorElement());
    
    // FIXME - could do all this a bit more elegantly with templates
    if ( (!tgcDetEl) && (!rpcDetEl)) { VP1Msg::message("WARNING: most prob ROT from cROT has Unknown DE type. Aborting."); std::cout<<*rot<<std::endl;return;}
    
    // Amg::Vector3D lastPos,currPos;
    // if (tgcDetEl){
    //   lastPos = tgcDetEl->stripPos(rot->identify());
    // } else {
    //   lastPos = rpcDetEl->stripPos(rot->identify());
    // }
    
    const Trk::Surface* lastSurf=&(crio->rioOnTrack(maxProb).associatedSurface());
    
    // std::cout<<"lastSurf = ("<<*lastSurf<<") for most prob="<<rot->identify().get_compact()<<std::endl;
    unsigned int nrio = crio->numberOfContainedROTs();
    for (unsigned int n = 0; n < nrio; n++)
    {
      if (n==maxProb) continue; // already drawn.
      // std::cout<<"Strip #"<<n<<std::endl;
      const Trk::RIO_OnTrack* rot = &(crio->rioOnTrack(n));
      
      // SoTranslation * theTransform = new SoTranslation;
      if (!rot) { VP1Msg::message("WARNING: cROT has empty ROT. Skipping."); continue;}
      
      // Now do offset from last strip pos, to new one (for strips on different layers, this will include a zoffset)
      // const MuonGM::TgcReadoutElement* tgcDetEl =
      //         dynamic_cast<const MuonGM::TgcReadoutElement*>(rot->detectorElement());
      //       const MuonGM::RpcReadoutElement* rpcDetEl =
      //         dynamic_cast<const MuonGM::RpcReadoutElement*>(rot->detectorElement());
      //       
      //     // FIXME - could do all this a bit more elegantly with templates
      // 
      //       if ( (!tgcDetEl) && (!rpcDetEl)) { VP1Msg::message("WARNING: ROT from cROT has Unknown DE type. Skipping."); std::cout<<*rot<<std::endl; continue;}
      // 
      //       if (tgcDetEl){
      //         currPos = tgcDetEl->stripPos(rot->identify());
      //       } else {
      //         currPos = rpcDetEl->stripPos(rot->identify());
      //       }
      //       Amg::Vector3D offset = currPos - lastPos;
      //       Amg::Vector3D<double> locPos = (rot->associatedSurface().transform().inverse())*(offset);
      //       theTransform->translation.setValue( locPos.x(), locPos.y(), locPos.z());
      //       lastPos=currPos;
      SoSeparator * rotSepSimple = new SoSeparator;
      SoSeparator * rotSepDetailed = new SoSeparator;
      
      if ( &(rot->associatedSurface ())!=lastSurf) {
        // std::cout<<"New surface!"<<std::endl;
        // Do transformation to centre of new surface, especially important if there is a zoffset.
        // NB we're assuming that the surfaces are aligned! Might not be entirely safe, but really should be! 
        // (I hate cROTs)
        Amg::Vector3D offset = rot->associatedSurface().center() - lastSurf->center();
        // std::cout<<"new surf offset = "<<Amg::toString(offset)<<std::endl;
        
        // lastSurf = &(rot->associatedSurface());
        Amg::Vector3D locPos = (rot->associatedSurface().transform().rotation().inverse())*(offset);
        // std::cout<<"new surf locPos = "<<Amg::toString(locPos)<<std::endl;
        
        SoTranslation * theTransform = new SoTranslation;
        theTransform->translation.setValue( locPos.x(), locPos.y(), locPos.z());
        rotSepDetailed->addChild(theTransform);      
        rotSepSimple->addChild(theTransform);
      }
      // lastSurf = &(rot->associatedSurface());
      
      // rotSepDetailed->addChild(theTransform);      
      // rotSepSimple->addChild(theTransform);      
      
     
      m_hitToSoNode.buildStripShapes(crio->rioOnTrack(n),rotSepSimple,rotSepDetailed,true);
      shape_simple->addChild(rotSepSimple);
      shape_detailed->addChild(rotSepDetailed);
      // back to centre
      
      // std::cout<<"Rot "<<n<<" Done"<<std::endl;
      
    }
  }
}

//____________________________________________________________________
void AscObj_TSOS::ensureInitSeps( SoSeparator*&shape_simple,
                  SoSeparator*&shape_detailed)
{
  if (!shape_simple)
    shape_simple = new SoSeparator;
  if (!shape_detailed)
    shape_detailed = new SoSeparator;
}

//____________________________________________________________________
const Trk::RIO_OnTrack * AscObj_TSOS::rioOnTrack() const
{
  return dynamic_cast<const Trk::RIO_OnTrack *>(m_tsos->measurementOnTrack());
}

//____________________________________________________________________
const Trk::CompetingRIOsOnTrack * AscObj_TSOS::competingRIOsOnTrack() const
{
  return dynamic_cast<const Trk::CompetingRIOsOnTrack *>(m_tsos->measurementOnTrack());
}

//____________________________________________________________________
const Trk::Surface * AscObj_TSOS::surface() const
{
  const Trk::Surface * surf(0);
  if (m_tsos->trackParameters())
    surf = &(m_tsos->trackParameters()->associatedSurface());
  if (!surf&&m_tsos->measurementOnTrack())
    surf = &(m_tsos->measurementOnTrack()->associatedSurface());
  if (!surf&&m_tsos->materialEffectsOnTrack())
    surf = &(m_tsos->materialEffectsOnTrack()->associatedSurface());
  return surf;
}

void AscObj_TSOS::addTransformToSurface(SoSeparator*& shape_simple,SoSeparator*& shape_detailed){
    // SurfaceToSoNode surfCnv;//fixme: check if need in common()
    // SoNode* theSurfSepSimple = surfCnv.translateSurface(*(surface()),true);
    // SoNode* theSurfSep = surfCnv.translateSurface(*(surface()));
  if (!surface()) {VP1Msg::message( "AscObj_TSOS::addTransformToSurface: WARNING: TSOS without Surface! Unable to get transform!");return;}
  
  // This is all a bit complicated, and can probably be cleaned up. The point is that TGCs need to be handled specially, or we could just use
  // surface(), I think.
  
  TrkObjToString::MeasurementType type=TrkObjToString::Unknown;
  if (surface()->associatedDetectorElement()) type= m_objToType.type(surface()->associatedDetectorElement());
  
//  const Trk::RIO_OnTrack* rio = rioOnTrack() ? rioOnTrack() : competingRIOsOnTrack() ? competingRIOsOnTrack()->rioOnTrack(competingRIOsOnTrack()->indexOfMaxAssignProb()) : 0;
  const Trk::RIO_OnTrack* rio = rioOnTrack() ? rioOnTrack() : competingRIOsOnTrack() ? &(competingRIOsOnTrack()->rioOnTrack(competingRIOsOnTrack()->indexOfMaxAssignProb())) : 0;
  if (type==TrkObjToString::Unknown && rio) type=m_objToType.type(rio);

  SoTransform*   theHitTransform=0;  
  if (rio) {
    TrkObjToString::MeasurementType type=m_objToType.type(rio);
    // std::cout<<"Got ROT of type"<<static_cast<unsigned int>(type)<<std::endl; 
    switch (type) {
      case TrkObjToString::Pixel:
      case TrkObjToString::SCT:
      case TrkObjToString::TRT:
      case TrkObjToString::CSC:
      case TrkObjToString::RPC:
      case TrkObjToString::MDT:
      {
        theHitTransform=m_hitToSoNode.createTransform(*rio, true);
        break;          
      }
      case TrkObjToString::TGC:
      {
        theHitTransform=m_hitToSoNode.createTGC_Transform(*rio, true);
        break;
      }
      default:
      {
        VP1Msg::message( "AscObj_TSOS::addTransformToSurface: WARNING: Unknown type of ROT.");
        // std::cout<<"ROT: "<<*rio<<std::endl;
      }        
    }
  } else {
    if (hasSurface() ) theHitTransform = VP1LinAlgUtils::toSoTransform(surface()->transform());
  }

  if (!theHitTransform) {
    VP1Msg::message( "AscObj_TSOS::addTransformToSurface: WARNING: Unable to build transform");
    return;
  }
  shape_simple->addChild(theHitTransform);
  shape_detailed->addChild(theHitTransform);
}

//____________________________________________________________________
void AscObj_TSOS::buildShapes(SoSeparator*&shape_simple,
                  SoSeparator*&shape_detailed)
{
  VP1Msg::messageVerbose("AscObj_TSOS::buildShapes() start");
  
  TrackCommonFlags::TSOSPartsFlags f(trackHandle()->shownTSOSParts()&m_parts);
  if (f==TrackCommonFlags::TSOS_NoObjects)
    return;
  const bool showPars(f & TrackCommonFlags::TSOS_TrackPars);
  const bool showParsErrors(f & TrackCommonFlags::TSOS_AnyParsErrors);
  const bool showRioOnTrack(f & TrackCommonFlags::TSOS_AnyMeasRioOnTrack);
  const bool showCompetingRioOnTrack(f & TrackCommonFlags::TSOS_AnyMeasCompetingRioOnTrack);
  const bool showSurfaces(f & TrackCommonFlags::TSOS_AnySurface);
  const bool showMaterialEffects(f & TrackCommonFlags::TSOS_AnyMaterialEffects);
  const bool showMeasErrors(f & TrackCommonFlags::TSOS_MeasError);
  
  ensureInitSeps(shape_simple,shape_detailed);
  
  // FIXME - would like to simplify this and use transform returned from surface, but means internal changes below
  SoSeparator* param_simple= new SoSeparator;
  SoSeparator* param_detailed= new SoSeparator;
  if (showPars||showParsErrors)
    addTrackParamInfoToShapes(param_simple,param_detailed,showPars,showParsErrors,showSurfaces);
  shape_simple->addChild(param_simple);
  shape_detailed->addChild(param_detailed);

  if (showMaterialEffects)
    addMaterialEffectsToShapes( shape_simple, shape_detailed);
  
  if (showSurfaces)
    addSurfaceToShapes( shape_simple, shape_detailed);
  
  // Transform to centre of surface. All other seps will (eventually) hang from this. 
  // TGCs are special, in that the position returned is that of the strip, due to rotation
  addTransformToSurface(shape_simple,shape_detailed); 
  
  // Measurements are a bit different and create a sep outside addRIO_OnTrackInfoToShapes, because addRIO_OnTrackInfoToShapes is used
  // by addCompetingRIO_OnTrackInfoToShapes too
  SoSeparator* meas_simple= new SoSeparator;
  SoSeparator* meas_detailed= new SoSeparator;
  if (rioOnTrack() && (showRioOnTrack||showMeasErrors))
    addRIO_OnTrackInfoToShapes(meas_simple,meas_detailed,rioOnTrack(), !(common()->controller()->drawMeasGlobalPositions()));
  if (showCompetingRioOnTrack)
    addCompetingRIO_OnTrackInfoToShapes(meas_simple,meas_detailed); // FIXME - handle cROT errors too.
  shape_simple->addChild(meas_simple);
  shape_detailed->addChild(meas_detailed);
  
  //Currently this doesn't do much as rio->globalPosition() returns a position
  //along the axis!
  /**bool showDeviationsFromMeasurements = false;//Fixme (and see below)
  if ( showDeviationsFromMeasurements
       && hasParameters()
       && rioOnTrack()
       && dynamic_cast<const Trk::TrackParameters *>(m_tsos->trackParameters()) )
    addDeviationFromMeasurementInfoToShapes(shape_simple,shape_detailed);
  **/
  VP1Msg::messageVerbose("AscObj_TSOS::buildShapes() end");
  
}

//____________________________________________________________________
QStringList AscObj_TSOS::clicked()
{
  QStringList l;
  if (!m_tsos) {
    l << "ERROR: This associated object handle has a NULL TSOS pointer!!";
    return l;
  }


  l << "---"<<"Track State On Surface #"+VP1Msg::str(m_indexOfPointOnTrack)+":";

  if (hasParameters())
  {
    l << "----> "+TrkObjToString::name(*(m_tsos->trackParameters()));
    QStringList list = TrkObjToString::fullInfo( *(m_tsos->trackParameters()) );
    for (int i = 0; i < (list.size()-1); ){
      l<< (list.at(i)+QString(" = ")+list.at(i+1) ); 
      i+=2;  
    }
    // l << " -> Position (x,y,z) [CLHEP::mm]: "+VP1Msg::str(m_tsos->trackParameters()->position()/CLHEP::mm);
    // l << " -> Momentum (Px,Py,Pz) [MeV]: "+VP1Msg::str(m_tsos->trackParameters()->momentum()/MeV) + ", |P| [GeV]: "+VP1Msg::str(m_tsos->trackParameters()->momentum().mag()/GeV);
    // l << " -> Momentum (Pt) [GeV]: "+VP1Msg::str(m_tsos->trackParameters()->momentum().perp()/GeV);
    //FIXME: parameter errors
  }

  bool showDeviationsFromMeasurements = true;//Fixme (and see above)
  if ( showDeviationsFromMeasurements
       && hasParameters()
       && (rioOnTrack() || competingRIOsOnTrack() )
       && dynamic_cast<const Trk::TrackParameters *>(m_tsos->trackParameters()) )
  {
    if ( m_tsos->type(Trk::TrackStateOnSurface::Outlier))
      l << " -> Std deviations from measurement: " + VP1Msg::str(deviationFromMeasurement()) + " (Outlier)";
    else
      l << " -> Std deviations from measurement: " + VP1Msg::str(deviationFromMeasurement());
  }

  if (m_tsos->measurementOnTrack()){
    l << "----> "+TrkObjToString::name(*(m_tsos->measurementOnTrack()));
    QStringList list = TrkObjToString::fullInfo( *(m_tsos->measurementOnTrack()) );
    for (int i = 0; i < (list.size()-1); ){
      l<< (list.at(i)+QString(" = ")+list.at(i+1) ); 
      i+=2;
    }
  }

 
  if (m_tsos->fitQualityOnSurface())
  {
    l << "----> "+TrkObjToString::name(*(m_tsos->fitQualityOnSurface()));
    QStringList list = TrkObjToString::fullInfo( *(m_tsos->fitQualityOnSurface()) );
    for (int i = 0; i < (list.size()-1); ){
      l<< (list.at(i)+QString(" = ")+list.at(i+1) ); 
      i+=2;   
    }
  }

  if (m_tsos->materialEffectsOnTrack())
  {
    l << "----> "+TrkObjToString::name(*(m_tsos->materialEffectsOnTrack())) ;
     QStringList list = TrkObjToString::fullInfo( *(m_tsos->materialEffectsOnTrack()) );
     for (int i = 0; i < (list.size()-1); ){
       l<< (list.at(i)+QString(" = ")+list.at(i+1) ); 
       i+=2;   
     }
  }

  if (common()->controller()->printVerboseInfoOnSingleSelection()) {
    l << " ===== TrackStateOnSurface =====";
    std::ostringstream s2;
    s2 << *m_tsos;
    l << QString(s2.str().c_str()).split('\n');
    
    if (surface()) {
      l << " ===== Surface =====";
      std::ostringstream s;
      s << *(surface());
      l << QString(s.str().c_str()).split('\n');
    }
  }

  // Object browser stuff m_indexOfPointOnTrack
  QTreeWidgetItem* trkObjBrowseritem = trackHandle()->browserTreeItem();
  if (trkObjBrowseritem){ 
    QTreeWidgetItem* me = trkObjBrowseritem->child(m_indexOfPointOnTrack); // I hope!
    if (!me) {
      VP1Msg::messageVerbose("ERROR! Can't find AscObj_TSOS item in track obj browser.");
    } else {
      QTreeWidget* trkObjBrowser = trkObjBrowseritem->treeWidget();
      trkObjBrowser->setCurrentItem(me);
      trkObjBrowser->scrollToItem(me); 
    }
  }
  
  zoomView(); // depending on controller settings, zoom view

  return l;
}

void AscObj_TSOS::setVisible(bool vis) {
  AssociatedObjectHandleBase::setVisible(vis);
  
  QTreeWidgetItem* trkObjBrowseritem = trackHandle()->browserTreeItem();
  if (trkObjBrowseritem){ 
    QTreeWidgetItem* me = trkObjBrowseritem->child(m_indexOfPointOnTrack); // I hope!
    if (!me) {
      VP1Msg::messageVerbose("ERROR! Can't find AscObj_TSOS item in track obj browser.");
    } else {
      QFont itemFont = me->font(1);
      
      if (!visible()) {
        // std::cout<<"Hidden"<<std::endl;
        me->setFlags(0); // not selectable, not enabled
        itemFont.setStrikeOut(true);
        
      } else {
        // std::cout<<"Vis"<<std::endl;
        me->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); //  selectable,  enabled
        itemFont.setStrikeOut(false);
        
      }  
    }
  }
}


bool AscObj_TSOS::isShortMeasurement(){
  bool isShortMeasurement;
  
  // FIXME! Duplication with clicked()
  const Trk::RIO_OnTrack* rio = rioOnTrack();
  if (!rio && competingRIOsOnTrack()){
    rio = &(competingRIOsOnTrack()->rioOnTrack(competingRIOsOnTrack()->indexOfMaxAssignProb()));
  }

  TrkObjToString::MeasurementType type = m_objToType.type(rio);
  bool isTRT = (type==TrkObjToString::TRT);
  bool isMDT = (type==TrkObjToString::MDT);

  if (rio)
  {
    const bool settingsShort = (isTRT?common()->controller()->useShortTRTMeasurements()
      :isMDT&&common()->controller()->useShortMDTMeasurements());
    const bool showRioOnTrack(trackHandle()->shownTSOSParts()&m_parts & TrackCommonFlags::TSOS_AnyMeasRioOnTrack);
    isShortMeasurement = (settingsShort && showRioOnTrack);
  } else
  {
    isShortMeasurement = false;
  }
  return isShortMeasurement;
}

void AscObj_TSOS::zoomView() {
  VP1Msg::messageVerbose("AscObj_TSOS::zoomView()");
  
  if ( common()->controller()->orientAndZoomOnSingleSelection() )
  {
    const bool showParsErrors( trackHandle()->shownTSOSParts()&m_parts & TrackCommonFlags::TSOS_AnyParsErrors);


    if (!showParsErrors && !isShortMeasurement())
    { // Zoom without orientation
      VP1Msg::messageVerbose("AscObj_TSOS::zoomView() Zoom without orientation");
      std::set<SoCamera*> cameras = common()->system()->getCameraList();
      std::set<SoCamera*>::iterator it,itE = cameras.end();
      for (it=cameras.begin();it!=itE;++it) {
        if (common()->controller()->assocObjDetailLevel()==TrackCommonFlags::SIMPLE)
        {
          VP1CameraHelper::animatedZoomToSubTree(*it,common()->ascObjSelectionManager()->getAscObjAttachSep(),shapeSimple(),2.0,1.0);
        } else
        {
          VP1CameraHelper::animatedZoomToSubTree(*it,common()->ascObjSelectionManager()->getAscObjAttachSep(),shapeDetailed(),2.0,1.0);
        }
      }
    } else
    { // Zoom and orient (this is dependant of surface type)
      VP1Msg::messageVerbose("AscObj_TSOS::zoomView() Zoom and orient");
      
      const Trk::Surface * theSurface = surface();
      const Trk::PerigeeSurface      * perSurf = dynamic_cast<const Trk::PerigeeSurface *>(theSurface);
      const Trk::PlaneSurface        * plaSurf = dynamic_cast<const Trk::PlaneSurface *>(theSurface);
      const Trk::StraightLineSurface * strSurf = dynamic_cast<const Trk::StraightLineSurface *>(theSurface);
      if ( plaSurf || perSurf )
      {
//        Trk::GlobalMomentum u(m_tsos->trackParameters()->momentum().unit());
    	  Amg::Vector3D u(m_tsos->trackParameters()->momentum().unit());

        SbVec3f lookat0 = SbVec3f(-u[0],-u[1],-u[2]);

        std::set<SoCamera*> cameras = common()->system()->getCameraList();
        std::set<SoCamera*>::iterator it,itE = cameras.end();
        for (it=cameras.begin();it!=itE;++it) {
          if (common()->controller()->assocObjDetailLevel()==TrackCommonFlags::SIMPLE)
          {
            VP1CameraHelper::animatedZoomToSubTree(*it,common()->ascObjSelectionManager()->getAscObjAttachSep(),shapeSimple(),2.0,100.0,100.0,1.0,lookat0);
          } else
          {
            VP1CameraHelper::animatedZoomToSubTree(*it,common()->ascObjSelectionManager()->getAscObjAttachSep(),shapeDetailed(),2.0,100.0,100.0,1.0,lookat0);
          }
        }
      } else if ( strSurf )
      {
        SbVec3f lookat0 = SbVec3f(0,0,1);
        SbVec3f lookat1;

        SbRotation rot = VP1LinAlgUtils::toSoTransform(theSurface->transform())->rotation.getValue();
        rot.multVec(lookat0,lookat0);

        std::set<SoCamera*> cameras = common()->system()->getCameraList();
        std::set<SoCamera*>::iterator it,itE = cameras.end();
        for (it=cameras.begin();it!=itE;++it) {
          SbRotation camrot = (*it)->orientation.getValue();
          SbVec3f cameraDir = SbVec3f(0,0,-1);
          camrot.multVec(cameraDir,cameraDir);

          lookat1 = cameraDir.dot(lookat0) > 0 ? lookat0 : -lookat0;
          if (common()->controller()->assocObjDetailLevel()==TrackCommonFlags::SIMPLE)
          {
            VP1CameraHelper::animatedZoomToSubTree(*it,common()->ascObjSelectionManager()->getAscObjAttachSep(),shapeSimple(),2.0,100.0,100.0,1.0,lookat1);
          } else
          {
            VP1CameraHelper::animatedZoomToSubTree(*it,common()->ascObjSelectionManager()->getAscObjAttachSep(),shapeDetailed(),2.0,100.0,100.0,1.0,lookat1);
          }
        }
      }
    }
  } else {
    VP1Msg::messageVerbose("AscObj_TSOS::zoomView() - zooming on selection not turned on.");
  }
}
