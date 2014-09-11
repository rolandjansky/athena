/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************

   @class HadFlavorTag

   Created - May 2011 as GbbNNTag

   @author: 
   * Maria Laura Gonzalez Silva (laugs@cern.ch)
   --- ( University of BUENOS AIRES ) ---

   (c) 2011- ATLAS Detector Software

   Modified - March 2012 to include QG tagging
   * L Lee (leejr@cern.ch), Yale

   QG lookup structure by M Swiatlowski (swiatlow@slac.stanford.edu), SLAC

********************************************************/

#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <istream> 
#include <string>
#include <stdlib.h>
#include "Riostream.h"
#include "TFile.h"
#include "TMath.h"
#include "TMatrix.h"

#include "TH2.h"

#include "JetTagTools/HadFlavorTag.h"

//#include "CLHEP/Vector/LorentzVector.h"
//#include "FourMom/P4PxPyPzE.h"
//#include "JetEvent/Jet.h"
#include "GaudiKernel/MsgStream.h"

#include "JetTagTools/HistoHelperRoot.h"

#include "JetTagTools/TrackSelector.h"
#include "JetTagInfo/HadFlavorTagInfo.h"
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfo/ITagInfo.h"

#include "StoreGate/StoreGateSvc.h"
#include "GeneratorObjects/McEventCollection.h"

#include "JetTagEvent/TrackAssociation.h"
#include "xAODTracking/TrackParticle.h"

#include "PathResolver/PathResolver.h"

//fastjet
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

namespace Analysis {

  HadFlavorTag::HadFlavorTag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_trackSelectorTool("Analysis::TrackSelector"),
      m_mcEventCollection("TruthEvent")
  {
    
    declareInterface<ITagTool>(this);
    // global configuration:
    declareProperty("Runmodus",     m_runModus     = "analysis");

    declareProperty("tagMode",     m_tagMode     = "GbbNN");
 
    // tools:
    declareProperty("trackSelectorTool", m_trackSelectorTool);

    //Neural Network configuration

    //weights
    declareProperty("weightsFileName", m_weightsFileName="weightsFile.txt");


    //QG LUT configuration

    declareProperty("QGMinPt",  m_QGMinPt= 40000.);
    declareProperty("QGMaxPt",  m_QGMaxPt=400000.);
    declareProperty("QGMaxEta", m_QGMaxEta=2.5);

    declareProperty("QGConeSize", m_QGConeSize=4);

    declareProperty("QGPtBounds",    m_QGPtBounds);
    declareProperty("QGEtaBounds",   m_QGEtaBounds);
    declareProperty("QGHistBaseName",m_QGHistBaseName="akt4TopoEM_trkwidth_vs_ntrk_PT");

    //Likelihood lookup
    declareProperty("QGLikelihoodMCFileName", m_QGLikelihoodMCFileName="QGLikelihood_MC_v0001.root");
    declareProperty("QGLikelihoodDATAFileName", m_QGLikelihoodDATAFileName="QGLikelihood_DATA_v0001.root");

    //truth stuff to check if we have data or mc
    declareProperty("McEventCollection", m_mcEventCollection);

  }

  HadFlavorTag::~HadFlavorTag() {
  }


  //==============================================================
  StatusCode HadFlavorTag::initialize() {

    // creation of TrackSelector: (private instance) 
    if ( m_trackSelectorTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_trackSelectorTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("Retrieved tool " << m_trackSelectorTool);
    }
  
    
    // book histograms in reference mode 
    if (m_runModus == "reference") {
      StatusCode sc = InitReferenceMode();
      if (sc==StatusCode::FAILURE) return StatusCode::FAILURE;
    }
    
    // read in calibration histograms 
    if( m_runModus == "analysis" ) {
      StatusCode sc = InitAnalysisMode();
      if (sc==StatusCode::FAILURE) return StatusCode::FAILURE;
    }

    
    return StatusCode::SUCCESS;
  }

 
  // =============================================================
  StatusCode HadFlavorTag::InitReferenceMode() 
  {
    // =====================
    // Book histograms here
    // =====================
  
    StatusCode sc = StatusCode::SUCCESS;
  
    ATH_MSG_DEBUG("reference mode not supported for HadFlavorTag");
  
    return sc;
  }


