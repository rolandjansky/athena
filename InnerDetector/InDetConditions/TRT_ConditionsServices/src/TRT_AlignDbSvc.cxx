/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file TRT_AlignDbSvc.cxx
 *  @brief Service to manage TRT alignment conditions
 *  @author AUTHOR John Alison <johnda@hep.upenn.edu>, Peter Hansen <phansen@nbi.dk>, Wouter Hulsbergen
 **/

#include "TRT_AlignDbSvc.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "RegistrationServices/IIOVRegistrationSvc.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "InDetIdentifier/TRT_ID.h"

#include "InDetReadoutGeometry/TRT_DetElementCollection.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetReadoutGeometry/InDetDetectorManager.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "TRT_ConditionsData/FloatArrayStore.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "AthenaKernel/IOVSvcDefs.h"

// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/CLHEPtoEigenEulerAnglesConverters.h"

TRT_AlignDbSvc::TRT_AlignDbSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator),
    m_detStore("DetectorStore",name),
    m_trtStrawAlignDbSvc("TRT_StrawAlignDbSvc",name),
    m_trtid(0),
    m_trtman(0),
    m_alignroot("/TRT/Align"),
    m_alignString("AL"),
    m_par_alitextfile(""),
    m_streamer("AthenaPoolOutputStreamTool/CondStream1"),
    m_alignDBprefix("/TRT/Align/"),
    m_dynamicDB(false),
    m_forceUserDBConfig(false)
{
  declareProperty("StreamTool",m_streamer);
  declareProperty("alignroot",m_alignroot);
  declareProperty("alignTextFile",m_par_alitextfile);
  declareProperty("DetectorStore",m_detStore);
  declareProperty("TrtStrawAlignDbSvc",   m_trtStrawAlignDbSvc,"Service for interaction with the TRT straw alignment DB");
  declareProperty("alignString",m_alignString);
  declareProperty("alignDBprefix",m_alignDBprefix);
  declareProperty("forceUserDBConfig",m_forceUserDBConfig, "Set to true to override any DB auto-configuration");
}

//-------------------------------------------------------------------------

TRT_AlignDbSvc::~TRT_AlignDbSvc()
{}

//
//-----------------------------------------------------------------------------
StatusCode TRT_AlignDbSvc::initialize() 
{
  msg(MSG::INFO) << " in initialize " << endmsg;
  
  // Get the DetectorStore
  if (StatusCode::SUCCESS!=m_detStore.retrieve()) {
    ATH_MSG_FATAL( "Couldn't retrieve " << m_detStore.name() );
    return StatusCode::FAILURE;
  }

  // get the straw db service 
  if( m_trtStrawAlignDbSvc.retrieve().isFailure() ) {
    msg(MSG::FATAL) << " Could not retrieve " << m_trtStrawAlignDbSvc << endmsg;
    return StatusCode::FAILURE;
  }
  else
    ATH_MSG_INFO("retrieved " << m_trtStrawAlignDbSvc);

  // Get the geometry.
  InDetDD::TRT_DetElementCollection::const_iterator iter,itermin,itermax;
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtman,"TRT") || 
    m_trtman==0) {
    msg(MSG::FATAL) << "Could not find TRT manager " << endmsg;
    return StatusCode::FAILURE;
  } else {
    
    // Get TRT manager and ID helper
    itermin=m_trtman->getDetectorElementBegin();
    itermax=m_trtman->getDetectorElementEnd();
    if (StatusCode::SUCCESS!=m_detStore->retrieve(m_trtid,"TRT_ID")) {
      ATH_MSG_FATAL("Could not find TRT ID Helper.");
      return StatusCode::FAILURE;
    }
    else msg(MSG::INFO) << "found all TRT services " << endmsg;
  }

  if (m_trtman->m_alignfoldertype == InDetDD::timedependent_run2 && !m_forceUserDBConfig){
    m_dynamicDB = true;
    m_alignroot = "/TRT/AlignL2";                                                         
    m_alignDBprefix = "/TRT/AlignL2/";
  }
  if (m_trtman->m_alignfoldertype == InDetDD::static_run1 && !m_forceUserDBConfig){
    m_dynamicDB = false;
    m_alignroot = "/TRT/Align";
    m_alignDBprefix = "/TRT/Align/";
  }


  // setup list of TDS objects from geometry description
  m_alignobjs.clear();
  m_alignchans.clear();

  // Include a level 1 alignment
  int ichan(0) ;
  if (!m_dynamicDB){
    m_alignobjs.push_back("/TRT/Align/TRT");
    msg(MSG::INFO) << "Adding key: /TRT/Align/TRT --> We are using static DB folder scheme" << endmsg;
    m_alignchans.push_back(ichan++);
  }
  
  //Check all detector elements in the present geometry setup
  for (iter=itermin;iter!=itermax;++iter) {
    const InDetDD::TRT_BaseElement* element=*iter;
    if (element!=0) {
      const Identifier ident=element->identify();
      if (m_trtid->is_trt(ident)) {           //OK this Element is included
	std::string key = findkey(ident,m_alignString); 
	std::vector<std::string>::const_iterator ix = find(m_alignobjs.begin(),m_alignobjs.end(),key);
	if (ix==m_alignobjs.end()) {
	  m_alignobjs.push_back(key);
	  msg(MSG::INFO) << "Adding key: " << key << endmsg;
	  m_alignchans.push_back(ichan++);
	}
      }
    }
  }
  
  /** Keeping Back compatibility with old db scheme
      This is needed when reading in an textFile with the old endcap Schema
      and the alignment folder in the db has been blocked.
  */
  if(m_alignString != "ALold"){
    
    for(unsigned int i=0; i<14; ++i){
      std::string testA = "A"+intToString(i);
      //m_alignobjs.push_back("/TRT/Align/"+testA);
      m_alignobjs.push_back(m_alignDBprefix+testA);
      m_alignchans.push_back(ichan++);
      
      std::string testC = "C"+intToString(i);
      //     m_alignobjs.push_back("/TRT/Align/"+testC);
      m_alignobjs.push_back(m_alignDBprefix+testC);
      m_alignchans.push_back(ichan++);
    
    }
  }
  
  /** If the folder exists, register a callback, we read from the text file if it exists in the call back
      If the folder does not yet exist, create and record it.
  */

  /** This is the folder given by the conditions db.(Eg: from the global tag)
      One can block this via: conddb.blockFolder("/TRT/Align") in the jobOptions
   */
  bool alignFolderExists = m_detStore->contains<AlignableTransformContainer>(m_alignroot) ;
  if (m_dynamicDB) alignFolderExists = ( alignFolderExists && m_detStore->contains<CondAttrListCollection>("/TRT/AlignL1/TRT"));

  /** This is the text files whence the constants.
  */
  bool alignTextFileExists = !m_par_alitextfile.empty();
  
  if( alignFolderExists ) {    
    
    /** register the callback */
    if( (m_detStore->regFcn(&TRT_AlignDbSvc::IOVCallBack,this,m_aligncontainerhandle,m_alignroot)).isFailure() ) {
      msg(MSG::FATAL) << "cannot register callback for folder " << m_alignroot << endmsg ;
      return StatusCode::FAILURE ;
    }
    
    /** Reminder that the constants will be read from text file. */
    if( alignTextFileExists ) 
      msg(MSG::INFO) << "AlignableTransformContainer with name " << m_alignroot
		     << " exists. Will read text file from callback function." << endmsg;
  
  } else if ( alignTextFileExists ) {
    
    /** create alignment objects */
    if(StatusCode::SUCCESS!=this->createAlignObjects()) {
      msg(MSG::FATAL) << "Could not create and record new AlignableTransformContainer. Existing." << endmsg ;
      return StatusCode::FAILURE ;
    }
    
    /** Read the newly created objects */
    if(StatusCode::SUCCESS!=this->readAlignTextFile(m_par_alitextfile)) {
      msg(MSG::FATAL) << "Could not read alignment objects. Existing." << endmsg;
      return StatusCode::FAILURE ;
    }
  
  } else {
    msg(MSG::ERROR) << "AlignableTransformContainer not in IOV service and no textfile specified." << endmsg ;
  }
  
  if(msgLvl(MSG::DEBUG)) msg() << "The TRT/Align keys and channels are:";
  for (unsigned int i=0;i<m_alignobjs.size();++i)
    if(msgLvl(MSG::DEBUG)) msg() << " key " << m_alignobjs[i] << " chan " << m_alignchans[i] << endmsg;
  
  return StatusCode::SUCCESS;
}
//
//-----------------------------------------------------------------------------

StatusCode TRT_AlignDbSvc::finalize()
{
  msg(MSG::INFO) << "TRT_AlignDbSvc finalize method called" << endmsg;
  for(std::vector<Amg::Transform3D*>::iterator it = m_amgTransformCache.begin(); it != m_amgTransformCache.end(); it++){
      delete *it;
  }
  return StatusCode::SUCCESS;
}

