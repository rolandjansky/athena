/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelInclinedRef/GeoPixelLadderInclRef.h"
#include "BarrelInclinedRef/GeoPixelStaveSupportInclRef.h"
#include "BarrelInclinedRef/GeoPixelEndcapModuleSvcRef.h"

#include "PixelGeoModelModule/GeoDetModulePixel.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "PixelGeoModel/PixelGeoBuilder.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "PathResolver/PathResolver.h"

using std::max;

// There must be a better way of passing layer information into this class - maybe it's possible to clone the layer xml reader?
GeoPixelLadderInclRef::GeoPixelLadderInclRef(const PixelGeoBuilderBasics* basics, const InDet::StaveTmp *staveTmp, 
					     int iLayer, int iLadder, int nSectors, int nSectorsLastLayer, int nSectorsNextLayer, 
					     double phiOfStaveZero, double phiOfStaveZeroLastLayer, HepGeom::Transform3D trf):
  PixelGeoBuilder(basics),							
  m_staveTmp(staveTmp),
  m_layer(iLayer),
  m_ladderType(iLadder),
  m_nSectors(nSectors),
  m_nSectorsLastLayer(nSectorsLastLayer),
  m_nSectorsNextLayer(nSectorsNextLayer),
  m_phiOfStaveZero(phiOfStaveZero),
  m_phiOfStaveZeroLastLayer(phiOfStaveZeroLastLayer),
  m_localTrf(trf),
  m_sector(0),
  m_barrelModule(NULL),
  m_barrelModuleDesign(NULL),
  m_endcapModule(NULL),
  m_endcapModuleDesign(NULL),
  m_transitionModule(NULL),
  m_transitionModuleDesign(NULL),
  m_staveSupport(NULL),
  m_ladderShape(NULL),
  m_minmaxDefined(false),
  m_pixelModuleSvc("PixelModuleSvc","PixelModuleSvc"),
  m_pixelDesignSvc("PixelDesignSvc","PixelDesignSvc"),
  m_IDserviceTool("InDetservicesTool/PixelServicesTool")
{

  StatusCode sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::INFO) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

  m_staveDBHelpers.push_back(new PixelInclRefStaveXMLHelper(m_layer, getBasics()));
  if (m_layer > 0) m_staveDBHelpers.push_back(new PixelInclRefStaveXMLHelper(m_layer-1,  getBasics()));

  // Build stave support and module
  m_svcMaterialCmpt=0;

  preBuild();
}


void GeoPixelLadderInclRef::preBuild( ) {


  // Check if the envelope is defined as a tool and available in ToolSvc
  StatusCode sc = m_pixelModuleSvc.retrieve();
  if(sc.isFailure())
    {
      msg(MSG::DEBUG) << "Could not retrieve pixel module builder tool " <<  m_pixelModuleSvc << ",  some services will not be built." << endreq;
    } 
  else 
    {
      msg(MSG::DEBUG) << "Pixel module builder tool retrieved: " << m_pixelModuleSvc << endreq;
    }

   m_pixelModuleSvc->initModuleMap(getBasics());
   m_pixelDesignSvc->initModuleMap(getBasics());

  std::string value;
  msg(MSG::DEBUG)<<endreq;
  msg(MSG::DEBUG)<<"************** BUILD LADDER for layer "<< m_layer<<"  "<<m_staveTmp->name<<" --"<<endreq;

  std::string staveType = m_staveTmp->type;

  m_barrelModuleNumber = m_staveTmp->b_modn;
  m_barrelModuleType = m_staveTmp->b_type;
  m_barrelModuleGap =  m_staveTmp->b_gap;
  m_barrelModuleTilt =  m_staveTmp->b_tilt;
  m_barrelModuleZeroTilt = 1.e-10 * m_barrelModuleTilt;
  if(m_barrelModuleTilt>0) m_barrelModuleZeroTilt /= fabs(m_barrelModuleTilt);

  m_barrelModuleZoffset = m_staveTmp->b_zoffset;

  m_endcapModuleNumber = 0;
  m_endcapModuleType = "";
  m_endcapModulePos.clear();
  m_endcapModuleGap = 0;
  m_endcapInclAngle = 0;
  m_transitionModuleNumber = 0;
  m_transitionModuleType = "";
  m_transitionTiltAngle = 0;
  m_transModulePos.clear();
  if(staveType.compare("Alpine")==0){
    m_endcapModulePos = m_staveTmp->alp_pos;
    m_endcapModuleType = m_staveTmp->alp_type;
    m_endcapModuleGap = 0.;
    m_endcapInclAngle = m_staveTmp->alp_angle;
    m_endcapModuleROffset = m_staveTmp->alp_roffset;
    m_endcapModuleRshift = m_staveTmp->alp_rshift;
    m_endcapModuleRtilt  = m_staveTmp->alp_radialTilt;

    m_endcapModuleZoffset = m_staveTmp->alp_zoffset;

    m_transModulePos = m_staveTmp->trans_pos;
    if(m_transModulePos.size()>0){
      m_transitionModuleNumber = (int)m_transModulePos.size();
      m_transitionModuleType = m_staveTmp->trans_type;
      m_transitionTiltAngle = m_staveTmp->trans_angle;
    }

    m_endcapModPos = DecodeEndCapModulePositions(0., 0., m_transModulePos,m_endcapModulePos);
    m_endcapModuleNumber = (int)m_endcapModPos.size();
  }

  int layerModuleIndex = 0;
  msg(MSG::DEBUG)<<"xxxxxxxxxxxxx Get barrel module from svc : "<<m_barrelModuleType<<" / "<<staveType<<endreq;
  m_barrelModule = m_pixelModuleSvc->getModule(getBasics(),0,layerModuleIndex,m_barrelModuleType);
  m_barrelModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_barrelModuleType);

  m_barrelZMax = 9.99e99;
  unsigned int index = (m_staveDBHelpers[0]->getSvcRoutingPos() == "inner") ? 1 : 0;       

  if (m_staveDBHelpers[index]->getStaveSupportType() == "Longeron"){
    int nStaveShapes = m_staveDBHelpers[index]->getNStaveShapes();
    for (int iss = 0; iss < nStaveShapes; iss++)
      if (m_barrelZMax > m_staveDBHelpers[index]->getBarrelZMax(iss)) m_barrelZMax = m_staveDBHelpers[index]->getBarrelZMax(iss);
  }
  else
    m_barrelZMax = 0.5 * (m_barrelModuleNumber * (m_barrelModule->Length() + m_barrelModuleGap));

  msg(MSG::DEBUG)<<"-- Barrel modules : "<<m_barrelModuleNumber<<" "<<m_barrelModuleType<<" / "<<staveType<<endreq;

  double maxTiltedModuleThick=0.; 
  // get the transition and endcap modules from moduelSvc
  if(staveType.compare("Alpine")==0 && m_endcapModuleNumber>0){
    msg(MSG::DEBUG)<<"xxxxxxxxxxxxx Get endcap module from svc : "<<m_endcapModuleType<<" / "<<staveType<<" "<<m_endcapModuleType<<"&"<<m_transitionModuleType<<endreq;
    m_endcapModule =  m_pixelModuleSvc->getModule(getBasics(),0,layerModuleIndex,m_endcapModuleType);
    m_endcapModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_endcapModuleType);
    maxTiltedModuleThick=m_endcapModule->Length()*cos(m_endcapInclAngle); 
    if(m_transitionModuleNumber>0){
      m_transitionModule =  m_pixelModuleSvc->getModule(getBasics(),0,layerModuleIndex,m_transitionModuleType);
      m_transitionModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_transitionModuleType);
      maxTiltedModuleThick=std::max(maxTiltedModuleThick,
				    m_transitionModule->Length()*cos(m_transitionTiltAngle));
    }

  }

  msg(MSG::DEBUG)<<"xxxxxxxxxxxxx Ladder modules barrel/endcap/transition "<<m_barrelModuleNumber<<" "<<m_endcapModuleNumber<<" "<<m_transitionModuleNumber<<"   Total "<<m_barrelModuleNumber*.5+m_endcapModuleNumber<<endreq;

  // ----------------------------------------------------------------------------
  // Stave module service thickness
  // ----------------------------------------------------------------------------
  m_moduleSvcThickness = .25 * CLHEP::mm;

  // ----------------------------------------------------------------------------
  // Build stave support
  // ----------------------------------------------------------------------------
  msg(MSG::DEBUG)<<"xxxxxxxxxxxxx Build stave support for layer : "<<m_layer<<endreq;
  double zEndOfNBarrelModulePos = (m_barrelModuleNumber*m_barrelModule->Length()+m_barrelModuleGap*(m_barrelModuleNumber-1))*.5;
  
  m_svcRouting = m_staveDBHelpers[0]->getSvcRoutingPos();

  m_gapPlanarStave   = m_staveDBHelpers[0]->getGapPlanarStave();
  double serviceOffsetX     = m_staveDBHelpers[0]->getServiceOffsetX();
  double staveSupportThick  = m_staveDBHelpers[0]->getStaveSupportThick();
  double ecMinRadialPos = m_endcapModuleRshift-maxTiltedModuleThick-m_endcapModuleROffset;
  double ecMaxRadialPos = m_endcapModuleRshift+m_endcapModuleROffset;
  double ecRadialPos    = (m_svcRouting=="inner") ? ecMinRadialPos : ecMaxRadialPos;

  m_thicknessN =  -(serviceOffsetX) + (0.5 * staveSupportThick); 
  if(ecRadialPos<0) m_thicknessN += (-ecRadialPos);
  m_thicknessP =  serviceOffsetX + (0.5*staveSupportThick);
  if (ecRadialPos>0) m_thicknessP += ecRadialPos;
  
  m_length = m_staveDBHelpers[0]->getStaveSupportLength() + 0.01;//TODO: safety offset hardcoded, find a better solution  
  m_width  = m_staveDBHelpers[0]->getStaveSupportWidth();
  //catching old layouts before support width was correctly specified
  if(m_width<0){
    m_width = m_barrelModule->Width()*.7;
    msg(MSG::DEBUG)<<"Special Case for old layouts! m_width set to"<<m_width<<" due via m_barrelModule->Width()*.7 - not to be relied on in new developments! Please set your stave support width correctly!"<<endreq;
  }
  // malformed volume
  if (m_width==0) msg(MSG::WARNING) <<"stave support width set to 0 for layer "<<m_layer<<" : is this intentional ? "<<endreq;

  if (m_staveDBHelpers[0]->getStaveSupportType() == "Standard" ) {
    m_staveSupport = new GeoPixelStaveSupportInclRef( getBasics(), m_layer, *m_barrelModule, m_barrelModuleTilt, 0., m_gapPlanarStave, ecMinRadialPos, ecMaxRadialPos, zEndOfNBarrelModulePos);
  }
 
  msg(MSG::DEBUG)<<"** stave support thicknesses   : "<<m_thicknessN<<"  "<<m_thicknessP<<endreq;
  msg(MSG::DEBUG)<<"** stave support width/length  : "<<m_width<<"  "<<m_length<<endreq;


  // ----------------------------------------------------------------------------
  // MinMax parameters
  // ----------------------------------------------------------------------------
  m_envLength = m_length;
  m_rmin = m_thicknessP;
  m_rmax = m_thicknessN;

  // Register the number of modules defined for a stave
  int nbTotModule = m_barrelModuleNumber;
  if(m_endcapModuleNumber>0) nbTotModule += 2*(m_endcapModuleNumber);
  getBasics()->getDetectorManager()->numerology().setNumEtaModulesForLayer(m_layer, nbTotModule);


}