  // ==================================================================
  StatusCode HadFlavorTag::InitAnalysisMode() 
  {

    StatusCode sc = StatusCode::SUCCESS;

    if( m_tagMode == "GbbNN" ){     // GbbNN Tagging

      // ==========================
      // Read calibration file here
      // ==========================
      fNvars=3;
      fNptbins=7;
    
      std::filebuf fb;
      std::string filename = PathResolver::find_file(m_weightsFileName, "DATAPATH");
      const char* name = filename.c_str();
      fb.open ( name,ios::in);
      std::istream is(&fb);
    
      sc = ReadWeightsFromFile(is);
      fb.close();
    
    } else if (m_tagMode == "QG") { // QG Tagging
      
      m_isMC=0;

      //try to retrieve some mc collection...
      const McEventCollection* myMcEventCollection(0);
      StatusCode sc = evtStore()->retrieve(myMcEventCollection, m_mcEventCollection);
      if (sc.isFailure()) {
	m_isMC=0;
      } else {
	m_isMC=1;
      }
      
      m_QGLikelihoodFile = 0;
      
      std::string m_tempFileName;
      if(m_isMC) m_tempFileName = m_QGLikelihoodMCFileName;
      else m_tempFileName = m_QGLikelihoodDATAFileName;

      m_QGLikelihoodFile = new TFile(PathResolver::find_file(m_tempFileName,"DATAPATH").c_str() , "READ") ;

      if( m_QGLikelihoodFile == 0 ){
	ATH_MSG_FATAL("Could not retrieve histograms for " << m_tempFileName);
	return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG( "Got histogram file for QG Likelihood: " << m_tempFileName);

    } else {                        // Don't know what kind of tagging to do

      sc = 0;

    }
  
    return sc;

  }


  //==========================================================
 
  void HadFlavorTag::finalizeHistos() {
  }
  

  //=============================================================
  StatusCode HadFlavorTag::finalize() {

    if( m_tagMode == "GbbNN" ){

      // nothing to clear
      
      return StatusCode::SUCCESS;

    } else if ( m_tagMode == "QG" ) {

      delete m_QGLikelihoodFile;

      return StatusCode::SUCCESS;
    } 
    
    return StatusCode::SUCCESS;

  }



  //============================================================
  void HadFlavorTag::tagJet(xAOD::Jet& jetToTag) {


    if (m_runModus == "analysis") 
      JetTagAnalysisMode(jetToTag);
    
    // run the part of the tagging algorithm done only in reference mode
    if (m_runModus == "reference") 
      JetTagReferenceMode(jetToTag);
  

    return;
    
  }
 
   

  // ======================================================
  void HadFlavorTag::JetTagAnalysisMode(xAOD::Jet& jetToTag)
  {
    // ===============================================================
    // part of the jet tagging algorithm done only in analysis mode
    // ===============================================================
  

    // initialize weight and set input vars default values
    double weight = -1001.;
    int ntrk = -1001;
    double trkwidth =-1001;
     double drktaxes = -1001;

    // Create the Info 
    std::string iname( this->name().substr(8) );
    std::string instanceName(iname);
    std::string::size_type pos = iname.find("Tag");
    if ( pos!=std::string::npos ) {
      std::string prefix = iname.substr(0,pos);
      std::string posfix = iname.substr(pos+3);
      instanceName = prefix;
      instanceName += posfix;
    }

    HadFlavorTagInfo* m_HadFlavorInfo = new HadFlavorTagInfo(instanceName);
    jetToTag.addInfo(m_HadFlavorInfo);

    if ( m_tagMode == "QG" ){
      if (jetToTag.jetAuthor() != "AntiKt4TopoEM" && jetToTag.jetAuthor() != "AntiKt6TopoEM") return;
    }else if ( m_tagMode == "GbbNN" ){
      if (jetToTag.jetAuthor() != "AntiKt4TopoEM") return;
    }

    // the jet
    xAOD::IParticle::FourMom_t jetDirection = jetToTag.p4();

    // retrieve associated tracks and do selection
    const Analysis::TrackAssociation* ta = jetToTag.getAssociation<Analysis::TrackAssociation>("Tracks");
    std::vector<const xAOD::TrackParticle*> trklist;  trklist.clear();
     
    if(ta) {
      ntrk=0;
      std::vector<const xAOD::TrackParticle*>* trackVector = ta->tracks();
      for(std::vector<const xAOD::TrackParticle*>::iterator trkItr = trackVector->begin(); 
	  trkItr !=trackVector->end(); ++trkItr) {
	 
	const xAOD::TrackParticle* aTemp = *trkItr;
	if(m_preselect(aTemp)){
	  ntrk++;
	  trklist.push_back(aTemp);
	}
      }	 
      delete trackVector;
    }
     
    if(ta && ntrk>1 ){
	 
      trkwidth = m_trkwidth(trklist,jetDirection);
      drktaxes = m_drktaxes(trklist);

      if(trkwidth > 0 && drktaxes >0){

	// vector of input vars
	vector<double> vars;

	if( m_tagMode == "GbbNN" ){

	  vars.push_back(drktaxes);
	  vars.push_back(ntrk);
	  vars.push_back(trkwidth);
	
	  //get neural net output value
	  int ibin= -1;
	  for(int ptbin = 0;ptbin<fNptbins;ptbin++){
	    if((jetToTag.pt()/1000)>=pt1[ptbin]&&(jetToTag.pt()/1000)<pt2[ptbin])ibin=ptbin;
	  }
	 
	  
	  if(ibin>=0){
	    bool checkVarRange = true;
	    for (unsigned int ivar=0; ivar<fNvars; ivar++) {
	      if(vars[ivar]<fVmin[ibin][ivar] || vars[ivar] > fVmax[ibin][ivar]){
		checkVarRange = false;
		ATH_MSG_DEBUG("In HadFlavorTag::JetTagAnalysisMode: GbbNN var["<<ivar << "]="<<vars[ivar] << " falls outside range [" <<fVmin[ibin][ivar]<< ","<<  fVmax[ibin][ivar] << "], setting  weight to default value"  );     
	      }
	    }	 
	    if(checkVarRange){
	      weight = GetMvaoutput(vars,ibin);
	      ATH_MSG_DEBUG("In HadFlavorTag::JetTagAnalysisMode: ibin =  "<<ibin << " Jet PT "<< jetToTag.pt()/1000 << " ntrk " << ntrk << " trkwidth "<< trkwidth << " drktaxes " << drktaxes << " NewGbbNN weight " << weight );
	    }
	  }
	  
	  
	} else if ( m_tagMode == "QG" ){

	  size_t found;

	  found = jetToTag.jetAuthor().find("4");
	  if( found!=string::npos ){
	    found = m_QGHistBaseName.find_first_of("46");
	    if( found!=string::npos ){
	      m_QGHistBaseName.replace(found,1,"4");
	    }
	  }
	  
	  found = jetToTag.jetAuthor().find("6");
	  if( found!=string::npos ){
	    found = m_QGHistBaseName.find_first_of("46");
	    if( found!=string::npos ){
	      m_QGHistBaseName.replace(found,1,"6");
	    }
	  }
	  
	  //get jet cone

	  vars.push_back(m_QGConeSize); // cone size
	  vars.push_back(jetToTag.pt()); // this should be @ calib state
	  vars.push_back(jetToTag.eta()); // this should be @ calib state
	  vars.push_back(ntrk);
	  vars.push_back(trkwidth);
	
	  //get likelihood output value
	  weight = GetQGLikelihood(vars);

	} else {

	  weight = -1.;

	}
      }  
    }



    std::vector<double> tmpll;
    tmpll.push_back(weight);
    // Store NN input vars and output as weight   
    m_HadFlavorInfo->setTagLikelihood(tmpll);
    m_HadFlavorInfo->setWeight(weight); /// you can keep it.
    m_HadFlavorInfo->setnMatchingTracks(ntrk);
    m_HadFlavorInfo->settrkJetWidth(trkwidth);
    m_HadFlavorInfo->settrkJetDRKtaxes(drktaxes);
     
    m_HadFlavorInfo->makeValid();

  }


  //================================================================
  double HadFlavorTag::m_trkwidth(std::vector<const xAOD::TrackParticle*> tracks, xAOD::IParticle::FourMom_t j){
  
    double trkWIDTH = -1;
    double trkWIDTH_num = 0;
    double trkWIDTH_den = 0;
    for(std::vector<const xAOD::TrackParticle*>::iterator trkItr = tracks.begin(); trkItr !=tracks.end(); ++trkItr) {
    
      const xAOD::TrackParticle* aTemp = *trkItr; // maybe without the '*'
      double dR =j.DeltaR(aTemp->p4());
    
      trkWIDTH_num+=aTemp->pt()*dR;
      trkWIDTH_den+=aTemp->pt(); 
    
    }
  
    if(trkWIDTH_den>0)trkWIDTH = trkWIDTH_num/trkWIDTH_den;
    return trkWIDTH;
  }
  


//================================================================
double HadFlavorTag::m_drktaxes(std::vector<const xAOD::TrackParticle*> tracks){
 double DRKtAxes = -1;

  vector<fastjet::PseudoJet> seed2axes;
  const int n_jets = 2;
  vector <fastjet::PseudoJet>  inputTracks;

  //get inputTracks out of TrackParticles
  for(std::vector<const xAOD::TrackParticle*>::iterator trkItr = tracks.begin(); trkItr !=tracks.end(); ++trkItr) {
    
    const xAOD::TrackParticle* aTemp = *trkItr;
    const xAOD::IParticle::FourMom_t& hlv = aTemp->p4();
    double Energy  =hlv.E();
    double Px  =hlv.Px();
    double Py  =hlv.Py();
    double Pz  =hlv.Pz();
    
    inputTracks.push_back(fastjet::PseudoJet(Px,Py,Pz,Energy/1000));

  }

  //cluster sequence
  fastjet::JetDefinition jet_def = fastjet::JetDefinition(fastjet::kt_algorithm,3.14159265/2,fastjet::E_scheme,fastjet::Best);
  fastjet::ClusterSequence jet_clust_seq(inputTracks, jet_def);
  seed2axes = jet_clust_seq.exclusive_jets(n_jets);

  if(inputTracks.size()>=2)DRKtAxes = sqrt(seed2axes[0].squared_distance(seed2axes[1]));
  

  return DRKtAxes;
  
  
}



  // ======================================================
  void HadFlavorTag::JetTagReferenceMode(xAOD::Jet& /*jetToTag*/)
  {

    return;
    /*********************************************************************

    Not filling reference histos for now (not being run in reference mode

    *********************************************************************/ 

  

  }




  //==========================================================
  bool HadFlavorTag::m_preselect(const xAOD::TrackParticle *t)
  {
 
    ATH_MSG_VERBOSE("m_preselect(): trkPrt->pt(): " << t->pt());
  
    m_trackSelectorTool->primaryVertex(m_priVtx->recVertex().position());
    m_trackSelectorTool->prepare();
  
    //define track selection in joboptions when specifying the trackselectortool

    if( m_trackSelectorTool->selectTrack(t) ) return true;
    else return false;
  }


  void HadFlavorTag::m_printParameterSettings()
  {
    ATH_MSG_INFO( "#HADFLAVORTAG# " << name() << "Parameter settings ");
    ATH_MSG_INFO( "#HADFLAVORTAG# I am in " << m_runModus << " modus.");
  }


  /* ------------------------------------------------------------------- */
  /*                        Private Functions                     */
  /* ------------------------------------------------------------------- */


//===============================================================
StatusCode HadFlavorTag::ReadWeightsFromFile( istream& istr )
{
   
   TString var;

    // read number of variables and classes
   int nptbins(0);
   double fEpsilon(0);
   size_t nva(0);
   istr >> nptbins >> nva >> fEpsilon;

   ATH_MSG_DEBUG( " In HadFlavorTag::ReadWeightsFromFile(): numer of variables " << nva);  
   
   if (fNvars != nva){
     ATH_MSG_ERROR( "In HadFlavorTag::ReadWeightsFromFile(): mismatch in number of variables" );
     return StatusCode::FAILURE;
   }
   
   if (fNptbins != nptbins){
     ATH_MSG_ERROR( "In HadFlavorTag::ReadWeightsFromFile(): mismatch in number of pt bins" );
     return StatusCode::FAILURE;
   }

   
   // check that we are not at the end of the file
   if (istr.eof( )){
     ATH_MSG_ERROR("In HadFlavorTag::ReadWeightsFromFile(): ERROR -> reached EOF prematurely ");
     return StatusCode::FAILURE;
   }
   
    
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     // fType[ivar]='';
     fNbin[ivar]=0;     
     fHasDiscretePDF[ivar]=false;
     istr >> fType[ivar] >> fNbin[ivar] >> boolalpha >>fHasDiscretePDF[ivar];
   }

   for (int i=0; i<fNptbins; i++){ pt1[i]=0; istr >> pt1[i];}
   for (int j=0; j<fNptbins; j++){ pt2[j]=0; istr >> pt2[j];}
   

   //================================
   //read values for different pt bins
   //================================
   
   // to read dummy lines
   const int nchar(100);
   char* dumchar = new char[nchar];
   
   // read max & min of input variables
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int ipt=0; ipt<fNptbins; ipt++) {
       fVmin[ipt][ivar]=0;
       istr >> fVmin[ipt][ivar];
       ATH_MSG_DEBUG("In HadFlavorTag::ReadWeightsFromFile(): var "<< ivar << " xmin " << fVmin[ipt][ivar] );
     }
   }
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int ipt=0; ipt<fNptbins; ipt++) {
       fVmax[ipt][ivar]=0;
       istr >> fVmax[ipt][ivar];
       ATH_MSG_DEBUG("In HadFlavorTag::ReadWeightsFromFile(): var "<< ivar << " xmax " << fVmax[ipt][ivar] );
     }
   }        
   
   
   
   // read HistMin & HistMax
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int ipt=0; ipt<fNptbins; ipt++) {
       fHistMin[ipt][ivar]=0;
       istr >> fHistMin[ipt][ivar];
       ATH_MSG_DEBUG("In HadFlavorTag::ReadWeightsFromFile(): var "<< ivar << " HistMin " << fHistMin[ipt][ivar] );
     }
   }
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int ipt=0; ipt<fNptbins; ipt++) {
       fHistMax[ipt][ivar]=0;
       istr >> fHistMax[ipt][ivar];
       ATH_MSG_DEBUG("In HadFlavorTag::ReadWeightsFromFile(): var "<< ivar << " HistMax " << fHistMax[ipt][ivar] );
     }
   }      
   


   //read fRefS[0] and fRefB[0]           
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
       fRefS[0][ivar][jbin]=0;
       istr>>fRefS[0][ivar][jbin];
     }
   }
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
       fRefB[0][ivar][jbin]=0;	
       istr>>fRefB[0][ivar][jbin];	
      }
   }
   //read fRefS[1] and fRefB[1]           
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
      for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
      	fRefS[1][ivar][jbin]=0;
	istr>>fRefS[1][ivar][jbin];
      }
   }
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
       fRefB[1][ivar][jbin]=0;	
       istr>>fRefB[1][ivar][jbin];	
     }
   }
   //read fRefS[2] and fRefB[2]           
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
      for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
      	fRefS[2][ivar][jbin]=0;
	istr>>fRefS[2][ivar][jbin];
      }
   }
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
      for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
	fRefB[2][ivar][jbin]=0;	
	istr>>fRefB[2][ivar][jbin];	
      }
   }
   //read fRefS[3] and fRefB[3]           
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
       fRefS[3][ivar][jbin]=0;
       istr>>fRefS[3][ivar][jbin];
     }
   }
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
      for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
	fRefB[3][ivar][jbin]=0;	
	istr>>fRefB[3][ivar][jbin];	
      }
   }
   //read fRefS[4] and fRefB[4]           
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
       fRefS[4][ivar][jbin]=0;
       istr>>fRefS[4][ivar][jbin];
     }
   }
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
       fRefB[4][ivar][jbin]=0;	
       istr>>fRefB[4][ivar][jbin];	
      }
   }
   //read fRefS[5] and fRefB[5]           
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
       fRefS[5][ivar][jbin]=0;
       istr>>fRefS[5][ivar][jbin];
     }
   }
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
       fRefB[5][ivar][jbin]=0;	
       istr>>fRefB[5][ivar][jbin];	
     }
   }
   //read fRefS[6] and fRefB[6]           
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
       fRefS[6][ivar][jbin]=0;
       istr>>fRefS[6][ivar][jbin];
     }
   }
   // skip 2 empty lines
   istr.getline( dumchar, nchar );
   istr.getline( dumchar, nchar );
   for (unsigned int ivar=0; ivar<fNvars; ivar++) {
     for (int jbin=0; jbin<fNbin[ivar]; jbin++) {
       fRefB[6][ivar][jbin]=0;	
       istr>>fRefB[6][ivar][jbin];	
     }
   }
   
   return StatusCode::SUCCESS;

}




