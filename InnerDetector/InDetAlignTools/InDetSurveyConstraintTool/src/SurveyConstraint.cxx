/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSurveyConstraintTool/SurveyConstraint.h"
#include "InDetSurveyConstraintTool/SimpleConstraintPointMinimizer.h"

// Gaudi & StoreGate
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "AthenaKernel/IAthenaOutputStreamTool.h"

// CLHEP includes

// Geometry Stuff 
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "RegistrationServices/IIOVRegistrationSvc.h" 

// Alignment DB StuffStuff
#include "InDetAlignGenTools/IInDetAlignDBTool.h"
//#include "InDetAlignTools/IInDetAlignDBTool.h"

// CLHEP includes

////namespace InDetSurveyConstraintTool {


/////////////////////////////////////////////////////////////////////////////

SurveyConstraint::SurveyConstraint(const std::string& type,
				   const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_pixelManager(0),
    m_SCT_Manager(0),
    m_pixid(0),
    m_sctid(0),
    m_toolsvc(0),     
    current_IDAlignDBTool(0),
    survey_IDAlignDBTool(0),
    randsvc(0), 
    m_SurveyWeightX(1.0),
    m_SurveyWeightY(1.0),
    m_SurveyWeightZ(1.0),
    m_SurveyWeightPhiX(1.0),
    m_SurveyWeightPhiY(1.0),
    m_SurveyWeightPhiZ(1.0),
    m_TransX(0.0),
    m_TransY(0.0),
    m_TransZ(0.0),
    m_RotX(0.0),
    m_RotX2(0.0),
    m_RotY(0.0),
    m_RotZ(0.0),
    m_TransXRand(0.0046),
    m_TransYRand(0.0047),
    m_TransZRand(0.0127),
    m_RotXRand(0.0003),
    m_RotYRand(0.0007),
    m_RotZRand(0.00012),
    m_TransXRandPixB(0.01),
    m_TransYRandPixB(0.01),
    m_TransZRandPixB(0.1),
    m_RotXRandPixB(0.001),
    m_RotYRandPixB(0.001),
    m_RotZRandPixB(0.00024),
    m_TransXRandSCTEC(0.03),
    m_TransYRandSCTEC(0.03),
    m_TransZRandSCTEC(0.03),
    m_RotXRandSCTEC(0.001),
    m_RotYRandSCTEC(0.001),
    m_RotZRandSCTEC(0.0003),
    m_TransXRandSCTB(0.1),
    m_TransYRandSCTB(0.1),
    m_TransZRandSCTB(0.1),
    m_RotXRandSCTB(0.001),
    m_RotYRandSCTB(0.001),
    m_RotZRandSCTB(0.001),
    m_gaus(false),
    m_TransXRandSect(0.0),
    m_TransYRandSect(0.0),
    m_TransZRandSect(0.0),
    m_RotXRandSect(0.0),
    m_RotYRandSect(0.0),
    m_RotZRandSect(0.0),
    m_TransLayerRand(-1),
    m_misaligncase(-1),
    m_gausSect(false),
    m_FullDisk(false),
    m_scaleZ(1.0),
    m_proximity(999999.),
    m_aligndbtoolinst("InDetCurrentDBTool"),
    m_surveydbtoolinst("InDetSurveyDBTool"),
    m_surveywfile(""),
    m_surveyrfile(""),
    m_ntuple(false)
 {
  declareInterface<ISurveyConstraint>(this);
  declareProperty("SurveyWeightX"              ,     m_SurveyWeightX);
  declareProperty("SurveyWeightY"              ,     m_SurveyWeightY);
  declareProperty("SurveyWeightZ"              ,     m_SurveyWeightZ);
  declareProperty("SurveyWeightPhiX"           ,     m_SurveyWeightPhiX);
  declareProperty("SurveyWeightPhiY"           ,     m_SurveyWeightPhiY);
  declareProperty("SurveyWeightPhiZ"           ,     m_SurveyWeightPhiZ);
  declareProperty("TransX"                     ,     m_TransX);
  declareProperty("TransY"                     ,     m_TransY);
  declareProperty("TransZ"                     ,     m_TransZ);
  declareProperty("RotX"                       ,     m_RotX);
  declareProperty("RotX2"                      ,     m_RotX2);
  declareProperty("RotY"                       ,     m_RotY);
  declareProperty("RotZ"                       ,     m_RotZ);
  declareProperty("TransXRand"                 ,     m_TransXRand);
  declareProperty("TransYRand"                 ,     m_TransYRand);
  declareProperty("TransZRand"                 ,     m_TransZRand);
  declareProperty("RotXRand"                   ,     m_RotXRand);
  declareProperty("RotYRand"                   ,     m_RotYRand);
  declareProperty("RotZRand"                   ,     m_RotZRand);
  declareProperty("TransXRandPixB"             ,     m_TransXRandPixB);
  declareProperty("TransYRandPixB"             ,     m_TransYRandPixB);
  declareProperty("TransZRandPixB"             ,     m_TransZRandPixB);
  declareProperty("RotXRandPixB"               ,     m_RotXRandPixB);
  declareProperty("RotYRandPixB"               ,     m_RotYRandPixB);
  declareProperty("RotZRandPixB"               ,     m_RotZRandPixB);
  declareProperty("TransXRandSCTEC"            ,     m_TransXRandSCTEC);
  declareProperty("TransYRandSCTEC"            ,     m_TransYRandSCTEC);
  declareProperty("TransZRandSCTEC"            ,     m_TransZRandSCTEC);
  declareProperty("RotXRandSCTEC"              ,     m_RotXRandSCTEC);
  declareProperty("RotYRandSCTEC"              ,     m_RotYRandSCTEC);
  declareProperty("RotZRandSCTEC"              ,     m_RotZRandSCTEC);
  declareProperty("TransXRandSCTB"             ,     m_TransXRandSCTB);
  declareProperty("TransYRandSCTB"             ,     m_TransYRandSCTB);
  declareProperty("TransZRandSCTB"             ,     m_TransZRandSCTB);
  declareProperty("RotXRandSCTB"               ,     m_RotXRandSCTB);
  declareProperty("RotYRandSCTB"               ,     m_RotYRandSCTB);
  declareProperty("RotZRandSCTB"               ,     m_RotZRandSCTB);
  declareProperty("Gaus"                       ,     m_gaus);
  declareProperty("TransXRandSect"             ,     m_TransXRandSect);
  declareProperty("TransYRandSect"             ,     m_TransYRandSect);
  declareProperty("TransZRandSect"             ,     m_TransZRandSect);
  declareProperty("RotXRandSect"               ,     m_RotXRandSect);
  declareProperty("RotYRandSect"               ,     m_RotYRandSect);
  declareProperty("RotZRandSect"               ,     m_RotZRandSect);
  declareProperty("TransLayerRand"             ,     m_TransLayerRand);
  declareProperty("Misaligncase"               ,     m_misaligncase);
  declareProperty("GausSect"                   ,     m_gausSect);
  declareProperty("FullDisk"                   ,     m_FullDisk);
  declareProperty("ScaleZ"                     ,     m_scaleZ);
  declareProperty("Proximity"                  ,     m_proximity);  
  declareProperty("CurrentDBToolInst"          ,     m_aligndbtoolinst);
  declareProperty("SurveyDBToolInst"           ,     m_surveydbtoolinst);
  declareProperty("SurveyWfile"                ,     m_surveywfile);
  declareProperty("SurveyRFile"                ,     m_surveyrfile);
  declareProperty("Ntuple"                     ,     m_ntuple);
}

SurveyConstraint::~SurveyConstraint()
{
  //  delete m_idHelper;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode SurveyConstraint::initialize(){
  
  // Part 1: Get the messaging service, print where you are
  msg(MSG::INFO) << "SurveyConstraint initialize()" << endmsg;

  // Get The ToolSvc
  StatusCode sc = service("ToolSvc",m_toolsvc);
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not find ToolSvc. Exiting." << endmsg;
    return sc;
  }
  msg(MSG::INFO) << "got ToolSvc" << endmsg;
  
  // Get current InDetAlignDataBaseTool from ToolService
  sc = m_toolsvc->retrieveTool("InDetAlignDBTool",m_aligndbtoolinst,current_IDAlignDBTool);
  if (sc.isFailure()) {
    msg(MSG::FATAL) <<"Could not find InDetAlignDBTool. Exiting."<<endmsg;
    return sc;
  }
  msg(MSG::INFO) << "got current_IDAlignDBTool" << endmsg;
  msg(MSG::INFO) << "current_IDAlignDBTool name = " << current_IDAlignDBTool->name() << endmsg;
  
  // Get survey InDetAlignDataBaseTool from ToolService
  sc = m_toolsvc->retrieveTool("InDetAlignDBTool",m_surveydbtoolinst,survey_IDAlignDBTool);
  if (sc.isFailure()) {
    msg(MSG::FATAL) <<"Could not find InDetAlignDBTool. Exiting."<<endmsg;
    return sc;
  }
  msg(MSG::INFO) << "got survey_IDAlignDBTool" << endmsg;

  //ID Helper
  sc = detStore()->retrieve(m_idHelper, "AtlasID" );
  if (sc.isFailure()) {
    msg(MSG::WARNING) << "Could not get AtlasDetectorID !" << endmsg;
    return StatusCode::SUCCESS;
  }else{
    if (msgLvl(MSG::DEBUG)) msg() << "Found AtlasDetectorID" << endmsg;
  }

  // get ID helpers from detector store (relying on GeoModel to put them)
  if ((StatusCode::SUCCESS!=detStore()->retrieve(m_pixid)) ||
      (StatusCode::SUCCESS!=detStore()->retrieve(m_sctid))) {
    msg(MSG::FATAL) << "Problem retrieving ID helpers" << endmsg;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "got ID helpers from detector store (relying on GeoModel to put them)" << endmsg;

  // get PixelManager
  sc = detStore()->retrieve(m_pixelManager, "Pixel");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get PixelManager !" << endmsg;
    return sc;
  }
  msg(MSG::INFO) << "got m_pixelManager" << endmsg;
  
  // get SCTManager
  sc = detStore()->retrieve(m_SCT_Manager, "SCT");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get SCT_Manager !" << endmsg;
    return sc;
  }
  msg(MSG::INFO) << "got m_SCT_Manager" << endmsg; 

  // random number service
  if (StatusCode::SUCCESS!=service("RndmGenSvc",randsvc,true))
    msg(MSG::ERROR) << "Cannot find RndmGenSvc" << endmsg;
  
  // Protection against singular weight matrix
  if (m_surveywfile==""){
    if(m_TransXRand<1.E-7) m_TransXRand=1.E-7;
    if(m_TransYRand<1.E-7) m_TransYRand=1.E-7;
    if(m_TransZRand<1.E-7) m_TransZRand=1.E-7;
    if(m_RotXRand<1.E-7) m_RotXRand=1.E-7;
    if(m_RotYRand<1.E-7) m_RotYRand=1.E-7;
    if(m_RotZRand<1.E-7) m_RotZRand=1.E-7;
    if(m_TransXRandPixB<1.E-7) m_TransXRandPixB=1.E-7;
    if(m_TransYRandPixB<1.E-7) m_TransYRandPixB=1.E-7;
    if(m_TransZRandPixB<1.E-7) m_TransZRandPixB=1.E-7;
    if(m_RotXRandPixB<1.E-7) m_RotXRandPixB=1.E-7;
    if(m_RotYRandPixB<1.E-7) m_RotYRandPixB=1.E-7;
    if(m_RotZRandPixB<1.E-7) m_RotZRandPixB=1.E-7;
    //
    if(m_TransXRandSCTEC<1.E-7) m_TransXRandSCTEC=1.E-7;
    if(m_TransYRandSCTEC<1.E-7) m_TransYRandSCTEC=1.E-7;
    if(m_TransZRandSCTEC<1.E-7) m_TransZRandSCTEC=1.E-7;
    if(m_RotXRandSCTEC<1.E-7) m_RotXRandSCTEC=1.E-7;
    if(m_RotYRandSCTEC<1.E-7) m_RotYRandSCTEC=1.E-7;
    if(m_RotZRandSCTEC<1.E-7) m_RotZRandSCTEC=1.E-7;
    if(m_TransXRandSCTB<1.E-7) m_TransXRandSCTB=1.E-7;
    if(m_TransYRandSCTB<1.E-7) m_TransYRandSCTB=1.E-7;
    if(m_TransZRandSCTB<1.E-7) m_TransZRandSCTB=1.E-7;
    if(m_RotXRandSCTB<1.E-7) m_RotXRandSCTB=1.E-7;
    if(m_RotYRandSCTB<1.E-7) m_RotYRandSCTB=1.E-7;
    if(m_RotZRandSCTB<1.E-7) m_RotZRandSCTB=1.E-7;
  }
  msg(MSG::INFO) << "now entering SurveyConstraint::setup_SurveyConstraintModules()" << endmsg;
  setup_SurveyConstraintModules();
  //if (m_surveywfile!="") 
  return StatusCode::SUCCESS;
 }
  
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode SurveyConstraint::finalize() {
  
  // Part 1: Get the messaging service, print where you are
  msg(MSG::INFO) << "finalize()" << endmsg;
  
  std::map<Identifier, SurveyConstraintModule*, std::less<Identifier> >::iterator it;
  for (it = m_ModuleMap.begin(); it != m_ModuleMap.end(); it++) {
    delete it->second;
  }
  m_ModuleMap.clear();
  
  return StatusCode::SUCCESS;
}


  void SurveyConstraint::MMap(std::map<Identifier, SurveyConstraintModule*, std::less<Identifier> >&  ModuleMap) {
    ModuleMap = m_ModuleMap;
  }