GeoVPhysVol* GeoPixelLadderInclRef::Build( ) {

 // Pixel general envelope XML reader
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML", getBasics());
  bool buildDeadEdges  = genDBHelper.propagateDeadEdgeToSensorPosition();

  GeoPhysVol* ladderPhys = 0;

  int iModuleCmpt = 0; 
  m_svcMaterialCmpt = 0;

  double radiusMin = 999999.9;
  double radiusMax = -999999.9;  
  
  bool bVerbose = false;
  
  if(m_endcapModuleNumber>0)
    iModuleCmpt = -(m_endcapModuleNumber+m_transitionModuleNumber)-m_barrelModuleNumber/2;
  else
    iModuleCmpt = -m_barrelModuleNumber/2 - (m_barrelModuleNumber%2);

  if(bVerbose)std::cout<<"FIRST module number "<<m_barrelModuleNumber<<"  "<<m_endcapModuleNumber<<"  "<<iModuleCmpt<<std::endl;
  
  int inclinedModTag = 500;   // new sensor identifier tag for inclined modules
  
  double rOffset = (m_sector%2==0) ? -0.5*m_endcapModuleROffset : 0.5*m_endcapModuleROffset;
  double m_EndcapGlobalRshift = m_endcapModuleRshift+rOffset;

  //--------------------------------------------------------
  // Compute endcap module positions
  //--------------------------------------------------------
  std::vector<HepGeom::Point3D<double> > endcapModulePos;
  std::vector<HepGeom::Point3D<double> > endcapModulePosBottom;
  std::vector<HepGeom::Point3D<double> > endcapModulePosTop;

  // Default values correspond to Outer position : top of the module aligned with the barrel modules
  //                                             :  zpos is the top corner of the endcap module
  if(m_endcapModuleNumber>0)
    {
      HepGeom::Point3D<double> vDir_endcap(-cos(m_endcapInclAngle), sin(m_endcapInclAngle), 0.);
      HepGeom::Point3D<double> vDirPerp_endcap(-sin(m_endcapInclAngle), -cos(m_endcapInclAngle), 0.);
      HepGeom::Point3D<double> vDir_transition(-cos(m_transitionTiltAngle), sin(m_transitionTiltAngle), 0.);
      HepGeom::Point3D<double> vDirPerp_transition(sin(m_transitionTiltAngle), cos(m_transitionTiltAngle), 0.);
      
      double EcModThick_chip= m_endcapModule->getModuleSensorThick()*.5;
      double EcModHalfLen = 0.5*( buildDeadEdges ? m_endcapModule->getModuleChipLength() : m_endcapModule->getModuleSensorLength());

      double zOffset = (m_sector%2==0) ? -0.5*m_endcapModuleZoffset : 0.5*m_endcapModuleZoffset;

      for(int iPos=0; iPos<m_endcapModuleNumber; iPos++)
	{
	  double xPos=buildDeadEdges ? 0. : m_barrelModule->ThicknessP();
	  double yPos=m_endcapModPos[iPos].y();
	  double zPos=m_endcapModPos[iPos].z() + zOffset;
	  
	  if(iPos==0&&m_transitionModuleNumber>0) {
	    xPos=xPos+EcModHalfLen*vDir_transition.x()+EcModThick_chip*vDirPerp_transition.x();
	    xPos+=m_EndcapGlobalRshift;
	    zPos=zPos+EcModHalfLen*vDir_transition.y()+EcModThick_chip*vDirPerp_transition.y();
	    yPos-=m_transitionModule->Width()*.5;
	    endcapModulePos.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	    endcapModulePosTop.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	    endcapModulePosBottom.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	  }	      
	  else {
	    xPos=xPos+EcModHalfLen*vDir_endcap.x()+EcModThick_chip*vDirPerp_endcap.x();
	    xPos+=m_EndcapGlobalRshift;
	    zPos=zPos+EcModHalfLen*vDir_endcap.y()+EcModThick_chip*vDirPerp_endcap.y();
	    yPos=0.;
	    endcapModulePos.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	    endcapModulePosTop.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	    endcapModulePosBottom.push_back(HepGeom::Point3D<double>(xPos,yPos,zPos));
	  }
	}
    }
       
  // Number of module services to take into account for each module
  std::vector<int> nbModuleSvc;
  int vecSize=(m_transitionModuleNumber==0)?2:3;
  if (m_endcapModuleNumber==0) vecSize--;
  nbModuleSvc.resize(vecSize);
  int brlModuleIndex=0;
  int transModuleIndex=(m_transitionModuleNumber>0)?brlModuleIndex+1:-1;
  int endcapModuleIndex=(m_endcapModuleNumber>0)?vecSize-1:-1;

  int brlModuleCmpt=-1;
  nbModuleSvc[brlModuleIndex]=m_barrelModuleNumber/2+m_barrelModuleNumber%2;
  if(transModuleIndex>0)nbModuleSvc[transModuleIndex]=m_transitionModuleNumber/2;
  if(endcapModuleIndex)nbModuleSvc[endcapModuleIndex]=m_endcapModuleNumber;  

  if(bVerbose){
    std::cout<<"MODULE SERVICES init for layer  "<<m_layer <<" : ";  for(int i=0; i<(int)nbModuleSvc.size(); i++) std::cout<<nbModuleSvc[i]<<" "; std::cout<<std::endl;
  }

   //------------------------------------------------------
   // Place the endcap modules (Z<0)
   //------------------------------------------------------
   if(m_endcapModuleNumber>0)
     {
       // Build the service structure of the endcap module
       //GeoPixelEndcapModuleSvcRef foamBuilder(getBasics(),m_layer, m_endcapModule, m_transitionModule, m_endcapInclAngle, m_transitionTiltAngle);
       GeoPixelEndcapModuleSvcRef foamBuilder(getBasics(), m_width, m_layer, m_endcapModule, m_transitionModule, m_endcapInclAngle, m_transitionTiltAngle);
       GeoPhysVol* foamEndcap = dynamic_cast<GeoPhysVol*>(foamBuilder.getEndcapFoam());
       
       //Adding correct staggering in r and z for the negative side
       double zOffset = (m_sector%2==0) ? -0.5*m_endcapModuleZoffset : 0.5*m_endcapModuleZoffset;
       double rOffset = (m_sector%2==0) ?  0.5*m_endcapModuleROffset : -0.5*m_endcapModuleROffset;

       // Endcap servcie transforms
       double zEndcapFoamShift = foamBuilder.getEndcapZshift();
       zEndcapFoamShift += (m_svcRouting=="inner") ? -0.25 : 0.25;
       GeoPhysVol* foamTrans = dynamic_cast<GeoPhysVol*>(foamBuilder.getTransFoam());
       double zTransFoamShift = foamBuilder.getTransZshift();
       zTransFoamShift += (m_svcRouting=="inner") ? -0.25 : 0.25;
       double xEndcapFoamShift = m_EndcapGlobalRshift +2*rOffset - m_endcapModule->Length()*cos(m_endcapInclAngle)  ;
       double xTransFoamShift = xEndcapFoamShift  ;

       if (foamEndcap && !foamTrans) {
	 foamTrans = foamEndcap;
         zTransFoamShift = zEndcapFoamShift;
       }  
     
       HepGeom::Transform3D trfFoam = (m_svcRouting=="inner") ? 
	 HepGeom::RotateZ3D(270.*CLHEP::deg)*HepGeom::RotateY3D((90.+m_endcapModuleRtilt)*CLHEP::deg):      
	 HepGeom::RotateZ3D(270.*CLHEP::deg)*HepGeom::RotateY3D((90.-m_endcapModuleRtilt)*CLHEP::deg);       
       HepGeom::Transform3D EcRot= (m_svcRouting=="inner") ?
	 HepGeom::RotateY3D(90.*CLHEP::deg-m_endcapInclAngle)*HepGeom::RotateZ3D((180.+m_endcapModuleRtilt)*CLHEP::deg):
	 HepGeom::RotateY3D(270.*CLHEP::deg-m_endcapInclAngle)*HepGeom::RotateZ3D((180.+m_endcapModuleRtilt)*CLHEP::deg);

       // Loop over the endcap modules
       int endcapIterator = m_endcapModuleNumber-1;
       for(int iNeg=0; iNeg<m_endcapModuleNumber; iNeg++)
	 {
	   double xPos=0., yPos=0., zPos=0.;
	   
	   GeoFullPhysVol* modulePhys = 0;
	   Identifier idwafer;
	   // Inclined module
	   HepGeom::Transform3D moduleTrans;
	   if(iNeg<m_endcapModuleNumber-m_transitionModuleNumber) {
	     std::ostringstream modName; 
	     modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	     modulePhys = m_endcapModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
	     
	     xPos=endcapModulePos[endcapIterator].x()+2*rOffset;
	     yPos=endcapModulePos[endcapIterator].y();
	     zPos=-endcapModulePos[endcapIterator].z()+2*zOffset;
	     if(bVerbose)std::cout<<"ENDCAP MODULE - ec : "<<m_layer<<" "<<iNeg<<"  "<<zPos<<std::endl;
	     if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;
	     idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	     InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_endcapModuleDesign, 
										   modulePhys, getBasics()->getCommonItems());
	    
	     getBasics()->getDetectorManager()->addDetectorElement(element);
	     moduleTrans = HepGeom::Transform3D(EcRot.getRotation(),HepGeom::Vector3D<double> (xPos,yPos,zPos));

	   }
	   // Transition module
	   else if(m_transitionModuleNumber>0){         
	     std::ostringstream modName; 
	     modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	     modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
	     HepGeom::Transform3D EcRotTrans= (m_svcRouting=="inner") ?
	       HepGeom::RotateY3D(90.*CLHEP::deg-m_transitionTiltAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg):
	       HepGeom::RotateY3D(270.*CLHEP::deg-m_transitionTiltAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);
	     xPos=endcapModulePos[endcapIterator].x();
	     yPos=endcapModulePos[endcapIterator].y();
	     zPos=-endcapModulePos[endcapIterator].z();
	     if(bVerbose)std::cout<<"ENDCAP MODULE - tr : "<<m_layer<<" "<<iNeg<<"  "<<zPos<<std::endl;
	     if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;
	     idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	     InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_transitionModuleDesign, 
										   modulePhys, getBasics()->getCommonItems());
	    
	     getBasics()->getDetectorManager()->addDetectorElement(element);
	     moduleTrans = HepGeom::Transform3D(EcRotTrans.getRotation(),HepGeom::Vector3D<double> (xPos,yPos,zPos));
	   }
	   
	   if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
	   if(bVerbose)std::cout<<"-> place endcap module: "<<iNeg<<" "<<endcapIterator<<"  "<<zPos<<std::endl;
	   
	   std::ostringstream nameTag; 
	   nameTag << "ModuleBrl" << iModuleCmpt;
	   GeoNameTag * tag = new GeoNameTag(nameTag.str());
	   GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrans);
	   
	   // Compute radial minmax sizes 
	   if(!m_minmaxDefined){
	     if(iNeg==0)
	       computeRadiusMinMax(m_localTrf*moduleTrans, 
				   m_endcapModule->Length(),  m_width, m_endcapModule->Thickness(), radiusMin, radiusMax);
	     else if(m_transitionModuleNumber>0&&iNeg==m_endcapModuleNumber-1)
	       computeRadiusMinMax(m_localTrf*moduleTrans, 
				   m_transitionModule->Length(),  m_transitionModule->Width(), m_transitionModule->Thickness(), radiusMin, radiusMax);
	     radiusMin-=.5;
	     radiusMax+=.5;

	     // Lock these values for future use
	     m_minmaxDefined = true;
	     m_rmin = radiusMin;
	     m_rmax = radiusMax;
	   }

	   if(ladderPhys==0){

	     if(m_minmaxDefined){
	       radiusMin = m_rmin; 
	       radiusMax = m_rmax; 
	     }


	     // Evaluate layer radius 
	     HepGeom::Point3D<double> testPoint = m_localTrf*HepGeom::Point3D<double>(0.0, 0.0, 0.0);
	     double radiusLayer = sqrt( (testPoint.x()*testPoint.x())  +   (testPoint.y()*testPoint.y()) );

	     double halfThickness  = 0.5*(radiusMax-radiusMin) +  0.5*(0.5*m_endcapModule->Width()*sin(fabs(m_barrelModuleTilt))); 
	     double halfWidth      = (m_endcapModule->Width()+0.01)/2.0;
	     double shiftThickness = ((0.5*(radiusMax - radiusMin)) - (radiusLayer-radiusMin)) - 0.5*(0.5*m_endcapModule->Width()*sin(fabs(m_barrelModuleTilt)));
	     double shiftWidth     = 0.0;
	    
	     // Take services into account (0 = svc X, 1 = svc Y, 2 = svcBoundBoxHalfThick (x), 3 = svcBoundingBoxHalfWidth (y)
	     // Construct dummy radial services block (never built, dimensions +/-10mm)
	     std::vector<double> svcBounds = ConstructAndPlaceModuleService(nbModuleSvc, -10.0, 10.0 , 0., ladderPhys, "endcap", false);
	     if (svcBounds.size() == 0) {
	       msg(MSG::ERROR) << "Error building dummy service module, Ladder envelope has incorrect size.  Attempting to recover..." << endmsg;
	     }
	     else {
	       if (svcBounds[0] + svcBounds[2] > shiftThickness + halfThickness){
		 double extraThick = svcBounds[0] + svcBounds[2] - (shiftThickness + halfThickness);
		 halfThickness  += 0.5*extraThick+0.5;
		 shiftThickness += 0.5*extraThick+0.5; // Add 0.5mm for safety
	       }
	       else if (svcBounds[0] - svcBounds[2] < shiftThickness - halfThickness) {
		 double extraThick = shiftThickness - halfThickness - (svcBounds[0] - svcBounds[2]);
		 halfThickness  += 0.5*fabs(extraThick)+0.5;
		 shiftThickness -= 0.5*fabs(extraThick)+0.5;
	       }
	       
	       if (svcBounds[1] + svcBounds[3] > shiftWidth + halfWidth){
		 double extraWidth = svcBounds[1] + svcBounds[3] - (shiftWidth + halfWidth);
		 halfWidth  += 0.5*extraWidth + 0.5;
		 shiftWidth += 0.5*extraWidth + 0.5;
	       }
	       else if (svcBounds[1] - svcBounds[3] < shiftWidth - halfWidth) {
		 double extraWidth = shiftWidth - halfWidth - (svcBounds[1] - svcBounds[3]);
		 halfWidth  += fabs(0.5*extraWidth)+0.5;
		 shiftWidth -= fabs(0.5*extraWidth)+0.5;
	       }
	     }
	     
	     GeoBox * box = new GeoBox(halfThickness, halfWidth, m_length/2.);
	     const GeoShape & shiftedBox = (*box) << HepGeom::Translate3D(shiftThickness, shiftWidth, 0.0);
	     m_ladderShape = &shiftedBox;  
	     
	     const GeoMaterial* air = matMgr()->getMaterial("special::Ether");
	     std::ostringstream ladderName; 
	     ladderName<<"_L"<<m_layer;
	     m_theLadder = new GeoLogVol(ladderName.str(),m_ladderShape,air);
	     
	     ladderPhys = new GeoPhysVol(m_theLadder);
	   }

	   ladderPhys->add(tag);
	   ladderPhys->add(new GeoIdentifierTag(iModuleCmpt) );
	   ladderPhys->add(xform);
	   ladderPhys->add(modulePhys);
	   
	   // Service box that containes all the module servcies running on the top of the stave
 	   if(m_IDserviceTool->svcRouteAuto()){
 	     double zInit=(iNeg==0)?-m_length*.495:-endcapModulePosTop[endcapIterator+1].z();
 	     double zFinal=-endcapModulePosTop[endcapIterator].z();
 	     BuildAndPlaceModuleService(nbModuleSvc, zInit, zFinal, m_barrelModuleZeroTilt, ladderPhys, "endcap");
 	   }

	   // Now store the xform by identifier:
	   getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);
	   
	   // Place the endcap services (behind the module)
	   if(foamEndcap&&iNeg<m_endcapModuleNumber-1) {
	     std::ostringstream nameTag; 
	     nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	     GeoNameTag * tag = new GeoNameTag(nameTag.str());
	     const HepGeom::Transform3D foamTrf = HepGeom::TranslateX3D(xEndcapFoamShift+(buildDeadEdges? 0.:m_barrelModule->ThicknessP()))
	       *HepGeom::TranslateZ3D(zPos-zEndcapFoamShift)*trfFoam;
	     GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	     ladderPhys->add(tag);
	     ladderPhys->add(xformFoam);
	     ladderPhys->add(foamEndcap);
	   }
	   else if(foamTrans){
	     std::ostringstream nameTag; 
	     nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	     GeoNameTag * tag = new GeoNameTag(nameTag.str());
	     const HepGeom::Transform3D foamTrf = HepGeom::TranslateY3D(yPos)*HepGeom::TranslateX3D(xTransFoamShift+(buildDeadEdges?0.:m_barrelModule->ThicknessP()))
	       *HepGeom::TranslateZ3D(zPos-zTransFoamShift)*trfFoam;
	     GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	     ladderPhys->add(tag);
	     ladderPhys->add(xformFoam);
	     ladderPhys->add(foamTrans);
	   }
	   
	   if(bVerbose){
	     std::cout<<"MODULE SERVICES ec neg  "<<iNeg <<" : ";  for(int i=0; i<(int)nbModuleSvc.size(); i++) std::cout<<nbModuleSvc[i]<<" "; std::cout<<std::endl;
	   }

	   // Decrement the number of module services 
	   if(iNeg<m_endcapModuleNumber-m_transitionModuleNumber) 
	     nbModuleSvc[endcapModuleIndex]--;
	   else if(m_transitionModuleNumber>0)
	     nbModuleSvc[transModuleIndex]--;
	     
	   iModuleCmpt++;
	   endcapIterator--;
	 }
     }

  //------------------------------------------------------
  // Place the barrel modules
  //------------------------------------------------------
   double zOffset = (m_sector%2==0) ? 0.5*m_barrelModuleZoffset : -0.5*m_barrelModuleZoffset;
   double zNegStavePos= (m_barrelModuleNumber*m_barrelModule->Length()+m_barrelModuleGap*(m_barrelModuleNumber-1))*.5 + zOffset;
   zNegStavePos *=-1.;

   double zpos = zNegStavePos;
   if(bVerbose)std::cout<<"BARREL BEGIN : "<<zpos<<std::endl;

   if(ladderPhys==0){
     
     if(m_minmaxDefined){
       radiusMin = m_rmin; 
       radiusMax = m_rmax; 
     }
          
     // Evaluate layer radius 
     HepGeom::Point3D<double> testPoint = m_localTrf*HepGeom::Point3D<double>(0.0, 0.0, 0.0);
     double radiusLayer = sqrt( (testPoint.x()*testPoint.x())  +   (testPoint.y()*testPoint.y()) );
     
     double halfThickness  = 0.5*(radiusMax-radiusMin) ; 
     double halfWidth      = (m_barrelModule->Width()+0.01)/2.0; //TODO: safety offset hardcoded, find a better solution 
     double shiftThickness = ((0.5*(radiusMax - radiusMin)) - (radiusLayer-radiusMin)) ;
     double shiftWidth     = 0.0;
     
     // Take services into account (0 = svc X, 1 = svc Y, 2 = svcBoundBoxHalfThick (x), 3 = svcBoundingBoxHalfWidth (y)
     // Construct dummy radial services block (never built, dimensions +/-10mm)
     std::vector<double> svcBounds = ConstructAndPlaceModuleService(nbModuleSvc, -10.0, 10.0 , 0., ladderPhys, "barrel", false);
     if (svcBounds.size() == 0) {
       msg(MSG::ERROR) << "Error building dummy service module, Ladder envelope has incorrect size.  Attempting to recover..." << endmsg;
     }
     else {
       if (svcBounds[0] + svcBounds[2] > shiftThickness + halfThickness){
	 double extraThick = svcBounds[0] + svcBounds[2] - (shiftThickness + halfThickness);
	 halfThickness  += 0.5*extraThick+0.5;
	 shiftThickness += 0.5*extraThick+0.5; // Add 0.5mm for safety
       }
       else if (svcBounds[0] - svcBounds[2] < shiftThickness - halfThickness) {
	 double extraThick = shiftThickness - halfThickness - (svcBounds[0] - svcBounds[2]);
	 halfThickness  += 0.5*fabs(extraThick)+0.5;
	 shiftThickness -= 0.5*fabs(extraThick)+0.5;
       }
       
       if (svcBounds[1] + svcBounds[3] > shiftWidth + halfWidth){
	 double extraWidth = svcBounds[1] + svcBounds[3] - (shiftWidth + halfWidth);
	 halfWidth  += 0.5*extraWidth + 0.5;
	 shiftWidth += 0.5*extraWidth + 0.5;
       }
       else if (svcBounds[1] - svcBounds[3] < shiftWidth - halfWidth) {
	 double extraWidth = shiftWidth - halfWidth - (svcBounds[1] - svcBounds[3]);
	 halfWidth  += fabs(0.5*extraWidth)+0.5;
	 shiftWidth -= fabs(0.5*extraWidth)+0.5;
	       }
     }
     
     GeoBox * box = new GeoBox(halfThickness, halfWidth, m_length/2.);
     const GeoShape & shiftedBox = (*box) << HepGeom::Translate3D(shiftThickness, shiftWidth, 0.0);
     m_ladderShape = &shiftedBox;  
     
     const GeoMaterial* air = matMgr()->getMaterial("special::Ether");
     m_theLadder = new GeoLogVol("Ladder",m_ladderShape,air);
     
     ladderPhys = new GeoPhysVol(m_theLadder);
   }

   double zPosFinal;
   int iBarrelModuleCmpt = 0;
   for(int iBrl = 0; iBrl < m_barrelModuleNumber; iBrl++) 
     {

       // --------------------------------------------------------------------------
       // BARREL TRANSITION REGION
       // --------------------------------------------------------------------------
       if(iBrl==0&&m_transitionModuleNumber>0){

	 double xPosTrans = 0;
	 double yPosTrans = (m_transitionModule->Width()*.5);
	 double zPosTrans = zNegStavePos-m_barrelModuleGap-m_transitionModule->Length()*.5;
	 
	 CLHEP::Hep3Vector modulepos(xPosTrans, yPosTrans, zPosTrans);
	 CLHEP::HepRotation rm; rm.rotateY(0.);
	 
	 if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
	 if(bVerbose)std::cout<<"xx > module barrel transition Z<0 "<<iModuleCmpt<<" "<<iBrl<<"/"<<m_barrelModuleNumber<<std::endl;

	 std::ostringstream modName; 
	 modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	 GeoFullPhysVol* modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());

	 if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;
	 Identifier idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	 InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_transitionModuleDesign, 
									       modulePhys, getBasics()->getCommonItems());
	
	 getBasics()->getDetectorManager()->addDetectorElement(element);

	 std::ostringstream nameTag; 
	 nameTag << "ModuleBrl" << iModuleCmpt;
	 GeoNameTag * tag = new GeoNameTag(nameTag.str());

	 HepGeom::Transform3D moduleTrans = (m_svcRouting=="inner") ?
	   HepGeom::RotateZ3D(180.*CLHEP::deg)*HepGeom::Transform3D(rm,modulepos) : HepGeom::Transform3D(rm,modulepos);
	 GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrans);
	 ladderPhys->add(tag);
	 ladderPhys->add(new GeoIdentifierTag(iModuleCmpt));
	 ladderPhys->add(xform);
	 ladderPhys->add(modulePhys);

	 // Now store the xform by identifier:
	 getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

	 // Increment module counting parametr
	 iModuleCmpt++;
	 iBarrelModuleCmpt++;
       }
    
       // --------------------------------------------------------------------------
       // BARREL CENTRAL REGION
       // --------------------------------------------------------------------------

       zpos+=m_barrelModule->Length()*.5;

       if(bVerbose)std::cout<<"BARREL MODULE : "<<iBrl<<"  "<<zpos<<std::endl;
       
       //       CLHEP::Hep3Vector modulepos(xpos+xposShift,yposShift,zpos);
       CLHEP::Hep3Vector modulepos(0.,0.,zpos);
       //
       //  Rotate if module is inclined.
       //
       CLHEP::HepRotation rm;
       //       rm.rotateY(gmt_mgr->PixelModuleAngle()*gmt_mgr->PixelModuleAngleSign(etaModule) );
       rm.rotateZ(m_barrelModuleTilt);

       //
       // Place the Module
       //

       if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
       if(bVerbose)std::cout<<"xx > module barrel planar "<<iModuleCmpt<<" "<<iBrl<<"/"<<m_barrelModuleNumber<<std::endl;
       //       m_barrelModule->setIdentifierFlags(0, m_layer, m_sector, iModuleCmpt);
       std::ostringstream modName; 
       modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
       GeoFullPhysVol* modulePhys = m_barrelModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
       
       if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;
       Identifier idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
       InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_barrelModuleDesign, 
									     modulePhys, getBasics()->getCommonItems());
      
       getBasics()->getDetectorManager()->addDetectorElement(element);

       std::ostringstream nameTag; 
       nameTag << "ModuleBrl" << iModuleCmpt;
       GeoNameTag * tag = new GeoNameTag(nameTag.str());
       HepGeom::Transform3D barrelTrans = (m_svcRouting=="inner") ? 
	   HepGeom::RotateZ3D(180.*CLHEP::deg)*HepGeom::Transform3D(rm,modulepos) : HepGeom::Transform3D(rm,modulepos);
       GeoAlignableTransform* xform = new GeoAlignableTransform(barrelTrans);
       ladderPhys->add(tag);
       ladderPhys->add(new GeoIdentifierTag(iModuleCmpt));
       ladderPhys->add(xform);
       ladderPhys->add(modulePhys);

       if(bVerbose){
	 std::cout<<"MODULE SERVICES brl "<<iBrl <<" : ";  for(int i=0; i<(int)nbModuleSvc.size(); i++) std::cout<<nbModuleSvc[i]<<" "; std::cout<<std::endl;
       }

       // Service box that containes all the module servcies running on the top of the stave
       if(m_IDserviceTool->svcRouteAuto()){
	 // Barrel module services
	 double z0 = zpos-m_barrelModule->Length()*.5;
	 double z1 = zpos+m_barrelModule->Length()*.5;
	 if(iBrl==0) z0 -= m_gapPlanarStave ;
	 if(iBrl==m_barrelModuleNumber-1) z1 += m_gapPlanarStave;

	 BuildAndPlaceModuleService(nbModuleSvc, z0, z1 ,m_barrelModuleTilt, ladderPhys, "barrel");
	 
	 // Barrel module to the first endcap module
	
	 if((iBrl==0||iBrl==m_barrelModuleNumber-1)&&m_endcapModuleNumber>0){
	   double z0 = 0., z1=0.;
	   if(iBrl==0) { z0 = -endcapModulePos[0].z(); z1 = zpos - m_barrelModule->Length()*.5- m_gapPlanarStave + m_moduleSvcThickness; }
	   else if(iBrl==m_barrelModuleNumber-1) {z0 = zpos + m_barrelModule->Length()*.5+ m_gapPlanarStave-m_moduleSvcThickness; z1= endcapModulePos[0].z(); }

	   if(iBrl==0 && (z1 - m_moduleSvcThickness) < -m_barrelZMax) z1 = -m_barrelZMax + m_moduleSvcThickness;
	   if(iBrl==m_barrelModuleNumber-1 && (z0 + m_moduleSvcThickness) > m_barrelZMax) z0 = m_barrelZMax - m_moduleSvcThickness;

    	  
	   BuildAndPlaceModuleService(nbModuleSvc, z0, z1 , m_barrelModuleZeroTilt, ladderPhys, "endcap");
	   
	   // In case endcap modules are shifted in R
	   if(m_EndcapGlobalRshift){
	     double z0 = 0., z1=0.;
	     if(iBrl==0) { z1 = zpos - m_barrelModule->Length()*.5- m_gapPlanarStave + m_moduleSvcThickness; z0 = z1 - m_moduleSvcThickness; }
	     else if(iBrl==m_barrelModuleNumber-1) { z0 = zpos + m_barrelModule->Length()*.5+ m_gapPlanarStave-m_moduleSvcThickness; z1 = z0 + m_moduleSvcThickness;}

	     if(iBrl==0 && z0 < -m_barrelZMax) { z0 = -m_barrelZMax; z1 = z0+m_moduleSvcThickness; }
	     else if(iBrl==m_barrelModuleNumber-1 && z1 > m_barrelZMax) { z1 = m_barrelZMax; z0 = z1-m_moduleSvcThickness;}

	    
	     BuildAndPlaceModuleService(nbModuleSvc, z0, z1 , m_barrelModuleZeroTilt, ladderPhys, "radial");
	   }
	 }

       }
       
       // Increment/decrement the number of module which services run on the top of the stave
       nbModuleSvc[brlModuleIndex]+=brlModuleCmpt;
       if(nbModuleSvc[brlModuleIndex]==0)
	 { nbModuleSvc[brlModuleIndex]=1; brlModuleCmpt=1; }
       
       if(!m_minmaxDefined&&iBrl==0){
	 
	 computeRadiusMinMax(m_localTrf*HepGeom::Transform3D(rm,modulepos), 
			     m_barrelModule->Length(),  m_barrelModule->Width(), m_barrelModule->Thickness(), radiusMin, radiusMax);

	 // Lock the values
	 m_minmaxDefined = true;
	 m_rmin = radiusMin;
	 m_rmax = radiusMax;
       }
       
       if(iBrl==0&&ladderPhys==0){

	 if(m_minmaxDefined){
	   radiusMin = m_rmin; 
	   radiusMax = m_rmax; 
	 }
	 
	 // Add safety to width
	 double halfThickness = 0.5*(radiusMax-radiusMin);
	 double shift = 0.5*(radiusMax-radiusMin)-halfThickness;
	 GeoBox * box = new GeoBox(halfThickness, (m_width + 0.01)/2., m_length/2.);
	 const GeoShape & shiftedBox = (*box) << HepGeom::TranslateX3D(shift);
	 m_ladderShape = &shiftedBox;  
	 
	 const GeoMaterial* air = matMgr()->getMaterial("special::Ether");
	 m_theLadder = new GeoLogVol("Ladder",m_ladderShape,air);
	 
	 ladderPhys = new GeoPhysVol(m_theLadder);
       }

       // Now store the xform by identifier:
       getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

       zpos+=(m_barrelModule->Length()*.5+m_barrelModuleGap);
       iModuleCmpt++;
       iBarrelModuleCmpt++;
       if(iModuleCmpt==0&&m_barrelModuleNumber%2==0) iModuleCmpt++;

       // --------------------------------------------------------------------------
       // BARREL TRANSITION REGION
       // --------------------------------------------------------------------------
       if((iBrl==m_barrelModuleNumber-1)&&m_transitionModuleNumber>0){

	 double xPosTrans = 0;
	 double yPosTrans = (m_transitionModule->Width()*.5);
	 double zPosTrans = -zNegStavePos+m_barrelModuleGap+m_transitionModule->Length()*.5;

	 CLHEP::Hep3Vector modulepos(xPosTrans, yPosTrans, zPosTrans);
	 CLHEP::HepRotation rm; rm.rotateY(0.);

	 if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
	 if(bVerbose)std::cout<<"xx > module barrel transition Z>0 "<<iModuleCmpt<<" "<<iBrl<<"/"<<m_barrelModuleNumber<<std::endl;
	
	 std::ostringstream modName; 
	 modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	 GeoFullPhysVol* modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
	 
	 if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;
	 Identifier idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	 InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_transitionModuleDesign, 
									       modulePhys, getBasics()->getCommonItems());
	 //	 getBasics()->getDetectorManager()->addDesign(m_barrelModuleDesign);
	 getBasics()->getDetectorManager()->addDetectorElement(element);

	 std::ostringstream nameTag; 
	 nameTag << "ModuleBrl" << iModuleCmpt;
	 GeoNameTag * tag = new GeoNameTag(nameTag.str());
	 HepGeom::Transform3D moduleTrans = (m_svcRouting=="inner") ? 
	   HepGeom::RotateY3D(180.*CLHEP::deg)*HepGeom::Transform3D(rm,modulepos) : HepGeom::Transform3D(rm,modulepos);
	 GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrans);
	 ladderPhys->add(tag);
	 ladderPhys->add(new GeoIdentifierTag(iModuleCmpt));
	 ladderPhys->add(xform);
	 ladderPhys->add(modulePhys);
	 
	 // Now store the xform by identifier:
	
	 getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

	 iModuleCmpt++;
	 iBarrelModuleCmpt++;

	 zPosFinal = zPosTrans+m_transitionModule->Length()*.5;
       }
       else
	 zPosFinal = zpos+m_barrelModule->Length()*.5;
     }


   if(bVerbose)std::cout<<"BARREL END "<<m_layer<<" : "<<-zNegStavePos<<"  "<<zPosFinal<<std::endl;

  //------------------------------------------------------
  // Place the endcap modules (Z>0)
  //------------------------------------------------------
  if(m_endcapModuleNumber>0)
    {
      // Increment/decrement the number of module which services run on the top of the stave
      nbModuleSvc[brlModuleIndex]--;

     
      GeoPixelEndcapModuleSvcRef foamBuilder(getBasics(), m_width, m_layer, m_endcapModule, m_transitionModule, m_endcapInclAngle, m_transitionTiltAngle);
      GeoPhysVol* foamEndcap = dynamic_cast<GeoPhysVol*>(foamBuilder.getEndcapFoam());

      double zEndcapFoamShift = foamBuilder.getEndcapZshift(); 
      zEndcapFoamShift += (m_svcRouting=="inner") ? -0.25 : 0.25;

      GeoPhysVol* foamTrans = dynamic_cast<GeoPhysVol*>(foamBuilder.getTransFoam());
      double zTransFoamShift = foamBuilder.getTransZshift();
      zTransFoamShift += (m_svcRouting=="inner") ? -0.25 : 0.25;

      double xEndcapFoamShift = m_EndcapGlobalRshift - m_endcapModule->Length()*cos(m_endcapInclAngle);
      double xTransFoamShift = xEndcapFoamShift  ;

      if (foamEndcap && !foamTrans) {
	foamTrans = foamEndcap;
	zTransFoamShift = zEndcapFoamShift;
      }  

      HepGeom::Transform3D trfFoam = (m_svcRouting=="inner") ? 
      HepGeom::RotateX3D(180.*CLHEP::deg)*HepGeom::RotateZ3D(270.*CLHEP::deg)*HepGeom::RotateY3D((90.-m_endcapModuleRtilt)*CLHEP::deg):      
      HepGeom::RotateX3D(180.*CLHEP::deg)*HepGeom::RotateZ3D(270.*CLHEP::deg)*HepGeom::RotateY3D((90.+m_endcapModuleRtilt)*CLHEP::deg);       
      HepGeom::Transform3D EcRot_pos= (m_svcRouting=="inner") ?
      HepGeom::RotateY3D(270.*CLHEP::deg+m_endcapInclAngle)*HepGeom::RotateZ3D((180.+m_endcapModuleRtilt)*CLHEP::deg):
      HepGeom::RotateY3D(90.*CLHEP::deg+m_endcapInclAngle)*HepGeom::RotateZ3D((180.+m_endcapModuleRtilt)*CLHEP::deg);

      int endcapIterator = 0;
      Identifier idwafer;
      HepGeom::Transform3D moduleTrans;
      for(int iPos=0; iPos<m_endcapModuleNumber; iPos++)
	{

	  double xPos=0., yPos=0., zPos=0.;
	  GeoFullPhysVol* modulePhys = 0;
	  if(iPos>=m_transitionModuleNumber) {
	  
	    std::ostringstream modName; 
	    modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	    modulePhys = m_endcapModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
	    xPos=endcapModulePos[iPos].x();
	    yPos=endcapModulePos[iPos].y();
	    zPos=endcapModulePos[iPos].z();

	    if(bVerbose)std::cout<<"ENDCAP MODULE : ec "<<m_layer<<" "<<iPos<<"  "<<zPos<<std::endl;
	    if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;

	    idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	    InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_endcapModuleDesign, 
										  modulePhys, getBasics()->getCommonItems());
	    
	    getBasics()->getDetectorManager()->addDetectorElement(element);
	    moduleTrans = HepGeom::Transform3D(EcRot_pos.getRotation(),HepGeom::Vector3D<double> (xPos,yPos,zPos));
	  }
	  else if(m_transitionModuleNumber>0) {
	   
	    std::ostringstream modName; 
	    modName<<"_"<<m_layer<<"_"<<m_sector<<"_"<<iModuleCmpt;
	    modulePhys = m_transitionModule->Build(0, m_layer, m_sector, iModuleCmpt, inclinedModTag , modName.str());
	    HepGeom::Transform3D EcRot_posTrans = (m_svcRouting=="inner") ?
	      HepGeom::RotateY3D(270.*CLHEP::deg+m_transitionTiltAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg):
	      HepGeom::RotateY3D(90.*CLHEP::deg+m_transitionTiltAngle)*HepGeom::RotateZ3D(180.*CLHEP::deg);
	    xPos=endcapModulePos[iPos].x();
	    yPos=endcapModulePos[iPos].y();
	    zPos=endcapModulePos[iPos].z();
	    if(bVerbose)std::cout<<"ENDCAP MODULE - tr "<<m_layer<<" "<<iPos<<"  "<<zPos<<std::endl;
	    if(bVerbose)std::cout<<"wafer id : "<<0<<" "<< m_layer<<" "<< m_sector<<" "<< iModuleCmpt<<std::endl;

	    idwafer = getBasics()->getIdHelper()->wafer_id(0, m_layer, m_sector, iModuleCmpt);
	    InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, m_transitionModuleDesign, 
										  modulePhys, getBasics()->getCommonItems());
	   
	    getBasics()->getDetectorManager()->addDetectorElement(element);
	    moduleTrans = HepGeom::Transform3D(EcRot_posTrans.getRotation(),HepGeom::Vector3D<double> (xPos,yPos,zPos));

	  }

	  // Decrement the number of module services 
	  if(iPos>=m_transitionModuleNumber)
	    nbModuleSvc[endcapModuleIndex]++;
	  else if(m_transitionModuleNumber>0)
	    nbModuleSvc[transModuleIndex]++;

	  if(bVerbose)std::cout<<"*******************************************************************"<<std::endl;
	  if(bVerbose)std::cout<<"-> place endcap module : "<<iPos<<" "<<endcapIterator<<"  "<<zPos<<std::endl;

	  std::ostringstream nameTag; 
	  nameTag << "ModuleBrl" << iModuleCmpt;
	  GeoNameTag * tag = new GeoNameTag(nameTag.str());
	  GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrans);
	  ladderPhys->add(tag);
	  ladderPhys->add(new GeoIdentifierTag(iModuleCmpt) );
	  ladderPhys->add(xform);
	  ladderPhys->add(modulePhys);

	  if(bVerbose){
	    std::cout<<"MODULE SERVICES ec pos  "<<iPos <<" : ";  for(int i=0; i<(int)nbModuleSvc.size(); i++) std::cout<<nbModuleSvc[i]<<" "; std::cout<<std::endl;
	  }

	  // Now store the xform by identifier:
	  getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);

	  // Service box that containes all the module servcies running on the top of the stave
 	  if(m_IDserviceTool->svcRouteAuto()){
 	    double zInit=endcapModulePosTop[iPos].z();
 	    double zFinal=(iPos<m_endcapModuleNumber-1)?endcapModulePosTop[iPos+1].z():m_length*.495;
 	    BuildAndPlaceModuleService(nbModuleSvc, zInit, zFinal , m_barrelModuleZeroTilt, ladderPhys, "endcap");
 	  }

	  if(foamEndcap&&iPos>0) {
	    std::ostringstream nameTag; 
	    nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	    GeoNameTag * tag = new GeoNameTag(nameTag.str());
	    const HepGeom::Transform3D foamTrf = HepGeom::TranslateX3D(xEndcapFoamShift+(buildDeadEdges?0.:m_barrelModule->ThicknessP()))
	      *HepGeom::TranslateZ3D(zPos+zEndcapFoamShift)*trfFoam;
	    GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	    ladderPhys->add(tag);
	    ladderPhys->add(xformFoam);
	    ladderPhys->add(foamEndcap);
	  }
	  else if(foamTrans){
	    std::ostringstream nameTag; 
	    nameTag << "EndcapFoam_L" <<m_layer<<"M"<<iModuleCmpt;
	    GeoNameTag * tag = new GeoNameTag(nameTag.str());
	    const HepGeom::Transform3D foamTrf = HepGeom::TranslateY3D(yPos)*HepGeom::TranslateX3D(xTransFoamShift+(buildDeadEdges?0.:m_barrelModule->ThicknessP()))
	      *HepGeom::TranslateZ3D(zPos+zTransFoamShift)*trfFoam;
	    GeoAlignableTransform* xformFoam = new GeoAlignableTransform(foamTrf);
	    ladderPhys->add(tag);
	    ladderPhys->add(xformFoam);
	    ladderPhys->add(foamTrans);
	  }

	  iModuleCmpt++;
	  endcapIterator++;
	}
    }
  
  if(bVerbose) std::cout<<"ALPINE ladder module number : "<<iModuleCmpt<<std::endl;
  
  //Add the TMT or other stave support
  if (m_staveSupport) {
    ladderPhys->add(new GeoTransform(m_staveSupport->transform()));
    if(m_staveSupport->getPhysVol())
      ladderPhys->add(m_staveSupport->getPhysVol()); 
  }
  
  if(!m_minmaxDefined){
    m_rmin = radiusMin;
    m_rmax = radiusMax;
    m_minmaxDefined = true;
   }
  
  return ladderPhys;

}

