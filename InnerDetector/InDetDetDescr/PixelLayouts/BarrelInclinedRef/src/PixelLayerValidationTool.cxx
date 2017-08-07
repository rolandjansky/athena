/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelInclinedRef/PixelLayerValidationTool.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "PathResolver/PathResolver.h"
// Amg
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <sstream>

PixelLayerValidationTool::PixelLayerValidationTool(const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc"),
    m_IDserviceTool("InDetservicesTool/PixelServicesTool")
{
  declareInterface<IPixelLayerValidationTool>(this);
  declareProperty("PixelServicesTool", m_IDserviceTool);
}

PixelLayerValidationTool::~PixelLayerValidationTool()
{
}

//================ Initialisation =================================================

StatusCode PixelLayerValidationTool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("PixelLayerValidationTool initialize() successful in " << name());

//   sc = m_IDserviceTool.retrieve(); 
//   if (sc.isFailure()){
//     msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
//     return sc;
//   }
//   msg(MSG::INFO) << "Service builder tool retrieved: " << endreq;

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
  }

  sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::INFO) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

  sc = service("THistSvc", m_thistSvc);
  if(sc.isFailure() ){
    msg(MSG::ERROR) << "Could not retrieve pointer to THistSvc" << endreq;
    //return sc;
  }

  int neta = 400;
  int nphi = 90;
  binEta = 4./neta;
  binPhi = 90./nphi*acos(-1.)/180.;

 
  for (unsigned int i=0; i<5; i++) {
    std::ostringstream hname;  
    std::ostringstream hname15;  
    std::ostringstream hname15n;  
    hname<<"l"<<i<<"_coverage";
    hname15<<"l"<<i<<"_coverage15";
    hname15n<<"l"<<i<<"_coverage15n";

    std::ostringstream h;  
    std::ostringstream h15;  
    std::ostringstream h15n;  
    h<<"/VALID/Hermeticity_l"<<i;
    h15<<"/VALID/Hermeticity15_l"<<i;
    h15n<<"/VALID/Hermeticity15n_l"<<i;

    m_coverage.push_back(new TH2F(hname.str().c_str(),h.str().c_str(),neta,0.,4.,nphi,-acos(-1.)/8.,3*acos(-1.)/8.));
    m_coverage15.push_back(new TH2F(hname15.str().c_str(),h15.str().c_str(),neta,0.,4.,nphi,-acos(-1.)/8.,3*acos(-1.)/8.));
    m_coverage15n.push_back(new TH2F(hname15n.str().c_str(),h15n.str().c_str(),neta,0.,4.,nphi,-acos(-1.)/8.,3*acos(-1.)/8.));
    sc = m_thistSvc->regHist(h.str().c_str(), m_coverage.back());
    sc = m_thistSvc->regHist(h15.str().c_str(), m_coverage15.back());
    sc = m_thistSvc->regHist(h15n.str().c_str(), m_coverage15n.back());
    
    std::ostringstream gname;  
    std::ostringstream gname15;  
    std::ostringstream gname15n;  
    gname<<"l"<<i<<"_hitrate";
    gname15<<"l"<<i<<"_hitrate15";
    gname15n<<"l"<<i<<"_hitrate15n";
    std::ostringstream g;  
    std::ostringstream g15;  
    std::ostringstream g15n;  
    g<<"/VALID/HitRate_l"<<i;
    g15<<"/VALID/HitRate15_l"<<i;
    g15n<<"/VALID/HitRate15n_l"<<i;
    m_hitrate.push_back(new TH2F(gname.str().c_str(),g.str().c_str(),neta,0.,4.,nphi,-acos(-1.)/8.,3*acos(-1.)/8.));
    m_hitrate15.push_back(new TH2F(gname15.str().c_str(),g15.str().c_str(),neta,0.,4.,nphi,-acos(-1.)/8.,3*acos(-1.)/8.));
    m_hitrate15n.push_back(new TH2F(gname15n.str().c_str(),g15n.str().c_str(),neta,0.,4.,nphi,-acos(-1.)/8.,3*acos(-1.)/8.));
    sc = m_thistSvc->regHist(g.str().c_str(), m_hitrate.back());
    sc = m_thistSvc->regHist(g15.str().c_str(), m_hitrate15.back());
    sc = m_thistSvc->regHist(g15n.str().c_str(), m_hitrate15n.back());
  }
  m_dInX0_mod_flat  = new TH1F("dInX0_mod_flat","Module material thickness ",60,-1250.,1250.);
  m_dInX0_svc_flat  = new TH1F("dInX0_svc_flat","Service material thickness ",60,-1250.,1250.);
  m_dInX0_stave_flat  = new TH1F("dInX0_stave_flat","Stave material thickness ",60,-1250.,1250.);
  sc = m_thistSvc->regHist("/VALID/ModRadThickFlat", m_dInX0_mod_flat);
  sc = m_thistSvc->regHist("/VALID/SvcRadThickFlat", m_dInX0_svc_flat);
  sc = m_thistSvc->regHist("/VALID/StaveRadThickFlat", m_dInX0_stave_flat);

  if(sc.isFailure() ){
    msg(MSG::ERROR) << "Could not register histogram" << endreq;
    //return sc;
  }



  return StatusCode::SUCCESS;
}