StatusCode SurveyConstraint::computeConstraint(const Identifier& ModuleID,
					       Amg::VectorX& dparams,         // alignment parameters
					       double& deltachisq,
					       Amg::VectorX& DOCA_Vector,  
					       Amg::MatrixX& DOCA_Matrix) {
  AmgVector(6) weightfactor;
  weightfactor[0] = m_SurveyWeightX;
  weightfactor[1] = m_SurveyWeightY;
  weightfactor[2] = m_SurveyWeightZ;
  weightfactor[3] = m_SurveyWeightPhiX;
  weightfactor[4] = m_SurveyWeightPhiY;
  weightfactor[5] = m_SurveyWeightPhiZ;

  
  //get the SurveyConstraintModule from the ModuleMap, it knows about the global 
  //Module-SurveyConstraintPoints, and the global Stave-SurveyConstraintPoints
  SurveyConstraintModule* mut = m_ModuleMap[ModuleID];
  
  // which subdetector?
  bool isPixEC = false, isPixB = false, isSCTEC = false, isSCTB = false;
  if(mut->isPixel()){
    if(abs(m_pixid->barrel_ec(ModuleID)) == 2) isPixEC = true;
    else if(m_pixid->barrel_ec(ModuleID) == 0) isPixB  = true;
  } else {
    if(abs(m_sctid->barrel_ec(ModuleID)) == 2) isSCTEC = true;
    else if(m_sctid->barrel_ec(ModuleID) == 0) isSCTB  = true;
  }

  //Define the transform which minimizes the distance between the set of
  //point pairs: this effectively defines the transform between the survey
  //and current alignment of the SOW.
  SimpleConstraintPointMinimizer _minimizer(m_proximity);
  Amg::Vector3D stavetrans;
  Amg::Vector3D staveangles;
  std::vector< SurveyConstraintPoint > Stavepoints;
  mut->getPoints(Stavepoints,SurveyConstraintModule::Stave);
  if (msgLvl(MSG::DEBUG)) msg() << "SurveyConstraint().computeConstraint: Stavepoints.size() " << Stavepoints.size() << endmsg;
  // transform GlobalToLocal
  GlobalToLocal(mut,Stavepoints);

  // scale z coordinate
  for (unsigned int iPoint(0); iPoint < Stavepoints.size(); ++iPoint ) {
    Amg::Vector3D survey = Stavepoints[iPoint].survey();
    if (msgLvl(MSG::DEBUG)) msg() << "Survey Stavepoints before: " << survey.x() << "," << survey.y() << "," << survey.z() << endmsg;
    SurveyConstraintPoint& Stavepoint = Stavepoints[iPoint];
    Stavepoint.scaleZ(m_scaleZ);
    survey = Stavepoints[iPoint].survey();
    if (msgLvl(MSG::DEBUG)) msg() << " and after: " << survey.x() << "," << survey.y() << "," << survey.z() << endmsg;
  }

  if (msgLvl(MSG::DEBUG)) msg() << "SurveyConstraint().computeConstraint: Now fitting the 2 Staves" << endmsg;
  double stavemin = _minimizer.findMinimum(Stavepoints,staveangles,stavetrans);

  
  
  
  stavetrans[2] = (stavetrans.z()/m_scaleZ);
  if (msgLvl(MSG::DEBUG)) msg() << "Stavepoints translation and rotations: (" << 
    stavetrans.x() << "," << stavetrans.y() << "," << stavetrans.z() << "," <<
    staveangles.x()/m_scaleZ << "," << staveangles.y()/m_scaleZ << "," << staveangles.z() << ")" << 
    endmsg;
      
  
  if(stavemin < 0.0){
    msg(MSG::FATAL) << "insufficient Points for Stave Fitting" << endmsg;
    return StatusCode::FAILURE;
  }  

  //(***) create the transform from survey to current with these parameters
  Amg::Translation3D amgstavetrans(stavetrans);
  Amg::Transform3D staveTransform = amgstavetrans * Amg::RotationMatrix3D::Identity();
  staveTransform *= Amg::AngleAxis3D(staveangles[2], Amg::Vector3D(0.,0.,1.));
  staveTransform *= Amg::AngleAxis3D(staveangles[1]/m_scaleZ, Amg::Vector3D(0.,1.,0.));
  staveTransform *= Amg::AngleAxis3D(staveangles[0]/m_scaleZ, Amg::Vector3D(1.,0.,0.));


  //now, get the points on the MUT, and move to the MUTs frame
  //we must get the points from the wafer itself
  std::vector< SurveyConstraintPoint > Modulepoints;
  mut->getPoints(Modulepoints,SurveyConstraintModule::Module);

  // transform GlobalToLocal
  GlobalToLocal(mut,Modulepoints);
  

  //transform ONLY the survey points(survey,-) according to the transform from survey to current from the SOW (***)
  for(unsigned ipoint=0;ipoint<Modulepoints.size();ipoint++){
    Amg::Vector3D& survey = Modulepoints[ipoint].survey();
    survey = staveTransform * survey;
  }

  //now compute the final parameters: build the 3D residuals between the two sets of MUT points
  Amg::Vector3D modtrans;
  Amg::Vector3D modangles;
  if (msgLvl(MSG::DEBUG)) msg() << "SurveyConstraint().computeConstraint: Now fitting the 2 Modules" << endmsg;
  double modmin = _minimizer.findMinimum(Modulepoints,modangles,modtrans);
  if(modmin < 0.0){
    msg(MSG::FATAL) << "insufficient Points for Module Fitting" << endmsg;
    return StatusCode::FAILURE;
  }  

  // fill the dparams vector
  for(unsigned ipar=0;ipar<3;ipar++)
    dparams[ipar] = modtrans[ipar];
  dparams[3] = modangles[0];
  dparams[4] = modangles[1];
  dparams[5] = modangles[2];
  
  // for this get the errors (weight) on the points (actually independent of MUTid)
  Amg::MatrixX weight;
  bool ierr=-1;
  if(isPixEC) ierr = getWeightPixEC(//ModuleID,
				    weight);
  else if(isPixB)  ierr = getWeightPixB(//ModuleID,
					weight);
  else if(isSCTEC) ierr = getWeightSCTEC(//ModuleID,
					 weight);
  else if(isSCTB)  ierr = getWeightSCTB(//ModuleID,
					weight);
  if(ierr != 0){
    msg(MSG::FATAL) << "matrixInvertFail" << endmsg;
    if(isPixEC) msg(MSG::FATAL) << "for PixEC" << endmsg;
    else if(isPixB) msg(MSG::FATAL) << "for PixB" << endmsg;
    else if(isSCTEC) msg(MSG::FATAL) << "for SCTEC" << endmsg;
    else if(isSCTB) msg(MSG::FATAL) << "for SCTB" << endmsg;    
    return StatusCode::FAILURE;
  }  

  // and asign additional weightfactor to simulate systematic uncertainties (bowing etc.)
  // Don't need this: Put systematics already in weight matrix!!!
//   for(unsigned icol=1;icol<=6;icol++)
//     for(unsigned irow=1;irow<=icol;irow++)
//       weight.fast(icol,irow) *= weightfactor(icol)*weightfactor(irow);
  
  // now get the chi2, add to the vector and the matrix.
  Amg::MatrixX temp =  dparams.transpose() * weight * dparams; 
  msg(MSG::ERROR) << "Chech that the size of the matrix is a 1,1: " << temp.rows() << ", " << temp.cols() << endmsg;
  deltachisq = temp(0,0);


  // the derivatives are trivial, since we're already in the wafers local
  // coordinate system.  The factor of 2 comes from the dfn of chisquared, the
  // sign from the convention that we measure dparams as current-survey
  DOCA_Vector = 2.0*weight*dparams;       // dchisqdparams
  DOCA_Matrix = 2.0*weight;               // d2chisqdpdp
  
  return StatusCode::SUCCESS;
}