std::vector<HepGeom::Point3D<double> > GeoPixelLadderInclRef::DecodeEndCapModulePositions(double xPos, double yPos, 
											  std::vector<double> sValues0, std::vector<double> sValues1)
{
  std::vector<HepGeom::Point3D<double> > vecPos;

  for(int i=0; i<(int)sValues0.size(); i++)
      vecPos.push_back(HepGeom::Point3D<double> (xPos,yPos,sValues0[i]));
  for(int i=0; i<(int)sValues1.size(); i++)
      vecPos.push_back(HepGeom::Point3D<double> (xPos,yPos,sValues1[i]));

  return vecPos;
}



void GeoPixelLadderInclRef::computeRadiusMinMax(HepGeom::Transform3D trf, double length, double width, double thick, double &rMin, double &rMax)
{

  std::vector<HepGeom::Point3D<double> >corners;
  std::vector<HepGeom::Point3D<double> >corners_trf;
  
  double halfLength=length*.5;
  double halfWidth=width*.5;
  double halfThick=thick*.5;
       
  corners.push_back( HepGeom::Point3D<double> (-halfThick, -halfWidth, -halfLength));
  corners.push_back( HepGeom::Point3D<double> (-halfThick, halfWidth, -halfLength));
  corners.push_back( HepGeom::Point3D<double> (halfThick, halfWidth, -halfLength));
  corners.push_back( HepGeom::Point3D<double> (halfThick, -halfWidth, -halfLength));

  corners.push_back( HepGeom::Point3D<double> (-halfThick, -halfWidth, halfLength));
  corners.push_back( HepGeom::Point3D<double> (-halfThick, halfWidth, halfLength));
  corners.push_back( HepGeom::Point3D<double> (halfThick, halfWidth, halfLength));
  corners.push_back( HepGeom::Point3D<double> (halfThick, -halfWidth, halfLength));

  double vMin=999999.;
  double vMax=-999999.;
  
  for(unsigned int i=0; i<corners.size(); i++)
    {
      HepGeom::Point3D<double> p = trf*corners[i];
      corners_trf.push_back(p);
    }


  for(int i=0; i<(int)corners.size(); i++)
    {
      HepGeom::Point3D<double> p = corners_trf[i];
      double r = (p.x()*p.x()+p.y()*p.y());
      vMin = std::min(vMin,r);
      vMax = std::max(vMax,r);

    }

  vMin=sqrt(vMin); vMax=sqrt(vMax);
  if(vMin<rMin) rMin=vMin;
  if(vMax>rMax) rMax=vMax;

  msg(MSG::DEBUG)<<"Compute ladder rminmax : "<<rMin<<" "<<rMax<<endmsg;

}



