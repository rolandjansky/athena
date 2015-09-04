/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDHandle_Planar.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1PRDSystems/PRDCollHandle_Planar.h"
#include "VP1HEPVis/nodes/SoTransparency.h"
#include "VP1Utils/VP1ErrorUtils.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoLineSet.h>

#include "ISF_FatrasDetDescrModel/PlanarDetElement.h"

#include "GeoPrimitives/GeoPrimitives.h"

//____________________________________________________________________
PRDHandle_Planar::PRDHandle_Planar(PRDCollHandle_Planar* collhandle, const iFatras::PlanarCluster* clus)
  : PRDHandleBase(static_cast<PRDCollHandleBase*>(collhandle)), m_cluster(clus)
{
    SoTransparency::initClass();
}

//____________________________________________________________________
void PRDHandle_Planar::buildShapes(SoNode*& shape_simple, SoNode*& shape_detailed)
{
  double striplength = 0;
  SoSeparator * errSimple  = new SoSeparator;
  SoSeparator * errDetailed  = new SoSeparator;
  
  if (!isSane()) {
    std::cerr<<"isSane check failed for PRD: "<<*m_cluster<<std::endl;
    return;
  }  
  
  bool isPixel = m_cluster->detectorElement()->isPixel();
  bool isDisc = ((m_cluster->detectorElement()->surface()).type() == Trk::Surface::Disc);

  Amg::Vector2D localpos = m_cluster->localPosition();
  Amg::Vector2D localposHIT = m_cluster->detectorElement()->localPositionOfCell( m_cluster->identify() );
  
  double radius = 0.;
  if (!isPixel and isDisc) {
    Amg::Vector2D localCart = Amg::Vector2D(localpos[Trk::locR]*cos(localpos[Trk::locPhi]), localpos[Trk::locR]*sin(localpos[Trk::locPhi]));
    Amg::Vector2D localCartHIT = Amg::Vector2D(localposHIT[Trk::locR]*cos(localposHIT[Trk::locPhi]), localposHIT[Trk::locR]*sin(localposHIT[Trk::locPhi]));
    localpos = localCart;
    localposHIT = localCartHIT;
    radius = localposHIT[Trk::locR];
  }
  
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
    //localtrans0->translation.setValue(localposHIT[Trk::locX]-localpos[Trk::locX],localposHIT[Trk::locY]-localpos[Trk::locY],0);
    localtrans0->translation.setValue(0.,0.,0.);
  }

  if (isPixel)
    errSimple->addChild(common()->nodeManager()->getShapeNode_Point());
  else {
    striplength = m_cluster->detectorElement()->length();
    shape_simple=common()->nodeManager()->getShapeNode_Strip(striplength);
  }
  
  const std::vector<Identifier> rdolist = m_cluster->rdoList();
  if (rdolist.size() == 1 || !collHandle()->drawRDOs())
  {
    errDetailed->addChild(localtrans0);
    
    //FIXME: dont hardcode thickness
    if (isPixel) 
      errDetailed->addChild(common()->nodeManager()->getShapeNode_Strip(m_cluster->detectorElement()->pitchX(),
									m_cluster->detectorElement()->pitchY(), 
									m_cluster->detectorElement()->thickness()/10.0));
    else 
      if (isDisc) 
	errDetailed->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
									  m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() )*radius,//strip width
									  m_cluster->detectorElement()->thickness()*3.0));
      else  
	errDetailed->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
									  m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() ),//strip width
									  m_cluster->detectorElement()->thickness()*3.0));
    //strip thickness - scaled up by factor of 3 (looks better)
    //Fixme: Should we drop this upscaling of thickness?

    //Translate back so errDetailed is left sane (eg. when drawing errors later)
    SoTranslation * localtransBack = new SoTranslation;
    localtransBack->translation.setValue(-(localtrans0->translation.getValue()));
    errDetailed->addChild(localtransBack);
    
  } else {
    SoSeparator * rdos = new SoSeparator;
    rdos->addChild(localtrans0);
    
    if (isPixel) 
      rdos->addChild(common()->nodeManager()->getShapeNode_Strip(m_cluster->detectorElement()->pitchX(),
								 m_cluster->detectorElement()->pitchY(), 
								 m_cluster->detectorElement()->thickness()/10.0));
    else 
      if (isDisc) 
	rdos->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
								   m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() )*radius,//strip width
								   m_cluster->detectorElement()->thickness()*3.0));
      else 
    	rdos->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
								   m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() ),//strip width
								   m_cluster->detectorElement()->thickness()*3.0));
    
    //strip thickness - scaled up by factor of 3 (looks better)
    //Fixme: Should we drop this upscaling of thickness?
    
    SoTransparency * transparent = new SoTransparency;
    transparent->transparency.setValue(0.5);
    rdos->addChild( transparent );
    Amg::Vector2D localposOLD = localposHIT;
    std::vector<Identifier>::const_iterator st = rdolist.begin();
    std::vector<Identifier>::const_iterator en = rdolist.end();
    for( std::vector<Identifier>::const_iterator it = st; it!=en; it++)
      {
	if (*it == m_cluster->identify() )
	  continue;
	
	const Amg::Vector2D localposRDO = m_cluster->detectorElement()->localPositionOfCell(*it);
	
	SoTranslation * localtrans = new SoTranslation;
	localtrans->translation.setValue(localposRDO[Trk::locX]-localposOLD[Trk::locX],
					 localposRDO[Trk::locY]-localposOLD[Trk::locY],
					 0);
	
	rdos->addChild(localtrans);
	//FIXME: dont hardcode thickness
	if (isPixel) 
	  rdos->addChild(common()->nodeManager()->getShapeNode_Strip(m_cluster->detectorElement()->pitchX(),
								     m_cluster->detectorElement()->pitchY(), 
								     m_cluster->detectorElement()->thickness()/10.0));
	else 
	  if (isDisc) 
	    rdos->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
								       m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() )*radius,//strip width
								       m_cluster->detectorElement()->thickness()*3.0));
	  else  
	    rdos->addChild(common()->nodeManager()->getShapeNode_Strip(striplength,
								       m_cluster->detectorElement()->phiPitch( m_cluster->localPosition() ),//strip width
								       m_cluster->detectorElement()->thickness()*3.0));
	
	localposOLD = localposRDO;
      }
    errDetailed->addChild(rdos);
  }
  
  if (isPixel) {
    const double settingsSIGMASCALE=3.0;
    if (collHandle()->drawErrors())
      {
	SoGroup * gr = createErrorAtPlanarCluster( settingsSIGMASCALE, 36);
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
  }
  shape_detailed = errDetailed;
}

