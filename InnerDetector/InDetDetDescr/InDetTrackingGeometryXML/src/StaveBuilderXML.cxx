///////////////////////////////////////////////////////////////////
// StaveBuilderXML.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include <InDetTrackingGeometryXML/XMLGeoTemplates.h>
#include <InDetTrackingGeometryXML/XMLReaderSvc.h>
#include "InDetTrackingGeometryXML/IModuleProvider.h"
#include "InDetTrackingGeometryXML/StaveBuilderXML.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkGeometryAlpine/AlpineStave.h"
#include "TrkSurfaces/Surface.h"
#include "GaudiKernel/IToolSvc.h"
#include "TMath.h"

InDet::StaveBuilderXML::StaveBuilderXML(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_pixelCase(true),
  m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","InDetXMLReaderSvc"),
  m_moduleProvider("InDet::SiModuleProvider/SiModuleProvider")
{
  declareInterface<StaveBuilderXML>(this);

  declareProperty("InDetXMLReader", m_xmlReader);
  declareProperty("ModuleProvider", m_moduleProvider);

  m_prevStaveTmp = 0;
  m_mountainPosPositions = 0;
  m_mountainNegPositions = 0;
}

InDet::StaveBuilderXML::~StaveBuilderXML()
{
}

StatusCode InDet::StaveBuilderXML::initialize()
{
  ATH_MSG_INFO("InDet::StaveBuilderXML::initialize()");

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_INFO("InDet::XMLReaderSvc successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("InDet::XMLReaderSvc: Couldn't retrieve " << m_xmlReader );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode InDet::StaveBuilderXML::finalize()
{
  ATH_MSG_INFO("InDet::StaveBuilderXML::finalize()");

  return StatusCode::SUCCESS;
}

Trk::AlpineStave *InDet::StaveBuilderXML::createStave(InDet::StaveTmp *staveTmp, int istave,  int nstaves, 
						     double R, double ztiltstave, double zoffsetstave, 
						     double phiOffset, bool isOuterMost)
{  
  if(!staveTmp) {
    ATH_MSG_WARNING("Can not build stave " << istave << " nstaves=" << nstaves << " R= " << R << " null pointer to template ");
    return 0;
  }

  unsigned int ilayer = staveTmp->layer;
  std::string stave_type = staveTmp->name;
  bool isBarrel = true;
  bool debug  = false;

  /////////////////////////////
  // Get/Build Plain modules //
  /////////////////////////////

  // stuff to compute the identifier
  int brl_ec  = 0; // barrel elements have brl_ec = 0 
  int iphi         = istave + 1 + round(nstaves/2.);
  if(istave*2+4>nstaves) iphi = istave + 1 - nstaves/2;
  double phistep = TMath::Pi()*2.0/nstaves;
  double phi = -TMath::Pi()+(istave+1)*phistep+phiOffset;
  Identifier id(0);
  IdentifierHash idhash(0);

  // Number of plain/transition and mountain modules
  int nmodplain = staveTmp->b_modn;
  int nmodtrans = 2*staveTmp->trans_pos.size();
  //  compute the stave length and steps for *tracking*
  double halfplainlength = m_xmlReader->getHalfPlainLength(staveTmp);
  double zstep = m_xmlReader->getZStep(staveTmp);
  
  // retrieve plain modules infos
  InDet::ModuleTmp* plainModuleTmp = m_xmlReader->getModuleTemplate(staveTmp->b_type);
  double moduleplain_length = plainModuleTmp->length;
  double moduleplain_gap = staveTmp->b_gap;

  // retrieve transition modules infos
  InDet::ModuleTmp* transModuleTmp=0 ;
  double moduletrans_length = 0.;
  double moduletrans_width = 0.;
  double moduletrans_gap = 0.;
  double moduletrans_thickness = 0.;
   if(nmodtrans>0){
    transModuleTmp = m_xmlReader->getModuleTemplate(staveTmp->trans_type) ;
    moduletrans_length = transModuleTmp->length;
    moduletrans_width = transModuleTmp->widthmax;
    moduletrans_gap = staveTmp->trans_gap;
    moduletrans_thickness = transModuleTmp->thickness;
  }

  ATH_MSG_DEBUG("Building stave " << istave << " (phi=" << phi << ",iphi= " << iphi << ") of type " << stave_type
		<< " for layer " << ilayer);

  // create stave
  Trk::AlpineStave *stave = 0;
  if(staveTmp == m_prevStaveTmp) { // This staveTmp is the same as the previous one, re-use vector
    stave = new Trk::AlpineStave(-halfplainlength,zstep,zoffsetstave,m_mountainPosPositions,m_mountainNegPositions);
  } else { // Different staveTmp, create vector of mountain positions 
    m_mountainPosPositions = new std::vector<double>;
    m_mountainNegPositions = new std::vector<double>;
    *m_mountainPosPositions = staveTmp->trans_pos;
    m_mountainPosPositions->insert( m_mountainPosPositions->end(), staveTmp->alp_pos.begin(), staveTmp->alp_pos.end() );
    m_mountainNegPositions->insert( m_mountainNegPositions->end(), m_mountainPosPositions->begin(), m_mountainPosPositions->end() );
    stave = new Trk::AlpineStave(-halfplainlength,zstep,zoffsetstave,m_mountainPosPositions,m_mountainNegPositions);
  }

  // loop over plain modules 
  double zstart_plain = -0.5*((nmodplain-1)*moduleplain_length + (nmodplain-1)*moduleplain_gap) + zoffsetstave;
  int ietastart = -nmodplain/2.;
  int ieta = ietastart;

  for(int imod=0;imod<nmodplain;imod++) {
    ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Getting/Building plains modules brl_ec = " 
		  <<brl_ec << " layer = " << ilayer << " iphi = " << iphi << " phi = " << phi << " ieta = " << ieta );

    // Compute transform
    double dR      = staveTmp->b_rshift; // module center translation w.r.t layer radius
    double xshift  = 0.;                 // shift along x-axis
    double zoffset = (iphi%2==0) ? 0.5*staveTmp->b_zoffset : -0.5*staveTmp->b_zoffset;
    double zpos    = zstart_plain+zstep*imod + zoffset;
    double tilt    = ztiltstave + staveTmp->b_tilt; // tilt around z-axis
    double stereo  = 0.;                            // stereo angle
    double rot     = 0.;                            // module angle w.r.t stave plane - none on the central module z=0
    if(ieta<0) rot = staveTmp->b_angle;
    if(ieta>0) rot = -staveTmp->b_angle;

    Amg::Transform3D transform = m_moduleProvider->getTransform(R,dR,xshift,zpos,tilt,stereo,rot,phi);    
    m_xmlReader->computeRbounds(transform, plainModuleTmp, staveTmp->rMin, staveTmp->rMax);

    // Create plain detector element 
    m_moduleProvider->setIdentifier(m_pixelCase,id,idhash,brl_ec,ilayer,iphi,ieta,0);
    Trk::TrkDetElementBase *elem = m_moduleProvider->getDetElement(id, idhash, plainModuleTmp, transform.translation(), transform, 
								   m_pixelCase, isBarrel, isOuterMost,debug);
    
//     std::cout << "        -->  Barrel Plain Tilted Element: " << std::endl;
//     std::cout << "        -->  brl_ec = " << brl_ec << "     layer_disc = " << ilayer << "     iphi = " << iphi << "     ieta = " << ieta << "     side = 0" << std::endl;
//     std::cout << "        -->  Surface Center = " << elem->surface().center() << std::endl;
//     std::cout << "        -->  Surface Phi = " << elem->surface().center().phi() << "     Eta = "<< elem->surface().center().eta() << std::endl;
  
    stave->addPlainModule(elem,0);
    
    // No module at ieta==0 for even number of modules
    ieta++; if(ieta==0 && (nmodplain%2)==0) ieta++; 

  } // end plain modules loop 

  //////////////////////////////
  // Alpine part of the stave //
  //////////////////////////////
  
  // kEEP TRACK OF IDENTIFIERS INDICES
  int ietamin = ietastart;
  int ietamax = ieta-1;

  // check if transition module are needed
  if(nmodtrans>0){
    /////////////////////////////////////
    // build plain transition modules //
    ////////////////////////////////////
    
    // plains transition module - negative side
    ieta    = ietamin-1;
    m_moduleProvider->setIdentifier(m_pixelCase,id,idhash,brl_ec,ilayer,iphi,ieta,0);
    ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Building transition plain module brl_ec = " << brl_ec << " layer = " 
		  << ilayer << " iphi = " << iphi << " phi = " << phi << " ieta = " << ieta );
    
    // Transformation
    double dR     = 0;
    double zpos   = -halfplainlength + moduleplain_length - 0.5*moduletrans_length + zoffsetstave;
    double tilt   = ztiltstave + staveTmp->b_tilt; // tilt around z-axis
    double stereo = 0.;                            // stereo angle
    double rot    = 0.;                            // module angle w.r.t stave plan
    double xshift = moduletrans_width/2. + moduletrans_gap/2.;
    Amg::Transform3D transform = m_moduleProvider->getTransform(R,dR,xshift,zpos,tilt,stereo,rot,phi);
    m_xmlReader->computeRbounds(transform, transModuleTmp, staveTmp->rMin, staveTmp->rMax);

    ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Transition plain module negative side");
    ATH_MSG_DEBUG("module width = " << moduletrans_width << " xshift = " << xshift);
    Trk::TrkDetElementBase *elem = m_moduleProvider->getDetElement(id, idhash, transModuleTmp, transform.translation(), transform, 
								   m_pixelCase, isBarrel, isOuterMost, debug);
    
//     std::cout << "        -->  Barrel Transition Negative Tilted Element: " << std::endl;
//     std::cout << "        -->  brl_ec = " << brl_ec << "     layer_disc = " << ilayer << "     iphi = " << iphi << "     ieta = " << ieta << "     side = 0" << std::endl;
//     std::cout << "        -->  Surface Center = " << elem->surface().center() << std::endl;
//     std::cout << "        -->  Surface Phi = " << elem->surface().center().phi() << "     Eta = "<< elem->surface().center().eta() << std::endl;
  

    // add transition module - negative side
    stave->addPlainModule(elem,1); // insert at the beginning of the plain modules vector
    
    // plains transition module - positive side
    ieta   = ietamax+1;
    m_moduleProvider->setIdentifier(m_pixelCase,id,idhash,brl_ec,ilayer,iphi,ieta,0);
    ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Building transition plain module brl_ec = " << brl_ec << " layer = " 
		  << ilayer << " iphi = " << iphi << " phi = " << phi << " ieta = " << ieta );
    
    // Transformation
    dR     = 0;
    zpos   = halfplainlength - moduleplain_length + 0.5*moduletrans_length + zoffsetstave;
    tilt   = ztiltstave + staveTmp->b_tilt;  // tilt around z-axis
    rot    = 0.;                             // module angle w.r.t stave plan
    stereo = 0.;                             // stereo angle
    xshift = moduletrans_width/2.+moduletrans_gap/2. ;
    transform = m_moduleProvider->getTransform(R,dR,xshift,zpos,tilt,stereo,rot,phi);
    m_xmlReader->computeRbounds(transform, transModuleTmp, staveTmp->rMin, staveTmp->rMax);
    
    ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Transition plain module positive side");
    ATH_MSG_DEBUG("module width = " << moduletrans_width << " xshift = " << xshift);
    
    elem = m_moduleProvider->getDetElement(id, idhash, transModuleTmp, transform.translation(), transform, 
					   m_pixelCase, isBarrel, isOuterMost, debug);
    
//     std::cout << "        -->  Barrel Transition Positive Tilted Element: " << std::endl;
//     std::cout << "        -->  brl_ec = " << brl_ec << "     layer_disc = " << ilayer << "     iphi = " << iphi << "     ieta = " << ieta << "     side = 0" << std::endl;
//     std::cout << "        -->  Surface Center = " << elem->surface().center() << std::endl;
//     std::cout << "        -->  Surface Phi = " << elem->surface().center().phi() << "     Eta = "<< elem->surface().center().eta() << std::endl;
  

    // add transition module - negative side
    stave->addPlainModule(elem,0); // insert at the end of the plain modules vector
      
    ////////////////////////////////////////
    // build mountain transition modules //
    //////////////////////////////////////
    
    // mountain transition module - negative side
    ieta    = ietamin-2;
    m_moduleProvider->setIdentifier(m_pixelCase,id,idhash,brl_ec,ilayer,iphi,ieta,0);
    ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Building transition mountain module brl_ec = " << brl_ec << " layer = " 
		  << ilayer << " iphi = " << iphi << " phi = " << phi << " ieta = " << ieta );
    
    // Transformation 
    double alpha = TMath::Pi()/2. - staveTmp->trans_angle;
    double Dz = 0.5*moduletrans_length*cos(alpha)-0.5*moduletrans_thickness*sin(alpha);
    dR     = -0.5*moduletrans_length*sin(alpha)+0.5*moduletrans_thickness*(1-cos(alpha));
    zpos   = zoffsetstave - staveTmp->trans_pos[0] - Dz; 
    tilt   = ztiltstave + staveTmp->trans_tilt;  // tilt around z-axis
    rot    = alpha;                              // module angle w.r.t stave plan
    stereo = 0.;                                 // stereo angle
    xshift = - (moduletrans_width/2.+ moduletrans_gap/2.); // warning - reverse w.r.t plain transition modules, of course
    transform = m_moduleProvider->getTransform(R,dR,xshift,zpos,tilt,stereo,rot,phi);
    m_xmlReader->computeRbounds(transform, transModuleTmp, staveTmp->rMin, staveTmp->rMax);

    ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Transition mountain module negative side");
    ATH_MSG_DEBUG("module width = " << moduletrans_width << " xshift = " << xshift);
    
    elem = m_moduleProvider->getDetElement(id, idhash, transModuleTmp, transform.translation(), transform, 
					   m_pixelCase, isBarrel, isOuterMost, debug);
    
//     std::cout << "        -->  Barrel Mountain Transition Negative Tilted Element: " << std::endl;
//     std::cout << "        -->  brl_ec = " << brl_ec << "     layer_disc = " << ilayer << "     iphi = " << iphi << "     ieta = " << ieta << "     side = 0" << std::endl;
//     std::cout << "        -->  Surface Center = " << elem->surface().center() << std::endl;
//     std::cout << "        -->  Surface Phi = " << elem->surface().center().phi() << "     Eta = "<< elem->surface().center().eta() << std::endl;
  

    // add mountain transition module
    stave->addMountainNegModule(elem);
    
    // mountain transition module - positive side
    ieta   = ietamax + 2;
    m_moduleProvider->setIdentifier(m_pixelCase,id,idhash,brl_ec,ilayer,iphi,ieta,0);
    ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Building transition mountain module brl_ec = " << brl_ec << " layer = " 
		  << ilayer << " iphi = " << iphi << " phi = " << phi << " ieta = " << ieta );

    // Transformation
    zpos   = zoffsetstave + staveTmp->trans_pos[0] + Dz; 
    tilt   = ztiltstave + staveTmp->trans_tilt;  // tilt around z-axis
    rot    = -alpha;                             // module angle w.r.t stave plan
    stereo = 0.;                                 // stereo angle
    xshift = -(moduletrans_width/2.+moduletrans_gap/2.); // warning - reverse w.r.t plain transition modules, of course
    transform = m_moduleProvider->getTransform(R,dR,xshift,zpos,tilt,stereo,rot,phi);
    m_xmlReader->computeRbounds(transform, transModuleTmp, staveTmp->rMin, staveTmp->rMax);
    
    ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Transition mountain module positive side");
    ATH_MSG_DEBUG("module width = " << moduletrans_width << " xshift = " << xshift);
    
    elem = m_moduleProvider->getDetElement(id, idhash, transModuleTmp, transform.translation(), transform, 
					 m_pixelCase, isBarrel,isOuterMost, debug);
    
