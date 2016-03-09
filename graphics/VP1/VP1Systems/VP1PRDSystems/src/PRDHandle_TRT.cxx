/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDHandle_TRT.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Utils/VP1DetInfo.h"
#include "VP1Base/VP1Msg.h"
#include "VP1GuideLineSystems/InDetProjHelper.h"
#include "VP1GuideLineSystems/InDetProjParams.h"

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

#include "InDetIdentifier/TRT_ID.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/Surface.h"

#include "GeoPrimitives/GeoPrimitives.h"
//____________________________________________________________________
PRDHandle_TRT::PRDHandle_TRT(PRDCollHandle_TRT*collhandle, const InDet::TRT_DriftCircle*dc)
  : PRDHandleBase(static_cast<PRDCollHandleBase*>(collhandle)), m_driftcircle(dc)
{
}

//____________________________________________________________________
void PRDHandle_TRT::buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed)
{
  const Trk::CylinderBounds* ccbo = dynamic_cast<const Trk::CylinderBounds*>(&(m_driftcircle->detectorElement()->surface(m_driftcircle->identify()).bounds()));
  assert(ccbo!=0);
  const double radius = m_driftcircle->localPosition()[0];
  const double halflength = ccbo->halflengthZ();
  const double mintuberadius = 0.1;

  if ( m_driftcircle->rdoList().size() > 1)
    VP1Msg::message("Warning: TRT has additional elements in rdoList: NOT IMPLEMENTED");

  const PRDCollHandle_TRT* trtcollhandle(static_cast<PRDCollHandle_TRT*>(collHandle()));

  if (trtcollhandle->project()) {

    ////////////////////
    //  Projections:  //
    ////////////////////

    const InDetProjFlags::InDetProjPartsFlags projparts(trtcollhandle->appropriateProjections());
    bool inner,outer;
    if (isBarrel()) {
      inner = (projparts&InDetProjFlags::BarrelCentral);
      outer = (projparts&(isPositiveZ()?InDetProjFlags::BarrelPositive:InDetProjFlags::BarrelNegative));
    } else {
      const bool posz(isPositiveZ());
      if ( (projparts&InDetProjFlags::TRT_EndCapZToRCentral)
	   || (projparts&(posz?InDetProjFlags::TRT_EndCapZToRPositive:InDetProjFlags::TRT_EndCapZToRNegative)) ) {
	//Special TRT EndCap Z->R projections.

	//Find center of tube:
	Amg::Transform3D prdtransform(getTransform_CLHEP());
	Amg::Vector3D center(prdtransform*Amg::Vector3D(0,0,0));

	// --> Get points for special projections
	const double eps = InDetProjParams::trt_data_disttosurface_epsilon();
	std::vector<Amg::Vector3D > points;

	if (posz) {
	  if ( projparts & InDetProjFlags::TRT_EndCapZToRPositive ) {
	    Amg::Vector3D p = center;
	    InDetProjHelper::transformECPointToZPlane_specialZtoR(p,
								  InDetProjParams::trt_barrel_posneg_z()-0.5* InDetProjParams::surfacethickness() - eps,
								  InDetProjParams::trt_endcap_zasr_innerradius(),
								  InDetProjParams::trt_endcap_zasr_endcapz_begin(),
								  InDetProjParams::trt_endcap_zasr_squeezefact());
	    points.push_back(p);
	  }
	  if ( projparts & InDetProjFlags::TRT_EndCapZToRCentral ) {
	    Amg::Vector3D p = center;
	    //Fixme: Use same parameters here as in InDetProjHelper!!
	    InDetProjHelper::transformECPointToZPlane_specialZtoR(p,
								  0.5* InDetProjParams::surfacethickness() + eps,
								  InDetProjParams::trt_endcap_zasr_innerradius(),
								  InDetProjParams::trt_endcap_zasr_endcapz_begin(),
								  InDetProjParams::trt_endcap_zasr_squeezefact() );
	    points.push_back(p);
	  }
	} else {
	  if ( projparts & InDetProjFlags::TRT_EndCapZToRNegative ) {
	    Amg::Vector3D p = center;
	    InDetProjHelper::transformECPointToZPlane_specialZtoR(p,
								  - InDetProjParams::trt_barrel_posneg_z()+0.5* InDetProjParams::surfacethickness() + eps,
								  InDetProjParams::trt_endcap_zasr_innerradius(),
								  InDetProjParams::trt_endcap_zasr_endcapz_begin(),
								  InDetProjParams::trt_endcap_zasr_squeezefact());
	    points.push_back(p);
	  }
	  if ( projparts & InDetProjFlags::TRT_EndCapZToRCentral ) {
	    Amg::Vector3D p = center;
	    InDetProjHelper::transformECPointToZPlane_specialZtoR(p,
								  -0.5* InDetProjParams::surfacethickness() - eps,
								  InDetProjParams::trt_endcap_zasr_innerradius(),
								  InDetProjParams::trt_endcap_zasr_endcapz_begin(),
								  InDetProjParams::trt_endcap_zasr_squeezefact());
	    points.push_back(p);
	  }
	}
	// --> Build scene objects for special projections

	const unsigned npoints = points.size();
	if (npoints<1||npoints>2) {
	  collHandle()->message("PRDHandle_TRT::buildShapes ERROR: Unexpected number of points in special endcap projections!!");
	} else {
	  SoSeparator * sepsimple = new SoSeparator;
	  SoTransform * transf1(VP1LinAlgUtils::toSoTransform((prdtransform.inverse())
							      * Amg::Translation3D(points.at(0).x(),points.at(0).y(),points.at(0).z())));
	  SoNode* point = common()->nodeManager()->getShapeNode_Point();
	  sepsimple->addChild(transf1);
	  sepsimple->addChild(point);
	  SoTranslation * transf2(0);
	  if (npoints==2) {
 	    transf2 = new SoTranslation;
	    transf2->translation.setValue(points[1].x()-points[0].x(),points[1].y()-points[0].y(),points[1].z()-points[0].z());
	    sepsimple->addChild(transf2);
	    sepsimple->addChild(point);
	  }
	  shape_simple = sepsimple;

	  if (radius<mintuberadius) {
	    shape_detailed = shape_simple;
	    return;
	  }
	  SoSeparator * sepdetailed = new SoSeparator;
	  SoNode* disc = common()->nodeManager()->getShapeNode_DriftDisc(radius);
	  sepdetailed->addChild(transf1);
	  sepdetailed->addChild(disc);
	  if (npoints==2) {
	    sepdetailed->addChild(transf2);
	    sepdetailed->addChild(disc);
	  }
	  shape_detailed = sepdetailed;
	  return;
	}
	inner = false;outer = false;
	//end special projections
      } else {
	inner = (projparts&(posz?InDetProjFlags::EndCapInnerPositive:InDetProjFlags::EndCapInnerNegative));
	outer = (projparts&(posz?InDetProjFlags::EndCapOuterPositive:InDetProjFlags::EndCapOuterNegative));
      }
    }//end endcap
    if (!inner&&!outer) {
      collHandle()->message("PRDHandle_TRT::buildShapes ERROR: Inconsistent projection settings! Won't project!");
    } else {
      //Perform projections
      shape_simple = common()->nodeManager()->getShapeNode_ProjectedDriftTube( halflength, 0.0, inner, outer );
      if (radius<mintuberadius)
	shape_detailed = shape_simple;
      else
	shape_detailed = common()->nodeManager()->getShapeNode_ProjectedDriftTube( halflength, radius, inner, outer );
      return;
    }
  }//endif projections

  ///////////////////////
  //  No projections:  //
  ///////////////////////

  shape_simple = common()->nodeManager()->getShapeNode_DriftTube( halflength, 0.0 );
  if (radius<mintuberadius)
    shape_detailed = shape_simple;
  else
    shape_detailed = common()->nodeManager()->getShapeNode_DriftTube( halflength, radius );

}