/** Call back function for alignment folders */
StatusCode TRT_AlignDbSvc::IOVCallBack(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  msg(MSG::DEBUG) << "In IOVCallBack" << endmsg;
  
  /** By the time this gets called the alignment transforms have been created. 
      Either by the create method in initialize, or from the align folder content from the cond db. 
      (Its not clear to johnda where the alignable tracnsforms get created from the conddb)

      If we read the constants from the db, there is a chance that they only contained the alignable transforms 
      for the old endcap keys. In which case we need to create the transforms for the new endcap keys
  */
  if(StatusCode::SUCCESS!=createAlignObjectsWhichDoNotAlreadyExist()){
    msg(MSG::FATAL) << "Failed to create the alignable objects which did not already exist " << endmsg;
    return StatusCode::FAILURE ;
  }
  
  /** Print the keys were setting 
   */
  for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr)
    msg(MSG::INFO) << "IOVCALLBACK for key " << *itr<< " number " << I << endmsg;
  
  
  if(!m_par_alitextfile.empty()){
    if(StatusCode::SUCCESS!=this->readAlignTextFile(m_par_alitextfile)) {
      msg(MSG::FATAL) << "Could not read alignment objects " << endmsg;
      return StatusCode::FAILURE ;
    }
  }else{
    msg(MSG::INFO) << "Text file " << m_par_alitextfile << " is empty" << endmsg;
  }

  /** print out the objects we have 
   */
  printCondObjects() ;

  msg(MSG::DEBUG) << "Leaving IOVCallBack" << endmsg;
  return StatusCode::SUCCESS;
}

/** write AlignableTransforms to flat text file */
StatusCode TRT_AlignDbSvc::writeAlignTextFile(std::string file) const
{
  msg(MSG::DEBUG) << " in writeAlignTextFile " << endmsg;
  std::ofstream* outfile=0;
  msg(MSG::INFO) << " Write AlignableTransforms to text file: "<< file << endmsg;
  outfile=new std::ofstream(file.c_str());

  int ntrans=0;
  int nobj=0;

  /** Loop over created AlignableTransforms */
  std::vector<std::string>::const_iterator iobj=m_alignobjs.begin();
  std::vector<std::string>::const_iterator iobjE=m_alignobjs.end();
  for (;iobj!=iobjE; ++iobj) {
    const AlignableTransform* pat=cgetTransPtr(*iobj);
    std::string key=*iobj;
    
    /** Only write out new keys unless explicitly ask for old keys */
    if(isOldKey(key) && m_alignString != "ALold")
      continue;

    /**  The object must exist in detector store */
    if (!pat){
      msg(MSG::ERROR) << "Cannot find AlignableTransform for key "<< key << endmsg;
      msg(MSG::ERROR) << "Skipping it. " << endmsg;
      *outfile << key << std::endl;
      continue;
    }
    
    // first record is the name
    msg(MSG::INFO) << " Write folder: " << key << endmsg;
    *outfile << key << std::endl;
    ++nobj;
    
    // loop over all transforms in the AlignableTransform object
    AlignableTransform::AlignTransMem_citr cit= pat->begin();
    AlignableTransform::AlignTransMem_citr citE= pat->end();
    for (;cit!=citE;++cit) {
      
      int bec(0);
      int layer(0);
      int sector(0);
      int strawLayer(0);
      
      /** Keep track of total number of transforms */
      ++ntrans;
      
      /** get the relavent identifing info */
      const Identifier& ident=cit->identify();
      bec=m_trtid->barrel_ec(ident);
      layer=m_trtid->layer_or_wheel(ident);
      sector=m_trtid->phi_module(ident);
      strawLayer=m_trtid->straw_layer(ident);
      
      /** Get the translation and rotation */ 
      const Amg::Transform3D& trans = Amg::CLHEPTransformToEigen(cit->transform());

      if(msgLvl(MSG::DEBUG)){
	std::string thisMess = key+" for "+file;
	printTransform(thisMess,trans);
      }


      HepGeom::Transform3D transCLHEP  =  Amg::EigenTransformToCLHEP(trans);
      CLHEP::HepRotation rot=transCLHEP.getRotation(); 
      Amg::Vector3D eulerangles;
      eulerangles[0] = rot.getPhi();
      eulerangles[1] = rot.getTheta();
      eulerangles[2] = rot.getPsi();

      Amg::Vector3D shift=trans.translation();
      //Matthias: was initially 2,1,2 --> this should be equivalent to complicated method above
      //Amg::Vector3D eulerangles = trans.rotation().eulerAngles(2,0,2) ; 

      if(key == (m_alignDBprefix+"L2A") || key == (m_alignDBprefix+"L2C") ){

	msg(MSG::INFO) << "   Write member: "
		       << ident << " " << bec << " " << layer << " " << strawLayer << " "  << key << endmsg; 
	
	*outfile << bec << " " << layer << " " << strawLayer << " "
		 << std::setprecision(10) << " "
		 << shift.x() << " " << shift.y() << " " << shift.z() << " "
		 << eulerangles[0] << " " << eulerangles[1] << " " << eulerangles[2]
		 << std::endl;

	msg(MSG::DEBUG) << "Writing transform to log file " << endmsg;
	msg(MSG::DEBUG) << bec << " " << layer << " " << strawLayer << " "
			<< std::setprecision(10) << " "
			<< shift.x() << " " << shift.y() << " " << shift.z() << " "
		        << eulerangles[0] << " " << eulerangles[1] << " " << eulerangles[2]
			<< endmsg;

      }else{
	//Need to validate this for writing out L1 Endcap alignments 
	msg(MSG::INFO) << "   Write member: "
		       << ident << " " << bec << " " << layer << " " << sector << " "  << key << endmsg; 
	
	*outfile << bec << " " << layer << " " << sector << " "
		 << std::setprecision(10) << " "
		 << shift.x() << " " << shift.y() << " " << shift.z() << " "
		 << eulerangles[0] << " " << eulerangles[1] << " " << eulerangles[2]
		 << std::endl;

	msg(MSG::DEBUG) << "Writing transform to log file " << endmsg;
	msg(MSG::DEBUG) << bec << " " << layer << " " << sector << " "
			<< std::setprecision(10) << " "
			<< shift.x() << " " << shift.y() << " " << shift.z() << " "
		        << eulerangles[0] << " " << eulerangles[1] << " " << eulerangles[2]
			<< endmsg;
      }
      
    }
  }
  
  outfile->close();
  delete outfile;
  
  msg(MSG::INFO) << "Written " << nobj << " AlignableTransform objects" << " with " << ntrans 
		 << " transforms to text file" << endmsg;
  
  msg(MSG::DEBUG) << " leaving writeAlignTextFile " << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode TRT_AlignDbSvc::writeStrawAlignTextFile(std::string file) const
{
  msg(MSG::DEBUG) << " in writeAlignTextFile " << endmsg;
  
  msg(MSG::INFO) << "Writing the straw  alignments " << endmsg;
  if( m_trtStrawAlignDbSvc -> writeTextFile(file) != StatusCode::SUCCESS )
    msg(MSG::ERROR)<<"Cannot write to file "<< file << endmsg;   
  
  return StatusCode::SUCCESS;
}

/** read AlignableTransforms from text file into TDS */
StatusCode TRT_AlignDbSvc::readAlignTextFile(std::string file) {
  msg(MSG::DEBUG) << " In  readAlignTextFile " << endmsg;
  msg(MSG::INFO) << "Read alignment constants from text file: " << file << endmsg;
  std::ifstream infile;
  infile.open(file.c_str());

  /** first clean the alignment container: loop over all
   AlignableTransforms and empty them.
  */
  const AlignableTransformContainer* container= m_aligncontainerhandle ;
  if(container) {
    AlignableTransformContainer* nccontainer = const_cast<AlignableTransformContainer*>(container) ;

    DataVector<AlignableTransform>::iterator it=nccontainer->begin();
    DataVector<AlignableTransform>::iterator itE=nccontainer->end();
    for(;it!=itE;++it)
      // there is no 'clear' function in alignable transform, so we do it with this hack
      **it = AlignableTransform( (*it)->tag() ) ;
  } else {
    return StatusCode::FAILURE ;
  }
  
  // loop over file
  int nobj=0;
  int ntrans=0;
  std::string atname;
  AlignableTransform* pat(0) ;
  char line[512] ;
  
  while( infile.getline(line,512) ) {
    if(line[0] != '#') {
      
      std::string linestring(line) ;
      if( linestring.find('/') != std::string::npos) {
	// this must be a line with a container name
	atname = linestring ;
	msg(MSG::INFO) << "now reading container: " << atname << endmsg ;
	pat=getTransPtr(atname);
	if (!pat) {
	  msg(MSG::ERROR) << "Cannot find AlignableTransform object for key " << atname << endmsg;
	  return StatusCode::FAILURE;
	} else {
	  nobj++;
	} 
      } else if(pat!=0) {
	// this must be a line with constants
	std::istringstream is(line) ;
	
	int bec(0),layer(0),sector(0),strawLayer(0);
	double dx,dy,dz,phi,theta,psi;
	
	if(atname == (m_alignDBprefix+"L2A") || atname == (m_alignDBprefix+"L2C") ){
	  if( is >> bec >> layer >> strawLayer >> dx >> dy >> dz >> phi ) {
         CLHEP::Hep3Vector translation(dx, dy, dz);
         CLHEP::HepRotation rotation;
         if (is >> theta >> psi) {
            if (msgLvl(MSG::DEBUG))
                msg() << "read a line with euler angles!" << phi
                      << " " << theta << " " << psi << endmsg;
            rotation.set(phi, theta, psi);
         }
	    //We have to give a phi sector, so we'll give 0.
	    Identifier ident=m_trtid->layer_id(bec,0,layer,strawLayer);
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "The identifier is for bec " << bec 
						   << " layer " << layer << " strawLayer " << strawLayer <<endmsg;
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "The code is  " << m_trtid->show_to_string(ident) << endmsg;
	    if( pat->findIdent(ident)!=pat->end() ) {
	      msg(MSG::WARNING) << "WARNING: read module from file which was already in AlignableTransform. Will skip it."
				<< " bec,lay,sec,strawlay = " << bec << " " << layer << " " << sector << " " << std::endl;
	    } else {
	      pat->add(ident,HepGeom::Transform3D(rotation, translation));
	      HepGeom::Transform3D testtrans = HepGeom::Transform3D(rotation, translation);
	      if(msgLvl(MSG::DEBUG)){
		std::string thisMess = atname+" for "+file;
		printTransform(thisMess,Amg::CLHEPTransformToEigen(testtrans));
	      }


	      ++ntrans;
	      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Set transform: "
						     << " [" << bec << "," << layer << ","  << strawLayer 
						     << "] key " << atname 
						     << " rotation=(" << phi << "," << theta << "," << psi
						     << "),  translation=(" << dx << "," << dy << "," << dz << "])" 
						     << endmsg;
	    }
	  } else if(!linestring.empty()) {
	    msg(MSG::WARNING) << "Read invalid line from textfile. Line=\'" << line << "\'" << endmsg ;
	  }
	}//if endcap
	else{
	  if( is >> bec >> layer >> sector >> dx >> dy >> dz >> phi ) {
	    CLHEP::Hep3Vector translation(dx,dy,dz)  ;
	    CLHEP::HepRotation rotation;
	    if( is >> theta >> psi ) {
	      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "read a line with euler angles!" << phi << " " << theta << " " << psi 
					   << endmsg ;
	        rotation.set(phi,theta,psi) ;

	    } else {
	      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "read a line with angle in phi!" << phi << endmsg ;
	      rotation = CLHEP::HepRotationZ(phi) ;
	    }
	    
	    Identifier ident=m_trtid->module_id(bec,sector,layer);
	    if( pat->findIdent(ident)!=pat->end() ) {
	      msg(MSG::WARNING) << "WARNING: read module from file which was already in AlignableTransform. Will skip it."
				<< " bec,lay,sec = " << bec << " " << layer << " " << sector << std::endl ;
	    } else {
	      pat->add(ident,HepGeom::Transform3D(rotation, translation));
	      ++ntrans ;
	      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Set transform: "
						     << " [" << bec << "," << layer << "," << sector 
						     << "] key " << atname << " rotation=(" << phi << "," << theta << "," << psi
						     << "),  translation=(" << dx << "," << dy << "," << dz << "])" << endmsg;
	    }
	  } else if(!linestring.empty()) {
	    msg(MSG::WARNING) << "Read invalid line from textfile. Line=\'" << line << "\'" << endmsg ;
	  }
	}//if not the endcap

	
      }//if pat!=0
    }//if line != "#"
  }//while 

  infile.close() ;
  msg(MSG::INFO) << "Read " << nobj << " objects from file with " << ntrans << " transforms." 
		 << " Now forcing callback in detector manager." << endmsg; 

  /** force a call back */
  int i(0);
  std::list<std::string> keys;
  if((const_cast<InDetDD::TRT_DetectorManager*>(m_trtman))->align(i,keys).isFailure()){
    msg(MSG::ERROR) <<"Failed to force the alignment callback!"<<endmsg;
  }
  
  msg(MSG::DEBUG) << " Leaving  readAlignTextFile " << endmsg;
  return StatusCode::SUCCESS;
}  

/** write the alignment objects to output */
StatusCode TRT_AlignDbSvc::streamOutAlignObjects() const{
  msg(MSG::DEBUG) << "In streamOutAlignObjects "  << endmsg;
  StatusCode sc;

  // Get Output Stream tool for writing
  sc = m_streamer.retrieve();
  
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to find AthenaOutputStreamTool" << endmsg;
    return StatusCode::FAILURE;
  }
  
  IAthenaOutputStreamTool*  streamer=const_cast<IAthenaOutputStreamTool*>(&(*m_streamer));
  
  sc = streamer->connectOutput();
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not connect stream to output" <<endmsg;
    return( StatusCode::FAILURE);
  }
  
  IAthenaOutputStreamTool::TypeKeyPairs  typeKeys;
  IAthenaOutputStreamTool::TypeKeyPair arraypair("AlignableTransformContainer",m_alignroot);
  typeKeys.push_back(arraypair);
  if(!(m_detStore->contains<AlignableTransformContainer>(m_alignroot))) {
    msg(MSG::ERROR) << " Container " << m_alignroot << " not found "
		    << endmsg;
    return( StatusCode::FAILURE);
  }
  
  sc =streamer->streamObjects(typeKeys);
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not stream out AlignableTransformContainer" <<endmsg;
    return( StatusCode::FAILURE);
  }
  
  sc = streamer->commitOutput();
  if (sc.isFailure()) {
    msg(MSG::ERROR) <<"Could not commit output stream" <<endmsg;
    return( StatusCode::FAILURE);
  }
  
  msg(MSG::INFO) << "   Streamed out and committed AlignableTransformContainer" << endmsg;
  printCondObjects() ;

  msg(MSG::DEBUG) << "Leaving streamOutAlignObjects "  << endmsg;
  return StatusCode::SUCCESS;
}