void SurveyConstraint::setup_SurveyConstraintModules()
{
  Rndm::Numbers gauss(randsvc,Rndm::Gauss(0.,1.));
  
  // read in or write an alignment file for the survey alignment, 
  // either a text file (SurveyText.txt) or an ntuple (reading in and writing out
  // an ntuple does not work at the moment). For this create a DB 
  // an set DBRoot to "/Indet/SiSurvey"
  if (m_surveyrfile!="" || m_surveywfile!="") survey_IDAlignDBTool->createDB();
  if (m_surveyrfile!=""){ 
    if(m_ntuple) survey_IDAlignDBTool->readNtuple(m_surveyrfile);
    else survey_IDAlignDBTool->readTextFile(m_surveyrfile);
  }
  
  // create some displacements at level 1/2/3, using only one parameter m_TransLayerRand
  if(m_TransLayerRand > 0){
    // displace all layers randomly
    if(m_misaligncase == 0) 
      survey_IDAlignDBTool->dispGroup(-1, -1, -1, -1, -1, m_TransLayerRand, m_TransLayerRand, m_TransLayerRand, 2, 2, 0);
    // displace pixel EC layer 0 in x
    else if(m_misaligncase == 1) 
      survey_IDAlignDBTool->dispGroup(1, 2, 0, -1, -1, m_TransLayerRand, 0, 0, 1, 2, 0);
    // displace pixel EC layer 2 in x
    else if(m_misaligncase == 2) 
      survey_IDAlignDBTool->dispGroup(1, 2, 2, -1, -1, m_TransLayerRand, 0, 0, 1, 2, 0);
  }

  int nSCT(0), nPixel(0);
  InDetDD::SiDetectorElementCollection::const_iterator iter, iter2;  
  std::vector< Amg::Vector3D > localSurveyCoords;
  Amg::Transform3D SurveyTransRand, SurveyTransRandSect;
  SurveyTransRand.setIdentity();
  SurveyTransRandSect.setIdentity();

  unsigned int nSCTMod = 0,nSCTModInMap = 0,nSCTModEC = 0,nSCTModPointsEC = 0;
  for (iter = m_SCT_Manager->getDetectorElementBegin(); iter != m_SCT_Manager->getDetectorElementEnd(); ++iter) {
    const Identifier SCT_ModuleID = (*iter)->identify(); 
    if(m_sctid->side(SCT_ModuleID) != 0) continue;
    ++nSCTMod;

    if (m_ModuleMap.find(SCT_ModuleID) == m_ModuleMap.end()) {
      ++nSCTModInMap;
      SurveyConstraintModule* newSCT_Module = new SurveyConstraintModule(SCT_ModuleID,false);
      Amg::Transform3D globaltolocal = (*iter)->transform().inverse();
      newSCT_Module->set_globaltolocal(globaltolocal);
      m_ModuleMap[SCT_ModuleID] = newSCT_Module;
      ++nSCT;
      if (msgLvl(MSG::DEBUG)) msg() << "new SCT Module " << nSCT << endmsg;

      

      // Get the nominal local coordinates (which are the same for all wafers, and for survey and current)
      // add SCT EC SurveyCoords to SurveyConstraintModule
      if(abs(m_sctid->barrel_ec(SCT_ModuleID)) == 2) getSurveyCoordsSCTEC(//SCT_ModuleID,
									  localSurveyCoords);
      // add SCT Barrel SurveyCoords to SurveyConstraintModule
      else if(m_sctid->barrel_ec(SCT_ModuleID) == 0) getSurveyCoordsSCTB(//SCT_ModuleID,
									 localSurveyCoords);
      // get the survey, current transformations from nominal
      Amg::Transform3D SurveyTrans = survey_IDAlignDBTool->getTrans(SCT_ModuleID,3);
      Amg::Transform3D CurrentTrans = current_IDAlignDBTool->getTrans(SCT_ModuleID,3);

      if(m_gaus){
        if(abs(m_sctid->barrel_ec(SCT_ModuleID)) == 2){
          double m1 = m_TransXRandSCTEC*gauss();
          double m2 = m_TransYRandSCTEC*gauss();
          double m3 = m_TransZRandSCTEC*gauss();
          double m4 = m_RotXRandSCTEC*gauss();
          double m5 = m_RotYRandSCTEC*gauss();
          double m6 = m_RotZRandSCTEC*gauss();
          SurveyTransRand =  Amg::Translation3D(m1,m2,m3) * Amg::RotationMatrix3D::Identity() 
                            * Amg::AngleAxis3D(m6, Amg::Vector3D(0.,0.,1.)) 
                            * Amg::AngleAxis3D(m5, Amg::Vector3D(0.,1.,0.)) 
                            * Amg::AngleAxis3D(m4, Amg::Vector3D(1.,0.,0.));
          if (m_surveywfile!=""){
            Amg::VectorX Misalign_Vector(6);
            Misalign_Vector[0]=m1;Misalign_Vector[1]=m2;Misalign_Vector[2]=m3;Misalign_Vector[3]=m4;Misalign_Vector[4]=m5;Misalign_Vector[5]=m6;
            newSCT_Module->set_DOCA_Vector(Misalign_Vector);
          }
        }
        else if(m_sctid->barrel_ec(SCT_ModuleID) == 0) {
          double m1 = m_TransXRandSCTB*gauss();
          double m2 = m_TransYRandSCTB*gauss();
          double m3 = m_TransZRandSCTB*gauss();
          double m4 = m_RotXRandSCTB*gauss();
          double m5 = m_RotYRandSCTB*gauss();
          double m6 = m_RotZRandSCTB*gauss();
          SurveyTransRand =  Amg::Translation3D(m1,m2,m3) * Amg::RotationMatrix3D::Identity() 
                            * Amg::AngleAxis3D(m6, Amg::Vector3D(0.,0.,1.)) 
                            * Amg::AngleAxis3D(m5, Amg::Vector3D(0.,1.,0.)) 
                            * Amg::AngleAxis3D(m4, Amg::Vector3D(1.,0.,0.));
          if (m_surveywfile!=""){
            Amg::VectorX Misalign_Vector(6);
            Misalign_Vector[0]=m1;Misalign_Vector[1]=m2;Misalign_Vector[2]=m3;Misalign_Vector[3]=m4;Misalign_Vector[4]=m5;Misalign_Vector[5]=m6;
            newSCT_Module->set_DOCA_Vector(Misalign_Vector);
          }
        }
      }
      
      // add constraint points to "SurveyConstraintModule newSCT_Module" in global coords
      for ( unsigned int iCorn(0); iCorn < localSurveyCoords.size(); ++iCorn ) {
        Amg::Vector3D temp = localSurveyCoords[iCorn];
        // Transform the local points into the MUT's (survey and current) local coordinate system	
        Amg::Vector3D surveyPoint = (SurveyTrans*SurveyTransRand) * localSurveyCoords[iCorn];
        if(m_TransLayerRand <= 0) survey_IDAlignDBTool->setTrans(SCT_ModuleID,3,SurveyTrans*SurveyTransRand);
        //survey_IDAlignDBTool->tweakTrans(SCT_ModuleID,3,SurveyTransRand);
        Amg::Vector3D currentPoint = CurrentTrans * temp;
        // Transform the local (survey and current) constraint points into the global coordinate system
        Amg::VectorX globalSurveyPoint  = (*iter)->globalPosition( surveyPoint );	  
        Amg::VectorX globalCurrentPoint = (*iter)->globalPosition( currentPoint);

        SurveyConstraintPoint newCPoint( globalSurveyPoint, globalCurrentPoint );            
        newSCT_Module->addModuleConstraintPoint(newCPoint);	
      }	
    }
  }
  
  bool first = true, NewDisk = true, NewSector = true, firstB = true;
  unsigned int nPixMod = 0,nPixModInMap = 0,nPixModEC = 0,nPixModPointsEC = 0;
  int previous_disk = -1, previous_sector = -1;
  for (iter = m_pixelManager->getDetectorElementBegin(); iter != m_pixelManager->getDetectorElementEnd(); ++iter) {
    const Identifier Pixel_ModuleID = (*iter)->identify(); 
    ++nPixMod;
    
    if (m_ModuleMap.find(Pixel_ModuleID) == m_ModuleMap.end()) {
      ++nPixModInMap;
      SurveyConstraintModule* newPixel_Module = new SurveyConstraintModule(Pixel_ModuleID,true);
      Amg::Transform3D globaltolocal = (*iter)->transform().inverse();
      newPixel_Module->set_globaltolocal(globaltolocal);
      m_ModuleMap[Pixel_ModuleID] = newPixel_Module;
      ++nPixel;
      if (msgLvl(MSG::DEBUG)) msg() << "new Pixel Module " << nPixel << endmsg;
      
      // add Pixel EC SurveyCoords
      if(abs(m_pixid->barrel_ec(Pixel_ModuleID)) == 2){
        ++nPixModEC;
        // Get the nominal local coordinates (which are the same for all wafers, and for survey and current)
        getSurveyCoordsPixEC(//Pixel_ModuleID,
                 localSurveyCoords);
        // get the survey, current transformations from nominal
        Amg::Transform3D SurveyTrans = survey_IDAlignDBTool->getTrans(Pixel_ModuleID,3);
        Amg::Transform3D CurrentTrans = current_IDAlignDBTool->getTrans(Pixel_ModuleID,3);


        // ********************************************
        // Do some tests for first Pixel EC module
        if(previous_disk == m_pixid->layer_disk(Pixel_ModuleID) && previous_sector == SectorNumber(m_pixid->phi_module(Pixel_ModuleID)))
          NewSector=false;
        else NewSector=true;
        if(previous_disk == m_pixid->layer_disk(Pixel_ModuleID))
          NewDisk=false;
        else NewDisk=true;
        if (NewSector && (!m_gausSect || NewDisk)){ 
          CurrentTrans = CurrentTrans * Amg::Translation3D(m_TransX,m_TransY,m_TransZ) 
                                      * Amg::AngleAxis3D(m_RotZ, Amg::Vector3D(0.,0.,1.)) 
                                      * Amg::AngleAxis3D(m_RotY, Amg::Vector3D(0.,1.,0.)) 
                                      * Amg::AngleAxis3D(m_RotX, Amg::Vector3D(1.,0.,0.))
                                      * Amg::AngleAxis3D(m_RotX2, Amg::Vector3D(1.,0.,0.));

        }
  
        if (m_gaus){
          double m1 = m_TransXRand*gauss();
          double m2 = m_TransYRand*gauss();
          double m3 = m_TransZRand*gauss();
          double m4 = m_RotXRand*gauss();
          double m5 = m_RotYRand*gauss();
          double m6 = m_RotZRand*gauss();
          SurveyTransRand = Amg::Translation3D(m1,m2,m3) * Amg::RotationMatrix3D::Identity() 
                            * Amg::AngleAxis3D(m6, Amg::Vector3D(0.,0.,1.)) 
                            * Amg::AngleAxis3D(m5, Amg::Vector3D(0.,1.,0.)) 
                            * Amg::AngleAxis3D(m4, Amg::Vector3D(1.,0.,0.));   
            
            
          if (m_surveywfile!=""){
            Amg::VectorX Misalign_Vector(6);
            Misalign_Vector[0]=m1;Misalign_Vector[1]=m2;Misalign_Vector[2]=m3;Misalign_Vector[3]=m4;Misalign_Vector[4]=m5;Misalign_Vector[5]=m6;
            newPixel_Module->set_DOCA_Vector(Misalign_Vector);
          }
        }
        if(m_gausSect && NewSector){
          double m1 = m_TransXRandSect*gauss();
          double m2 = m_TransYRandSect*gauss();
          double m3 = m_TransZRandSect*gauss();
          double m4 = m_RotXRandSect*gauss();
          double m5 = m_RotYRandSect*gauss();
          double m6 = m_RotZRandSect*gauss();
          SurveyTransRandSect  =  Amg::Translation3D(m1,m2,m3) * Amg::RotationMatrix3D::Identity() 
                            * Amg::AngleAxis3D(m6, Amg::Vector3D(0.,0.,1.)) 
                            * Amg::AngleAxis3D(m5, Amg::Vector3D(0.,1.,0.)) 
                            * Amg::AngleAxis3D(m4, Amg::Vector3D(1.,0.,0.));
          
        }
        // ********************************************
  
  
        // add constraint points to "SurveyConstraintModule newPixel_Module" in global coords
        for ( unsigned int iCorn(0); iCorn < localSurveyCoords.size(); ++iCorn ) {
          ++nPixModPointsEC;
          Amg::Vector3D temp = localSurveyCoords[iCorn];
          // Transform the local points into the MUT's (survey and current) local coordinate system
          Amg::Vector3D surveyPoint = (SurveyTrans*SurveyTransRand) * localSurveyCoords[iCorn];
          if(m_TransLayerRand <= 0) survey_IDAlignDBTool->setTrans(Pixel_ModuleID,3,SurveyTrans*SurveyTransRand);
          //survey_IDAlignDBTool->tweakTrans(Pixel_ModuleID,3,SurveyTransRand);
          Amg::Vector3D currentPoint = CurrentTrans *temp;
          //Amg::Vector3D currentPoint = temp.transform(CurrentTrans);
          // Transform the local (survey and current) constraint points into the global coordinate system
          Amg::VectorX globalSurveyPoint  = (*iter)->globalPosition( surveyPoint);	  
          Amg::VectorX globalCurrentPoint = (*iter)->globalPosition( currentPoint );



          // transform globalCurrentPoint according to Sector Transformation SurveyTransRandSect
          //TransformSector(Pixel_ModuleID,newPixel_Module,globalCurrentPoint,SurveyTransRandSect);
    
          SurveyConstraintPoint newCPoint( globalSurveyPoint, globalCurrentPoint );            
          newPixel_Module->addModuleConstraintPoint(newCPoint); 
    
          // ********************************************
          // Do some tests for first Pixel EC module 
          if (first){ 
            //if(SurveyTrans == CurrentTrans) msg(MSG::INFO) << "SurveyTrans == CurrentTrans" << endmsg;
            //if(surveyPoint == currentPoint) msg(MSG::INFO) << "surveyPoint == currentPoint" << endmsg;
            //if(globalSurveyPoint == globalCurrentPoint) msg(MSG::INFO) << "globalSurveyPoint == globalCurrentPoint" << endmsg;
            msg(MSG::INFO)  << "Local Coordinates = (" <<  localSurveyCoords[iCorn][0] << "," 
                            << localSurveyCoords[iCorn][1] << "," << localSurveyCoords[iCorn][2] << ")" << endmsg; 
            msg(MSG::INFO)  << "Survey Local Coordinates = (" <<  surveyPoint[0] << "," 
                            << surveyPoint[1] << "," << surveyPoint[2] << ")" << endmsg; 
            msg(MSG::INFO)  << "Current Local Coordinates = (" <<  currentPoint[0] << "," 
                            << currentPoint[1] << "," << currentPoint[2] << ")" << endmsg; 
            msg(MSG::INFO)  << "Survey Global Coordinates = (" <<  globalSurveyPoint[0] << "," 
                            << globalSurveyPoint[1] << "," << globalSurveyPoint[2] << ")" << endmsg; 
            msg(MSG::INFO)  << "Current Global Coordinates = (" <<  globalCurrentPoint[0] << "," 
                            << globalCurrentPoint[1] << "," << globalCurrentPoint[2] << ")" << endmsg; 
            msg(MSG::INFO) << "SurveyConstraint().setup_SurveyConstraintModules: nModulePoints " << m_ModuleMap[Pixel_ModuleID]->nModulePoints() << endmsg;
            first = false;
          }
          // ********************************************

        }
        previous_disk = m_pixid->layer_disk(Pixel_ModuleID);
        previous_sector = SectorNumber(m_pixid->phi_module(Pixel_ModuleID));  
            }

            // add Pixel Barrel SurveyCoords to SurveyConstraintModule
            if(m_pixid->barrel_ec(Pixel_ModuleID) == 0){
              // Get the nominal local coordinates (which are the same for all wafers, and for survey and current)
              getSurveyCoordsPixB(//Pixel_ModuleID,
                      localSurveyCoords);
              // get the survey, current transformations from nominal
              Amg::Transform3D SurveyTrans = survey_IDAlignDBTool->getTrans(Pixel_ModuleID,3);
              Amg::Transform3D CurrentTrans = current_IDAlignDBTool->getTrans(Pixel_ModuleID,3);

              if(m_gaus){
                double m1 = m_TransXRandPixB*gauss();
                double m2 = m_TransYRandPixB*gauss();
                double m3 = m_TransZRandPixB*gauss();
                double m4 = m_RotXRandPixB*gauss();
                double m5 = m_RotYRandPixB*gauss();
                double m6 = m_RotZRandPixB*gauss();
                SurveyTransRand  =  Amg::Translation3D(m1,m2,m3) * Amg::RotationMatrix3D::Identity() 
                                  * Amg::AngleAxis3D(m6, Amg::Vector3D(0.,0.,1.)) 
                                  * Amg::AngleAxis3D(m5, Amg::Vector3D(0.,1.,0.)) 
                                  * Amg::AngleAxis3D(m4, Amg::Vector3D(1.,0.,0.));
          
                if (m_surveywfile!=""){
                  Amg::VectorX Misalign_Vector(6);
                  Misalign_Vector[0]=m1;Misalign_Vector[1]=m2;Misalign_Vector[2]=m3;Misalign_Vector[3]=m4;Misalign_Vector[4]=m5;Misalign_Vector[5]=m6;
                  newPixel_Module->set_DOCA_Vector(Misalign_Vector);
                }
              }

              // add constraint points to "SurveyConstraintModule newPixel_Module" in global coords
              for ( unsigned int iCorn(0); iCorn < localSurveyCoords.size(); ++iCorn ) {
                Amg::Vector3D temp = localSurveyCoords[iCorn];
                // Transform the local points into the MUT's (survey and current) local coordinate system
                Amg::Vector3D surveyPoint = (SurveyTrans*SurveyTransRand) *localSurveyCoords[iCorn] ;
                if(m_TransLayerRand <= 0) survey_IDAlignDBTool->setTrans(Pixel_ModuleID,3,SurveyTrans*SurveyTransRand);
                //survey_IDAlignDBTool->tweakTrans(Pixel_ModuleID,3,SurveyTransRand);
                Amg::Vector3D currentPoint = CurrentTrans *temp;
                // Transform the local (survey and current) constraint points into the global coordinate system
                Amg::VectorX globalSurveyPoint  = (*iter)->globalPosition( surveyPoint );	  
                Amg::VectorX globalCurrentPoint = (*iter)->globalPosition( currentPoint );
    
                SurveyConstraintPoint newCPoint( globalSurveyPoint, globalCurrentPoint );            
                newPixel_Module->addModuleConstraintPoint(newCPoint); 
              } 
            }
          }
        }
        msg(MSG::INFO) << "nSCTMod " << nSCTMod 
            << ", nSCTModInMap " << nSCTModInMap
            << ", nSCTModEC " << nSCTModEC
            << ", nSCTModPointsEC " << nSCTModPointsEC
            << ", nPixMod " << nPixMod 
            << ", nPixModInMap " << nPixModInMap
            << ", nPixModEC " << nPixModEC
            << ", nPixModPointsEC " << nPixModPointsEC
            << endmsg;




        // find the set of modules (=stave=modules used to constrain the MUT) associated with this module ID
        // add the SurveyCoords from this Stave to the MUT (SurveyConstraintModule newPixel_Module),
        // exclude the points which lie actually on the MUT, to get an unbiased alignment of the two Staves (survey and current) 
        std::vector< SurveyConstraintPoint > Stavepoints;
        // Pix EC
        unsigned int nPixModEC2 = 0,nPixModPixModEC = 0,nPixModECPixModEC = 0,nSameLayer = 0,nNotIdentical = 0;
        for (iter = m_pixelManager->getDetectorElementBegin(); iter != m_pixelManager->getDetectorElementEnd(); ++iter) {
          const Identifier Pixel_ModuleID = (*iter)->identify(); 
          if(abs(m_pixid->barrel_ec(Pixel_ModuleID)) == 2){
            ++nPixModEC2;
            for (iter2 = m_pixelManager->getDetectorElementBegin(); iter2 != m_pixelManager->getDetectorElementEnd(); ++iter2) {
              ++nPixModPixModEC;
              const Identifier Pixel_ModuleID2 = (*iter2)->identify(); 
              if(m_pixid->barrel_ec(Pixel_ModuleID2) == m_pixid->barrel_ec(Pixel_ModuleID)){
                ++nPixModECPixModEC;
                if(m_pixid->layer_disk(Pixel_ModuleID2) == m_pixid->layer_disk(Pixel_ModuleID)){
                  ++nSameLayer;
                  // require Pixel_ModuleID2 and Pixel_ModuleID from same sector OR
                  // m_FullDisk=true which means use all modules from one disk
                  if(m_FullDisk || SectorNumber(m_pixid->phi_module(Pixel_ModuleID2)) == SectorNumber(m_pixid->phi_module(Pixel_ModuleID))){
                    //if(SectorNumber(m_pixid->phi_module(Pixel_ModuleID2)) == SectorNumber(m_pixid->phi_module(Pixel_ModuleID))){
                    if(Pixel_ModuleID != Pixel_ModuleID2){
                ++nNotIdentical;
                (m_ModuleMap[Pixel_ModuleID2])->getPoints(Stavepoints,SurveyConstraintModule::Module);
                (m_ModuleMap[Pixel_ModuleID])->addStaveConstraintPoint(Stavepoints); 
  
                // ********************************************
                // Do some tests for first Pixel EC module     
                if (firstB){ 
                  std::vector< SurveyConstraintPoint > Testpoints;
                  m_ModuleMap[Pixel_ModuleID]->getPoints(Testpoints,SurveyConstraintModule::Stave);
                  msg(MSG::INFO) << "SurveyConstraint().setup_SurveyConstraintModules: Stavepoints.size() (from map) " << Testpoints.size() << endmsg;
                  firstB = false;
                }
                // ********************************************
    
              }
            }
          }
        }
      }
    }  
  }
  msg(MSG::INFO) << "Loop 2, filling stave-points, nPixModEC2 " << nPixModEC2 
      << ", nPixModPixModEC " <<  nPixModPixModEC
      << ", nPixModECPixModEC " <<  nPixModECPixModEC
      << ", nSameLayer " <<  nSameLayer
      << ", nNotIdentical " <<  nNotIdentical
      << endmsg;
  
  // Pix B
  nPixModEC2 = 0;nPixModPixModEC = 0;nPixModECPixModEC = 0;nSameLayer = 0;nNotIdentical = 0;
  for (iter = m_pixelManager->getDetectorElementBegin(); iter != m_pixelManager->getDetectorElementEnd(); ++iter) {
    const Identifier Pixel_ModuleID = (*iter)->identify(); 
    if(m_pixid->barrel_ec(Pixel_ModuleID) != 0) continue;
    ++nPixModEC2;
    for (iter2 = m_pixelManager->getDetectorElementBegin(); iter2 != m_pixelManager->getDetectorElementEnd(); ++iter2) {
      ++nPixModPixModEC;
      const Identifier Pixel_ModuleID2 = (*iter2)->identify(); 
      if(m_pixid->barrel_ec(Pixel_ModuleID2) != m_pixid->barrel_ec(Pixel_ModuleID))continue;
      ++nPixModECPixModEC;
      if(m_pixid->layer_disk(Pixel_ModuleID2) != m_pixid->layer_disk(Pixel_ModuleID))continue;
      ++nSameLayer;
      // require Pixel_ModuleID2 and Pixel_ModuleID from same stave:
      if(m_pixid->phi_module(Pixel_ModuleID2) != m_pixid->phi_module(Pixel_ModuleID))continue;
      if(Pixel_ModuleID == Pixel_ModuleID2)continue;
      ++nNotIdentical;
      (m_ModuleMap[Pixel_ModuleID2])->getPoints(Stavepoints,SurveyConstraintModule::Module);
      (m_ModuleMap[Pixel_ModuleID])->addStaveConstraintPoint(Stavepoints); 
    }  
  }
  msg(MSG::INFO) << "Loop 2, filling stave-points, nPixModB2 " << nPixModEC2 
      << ", nPixModPixModB " <<  nPixModPixModEC
      << ", nPixModBPixModB " <<  nPixModECPixModEC
      << ", nSameLayer " <<  nSameLayer
      << ", nNotIdentical " <<  nNotIdentical
      << endmsg;

  // SCT EC
  nPixModEC2 = 0;nPixModPixModEC = 0;nPixModECPixModEC = 0;nSameLayer = 0;nNotIdentical = 0;
  for (iter = m_SCT_Manager->getDetectorElementBegin(); iter != m_SCT_Manager->getDetectorElementEnd(); ++iter) {
    const Identifier SCT_ModuleID = (*iter)->identify(); 
    if(m_sctid->side(SCT_ModuleID) != 0) continue;
    if(abs(m_sctid->barrel_ec(SCT_ModuleID)) != 2) continue;
    ++nPixModEC2;
    for (iter2 = m_SCT_Manager->getDetectorElementBegin(); iter2 != m_SCT_Manager->getDetectorElementEnd(); ++iter2) {
      ++nPixModPixModEC;
      const Identifier SCT_ModuleID2 = (*iter2)->identify(); 
      if(m_sctid->side(SCT_ModuleID2) != 0)continue;
      if(m_sctid->barrel_ec(SCT_ModuleID2) != m_sctid->barrel_ec(SCT_ModuleID))continue;
      ++nPixModECPixModEC;
      if(m_sctid->layer_disk(SCT_ModuleID2) != m_sctid->layer_disk(SCT_ModuleID))continue;
      ++nSameLayer;
      //if(m_sctid->eta_module(SCT_ModuleID2) != m_sctid->eta_module(SCT_ModuleID))continue;
      //if(SectorNumber(m_sctid->phi_module(SCT_ModuleID2)) != SectorNumber(m_sctid->phi_module(SCT_ModuleID)))continue;
      if(SCT_ModuleID == SCT_ModuleID2)continue;
      ++nNotIdentical;
      (m_ModuleMap[SCT_ModuleID2])->getPoints(Stavepoints,SurveyConstraintModule::Module);
      (m_ModuleMap[SCT_ModuleID])->addStaveConstraintPoint(Stavepoints); 
    }  
	
    // ********************************************
    // Do some tests for SCT EC module 	 	 
    if (m_sctid->barrel_ec(SCT_ModuleID)==2 && 
	m_sctid->layer_disk(SCT_ModuleID)==0 && 
	m_sctid->eta_module(SCT_ModuleID)==0 &&
	m_sctid->side(SCT_ModuleID) == 0 &&
	SectorNumber(m_sctid->phi_module(SCT_ModuleID)) == 0
	){ 
      std::vector< SurveyConstraintPoint > Testpoints;
      m_ModuleMap[SCT_ModuleID]->getPoints(Testpoints,SurveyConstraintModule::Stave);
      msg(MSG::INFO) << "SurveyConstraint().setup_SurveyConstraintModules: Stavepoints.size() (from map) " << Testpoints.size() << endmsg;
    }
    // ********************************************

 
  }
  msg(MSG::INFO) << "Loop 2, filling stave-points, nSCTModEC2 " << nPixModEC2 
      << ", nSCTModSCTModEC " <<  nPixModPixModEC
      << ", nSCTModECSCTModEC " <<  nPixModECPixModEC
      << ", nSameLayer " <<  nSameLayer
      << ", nNotIdentical " <<  nNotIdentical
      << endmsg;

  // SCT B
  nPixModEC2 = 0;nPixModPixModEC = 0;nPixModECPixModEC = 0;nSameLayer = 0;nNotIdentical = 0;
  for (iter = m_SCT_Manager->getDetectorElementBegin(); iter != m_SCT_Manager->getDetectorElementEnd(); ++iter) {
    const Identifier SCT_ModuleID = (*iter)->identify(); 
    if(m_sctid->side(SCT_ModuleID) != 0) continue;
    if(m_sctid->barrel_ec(SCT_ModuleID) != 0) continue;
    ++nPixModEC2;
    for (iter2 = m_SCT_Manager->getDetectorElementBegin(); iter2 != m_SCT_Manager->getDetectorElementEnd(); ++iter2) {
      ++nPixModPixModEC;
      const Identifier SCT_ModuleID2 = (*iter2)->identify(); 
      if(m_sctid->side(SCT_ModuleID2) != 0)continue;
      if(m_sctid->barrel_ec(SCT_ModuleID2) != m_sctid->barrel_ec(SCT_ModuleID))continue;
      ++nPixModECPixModEC;
      if(m_sctid->layer_disk(SCT_ModuleID2) != m_sctid->layer_disk(SCT_ModuleID))continue;
      ++nSameLayer;
      // require SCT_ModuleID2 and SCT_ModuleID from same stave:
      if(m_sctid->phi_module(SCT_ModuleID2) != m_sctid->phi_module(SCT_ModuleID))continue;
      if(SCT_ModuleID == SCT_ModuleID2)continue;
      ++nNotIdentical;
      (m_ModuleMap[SCT_ModuleID2])->getPoints(Stavepoints,SurveyConstraintModule::Module);
      (m_ModuleMap[SCT_ModuleID])->addStaveConstraintPoint(Stavepoints); 
    }  
  }
  msg(MSG::INFO) << "Loop 2, filling stave-points, nSCTModB2 " << nPixModEC2 
      << ", nSCTModSCTModB " <<  nPixModPixModEC
      << ", nSCTModBSCTModB " <<  nPixModECPixModEC
      << ", nSameLayer " <<  nSameLayer
      << ", nNotIdentical " <<  nNotIdentical
      << endmsg;
  

  // write out to Condstream1 and write out ntuple or textfile 
  if (m_surveywfile!=""){ 
    if(m_ntuple) survey_IDAlignDBTool->writeFile(true,m_surveywfile);
    else survey_IDAlignDBTool->writeFile(false,m_surveywfile);
    if (StatusCode::SUCCESS!=survey_IDAlignDBTool->outputObjs()) 
      msg(MSG::ERROR) << "Write of AlignableTransforms fails" << endmsg;
  }
}