//================ Create =================================================


//================ Finalisation =================================================

StatusCode PixelLayerValidationTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

// Register callback function on ConDB object
// Empty for now
//StatusCode PixelLayerValidationTool::registerCallback( StoreGateSvc*)
//{
//  return StatusCode::SUCCESS;
//}


void PixelLayerValidationTool::printInfo(const GeoVPhysVol* vol, int iLayer) const
{
  ATH_MSG_INFO( " pixel layer validation tool called for layer :"<<iLayer); 

  //
  // create a barrel layer
  //

  InDet::BarrelLayerTmp *layerTmp = m_xmlReader->getPixelBarrelLayerTemplate(iLayer);

  // retrieve stave template for this layer
  //  int layerIndex           = layerTmp->index;
  std::string layerName    = layerTmp->name;
  double layerRadius       = layerTmp->radius;
  int    nSectors          = layerTmp->stave_n; 
  double ladderTilt        = layerTmp->stave_tilt;
  double staveOffset       = layerTmp->stave_zoffset;   
  std::string staveType    = layerTmp->stave_type;

  //double phiOfStaveZero = 0.;

  double deltaPhi = 360.*CLHEP::deg/(double)nSectors;

  HepGeom::Transform3D transRadiusAndTilt = HepGeom::TranslateX3D(layerRadius)*HepGeom::RotateZ3D(ladderTilt);

  std::vector<InDet::StaveTmp*> staveTmp = m_xmlReader->getPixelStaveTemplate(iLayer);

  int nbLadderType = staveTmp.size();

  std::vector<float> zSteps;
  std::vector<float> svcRadThick;
  
  int barrelModuleNumber = 0;
  float barrelModuleGap = 0.;
  int endcapModuleNumber = 0;
  double ladderOffset = 0.;

  std::vector<double> endcapModulePos;
  std::vector<HepGeom::Point3D<double> > endcapModPos;
  float phi = 0;
  for (int is=0; is<2; is++) {
    endcapModuleNumber = 0;
    int istave = is < nbLadderType ? is : nbLadderType-1;
    std::string endcapModuleType = "";
    endcapModulePos.clear();
    float endcapModuleGap = 0;
    float endcapInclAngle = 0;
    float endcapModuleRshift = 0;
    ladderOffset = (is%2==0)? staveOffset: -staveOffset;

    std::vector<int> nbSvcModule(2,0);

    barrelModuleNumber = staveTmp[istave]->b_modn;
    std::string barrelModuleType   = staveTmp[istave]->b_type;
    barrelModuleGap =  staveTmp[istave]->b_gap;
    float barrelModuleTilt =  staveTmp[istave]->b_tilt;
         
    InDet::ModuleTmp* module = m_xmlReader-> getModuleTemplate(barrelModuleType);
    InDet::ModuleTmp* emodule = 0;

    // barrel
    zSteps.clear();
    svcRadThick.clear();
    float zcurr = (barrelModuleNumber%2==0) ? -0.5*barrelModuleGap : +0.5*module->length-barrelModuleGap;
    zcurr += ladderOffset;

    for (int ib=0; ib<int(barrelModuleNumber/2.); ib++) {
      nbSvcModule[0]++; 

      zSteps.push_back(zcurr+barrelModuleGap);
      float zmin = zSteps.back();
      float zmax = zmin + module->length;
      double thi = atan(layerRadius/zmin);
      double thi15 = atan(layerRadius/(zmin-150.));
      double thi15n = atan(layerRadius/(zmin+150.));
      if (thi<0) thi+=acos(-1.);
      if (thi15<0) thi15+=acos(-1.);
      if (thi15n<0) thi15n+=acos(-1.);
      double the = atan(layerRadius/zmax);
      double the15 = atan(layerRadius/(zmax-150.));
      double the15n = atan(layerRadius/(zmax+150.));
      if (the<0) the+=acos(-1.);
      if (the15<0) the15+=acos(-1.);
      if (the15n<0) the15n+=acos(-1.);
      float phimin = phi-atan(module->widthmin/2./layerRadius/(1.+sin(barrelModuleTilt)));
      float phimax = phi+atan(module->widthmin/2./layerRadius/(1.-sin(barrelModuleTilt)));
      //std::cout<<"module width:"<< module->widthmin<<":"<<phimin<<":"<<phimax<< std::endl;
      //std::cout<<"module length:"<< module->length<< std::endl;
      //std::cout<< "check tan:"<< thi15<<":"<<tan(thi15/2.) <<":"<<tan(thi15/2.+0.5*acos(-1.))<< std::endl;
      double etamin = -log(tan(thi/2.));
      double etamax = -log(tan(the/2.));
      double etamin15 = -log(tan(thi15/2.));
      double etamax15 = -log(tan(the15/2.));
      //std::cout<<"barrel module eta limits:"<< is <<":"<<ladderOffset<<":"<<ib<<":"<<etamin15<<","<< etamax15<< std::endl;
      double etamin15n = -log(tan(thi15n/2.));
      double etamax15n = -log(tan(the15n/2.));
      for ( float phis = phimin; phis<phimax; phis+=binPhi ) {
        m_coverage[iLayer]->Fill(etamax,phis,1.);
        m_coverage[iLayer]->Fill(etamin,phis,1.);
        m_coverage15[iLayer]->Fill(etamax15,phis,1.);
        m_coverage15[iLayer]->Fill(etamin15,phis,1.);
        m_coverage15n[iLayer]->Fill(etamax15n,phis,1.);
        m_coverage15n[iLayer]->Fill(etamin15n,phis,1.);
      }
      for ( float es = etamin; es<etamax; es+=binEta ) {
        m_coverage[iLayer]->Fill(es,phimin,1.);
        m_coverage[iLayer]->Fill(es,phimax,1.);
	for ( float phis = phimin; phis<phimax; phis+=binPhi ) { 
	  m_hitrate[iLayer]->Fill(es,phis,1.);
	  m_hitrate[iLayer]->Fill(es,phis+2*deltaPhi,1.);
	  m_hitrate[iLayer]->Fill(es,phis-2*deltaPhi,1.);
	}
      }
      for ( float es = etamin15; es<etamax15; es+=binEta ) {
        m_coverage15[iLayer]->Fill(es,phimin,1.);
        m_coverage15[iLayer]->Fill(es,phimax,1.);
	for ( float phis = phimin; phis<phimax; phis+=binPhi ) { 
	  m_hitrate15[iLayer]->Fill(es,phis,1.);
	  m_hitrate15[iLayer]->Fill(es,phis+2*deltaPhi,1.);
	  m_hitrate15[iLayer]->Fill(es,phis-2*deltaPhi,1.);
	}
      }
      for ( float es = etamin15n; es<etamax15n; es+=binEta ) {
        m_coverage15n[iLayer]->Fill(es,phimin,1.);
        m_coverage15n[iLayer]->Fill(es,phimax,1.);
	for ( float phis = phimin; phis<phimax; phis+=binPhi ) { 
	  m_hitrate15n[iLayer]->Fill(es,phis,1.);
	  m_hitrate15n[iLayer]->Fill(es,phis+2*deltaPhi,1.);
	  m_hitrate15n[iLayer]->Fill(es,phis-2*deltaPhi,1.);
	}
      }
      zcurr=zmax;

      std::string matName = m_IDserviceTool->getLayerModuleMaterialName(iLayer ,nbSvcModule);   
      //std::cout<<"Barrel module service material  : "<<matName<<"  "<<std::endl;
      
      std::ostringstream wg_matName;  
      wg_matName<<matName<<"_L"<<iLayer<<"_"<<int(ib+barrelModuleNumber/2.+endcapModuleNumber+2);

      std::ostringstream base_matName;   
      base_matName<<matName<<"_Base";
 
      //std::cout<<"Barrel module weighted service material : "<<matName<<"  "<<wg_matName.str()<<std::endl;

      // GeoMaterial* svcMat = 0;  // do not redefine material if already done for sector 0
       
      float dx0 = 0.;
      /*
      InDetMaterialManager* matMgr = m_IDserviceTool->materialMgr();      
      if (matMgr) {
	double volume = (module->length+barrelModuleGap);   // mm^3   
	svcMat = const_cast<GeoMaterial*>(m_IDserviceTool->materialMgr()->getMaterialForVolumeLength(matName, volume,
											      module->length+barrelModuleGap));   

	if (!svcMat)
	  std::cout <<"service module material not found "<< matName << std::endl;
	else { 
	  std::cout<<"linear material:rho:"<<svcMat->getDensity()<<":x0:"<< svcMat->getRadLength()<<":dInX0:"<<CLHEP::mm/svcMat->getRadLength()*nSectors/2/acos(-1.)/layerRadius<<std::endl;
          dx0 =  CLHEP::mm/svcMat->getRadLength()*nSectors/2/acos(-1.)/layerRadius;
	}
      } else std::cout <<"material manager not available"<< std::endl;
      */
      svcRadThick.push_back(dx0);
    }

    // endcap   
    endcapModPos.clear();

    if(staveType.compare("Alpine")==0){
      endcapModulePos = staveTmp[istave]->alp_pos;
      endcapModuleType = staveTmp[istave]->alp_type;
      endcapModuleGap = 0.;
      endcapInclAngle = staveTmp[istave]->alp_angle;
      endcapModuleRshift = staveTmp[istave]->alp_rshift;

      emodule = m_xmlReader-> getModuleTemplate(endcapModuleType);
      //std::cout<<"endcap module width:"<< emodule->widthmin<< std::endl;
      //std::cout<<"endcap module length:"<< emodule->length<< std::endl;

      
      for (unsigned int iz=0; iz<endcapModulePos.size(); iz++) {
	endcapModPos.push_back(HepGeom::Point3D<double> (0.,0.,endcapModulePos[iz]));
      
	//std::cout << "position module valid:R:z:"<<  layerRadius + endcapModuleRshift - 10.1*cos(endcapInclAngle)
	//  + 0.2 -0.05*sin(endcapInclAngle) <<":"<< endcapModulePos[iz]+ladderOffset+10.1*sin(endcapInclAngle)
	//  -0.05*cos(endcapInclAngle)<<" edge:"<< endcapModulePos[iz]-0.5*emodule->length*sin(endcapInclAngle)<< std::endl;

        zSteps.push_back(endcapModulePos[iz]-0.5*emodule->length*sin(endcapInclAngle)+ladderOffset);
	float zmin = zSteps.back();
	float zmax = zmin + emodule->length*sin(endcapInclAngle);
        double thi = atan((layerRadius+endcapModuleRshift)/zmin);
        double the = atan((layerRadius+endcapModuleRshift-emodule->length*cos(endcapInclAngle))/zmax);
        double thi15 = atan((layerRadius+endcapModuleRshift)/(zmin-150.));
        double the15 = atan((layerRadius+endcapModuleRshift-emodule->length*cos(endcapInclAngle))/(zmax-150.));
        double thi15n = atan((layerRadius+endcapModuleRshift)/(zmin+150.));
        double the15n = atan((layerRadius+endcapModuleRshift-emodule->length*cos(endcapInclAngle))/(zmax+150.));
	float dphimin = atan(emodule->widthmin/2./(layerRadius+endcapModuleRshift));
	float dphimax = atan(emodule->widthmin/2./(layerRadius+endcapModuleRshift-emodule->length*cos(endcapInclAngle)));
	double etamin = -log(tan(thi/2.));
	double etamax = -log(tan(the/2.));
	double etamin15 = -log(tan(thi15/2.));
	double etamax15 = -log(tan(the15/2.));
	double etamin15n = -log(tan(thi15n/2.));
	double etamax15n = -log(tan(the15n/2.));
	for ( float phis = phi-dphimin; phis<phi+dphimin; phis+=binPhi ) m_coverage[iLayer]->Fill(etamin,phis,1.);
	for ( float phis = phi-dphimax; phis<phi+dphimax; phis+=binPhi ) m_coverage[iLayer]->Fill(etamax,phis,1.);
	for ( float es = etamin; es<etamax; es+=binEta ) {
	  double th = 2*atan(exp(-es));
          double a = tan(th);
          double t = (a*zmax-layerRadius)/(endcapModuleRshift+a*(zmax-zmin));
          double rcurr = layerRadius+t*endcapModuleRshift;
	  //double zcurr = zmax-t*(zmax-zmin);
	  m_coverage[iLayer]->Fill(es,phi-atan(emodule->widthmin/2./rcurr),1.);
	  m_coverage[iLayer]->Fill(es,phi+atan(emodule->widthmin/2./rcurr),1.);
	  for ( float phis = phi-atan(emodule->widthmin/2./rcurr); phis<phi+atan(emodule->widthmin/2./rcurr); phis+=1./180.*acos(-1.) ) { 
	    m_hitrate[iLayer]->Fill(es,phis,1.);
	    m_hitrate[iLayer]->Fill(es,phis+2*deltaPhi,1.);
	    m_hitrate[iLayer]->Fill(es,phis-2*deltaPhi,1.);
	  }
	}
	for ( float phis = phi-dphimin; phis<phi+dphimin; phis+=1./180.*acos(-1.) ) m_coverage15[iLayer]->Fill(etamin15,phis,1.);
	for ( float phis = phi-dphimax; phis<phi+dphimax; phis+=1./180.*acos(-1.) ) m_coverage15[iLayer]->Fill(etamax15,phis,1.);
	for ( float es = etamin15; es<etamax15; es+=binEta ) {
	  double th = 2*atan(exp(-es));
          double a = tan(th);
          double t = (a*(zmax-150.)-layerRadius)/(endcapModuleRshift+a*(zmax-zmin));
          double rcurr = layerRadius+t*endcapModuleRshift;
	  //double zcurr = zmax-t*(zmax-zmin);
	  m_coverage15[iLayer]->Fill(es,phi-atan(emodule->widthmin/2./rcurr),1.);
	  m_coverage15[iLayer]->Fill(es,phi+atan(emodule->widthmin/2./rcurr),1.);
	  for ( float phis = phi-atan(emodule->widthmin/2./rcurr); phis<phi+atan(emodule->widthmin/2./rcurr); phis+=binPhi ) { 
	    m_hitrate15[iLayer]->Fill(es,phis,1.);
	    m_hitrate15[iLayer]->Fill(es,phis+2*deltaPhi,1.);
	    m_hitrate15[iLayer]->Fill(es,phis-2*deltaPhi,1.);
	  }
	}
	for ( float phis = phi-dphimin; phis<phi+dphimin; phis+=binPhi ) m_coverage15n[iLayer]->Fill(etamin15n,phis,1.);
	for ( float phis = phi-dphimax; phis<phi+dphimax; phis+=binPhi ) m_coverage15n[iLayer]->Fill(etamax15n,phis,1.);
	for ( float es = etamin15n; es<etamax15n; es+=binEta ) {
	  double th = 2*atan(exp(-es));
          double a = tan(th);
          double t = (a*(zmax+150.)-layerRadius)/(endcapModuleRshift+a*(zmax-zmin));
          double rcurr = layerRadius+t*endcapModuleRshift;
	  //double zcurr = zmax-t*(zmax-zmin);
	  m_coverage15n[iLayer]->Fill(es,phi-atan(emodule->widthmin/2./rcurr),1.);
	  m_coverage15n[iLayer]->Fill(es,phi+atan(emodule->widthmin/2./rcurr),1.);
	  for ( float phis = phi-atan(emodule->widthmin/2./rcurr); phis<phi+atan(emodule->widthmin/2./rcurr); phis+=binPhi ) { 
	    m_hitrate15n[iLayer]->Fill(es,phis,1.);
	    m_hitrate15n[iLayer]->Fill(es,phis+2*deltaPhi,1.);
	    m_hitrate15n[iLayer]->Fill(es,phis-2*deltaPhi,1.);
	  }
	}
      }
      endcapModuleNumber = (int)endcapModPos.size();
    } 

    //int outerTrans = iLayer==0 ? 2 : 0;      // fix : 2 for outer routing, 0 for inner routing

    for (int ib=0; ib<endcapModuleNumber; ib++) {
      nbSvcModule[1]=ib; 
      std::string matName = m_IDserviceTool->getLayerModuleMaterialName(iLayer ,nbSvcModule);   
      //std::cout<<"Endcap module service material  : "<<matName<<"  "<<std::endl;
      
      std::ostringstream wg_matName;  
      // TO DO : debug presence of transition region
     //wg_matName<<matName<<"_L"<<iLayer<<"_"<<int(ib+barrelModuleNumber+endcapModuleNumber+2+2);    // 2+2 account for the transition region
      //wg_matName<<matName<<"_L"<<iLayer<<"_"<<int(ib+barrelModuleNumber+endcapModuleNumber+2 + outerTrans);    // 2+2 account for the transition region
      wg_matName<<matName<<"_L"<<iLayer<<"_"<<int(ib+barrelModuleNumber+endcapModuleNumber);    // 2+2 account for the transition region

      //std::ostringstream base_matName;   
      //base_matName<<matName<<"_Base";
  
      //std::cout<<"Endcap module weighted service material : "<<matName<<"  "<<wg_matName.str()<<std::endl;

      //GeoMaterial* svcMat = 0;  // do not redefine material if already done for sector 0
      
      // InDetMaterialManager* matMgr = m_IDserviceTool->materialMgr();
      
      float dx0 = 0.;
      /*
      if (matMgr) {
        double length = ib>0 ?  fabs(endcapModulePos[ib]-endcapModulePos[ib-1]) :
	  fabs(endcapModulePos[ib]-barrelModuleNumber/2.*(module->length+barrelModuleGap));
	double volume = length;   // mm^3   
	svcMat = const_cast<GeoMaterial*>(m_IDserviceTool->materialMgr()->getMaterialForVolumeLength(matName, volume, length)); 

        GeoMaterial* testMat =  const_cast<GeoMaterial*>(m_IDserviceTool->materialMgr()->getMaterial(matName));  
        GeoMaterial* scaledMat =  const_cast<GeoMaterial*>(m_IDserviceTool->materialMgr()->getMaterial(wg_matName.str()));  
 
        if (testMat)  std::cout<<"test: linear material:rho:"<<testMat->getDensity()/CLHEP::g*CLHEP::cm3<<":x0:"<< testMat->getRadLength()<<":dInX0 averaged:"<<CLHEP::mm/testMat->getRadLength()/2/acos(-1.)/layerRadius*nSectors<<std::endl;
        if (scaledMat)  std::cout<<"test: linear material:rho:"<<scaledMat->getDensity()/CLHEP::g*CLHEP::cm3<<":x0:"<< scaledMat->getRadLength()<<":dInX0 mod width:"<<0.25*CLHEP::mm/scaledMat->getRadLength()<<std::endl;

	if (!svcMat)
	  std::cout <<"service module material not found "<< matName << std::endl;
	else { 
	  std::cout<<"linear material:rho:"<<svcMat->getDensity()/CLHEP::g*CLHEP::cm3<<":x0:"<< svcMat->getRadLength()<<":dInX0:"<<CLHEP::mm/svcMat->getRadLength()/2/acos(-1.)/layerRadius*nSectors<<std::endl;
          dx0 = CLHEP::mm/svcMat->getRadLength()*nSectors/2/acos(-1.)/layerRadius;
	}
      } else std::cout <<"material manager not available"<< std::endl;
      */        
      svcRadThick.push_back(dx0);
    }

   
    phi +=deltaPhi;


    std::cout <<"LAYER "<< iLayer<<""<< is<< ": radius & number of staves:" <<layerRadius <<" " <<nSectors<< std::endl;
    std::cout <<"LAYER "<< iLayer<<""<< is<< ": number of modules (barrel,endcap):" << barrelModuleNumber <<" "<<2*endcapModuleNumber<< std::endl;
    std::cout <<"LAYER "<< iLayer<<""<< is<< ": surface per stave (barrel, endcap):"<< barrelModuleNumber*module->length*module->widthmin<<" " << 2*endcapModuleNumber*emodule->length*emodule->widthmin<< std::endl;


  }

  zSteps.push_back(1250.);

  Float_t bins[2*int(barrelModuleNumber/2)+2*endcapModuleNumber+2];

  bins[0]=-1250;
  int bIndex = -1;

  //std::cout <<"ladderOffset:"<<ladderOffset<< std::endl;

  for (int ib=zSteps.size()-1; ib>=0; ib--) {
    //std::cout <<zSteps[0]<<":"<<ib<< std::endl;
    if (ib<0) break;
    float zori = zSteps[ib]-ladderOffset;
    if (zori<0) break;
    bIndex++;
    //std::cout <<2*int(barrelModuleNumber/2)+2*endcapModuleNumber+2<<":"<<bIndex<<":"<<zSteps[ib]<<":"<<-zori+2*ladderOffset<< std::endl;
    bins[bIndex]=-zori+2*ladderOffset;
  }
  //int index = zSteps.size()-2;
  //int iblow = (barrelModuleNumber%2==0)? 0 : 1;
  for (unsigned int ib=0; ib<zSteps.size(); ib++){
    bIndex++;
    //std::cout << barrelModuleNumber+2*endcapModuleNumber+1<<":"<<bIndex<<":"<<zSteps[ib]<<std::endl;;
    bins[bIndex]=zSteps[ib];   
  }

  bIndex++; bins[bIndex] = 1250.;

  int svcInt = 7;

 

  int svcBins = barrelModuleNumber+2*endcapModuleNumber+1;
  m_dInX0_svc  = new TH1F("dInX0_svc","Service material thickness ",svcBins,bins);
  StatusCode sc = m_thistSvc->regHist("/VALID/ModRadThick", m_dInX0_svc);

  //std::cout <<"number of svc bins:"<< svcBins <<":"<<svcRadThick.size()<< std::endl;


  if(sc.isFailure() ){
    msg(MSG::ERROR) << "Could not register histogram for inclined svc material" << endreq;
    //return sc;
  } else {

    m_dInX0_svc->SetBinContent(svcRadThick.size()+1, svcRadThick[0]);
    for (unsigned int ib=0; ib<svcRadThick.size(); ib++){
      m_dInX0_svc->SetBinContent(svcRadThick.size()-ib, svcRadThick[ib]);
      m_dInX0_svc->SetBinContent(svcBins-svcRadThick.size()+ib, svcRadThick[ib]);
      m_dInX0_svc_flat->SetBinContent(30-ib, svcRadThick[ib]);
      m_dInX0_svc_flat->SetBinContent(31+ib, svcRadThick[ib]);
    }
    for (unsigned int ib=svcRadThick.size(); ib<31; ib++){
      float dsvc=  m_dInX0_svc_flat->GetBinContent(31+svcInt)-m_dInX0_svc_flat->GetBinContent(31);
      float c0 = m_dInX0_svc_flat->GetBinContent(ib+31-svcInt);
      m_dInX0_svc_flat->SetBinContent(31+ib,c0+dsvc );
      m_dInX0_svc_flat->SetBinContent(30-ib,c0+dsvc );
    }
  }
 
  
  //bins[barrelModuleNumber+2*endcapModuleNumber]=1250.;
  //bins[barrelModuleNumber+2*endcapModuleNumber+1]=1250.;
  
  //m_dInX0_mod  = new TH1F("dInX0_mod","Module material thickness ",barrelModuleNumber+2*endcapModuleNumber,bins);
  // m_dInX0_svc  = new TH1F("dInX0_svc","Service material thickness ",barrelModuleNumber+2*endcapModuleNumber,bins);
  //m_dInX0_stave  = new TH1F("dInX0_stave","Stave material thickness ",barrelModuleNumber+2*endcapModuleNumber,bins);
  //StatusCode sc = m_thistSvc->regHist("/VALID/ModRadThick", m_dInX0_mod);
  //sc = m_thistSvc->regHist("/VALID/SvcRadThick", m_dInX0_svc);
  //sc = m_thistSvc->regHist("/VALID/StaveRadThick", m_dInX0_stave);
  //if(sc.isFailure() ){
  //  msg(MSG::ERROR) << "Could not register histogram" << endreq;
  // //return sc;
  //}

  /*
  m_barrelModuleNumber = m_staveTmp->b_modn;
  m_barrelModuleType = m_staveTmp->b_type;
  m_barrelModuleGap =  m_staveTmp->b_gap;
  m_barrelModuleTilt =  m_staveTmp->b_tilt;

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
    m_endcapModuleRshift = m_staveTmp->alp_rshift;

    m_transModulePos = m_staveTmp->trans_pos;
    if(m_transModulePos.size()>0){
      m_transitionModuleNumber = (int)m_transModulePos.size();
      m_transitionModuleType = m_staveTmp->trans_type;
      m_transitionTiltAngle = m_staveTmp->trans_angle;
    }

    m_endcapModPos = DecodeEndCapModulePositions(0., 0., m_transModulePos,m_endcapModulePos);
    m_endcapModuleNumber = (int)m_endcapModPos.size();
  }

  */


  //double ladderOffset = (ii%2==0)? staveOffset: -staveOffset;
  //double phiOfSector = phiOfStaveZero + ii*deltaPhi;
  //  HepGeom::Transform3D ladderTransform = HepGeom::TranslateZ3D(ladderOffset) * HepGeom::RotateZ3D(phiOfSector) * transRadiusAndTilt;

  ATH_MSG_INFO(" GeoModel dump ");

  const GeoLogVol* lv = vol->getLogVol();
  std::cout << "New Object:"<<lv->getName()<<", made of"<<lv->getMaterial()->getName()<<","<<lv->getShape()->type()<<std::endl;
  //m_geoShapeConverter->decodeShape(lv->getShape());
  const GeoTrd* trd=dynamic_cast<const GeoTrd*> (lv->getShape());
  if (trd) std::cout<<"trddim:"<< trd->getXHalfLength1()<<","<<trd->getXHalfLength2()<<","<<trd->getYHalfLength1()<<","<<trd->getYHalfLength2()<<","<<trd->getZHalfLength()<< std::endl;
  const GeoTubs* tub=dynamic_cast<const GeoTubs*> (lv->getShape());
  if (tub) std::cout<<"tubdim:"<< tub->getRMin()<<","<<tub->getRMax()<<","<<tub->getZHalfLength()<< std::endl;
  const GeoTube* tube=dynamic_cast<const GeoTube*> (lv->getShape());
  if (tube) std::cout<<"tubdim:"<< tube->getRMin()<<","<<tube->getRMax()<<","<<tube->getZHalfLength()<< std::endl;
  const GeoPcon* con=dynamic_cast<const GeoPcon*> (lv->getShape());
  if (con) {
    const unsigned int nPlanes=con->getNPlanes();
    for (unsigned int i=0; i<nPlanes; i++) {
      std::cout<<"polycone:"<<i<<":"<< con->getRMinPlane(i)<<","<<con->getRMaxPlane(i)<<","<<con->getZPlane(i)<< std::endl;
    }
  }
  const GeoBox* box=dynamic_cast<const GeoBox*> (lv->getShape());
  if (box) std::cout<<"boxdim:"<< box->getXHalfLength()<<","<<box->getYHalfLength()<<","<<box->getZHalfLength()<< std::endl;

  //Amg::Transform3D transf =  Amg::CLHEPTransformToEigen(vol->getX());
  Amg::Transform3D transf; transf.setIdentity();
  int igen = 0;
  printChildren(vol,-1,igen,transf);
}