/** register alignment objects with the IoV service */
StatusCode TRT_AlignDbSvc::registerAlignObjects(std::string tag, int run1, int event1, int run2, int event2) const{
  
  msg(MSG::INFO) << "registerAlignObjects with IOV " << endmsg;
  msg(MSG::INFO) << "Run/evt1 [" << run1 << "," << event1 << "]" << endmsg;
  msg(MSG::INFO) << "Run/evt2 [" << run2 << "," << event2 << "]" << endmsg;

  // get pointer to registration svc
  IIOVRegistrationSvc* regsvc;
  if (StatusCode::SUCCESS!=service("IOVRegistrationSvc",regsvc)) {
    msg(MSG::FATAL) << "IOVRegistrationSvc not found" << endmsg;
    return( StatusCode::FAILURE);
  }
  
  if (StatusCode::SUCCESS==regsvc->registerIOV("AlignableTransformContainer",m_alignroot,tag,run1,run2,event1,event2)){
    msg(MSG::INFO) << " Register AlignableTransformContainer object " 
		   << m_alignroot << endmsg;
  } else {
    msg(MSG::ERROR) << " Failed to register AlignableTranformContainer " 
		    << m_alignroot << endmsg;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

/** get Level L2 Transform for an identifier */
const Amg::Transform3D TRT_AlignDbSvc::getAlignmentTransform(const Identifier& ident, unsigned int level) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In getAlignmentTransform" << endmsg;
  
  if(level != 1 && level != 2){
    if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Call to trans wrong!" << endmsg;
    if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Level must be 1 or 2!" << endmsg;
    if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Returning NULL" << endmsg;
    return Amg::Transform3D();
  }

  return (level == 1) ? getAlignmentTransformL1(ident) : getAlignmentTransformL2(ident);
}

/** get Level 1 AlignableTransform for an identifier */
const Amg::Transform3D  TRT_AlignDbSvc::getAlignmentTransformL1(Identifier const& ident) const{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In getAlignmentTransformL1" << endmsg;
  // return level 1 AlignableTransform for the TRT subdetector bec (+-1,+-2)
  // or return null transform.
  
  std::string key="/TRT/Align/TRT";
  const AlignableTransform* pat=cgetTransPtr(key);
  if (!pat) return Amg::Transform3D();

  // get Amg::Transform3d via AlignTransMember
  AlignableTransform::AlignTransMem_citr itr = pat->findIdent(ident);
  return ( itr!=pat->end() ? Amg::CLHEPTransformToEigen(itr->transform()) : Amg::Transform3D() );
}

/** get Level 2 AlignableTransform for an identifier */
const Amg::Transform3D TRT_AlignDbSvc::getAlignmentTransformL2(Identifier const& ident) const{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In getAlignmentTransformL2" << endmsg;
  // return level 2 AlignableTransform for the module containing ident
  // or return null transform.
  
  std::string key=findkey(ident,m_alignString);
  //does a key exist corresponding to this identifier?
  
  const AlignableTransform* pat=cgetTransPtr(key);
  if (!pat) return Amg::Transform3D();

  // OK it exists
  // get "module identifier" from identifier
  int bec=m_trtid->barrel_ec(ident);
  int layer=m_trtid->layer_or_wheel(ident);
  int sector=m_trtid->phi_module(ident);
  int strawLayer=m_trtid->straw_layer(ident);
  int ring = getRingForStrawLayer(strawLayer);

  Identifier mid;
  if(key == (m_alignDBprefix+"L2A") || key == (m_alignDBprefix+"L2C") )
    mid=m_trtid->layer_id(bec,0,layer,ring);
  else
    mid=m_trtid->module_id(bec,sector,layer);
  
  // get Amg::Transform3d via AlignTransMember
  AlignableTransform::AlignTransMem_citr itr = pat->findIdent(mid);
  return ( itr!=pat->end() ? Amg::CLHEPTransformToEigen(itr->transform()) : Amg::Transform3D() );
}

/** get Level L2 Transform for an identifier */
const Amg::Transform3D* TRT_AlignDbSvc::getAlignmentTransformPtr(const Identifier& ident, unsigned int level) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In getAlignmentTransformPtr" << endmsg;
  
  if(level != 1 && level != 2){
    if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Call to trans wrong!" << endmsg;
    if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Level must be 1 or 2!" << endmsg;
    if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Returning NULL" << endmsg;
    return NULL;
  }

  return (level == 1) ? getAlignmentTransformL1Ptr(ident) : getAlignmentTransformL2Ptr(ident);
}