//     std::cout << "        -->  Barrel Mountain Transition Positive Tilted Element: " << std::endl;
//     std::cout << "        -->  brl_ec = " << brl_ec << "     layer_disc = " << ilayer << "     iphi = " << iphi << "     ieta = " << ieta << "     side = 0" << std::endl;
//     std::cout << "        -->  Surface Center = " << elem->surface().center() << std::endl;
//     std::cout << "        -->  Surface Phi = " << elem->surface().center().phi() << "     Eta = "<< elem->surface().center().eta() << std::endl;
  

    // add mountain transition module
    stave->addMountainPosModule(elem);
    
    // kEEP TRACK OF IDENTIFIERS INDICES
    ietamin -= 2;
    ietamax += 2;
  }
    
  //////////////////////////////
  // build mountains modules  //
  //////////////////////////////
  
  unsigned int nmountains = staveTmp->alp_pos.size(); 

  if(nmountains>0){

    // retrieve mountains modules template
    InDet::ModuleTmp* mountModuleTmp = m_xmlReader->getModuleTemplate(staveTmp->alp_type);
    double modulealp_length = mountModuleTmp->length;
    //double modulealp_width = mountModuleTmp->widthmax;
    double modulealp_thickness = mountModuleTmp->thickness_sensor;
    
    // loop over mountains positions // start at i=1, since i=0 is the transition mountain module
    for (unsigned int i=0;i<nmountains;i++){
      Trk::TrkDetElementBase *elem;
    
      double alpha = TMath::Pi()/2. - staveTmp->alp_angle;
      double Dz = 0.5*modulealp_length*cos(alpha)-0.5*modulealp_thickness*sin(alpha);
      double dR = -0.5*modulealp_length*sin(alpha)+0.5*modulealp_thickness*(1-cos(alpha));
    
      // mountains - negative side
      ieta       = ietamin-(i+1);
      m_moduleProvider->setIdentifier(m_pixelCase,id,idhash,brl_ec,ilayer,iphi,ieta,0);
      ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Building mountain module  brl_ec = " << brl_ec << " layer = " 
		    << ilayer << " iphi = " << iphi << " phi = " << phi << " ieta = " << ieta );
    
      // Transformation
      double zoffset = (iphi%2==0) ? 0.5*staveTmp->alp_zoffset : -0.5*staveTmp->alp_zoffset;
      double zpos   = zoffsetstave - staveTmp->alp_pos[i] - Dz + zoffset;
      double tilt   = ztiltstave + staveTmp->alp_tilt ; // stave tilt around z-axis
      double rot    = alpha;                            // module angle w.r.t stave plan
      double roffset = (iphi%2==0) ? 0.5*staveTmp->alp_roffset : -0.5*staveTmp->alp_roffset;
      dR +=  staveTmp->alp_rshift + roffset;  // module center translation w.r.t layer radius 
      double stereo = 0.;                               // stereo angle
      double xshift = 0.;                               // shift along x-axis

      Amg::Transform3D transform = m_moduleProvider->getTransform(R,dR,xshift,zpos,tilt,stereo,rot,phi);
      m_xmlReader->computeRbounds(transform, mountModuleTmp, staveTmp->rMin, staveTmp->rMax);

      elem = m_moduleProvider->getDetElement(id, idhash, mountModuleTmp, transform.translation(), transform, 
					   m_pixelCase, isBarrel,isOuterMost, debug);
      
//       Amg::Transform3D transform_os = elem->transform();
//       const Amg::Vector3D posModule = transform_os * Amg::Vector3D(0.,0.,0.);
//       std::cout << "        -->  Barrel module gbl position - Layer" << ilayer<<" : "<<iphi<<" "<<ieta<<" : "<<posModule.x() << " "<<posModule.y()<<" "<<posModule.z()<<std::endl;

//       std::cout << "        -->  Barrel Mountain Negative Tilted Element: " << std::endl;
//       std::cout << "        -->  brl_ec = " << brl_ec << "     layer_disc = " << ilayer << "     iphi = " << iphi << "     ieta = " << ieta << "     side = 0" << std::endl;
//       std::cout << "        -->  Surface Center = " << elem->surface().center() << std::endl;
//       std::cout << "        -->  Surface Phi = " << elem->surface().center().phi() << "     Eta = "<< elem->surface().center().eta() << std::endl;
  
      // add negative mountains modules to the stave
      stave->addMountainNegModule(elem);
    
      // mountains - positive side
      ieta       = ietamax + (i+1);
      m_moduleProvider->setIdentifier(m_pixelCase,id,idhash,brl_ec,ilayer,iphi,ieta,0);
      ATH_MSG_DEBUG("InDet::StaveBuilderXML::createStave(): Building mountain module  brl_ec = " << brl_ec << " layer = " 
		    << ilayer << " iphi = " << iphi << " phi = " << phi << " ieta = " << ieta );

      // Transformation
      zpos  = zoffsetstave + staveTmp->alp_pos[i] + Dz + zoffset; 
      rot   = -alpha;   // module angle w.r.t stave plane

      transform = m_moduleProvider->getTransform(R,dR,xshift,zpos,tilt,stereo,rot,phi);
      m_xmlReader->computeRbounds(transform, mountModuleTmp, staveTmp->rMin, staveTmp->rMax);    
      elem = m_moduleProvider->getDetElement(id, idhash, mountModuleTmp, transform.translation(), transform, 
					     m_pixelCase, isBarrel, isOuterMost, debug);
      
//       std::cout << "        -->  Barrel Mountain Positive Tilted Element: " << std::endl;
//       std::cout << "        -->  brl_ec = " << brl_ec << "     layer_disc = " << ilayer << "     iphi = " << iphi << "     ieta = " << ieta << "     side = 0" << std::endl;
//       std::cout << "        -->  Surface Center = " << elem->surface().center() << std::endl;
//       std::cout << "        -->  Surface Phi = " << elem->surface().center().phi() << "     Eta = "<< elem->surface().center().eta() << std::endl;
  
      // add positive mountains modules to the stave
      stave->addMountainPosModule(elem);
    
    } // end of mountains loop
  } // end if mountains
  
  return stave;
}

std::vector<InDet::StaveTmp*> InDet::StaveBuilderXML::getStaveTemplate(int ilayer)
{
  if(m_pixelCase) return m_xmlReader->getPixelStaveTemplate(ilayer);
  return m_xmlReader->getSCTStaveTemplate(ilayer);
}

