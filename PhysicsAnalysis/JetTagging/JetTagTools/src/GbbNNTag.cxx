/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************

   @class GbbNNTag - new gbb BTagging algorithm

   Created - May 2011

   @author: 
   * Maria Laura Gonzalez Silva (laugs@cern.ch)
   --- ( University of BUENOS AIRES ) ---

   (c) 2011- ATLAS Detector Software

********************************************************/

#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <istream> 
#include <string>
#include <stdlib.h>
#include "Riostream.h"
#include "TMath.h"
#include "TMatrix.h"

#include "JetTagTools/GbbNNTag.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "FourMom/P4PxPyPzE.h"
//#include "JetEvent/Jet.h"
#include "GaudiKernel/MsgStream.h"

#include "JetTagTools/TrackSelector.h"
#include "JetTagInfo/GbbNNTagInfo.h"
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfo/ITagInfo.h"

#include "StoreGate/StoreGateSvc.h"

#include "JetTagEvent/TrackAssociation.h"
#include "xAODTracking/TrackParticle.h"

#include "PathResolver/PathResolver.h"


namespace Analysis {

GbbNNTag::GbbNNTag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_trackSelectorTool("Analysis::TrackSelector")
   {
    
    declareInterface<ITagTool>(this);
    // global configuration:
     declareProperty("Runmodus",     m_runModus     = "analysis");
 
  // tools:
  declareProperty("trackSelectorTool", m_trackSelectorTool);

  //Neural Network configuration

  //weights
  declareProperty("NNweightsFileName", m_weightsFileName="GbbNNweightsFile.txt");
  
  declareProperty("NNmaxNumerOfLayers",  m_max_nLayers_  = 6);
  
  declareProperty("NNmaxNumerOfNodes",  m_max_nNodes_  = 32);

  declareProperty("NNmaxNumerOfVariables",  m_max_nVar_  = 6);
  
  declareProperty("NNnumerOfInputVariables",  m_fNvars  = 4);


}

GbbNNTag::~GbbNNTag() {
}


//==============================================================
StatusCode GbbNNTag::initialize() {


  
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
StatusCode GbbNNTag::InitReferenceMode() 
{
  // =====================
  // Book histograms here
  // =====================
  
  StatusCode sc = StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG("reference mode not supported for GbbNNTag");
  
  return sc;
}


// ==================================================================
StatusCode GbbNNTag::InitAnalysisMode() 
{
  // ==========================
  // Read calibration file here
  // ==========================
  fYNN=0;
  StatusCode sc = StatusCode::SUCCESS;
   
  std::filebuf fb;
  std::string filename = PathResolver::find_file(m_weightsFileName, "DATAPATH");
  const char* name = filename.c_str();
  fb.open ( name,ios::in);
  std::istream is(&fb);
  
  sc = ReadWeightsFromFile(is);

  fb.close();

  return sc;
}


//==========================================================
 
void GbbNNTag::finalizeHistos() {
}
  

//=============================================================
StatusCode GbbNNTag::finalize() {

 if (fYNN != 0) {
   for (int i=0; i<fNlayers; i++){
     delete[] fYNN[i];
   }
   delete[] fYNN;
   fYNN = 0;
 }

  return StatusCode::SUCCESS;
}



//============================================================
void GbbNNTag::tagJet(xAOD::Jet& jetToTag) {


    if (m_runModus == "analysis") 
      JetTagAnalysisMode(jetToTag);
    
    // run the part of the tagging algorithm done only in reference mode
    if (m_runModus == "reference") 
      JetTagReferenceMode(jetToTag);
  

    return;
    
}
 
   

// ======================================================
void GbbNNTag::JetTagAnalysisMode(xAOD::Jet& jetToTag)
{
  // ===============================================================
  // part of the jet tagging algorithm done only in analysis mode
  // ===============================================================
  

  // initialize weight and set input vars default values
  double weight = -1001;
  int ntrk = -1001;
  double trkwidth =-1001;
  double drmax = -1001;

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

  GbbNNTagInfo* m_gbbnnInfo = new GbbNNTagInfo(instanceName);
  jetToTag.addInfo(m_gbbnnInfo);
    

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
     
  if(ta && ntrk>0 ){
	 
    trkwidth = m_trkwidth(trklist,jetDirection);
    drmax = m_drmax(trklist);

    if(trkwidth > 0 && drmax >0){

      // vector of input vars
      vector<double> vars;
      vars.push_back(drmax);
      vars.push_back(jetToTag.pt()); // this should be @ calib state
      vars.push_back(ntrk);
      vars.push_back(trkwidth);
      
      //get neural net output value
      weight = GetNNoutput(vars);
      
    }  
    
  }
 

  ATH_MSG_DEBUG("In  GbbNNTag::JetTagAnalysisMode: weight "<< weight);

  std::vector<double> tmpll;
  tmpll.push_back(weight);
  // Store NN input vars and output as weight   
  m_gbbnnInfo->setTagLikelihood(tmpll);
  m_gbbnnInfo->setWeight(weight); /// you can keep it.
  m_gbbnnInfo->setnMatchingTracks(ntrk);
  m_gbbnnInfo->settrkJetWidth(trkwidth);
  m_gbbnnInfo->settrkJetMaxDeltaR(drmax);
     

  m_gbbnnInfo->makeValid();

   
}


//================================================================
double GbbNNTag::m_trkwidth(std::vector<const xAOD::TrackParticle*> tracks, xAOD::IParticle::FourMom_t j){
  
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
double GbbNNTag::m_drmax(std::vector<const xAOD::TrackParticle*> tracks){
  
  double maxDR = -1;
  for(std::vector<const xAOD::TrackParticle*>::iterator trkItr1 = tracks.begin(); trkItr1 !=tracks.end(); ++trkItr1) {
    
    for(std::vector<const xAOD::TrackParticle*>::iterator trkItr2 = tracks.begin(); trkItr2 !=tracks.end(); ++trkItr2) {
      
      const xAOD::TrackParticle* aTemp1 = *trkItr1;
      const xAOD::TrackParticle* aTemp2 = *trkItr2;
      if(aTemp2==aTemp1)continue; 
      
      double dR =aTemp1->p4().DeltaR(aTemp2->p4());
      if(dR>maxDR) maxDR = dR;
    }
  }
  
  return maxDR;
  
}



// ======================================================
void GbbNNTag::JetTagReferenceMode(xAOD::Jet& /*jetToTag*/)
{

  return;
  /*********************************************************************

    Not filling reference histos for now (not being run in reference mode

  *********************************************************************/ 

  

}




//==========================================================
bool GbbNNTag::m_preselect(const xAOD::TrackParticle *t)
{
 
  // performs track selection 
  //
  
  ATH_MSG_VERBOSE("m_preselect(): trkPrt->pt(): " << t->pt());
  
  m_trackSelectorTool->primaryVertex(m_priVtx->recVertex().position());
  m_trackSelectorTool->prepare();
  
  // apply track selection  (here i'm using standar b-tagging track selection)
  if( m_trackSelectorTool->selectTrack(t) ) {
    
    // apply further cuts here    
    // double eta = fabs(t->measuredPerigee()->eta());
    double pt = t->pt()/1000.;
    
    // retrieve summary
    // const Trk::TrackSummary* summary = (*t).trackSummary();
    
    // fraction hits in TRT (with outliers)
    // int nTRT         = summary->get(Trk::numberOfTRTHits);
        

    /// you can use the track selector to do this.
    if (pt>1.) return true;
  }
  
  return false;
}


void GbbNNTag::m_printParameterSettings()
{
  ATH_MSG_INFO( "#BTAG# " << name() << "Parameter settings ");
  ATH_MSG_INFO( "#BTAG# I am in " << m_runModus << " modus.");
}



/* ------------------------------------------------------------------- */
/*                        Private Functions                     */
/* ------------------------------------------------------------------- */


//===============================================================
double  GbbNNTag::W_ref(double wNN[], int a_1, int a_2, int a_3) const{ 
  return wNN [(a_3*m_max_nNodes_ + a_2)*m_max_nLayers_ + a_1 - 187];
}
  
//===============================================================
double&  GbbNNTag::W_ref(double wNN[], int a_1, int a_2, int a_3) {
    return wNN [((a_3)*m_max_nNodes_ + (a_2))*m_max_nLayers_ + a_1 - 187];
}
  

//===============================================================
double  GbbNNTag::Ww_ref(double wwNN[], int a_1,int a_2) const{ 
  return wwNN[(a_2)*m_max_nLayers_ + a_1 - 7];
}

//===============================================================  
double& GbbNNTag::Ww_ref(double wwNN[], int a_1,int a_2) {
  return wwNN[(a_2)*m_max_nLayers_ + a_1 - 7];
}

//===============================================================
double GbbNNTag::ActivationFunction( int i, double u ){ 
  double f(0);
  if      (u/temp[i] >  170) f = +1;
  else if (u/temp[i] < -170) f = -1;
  else {
    double yy = TMath::Exp(-u/temp[i]);
    f  = (1 - yy)/(1 + yy);
  }
  
  return f;
}


//===============================================================
StatusCode GbbNNTag::ReadWeightsFromFile( istream& istr )
{
   
   TString var;

    // read number of variables and classes
    int nva(0), lclass(0);
    istr >> nva >> lclass;

    ATH_MSG_DEBUG( " In GbbNNTag::ReadWeightsFromFile(): numer of variables " << nva);  
    
    if (m_fNvars != nva){
      ATH_MSG_ERROR( "In GbbNNTag::ReadWeightsFromFile(): mismatch in number of variables" );
      return StatusCode::FAILURE;
    }

   // number of output classes must be 2
    if (lclass != 2){ // wrong file
      ATH_MSG_ERROR("In GbbNNTag::ReadWeightsFromFile(): mismatch in number of classes" );
      return StatusCode::FAILURE;
    }
          
   // check that we are not at the end of the file
    if (istr.eof( )){
      ATH_MSG_ERROR("In GbbNNTag::ReadWeightsFromFile(): ERROR -> reached EOF prematurely ");
      return StatusCode::FAILURE;
    }
   
    
   // read max & min of input variables
     for (int ivar=0; ivar<m_fNvars; ivar++) {
       xmax[ivar]=0;
       xmin[ivar]=0;
       istr >> xmin[ivar] >> xmax[ivar];
       ATH_MSG_DEBUG("In GbbNNTag::ReadWeightsFromFile(): var "<< ivar << " xmin " << xmin[ivar] << " xmax " << xmax[ivar]);
     }
            

   // read number of layers (sum of: input + output + hidden)
   istr >> layerm;
      
   //  if (fYNN != 0) {
   //      for (int i=0; i<fNlayers; i++){
   //        delete[] fYNN[i];
   //      }
   //      delete[] fYNN;
   //      fYNN = 0;
   //    }

   fYNN = new double*[layerm];
   for (int layer=0; layer<layerm; layer++) {              
      // read number of neurons for each layer
      istr >> neuron[layer];
      
      fYNN[layer] = new double[neuron[layer]];
   }
         
   // to read dummy lines
   const int nchar(100);
   char* dumchar = new char[nchar];
            
   // read weights
   for (int layer=1; layer<=layerm-1; layer++) {
              
      int nq = neuron[layer]/10;
      int nr = neuron[layer] - nq*10;
              
      int kk(0);
      if (nr==0) kk = nq;
      else       kk = nq+1;
              
      for (int k=1; k<=kk; k++) {
         int jmin = 10*k - 9;
         int jmax = 10*k;
         if (neuron[layer]<jmax) jmax = neuron[layer];
         for (int j=jmin; j<=jmax; j++) {
	   istr >> Ww_ref(WweightNN, layer+1, j);
         }
         for (int i=1; i<=neuron[layer-1]; i++) {
            for (int j=jmin; j<=jmax; j++) {
               istr >> W_ref(weightNN, layer+1, j, i);
            }
         }
         // skip two empty lines
         istr.getline( dumchar, nchar );
      }
   }

   for (int layer=0; layer<layerm; layer++) {
              
      // skip 2 empty lines
      istr.getline( dumchar, nchar );
      istr.getline( dumchar, nchar );
              
      istr >> temp[layer];
   }            

   // sanity check
   if (m_fNvars != neuron[0]) {
     ATH_MSG_ERROR(  " <ReadWeightsFromFile> mismatch in zeroth layer:"  << m_fNvars << " " << neuron[0] );
     return  StatusCode::FAILURE;
   }

   fNlayers = layerm;

   return StatusCode::SUCCESS;

}



//===============================================================
 double GbbNNTag::GetNNoutput(std::vector<double>& inputVars){ 
  
   //copy of vars
   int Nvar = inputVars.size();
   m_fNvars = Nvar;

    double* vars = new double[Nvar];
    for (int ivar=0; ivar<Nvar; ivar++) vars[ivar] = inputVars[ivar];
    
    //Vars normalizacion
    for (int jvar=0; jvar<Nvar; jvar++) {
      
      if (xmax[jvar] < vars[jvar]) vars[jvar] = xmax[jvar];
      if (xmin[jvar] > vars[jvar]) vars[jvar] = xmin[jvar];
      if (xmax[jvar] == xmin[jvar]) vars[jvar] = 0;
      else {
	vars[jvar] = vars[jvar] - ((xmax[jvar] + xmin[jvar])/2);    
	vars[jvar] = vars[jvar] / ((xmax[jvar] - xmin[jvar])/2);  
      }
      
    }
    
    
    // auxiliary functions
   for (int ivar=0; ivar<neuron[0]; ivar++) fYNN[0][ivar] = vars[ivar];

   for (int layer=1; layer<layerm; layer++) {
      for (int j=1; j<=neuron[layer]; j++) {

         double x = Ww_ref(WweightNN, layer+1,j); // init with the bias layer

         for (int k=1; k<=neuron[layer-1]; k++) { // neurons of originating layer
            x += fYNN[layer-1][k-1]*W_ref(weightNN, layer+1, j, k);
         }
         fYNN[layer][j-1] = ActivationFunction( layer, x );
      }
   }  
   
   //NN output in [0,1]
   double retval = 0.5*(1.0 + fYNN[layerm-1][0]);
   
   delete [] vars;

   return retval;
   
   
   
}//GetNNResponse
  



}