/** get Level L1 Transform for an identifier */
const Amg::Transform3D*  TRT_AlignDbSvc::getAlignmentTransformL1Ptr(Identifier const& ident) const{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In getAlignmentTransformL1Ptr" << endmsg;
  // get level 1 (despite name, will change soon) AlignableTransform for the subdetector containing ident
  const Amg::Transform3D* rc(0) ;
  if( m_trtid->is_trt(ident) ) {
    const AlignableTransform* pat = cgetTransPtr( "/TRT/Align/TRT" ) ;
    if( pat ) {
      int bec=m_trtid->barrel_ec(ident);
      Identifier mid=m_trtid->module_id(bec,0,0);
      AlignableTransform::AlignTransMem_citr iter = pat->findIdent(mid) ;
      if(iter != pat->end() ) {
          Amg::Transform3D* amgTransform = new Amg::Transform3D((Amg::CLHEPTransformToEigen(iter->transform())));
          m_amgTransformCache.push_back(amgTransform);
          rc = amgTransform;
      }
    }
  }
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving getAlignmentTransformL1Ptr" << endmsg;
  return rc ;
}

/** get Level L2 Transform for an identifier */
const Amg::Transform3D* TRT_AlignDbSvc::getAlignmentTransformL2Ptr(Identifier const& ident) const{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In getAlignmentTransformL2Ptr" << endmsg;
  // set level 2 (despite name, will change soon) AlignableTransform for the module containing ident
  // or add a new one.
  const Amg::Transform3D* rc(0) ;
  //does a folder exist corresponding to this identifier?
  if( m_trtid->is_trt(ident) ) {
    std::string key=findkey(ident,m_alignString);
    const AlignableTransform* pat=cgetTransPtr(key);
    if(pat) {
      // make sure the identifier is a "module identifier"
      int bec=m_trtid->barrel_ec(ident);
      int layer=m_trtid->layer_or_wheel(ident);
      int sector=m_trtid->phi_module(ident);
      int strawLayer=m_trtid->straw_layer(ident);
      int ring = getRingForStrawLayer(strawLayer);
      Identifier mid;
      if(key == (m_alignDBprefix+"L2A") || key == (m_alignDBprefix+"L2C") ){
	mid =m_trtid->layer_id(bec,0,layer,ring);
      }else
	mid =m_trtid->module_id(bec,sector,layer);
      
      AlignableTransform::AlignTransMem_citr iter = pat->findIdent(mid) ;
      if(iter != pat->end() ){
          AlignableTransform::AlignTransMem_citr iter = pat->findIdent(mid) ;
          Amg::Transform3D* amgTransform = new Amg::Transform3D((Amg::CLHEPTransformToEigen(iter->transform())));
          m_amgTransformCache.push_back(amgTransform);
          rc = amgTransform;
      }

      else
	if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Did not find the transform for " << m_trtid->show_to_string(mid) << endmsg;
    }
  }
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving getAlignmentTransformL2Ptr" << endmsg;
  return rc ;
}

/** set AlignableTransform for an identifier */
StatusCode TRT_AlignDbSvc::setAlignTransform(Identifier ident, Amg::Transform3D trans, unsigned int level) {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In getAlignmentTransform" << endmsg;
  
  if(level != 1 && level != 2 && level != 3){
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Call to setAlignTransform wrong!" << endmsg;
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Level must be 1,2 or 3!" << endmsg;
    return StatusCode::FAILURE;
  }

  switch (level) {
    
  case 1:
    if(setAlignTransformL1(ident,trans).isFailure()){
      msg(MSG::FATAL)<<"Failed to set align Transform L1" << endmsg;
      return StatusCode::FAILURE;
    }
    break;
  case 2:
    if(setAlignTransformL2(ident,trans).isFailure()){
      msg(MSG::FATAL)<<"Failed to set align Transform L2" << endmsg;
      return StatusCode::FAILURE;
    }
    break;
  case 3:
    if(setAlignTransformL3(ident,trans).isFailure()){
      msg(MSG::FATAL)<<"Failed to set align Transform L3" << endmsg;
      return StatusCode::FAILURE;
    }
    break;
  }

  return StatusCode::SUCCESS;
}

/** set Level 1 AlignableTransform for an identifier */
StatusCode TRT_AlignDbSvc::setAlignTransformL1(Identifier ident, Amg::Transform3D trans) {
  /** AlignableTransform for the subdetector containing ident
   or add a new one.
  */
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In setAlignTransformL1" << endmsg;
  if(msgLvl(MSG::DEBUG)){
    printTransform("Transform in setAlignTransformL1",trans);
  }

  // New additions for new global folder structure
  // No ATs exist for levels 1 & 2 --> need alternative
  if (m_dynamicDB){
    if(!tweakGlobalFolder(ident, trans)) {
      ATH_MSG_ERROR( "Attempt tweak GlobalDB folder failed" );
      return StatusCode::FAILURE;
    }
  }
  else {
  
    //does a folder exist corresponding to this identifier?
    AlignableTransform* pat=getTransPtr("/TRT/Align/TRT");
    if (!pat){
      msg(MSG::FATAL) << "The AlignableTransform for key " << "/TRT/Align/TRT" << " does not exist " << endmsg;
      msg(MSG::FATAL) << "Failing ... " << endmsg;
      return StatusCode::FAILURE;
    }
    
    // make sure the identifier is a "subdetector identifier"
    if( !(m_trtid->is_trt(ident)) ){
      msg(MSG::FATAL) << "The identifier " << ident << " is not from the TRT " << endmsg;
      msg(MSG::FATAL) << "Failing ... " <<endmsg;
      return StatusCode::FAILURE;
    }
    
    int bec=m_trtid->barrel_ec(ident);
    Identifier mid=m_trtid->module_id(bec,0,0);
    // update or add (mid,trans) pair
    HepGeom::Transform3D clhepTrans = Amg::EigenTransformToCLHEP(trans);
    if( !(pat->update(mid,clhepTrans)) ) {
      pat->add(mid,clhepTrans);
      pat->sortv() ;
    }
  }

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving setAlignTransformL1" << endmsg;
  return StatusCode::SUCCESS;
}

/** set Level 2 AlignableTransform for an identifier */
StatusCode TRT_AlignDbSvc::setAlignTransformL2 (Identifier ident, Amg::Transform3D trans) {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In setAlignTransformL2"  << endmsg;
  /** AlignableTransform for the module containing ident
      or add a new one.
  */
  
  //does a folder exist corresponding to this identifier?
  std::string key=findkey(ident,m_alignString);
  AlignableTransform* pat=getTransPtr(key);
  if (!pat) return StatusCode::FAILURE;
  
  // make sure the identifier is a "module identifier"
  if( !(m_trtid->is_trt(ident)) ) return StatusCode::FAILURE;
  int bec=m_trtid->barrel_ec(ident);
  int layer=m_trtid->layer_or_wheel(ident);
  int sector=m_trtid->phi_module(ident);
  int strawLayer=m_trtid->straw_layer(ident);
  int ring = getRingForStrawLayer(strawLayer);
  Identifier ident2;

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Setting the L2 Alignment for: " 
					 << " Bec= " << bec << " layer= " << layer << " sector= " << sector 
					 << " strawLayer= " << strawLayer << endmsg;
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "The translations are: x= " << trans(0,3) << " y= "<<trans(1,3) << " z= "<<trans(2,3) << endmsg;

  if(key == (m_alignDBprefix+"L2A") || key == (m_alignDBprefix+"L2C"))
    ident2=m_trtid->layer_id(bec,0,layer,ring);    
  else
    ident2=m_trtid->module_id(bec,sector,layer);

  // make sure the identifier is a TRT identifier
  if( !(m_trtid->is_trt(ident2)) ){
    msg(MSG::FATAL) << "The identifier " << ident2 << " is not from the TRT " << endmsg;
    msg(MSG::FATAL) << "Failing ... " <<endmsg;
    return StatusCode::FAILURE;
  }
  
  // update or add (mid,trans) pair
  HepGeom::Transform3D clhepTrans = Amg::EigenTransformToCLHEP(trans);
  if( !(pat->update(ident2,clhepTrans)) ) {
    pat->add(ident2,clhepTrans);
    pat->sortv() ;
  }
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving setAlignTransformL2 "  << endmsg;
  return StatusCode::SUCCESS;
}