//===============================================================
double HadFlavorTag::TransformLikelihoodOutput( double ps, double pb ) const{

  // returns transformed or non-transformed output
  if (ps < fEpsilon) ps = fEpsilon;
  if (pb < fEpsilon) pb = fEpsilon;
  double r = ps/(ps + pb);
  if (r >= 1.0) r = 1. - 1.e-15;

   if (false) {
     // inverse Fermi function
     
     // sanity check
     if      (r <= 0.0) r = fEpsilon;
     else if (r >= 1.0) r = 1. - 1.e-15;

     double tau = 15.0;
     r = - log(1.0/r - 1.0)/tau;
   }
   
   return r;
}


  //===============================================================
  double HadFlavorTag::GetMvaoutput__(std::vector<double>& inputValues, int ptbin){ 

   double ps(1), pb(1);
   std::vector<double> inputValuesSig = inputValues;
   std::vector<double> inputValuesBgd = inputValues;
   for (size_t ivar = 0; ivar < GetNvar(); ivar++) {

      // dummy at present... will be used for variable transforms
      double x[2] = { inputValuesSig[ivar], inputValuesBgd[ivar] };

      for (int itype=0; itype < 2; itype++) {

         // interpolate linearly between adjacent bins
         // this is not useful for discrete variables (or forced Spline0)
         int bin = int((x[itype] - fHistMin[ptbin][ivar])/(fHistMax[ptbin][ivar] - fHistMin[ptbin][ivar])*fNbin[ivar]) + 0;

         // since the test data sample is in general different from the training sample
         // it can happen that the min/max of the training sample are trespassed --> correct this
         if      (bin < 0) {
            bin = 0;
            x[itype] = fHistMin[ptbin][ivar];
         }
         else if (bin >= fNbin[ivar]) {
            bin = fNbin[ivar]-1;
            x[itype] = fHistMax[ptbin][ivar];
         }

         // find corresponding histogram from cached indices
         float ref = (itype == 0) ? fRefS[ptbin][ivar][bin] : fRefB[ptbin][ivar][bin];

         // sanity check
         if (ref < 0) {
            std::cout << "Fatal error in ReadLikelihoodKDE: bin entry < 0 ==> abort" << std::endl;
            std::exit(1);
         }

         double p = ref;

         if (GetType(ivar) != 'I' && !fHasDiscretePDF[ivar]) {
            float bincenter = (bin + 0.5)/fNbin[ivar]*(fHistMax[ptbin][ivar] - fHistMin[ptbin][ivar]) + fHistMin[ptbin][ivar];
            int nextbin = bin;
            if ((x[itype] > bincenter && bin != fNbin[ivar]-1) || bin == 0) 
               nextbin++;
            else
               nextbin--;  

            double refnext      = (itype == 0) ? fRefS[ptbin][ivar][nextbin] : fRefB[ptbin][ivar][nextbin];
            float nextbincenter = (nextbin + 0.5)/fNbin[ivar]*(fHistMax[ptbin][ivar] - fHistMin[ptbin][ivar]) + fHistMin[ptbin][ivar];

            double dx = bincenter - nextbincenter;
            double dy = ref - refnext;
            p += (x[itype] - bincenter) * dy/dx;
         }

         if (p < fEpsilon) p = fEpsilon; // avoid zero response

         if (itype == 0) ps *= p;
         else            pb *= p;
      }            
   }     

   // the likelihood ratio (transform it ?)
   return TransformLikelihoodOutput( ps, pb );   
   
}//GetMvaoutput__
  


