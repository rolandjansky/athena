/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCalibAlgs/PixelBarrelSurvey.h"
#include "PixelCalibAlgs/PixelConvert.h"
#include "PathResolver/PathResolver.h"
#include "Identifier/Identifier.h"
#include "DetDescrConditions/AlignableTransform.h"
#include "DetDescrConditions/DetCondCFloat.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelCalibAlgs/PixelBarrelSurveyUtils.h"
//#include "GeoPrimitives/CLHEPtoEigenConverter.h" 


/////////////////////////////////////////////////////////////////////////////

const int distosize=3;


PixelBarrelSurvey::PixelBarrelSurvey(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_pixelID(0),
  m_pixmgr(0),
  m_transforms(0),
  m_distorsions(0)
{

// Part 2: Properties go here
  // declareInterface< PixelBarrelSurvey >( this );   
   declareProperty("MappingFile", 
		   m_mappingDataFile = "NamingConversions.txt",
		   "PDB number <-> Athena mapping file" );
   declareProperty("SurveyFile" , 
		   m_surveyDataFile  = "StaveSurveyData.txt",
		   "File containing survey information");
   declareProperty("COOL_Tag"   , 
		   m_tag  = "dummytag",
		   "Tag for storing these data file (not used)");
   declareProperty("OldFileFormat",
		   m_oldOutputFormat=false,
		   "Use Euler angles instead of rotations about coordinate axes");
   declareProperty("OutputFile" , 
		   m_outputDataFile  = "SurveyTransform.txt",
		   "AlignTransform output text file");
   declareProperty("DistoFile"  , 
		   m_outputDistoFile = "SurveyDistorsions.txt",
		   "Distortion output text file");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PixelBarrelSurvey::initialize(){

  msg(MSG::INFO) << "initialize()" << endmsg;

  std::string managerName("Pixel");
  if ( StatusCode::SUCCESS!= detStore()->retrieve(m_pixmgr,managerName) ) {
    msg(MSG::FATAL) << "PixelDetectorManager not found" << endmsg;
    return StatusCode::FAILURE;
  }

  if( StatusCode::SUCCESS!= detStore()->retrieve( m_pixelID, "PixelID" ) ){
    msg(MSG::FATAL) << "Unable to retrieve pixel ID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  /*
  if (StatusCode::SUCCESS!=
      p_toolsvc->retrieveTool("InDetAlignDBTool",p_iddbtool)) {
      msg(MSG::FATAL) << "InDetAlignDBTool not found" << endmsg;
    return StatusCode::FAILURE;
  }
  p_iddbtool->createDB();
  */
  // Load the map for production module <-> Athena ID conversion
  std::string file_name = PathResolver::find_file (m_mappingDataFile, "DATAPATH");
  if (file_name.size()==0) {
    msg(MSG::FATAL) << "Mapping File: " << m_mappingDataFile << " not found!" << endmsg;
    return StatusCode::FAILURE;
  } 

  int sc = PixelConvert::ReadMap(file_name);
  if ( sc ) {
    msg(MSG::FATAL) << "Error in accessing file " << file_name << endmsg;
    return StatusCode::FAILURE;
  }

  // Load the container for the survey transform
  file_name = PathResolver::find_file (m_surveyDataFile, "DATAPATH");
  if (file_name.size()==0) {
    msg(MSG::FATAL) << "Survey File: " << m_surveyDataFile << " not found!" << endmsg;
    return StatusCode::FAILURE;
  } 

  std::ifstream in(file_name.c_str());
  if ( !in.good() ) {
    msg(MSG::FATAL) << "Error in opening file " << file_name << endmsg;
    return StatusCode::FAILURE;
  }
  PixelBarrelSurveyUtils::StaveStruct *stave;
  while ( (stave=PixelBarrelSurveyUtils::ReadStave(in)) ) {
    m_staves.push_back(*stave);
    delete stave;
  }
  if ( in.bad() ) msg(MSG::ERROR) << "Error reading file " << file_name;
  else msg(MSG::INFO) << "Reached end of file " << file_name;
  msg() << " after " << m_staves.size() << " staves" << endmsg;
  in.close();

  m_transforms = new AlignableTransform(m_tag);
  m_distorsions = new DetCondCFloat(distosize,m_tag);

  if ( !m_transforms || !m_distorsions ) {
    msg(MSG::FATAL) << "Unable to initialize transform objects" << endmsg;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PixelBarrelSurvey::execute() {

// Part 1: Get the messaging service, print where you are
  if(msgLvl(MSG::INFO)) msg() << "execute()" << endmsg;
  
// Part 2: Print out the different levels of messages
  std::vector<PixelBarrelSurveyUtils::StaveStruct>::iterator theStave=m_staves.begin();
  std::vector<PixelBarrelSurveyUtils::StaveStruct>::iterator lastStave=m_staves.end();
  int eta_max=ModulesOnStave/2;
  float *disto=new float[distosize];
  while (theStave!=lastStave) {
    for (int i=0; i<ModulesOnStave; i++) {
      HepGeom::Transform3D trans=PixelBarrelSurveyUtils::GetModuleTransform(*theStave,i-eta_max);
      static const HepGeom::Point3D<double> x0(0.,0.,0.);
      static const HepGeom::Point3D<double> x1(8.9,29.9,0.);
      HepGeom::Point3D<double> v0=trans*x0;
      HepGeom::Point3D<double> v1=trans*x1;
      unsigned int prodID=(theStave->module[i].serialNumber)%1000000;
      Identifier hashID(PixelConvert::GetID(prodID));
      std::cout << m_pixelID->show_to_string(hashID) 
	  << " " << v0.x() << " " << v0.y() << " " << v0.z()
	  << " " << v1.x() << " " << v1.y() << " " << v1.z()
	  << std::endl;
      InDetDD::SiDetectorElement *element=m_pixmgr->getDetectorElement(hashID);
      HepGeom::Transform3D idealTransform=element->defTransformCLHEP();
      v0=idealTransform*x0;
      v1=idealTransform*x1;
      std::cout << m_pixelID->show_to_string(hashID) 
	  << " " << v0.x() << " " << v0.y() << " " << v0.z()
	  << " " << v1.x() << " " << v1.y() << " " << v1.z()
	  << std::endl;
      HepGeom::Transform3D localTrans=idealTransform.inverse()*trans;
//      m_transforms->add(hashID,Amg::CLHEPTransformToEigen(localTrans)); 
	 m_transforms->add(hashID,localTrans); 
      // p_iddbtool->setTrans(hashID,3,trans);
      for (int j=0; j<distosize; j++) 
	disto[j]=theStave->module[i].disto[j];
      m_distorsions->add(hashID,disto);
    }
    theStave++;
  }
  delete[] disto;
  
  m_transforms->sortv();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode PixelBarrelSurvey::finalize() {

  msg(MSG::INFO) << "finalize()" << endmsg;

  std::ofstream outfile(m_outputDataFile.c_str());
  outfile.precision(7);
  std::ofstream distfile(m_outputDistoFile.c_str());
  distfile.precision(7);

  AlignableTransform::AlignTransMem_itr i=m_transforms->mbegin();
  AlignableTransform::AlignTransMem_itr end=m_transforms->mend();
  while (i!=end) {
    HepGeom::Transform3D trans=i->transform(); 
    //HepGeom::Transform3D trans=Amg::EigenTransformToCLHEP(i->transform()); 
    Identifier id=i->identify();    
    int det,bec,layer,ring,sector,side;
    float dx,dy,dz,phi,theta,psi;
    det    =1;
    bec    = m_pixelID->barrel_ec(id);
    layer  = m_pixelID->layer_disk(id);
    ring   = m_pixelID->phi_module(id);
    sector = m_pixelID->eta_module(id);
    side   = 0;
    CLHEP::Hep3Vector shift=trans.getTranslation();
    CLHEP::HepRotation rot=trans.getRotation();
    dx=shift.x();
    dy=shift.y();
    dz=shift.z();
    if ( m_oldOutputFormat ) {
      phi=rot.getPhi();
      theta=rot.getTheta();
      psi=rot.getPsi();
    } else {
      double alpha, beta, gamma;
      double siny = trans(0,2);
      beta = asin(siny);
      // Check if cosy = 0. This requires special treatment.
      // can check either element (1,2),(2,2) both equal zero
      // or (0,1) and (0,0)
      // Probably not likely it will be exactly 0 and may still 
      // have some problems when very close to zero. We mostly
      // deal with small rotations so its not too important.
      if ((trans(1,2) == 0) && (trans(2,2) == 0)) {
	// alpha and gamma are degenerate. We arbitrarily choose
	// gamma = 0.
	gamma = 0;
	alpha = atan2(trans(1,1),trans(2,1));
      } else {
	alpha = atan2(-trans(1,2),trans(2,2));
	gamma = atan2(-trans(0,1),trans(0,0));
	if (alpha == 0) alpha = 0; // convert -0 to 0
	if (gamma == 0) gamma = 0; // convert -0 to 0
      }
      phi  =alpha/CLHEP::mrad;
      theta=beta /CLHEP::mrad;
      psi  =gamma/CLHEP::mrad;
    }
    outfile  << det << " " << bec << " " << layer << " " << ring 
	     << " " << sector << " " << side << " " 
	     << dx << " "  << dy << " " << dz << " " 
	     << phi << " " << theta << " " << psi << std::endl;
    const float *disto=m_distorsions->find(id);
    distfile << std::hex << "0x" << id;
    for ( int j=0; j<m_distorsions->size(); j++) distfile << " " << disto[j];
    distfile << std::endl;
    i++;
  }
  outfile.close();
  distfile.close();
  delete m_transforms;
  // m_distorsions->print2();
  delete m_distorsions;
  // p_iddbtool->writeFile(false,m_outputDataFile);
  return StatusCode::SUCCESS;
}
 