/** set Level 3 AlignableTransform for an identifier */
StatusCode TRT_AlignDbSvc::setAlignTransformL3 (Identifier ident, Amg::Transform3D trans) {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In setAlignTransformL3"  << endmsg;
    
  // make sure the identifier is a "module identifier"
  if( !(m_trtid->is_trt(ident)) ) return StatusCode::FAILURE;
  int bec=m_trtid->barrel_ec(ident);
  
  //Only L3 in the barrel is currently supported 
  if(abs(bec) == 1){
    
    // Currently at L3 only displacements in the plane of the straw layer can be stored.
    // For tracks coming from the interaction point this coorisponds to the sensitive coordinate
    ATH_MSG_INFO("Storing L3 Barrel constants, Only dx and rotz will be written to DB");
    float dx = trans.translation().x();
    float rotz = atan2(trans.rotation()(0,1),trans.rotation()(0,0));

    // Need the length of the wires to translate the rotation to a translation.
    const InDetDD::TRT_BaseElement* strawElement =  m_trtman->getElement( ident );
    double strawLenthOver2 = 0.5* strawElement->strawLength();

    // The alignment frames are the same for straws on side A and side C (verrify this!!)
    // but variables stored in the db are not. We'll calculate the dispacement of each end 
    // in the alignment frame and in a second step, convert to the values in the DB. 
    double delta_dx_atLargerZ = dx + strawLenthOver2 * sin(rotz);
    double delta_dx_atSmallerZ = dx - strawLenthOver2 * sin(rotz);
	
    // For the definition of dx1 and dx2 see TRT_ConditionsData/StrawDx.h
    // Now we need to know the real meaning of dx1 and dx2. Alas, we
    // are in the frame of either of the two barrel sides, but not
    // both, so we need to know which one.
    bool sideA = m_trtid->barrel_ec(ident) == 1 ? true : false;
	
    // Straw position closest to the electronics 
    double dx1_new = sideA ? delta_dx_atLargerZ : delta_dx_atSmallerZ;
	
    // Straw position away from the electronics 
    double dx2_new = sideA ? delta_dx_atSmallerZ : delta_dx_atLargerZ;

    // Uncertianty on straw positions (Arbitrary for now)
    double dxErr = 0.001;
	
    m_trtStrawAlignDbSvc->setDx(ident, dx1_new, dx2_new, dxErr);
  }else{
    // Currently at L3 only displacements in the plane of the straw layer can be stored.
    // For tracks coming from the interaction point this coorisponds to the sensitive coordinate
    ATH_MSG_INFO("Storing L3 Endcap constants, Only dy and rotz will be written to DB");
    
    // The expected transformation is in the local frame of the Endcap Straws
    // In the local straw frame:
    //   - the z-axis is along the straw and points toward the beampipe
    //   - the x-axis is along global-z and away from the interaction point 
    //          (locX = globZ A-side / locX = -1 *gobZ C-side) 
    //   - the y-axis is along global phi_hat direction determined by the other 2. 
    //          (clockwise C-side, counter clockwise A-Side)
    float dy = trans.translation().y();
    float rotx = atan2(trans.rotation()(1,2),trans.rotation()(2,2));

    // Need the length of the wires to translate the rotation to a translation.
    const InDetDD::TRT_BaseElement* strawElement =  m_trtman->getElement( ident );
    double strawLenthOver2 = 0.5* strawElement->strawLength();

    // In the global frame, 'dx1' corresponds to the readout side and 'dx2'
    // to the side closest the beampipe.
    double delta_dx_nearBeamPipe = dy + strawLenthOver2 * sin(rotx);
    double delta_dx_nearReadOut = dy - strawLenthOver2 * sin(rotx);
    
    // Uncertianty on straw positions (Arbitrary for now)
    double dxErr = 0.001;
    
    m_trtStrawAlignDbSvc->setDx(ident, delta_dx_nearReadOut, delta_dx_nearBeamPipe, dxErr);
  }
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving setAlignTransformL3 "  << endmsg;
  return StatusCode::SUCCESS;
}

