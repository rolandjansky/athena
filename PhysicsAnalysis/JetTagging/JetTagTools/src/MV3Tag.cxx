/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/MV3Tag.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "GaudiKernel/IToolSvc.h"
//#include "JetEvent/Jet.h"
#include "JetTagInfo/IPInfoBase.h"
#include "JetTagInfo/SVInfoBase.h"
#include "JetTagInfo/SVInfoPlus.h"
#include "JetTagInfo/JetFitterTagInfo.h"
#include "JetTagInfo/IJetFitterTagInfo.h"
#include "JetTagEvent/ISvxAssociation.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxJetVertex/VxJetCandidate.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"
#include "VxJetVertex/TwoTrackVerticesInJet.h"
#include "JetTagCalibration/CalibrationBroker.h"
#include "TMVA/Reader.h"
#include "TList.h"
#include "TString.h"
#include "TObjString.h"
#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>
#include <map>
#include <list>

namespace Analysis {

  /** 
      @class MV3Tag 
      BDT-based tagger combining 27 low-level b-tagging variables
      @author M. Ughetto
  */

  static const unsigned int njMV3ptbin = 11;
  static const double _jMV3ptbins[njMV3ptbin] = { 20., 30., 40., 50., 60., 75., 90., 110., 140., 200., 500. };/* in GeV */
  std::vector<double> _vMV3ptbins(_jMV3ptbins,_jMV3ptbins+njMV3ptbin);
  
  MV3Tag::MV3Tag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_calibrationTool("BTagCalibrationBroker"),
      m_runModus("analysis") {
    declareInterface<ITagTool>(this);
    // access to XML configuration files for TMVA from COOL:
    declareProperty("calibrationTool", m_calibrationTool);
    // force MV3 to always use a calibration derived from MV3CalibAlias jet collection
    declareProperty("forceMV3CalibrationAlias", m_forceMV3CalibrationAlias = false);
    declareProperty("MV3CalibAlias", m_MV3CalibAlias = "AntiKt4TopoEM");
    // MV3 flavor, must match with the one in calibration file. Can be: bVSu, bVSc, cVSu
    declareProperty("MV3Flavor", m_MV3Flavor = "bVSu" );
    // global configuration:
    declareProperty("Runmodus", m_runModus);
    // information to persistify:
    declareProperty("writeInfoBase", m_writeInfoBase = true);
    // to change input weights:
    declareProperty("inputSV0WeightName", m_inputSV0WeightName = "SV0");
    // to use the flipped version
    declareProperty("flipMV3", m_flipMV3 = false);
  }


  MV3Tag::~MV3Tag() {
  }