//dumb implementation of weight calculation for PixEC
//for now which doesn't care about wafer id 
int SurveyConstraint::getWeightPixEC(//const Identifier& ModuleID,
				     Amg::MatrixX& weight) {
  
  AmgSymMatrix(6) covar;
  // in local coords, set errors to be diagonal, with values provided by Gil and Ron
  covar(0,0) = m_TransXRand*m_TransXRand;
  covar(1,1) = m_TransYRand*m_TransYRand;
  covar(2,2) = m_TransZRand*m_TransZRand;
  covar(3,3) = m_RotXRand*m_RotXRand;
  covar(4,4) = m_RotYRand*m_RotYRand;
  covar(5,5) = m_RotZRand*m_RotZRand;
  // invert    
  weight = covar.inverse();
  return 0;
} 

//dumb implementation of weight calculation for PixB
//for now which doesn't care about wafer id 
int SurveyConstraint::getWeightPixB(//const Identifier& ModuleID,
				    Amg::MatrixX& weight) {
  
  AmgSymMatrix(5) covar;
  // in local coords, set errors to be diagonal, with values to be provided by Vadim
  covar(0,0) = m_TransXRandPixB*m_TransXRandPixB;
  covar(1,1) = m_TransYRandPixB*m_TransYRandPixB;
  covar(2,2) = m_TransZRandPixB*m_TransZRandPixB;
  covar(3,3) = m_RotXRandPixB*m_RotXRandPixB;
  covar(4,4) = m_RotYRandPixB*m_RotYRandPixB;
  covar(5,5) = m_RotZRandPixB*m_RotZRandPixB;
  // invert    
  weight = covar.inverse();
  return 0;
} 