/** tweak AlignableTransform for an identifier */
StatusCode TRT_AlignDbSvc::tweakAlignTransform(Identifier ident, Amg::Transform3D trans, unsigned int level) {
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In tweakAlignTransform" << endmsg;
  
  if(level != 1 && level != 2 && level != 3){
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Incorrect call to tweakTrans" << endmsg;
    if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "level must be 1,2 or 3" << endmsg;
    return StatusCode::FAILURE;
  }
  
  if(level == 1){
    if(tweakAlignTransformL1(ident,trans).isFailure()){
      
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "tweak failed...just setting it." << endmsg;
      
      if(setAlignTransformL1(ident,trans).isFailure()){
	if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Set also failed!" << endmsg;
	if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Fail for real." << endmsg;
	return StatusCode::FAILURE;
      }
    }
  }
  
  else if(level == 2){
    if(tweakAlignTransformL2(ident,trans).isFailure()){
      
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "tweak failed...just setting it." << endmsg;
      
      if(setAlignTransformL2(ident,trans).isFailure()){
	if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Set also failed!" << endmsg;
	if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Fail for real." << endmsg;
	return StatusCode::FAILURE;
      }
    }    
  }
  
  else if(level == 3){
    if(tweakAlignTransformL3(ident,trans).isFailure()){
      
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "tweak failed...just setting it." << endmsg;
      
      if(setAlignTransformL3(ident,trans).isFailure()){
	if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Set also failed!" << endmsg;
	if(msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Fail for real." << endmsg;
	return StatusCode::FAILURE;
      }
    }    
  }
    
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving tweakAlignTransform" << endmsg;
  return StatusCode::SUCCESS;
}

/** tweak Level 1 AlignableTransform for an identifier */
StatusCode TRT_AlignDbSvc::tweakAlignTransformL1(Identifier ident, Amg::Transform3D trans) {
  /** multiply level 1 AlignableTransform for the module containing ident
      by an additional transform.
  */
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In tweakAlignTransformL1" << endmsg;

  // New additions for new global folder structure                                                                                                                    
  // No ATs exist for levels 1 & 2 --> need alternative                                                                                                               
  if (m_dynamicDB){
    if(!tweakGlobalFolder(ident, trans)) {
      ATH_MSG_ERROR( "Attempt tweak GlobalDB folder failed" );
      return StatusCode::FAILURE;
    }
  }
  else {
    
    /** does a folder exist corresponding to this identifier? */
    AlignableTransform* pat=getTransPtr("/TRT/Align/TRT");
    if (!pat){
      msg(MSG::WARNING) << "The AlignableTransform for key " << "/TRT/Align/TRT" << " does not exist " << endmsg;
      msg(MSG::WARNING) << "Failing ... " << endmsg;
      return StatusCode::FAILURE;
    }
    
    /** OK the key exists
	make sure the identifier is a "subdetector identifier"
    */
    if( !(m_trtid->is_trt(ident)) ){
      msg(MSG::WARNING) << "The identifier " << ident << " is not from the TRT " << endmsg;
      msg(MSG::WARNING) << "Failing ... " <<endmsg;
      return StatusCode::FAILURE;
    }
    
    int bec=m_trtid->barrel_ec(ident);
    Identifier mid=m_trtid->module_id(bec,0,0);
    
    /** multiply the additional transformation */
    if( !(pat->tweak(mid,Amg::EigenTransformToCLHEP(trans))) ){
      msg(MSG::WARNING) << "The Alignable transfor tweek failed for " << mid << endmsg;
      msg(MSG::WARNING) << "Failing ... " <<endmsg;
      return StatusCode::FAILURE;
    }
  }

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving tweakAlignTransformL1" << endmsg;
  return StatusCode::SUCCESS;
}

/** tweak Level 2 AlignableTransform for an identifier */
StatusCode TRT_AlignDbSvc::tweakAlignTransformL2(Identifier ident, Amg::Transform3D trans) {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In tweakAlignTransformL2" << endmsg;
  // multiply level 2 AlignableTransform for the module containing ident
  // by an additional transform.

  //does a folder exist corresponding to this identifier?
  std::string key=findkey(ident,m_alignString);
  AlignableTransform* pat=getTransPtr(key);
  if (!pat){
    msg(MSG::WARNING) << "The transfor for key: " << key << "Does not exist" << endmsg;
    msg(MSG::WARNING) << " Failing ... " << endmsg;
    return StatusCode::FAILURE;
  }

  //OK the key exists
  // make sure the identifier is a "module identifier"
  if( !(m_trtid->is_trt(ident)) ) return StatusCode::FAILURE;
  int bec=m_trtid->barrel_ec(ident);
  int layer=m_trtid->layer_or_wheel(ident);
  int sector=m_trtid->phi_module(ident);
  int strawLayer=m_trtid->straw_layer(ident);
  int ring = getRingForStrawLayer(strawLayer);
  Identifier mid;
  if(key == (m_alignDBprefix+"L2A") || key == (m_alignDBprefix+"L2C"))
    mid = m_trtid->layer_id(bec,0,layer,ring);
  else
    mid = m_trtid->module_id(bec,sector,layer);

  // multiply the additional transformation
  if( !(pat->tweak(mid,Amg::EigenTransformToCLHEP(trans))) ){
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Leaving tweakAlignTransformL2.  TWEAK FAILED!!" << endmsg;
    return StatusCode::FAILURE;
  }

  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving tweakAlignTransformL2" << endmsg;
  return StatusCode::SUCCESS;
}

/** tweak Level 3 AlignableTransform for an identifier */
StatusCode TRT_AlignDbSvc::tweakAlignTransformL3 (Identifier ident, Amg::Transform3D trans) {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In tweakAlignTransformL3"  << endmsg;
    
  // make sure the identifier is a "module identifier"
  if( !(m_trtid->is_trt(ident)) ) return StatusCode::FAILURE;
  int bec=m_trtid->barrel_ec(ident);
  
  //Only L3 in the barrel is currently supported 
  if(abs(bec) == 1){
    
    // Currently at L3 only displacements in the plane of the straw layer can be stored.
    // For tracks coming from the interaction point this coorisponds to the sensitive coordinate
    ATH_MSG_INFO("Storing L3 constants, Only dx and rotz will be written to DB");
    float dx = trans.translation().x();

    ATH_MSG_DEBUG("xx: " << trans.rotation()(0,0));    
    ATH_MSG_DEBUG("xy: " << trans.rotation()(0,1));    
    ATH_MSG_DEBUG("xz: " << trans.rotation()(0,2));    
    ATH_MSG_DEBUG("yx: " << trans.rotation()(1,0));    
    ATH_MSG_DEBUG("yy: " << trans.rotation()(1,1));    
    ATH_MSG_DEBUG("yz: " << trans.rotation()(1,2));    
    ATH_MSG_DEBUG("zx: " << trans.rotation()(2,0));    
    ATH_MSG_DEBUG("zy: " << trans.rotation()(2,1));    
    ATH_MSG_DEBUG("zz: " << trans.rotation()(2,2));    
	
    // For the definition of dx1 and dx2 see TRT_ConditionsData/StrawDx.h
    // Now we need to know the real meaning of dx1 and dx2. Alas, we
    // are in the frame of either of the two barrel sides, but not
    // both, so we need to know which one.
    bool sideA = m_trtid->barrel_ec(ident) == 1 ? true : false;

    float rotz = atan2(trans.rotation()(0,2),trans.rotation()(0,0));

    // Need the length of the wires to translate the rotation to a translation.
    const InDetDD::TRT_BaseElement* strawElement =  m_trtman->getElement( ident );
    double strawLenthOver2 = 0.5* strawElement->strawLength();

    // Old way (buggy)
    //double delta_dx_atLargerZ = dx + strawLenthOver2 * sin(rotz);
    //double delta_dx_atSmallerZ = dx - strawLenthOver2 * sin(rotz);

    // New way - The rotation is opposite for side A as compared to side C
    double delta_dx_atLargerZ;
    double delta_dx_atSmallerZ;
    if (sideA){
      delta_dx_atLargerZ = dx - strawLenthOver2 * sin(rotz);
      delta_dx_atSmallerZ = dx + strawLenthOver2 * sin(rotz);
    }
    else{
      delta_dx_atLargerZ = dx + strawLenthOver2 * sin(rotz);
      delta_dx_atSmallerZ = dx - strawLenthOver2 * sin(rotz);
    }

    // Straw position closest to the electronics 
    double delta_dx1 = sideA ? delta_dx_atLargerZ : delta_dx_atSmallerZ;
	
    // Straw position away from the electronics 
    double delta_dx2 = sideA ? delta_dx_atSmallerZ : delta_dx_atLargerZ;

    // Uncertianty on straw positions
    double dxErr = 0.001;
	
    // now we do some gymnastics: because the strawdisplacement is
    // binned, we would rather not just 'add' the current change,
    // but rather 'set' the total change. note that this goes
    // wrong if there were other straw corrections
    // applied. We might have to worry about that at
    // some point. 
    double dx1_current = m_trtStrawAlignDbSvc->getDx1(ident);
    double dx2_current = m_trtStrawAlignDbSvc->getDx2(ident) ;
    ATH_MSG_DEBUG("Current dx1 is: " << dx1_current);
    ATH_MSG_DEBUG("Current dx2 is: " << dx2_current);
	
    double dx1_new = dx1_current + delta_dx1;
    double dx2_new = dx2_current + delta_dx2;

    //ATH_MSG_INFO("New dx1 is: " << dx1_new);
    //ATH_MSG_INFO("New dx2 is: " << dx2_new);

    m_trtStrawAlignDbSvc->setDx(ident, dx1_new, dx2_new, dxErr);
  }else{
    
    // Currently at L3 only displacements in the plane of the straw layer can be stored.
    // For tracks coming from the interaction point this coorisponds to the sensitive coordinate
    ATH_MSG_INFO("Storing L3 Endcap constants, Only dy and rotz will be written to DB");
    
    // The expected transformation is in the local frame of the Endcap Straws
    // In the local straw frame:
    //   - the z-axis is along the straw and points toward the beampipe
    //   - the x-axis is along global-z and away from the interaction point 
    //          (locX = globZ A-side / locX = -1 *gobZ C-side) 
    //   - the y-axis is along global phi_hat direction determined by the other 2. 
    //          (clockwise C-side, counter clockwise A-Side)
    float dy = trans.translation().y();
    float rotx = atan2(trans.rotation()(1,2),trans.rotation()(2,2));

    // Need the length of the wires to translate the rotation to a translation.
    const InDetDD::TRT_BaseElement* strawElement =  m_trtman->getElement( ident );
    double strawLenthOver2 = 0.5* strawElement->strawLength();

    // In the global frame, 'dx1' corresponds to the readout side and 'dx2'
    // to the side closest the beampipe.
    double delta_dx_nearBeamPipe = dy + strawLenthOver2 * sin(rotx);
    double delta_dx_nearReadOut = dy - strawLenthOver2 * sin(rotx);

    // Uncertianty on straw positions
    double dxErr = 0.001;

    // now we do some gymnastics: because the strawdisplacement is
    // binned, we would rather not just 'add' the current change,
    // but rather 'set' the total change. note that this goes
    // wrong if there were other straw corrections
    // applied. We might have to worry about that at
    // some point. 
    double dx1_current = m_trtStrawAlignDbSvc->getDx1(ident);
    double dx2_current = m_trtStrawAlignDbSvc->getDx2(ident) ;
    ATH_MSG_DEBUG("Current dx1 is: " << dx1_current);
    ATH_MSG_DEBUG("Current dx2 is: " << dx2_current);
	
    double dx1_new = dx1_current + delta_dx_nearReadOut;
    double dx2_new = dx2_current + delta_dx_nearBeamPipe;

    m_trtStrawAlignDbSvc->setDx(ident, dx1_new, dx2_new, dxErr);
  }
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving tweakAlignTransformL3 "  << endmsg;
  return StatusCode::SUCCESS;
}

/** return the object key for a given identifier and data type */
std::string TRT_AlignDbSvc::findkey(const Identifier& ident, std::string type) const {
  /** given a TRT identifier and a type ("RT", "T0", "DF" or "AL")
      return the key for the associated ArrayStore or AlignableTransForm in TDS
      
      The keys in the old (Lisbon) scheme coincide with TDS folder names:
      
      /Indet/Align/TRTB0     AlignableTransform for barrel layer 0. Etc
      /Indet/Align/TRT_DF_B0 Wire shift FloatArrayStore for barrel layer 0. Etc
      /Indet/Calib/TRT_T0_B0 T0 FloatArrayStore for barrel layer 0. Etc
      /Indet/Calib/TRT_RT_B0 R(t) FloatArrayStore for barrel layer 0. Etc
      
      In the COOL scheme there is only one folder for AlignableTransforms
      and one for FloatArrayStores (/TRT/Align and /TRT/Calib).
      These contains collections of objects with the following keys:
  
      /TRT/Align/L2A     AlignableTransform for Endcap A.
      /TRT/Align/AX     DEPRECATED! Old AlignableTransform for Endcap A wheel X where X is [0-13].
      /TRT/Align/B0     AlignableTransform for barrel layer 0.
      /TRT/Align/B1     AlignableTransform for barrel layer 1.
      /TRT/Align/B2     AlignableTransform for Endcap C.
      /TRT/Align/L2C     AlignableTransform for barrel layer 0.
      /TRT/Align/CX     DEPRECATED! Old AlignableTransform for Endcap C wheel X where X is [0-13].
      /TRT/Calib/DF_B0 Wire shift FloatArrayStore for barrel layer 0. Etc
      /TRT/Calib/T0_B0 T0 FloatArrayStore for barrel layer 0. Etc
      /TRT/Calib/RT_B0 R(t) FloatArrayStore for barrel layer 0. Etc
  */
  std::ostringstream result;
  if (!m_trtid->is_trt(ident)) return result.str();
  
  if(type=="AL" || type=="ALold") {
    //    result << "/TRT/Align/";
    result << m_alignDBprefix;
  } else if(type=="DF") {
    result << "/TRT/Calib/DF";
  } else return result.str();

  int bec=m_trtid->barrel_ec(ident);

  if(type!="AL" && type!="ALold") result << "_";

  if(type=="AL"){
    if(bec==2) {
      result << "L2A";
    } else if(bec==-2) {
      result << "L2C";
    } else {
      result << "B";
      int layer = m_trtid->layer_or_wheel(ident);
      result << layer;
    }
  }else if(type=="ALold"){
    if(bec==2) {
      result << "A";
    } else if(bec==-2) {
      result << "C";
    } else {
      result << "B";
    }
    int layer = m_trtid->layer_or_wheel(ident);
    result << layer;
  }
  
  /** So the result is eg /Indet/Calib/TRT_T0_A6 in the Lisbon scheme
   and /TRT/Calib/T0_A6 in the COOL scheme
   - for the T0 folder for wheel 6 in endcap A 
  */
  
  return result.str();
}     

/** return the prefix tag for a given calibration folder */
std::string TRT_AlignDbSvc::prefixtag(std::string key) const {
  /** given a TRT calibration folder key, return the prefix tag for the
      associated (Float)ArrayStore or AlignableTransform.
      
      This is used in version tags if big container is NOT used.
      
      Prefixes are
      TrtDfB0_ Wire shift FloatArrayStore for barrel layer 0. Etc
      TrtT0B0_ T0 FloatArrayStore for barrel layer 0. Etc
      TrtRtB0_ R(t) FloatArrayStore for barrel layer 0. Etc
      TrtAlB0_ AlignableTransform for barrel layer 0. Etc
      TrtAlId_ AlignableTransform for TRT.
  */

  std::ostringstream result;
  std::string  detector=key.substr(1,3);

  std::string type,quantity;
  int ind,ind1;
  if(detector=="TRT") {
    type=key.substr(5,5);
    ind=11;
    ind1=11;
    quantity=key.substr(ind,2);
  } else if(detector=="Indet") {
    type=key.substr(7,5);
    ind=17;
    ind1=13;
    quantity="";
    if(type=="Align" && key.substr(ind1,2)!="ID") {
      if(key.substr(ind1+3,1)=="_") quantity="DF";
    }
  } else {
    return "";
  }
  if(type=="Calib") {

    if(quantity=="DF") {
       result << "TrtDf";
    } else if (quantity=="RT") {
       result <<  "TrtRt";
    } else if (quantity=="T0") {
       result <<  "TrtT0";
    } else {
       return "TrtDe_";
    }
    std::string module=key.substr(ind+3,2);
    result << module;
  } else if(type=="Align") {
    std::string  module=key.substr(ind1,2);
    if(module=="ID" || module=="TR") {
      return "TrtAl_";
    } else {
      result << "TrtAl";
    }
    module=key.substr(ind+3,2);
    result << module;
  } else {
    return "";
  }
  result << "_";
  return result.str();
}

/** create an empty set of AlignableTransforms for the GeoModel setup */
StatusCode TRT_AlignDbSvc::createAlignObjects() const{

  /** Create empty alignment objects.
      Use when constants are read in from textfiles.
      Make sure that the objects are not read in by IOV before
  */
  msg(MSG::DEBUG) << "createAlignObjects method called" << endmsg;

  /** check if collection already exists */
  if (m_detStore->contains<AlignableTransformContainer>(m_alignroot)) {
    msg(MSG::WARNING) << "createAlignObjects: AlignableTransformContainer with name " << m_alignroot
		      << " already exists." << endmsg;
    return StatusCode::FAILURE;
  }
  
  /** Create the appropiate (empty) AlignableTransforms for this geometry */
  AlignableTransformContainer* patc = new AlignableTransformContainer();

  /**  record it */
  if (StatusCode::SUCCESS!=m_detStore->record(patc,m_alignroot)) {
    msg(MSG::ERROR) << "Could not record AlignableTransformContainer " << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Created and recorded in new AlignableTransformContainer" << endmsg;
  }

  /** set the data handle (can probably be done more efficiently). it
      is also not clear who is the owner now ...
  */
  if (StatusCode::SUCCESS!=m_detStore->retrieve(m_aligncontainerhandle,m_alignroot)) {
    msg(MSG::ERROR) << "Could not retrieve data handle for AlignableTransformContainer " << endmsg;
    return StatusCode::FAILURE;
  }
  
  for (unsigned int i=0;i<m_alignobjs.size();++i) {
    AlignableTransform* pat=new AlignableTransform(m_alignobjs[i]);
    patc->push_back(pat);
    patc->add(m_alignchans[i]);
    msg(MSG::INFO) << " added empty AlignableTransform for key " << m_alignobjs[i] << endmsg;
  }

  msg(MSG::DEBUG) << "Leaving createAlignObjects" << endmsg;
  return StatusCode::SUCCESS;
}

/** create an empty set of AlignableTransforms for the transforms 
      which are not created by XXXXXXX from the conddb  */
StatusCode TRT_AlignDbSvc::createAlignObjectsWhichDoNotAlreadyExist(){

  /** Create empty alignment objects for the object which do do already exist
   */
  msg(MSG::DEBUG) << "createAlignObjectsWhichDoNotAlreadyExist method called" << endmsg;

  /** check if collection already exists */
  if (!m_detStore->contains<AlignableTransformContainer>(m_alignroot)) {
    msg(MSG::FATAL) << "createAlignObjects: AlignableTransformContainer with name " << m_alignroot
		    << " does not exists." << endmsg;
    return StatusCode::FAILURE;
  }
  
  /** Get the alignabletransformcontainer */
  const AlignableTransformContainer* patc= m_aligncontainerhandle;
  if (StatusCode::SUCCESS!=m_detStore->retrieve(patc,m_alignroot)) {
    msg(MSG::ERROR) << "Could not retrieve data handle for AlignableTransformContainer " << endmsg;
    return StatusCode::FAILURE;
  }

  /** Need to cost cast b/c were are chaning it, Ugly but so be it.*/
  AlignableTransformContainer* patc_NonConst = const_cast<AlignableTransformContainer*>(patc);
  
  for (unsigned int i=0;i<m_alignobjs.size();++i) {
    
    /** Only create the new keys that dont exist*/
    if(m_alignString != "Alold" && !isOldKey(m_alignobjs[i] )){
      
      /** If the pointer doesnt exist */
      if(!cgetTransPtr(m_alignobjs[i])){
	
	AlignableTransform* pat=new AlignableTransform(m_alignobjs[i]);
	patc_NonConst->push_back(pat);
	patc_NonConst->add(m_alignchans[i]);
	msg(MSG::INFO) << " added empty AlignableTransform for key " << m_alignobjs[i] << endmsg;

      }
    }
  }

  msg(MSG::DEBUG) << "Leaving createAlignObjectsWhichDoNotAlreadyExist" << endmsg;
  return StatusCode::SUCCESS;
}

/** Output the conditions objects currently in memory */
void TRT_AlignDbSvc::printCondObjects() const {
  msg(MSG::DEBUG) << " In printCondObjects " << endmsg;
  msg(MSG::INFO) << " TRT Conditions DB contains " << m_alignobjs.size() <<" AlignableTransforms: " << endmsg;
  
  const AlignableTransform *pat;
  std::vector<std::string>::const_iterator iobj=m_alignobjs.begin();
  std::vector<std::string>::const_iterator iobjE=m_alignobjs.end();
  for (;iobj!=iobjE;++iobj) {
    msg(MSG::INFO) << " " << *iobj << endmsg;
    pat=cgetTransPtr(*iobj);
    if (pat) {
      pat->print2();
    } else {
      msg(MSG::ERROR) << " Could not find key: " << *iobj << endmsg;
    }
  }

  msg(MSG::DEBUG) << " Leaving printCondObjects" << endmsg;
  return;
}

/** get AlignableTransform pointer for an object key */
AlignableTransform* TRT_AlignDbSvc::getTransPtr(const std::string key) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In (and leaving) getTransPtr" << endmsg;
  return const_cast<AlignableTransform*>(cgetTransPtr(key)) ;
}

