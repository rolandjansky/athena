/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDHandle_Pixel.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1PRDSystems/PRDCollHandle_Pixel.h"
#include "VP1HEPVis/nodes/SoTransparency.h"
#include "VP1Utils/VP1ErrorUtils.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoLineSet.h>

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "GeoPrimitives/GeoPrimitives.h"



//____________________________________________________________________
PRDHandle_Pixel::PRDHandle_Pixel(PRDCollHandle_Pixel*collhandle, const InDet::PixelCluster*clus)
  : PRDHandleBase(static_cast<PRDCollHandleBase*>(collhandle)), m_cluster(clus)
{
    SoTransparency::initClass();
}

//____________________________________________________________________
void PRDHandle_Pixel::buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed)
{
  SoSeparator * errSimple  = new SoSeparator;
  SoSeparator * errDetailed  = new SoSeparator;
  
  if (!isSane()) {
    std::cerr<<"isSane check failed for PRD: "<<*m_cluster<<std::endl;
    return;
  }  
  errSimple->addChild(common()->nodeManager()->getShapeNode_Point());

  Amg::Vector2D localpos = m_cluster->localPosition();
  Amg::Vector2D localposHIT = m_cluster->detectorElement()->rawLocalPositionOfCell( m_cluster->identify() ); // Lorentz shift is not corrected because this class cannot use SiLorentzAngleTool
  SoTranslation * localtrans0 = new SoTranslation;

  double xdiff = localposHIT[Trk::locX]-localpos[Trk::locX];
  double ydiff = localposHIT[Trk::locY]-localpos[Trk::locY];
  
  if (xdiff!=xdiff || ydiff!=ydiff) {
    std::cerr<<"NaN is in local pos calc"<<std::endl;
    if (xdiff!=xdiff) std::cerr<<"X diff"<<std::endl;
    if (ydiff!=ydiff) std::cerr<<"Y diff"<<std::endl;
    std::cerr<<"localposHIT:"<<localposHIT<<std::endl;
    std::cerr<<"localpos:"<<localpos<<std::endl;
    std::cerr<<"localposHIT[Trk::locX]-localpos[Trk::locX]:"<<xdiff<<std::endl;
    std::cerr<<"localposHIT[Trk::locY]-localpos[Trk::locY]:"<<ydiff<<std::endl;
    localtrans0->unref();
    return;
  } else {
    localtrans0->translation.setValue(localposHIT[Trk::locX]-localpos[Trk::locX],localposHIT[Trk::locY]-localpos[Trk::locY],0);
  }

  const std::vector<Identifier> rdolist = m_cluster->rdoList();
  if (rdolist.size() == 1 || !collHandle()->drawRDOs())
  {
    errDetailed->addChild(localtrans0);
    //FIXME: dont hardcode thickness
    errDetailed->addChild(common()->nodeManager()->getShapeNode_Strip(m_cluster->detectorElement()->etaPitch(),m_cluster->detectorElement()->etaPitch(),m_cluster->detectorElement()->thickness()/10.0));

    //Translate back so errDetailed is left sane (eg. when drawing errors later)
    SoTranslation * localtransBack = new SoTranslation;
    localtransBack->translation.setValue(-(localtrans0->translation.getValue()));
    errDetailed->addChild(localtransBack);
  } else {
    SoSeparator * rdos = new SoSeparator;
    rdos->addChild(localtrans0);

    rdos->addChild(common()->nodeManager()->getShapeNode_Strip(m_cluster->detectorElement()->etaPitch(),m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() ),m_cluster->detectorElement()->thickness()/10.0));

    SoTransparency * transparent = new SoTransparency;
    transparent->transparency.setValue(0.5);
    rdos->addChild( transparent );
    Amg::Vector2D localposOLD = localposHIT;
    for (const Identifier& rdo_id : rdolist)
    {
      if (rdo_id == m_cluster->identify() )
        continue;

      const Amg::Vector2D localposRDO = m_cluster->detectorElement()->rawLocalPositionOfCell(rdo_id); // Lorentz shift is not corrected because this class cannot use SiLorentzAngleTool

      SoTranslation * localtrans = new SoTranslation;
      localtrans->translation.setValue(localposRDO[Trk::locX]-localposOLD[Trk::locX],
                                       localposRDO[Trk::locY]-localposOLD[Trk::locY],
                                       0);
      rdos->addChild(localtrans);

      //FIXME: dont hardcode thickness
      rdos->addChild(common()->nodeManager()->getShapeNode_Strip(m_cluster->detectorElement()->etaPitch(),
                     m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() ),
                     m_cluster->detectorElement()->thickness()/10.0));

      localposOLD = localposRDO;
    }
    errDetailed->addChild(rdos);
  }

  const double settingsSIGMASCALE=3.0;
  if (collHandle()->drawErrors())
  {
    SoGroup * gr = createErrorAtPixelCluster( settingsSIGMASCALE, 36);
    if (gr->getNumChildren()==0) {
      gr->unref();
      std::cerr<<"Something seems to have gone wrong: no error added. Dumping PRD: "<<*m_cluster<<std::endl;
    } else {
      errSimple->addChild( gr );
      errDetailed->addChild( gr );
    }
  }

  //FIXME: hardcoded colour  
  SoMaterial * mat = new SoMaterial;
  mat->diffuseColor.setValue(1.0,0,0);
  errDetailed->addChild(mat);
  errDetailed->addChild(common()->nodeManager()->getShapeNode_Cross(0.1));

  shape_simple = errSimple;
  shape_detailed = errDetailed;
}