  StatusCode MV3Tag::initialize() {
    // define tagger name:
    // (remove ToolSvc. in front and Tag inside name):
    std::string iname( this->name().substr(8) );
    std::string instanceName(iname);
    std::string::size_type pos = iname.find("Tag");
    if ( pos!=std::string::npos ) {
      std::string prefix = iname.substr(0,pos);
      std::string posfix = iname.substr(pos+3);
      instanceName = prefix;
      instanceName += posfix;
    }
    m_taggerName = instanceName;
    // remove also Flip:
    std::string instanceName2(instanceName);
    pos = instanceName.find("Flip");
    if ( pos!=std::string::npos ) {
      std::string prefix = instanceName.substr(0,pos);
      std::string posfix = instanceName.substr(pos+4);
      instanceName2 = prefix;
      instanceName2 += posfix;
    }
    m_taggerNameBase = instanceName2;
    // prepare calibration tool:
    StatusCode sc = m_calibrationTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_calibrationTool);
      return sc;
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_calibrationTool);
    }
    
    for (unsigned int iptbin=1; iptbin<njMV3ptbin; iptbin++){
      TString calibname("bin"); calibname+=(iptbin);calibname+="/";
      calibname+=m_taggerNameBase;calibname+="Calib";
      std::string calibstring(calibname.Data());
      
      m_calibrationTool->registerHistogram("MV3"// m_taggerNameBase
					   , calibstring);
    }
    for (unsigned int iptbin=0; iptbin<njMV3ptbin; iptbin++){
      //      m_tmvaReaders[iptbin].clear();
    }
    return StatusCode::SUCCESS;
  }


  StatusCode MV3Tag::finalize() {
    // delete readers:
    for (unsigned int iptbin=0; iptbin<njMV3ptbin-1; iptbin++){
      std::map<std::string, TMVA::Reader*>::iterator pos = m_tmvaReaders[iptbin].begin();
      for( ; pos != m_tmvaReaders[iptbin].end(); ++pos ) delete pos->second;
    }
    return StatusCode::SUCCESS;
  }

  void MV3Tag::tagJet(xAOD::Jet& myJet) {  
    /* default D3PD values*/
    m_jf_nvtx   = -1;
    m_jf_nvtx1t = -1;
    m_jf_ntrkv  = -1;
    m_jf_efrc   = -999;
    m_jf_mass   = -999;
    m_jf_sig3   = -999;
    m_jf_dphi   = -999;
    m_jf_deta   = -999;

    m_sv0mass    = -1;
    m_sv0_efrc   = -1;
    m_sv0_n2t    = -1;
    m_sv0_ntkv   = -1;
    m_sv0_radius = 0.;


    /* jet author: */
    std::string author ;
    if (m_forceMV3CalibrationAlias) {
      author = m_MV3CalibAlias;
    }
    else {
      author = myJet.jetAuthor();
    }

    std::string alias = m_calibrationTool->channelAlias(author);
    /* creating njptbin xml files and TMVA readers */ 
    TString xmlFileNames[10];
    TMVA::Reader* tmvaReaders[10];
    std::map<std::string, TMVA::Reader*>::iterator pos[10];

    for (unsigned int iptbin=0; iptbin<njMV3ptbin-1; iptbin++){
      unsigned int ptbin = iptbin+1;
      
      xmlFileNames[iptbin]+="btag"; xmlFileNames[iptbin]+=m_taggerNameBase;
      xmlFileNames[iptbin]+="ptbin"; xmlFileNames[iptbin]+=ptbin; xmlFileNames[iptbin]+="Config_";
      xmlFileNames[iptbin]+=alias; xmlFileNames[iptbin]+=".xml";

      ATH_MSG_DEBUG("#BTAG# Jet author for MV3: " << author << ", alias: " << alias );

      /* tweak name for Flip versions: */

      if (!m_flipMV3) {
	m_ip2d_infosource  = "IP2D";
	m_ip3d_infosource  = "IP3D";
	m_sv1_infosource   = "SV1";
	m_sv0p_infosource  = "SV0InfoPlus";
	m_jftNN_infosource = "JetFitterTagNN";
	m_jfcNN_infosource = "JetFitterCOMBNN";
      }
      else {
	m_ip2d_infosource  = "IP2DNeg";
	m_ip3d_infosource  = "IP3DNeg";
	m_sv1_infosource   = "SV1Flip";
	m_sv0p_infosource  = "SV0InfoPlus";
	m_jftNN_infosource = "JetFitterTagNNFlip";
	m_jfcNN_infosource = "JetFitterCOMBNNIP3DNeg";
      }
      /* check if calibration (neural net structure or weights) has to be updated: */
      TString calibname("bin"); calibname+=(iptbin+1);calibname+="/";
      calibname+=m_taggerNameBase;calibname+="Calib";
      std::string calibstring(calibname.Data());

      std::pair<TList*, bool> calib = m_calibrationTool->retrieveTObject<TList>("MV3",//m_taggerNameBase,
										author,calibstring);
      bool calibHasChanged = calib.second;
      if(calibHasChanged) {
	ATH_MSG_DEBUG("#BTAG# " << m_taggerNameBase << " calib updated -> try to retrieve");
	if(!calib.first) {
	  ATH_MSG_WARNING("#BTAG# Tlist can't be retrieved -> no calibration for MV3");
	  return;
	}
	//m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib", false);
	m_calibrationTool->updateHistogramStatus("MV3",// m_taggerNameBase,
						 alias, calibstring, false);

	// now the ugly part: write an xml text file to be read by TMVAReader:
	TList* list = calib.first;
	std::ofstream ofile( xmlFileNames[iptbin] );
	if(!ofile) {
	  ATH_MSG_WARNING("#BTAG# Unable to create output file " << xmlFileNames[iptbin] );
	  return;
	}
	for(int i=0; i<list->GetSize(); ++i) {
	  TObjString* ss = (TObjString*)list->At(i);
	  ofile << ss->String() << std::endl;
	}
	ofile.close();
	ATH_MSG_DEBUG("#BTAG# XML file created: " << xmlFileNames[iptbin] );

      }
    

      // now configure the TMVAReaders:
      /// check if the reader for this tagger needs update

      //      if(!m_calibrationTool->updatedTagger(m_taggerNameBase, alias, m_taggerNameBase+"Calib", name()) ){
      if(!m_calibrationTool->updatedTagger("MV3",// m_taggerNameBase, 
					   alias, calibstring, name()) ){
      
	std::ifstream ifile(xmlFileNames[iptbin]);
	if(ifile){
	
	  tmvaReaders[iptbin] = new TMVA::Reader();

	  tmvaReaders[iptbin]->AddVariable("ip2_pu",&m_ip2_pu);
	  tmvaReaders[iptbin]->AddVariable("ip2_pb",&m_ip2_pb);
	  // IP3D posteriors
	  tmvaReaders[iptbin]->AddVariable("ip3_pu",&m_ip3_pu);
	  tmvaReaders[iptbin]->AddVariable("ip3_pb",&m_ip3_pb);
	  tmvaReaders[iptbin]->AddVariable("ip3_pc",&m_ip3_pc);
	  //SV1 posteriors
	  tmvaReaders[iptbin]->AddVariable("sv1_pu",&m_sv1_pu);
	  tmvaReaders[iptbin]->AddVariable("sv1_pb",&m_sv1_pb);
	  tmvaReaders[iptbin]->AddVariable("sv1_pc",&m_sv1_pc);
	  //JetFitterCombNN posteriors
	  tmvaReaders[iptbin]->AddVariable("jfc_pu",&m_jfc_pu);
	  tmvaReaders[iptbin]->AddVariable("jfc_pb",&m_jfc_pb);
	  tmvaReaders[iptbin]->AddVariable("jfc_pc",&m_jfc_pc);
	  //SV0 informations
	  tmvaReaders[iptbin]->AddVariable("sv0",&m_sv0);
	  tmvaReaders[iptbin]->AddVariable("sv0_ntkv",&m_sv0_ntkv);
	  tmvaReaders[iptbin]->AddVariable("sv0mass",&m_sv0mass);
	  tmvaReaders[iptbin]->AddVariable("sv0_efrc",&m_sv0_efrc);
	  tmvaReaders[iptbin]->AddVariable("sv0_n2t",&m_sv0_n2t);
	  tmvaReaders[iptbin]->AddVariable("sqrt(pow(sv0_x,2)+pow(sv0_y,2))",&m_sv0_radius);
	  //JetFitter informations
	  tmvaReaders[iptbin]->AddVariable("jf_mass",&m_jf_mass);
	  tmvaReaders[iptbin]->AddVariable("jf_efrc",&m_jf_efrc);
	  tmvaReaders[iptbin]->AddVariable("jf_n2tv",&m_jf_n2tv);
	  tmvaReaders[iptbin]->AddVariable("jf_ntrkv",&m_jf_ntrkv);
	  tmvaReaders[iptbin]->AddVariable("jf_nvtx",&m_jf_nvtx);
	  tmvaReaders[iptbin]->AddVariable("jf_nvtx1t",&m_jf_nvtx1t);
	  tmvaReaders[iptbin]->AddVariable("jf_dphi",&m_jf_dphi);
	  tmvaReaders[iptbin]->AddVariable("jf_deta",&m_jf_deta);
	  tmvaReaders[iptbin]->AddVariable("jfitvx_chi2/jfitvx_ndof",&m_chi2Ondof);
	  tmvaReaders[iptbin]->AddVariable("jf_sig3",&m_jf_sig3);
	  
	  TString bdtname("BDT_ptbin"); bdtname+=ptbin;

	  tmvaReaders[iptbin]->BookMVA(bdtname, xmlFileNames[iptbin]);
	  ATH_MSG_DEBUG("#BTAG# new TMVA reader created from configuration " << xmlFileNames[iptbin] );
	  // add it or overwrite it in the map of readers:
	  pos[iptbin] = m_tmvaReaders[iptbin].find(alias);
	  if(pos[iptbin]!=m_tmvaReaders[iptbin].end()) {
	    delete pos[iptbin]->second;
	    m_tmvaReaders[iptbin].erase(pos[iptbin]);
	  }
	  m_tmvaReaders[iptbin].insert( std::make_pair( alias, tmvaReaders[iptbin] ) );

	  //	  m_calibrationTool->updateHistogramStatusPerTagger(m_taggerNameBase, 
	  //						    alias, m_taggerNameBase+"Calib", false, name());
	  m_calibrationTool->updateHistogramStatusPerTagger("MV3",// m_taggerNameBase, 
							    alias, calibstring, false, name());
	}
      }
    } /* end of pT bins loop */

    /* retrieving MV3 inputs*/

    std::vector<const JetTagInfoBase*> infoVector = myJet.jetTagInfoVector();

    /* retrieving pT to select the good BDT forest*/
    double jpt = myJet.pt();
    
    /* retrieving weights: */
    m_sv0 = myJet.getFlavourTagWeight(m_inputSV0WeightName);

    /* retrieving tagger posteriors */

    for(uint iInfo = 0; iInfo < infoVector.size(); iInfo++) {      
      if(0==infoVector[iInfo]) continue;
      ATH_MSG_DEBUG("#BTAG# Info. type: " << infoVector[iInfo]->infoType());

      bool is2D = (infoVector[iInfo]->infoType()==m_ip2d_infosource);
      bool is3D = (infoVector[iInfo]->infoType()==m_ip3d_infosource);
      bool isS1 = (infoVector[iInfo]->infoType()==m_sv1_infosource);
      bool isSV0Plus = (infoVector[iInfo]->infoType()==m_sv0p_infosource);
      bool isJFit = (infoVector[iInfo]->infoType()==m_jftNN_infosource);
      bool isJFC = (infoVector[iInfo]->infoType()==m_jfcNN_infosource);
      if(is2D) { // impact parameter 2D
	// basic additional information:
	const Analysis::IPInfoBase* infob = dynamic_cast<const Analysis::IPInfoBase*>(infoVector[iInfo]);
	if(infob) {
	  m_ip2_pb = infob->tagLikelihood()[0];   // b likelihood
	  m_ip2_pu = infob->tagLikelihood()[1];   // u likelihood
	}
	else{
	  ATH_MSG_WARNING("#BTAG# MV3: IP2D info. not found" );
	}
      }
      if(is3D) { // impact parameter 3D
	// basic additional information:
	const Analysis::IPInfoBase* infob = dynamic_cast<const Analysis::IPInfoBase*>(infoVector[iInfo]);
	if(infob) {
	  m_ip3_pb = infob->tagLikelihood()[0];   // b likelihood
	  m_ip3_pu = infob->tagLikelihood()[1];   // u likelihood
	  m_ip3_pc = infob->tagLikelihood()[2];   // c likelihood
	}
	else{
	  ATH_MSG_WARNING("#BTAG# MV3: IP3D info. not found" );
	}
      }
      if(isS1) {
	// basic additional information:
	const Analysis::SVInfoBase* infob = dynamic_cast<const Analysis::SVInfoBase*>(infoVector[iInfo]);
	if(infob) {
	  m_sv1_pb = infob->tagLikelihood()[0];   // b likelihood
	  m_sv1_pu = infob->tagLikelihood()[1];   // u likelihood
	  m_sv1_pc = infob->tagLikelihood()[2];   // c likelihood
	}
	else{
	  ATH_MSG_WARNING("#BTAG# MV3: SV1 info. not found" );
	}
      }
      if(isJFC) {
      	// basic additional information:
      	const Analysis::BaseTagInfo* infob = dynamic_cast<const Analysis::BaseTagInfo*>(infoVector[iInfo]);
      	if(infob) {
	  m_jfc_pb = infob->tagLikelihood()[0];   // b likelihood
      	  m_jfc_pu = infob->tagLikelihood()[1];   // u likelihood
      	  m_jfc_pc = infob->tagLikelihood()[2];   // c likelihood
      	}
	else{
	  ATH_MSG_WARNING("#BTAG# MV3: JetFitterCOMBNN info. not found" );
	}
      }
      if(isSV0Plus) {
	const Analysis::SVInfoPlus* infob = dynamic_cast<const Analysis::SVInfoPlus*>(infoVector[iInfo]);
	if(infob) {
	  m_sv0mass = infob->getMass();            // mass of secondary vertex
	  m_sv0_n2t = infob->getN2T();                 // number of two-track vertices
	  m_sv0_ntkv= infob->getNGTrackInSvx();      // number of good tracks in vertex
	  m_sv0_efrc = infob->getEnergyFraction();  // energy fraction svx/jet        
	  
	  const Trk::RecVertex& sv = infob->getRecSvx();
	  double x = sv.position().x();
	  double y = sv.position().y();
	  m_sv0_radius = sqrt(pow(x,2)+pow(y,2));
	}
	else{
	  ATH_MSG_WARNING("#BTAG# MV3: SVInfoPlus for SV0 not found" );
	}

      }
      if(isJFit) {
      	const Analysis::JetFitterTagInfo* infob = dynamic_cast<const Analysis::JetFitterTagInfo*>(infoVector[iInfo]);
      	if(infob) {
	  m_jf_nvtx = infob->nVTX();
	  m_jf_nvtx1t = infob->nSingleTracks();
	  m_jf_ntrkv = infob->nTracksAtVtx();
	  m_jf_efrc = infob->energyFraction();
	  m_jf_mass = infob->mass();
	  m_jf_sig3 = infob->significance3d();
	  m_jf_dphi = infob->deltaphi();
	  m_jf_deta = infob->deltaeta();
      	}
	else{
	  ATH_MSG_WARNING("#BTAG# MV3: JetFitter info. not found, setting to dummies " );
	  
	  m_jf_nvtx   = -1;
	  m_jf_nvtx1t = -1;
	  m_jf_ntrkv  = -1;
	  m_jf_efrc   = -999;
	  m_jf_mass   = -999;
	  m_jf_sig3   = -999;
	  m_jf_dphi   = -999;
	  m_jf_deta   = -999;
	}

      }
    }

    /* Getting back some JetFitter vertex informations */
    
    const Analysis::ISvxAssociation* newJFSvxAssociation=
      myJet.getAssociation<Analysis::ISvxAssociation>("NewJetFitterVxFinder");

    const Trk::VxSecVertexInfo* myVertexInfo=newJFSvxAssociation->vertexInfo();     

    const std::vector<Trk::VxCandidate*>& myVertices=myVertexInfo->vertices();
    Trk::VxJetCandidate* myVxJetCandidate=dynamic_cast<Trk::VxJetCandidate*>(myVertices[0]);

    const Trk::RecVertexPositions& recVertexPositions=myVxJetCandidate->getRecVertexPositions();

    double chi2 = -999;
    double ndof = -999;

    chi2=recVertexPositions.fitQuality().chiSquared();
    ndof=recVertexPositions.fitQuality().numberDoF();

    if(ndof!=0){
      m_chi2Ondof = chi2/ndof;
    }
    else{
      m_chi2Ondof=0;
    }

    /* JetFitter: number of two track vertices */

    newJFSvxAssociation = myJet.getAssociation<Analysis::ISvxAssociation>("NewJetFitterVxFinder");

    myVertexInfo=newJFSvxAssociation->vertexInfo();     
    const Trk::VxJetFitterVertexInfo* myJetFitterInfo=dynamic_cast<const Trk::VxJetFitterVertexInfo*>(myVertexInfo);
    const Trk::TwoTrackVerticesInJet* twoTrackVerticesInfo=myJetFitterInfo->getTwoTrackVerticesInJet();
    const std::vector<const Trk::VxCandidate*>& twoTrackVertices=twoTrackVerticesInfo->getTwoTrackVertice();

    m_jf_n2tv = twoTrackVertices.size();

    /* End of MV3 inputs retrieving */

    ATH_MSG_DEBUG("#BTAG# MV3 inputs: " <<
		  " ip2_pu " << m_ip2_pu  <<
		  " ip2_pb " << m_ip2_pb <<
		  " ip3_pu " << m_ip3_pu  <<
		  " ip3_pb " << m_ip3_pb  <<
		  " ip3_pc " << m_ip3_pc <<
		  " sv1_pu " << m_sv1_pu  <<
		  " sv1_pb " << m_sv1_pb  <<
		  " sv1_pc " << m_sv1_pc <<
		  " jfc_pu " << m_jfc_pu  <<
		  " jfc_pb " << m_jfc_pb  <<
		  " jfc_pc " << m_jfc_pc <<
		  " sv0 "    << m_sv0  <<
		  " sv0_ntkv " << m_sv0_ntkv  <<
		  " sv0mass " << m_sv0mass  <<
		  " sv0_efrc " << m_sv0_efrc  <<
		  " sv0_n2t " << m_sv0_n2t  <<
		  " sv0_radius " << m_sv0_radius  <<
		  " jf_mass " << m_jf_mass  <<
		  " jf_efrc " << m_jf_efrc  <<
		  " jf_n2tv " << m_jf_n2tv  <<
		  " jf_ntrkv " << m_jf_ntrkv  <<
		  " jf_nvtx " << m_jf_nvtx  <<
		  " jf_nvtx1t " << m_jf_nvtx1t  <<
		  " jf_dphi " << m_jf_dphi  <<
		  " jf_deta " << m_jf_deta  <<
		  " chi2Ondof " << m_chi2Ondof <<
		  " jf_sig3 " << m_jf_sig3 <<
		  " jpt " << jpt
		  );

    /* compute MV3: */
    double mv3 = 0.;
    int binnb = -1;
    for (unsigned int iptbin=0; iptbin<njMV3ptbin; iptbin++){
      if ((jpt/1000.)<=_vMV3ptbins[0]){
	binnb=1;
	break;
      }
      else if ((jpt/1000.)>_vMV3ptbins[iptbin] && (jpt/1000.)<=_vMV3ptbins[iptbin+1]){
	binnb=iptbin+1;
	break;
      }
      else if ((jpt/1000.)>_vMV3ptbins[njMV3ptbin-1]){
	binnb = njMV3ptbin-1;
	break;
      }
    } 
    ATH_MSG_DEBUG("#BTAG# MV3 pt=" << jpt << " binnb = " << binnb);

    std::map<std::string, TMVA::Reader*>::iterator pos2 = m_tmvaReaders[binnb-1].find(alias);

    if(pos2==m_tmvaReaders[binnb-1].end()) {
      int alreadyWarned = std::count(m_undefinedReaders.begin(),m_undefinedReaders.end(),alias);
      if(0==alreadyWarned) {
        ATH_MSG_WARNING("#BTAG# no TMVAReader defined for jet collection " << alias);
        m_undefinedReaders.push_back(alias);
      }
      return;
    } else {
      TString bdttoeval("BDT_ptbin"); bdttoeval+=binnb;
      mv3 = pos2->second->EvaluateMVA(bdttoeval);
      ATH_MSG_DEBUG("#BTAG# MV3 weight: " << mv3);
    }
    
    /** give information to the info class. */
    if(m_runModus=="analysis") {
      IPInfoBase* infoBase(0);
      // -- fill new base info class ... 
      if(m_writeInfoBase) {
        ATH_MSG_VERBOSE("#BTAG# Filling IPInfoBase " << m_taggerName);
        infoBase = new IPInfoBase(m_taggerName);
        std::vector<double> like;
        like.push_back(mv3);
        if(infoBase) infoBase->setTagLikelihood(like);
      }
                                        
      /** tagging done. Fill the JetTag and return ... */
      if(infoBase) {
        myJet.addInfo(infoBase);
      }
      
    }


  }
}//end namespace