GeoPhysVol* GeoPixelLadderInclRef::createServiceVolume(double length, double thick, double width, std::vector<int> nModuleSvc)

{
  // Place the stave module service (on the top of the stave support...)
  GeoBox * svcBox = new GeoBox(thick, width, length);

  msg(MSG::DEBUG) <<"Barrel module service material for layer  "<<m_layer <<" :  size WxTxL "<<width<<" "<<thick<<" "<<length<<"     # modules ";  


  if(length<thick){
    msg(MSG::DEBUG) <<"Swapping thickness with length for mass calculation"<<endmsg;
    double tmp = length;
    length=thick;
    thick=tmp;
  }

  GeoMaterial* svcMat = 0;  // do not redefine material if already done

  //if ( m_ladderType>0 ) {
  std::string matNameStave = m_IDserviceTool->getLayerStaveModuleMaterialName(m_layer, m_ladderType, nModuleSvc);   // material name stored in PixelServicesTool
  msg(MSG::DEBUG) <<"Barrel module service material  : "<<matNameStave<<"  "<<endmsg;  
  std::ostringstream wg_matNameStave;  
  wg_matNameStave<<matNameStave<<"_"<<m_svcMaterialCmpt;  
  msg(MSG::DEBUG) <<"Barrel module weighted service material : "<<matNameStave<<"  "<<wg_matNameStave.str()<<"   / sector : "<<m_sector<<endmsg;
  if(matNameStave!="None") {
    if(matMgr()->hasMaterial(wg_matNameStave.str()))
      svcMat = const_cast < GeoMaterial* > (matMgr()->getMaterial(wg_matNameStave.str())); // material already defined
    else
      svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterialForVolumeLength(matNameStave, svcBox->volume(), 2*length, wg_matNameStave.str()));  // define material
  }
  //}

  if (!svcMat) {
    std::string matName = m_IDserviceTool->getLayerModuleMaterialName(m_layer ,nModuleSvc);   // material name stored in PixelServicesTool (material are built there)
    msg(MSG::DEBUG) <<"Barrel module service material  : "<<matName<<"  "<<endmsg;
    
    std::ostringstream wg_matName;  
    wg_matName<<matName<<"_"<<m_svcMaterialCmpt;
    
    msg(MSG::DEBUG) <<"Barrel module weighted service material : "<<matName<<"  "<<wg_matName.str()<<"   / sector : "<<m_sector<<endmsg;

    if(matName=="None") return 0;
    if (matMgr()->hasMaterial(wg_matName.str()))
      svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterial(wg_matName.str()));   // material already defined
    else {
      double fudge = m_IDserviceTool->getMaterialFudgeModuleSvc(m_layer);
      svcMat = const_cast<GeoMaterial*>(matMgr()->getMaterialForVolumeLength(matName, svcBox->volume(), 2*length, wg_matName.str(),fudge));  // define material
    }
  }
  
  if (svcMat==0)  msg(MSG::DEBUG)<<"service material not defined for layer, sector: "<< m_layer<<":"<<m_sector<<endmsg;
  msg(MSG::DEBUG)<<"Material density : "<<svcMat->getDensity()<<endmsg;

  GeoLogVol* svcLog = new GeoLogVol("ModuleSvc",svcBox,svcMat);
  GeoPhysVol* svcPhys = new GeoPhysVol(svcLog);

  m_svcMaterialCmpt++;  
  return svcPhys;
}
 