//dumb implementation of weight calculation for SCTEC
//for now which doesn't care about wafer id 
int SurveyConstraint::getWeightSCTEC(//const Identifier& ModuleID,
				     Amg::MatrixX& weight) {
  
  AmgSymMatrix(6) covar;
  // in local coords, set errors to be diagonal, with values to be provided by Steve Snow
  covar(0,0) = m_TransXRandSCTEC*m_TransXRandSCTEC;
  covar(1,1) = m_TransYRandSCTEC*m_TransYRandSCTEC;
  covar(2,2) = m_TransZRandSCTEC*m_TransZRandSCTEC;
  covar(3,3) = m_RotXRandSCTEC*m_RotXRandSCTEC;
  covar(4,4) = m_RotYRandSCTEC*m_RotYRandSCTEC;
  covar(5,5) = m_RotZRandSCTEC*m_RotZRandSCTEC;
  // invert    
  weight = covar.inverse();
  return 0;
} 

//dumb implementation of weight calculation for SCTB
//for now which doesn't care about wafer id 
int SurveyConstraint::getWeightSCTB(//const Identifier& ModuleID,
				    Amg::MatrixX& weight) {
  
  AmgSymMatrix(6) covar;
 
  // in local coords, set errors to be diagonal, with values to be provided by Stephen Gibson
  covar(0,0) = m_TransXRandSCTB*m_TransXRandSCTB;
  covar(1,1) = m_TransYRandSCTB*m_TransYRandSCTB;
  covar(2,2) = m_TransZRandSCTB*m_TransZRandSCTB;
  covar(3,3) = m_RotXRandSCTB*m_RotXRandSCTB;
  covar(4,4) = m_RotYRandSCTB*m_RotYRandSCTB;
  covar(5,5) = m_RotZRandSCTB*m_RotZRandSCTB;
  // invert    
  weight = covar.inverse();
  return 0;
} 