//===============================================================
 double HadFlavorTag::GetMvaoutput(std::vector<double>& inputVars, int ptbin){ 

   // classifier response value
   double retval = 0;
   
   //GetMVaValue
   if (IsNormalised()) {
     // normalise variables
     std::vector<double> iV;
     unsigned int ivar = 0;
     for (std::vector<double>::const_iterator varIt = inputVars.begin(); varIt != inputVars.end(); varIt++, ivar++) {
       iV.push_back(NormVariable( *varIt, fVmin[ptbin][ivar], fVmax[ptbin][ivar] ));
     }
     
     //Compute MvaValue using iV
     retval = GetMvaoutput__( iV, ptbin );
     
   } else {
     //compute MVa value using inputValues
     retval = GetMvaoutput__( inputVars, ptbin );
   }
   
   return retval;
   
}//GetMvaoutput
  




  //===============================================================
  double HadFlavorTag::GetQGLikelihood(std::vector<double>& inputVars){ 
  
    //expect inputVars to be of form:
    //    cone, pt, eta, ntrk, trkwidth

    //copy of vars
    int Nvar = inputVars.size();
    fNvars = Nvar;

    if(fNvars!=5) return -1.;

    double cone,pt,eta,ntrk,trkwidth;
    
    cone     = inputVars[0];
    pt       = inputVars[1];
    eta      = TMath::Abs( inputVars[2] );
    ntrk     = inputVars[3];
    trkwidth = inputVars[4];

    if( pt > m_QGMaxPt || pt < m_QGMinPt ){
      ATH_MSG_DEBUG("In HadFlavorTag::GetQGLikelihood(): Jet fails pT taggability requirement");
      return -1.;
    }
    if( eta  > m_QGMaxEta ){
      ATH_MSG_DEBUG("In HadFlavorTag::GetQGLikelihood(): Jet fails eta taggability requirement");
      return -1;
    }

    //Given cone, pt, and eta...
    //get the right histogram from m_histList

    float selectedPtBin=-1;
    for( unsigned int iptbin=0; iptbin<m_QGPtBounds.size()-1; iptbin++){
      if( pt/1000. < m_QGPtBounds.at(iptbin+1) ){
	selectedPtBin = m_QGPtBounds.at(iptbin);
	break;
      }
    }

    float selectedEtaBin=-1;
    for( unsigned int ietabin=0; ietabin<m_QGEtaBounds.size()-1; ietabin++){
      if( eta < m_QGEtaBounds.at(ietabin+1) ){
	selectedEtaBin = m_QGEtaBounds.at(ietabin);
	break;
      }
    }

    //Convert floats to strings to find correct hist
    ostringstream tempss;
    tempss << selectedPtBin;
    std::string selectedPtBinString = tempss.str();
    tempss.str("");
    tempss << selectedEtaBin * 10.;
    std::string selectedEtaBinString = tempss.str();

    TH2F* tmphist = 0;

    const std::string histName = m_QGHistBaseName+selectedPtBinString+"_eta"+selectedEtaBinString+"_q";
    tmphist = dynamic_cast<TH2F*>( m_QGLikelihoodFile->Get( histName.c_str() ) );

    double QGLikelihood=-1.;

    if ( !tmphist ){
      ATH_MSG_ERROR("dynamic_cast<TH2F*> failed for TObject: " << histName );
    }  else  {
      int tmpbin = tmphist->FindBin( trkwidth, ntrk );
      QGLikelihood = tmphist->GetBinContent( tmpbin );
    }

    delete tmphist;

    return QGLikelihood;

  }


}