/** get const AlignableTransform pointer for an object key */
const AlignableTransform* TRT_AlignDbSvc::cgetTransPtr(const std::string key) const {
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In cgetTransPtr" << endmsg;
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Getting the poiter for key " << key << endmsg;

  // Retrieve AlignableTransform pointer for a given key - const version
  const AlignableTransform* pat=0;
  const AlignableTransformContainer* patc= m_aligncontainerhandle ;
  // the retrieve is unnecessary. in fact, if patc==0, retrieve should
  // fail as well.

  if( patc || StatusCode::SUCCESS==m_detStore->retrieve(patc,m_alignroot)) {
    for(DataVector<AlignableTransform>::const_iterator dva=patc->begin();dva!=patc->end();++dva) {
      if( (*dva)->tag()==key ) {
	pat = *dva;
	break;
      }
    }
  }
  
  if(!pat){
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "AlignableTransform poiter is NULL!!!!!" << endmsg;
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Failed to get the AlignableTransform for key " << key << endmsg;
  }
  
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "leaving cgetTransPtr " << endmsg;
  return pat;
}

/** Returns the ring for a given strawLayer
 */
int TRT_AlignDbSvc::getRingForStrawLayer(int strawlayer) const{
  return strawlayer / 4 * 4;
}


/** Returns the true if the input key is from the old endcap scheme
 */