//____________________________________________________________________
int PRDHandle_TRT::regionIndex()
{
  const TRT_ID * id_helper = VP1DetInfo::trtIDHelper();
  if (!id_helper)
    return 0;
  Identifier id = m_driftcircle->identify();
  return id_helper->barrel_ec(id)*10000+id_helper->layer_or_wheel(id)*100+(id_helper->phi_module(id)%4);
}
// MuonGM::MuonDetectorManager::getMdtReadoutElement ()

//____________________________________________________________________
QStringList PRDHandle_TRT::clicked() const
{
  QStringList l;
  l << PRDHandleBase::clicked();
  const unsigned int data = m_driftcircle->getWord();

  QString s;
  int i;
  for (i = 26; i >=0; i--)
  {
    if (i!=26&&(i+1)%9==0)
      s+=" ";
    s += ( data & (0x1<<i) ? "1" : "0");
    if ((i+1)%9==0)
      s+=" ";
  }
  l << "Raw data bits: "+s;


  std::ostringstream os;
  os << std::hex<<data;
  l << QString(os.str().c_str());


//   l << "Drift radius: "+QString::number(m_driftcircle->localPosition()[0]/CLHEP::mm)+" CLHEP::mm";
//   const Trk::CylinderBounds* ccbo = dynamic_cast<const Trk::CylinderBounds*>(&(m_driftcircle->detectorElement()->surface(m_driftcircle->identify()).bounds()));
//   if (ccbo)
//     l << "Straw length: "+QString::number(ccbo->halflengthZ()*2/CLHEP::cm)+" CLHEP::cm";

  return l;
}
