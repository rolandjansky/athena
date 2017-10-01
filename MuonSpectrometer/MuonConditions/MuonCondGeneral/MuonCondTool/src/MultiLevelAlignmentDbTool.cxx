/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "DetDescrConditions/AlignableTransform.h"

#include "Identifier/IdentifierHash.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>

#include "MuonAlignmentData/ALinePar.h"
#include "MuonAlignmentData/BLinePar.h"
#include "MuonAlignmentData/CorrContainer.h"

#include "MuonCondTool/MultiLevelAlignmentDbTool.h" 
#include "MuonCondTool/MuonAlignmentDbTool.h"
//#include "MuonCondInterface/IMuonAlignmentDbTool.h"
#include "MuonCondSvc/MdtStringUtils.h"
#include "Identifier/Identifier.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/CscDetectorElement.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include <map>

//________________________________________________________________________
MultiLevelAlignmentDbTool::MultiLevelAlignmentDbTool (const std::string& type,
						      const std::string& name,
						      const IInterface* parent)
  : AthAlgTool(type, name, parent)   
  , m_abLineDbTool("MuonAlignmentDbTool") 
  , p_muonMgr(0)
  , m_IOVSvc(0)
  , m_alineData(0)
  , m_blineData(0)
  , m_alignModuleALines(0)
  , m_level0Data(0)
  , m_level1Data(0)
  , m_level23Data(0)
  , m_level3Data(0)
  , m_alignroot("/Muon/Align")
  , m_doL0(false)
  , m_doL1(false)
  , m_doL23(false)
  , m_doL3(false)
  , m_useCOOLonly(false)
{
  
  declareInterface<IMultiLevelAlignmentDbTool>(this);
  declareInterface<IMuonAlignmentDbTool>(this);
  
  declareProperty("ABLineDbTool",   m_abLineDbTool);
  declareProperty("L0AsciiFile",    m_level0AsciiFile="");
  declareProperty("L1AsciiFile",    m_level1AsciiFile="");
  declareProperty("L2AsciiFile",    m_level2AsciiFile="");
  declareProperty("L23AsciiFile",   m_level23AsciiFile="");
  declareProperty("L3AsciiFile",    m_level3AsciiFile="");

  declareProperty("DumpALines",     m_dumpALines=false);
  declareProperty("DoSimpleAdd",    m_doSimpleAdd=true);

  m_muonIdHelper  = 0;
  m_mdtIdHelper   = 0;
  m_cscIdHelper   = 0;
  m_rpcIdHelper   = 0;
  m_tgcIdHelper   = 0;

}

//________________________________________________________________________
//StatusCode MultiLevelAlignmentDbTool::updateAddress(SG::TransientAddress* tad)
StatusCode MultiLevelAlignmentDbTool::updateAddress(StoreID::type storeID,
                                                    SG::TransientAddress* tad,
                                                    const EventContext& ctx)
{
  //  StatusCode sc=m_abLineDbTool->updateAddress(tad);
  StatusCode sc=m_abLineDbTool->updateAddress(storeID, tad, ctx);
  if (sc == StatusCode::FAILURE) return sc;

  return StatusCode::FAILURE;
}

//________________________________________________________________________
StatusCode MultiLevelAlignmentDbTool::initialize()
{ 
  ATH_MSG_INFO("Initializing");

  ATH_MSG_INFO("L0  ascii file: "<<m_level0AsciiFile);
  ATH_MSG_INFO("L1  ascii file: "<<m_level1AsciiFile);
  ATH_MSG_INFO("L2  ascii file: "<<m_level2AsciiFile);
  ATH_MSG_INFO("L23 ascii file: "<<m_level23AsciiFile);
  ATH_MSG_INFO("L3  ascii file: "<<m_level3AsciiFile);
  
  m_useCOOLonly = 
    (m_level0AsciiFile=="") && (m_level1AsciiFile=="") && 
    (m_level2AsciiFile=="") && (m_level23AsciiFile=="") &&
    (m_level3AsciiFile=="");
  ATH_MSG_DEBUG("useCOOL: "<<m_useCOOLonly);

  // Get interface to IOVSvc
  m_IOVSvc = 0;
  bool CREATEIF(true);
  StatusCode sc = service( "IOVSvc", m_IOVSvc, CREATEIF );
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Unable to get the IOVSvc");
    return StatusCode::FAILURE;
  }
  
  if (m_abLineDbTool.retrieve().isSuccess())
    msg(MSG::INFO) << "Retrieved " << m_abLineDbTool << endmsg; 
  else{
    msg(MSG::FATAL)<<"Could not get " << m_abLineDbTool <<endmsg; 
    return StatusCode::FAILURE;
  }
   
  m_abParlineFolderNames=m_abLineDbTool->abLineFolderNames();

  // Get the TransientAddress from DetectorStore and set "this" as the
  // AddressProvider
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<ALineMapContainer>::ID(), 
					   m_abLineDbTool->aLineDataLocation());
  if (!proxy) {
    ATH_MSG_ERROR("Unable to get the proxy for class ALineParContainer");
    return StatusCode::FAILURE;
  }
  else ATH_MSG_INFO("proxy for class ALineParContainer found");
  
  IAddressProvider* addp = this;
  proxy->setProvider(addp, StoreID::DETECTOR_STORE);
  //tad->setProvider(addp);
  ATH_MSG_DEBUG("set address provider for ALineParContainer");
  
  proxy = detStore()->proxy(ClassID_traits<BLineMapContainer>::ID(), 
			    m_abLineDbTool->bLineDataLocation());
  if (!proxy) {
    ATH_MSG_ERROR("Unable to get the proxy for class BLineParContainer");
    return StatusCode::FAILURE;
  }
  else ATH_MSG_INFO("proxy for class BLineParContainer found");
  
  addp = this;
  proxy->setProvider(addp, StoreID::DETECTOR_STORE);
  //tad->setProvider(addp);
  ATH_MSG_DEBUG("set address provider for BLineParContainer");
  
  m_alignobjs.clear();

  m_level0Data  = new ALineMapContainer();
  m_level1Data  = new ALineMapContainer();
  m_level23Data = new ALineMapContainer();
  m_level3Data  = new ALineMapContainer();

  ATH_MSG_DEBUG("done with initialize, alineData: "<<m_alineData);
  if (m_alineData) ATH_MSG_DEBUG("alineData size: "<<m_alineData->size());

  return sc;
  
}

//________________________________________________________________________
MultiLevelAlignmentDbTool::~MultiLevelAlignmentDbTool()
{ 
  //delete m_level0Data;
  //delete m_level1Data;
  //delete m_level3Data;
}