//____________________________________________________________________
int PRDHandle_Planar::regionIndex() { //instead of identifier juggling, we simply discretize by center coordinate.
  double myl = 200.0;//CLHEP::mm
  bool isPixel = m_cluster->detectorElement()->isPixel();
  if (!isPixel)
    myl = 100.0;//CLHEP::mm
  static const double l=myl;//CLHEP::mm
  return       static_cast<int>(m_cluster->detectorElement()->center().z()/l)
    +1000*static_cast<int>(m_cluster->detectorElement()->center().y()/l)
    +1000000*static_cast<int>(m_cluster->detectorElement()->center().x()/l);
  //Fixme: Use identifiers instead for more intuitive regions.
}


//____________________________________________________________________
SoGroup * PRDHandle_Planar::createErrorAtPlanarCluster( const double& sigmaScale,
							const int& numPoints ) const
{
  SoGroup * ellipseGrp = new SoSeparator;
  
  VP1ErrorUtils::addRotatedErrorEllipse(ellipseGrp, m_cluster->localCovariance(), sigmaScale, numPoints, m_cluster->detectorElement()->thickness()/20.0 + 0.001, true);
  
  return ellipseGrp;
}

bool PRDHandle_Planar::isSane() const {
  // check for NaNs
  bool isSane=true;
  const iFatras::PlanarCluster * prd =cluster();
  if (!prd) return false;
  if (prd->localPosition()[0]!=prd->localPosition()[0] || prd->localPosition()[1]!=prd->localPosition()[1]){
    isSane=false;
    std::cerr<<"For planar cluster with Id="<<prd->identify()<<", localposition is not sane:"<<(prd->localPosition())<<std::endl;
  }
   bool XNan = std::isnan(m_cluster->detectorElement()->pitchX());
   bool YNan = std::isnan(m_cluster->detectorElement()->pitchY());
   bool thickNan = std::isnan(m_cluster->detectorElement()->thickness());
   
   if ( XNan || YNan || thickNan ){
     std::cerr<<"For planar cluster with Id="<<prd->identify()<<", NaN is in detectorElement "<<std::endl;
     if (XNan) std::cerr<<" - eta (which is returning "<<m_cluster->detectorElement()->pitchX()<<")"<<std::endl;
     if (YNan) std::cerr<<" - phi (which is returning "<<m_cluster->detectorElement()->pitchY()<<")"<<std::endl;
     if (thickNan) std::cerr<<" - thickness (which is returning "<<m_cluster->detectorElement()->thickness()<<")"<<std::endl;
   }
   
   return isSane;
}