bool TRT_AlignDbSvc::isOldKey(std::string input) const{
  
  for(unsigned int i=0; i<14; ++i){
    std::string testA = "A"+intToString(i);
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " The testA is " << testA << " " 
					       << bool(input.find(testA)!=std::string::npos) << endmsg;
    if(input.find(testA)!=std::string::npos){
      return true;
    }

    std::string testC = "C"+intToString(i);
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << " The testC is " << testC << " " 
					       << bool(input.find(testC)!=std::string::npos) << endmsg;
    if(input.find(testC)!=std::string::npos){
      return true;
    }
  
  }
  
  return false;
}

/** Convert from an int to a string */
std::string TRT_AlignDbSvc::intToString(int input) const{
  std::ostringstream stm;
  stm << input;
  return stm.str();
}

/** Out put the transfor to the cout, for debugging */
void TRT_AlignDbSvc::printTransform(std::string thisName,  const Amg::Transform3D& transform) const{ 
  std::cout << thisName << " " << transform(0,3) << " " << transform(1,3) << "  " << transform(2,3) << std::endl;
  std::cout << thisName << " " << transform(0,0) << " " << transform(0,1) << "  " << transform(0,2) << std::endl;
  std::cout << thisName << " " << transform(1,0) << " " << transform(1,1) << "  " << transform(1,2) << std::endl;
  std::cout << thisName << " " << transform(2,1) << " " << transform(2,1) << "  " << transform(2,2) << std::endl;
  return;
}

// write global DB to file
StatusCode TRT_AlignDbSvc::writeGlobalFolderFile( const std::string file)
  const {
  std::ofstream* outfile=0;

  if (m_dynamicDB){
    ATH_MSG_DEBUG( "writeFile: Write GlobalFolder DB in text file: " << file );
    outfile=new std::ofstream(file.c_str());
    std::vector<std::string> folder_list = {"/TRT/AlignL1/TRT"};

    for (std::vector<std::string>::iterator it = folder_list.begin(); it != folder_list.end(); ++it){

      *outfile << *it << std::endl;
      const CondAttrListCollection* atrlistcol=0;
      if (StatusCode::SUCCESS==m_detStore->retrieve(atrlistcol,*it)) {
        // loop over objects in collection
        for (CondAttrListCollection::const_iterator citr=atrlistcol->begin(); citr!=atrlistcol->end();++citr) {

          const coral::AttributeList& atrlist=citr->second;
          *outfile  << atrlist["bec"].data<int>()
                    << " "     << atrlist["layer"].data<int>()
                    << " "     << atrlist["sector"].data<int>()
                    << " "     << atrlist["Tx"].data<float>()
                    << " "     << atrlist["Ty"].data<float>()
                    << " "     << atrlist["Tz"].data<float>()
                    << " "     << atrlist["phi"].data<float>()
                    << " "     << atrlist["theta"].data<float>()
                    << " "     << atrlist["psi"].data<float>() << std::endl;
        }
      }
      else {
        if (msgLvl(MSG::INFO)){
          msg(MSG::INFO) << "Cannot find " << *it << " Container - cannot write DB in text file " << endmsg;
	  return StatusCode::FAILURE;
	}
      }
    }
    outfile->close();
    delete outfile;
    return StatusCode::SUCCESS;
  }
  else {
    ATH_MSG_DEBUG( "writeFile: No dynamic Run2 DB structure is present --> skipping writing file " << file );
    return StatusCode::SUCCESS;
  }
}






StatusCode TRT_AlignDbSvc::tweakGlobalFolder(Identifier ident, Amg::Transform3D trans ) {

  // find transform key, then set appropriate transform
  const CondAttrListCollection* atrlistcol1=0;
  CondAttrListCollection* atrlistcol2=0;
  bool result = false;
  std::string key="/TRT/AlignL1/TRT";
  msg(MSG::DEBUG) << " Identifier is valid: "<< ident.is_valid() << endmsg;
  int bec=m_trtid->barrel_ec(ident);
  const unsigned int DBident=1000+bec*100;
  // so far not a very fancy DB identifier, but seems elaborate enough for this simple structure

  if (StatusCode::SUCCESS==m_detStore->retrieve(atrlistcol1,key)) {
    // loop over objects in collection
    atrlistcol2 = const_cast<CondAttrListCollection*>(atrlistcol1);
    if (atrlistcol1!=0){
      for (CondAttrListCollection::const_iterator citr=atrlistcol2->begin(); citr!=atrlistcol2->end();++citr) {

        const coral::AttributeList& atrlist=citr->second;
	coral::AttributeList& atrlist2  = const_cast<coral::AttributeList&>(atrlist);

        if(citr->first!=DBident){
          msg(MSG::DEBUG) << "tweakGlobalFolder fails due to identifier mismatch" << endmsg;
          continue;
	}
        else {
          msg(MSG::DEBUG) << "Tweak Old global DB -- channel: " << citr->first
			  << " ,bec: "    << atrlist2["bec"].data<int>()
                          << " ,layer: "  << atrlist2["layer"].data<int>()
			  << " ,sector: " << atrlist2["sector"].data<int>()
                          << " ,Tx: "     << atrlist2["Tx"].data<float>()
                          << " ,Ty: "     << atrlist2["Ty"].data<float>()
                          << " ,Tz: "     << atrlist2["Tz"].data<float>()
                          << " ,phi: "    << atrlist2["phi"].data<float>()
                          << " ,theta: "  << atrlist2["theta"].data<float>()
                          << " ,psi: "    << atrlist2["psi"].data<float>() << endmsg;


          // Follow same definitions as in TRT_AlignDbSvc.cxx                                                                                               
	  CLHEP::Hep3Vector  oldtranslation(atrlist2["Tx"].data<float>(),atrlist2["Ty"].data<float>(),atrlist2["Tz"].data<float>());
	  CLHEP::HepRotation oldrotation;
	  oldrotation.set(atrlist["phi"].data<float>(),atrlist["theta"].data<float>(),atrlist["psi"].data<float>());
	  HepGeom::Transform3D oldtransform(oldrotation, oldtranslation);
	  	  
          // get the new transform
	  HepGeom::Transform3D newtrans = Amg::EigenTransformToCLHEP(trans)*oldtransform;
	  Amg::Transform3D newtransAMG = trans*Amg::CLHEPTransformToEigen(oldtransform);

          // Extract the values we need to write to DB
	  Amg::Vector3D shift=newtransAMG.translation();
	  CLHEP::HepRotation rot=newtrans.getRotation();
	  Amg::Vector3D eulerangles;
          eulerangles[0] = rot.getPhi();
          eulerangles[1] = rot.getTheta();
          eulerangles[2] = rot.getPsi();	  

          atrlist2["Tx"].data<float>() = shift.x();
          atrlist2["Ty"].data<float>() = shift.y();
          atrlist2["Tz"].data<float>() = shift.z();
          atrlist2["phi"].data<float>()   = eulerangles[0] ;
          atrlist2["theta"].data<float>() = eulerangles[1] ;
          atrlist2["psi"].data<float>()   = eulerangles[2] ;

	  result = true;
	  msg(MSG::DEBUG) << "Tweak New global DB -- channel: " << citr->first
			  << " ,bec: "    << atrlist2["bec"].data<int>()
                          << " ,layer: "  << atrlist2["layer"].data<int>()
                          << " ,sector: " << atrlist2["sector"].data<int>()
                          << " ,Tx: "     << atrlist2["Tx"].data<float>()
                          << " ,Ty: "     << atrlist2["Ty"].data<float>()
                          << " ,Tz: "     << atrlist2["Tz"].data<float>()
                          << " ,phi: "    << atrlist2["phi"].data<float>()
                          << " ,theta: "  << atrlist2["theta"].data<float>()
                          << " ,psi: "    << atrlist2["psi"].data<float>() << endmsg;

        }
      }
    }
    else {
      ATH_MSG_ERROR("tweakGlobalFolder: cast fails for DBident " << DBident );
      return StatusCode::FAILURE;
    }
  }
  else {
    ATH_MSG_ERROR("tweakGlobalFolder: cannot retrieve CondAttrListCollection for key " << key );
    return StatusCode::FAILURE;
  }

  if (result) return StatusCode::SUCCESS;
  else return StatusCode::FAILURE;
}