//________________________________________________________________________
StatusCode MultiLevelAlignmentDbTool::loadParameters(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  ATH_MSG_INFO("entering multi-level loadParameters");

  ATH_MSG_DEBUG("alineData: "<<m_alineData);
  if (m_alineData) ATH_MSG_DEBUG("alineData size: "<<m_alineData->size());

  StatusCode sc=StatusCode::SUCCESS;

  if (m_alignobjs.size()==0 && 
      StatusCode::SUCCESS!=initializeMdtAlignObjs()) {
    ATH_MSG_ERROR("failed to properly initialize MDT alignment objects!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("after initializeMdtAlignObjs, alineData: "<<m_alineData);
  if (m_alineData) ATH_MSG_DEBUG("alineData size: "<<m_alineData->size());

  // get set of keys for aline and bline folders and reload a/b lines
  std::list<std::string> abKeys;
  if (!keys.empty()) {
    ATH_MSG_DEBUG("keys not empty, creating abKeys with abParlineFolderNames");
    std::list<std::string>::const_iterator itr;    
    for (itr=keys.begin();itr!=keys.end();++itr) 
      if (find(m_abParlineFolderNames.begin(),m_abParlineFolderNames.end(),*itr) != 
	  m_abParlineFolderNames.end()) abKeys.push_back(*itr);  
  }

  // get parameter folders from MuonAlignmentDbTool
  std::vector<std::string> parlineFolders=m_abLineDbTool->abLineFolderNames();
  if (abKeys.size()>0 || parlineFolders.size()>0) {
    ATH_MSG_DEBUG("calling abLineDbTool->loadParameters()");
    sc = m_abLineDbTool->loadParameters(I,abKeys);
    ATH_MSG_DEBUG("sc: "<<sc);
    if (!sc.isSuccess()&&!sc.isRecoverable()) {
      ATH_MSG_DEBUG("not recoverable");
      return sc;
    }
    ATH_MSG_DEBUG("getting a/b lines");
    m_alineData=const_cast<ALineMapContainer*>(m_abLineDbTool->ALineContainer());
    m_blineData=const_cast<BLineMapContainer*>(m_abLineDbTool->BLineContainer());
    ATH_MSG_DEBUG("got a/b lines");   
  } 
  else {   
    ATH_MSG_DEBUG("retrieving a/b lines from oracle or ascii file");
    if (StatusCode::SUCCESS != detStore()->retrieve(m_alineData,"MDT_A_LINE_CORR") ||
        StatusCode::SUCCESS != detStore()->retrieve(m_blineData,"MDT_B_LINE_CORR")) {
      ATH_MSG_ERROR("couldn't retrieve A/B lines from detector store!  Exiting.");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG("retrieved a/b lines");
      if (m_alineData) ATH_MSG_DEBUG("alineData size: "<<m_alineData->size());
    }
  }    

  ATH_MSG_DEBUG("alineData: "<<m_alineData);
  if (m_alineData) ATH_MSG_DEBUG("alineData size: "<<m_alineData->size());
  else {
    ATH_MSG_INFO("multilevel LoadParameters done - with no alineData!");
    return sc;
  }

//   if (abKeys.size()>0) {
//     ATH_MSG_DEBUG("calling abLineDbTool->loadParameters()");
//     sc = m_abLineDbTool->loadParameters(I,abKeys);
////     if (sc != StatusCode::SUCCESS) return sc;
//    if (!sc.isRecoverable()) return sc;
//     ATH_MSG_DEBUG("getting a/b lines");
//     m_alineData=const_cast<ALineMapContainer*>(m_abLineDbTool->ALineContainer());
//     m_blineData=const_cast<BLineMapContainer*>(m_abLineDbTool->BLineContainer());
//     ATH_MSG_DEBUG("got a/b lines");
//   }
//   else {
//  ATH_MSG_DEBUG("retrieving a/b lines from oracle or ascii file");
//  if (StatusCode::SUCCESS != detStore()->retrieve(m_alineData,"A_LINE_CORR_updates") ||
//      StatusCode::SUCCESS != detStore()->retrieve(m_blineData,"B_LINE_CORR_updates")) {
    //ATH_MSG_DEBUG("retrieving a/b lines");
    //if (StatusCode::SUCCESS != detStore()->retrieve(m_alineData,"MDT_A_LINE_CORR") ||
    //	StatusCode::SUCCESS != detStore()->retrieve(m_blineData,"MDT_B_LINE_CORR")) {
  //    ATH_MSG_ERROR("couldn't retrieve A/B lines from detector store!  Exiting.");
  //    return StatusCode::FAILURE;
  //  }
  // else ATH_MSG_DEBUG("retrieved a/b lines");
    //  }
  
  //if (m_useCOOLonly) return StatusCode::SUCCESS;

  const Amg::Transform3D L0Transform = level0Transform();
  /*
  ATH_MSG_DEBUG("L0 transform: "<<endmsg
		<<L0Transform[0][0]<<" "<<L0Transform[0][1]<<" "<<L0Transform[0][2]<<endmsg
		<<L0Transform[1][0]<<" "<<L0Transform[1][1]<<" "<<L0Transform[1][2]<<endmsg
		<<L0Transform[2][0]<<" "<<L0Transform[2][1]<<" "<<L0Transform[2][2]);
  */

  for (ciALineMap cialine = m_alineData->begin(); 
       cialine != m_alineData->end(); ++cialine) {
    Identifier ALineId = (*cialine).first;
    ALinePar* ALine = (*cialine).second;
    ATH_MSG_DEBUG("------------------------------------");
    ATH_MSG_DEBUG("have ALine for "
		  <<m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(ALineId))
		  <<" "<<m_mdtIdHelper->stationPhi(ALineId)
		  <<" "<<m_mdtIdHelper->stationEta(ALineId)
		  <<" -- ALineId="<<ALineId);
  
    const Amg::Transform3D L1Transform   = level1Transform(ALineId);
    const Amg::Transform3D L2Transform   = level2Transform(ALineId);
    const Amg::Transform3D L23Transform  = level23Transform(ALineId);
    const Amg::Transform3D L3Transform   = level3Transform(ALineId);
    
    const Amg::Transform3D alignToGlobal = alignToGlobalTransform(ALineId);
    const Amg::Transform3D globalToL23Module = globalToL23ModuleTransform(ALineId);
    
    ATH_MSG_VERBOSE("L0 transform: "<<endmsg<<std::showpos<<std::setw(6) << Amg::toString(L0Transform));
    ATH_MSG_VERBOSE("L1 transform: "<<endmsg<<std::showpos<<std::setw(6) << Amg::toString(L1Transform));
    ATH_MSG_VERBOSE("L2 transform: "<<endmsg<<std::showpos<<std::setw(6) << Amg::toString(L2Transform));
    ATH_MSG_VERBOSE("L23 transform: "<<endmsg<<std::showpos<<std::setw(6) << Amg::toString(L23Transform));
    ATH_MSG_VERBOSE("L3 transform: "<<endmsg<<std::showpos<<std::setw(6) << Amg::toString(L3Transform));
    ATH_MSG_VERBOSE("alignToGlobal transform: "<<endmsg<<std::showpos<<std::setw(6) << Amg::toString(alignToGlobal));
    ATH_MSG_VERBOSE("globalToL23Module transform: "<<endmsg<<std::showpos<<std::setw(6) << Amg::toString(globalToL23Module));

    ATH_MSG_VERBOSE(" for: "
                    <<m_mdtIdHelper->stationNameString(m_mdtIdHelper->stationName(ALineId))
                    //<<m_mdtIdHelper->stationName(ALineId)
                    <<" "<<m_mdtIdHelper->stationPhi(ALineId)
                    <<" "<<m_mdtIdHelper->stationEta(ALineId));
    
    

    Amg::Transform3D delta=alignToGlobal.inverse() 
      * L0Transform * L1Transform * L2Transform 
      * globalToL23Module.inverse() * L23Transform * globalToL23Module 
      * alignToGlobal * L3Transform;

    ATH_MSG_VERBOSE("delta transform: "<<endmsg<<std::showpos<<std::setw(6) << Amg::toString(delta) );
    
    double* newALines=new double[6];
    
    Amg::Transform3D deltaTransform = 
      alignToGlobal.inverse() * L0Transform * L1Transform * L2Transform 
      * globalToL23Module.inverse() * L23Transform * globalToL23Module
      * alignToGlobal * L3Transform;

    if (m_doSimpleAdd) {

      float tras,traz,trat,rots,rotz,rott;
      ALine->getParameters(tras,traz,trat,rots,rotz,rott);
      newALines[0]=trat;
      newALines[1]=tras;
      newALines[2]=traz;
      newALines[3]=rott;
      newALines[4]=rots;
      newALines[5]=rotz;
      
      double* deltas=new double[6];
      decomposeTransform(deltaTransform,deltas);
      for (int i=0;i<6;i++) newALines[i]+=deltas[i];
      delete [] deltas;
      
    }
    else {
      

      ATH_MSG_VERBOSE("old transform: "<<endmsg<<std::showpos<<std::setw(6) << Amg::toString(ALine->deltaTransform()) );
      
      Amg::Transform3D  newDeltaTransform = deltaTransform * Amg::CLHEPTransformToEigen(ALine->deltaTransform());
      	
      ATH_MSG_VERBOSE("new transform: "<<endmsg<<std::showpos<<std::setw(6) << Amg::toString(newDeltaTransform) );
      decomposeTransform(newDeltaTransform,newALines);
    }

    float new_T    = newALines[0];
    float new_S    = newALines[1];
    float new_Z    = newALines[2];
    float new_RotT = newALines[3];
    float new_RotS = newALines[4];
    float new_RotZ = newALines[5];

    // dump old ALines
    float s,z,t,rots,rotz,rott;
    ALine->getParameters(s,z,t,rots,rotz,rott);
    
    ATH_MSG_DEBUG("old translations: "<<std::showpos<<std::setw(6)<<std::fixed
		  <<s<<", "<<z<<", "<<t
		  <<", rotations: "<<rots<<", "<<rotz<<", "<<rott);    
    
    // dump new ALines
    ATH_MSG_DEBUG("new translations: "<<std::showpos<<std::setw(6)<<std::fixed
		  <<new_S<<", "<<new_Z<<", "<<new_T
		  <<", rotations: "<<new_RotS<<", "<<new_RotZ<<", "<<new_RotT);
    
    ALine->setParameters(new_S,new_Z,new_T,new_RotS,new_RotZ,new_RotT);
    delete [] newALines;
  
  }
  
  if (m_dumpALines) {
    
    ATH_MSG_INFO("dumping A-line");
    
    std::cout << "A "
	      << "type jff jzz job s(cm)  z(cm)  t(cm)  ths(rad)  thz(rad)  tht(rad)  ID"<< std::endl;
    
    for (ciALineMap cialine = m_alineData->begin(); cialine != m_alineData->end(); ++cialine) {
      Identifier ALineId = (*cialine).first;
      ALinePar* ALine = (*cialine).second;
      std::string stationType;
      int jff,jzz,job;
      ALine->getAmdbId(stationType,jff,jzz,job);
      float s,z,t,ths,thz,tht;
      ALine->getParameters(s,z,t,ths,thz,tht);
      
      std::cout <<"A " << std::setiosflags(std::ios::fixed|std::ios::right)
		<< std::setw(4) << stationType  <<" " 
		<< std::setw(2) << jff  <<"  " 
		<< std::setw(2) << jzz  <<" " 
		<< std::setw(2) << job  <<" "
		<< std::setw(6) << std::setprecision(3) <<  s   <<" "  // here cm !
		<< std::setw(6) << std::setprecision(3) <<  z   <<" "  // here cm !
		<< std::setw(6) << std::setprecision(3) <<  t   <<" "  // here cm !
		<< std::setw(6) << std::setprecision(6) << ths  <<" " 
		<< std::setw(6) << std::setprecision(6) << thz  <<" " 
		<< std::setw(6) << std::setprecision(6) << tht  <<" "
		<< ALineId << std::endl;
    }
    std::cout<<std::endl;        
  }

  ATH_MSG_INFO("multilevel LoadParameters done !");
  
  return sc;
  
}

//________________________________________________________________________
StatusCode MultiLevelAlignmentDbTool::loadAlignALine(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
  ATH_MSG_WARNING("Loading A-lines directly!");
  return m_abLineDbTool->loadAlignALine(I,keys);
}

//________________________________________________________________________
StatusCode MultiLevelAlignmentDbTool::loadAlignBLine(IOVSVC_CALLBACK_ARGS_P(I,keys)) 
{
  ATH_MSG_WARNING("Loading B-lines directly!");
  return m_abLineDbTool->loadAlignBLine(I,keys);
}
   
//________________________________________________________________________
std::vector<std::string> MultiLevelAlignmentDbTool::abLineFolderNames() const
{return m_abLineDbTool->abLineFolderNames();}
std::string MultiLevelAlignmentDbTool::aLineFolderName() const
{return m_abLineDbTool->aLineFolderName();}
std::string MultiLevelAlignmentDbTool::bLineFolderName() const
{return m_abLineDbTool->bLineFolderName();}
const ALineMapContainer*
MultiLevelAlignmentDbTool::ALineContainer() const
{return m_alineData;}
const BLineMapContainer*
MultiLevelAlignmentDbTool::BLineContainer() const
{return m_blineData;}
const CscInternalAlignmentMapContainer*
MultiLevelAlignmentDbTool::ILineContainer() const
{ATH_MSG_FATAL("Functionality for CSC I-lines not implemented here!");
return NULL;}
const MdtAsBuiltMapContainer*
MultiLevelAlignmentDbTool::AsBuiltContainer() const
{ATH_MSG_FATAL("Functionality for AsBuilt parameteres not implemented here!");
return NULL;}
const ALineMapContainer*
MultiLevelAlignmentDbTool::level0Container() const
{return m_level0Data;}
const ALineMapContainer*
MultiLevelAlignmentDbTool::level1Container() const
{return m_level1Data;}
const ALineMapContainer*
MultiLevelAlignmentDbTool::level23Container() const
{return m_level23Data;}
const ALineMapContainer*
MultiLevelAlignmentDbTool::level3Container() const
{return m_level3Data;}

//________________________________________________________________________
StatusCode MultiLevelAlignmentDbTool::readAlignTextFile(const std::string& file, 
							const std::string& level)
{
  
  ATH_MSG_INFO(" Set alignment constants from text file "<< file);
  
  std::ifstream infile;
  infile.open(file.c_str());
  
  // first clean the alignment container: loop over all
  // AlignableTransforms and empty them.
  const AlignableTransformContainer* container= m_aligncontainerhandle;
  if (container) {
    AlignableTransformContainer* nccontainer = 
      const_cast<AlignableTransformContainer*>(container) ;
    for(DataVector<AlignableTransform>::iterator it=nccontainer->begin();
        it!=nccontainer->end();++it)
      // there is no 'clear' function in alignable transform, so we do it with this hack
      **it = AlignableTransform( (*it)->tag() ) ;
  } 
  else {
    ATH_MSG_ERROR("no container!");
    return StatusCode::FAILURE ;
  }  

  // loop over file
  int nobj=0;
  int ntrans=0;
  std::string atname;
  AlignableTransform* pat(0) ;
  char line[512] ;
  ATH_MSG_DEBUG("reading file");
  //int counter = 0;
  
  ATH_MSG_DEBUG("is open: " << infile.is_open());
  ATH_MSG_DEBUG("is good: " << infile.good());

  //while(!infile.eof()){
  //  infile >> line;
  //  ATH_MSG_DEBUG("line1: " <<line);
  // }

  //infile.getline(line,512);
  //ATH_MSG_DEBUG("line1: " <<line);
  //ATH_MSG_DEBUG("start loop");
  while( infile.getline(line,512) ) {
    //ATH_MSG_DEBUG("linenumber: " << counter);
    //ATH_MSG_DEBUG("line: " << line);

    if (line[0] != '#') {
      
      std::string linestring(line);
      ATH_MSG_DEBUG("linestring "<<linestring);
      if( linestring.find('/') != std::string::npos) {
        // this must be a line with a container name
        atname = linestring ;
        ATH_MSG_INFO("now reading container: " << atname);
        pat=getTransPtr(atname);
        if (!pat) {
          ATH_MSG_ERROR("Cannot find AlignableTransform object for key" 
			<< atname);
          return StatusCode::FAILURE;
        } 
	else nobj++;
        
      } 
      else if (pat!=0) {

        // this must be a line with constants
        std::istringstream is(line) ;
        
        char AlineMarker[2];
	std::string name; int jff,jzz,obj;
        float tras,traz,trat,rots,rotz,rott;
        is >> AlineMarker >> name >> jff >> jzz >> obj
	   >> tras >> traz >> trat >> rots >> rotz >> rott;

	ATH_MSG_INFO("read line: "<<is.str());

        if( AlineMarker[0] == '\0') {
          ATH_MSG_INFO("read empty line!");
	}
	else {
	  
	  float tx = tras;
	  float ty = traz;
	  float tz = trat;
	  float rx = rots;
	  float ry = rotz;
	  float rz = rott;
	  if (level=="L3" || level=="L23") {
	    tx = trat;
	    ty = tras;
	    tz = traz;
	    rx = rott;
	    ry = rots;
	    rz = rotz;
	  }
	  Amg::Transform3D delta = Amg::Transform3D::Identity();
	  delta *= Amg::AngleAxis3D(rx,Amg::Vector3D(1.,0.,0.));
	  delta *= Amg::AngleAxis3D(ry,Amg::Vector3D(0.,1.,0.));
	  delta *= Amg::AngleAxis3D(rz,Amg::Vector3D(0.,0.,1.));
	  delta *= Amg::Translation3D(tx,ty,tz);

	  bool isValid(true);
          Identifier ident=identify(name,jff,jzz,true,&isValid,level);
	  if (!isValid) ATH_MSG_ERROR("not valid ID!");

	  HepGeom::Transform3D deltaCLHEP(HepGeom::Translate3D(tx,ty,tz)*
					  HepGeom::RotateX3D(rx)* 
					  HepGeom::RotateY3D(ry)*
					  HepGeom::RotateZ3D(rz));

	  pat->add(ident,deltaCLHEP);
          //pat->add(ident,delta);

	  // add to ALineContainer 
	  ALineMapContainer* data;
	  if      (level=="L0")  { data=m_level0Data;  m_doL0 =true; }
	  else if (level=="L1")  { data=m_level1Data;  m_doL1 =true; }
	  else if (level=="L23") { data=m_level23Data; m_doL23=true; }
	  else if (level=="L3")  { data=m_level3Data;  m_doL3 =true; }
	  else {
	    ATH_MSG_ERROR("bad level!");
	    return StatusCode::FAILURE;
	  }

	  iALineMap ialine;
	  if ((ialine=data->find(ident)) != data->end()) {
	    ATH_MSG_WARNING("More than one entry for "<<name<<" at Jzz/Jff "
			    <<jzz<<"/"<<jff<<" --- keep the latest one");
	    data->erase(ident);	     
	  }

	  ALinePar* newALine = new ALinePar();
	  newALine->setAmdbId(name,jff,jzz,obj);
	  newALine->setParameters(tras,traz,trat,rots,rotz,rott);
	  newALine->isNew(true);

	  data->insert(std::make_pair(ident,(ALinePar*)newALine));

	  // dump to screen
          ATH_MSG_INFO("Set transform: "
		       << " [" << name << " " << jff << " " << jzz 
		       << "] id " << ident 
		       << endmsg
		       << " rot   [" << rots << "," << rotz << "," << rott << "]"
		       <<endmsg
		       << " shift [" << tras << "," << traz << "," << trat << "]");
	  if (msgLvl(MSG::DEBUG)) {
	    ATH_MSG_DEBUG("decomposed: ");
	    double* values=new double[6];
	    decomposeTransform(delta,values);
	    ATH_MSG_DEBUG(" [" << name << " " << jff << " " << jzz 
			  << "] id " << ident 
			  << endmsg
			  << " rot   [" << values[3] << "," << values[4] << "," << values[5] << "]"
			  <<endmsg
			  << " shift [" << values[0] << "," << values[1] << "," << values[2] << "]");
	    delete [] values;
	  }
          ntrans++;
        }
      }
    }
  }
  infile.close() ;
  ATH_MSG_INFO("Read " << nobj << 
	       " objects from file with " << ntrans << " transforms." );
  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
AlignableTransform* MultiLevelAlignmentDbTool::getTransPtr(const std::string key) const 
{
  return const_cast<AlignableTransform*>(cgetTransPtr(key));
}

//----------------------------------------------------------------------------
const AlignableTransform* 
MultiLevelAlignmentDbTool::cgetTransPtr(const std::string key) const 
{
  // Retrieve AlignableTransform pointer for a given key - const version
  const AlignableTransform* pat=0;
  const AlignableTransformContainer* patc = m_aligncontainerhandle;

  if (patc) {
    for(DataVector<AlignableTransform>::const_iterator dva=patc->begin();
	dva!=patc->end();++dva) {
      if( (*dva)->tag()==key ) {
	pat = *dva;
	break;
      }
    }
  }
  return pat;  
}

//________________________________________________________________________
StatusCode MultiLevelAlignmentDbTool::initializeMdtAlignObjs()
{  
  ATH_MSG_DEBUG("in initializeMdtAlignObjs");

  // get muon ID helpers  
  StatusCode sc = detStore()->retrieve(m_mdtIdHelper, "MDTIDHELPER" );
  if (!sc.isSuccess()) {	
    ATH_MSG_ERROR("Can't retrieve MdtIdHelper");
    return sc;
  }
  else ATH_MSG_INFO("MdtIdHelper retrieved from the DetectorStore");

  sc = detStore()->retrieve(m_cscIdHelper, "CSCIDHELPER" );
  if (!sc.isSuccess()) {	
    ATH_MSG_ERROR("Can't retrieve CscIdHelper");
    return sc;
  }
  else ATH_MSG_INFO("CscIdHelper retrieved from the DetectorStore");

  sc = detStore()->retrieve(m_rpcIdHelper, "RPCIDHELPER" );
  if (!sc.isSuccess()) {	
    ATH_MSG_ERROR("Can't retrieve RpcIdHelper");
    return sc;
  }
  else ATH_MSG_INFO("RpcIdHelper retrieved from the DetectorStore");

  sc = detStore()->retrieve(m_tgcIdHelper, "TGCIDHELPER" );
  if (!sc.isSuccess()) {	
    ATH_MSG_ERROR("Can't retrieve TgcIdHelper");
    return sc;
  }
  else ATH_MSG_INFO("TgcIdHelper retrieved from the DetectorStore");
  
  // get muon detector manager  
  sc=detStore()->retrieve(p_muonMgr);
  if (sc.isFailure()) {
    msg(MSG::FATAL)<<"Cannot retrieve MuonDetectorManager"<<endmsg;
    return sc;
  }
  else {
    msg(MSG::DEBUG)<<"retrieved MuonDetectorManager"<<endmsg;
    p_muonMgr->msgSvc()->setOutputLevel("MuonDetectorManager", 
					this->msgSvc()->outputLevel());
  }
  
  int ichan(0);
  
  // Level 0 alignment
  m_alignobjs.push_back("/Muon/Align/L0");
  m_alignchans.push_back(ichan++);

  // Level 1 alignment
  m_alignobjs.push_back("/Muon/Align/L1");
  m_alignchans.push_back(ichan++);

  // Level 2 alignment
  m_alignobjs.push_back("/Muon/Align/L2_ECA");
  m_alignobjs.push_back("/Muon/Align/L2_ECC");
  m_alignobjs.push_back("/Muon/Align/L2_Bar");
  for (int i=0;i<3;i++) m_alignchans.push_back(ichan++);

  // Level 23 alignment
  m_alignobjs.push_back("/Muon/Align/L23_ECA");
  m_alignobjs.push_back("/Muon/Align/L23_ECC");
  m_alignobjs.push_back("/Muon/Align/L23_Bar");
  for (int i=0;i<3;i++) m_alignchans.push_back(ichan++);

  // Level 3 alignment
  //m_alignobjs.push_back("/Muon/Align/L3_ECA");
  //m_alignobjs.push_back("/Muon/Align/L3_ECC");
  //m_alignobjs.push_back("/Muon/Align/L3_Bar");
  //for (int i=0;i<3;i++) m_alignchans.push_back(ichan++);

  m_alignobjs.push_back("/Muon/Align/L3");
  for (int i=0;i<1;i++) m_alignchans.push_back(ichan++);

  // If the folder exists, register a callback. If we read from
  // textfile and the folder already exists, read the texfile in the
  // callback. If the folder does not yet exist, create and record it.
  
  bool alignfolderexists = detStore()->contains<AlignableTransformContainer>(m_alignroot) ;

  if (alignfolderexists && m_useCOOLonly ) {
    // register the callback
    if ( (detStore()->regFcn(&IMuonAlignmentDbTool::loadParameters,
			     dynamic_cast<IMuonAlignmentDbTool*>(this),
			     m_aligncontainerhandle,m_alignroot)).isFailure() ) {
      ATH_MSG_FATAL("cannot register callback for folder " << m_alignroot);
      return StatusCode::FAILURE ;
    }
  }
  else if (!m_useCOOLonly) {
    
    // create alignment objects and fill them from textfile.
    if (StatusCode::SUCCESS!=createAlignObjects()) {
      ATH_MSG_FATAL("Could not create and record new AlignableTransformContainer. Exiting.");
      return StatusCode::FAILURE ;
    }
    
    if (m_level0AsciiFile!="" &&
	StatusCode::SUCCESS!=readAlignTextFile(m_level0AsciiFile,"L0")) {
      ATH_MSG_FATAL("Could not read "<<m_level0AsciiFile);
      return StatusCode::FAILURE;
    }
    if (m_level1AsciiFile!="" &&
	StatusCode::SUCCESS!=readAlignTextFile(m_level1AsciiFile,"L1")) {
      ATH_MSG_FATAL("Could not read "<<m_level1AsciiFile);
      return StatusCode::FAILURE;
    }    
    if (m_level2AsciiFile!="" &&
	StatusCode::SUCCESS!=readAlignTextFile(m_level2AsciiFile,"L2")) {
      ATH_MSG_FATAL("Could not read "<<m_level2AsciiFile);
      return StatusCode::FAILURE;
    }
    if (m_level23AsciiFile!="" &&
	StatusCode::SUCCESS!=readAlignTextFile(m_level23AsciiFile,"L23")) {
      ATH_MSG_FATAL("Could not read "<<m_level23AsciiFile);
      return StatusCode::FAILURE;
    }
    if (m_level3AsciiFile!="" &&
	StatusCode::SUCCESS!=readAlignTextFile(m_level3AsciiFile,"L3")) {
      ATH_MSG_FATAL("Could not read "<<m_level3AsciiFile);
      return StatusCode::FAILURE;
    } 

  } 
  else 
    ATH_MSG_WARNING("AlignableTransformContainer not in IOV service and no textfile specified.");    
  
  
  ATH_MSG_DEBUG("The Muon/Align keys and channels are:");
  for (unsigned int i=0;i<m_alignobjs.size();++i)
    ATH_MSG_DEBUG(" key " << m_alignobjs[i] << " chan " << m_alignchans[i]);  
  
  return StatusCode::SUCCESS;

}

//-----------------------------------------------------------------------------
StatusCode MultiLevelAlignmentDbTool::createAlignObjects() const
{
  
  // Create empty alignment objects.
  // Use when constants are read in from textfiles.
  // Make sure that the objects are not read in by IOV before
  
  ATH_MSG_INFO("createAlignObjects method called");

  // check if collection already exists
  if (detStore()->contains<AlignableTransformContainer>(m_alignroot)) {
    ATH_MSG_WARNING("createAlignObjects: AlignableTransformContainer with name " 
		    << m_alignroot << " already exists.");
    return StatusCode::FAILURE;
  }
  
  // Create the appropiate (empty) AlignableTransforms for this geometry
  AlignableTransformContainer* patc = new AlignableTransformContainer();
  
  // record it
  if (StatusCode::SUCCESS!=detStore()->record(patc,m_alignroot)) {
    ATH_MSG_ERROR("Could not record AlignableTransformContainer ");
    return StatusCode::FAILURE;
  } 
  else 
    ATH_MSG_INFO("Created and recorded new AlignableTransformContainer");
  
  
  // set the data handle (can probably be done more efficiently). it
  // is also not clear who is the owner now ...
  if (StatusCode::SUCCESS!=detStore()->retrieve(m_aligncontainerhandle,m_alignroot)) {
    ATH_MSG_ERROR("Could not retrieve data handle for AlignableTransformContainer ");
    return StatusCode::FAILURE;
  }
  
  for (unsigned int i=0;i<m_alignobjs.size();++i) {
    AlignableTransform* pat=new AlignableTransform(m_alignobjs[i]);
    patc->push_back(pat);
    patc->add(m_alignchans[i]);
    ATH_MSG_INFO(" added empty AlignableTransform for key "<< m_alignobjs[i]);
  }
  
  return StatusCode::SUCCESS;
}


//________________________________________________________________________
Identifier 
MultiLevelAlignmentDbTool::identify(std::string name, int stPhi, int stEta,
				    bool check, bool* isValid,
                                    std::string level) const
{
  ATH_MSG_DEBUG("in identify, name="<<name<<", phi/eta="<<stPhi<<"/"<<stEta);

  std::string subname=name.substr(0,3);
  if (level=="L0")  return m_mdtIdHelper->mdt(); // use mdt or muon?
  
  // if L1 or L2, Identifiers don't exist (yet) so use these that I made up
  else if (level=="L1") {
    unsigned int id;   
    if      (name=="ECA") id = L1_ECA;
    else if (name=="ECC") id = L1_ECC;
    else if (name=="Bar") id = L1_BAR;
    else {
      ATH_MSG_ERROR("bad name. returning 0"); 
      return Identifier(0); 
    }

    std::string endname=name.substr(3,1);

    // TGC chambers
    if      (endname=="4") id += L2_EI;
    else if (endname=="2") id += L2_EM;
    else if (endname=="3") id += L2_EO;
    else if (endname=="1") id += L2_EE;
    
    // CSC chambers
    else if (name.substr(0,3)=="CSS" || name.substr(0,3)=="CSL") id += L2_EI;    

    // MDT endcap chambers
    else if (name.substr(0,1)=="E") {
      if      (endname=="I" || endname=="4") id += L2_EI;
      else if (endname=="M" || endname=="2") id += L2_EM;
      else if (endname=="O" || endname=="3") id += L2_EO;
      else if (endname=="R" || endname=="1") id += L2_EE;
      else if (name.substr(0,3)=="CSS" || name.substr(0,3)=="CSL") id += L2_EI;
    }

    // MDT barrel chambers
    else {
      if      (endname=="0") id += L2_B0;
      else if (endname=="1") id += L2_B1;
      else if (endname=="2") id += L2_B2;
      else if (endname=="3") id += L2_B3;
      else if (endname=="4") id += L2_B4;
      else if (endname=="5") id += L2_B5;
      else if (endname=="6") id += L2_B6;
      else if (endname=="7") id += L2_B7;
    }
    // To be compatible with 64-bit ids, we first create an id32, then
    // a normal id. This does the bit shifting correctly compared to
    // the standard ids. RDS 02/2010
    return Identifier(Identifier32(id));
  }
  else if(level=="L23"){
     
    std::string chambername;
    if      (name=="SAL") chambername="BIL";
    else if (name=="SIS") chambername="BIS";
    else if (name=="SMS") chambername="BMS";
    else if (name=="SOS") chambername="BOS";
    else { 
      chambername = name;    
    }
    
    ATH_MSG_DEBUG("name: " << chambername << ", eta: " << stEta << ", stPhi: " << stPhi);
    Identifier id = m_mdtIdHelper->elementID(chambername,stEta,stPhi,check,isValid);
    ATH_MSG_DEBUG("aline identifier: " << id.getString());
   
    return id;
  }

  

  // L3 -- use standard Identifiers
  else 
    return m_mdtIdHelper->elementID(name,stEta,stPhi,check,isValid);
}

//________________________________________________________________________
Identifier
MultiLevelAlignmentDbTool::getL23ModuleId(Identifier id) const
{

  ATH_MSG_DEBUG("in getL23ModuleId");

  const MuonIdHelper* midh = muonIdHelper(id);
  if (!midh || !midh->is_mdt(id)){
    ATH_MSG_DEBUG("Not a Level23 module");
    return Identifier();
  }
  ATH_MSG_DEBUG("have IdHelper");

  std::string chambername = midh->stationNameString(midh->stationName(id));
  int phi = midh->stationPhi(id);
  int eta = midh->stationEta(id);

  ATH_MSG_DEBUG("chamber name: " << chambername << ", eta: " << eta << ", phi: " << phi);

  std::string firstChamberName = chambername.substr(2,1)=="L" ? "BIL" : chambername;
  int firstChamberEta = eta!=0 ? eta/abs(eta) : eta;

  ATH_MSG_DEBUG("chamber: " << chambername 
                << ", firstChamber: " <<  firstChamberName
                << ", eta: " << eta
                << ", firstEta: " << firstChamberEta << ", firstPhi: " << phi);
  bool isValid(true);
  Identifier firstChamberId 
    = m_mdtIdHelper->elementID(firstChamberName,firstChamberEta,phi,true,&isValid);
  
  ATH_MSG_DEBUG("identifier: " << firstChamberId.getString());
  const MuonGM::MdtReadoutElement* mdt=p_muonMgr->getMdtReadoutElement(id);
  if (mdt) {
    ATH_MSG_DEBUG("first L23 tube position (" << mdt->tubePos(mdt->getMultilayer(),1,1)<<")");
    ATH_MSG_DEBUG("for chamber "<<mdt->identify() << ", ml: " << mdt->getMultilayer());
  }

  return firstChamberId;
  
}

//________________________________________________________________________
const Amg::Transform3D
MultiLevelAlignmentDbTool::level0Transform() const
{
  if (!m_doL0) return Amg::Transform3D();

  const AlignableTransform* pat=cgetTransPtr("/Muon/Align/L0");
  if (!pat) return Amg::Transform3D();
  
  Identifier mid=m_mdtIdHelper->mdt();
  AlignableTransform::AlignTransMem_citr itr=pat->findIdent(mid);
  return ( itr!=pat->end() ? Amg::CLHEPTransformToEigen(itr->transform()) : Amg::Transform3D() );
}

//________________________________________________________________________
const Amg::Transform3D 
MultiLevelAlignmentDbTool::level1Transform(Identifier mdtId) const
{
  if (!m_doL1) return Amg::Transform3D();

  const AlignableTransform* pat=cgetTransPtr("/Muon/Align/L1");

  const MuonIdHelper* midh = muonIdHelper(mdtId);
  if (!pat || !midh) return Amg::Transform3D();
  
  Identifier id;
  if (midh->isBarrel(mdtId))       id = L1_BAR;
  else {
    if (midh->stationEta(mdtId)>0) id = L1_ECA;
    else                           id = L1_ECC;
  }

  AlignableTransform::AlignTransMem_citr itr=pat->findIdent(id);
  return ( itr!=pat->end() ? Amg::CLHEPTransformToEigen(itr->transform()) : Amg::Transform3D() );
}

//________________________________________________________________________
const Amg::Transform3D 
MultiLevelAlignmentDbTool::level2Transform(Identifier mdtId) const
{

  const MuonIdHelper* midh = muonIdHelper(mdtId);
  if (!midh) return Amg::Transform3D();

  const AlignableTransform* pat;  
  
  int intId;
  
  // barrel
  if (midh->isBarrel(mdtId)) {
    pat=cgetTransPtr("/Muon/Align/L2_Bar");
    intId=L1_BAR + midh->stationPhi(mdtId)+1;
  }
  
  // endcap
  else {
    if (midh->stationEta(mdtId)>0) {
      pat=cgetTransPtr("/Muon/Align/L2_ECA");
      intId=L1_ECA;
    }
    else {
      pat=cgetTransPtr("/Muon/Align/L2_ECC");
      intId=L1_ECC;
    }
    
    int statRegion = midh->stationRegion(mdtId);
    intId += statRegion;
  }
  
  if (!pat) return Amg::Transform3D();
  AlignableTransform::AlignTransMem_citr itr=pat->findIdent(Identifier(intId));
  return ( itr!=pat->end() ? Amg::CLHEPTransformToEigen(itr->transform()) : Amg::Transform3D() );
  
}


//________________________________________________________________________
const Amg::Transform3D 
MultiLevelAlignmentDbTool::level23Transform(Identifier mdtId) const
{
  if (!m_doL23) return Amg::Transform3D();
  
  ATH_MSG_DEBUG("in level23Transform");

  const AlignableTransform* pat;    

  const MuonIdHelper* midh = muonIdHelper(mdtId);
  if (!midh) return Amg::Transform3D();

  if (midh->isBarrel(mdtId))       pat=cgetTransPtr("/Muon/Align/L23_Bar");  
  else {
    if (midh->stationEta(mdtId)>0) pat=cgetTransPtr("/Muon/Align/L23_ECA");
    else                           pat=cgetTransPtr("/Muon/Align/L23_ECC");
  }    
  if (!pat){ 
    ATH_MSG_WARNING("no pat");
    return Amg::Transform3D();
  }

  ATH_MSG_WARNING("get module Id");
  Identifier moduleId = getL23ModuleId(mdtId);
  if(!moduleId.is_valid()){
    ATH_MSG_WARNING("L23 ModuleId is not valid, return IdentityTransform");
    return Amg::Transform3D();
  }
    
  AlignableTransform::AlignTransMem_citr itr=pat->findIdent(moduleId);
  if(itr==pat->end()){
    ATH_MSG_WARNING("couldn't find L23 transform, return IdentityTransform");
  }

  return ( itr!=pat->end() ? Amg::CLHEPTransformToEigen(itr->transform()) : Amg::Transform3D() );
}

//________________________________________________________________________
const Amg::Transform3D 
MultiLevelAlignmentDbTool::level3Transform(Identifier mdtId) const
{
  if (!m_doL3) return Amg::Transform3D();

  const AlignableTransform* pat;    

  const MuonIdHelper* midh = muonIdHelper(mdtId);
  if (!midh) return Amg::Transform3D();

  /*
  if (midh->isBarrel(mdtId))       pat=cgetTransPtr("/Muon/Align/L3_Bar");  
  else {
    if (midh->stationEta(mdtId)>0) pat=cgetTransPtr("/Muon/Align/L3_ECA");
    else                           pat=cgetTransPtr("/Muon/Align/L3_ECC");
  }    
  */
  pat=cgetTransPtr("/Muon/Align/L3");  

  if (!pat) return Amg::Transform3D();
  
  AlignableTransform::AlignTransMem_citr itr=pat->findIdent(mdtId);
  return ( itr!=pat->end() ? Amg::CLHEPTransformToEigen(itr->transform()) : Amg::Transform3D() );
}

//________________________________________________________________________
const Amg::Transform3D 
MultiLevelAlignmentDbTool::alignToGlobalTransform(Identifier id) const
{

  if (!m_doL0 && !m_doL1 && !m_doL23) return Amg::Transform3D();

   ATH_MSG_DEBUG("in alignToGlobalTransform"); 

  const MuonIdHelper* midh = muonIdHelper(id);
  if (!midh) return Amg::Transform3D();

  const MuonGM::MuonReadoutElement* mre(0);
  if (midh->is_mdt(id)) {
    const MuonGM::MdtReadoutElement* mdt=p_muonMgr->getMdtReadoutElement(id);
    if (mdt) {
      ATH_MSG_DEBUG("first tube position (" << mdt->tubePos(mdt->getMultilayer(), 1, 1) <<")");
      ATH_MSG_DEBUG("for chamber "<<mdt->identify() << ", ml: " << mdt->getMultilayer());
      mre=dynamic_cast<const MuonGM::MuonReadoutElement*>(mdt);
    }
  }
  else if (midh->is_tgc(id)) 
    mre=dynamic_cast<const MuonGM::MuonReadoutElement*>(p_muonMgr->getTgcReadoutElement(id));
  
  else if (midh->is_csc(id)) {
    
    // if CSC, id is for a CscDetectorElement.  Have to get 
    // CscReadoutElement for this CscDetectorElement    
    const MuonGM::CscDetectorElement* cde(0);
    for (int idHash=0; idHash<MuonGM::MuonDetectorManager::CscDetElMaxHash; idHash++) {
      const MuonGM::CscDetectorElement* de=p_muonMgr->getCscDetectorElement(idHash);
      if (de->identify()==id) {
	cde=de;
	break;
      }
    }
    if (!cde) { 
      ATH_MSG_WARNING("no CscDetectorElement!");
      return Amg::Transform3D();
    }
    if (cde->nReadoutElements()>1) 
      ATH_MSG_WARNING("more than one readout element!  Only setting one readout element.");
    
    mre = cde->readoutElement();
  }
  
  if (mre) 
    return Amg::CLHEPTransformToEigen(mre->parentMuonStation()->getAmdbLRSToGlobal()); //* tsz_to_szt;
  
  ATH_MSG_WARNING("no muon readout element! return empty transform.");
  return Amg::Transform3D();
}

//________________________________________________________________________
const Amg::Transform3D 
MultiLevelAlignmentDbTool::globalToL23ModuleTransform(Identifier id) const
{

  if (!m_doL23) return Amg::Transform3D();

  ATH_MSG_DEBUG("in globalToL23ModuleTransform"); 

  Identifier moduleId = getL23ModuleId(id);
  if(!moduleId.is_valid()){
    ATH_MSG_WARNING("L23 ModuleId is not valid, return IdentityTransform");
    return Amg::Transform3D();
  }
  ATH_MSG_DEBUG("ChamberID: " << id.getString() << ", L23ModuleID: " << moduleId.getString());
  return alignToGlobalTransform(moduleId).inverse();
 

  // const MuonIdHelper* midh = muonIdHelper(id);
//   ATH_MSG_DEBUG("1");
//   if (!midh) return Amg::Transform3D();
//   ATH_MSG_DEBUG("2");

//   if (!midh->is_mdt(id)){
//     ATH_MSG_WARNING("globalToModuleTransform only implemented for MDTs");
//     return Amg::Transform3D();
//   }
//   const MuonGM::MdtReadoutElement* mdt=p_muonMgr->getMdtReadoutElement(id);
//   if (!mdt){
//     ATH_MSG_WARNING("no ReadoutElement");
//     return Amg::Transform3D();
//   }

//   ATH_MSG_DEBUG("3");
//   std::string chambername=midh->stationNameString(id);
//   ATH_MSG_DEBUG("4");
//   std::string firstChamberName = chambername.substr(2,1)=="L" ? "BIL" : chambername;
//   ATH_MSG_DEBUG("5");
//   int stEta=midh->stationEta(id);
//   stEta/=abs(stEta);
//   int stPhi=midh->stationPhi(id);
 
//   ATH_MSG_DEBUG("chamber: " << chambername 
//                 << ", firstChamber: " <<  firstChamberName
//                 << ", eta: " << midh->stationEta(id)
//                 << ", firstEta: " << stEta << ", firstPhi: " << stPhi);
//   bool isValid(true);
//   Identifier firstChamberId = m_mdtIdHelper->elementID(firstChamberName,stEta,stPhi,true,&isValid);
  //ATH_MSG_DEBUG("identifier: " << firstChamberId.getString());

  //return alignToGlobalTransform(firstChamberId).inverse();

}

//________________________________________________________________________
void MultiLevelAlignmentDbTool::decomposeTransform(const Amg::Transform3D& transform, double* values) 
{ 
  // copied from AlignModule 
  Amg::RotationMatrix3D rotation = transform.rotation(); 
  Amg::Vector3D         translation = transform.translation(); 
  
  values[0]=translation[0];
  values[1]=translation[1];
  values[2]=translation[2];
  
  
  // a12 = cosbeta singamma     
  // a13 = -sinbeta 
  // a23 =  sinalpha cosbeta 
  
  double sinbeta = -rotation(0,2); 
  double cosbeta = std::sqrt(1.-sinbeta*sinbeta); 
  
  double sinalpha = rotation(1,2)/cosbeta;     
  double singamma = rotation(0,1)/cosbeta; 
  
  values[3]=-std::asin(sinalpha); 
  values[4]=-std::asin(sinbeta); 
  values[5]=-std::asin(singamma); 
  
} 

//________________________________________________________________________
const MuonIdHelper* MultiLevelAlignmentDbTool::muonIdHelper(Identifier ID) const
{ 
  
  //determine technology
  if (m_mdtIdHelper->is_mdt(ID))
    return m_mdtIdHelper;
  
  else if (m_cscIdHelper->is_csc(ID))
    return m_cscIdHelper;
  
  else if (m_rpcIdHelper->is_rpc(ID))
    return m_rpcIdHelper;
  
  else if (m_tgcIdHelper->is_tgc(ID))
    return m_tgcIdHelper;
  
  
  ATH_MSG_ERROR("Failure to determine technology type of ID#, returning false");
  return 0;

}