void GeoPixelLadderInclRef:: BuildAndPlaceModuleService(std::vector<int> moduleNumber, double zInit, double zFinal, double locTilt, GeoPhysVol* volPhys, std::string type)
{
  std::vector<double> UselessValues = ConstructAndPlaceModuleService(moduleNumber, zInit, zFinal, locTilt,  volPhys, type, true);
}

std::vector<double> GeoPixelLadderInclRef:: ConstructAndPlaceModuleService(std::vector<int> moduleNumber, double zInit, double zFinal, double locTilt, GeoPhysVol* volPhys, std::string type, bool build)
{
  // Guarantee a return vector
  std::vector<double> svcBoundingBox;
  double svcLength = zFinal - zInit;
  if(svcLength<=0) return svcBoundingBox;

  double rOffset = (m_sector%2==0) ? -0.5*m_endcapModuleROffset : 0.5*m_endcapModuleROffset;
  double m_EndcapGlobalRshift = m_endcapModuleRshift+rOffset;

  // Check for errors with use of "None" stave support type
  if (m_staveDBHelpers[0]->getStaveSupportType() == "None") {
    if (m_layer == 0) 
      msg(MSG::WARNING) << "Cannot place service modules! No stave support on Layer 0. Check your XML!" << endreq;
    else{
      if (m_staveDBHelpers[1]->getStaveSupportType() != "Longeron")
	msg(MSG::ERROR) << "Cannot build service modules! Layer " << m_layer << " has no stave support (Standard or Longeron). Check your XML!" << endreq;
    }
  }
   
  // Standard Stave built on this layer, index [0]
  if (m_staveDBHelpers[0]->getStaveSupportType() == "Standard") {
    double delta = .75;    // delta defines the relative width of svc box
    double xshift = m_staveDBHelpers[0]->getServiceOffsetX();
    double ec_xshift = m_staveDBHelpers[0]->getServiceECOffsetX();

    double svcHalfThick = m_moduleSvcThickness*.5-0.001;
    double svcHalfWidth = m_barrelModule->Width()*delta*.5;
    double svcOffset = m_barrelModule->Width()*.5-svcHalfWidth;
    if(locTilt<0.) svcOffset = -svcOffset;
    double xPos_svc = 0.;
    double xPos_brl = 0.;
    double xPos_ec = 0.;
    if ( m_svcRouting=="outer" ){
      xPos_brl = m_staveSupport->thicknessP_barrel() + m_moduleSvcThickness*.5 + xshift;
      xPos_ec = m_staveSupport->thicknessP_endcap() + m_moduleSvcThickness*.5 + xshift + ec_xshift;
    } else {
      double maxTiltedModuleThick=m_endcapModule->Length()*cos(m_endcapInclAngle);
      xPos_brl = -m_staveSupport->thicknessP_barrel() - m_moduleSvcThickness*.5 - xshift;
      xPos_ec = m_EndcapGlobalRshift - maxTiltedModuleThick - m_moduleSvcThickness*.5 - xshift - ec_xshift;
    }      

    if (type=="barrel") xPos_svc = xPos_brl;
    else if (type=="endcap") xPos_svc = xPos_ec;
    else if (type=="radial") {
      xPos_svc = (xPos_brl + xPos_ec) * 0.5 + svcHalfWidth*fabs(sin(m_staveTmp->b_tilt))*0.5 + svcHalfThick*fabs(cos(m_staveTmp->b_tilt))*0.5;
      svcHalfThick = fabs(xPos_brl - xPos_ec)*0.5 - svcHalfWidth*fabs(sin(m_staveTmp->b_tilt))*0.5 - svcHalfThick*(1+fabs(cos(m_staveTmp->b_tilt))*0.5);
      if (svcHalfThick<1.) return svcBoundingBox;
    }

    // Service location and dimensions
    svcBoundingBox.push_back(xPos_svc);
    svcBoundingBox.push_back(-svcOffset);
    svcBoundingBox.push_back(svcHalfWidth*sin(locTilt) + svcHalfThick*cos(locTilt));
    svcBoundingBox.push_back(svcHalfWidth*cos(locTilt) + svcHalfThick*sin(locTilt));
    
    if (build) {
      GeoPhysVol* svcPhys = createServiceVolume(svcLength*.5, svcHalfThick, svcHalfWidth, moduleNumber);
      if (!svcPhys) return svcBoundingBox;

      GeoTransform* xform_svc = new GeoTransform(HepGeom::Translate3D(xPos_svc, -svcOffset, (zInit+zFinal)*.5)*HepGeom::RotateZ3D(locTilt));
      volPhys->add(xform_svc);
      volPhys->add(svcPhys);
    }
  }

  // ==========================
  // Building longeron services
  // ==========================

  // If svc routing inner, services follow longeron from whose properties were defined on previous layer
  // If svc routing outer, then it goes along the inside edge of the longeron built by the current layer
  double svcRouteDir = (m_svcRouting == "inner") ? 1.0 : -1.0;
  unsigned int index = (svcRouteDir > 0) ? 1 : 0;

  if (m_staveDBHelpers[index]->getStaveSupportType() == "Longeron") {
    // deltaphi is calculated from n sectors on layer which "owns" the longeron, find nsectors for this layer
    // Apologies, but I can find no better way of getting this info
    int owningLayer = m_staveDBHelpers[index]->getOwningLayer();
    int nSectors = 0;
    if      (owningLayer == m_layer)   nSectors = m_nSectors;
    else if (owningLayer == m_layer-1) nSectors = m_nSectorsLastLayer;
    else if (owningLayer == m_layer+1) nSectors = m_nSectorsNextLayer;
    else {
      msg(MSG::ERROR) << "Could not build Longeron Services! Expected owning layer ID to be between " << m_layer-1 << " and " << m_layer+1 << ", actual owning layer = " << owningLayer << endreq;
      return svcBoundingBox;
    }
    if (nSectors == 0)  {
      msg(MSG::ERROR) << "Could not build Longeron Services! Longeron phi comes from layer with 0 sectors!" << owningLayer << endreq;
      return svcBoundingBox;
    }
    
    // Find closest longeron to current sector phi
    double deltaPhi = 360.*CLHEP::deg/(double)nSectors;
    double sectorPhi = (m_sector* (360.*CLHEP::deg/(double)m_nSectors)  ) + m_phiOfStaveZero;

    int    longeronIndex     = 0;
    double phiOffsetLongeron = 9999999;

    int    testLongeronIndex = 0;
    double testLongeronPhi   = (m_staveDBHelpers[index]->getStartPhi()*deltaPhi) ;
    testLongeronPhi += (index == 1) ? m_phiOfStaveZeroLastLayer : m_phiOfStaveZero;
    double lastLongeronPhi   = testLongeronPhi + 360*CLHEP::deg;
    int    nStaveShapes      = m_staveDBHelpers[index]->getNStaveShapes();

    while (testLongeronPhi < lastLongeronPhi) {
      double phiDiff = testLongeronPhi - sectorPhi;
      if (phiDiff >  CLHEP::pi) phiDiff -= CLHEP::twopi;
      if (phiDiff < -CLHEP::pi) phiDiff += CLHEP::twopi;
      if (fabs(phiDiff) < fabs(phiOffsetLongeron)) {
	phiOffsetLongeron = phiDiff;
	longeronIndex     = testLongeronIndex; 
      }

      // Advance to next test longeron
      testLongeronPhi += m_staveDBHelpers[index]->getPhiStepSize(longeronIndex)*deltaPhi;
      testLongeronIndex++;
      if (testLongeronIndex == nStaveShapes) testLongeronIndex = 0;
    }
    
    // Get service volume dimensions
    double svcHalfThick = m_moduleSvcThickness;
    //Why the magic numbers here????? 
    double svcHalfWidth = m_barrelModule->Width()*0.75*.25;
    if (type == "endcap") svcHalfThick *= 2.0; // = Barrel + Endcap service thickness

    // Useful longeron properties
    double supRadialMidpoint  = m_staveDBHelpers[index]->getRadialMidpointAtEOS();
    double supRadialLength    = m_staveDBHelpers[index]->getRadialLengthAtEOS(longeronIndex);
    double supShellThickness  = m_staveDBHelpers[index]->getWallThickness    (longeronIndex);
    double supRadialExtHighR  = m_staveDBHelpers[index]->getXStepHighR       (longeronIndex);
    double supRadialExtLowR   = m_staveDBHelpers[index]->getXStepLowR        (longeronIndex);

    // stave support width
    double supWidth = (svcRouteDir > 0) ? m_staveDBHelpers[index]->getTopWidthAtEOS(longeronIndex) : m_staveDBHelpers[index]->getBaseWidthAtEOS(longeronIndex);
    supWidth = std::min(m_staveDBHelpers[index]->getBarrelWidth(longeronIndex), supWidth);

    // Alternating layers have overlapping services, offset one radially
    double svcOverlapOffset   = 0.0;
  
    if (fabs(phiOffsetLongeron) > 0.0001 && m_sector%2 == 0) svcOverlapOffset = (2.2*m_moduleSvcThickness);
    // Corrections to parameters - barrel offset, and radial link from barrel to eos
    double supBarrelExtension = 0.0;
    if (type == "barrel" || type == "radial" ) {
      supBarrelExtension = (svcRouteDir > 0) ? supRadialExtHighR : supRadialExtLowR;
    }

    // Reducing svc width if it does not fit in longeron
    if (fabs(phiOffsetLongeron) > 0.0001){ //two svc in one longeron
      while((2.2*svcHalfWidth) > ((supWidth*0.5) - supShellThickness) && svcHalfWidth > 0.1)
	svcHalfWidth*=0.98;
    }
    else{ // one svc in one longeron
      while((1.1*svcHalfWidth) > ((supWidth*0.5) - supShellThickness) && svcHalfWidth > 0.1)
	svcHalfWidth*=0.98;
    }
    
    // Defining width for radial module
    double radialSvcOffset = 0.0;
    if (type == "radial") {
      radialSvcOffset += 2.0*svcHalfThick;
      svcHalfThick = (0.5*supBarrelExtension) - svcHalfThick;
    }

    // svc routing inner = along outer half of longeron
    double radiusSvc =  supRadialMidpoint + ((0.5*supRadialLength) - supShellThickness + supBarrelExtension - svcHalfThick - svcOverlapOffset - radialSvcOffset) * svcRouteDir;
   
    HepGeom::Point3D<double> gServicePos =  HepGeom::RotateZ3D( phiOffsetLongeron )*HepGeom::Point3D<double>(radiusSvc, 0.0, 0.0);
    
    // Shift svc along longeron inside wall (so two services can be placed next to each other)
    double supSideShiftDir = (phiOffsetLongeron < 0.0) ? 1.0 : -1.0;
    if (fabs(phiOffsetLongeron) < 0.0001) supSideShiftDir = 0.0;  // no shift if longeron already aligned - one svc only
   
    double distanceShiftSvc = ((supWidth*0.5)  - supShellThickness) - (1.1*svcHalfWidth);
    //if (svcHalfWidth + distanceShiftSvc > supWidth) distanceShiftSvc = supWidth;
    double xShiftSvc = supSideShiftDir * distanceShiftSvc * sin(phiOffsetLongeron);
    double yShiftSvc = supSideShiftDir * distanceShiftSvc * cos(phiOffsetLongeron);
    gServicePos =    HepGeom::Translate3D(-xShiftSvc, yShiftSvc, 0.0) * gServicePos;
    
    // Evaluate ladder position in global frame (barrel origin = 0,0,0), assume no phi rotation!!
    HepGeom::Point3D<double> gLadderPos = m_localTrf*HepGeom::Point3D<double>(0.0, 0.0, 0.0);

    // Final coordinates of service relative to ladder position
    double xOffsetSvc = gServicePos.x() - gLadderPos.x();
    double yOffsetSvc = gServicePos.y() - gLadderPos.y();
    double zOffsetSvc = (zInit+zFinal)*.5;

    // Compute return vector: 0 = xOffsetSvc, 1 = yOffsetSvc, 2 = boundingBoxX, 3 = BounfdingBoxY
    svcBoundingBox.push_back(xOffsetSvc);
    svcBoundingBox.push_back(yOffsetSvc);
    svcBoundingBox.push_back(svcHalfWidth*sin(fabs(phiOffsetLongeron)) + svcHalfThick*cos(phiOffsetLongeron));
    svcBoundingBox.push_back(svcHalfWidth*cos(phiOffsetLongeron)       + svcHalfThick*sin(fabs(phiOffsetLongeron)));

    // Build service element, make final transform, and add to ladder 
    if (build) {
      GeoPhysVol* svcPhys = createServiceVolume(svcLength*.5, svcHalfThick, svcHalfWidth, moduleNumber);
      if(!svcPhys) return svcBoundingBox;
      GeoTransform* xform_svc = new GeoTransform(HepGeom::Translate3D(xOffsetSvc, yOffsetSvc, zOffsetSvc) * HepGeom::RotateZ3D( phiOffsetLongeron ));
      volPhys->add(xform_svc);
      volPhys->add(svcPhys);
    }
  
  }

  return svcBoundingBox;
}