void PixelLayerValidationTool::printChildren(const PVConstLink pv,int gen, int igen, Amg::Transform3D trIn) const
{
  // subcomponents
  unsigned int nc = pv->getNChildVols();
  igen++; 
  if (gen>-1 && igen>gen) return; 
  std::string cname; 
  for (unsigned int ic=0; ic<nc; ic++) {
    Amg::Transform3D transf = trIn*Amg::CLHEPTransformToEigen(pv->getXToChildVol(ic));
 
    //
    //std::cout << " dumping transform to subcomponent" << std::endl;
    //std::cout << transf[0][0]<<"," <<transf[0][1]<<"," <<transf[0][2]<<","<<transf[0][3] << std::endl;
    //std::cout << transf[1][0]<<"," <<transf[1][1]<<"," <<transf[1][2]<<","<<transf[1][3] << std::endl;
    //std::cout << transf[2][0]<<"," <<transf[2][1]<<"," <<transf[2][2]<<","<<transf[2][3] << std::endl;
    //
    const PVConstLink cv = pv->getChildVol(ic);
    const GeoLogVol* clv = cv->getLogVol();
    std::cout << "  ";
    std::cout << "subcomponent:"<<igen<<":"<<ic<<":"<<clv->getName()<<", made of"<<clv->getMaterial()->getName()<<"x0:"<<clv->getMaterial()->getRadLength()<<","<<clv->getShape()->type()
	      <<std::endl;
    std::cout << "position:"<< "R:"<<transf.translation().perp()<<",phi:"<< transf.translation().phi()<<",x:"<<transf.translation().x()<<",y:"<<transf.translation().y()<<",z:"<<transf.translation().z()<<std::endl;
    const GeoTrd* trd=dynamic_cast<const GeoTrd*> (clv->getShape());
    if (trd) std::cout<<"trddim:"<< trd->getXHalfLength1()<<","<<trd->getXHalfLength2()<<","<<trd->getYHalfLength1()<<","<<trd->getYHalfLength2()<<","<<trd->getZHalfLength()<< std::endl;
    const GeoTubs* tub=dynamic_cast<const GeoTubs*> (clv->getShape());
    if (tub) std::cout<<"tubdim:"<< tub->getRMin()<<","<<tub->getRMax()<<","<<tub->getZHalfLength()<< std::endl;
    const GeoPcon* con=dynamic_cast<const GeoPcon*> (clv->getShape());
    if (con) {
      const unsigned int nPlanes=con->getNPlanes();
      for (unsigned int i=0; i<nPlanes; i++) {
	std::cout<<"polycone:"<<i<<":"<< con->getRMinPlane(i)<<","<<con->getRMaxPlane(i)<<","<<con->getZPlane(i)<< std::endl;
      }
    }
    const GeoBox* box=dynamic_cast<const GeoBox*> (clv->getShape());
    if (box) std::cout<<"boxdim:"<< box->getXHalfLength()<<","<<box->getYHalfLength()<<","<<box->getZHalfLength()<< std::endl;

    if (ic==0 || cname != clv->getName() ) {  
      //m_geoShapeConverter->decodeShape(clv->getShape()); 	 
      printChildren(cv,gen,igen,transf);
      cname = clv->getName();
    }
  }  
   
}