//____________________________________________________________________
int PRDHandle_Pixel::regionIndex() { //instead of identifier juggling, we simply discretize by center coordinate.
  static const double l=200.0;//CLHEP::mm
  return       static_cast<int>(m_cluster->detectorElement()->center().z()/l)
         +1000*static_cast<int>(m_cluster->detectorElement()->center().y()/l)
      +1000000*static_cast<int>(m_cluster->detectorElement()->center().x()/l);
  //Fixme: Use identifiers instead for more intuitive regions.
}


//____________________________________________________________________
SoGroup * PRDHandle_Pixel::createErrorAtPixelCluster( const double& sigmaScale,
						      const int& numPoints ) const
{
  SoGroup * ellipseGrp = new SoSeparator;

  //FIXME: Try to automatically use the pixel height as in PRDHandle_Pixel.cxx (this is just copy paste)
  // change the last true to false above to get rid of one of the error ellipses
//  VP1ErrorUtils::addRotatedErrorEllipse(ellipseGrp, m_cluster->localErrorMatrix().covariance(), sigmaScale, numPoints,m_cluster->detectorElement()->thickness()/20.0 + 0.001, true);
  VP1ErrorUtils::addRotatedErrorEllipse(ellipseGrp, m_cluster->localCovariance(), sigmaScale, numPoints,m_cluster->detectorElement()->thickness()/20.0 + 0.001, true);

  return ellipseGrp;
}

bool PRDHandle_Pixel::isSane() const {
  // check for NaNs
  bool isSane=true;
  const InDet::PixelCluster * prd =cluster();
  if (!prd) return false;
  if (prd->localPosition()[0]!=prd->localPosition()[0] || prd->localPosition()[1]!=prd->localPosition()[1]){
    isSane=false;
    std::cerr<<"For pixel cluster with Id="<<prd->identify()<<", localposition is not sane:"<<(prd->localPosition())<<std::endl;
  }
  bool etaNan = std::isnan(m_cluster->detectorElement()->etaPitch());
  bool phiNan = std::isnan(m_cluster->detectorElement()->phiPitch( m_cluster->localPosition()));
  bool thickNan = std::isnan(m_cluster->detectorElement()->thickness());

  if ( etaNan || phiNan || thickNan){
    std::cerr<<"For pixel cluster with Id="<<prd->identify()<<", NaN is in detectorElement pitches "<<std::endl;
    if (etaNan) std::cerr<<" - eta (which is returning "<<m_cluster->detectorElement()->etaPitch()<<")"<<std::endl;
    if (phiNan) std::cerr<<" - phi (which is returning "<<m_cluster->detectorElement()->phiPitch(m_cluster->localPosition())<<")"<<std::endl;
    if (thickNan) std::cerr<<" - thickness (which is returning "<<m_cluster->detectorElement()->thickness()<<")"<<std::endl;
  }
  
  return isSane;
}