//stupid implementation of Pixel EC survey coordinates
void SurveyConstraint::getSurveyCoordsPixEC(//const Identifier& ModuleID,
					    std::vector< Amg::Vector3D > & coords) {
  coords.clear();
  const double m_SurveyTargetX = 17.8/2.0;
  const double m_SurveyTargetY = 59.8/2.0; 
 // 4 points
  coords.push_back(Amg::Vector3D(-m_SurveyTargetX,-m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D(-m_SurveyTargetX, m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D( m_SurveyTargetX,-m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D( m_SurveyTargetX, m_SurveyTargetY,0.0));
}

//stupid implementation of Pixel barrel survey coordinates
void SurveyConstraint::getSurveyCoordsPixB(//const Identifier& ModuleID,
					   std::vector< Amg::Vector3D > & coords) {
  coords.clear();
  const double m_SurveyTargetX = 17.8/2.0;
  const double m_SurveyTargetY = 59.8/2.0; 
 // 4 points
  coords.push_back(Amg::Vector3D(-m_SurveyTargetX,-m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D(-m_SurveyTargetX, m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D( m_SurveyTargetX,-m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D( m_SurveyTargetX, m_SurveyTargetY,0.0));
} 

//stupid implementation of SCT EC survey coordinates
void SurveyConstraint::getSurveyCoordsSCTEC(//const Identifier& ModuleID,
					    std::vector< Amg::Vector3D > & coords) {
  coords.clear();
  const double m_SurveyTargetX = 63.6/2.0;
  const double m_SurveyTargetY = 128.2/2.0; 
 // 4 points
  coords.push_back(Amg::Vector3D(-m_SurveyTargetX,-m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D(-m_SurveyTargetX, m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D( m_SurveyTargetX,-m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D( m_SurveyTargetX, m_SurveyTargetY,0.0));
}

//stupid implementation of SCT barrel survey coordinates
void SurveyConstraint::getSurveyCoordsSCTB(//const Identifier& ModuleID,
					   std::vector< Amg::Vector3D > & coords) {
  coords.clear();
  const double m_SurveyTargetX = 63.6/2.0;
  const double m_SurveyTargetY = 128.2/2.0; 
 // 4 points
  coords.push_back(Amg::Vector3D(-m_SurveyTargetX,-m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D(-m_SurveyTargetX, m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D( m_SurveyTargetX,-m_SurveyTargetY,0.0));
  coords.push_back(Amg::Vector3D( m_SurveyTargetX, m_SurveyTargetY,0.0));
}


void SurveyConstraint::GlobalToLocal(SurveyConstraintModule* mut,
				     std::vector<SurveyConstraintPoint>& points) {
  // get global to wut's local transformation 
  Amg::Transform3D globaltolocal = mut->get_globaltolocal();
  // Transform the points into the wut's coordinate system
  for(unsigned ipoint=0;ipoint<points.size();ipoint++){
    Amg::Vector3D& survey = points[ipoint].survey();
    Amg::Vector3D& current = points[ipoint].current();
    survey =globaltolocal *survey;
    current=globaltolocal * current;
  }
}


void SurveyConstraint::TransformSector(Identifier Pixel_ModuleID,
				       SurveyConstraintModule* mut,
				       Amg::Vector3D& current,
				       Amg::Transform3D SurveyTransRandSect) {
  // get rotation angle phi (in global coordinates) to go from module to sector 
  // coordinate frame
  double phi = PhiModuleToSector(m_pixid->phi_module(Pixel_ModuleID));
  // transform current point from module to sector coordinate frame
  current = Amg::AngleAxis3D(phi, Amg::Vector3D(0.,0.,1.)) * current;
  // get global to module's (which is now sector's) local transformation 
  Amg::Transform3D globaltolocal = mut->get_globaltolocal();
  // Transform the points into the sector's local coordinate system
  current =  globaltolocal * current;
  // do the actual sector transformation
  current =  SurveyTransRandSect * current ;
  // go back to global
  Amg::Transform3D localtoglobal = globaltolocal.inverse();
  current =  localtoglobal * current ;
  // transform current point back from sector to module coordinate frame
  current = Amg::AngleAxis3D(-phi, Amg::Vector3D(0.,0.,1.)) * current ;  
}


int SurveyConstraint::SectorNumber(int phi_module) {
  if(phi_module >=  0 && phi_module <=  5) return 0;
  if(phi_module >=  6 && phi_module <= 11) return 1;
  if(phi_module >= 12 && phi_module <= 17) return 2;
  if(phi_module >= 18 && phi_module <= 23) return 3;
  if(phi_module >= 24 && phi_module <= 29) return 4;
  if(phi_module >= 30 && phi_module <= 35) return 5;
  if(phi_module >= 36 && phi_module <= 41) return 6;
  if(phi_module >= 42 && phi_module <= 47) return 7;
  return -1;
}


double SurveyConstraint::PhiModuleToSector(int phi_module) {
  if(phi_module%6 == 0) return ( 7.5 - 26.25) * (3.14159265/180.);
  if(phi_module%6 == 1) return (15   - 26.25) * (3.14159265/180.);
  if(phi_module%6 == 2) return (22.5 - 26.25) * (3.14159265/180.);
  if(phi_module%6 == 3) return (30   - 26.25) * (3.14159265/180.);
  if(phi_module%6 == 4) return (37.5 - 26.25) * (3.14159265/180.);
  if(phi_module%6 == 5) return (45   - 26.25) * (3.14159265/180.);
  return -1;
}

  //__________________________________________________________________________
////} // end of namespace bracket

