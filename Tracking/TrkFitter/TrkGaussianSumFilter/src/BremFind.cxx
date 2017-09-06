/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************************
                      BremFind.cxx * description

completed   : Tuesday 18th November 2008
author      : Tony Shao
email       : Qi.Tao.Shao@cern.ch
description : Class for finding brem points in the inner detector using the GSF
**********************************************************************************/



#include "TrkGaussianSumFilter/BremFind.h"
#include "QoverPBremFit.h"

#include <math.h>
#include "TMath.h"

#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkTrack/Track.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"

#include "TrkGeometry/Layer.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/MagneticFieldProperties.h"

#include "TrkVolumes/AbstractVolume.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderSurface.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TTree.h"



Trk::BremFind::BremFind(const std::string& type, const std::string& name, const IInterface* parent):
  AthAlgTool(type, name, parent),
  m_stateCombiner("Trk::MultiComponentStateCombiner"),
  m_propagator("Trk::IntersectorWrapper/IntersectorWrapper"),
  //m_fieldProperties
  m_trackingGeometrySvc("TrackingGeometrySvc","AtlasTrackingGeometrySvc"),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_trackingGeometry(nullptr),
  //m_validationMode
  m_validationTreeName("BremInfo"),
  m_validationTreeName2("BremInfoZ"),
  m_validationTreeDescription("Brem Information"),
  m_validationTreeFolder("/valGSF2/BremInfo"),
  m_validationTreeFolder2("/valGSF2/BremInfoZ"),
  m_validationTree(nullptr),
  m_validationTree2(nullptr),
  //m_useCalibration{},
  //m_usePropagate{},
  m_forwardparameters{},
  m_smoothedparameters{},
  m_combinedparameters{},
  m_perigee_1overP{},
  m_perigee_Phi{},
  m_perigee_Theta{},
  m_perigee_d0{},
  m_perigee_z0{},
  m_brem_value{},
  m_brem_phi{},
  m_brem_theta{},
  m_brem_energy{},
  m_brem_UpperBound{},
  m_brem_LowerBound{},
  m_forward_kink{},
  m_smoothed_kink{},
  m_brem_significance{},
  m_brem_valueCalibrated{},
  m_surfaceX{},
  m_surfaceY{},
  m_surfaceZ{},
  m_nBrems{},
  m_Z_mode{},
  //vectors could be initialised here
  m_event_ID{},
  m_forwardparameter_constant{},
  m_forwardparameter_coefficient{},
  m_forwardparameter_value{},
  m_forwardparameter_width{},
  m_smoothparameter_constant{},
  m_smoothparameter_coefficient{},
  m_smoothparameter_value{},
  m_smoothparameter_width{},
  m_forward_1overP{},
  m_forward_1overPerr{},
  m_forward_value{},
  m_smooth_1overP{},
  m_smooth_1overPerr{},
  m_smooth_value{},
  m_KinkSeparationScores{},
  m_KinkSeparationScoresErr{},
  m_forwardBremFit{},
  m_smoothedBremFit{}
{
  declareInterface<IBremsstrahlungFinder>(this);
  //jobOptions Variables
  declareProperty("StateCombiner",   m_stateCombiner );
  declareProperty("TrackingGeometrySvc",  m_trackingGeometrySvc);
  declareProperty("Propagator", m_propagator);
  declareProperty("UseCalibration", m_useCalibration=true);
  declareProperty("ValidationMode", m_validationMode=false);
  declareProperty("UseSurfacePropagation", m_usePropagate=false);
  declareProperty("TreeFolderName", m_validationTreeFolder);
  declareProperty("TreeFolderName2", m_validationTreeFolder2);
  declareProperty("EventInfoKey", m_readKey = "EventInfo");
}


StatusCode Trk::BremFind::initialize()
{
   // The TrackingGeometrySvc ------------------------------------------------------
  ATH_CHECK (m_trackingGeometrySvc.retrieve());
  
  msg(MSG::DEBUG) << "Retrieved service " << m_trackingGeometrySvc << endmsg;
  m_trackingGeometryName = m_trackingGeometrySvc->trackingGeometryName();
  

  // Request the state combiner
  ATH_CHECK ( m_stateCombiner.retrieve() );
 
  //Retrieve the propagator
  ATH_CHECK (m_propagator.retrieve());
  
  m_fieldProperties= Trk::MagneticFieldProperties(Trk::FullField);



  //The Validation step ----------------------------------------------------------
  
  if (m_validationMode) {
    
    if (not m_validationTree) {
      //Crate a new tree if there doesn't exist one already
      m_validationTree = new TTree( m_validationTreeName.c_str(), m_validationTreeDescription.c_str() );
    
      
      m_validationTree->Branch("nBrems" , &m_nBrems, "nBrems/I");
      m_validationTree->Branch("Brem_Radii", &m_brem_value);
      m_validationTree->Branch("Brem_Phi", &m_brem_phi);
      m_validationTree->Branch("Brem_Theta", &m_brem_theta);
      m_validationTree->Branch("Brem_Energy", &m_brem_energy);
      m_validationTree->Branch("Brem_UpperBound", &m_brem_UpperBound);
      m_validationTree->Branch("Brem_LowerBound", &m_brem_LowerBound);
      m_validationTree->Branch("Forward_Kink", &m_forward_kink);
      m_validationTree->Branch("Backward_Kink", &m_smoothed_kink);
      m_validationTree->Branch("Brem_Significance", &m_brem_significance);
      m_validationTree->Branch("Brem_RadiiCalibrated", &m_brem_valueCalibrated);
      m_validationTree->Branch("SurfaceX", &m_surfaceX);
      m_validationTree->Branch("SurfaceY", &m_surfaceY);
      m_validationTree->Branch("SurfaceZ", &m_surfaceZ);
      m_validationTree->Branch("EventID" , &m_event_ID, "EventID/I");
      m_validationTree->Branch("ForwardParameterConstant", &m_forwardparameter_constant,"ForwardParameterConstant/D");
      m_validationTree->Branch("SmoothParameterConstant", &m_smoothparameter_constant,"SmoothParameterConstant/D");
      m_validationTree->Branch("ForwardParameterCoefficient", &m_forwardparameter_coefficient);
      m_validationTree->Branch("ForwardParameterRadii", &m_forwardparameter_value);
      m_validationTree->Branch("ForwardParameterWidth", &m_forwardparameter_width);
      m_validationTree->Branch("SmoothParameterCoefficient", &m_smoothparameter_coefficient);
      m_validationTree->Branch("SmoothParameterRadii", &m_smoothparameter_value);
      m_validationTree->Branch("SmoothParameterWidth", &m_smoothparameter_width);
      m_validationTree->Branch("Forward1overP", &m_forward_1overP);
      m_validationTree->Branch("Forward1overPerr", &m_forward_1overPerr);
      m_validationTree->Branch("ForwardRadii", &m_forward_value);
      m_validationTree->Branch("Smooth1overP", &m_smooth_1overP);
      m_validationTree->Branch("Smooth1overPerr", &m_smooth_1overPerr);
      m_validationTree->Branch("SmoothRadii", &m_smooth_value);
      m_validationTree->Branch("SeparationQuality", &m_KinkSeparationScores);
      m_validationTree->Branch("SeparationQualityErr", &m_KinkSeparationScoresErr);
           
    }

    


    if (not m_validationTree2) {
      //Crate a new tree if there doesn't exist one already
      m_validationTree2 = new TTree( m_validationTreeName2.c_str(), m_validationTreeDescription.c_str() );
    
      
      m_validationTree2->Branch("nBrems" , &m_nBrems, "nBrems/I");
      m_validationTree2->Branch("Brem_Z", &m_brem_value);
      m_validationTree2->Branch("Brem_Phi", &m_brem_phi);
      m_validationTree2->Branch("Brem_Theta", &m_brem_theta);
      m_validationTree2->Branch("Brem_Energy", &m_brem_energy);
      m_validationTree2->Branch("Brem_UpperBound", &m_brem_UpperBound);
      m_validationTree2->Branch("Brem_LowerBound", &m_brem_LowerBound);
      m_validationTree2->Branch("Forward_Kink", &m_forward_kink);
      m_validationTree2->Branch("Backward_Kink", &m_smoothed_kink);
      m_validationTree2->Branch("Brem_Significance", &m_brem_significance);
      m_validationTree2->Branch("Brem_ZCalibrated", &m_brem_valueCalibrated);
      m_validationTree2->Branch("SurfaceX", &m_surfaceX);
      m_validationTree2->Branch("SurfaceY", &m_surfaceY);
      m_validationTree2->Branch("SurfaceZ", &m_surfaceZ);
      m_validationTree2->Branch("EventID" , &m_event_ID, "EventID/I");
      m_validationTree2->Branch("ForwardParameterConstant", &m_forwardparameter_constant,"ForwardParameterConstant/D");
      m_validationTree2->Branch("SmoothParameterConstant", &m_smoothparameter_constant,"SmoothParameterConstant/D");
      m_validationTree2->Branch("ForwardParameterCoefficient", &m_forwardparameter_coefficient);
      m_validationTree2->Branch("ForwardParameterZ", &m_forwardparameter_value);
      m_validationTree2->Branch("ForwardParameterWidth", &m_forwardparameter_width);
      m_validationTree2->Branch("SmoothParameterCoefficient", &m_smoothparameter_coefficient);
      m_validationTree2->Branch("SmoothParameterZ", &m_smoothparameter_value);
      m_validationTree2->Branch("SmoothParameterWidth", &m_smoothparameter_width);
      m_validationTree2->Branch("Forward1overP", &m_forward_1overP);
      m_validationTree2->Branch("Forward1overPerr", &m_forward_1overPerr);
      m_validationTree2->Branch("ForwardZ", &m_forward_value);
      m_validationTree2->Branch("Smooth1overP", &m_smooth_1overP);
      m_validationTree2->Branch("Smooth1overPerr", &m_smooth_1overPerr);
      m_validationTree2->Branch("SmoothZ", &m_smooth_value);
      m_validationTree2->Branch("SeparationQuality", &m_KinkSeparationScores);
      m_validationTree2->Branch("SeparationQualityErr", &m_KinkSeparationScoresErr);
           
    }
 

    //register the Tree
    ITHistSvc* tHistSvc = 0;
    if (service("THistSvc",tHistSvc).isFailure()) {
      msg(MSG::ERROR) << "initialize() Could not find Hist Service -> Switching ValidationMode Off !" << endmsg;
      delete m_validationTree; 
      delete m_validationTree2;
      m_validationTree=0;
      m_validationTree2=0;
    }
    if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()) {
      msg(MSG::ERROR)<<"initialize() Could not register the validation Tree -> Switching ValidationMode off !" << endmsg;
      delete m_validationTree;
      m_validationTree = 0;
    }
    if ((tHistSvc->regTree(m_validationTreeFolder2, m_validationTree2)).isFailure()) {
      msg(MSG::ERROR)<<"initialize() Could not register the validation Tree 2 -> Switching ValidationMode off !" << endmsg;
      delete m_validationTree2;
      m_validationTree2 = 0;
    }
    
  }

  //---------------------------- end of validation mode ------------------------------------

 
  ATH_CHECK( m_readKey.initialize() );

  ATH_MSG_DEBUG( "Initialisation of " << name() << " was successful" );
  
  return StatusCode::SUCCESS;

}


StatusCode Trk::BremFind::finalize(){
 
 ATH_MSG_DEBUG( "Finalisation of " << name() << " was successful" );

 return StatusCode::SUCCESS;

}

void Trk::BremFind::BremFinder(const Trk::ForwardTrajectory& forwardTrajectory, const Trk::SmoothedTrajectory& smoothedTrajectory) {
  BremFinder(forwardTrajectory, smoothedTrajectory, false);
}

void Trk::BremFind::BremFinder(const Trk::ForwardTrajectory& forwardTrajectory, const Trk::SmoothedTrajectory& smoothedTrajectory, bool Z_mode)
{
  m_Z_mode = Z_mode;
  
  if (!m_trackingGeometry)
    retrieveTrackingGeometry();

  
  msg(MSG::DEBUG) << "Entering BremFinder Function " << endmsg;

  m_brem_value = new std::vector<double>;
  m_brem_phi = new std::vector<double>;
  m_brem_theta = new std::vector<double>;
  m_brem_energy = new std::vector<double>;
  m_brem_UpperBound = new std::vector<double>;
  m_brem_LowerBound = new std::vector<double>;
  m_forward_kink = new std::vector<double>;
  m_smoothed_kink = new std::vector<double>;
  m_brem_significance = new std::vector<double>;
  m_brem_valueCalibrated = new std::vector<double>;
  m_surfaceX = new std::vector<double>;
  m_surfaceY = new std::vector<double>;
  m_surfaceZ = new std::vector<double>;
  m_brem_trackStateOnSurface.clear();
  m_EstimatedBremOnTrack.clear();
  m_brem_TrackParameters.clear();
  m_brem_surfaces.clear();
  m_brem_layers.clear();
  m_surface_positions.clear();

  m_forwardparameter_coefficient = new std::vector<double>;
  m_forwardparameter_value = new std::vector<double>;
  m_forwardparameter_width = new std::vector<double>;
  m_smoothparameter_coefficient = new std::vector<double>;
  m_smoothparameter_value = new std::vector<double>;
  m_smoothparameter_width = new std::vector<double>;
  m_forward_1overP = new std::vector<double>;
  m_forward_1overPerr = new std::vector<double>;
  m_forward_value = new std::vector<double>;
  m_smooth_1overP = new std::vector<double>;
  m_smooth_1overPerr = new std::vector<double>;
  m_smooth_value = new std::vector<double>;
  m_KinkSeparationScores = new std::vector<double>;
  m_KinkSeparationScoresErr = new std::vector<double>;


  //Obtain the parameters of the Tanh fit for both forward and smooth trajectories
  m_forwardBremFit = new QoverPBremFit(1,m_Z_mode);
  m_forwardparameters = m_forwardBremFit->GetParameters(forwardTrajectory);
  m_smoothedBremFit = new QoverPBremFit(-1,m_Z_mode);
  m_smoothedparameters = m_smoothedBremFit->GetParameters(smoothedTrajectory);
  //GetPerigee functions will only make sense when using the smoothed trajectory
  m_perigee_1overP = m_smoothedBremFit->GetPerigee_1overP();
  m_perigee_Phi = m_smoothedBremFit->GetPerigee_Phi();
  m_perigee_Theta = m_smoothedBremFit->GetPerigee_Theta();
  m_perigee_d0 = m_smoothedBremFit->GetPerigee_d0();
  m_perigee_z0 = m_smoothedBremFit->GetPerigee_z0();
  
  
  CombineParameters();
  
  GetBremLocations();
  
  m_nBrems = m_brem_value->size();
  
  for (int bound_counter(0); bound_counter < (int) m_brem_value->size(); bound_counter++) {
    m_brem_UpperBound->push_back(GetBound((*m_brem_value)[bound_counter],1));
    m_brem_LowerBound->push_back(GetBound((*m_brem_value)[bound_counter],-1));
    m_brem_significance->push_back(BremSignificance((*m_brem_value)[bound_counter],m_forwardBremFit,m_smoothedBremFit));
  }
  
  //Calibration of the brem value
  if (m_useCalibration) {
    CalibrateValue();
  }
  else {
    for (int copy_counter(0); copy_counter < (int) m_brem_value->size(); copy_counter++) {
      m_brem_valueCalibrated->push_back((*m_brem_value)[copy_counter]);
    }
  }
  
  for (int fill_counter(0); fill_counter < (int) m_brem_value->size(); fill_counter++) {
    m_brem_theta->push_back(m_perigee_Theta);
    m_brem_phi->push_back(GetPhiFromValue((*m_brem_valueCalibrated)[fill_counter],m_smoothedBremFit));
  }

  //double closest_floorF,closest_floorS;
  //int closestSurfaceNoF(0);
  int closestSurfaceNoS(0);

  for (int brem_counter2(0); brem_counter2 < (int) m_brem_value->size(); brem_counter2++) {

    //The closest measurement numbers to the brem location
    //closestSurfaceNoF=ClosestMeasurement((*m_brem_valueCalibrated)[brem_counter2],m_forwardBremFit);
    closestSurfaceNoS=ClosestMeasurement((*m_brem_valueCalibrated)[brem_counter2],m_smoothedBremFit);

    //closest_floorF = m_forwardBremFit->GetValue(closestSurfaceNoF);
    //closest_floorS = m_smoothedBremFit->GetValue(closestSurfaceNoS);
    
    //Filling the associated surfaces for each brem
    const Trk::TrackParameters* closestTrackParameters = m_smoothedBremFit->GetTrackParameters(closestSurfaceNoS);
    m_brem_surfaces.push_back(ClosestSurface((*m_brem_value)[brem_counter2],(*m_brem_phi)[brem_counter2],(*m_brem_theta)[brem_counter2],*closestTrackParameters));
    
    m_surface_positions.push_back(SurfacePosition(*closestTrackParameters,*m_brem_surfaces[brem_counter2],(*m_brem_value)[brem_counter2],(*m_brem_phi)[brem_counter2],(*m_brem_theta)[brem_counter2]));


    m_surfaceX->push_back(m_surface_positions[brem_counter2].x());
    m_surfaceY->push_back(m_surface_positions[brem_counter2].y());
    m_surfaceZ->push_back(m_surface_positions[brem_counter2].z());
    
       
  }
  
  double remaining_energy(1/m_perigee_1overP);
  

  //This loop fills the EstimatedBremOnTrack vector
  
  for (int brem_counter(0); brem_counter < (int) m_brem_value->size(); brem_counter++) {

    
    Trk::EstimatedBremOnTrack *estimatedBremOnTrack(0);
    remaining_energy -= (*m_brem_energy)[brem_counter];
    double retainedEnFraction(remaining_energy*m_perigee_1overP);
    
    double sigma1overPsquared(m_smoothedBremFit->Get1overPerror(0));
    
    
    if (m_brem_layers[brem_counter] && m_brem_surfaces[brem_counter] && m_brem_TrackParameters[brem_counter]) {
      estimatedBremOnTrack = new EstimatedBremOnTrack(m_brem_layers[brem_counter]->thickness(),
						      retainedEnFraction,0.0,
						      sigma1overPsquared,
						      //*m_brem_surfaces[brem_counter], 
						      *m_brem_TrackParameters[brem_counter]->associatedSurface().clone(),
						      bothway);
      
    }

    m_EstimatedBremOnTrack.push_back(estimatedBremOnTrack);
    
  }
  
  //Creates a vector of TrackStateOnSurface
  MakeTrackStateOnSurfaces();
    
  
  //* Retrieve the event info for later syncrinization
  SG::ReadHandle< xAOD::EventInfo>  eventInfo (m_readKey);
  if (!eventInfo.isValid()) {
    msg(MSG::ERROR) << "Could not retrieve event info" << endmsg;
  }
  
  m_event_ID            =  eventInfo->eventNumber();

  //Fill the TanH coefficients and graph values
  for (int forward_fill(0); forward_fill < (int) m_forwardparameters.coefficient.size(); forward_fill++) {
    m_forwardparameter_coefficient->push_back(m_forwardparameters.coefficient[forward_fill]);
    m_forwardparameter_value->push_back(m_forwardparameters.value[forward_fill]);
    m_forwardparameter_width->push_back(m_forwardparameters.width[forward_fill]);
  }

  for (int smooth_fill(0); smooth_fill < (int) m_smoothedparameters.coefficient.size(); smooth_fill++) {
    m_smoothparameter_coefficient->push_back(m_smoothedparameters.coefficient[smooth_fill]);
    m_smoothparameter_value->push_back(m_smoothedparameters.value[smooth_fill]);
    m_smoothparameter_width->push_back(m_smoothedparameters.width[smooth_fill]);
  }

  for (int forwardgraph_fill(0); forwardgraph_fill < m_forwardBremFit->GetnSurfaces(); forwardgraph_fill++) {
    m_forward_1overP->push_back(m_forwardBremFit->Get1overP(forwardgraph_fill));
    m_forward_1overPerr->push_back(m_forwardBremFit->Get1overPerror(forwardgraph_fill));
    m_forward_value->push_back(m_forwardBremFit->GetValue(forwardgraph_fill));
  } 

  for (int smoothgraph_fill(0); smoothgraph_fill < m_smoothedBremFit->GetnSurfaces(); smoothgraph_fill++) {
    m_smooth_1overP->push_back(m_smoothedBremFit->Get1overP(smoothgraph_fill));
    m_smooth_1overPerr->push_back(m_smoothedBremFit->Get1overPerror(smoothgraph_fill));
    m_smooth_value->push_back(m_smoothedBremFit->GetValue(smoothgraph_fill));
  } 

  m_forwardparameter_constant = m_forwardparameters.constant;
  m_smoothparameter_constant = m_smoothedparameters.constant;
  
  //Calculate the SeparationQuality

  SeparationQuality(m_smoothed_kink,m_forward_kink);

  if (m_validationTree && !m_Z_mode)
    m_validationTree->Fill();
  else if (m_validationTree2 && m_Z_mode)
    m_validationTree2->Fill();
  
  delete m_brem_value;
  delete m_brem_phi;
  delete m_brem_theta;
  delete m_brem_energy;
  delete m_brem_UpperBound;
  delete m_brem_LowerBound;
  delete m_forward_kink;
  delete m_smoothed_kink;
  delete m_brem_significance;
  delete m_brem_valueCalibrated;
  delete m_surfaceX;
  delete m_surfaceY;
  delete m_surfaceZ;
  
  delete m_forwardBremFit;
  delete m_smoothedBremFit;


  delete m_forwardparameter_coefficient;
  delete m_forwardparameter_value;
  delete m_forwardparameter_width;
  delete m_smoothparameter_coefficient;
  delete m_smoothparameter_value;
  delete m_smoothparameter_width;
  delete m_forward_1overP;
  delete m_forward_1overPerr;
  delete m_forward_value;
  delete m_smooth_1overP;
  delete m_smooth_1overPerr;
  delete m_smooth_value;
  delete m_KinkSeparationScores;
  delete m_KinkSeparationScoresErr;

  
}

void Trk::BremFind::CombineParameters()
{
  //Firstly clear all previous entries
  m_combinedparameters.coefficient.clear();
  m_combinedparameters.width.clear();
  m_combinedparameters.value.clear();

  //The combined graph is the backward fit - forward fit
  m_combinedparameters.constant = m_smoothedparameters.constant - m_forwardparameters.constant;
  
  for (int forward_counter(0); forward_counter < (int) m_forwardparameters.value.size(); forward_counter++) {
    m_combinedparameters.coefficient.push_back(-m_forwardparameters.coefficient[forward_counter]);
    m_combinedparameters.value.push_back(m_forwardparameters.value[forward_counter]);
    m_combinedparameters.width.push_back(m_forwardparameters.width[forward_counter]);
  }

  for (int smooth_counter(0); smooth_counter < (int) m_smoothedparameters.value.size(); smooth_counter++) {
    m_combinedparameters.coefficient.push_back(m_smoothedparameters.coefficient[smooth_counter]);
    m_combinedparameters.value.push_back(m_smoothedparameters.value[smooth_counter]);
    m_combinedparameters.width.push_back(m_smoothedparameters.width[smooth_counter]);
  }

}


//This function returns the value of the tanh series at position x
double Trk::BremFind::Graph_value(GraphParameter graphparameter, double x)
{
  double arg = graphparameter.constant;

  for (int arg_counter(0); arg_counter < (int) graphparameter.value.size(); arg_counter++) {
    arg+=graphparameter.coefficient[arg_counter]*TMath::TanH(x/graphparameter.width[arg_counter] - graphparameter.value[arg_counter]/graphparameter.width[arg_counter]);
  }
  return arg;

}

//Returns the x value of the local maximum between the min and max terms
double Trk::BremFind::GetMaximumX(GraphParameter graphparameter, double min, double max)
{
  double value(0);
  double maximum(0);
  double eval(0);
  for (double value_counter(min); value_counter < max; value_counter++) {
    eval=Graph_value(graphparameter, value_counter);
   
    if (eval > maximum) {
      maximum=eval;
      value=value_counter;
    }
      
  }
  return value;
}

//Returns the y value of the local maximum between the min and max terms
double Trk::BremFind::GetMaximumY(GraphParameter graphparameter, double min, double max)
{
  double maximum(0);
  double eval(0);
  for (double value_counter(min); value_counter < max; value_counter++) {
    eval=Graph_value(graphparameter, value_counter);
    if (eval > maximum) {
      maximum = eval;
    }
  }
  return maximum;
}


void Trk::BremFind::GetBremLocations()
{
  bool double_back_boolean=false;
  //Normalisation factor introduced because initial parameters were chosen and optimised
  //for 10GeV Pt electrons. So the normalization factor for 10GeV events will be 1.0
  
  double P_T(sin(m_perigee_Theta)*(1/(m_perigee_1overP)));

  double normalisation(P_T/10.0);

  for (int smooth_counter(0); smooth_counter < (int) m_smoothedparameters.value.size(); smooth_counter++) {
    double_back_boolean=false;

    for (int forward_counter(0); forward_counter < (int) m_forwardparameters.value.size(); forward_counter++) {
      
      //backward kink must have a value lower than the forward kink
      if (m_forwardparameters.value[forward_counter] < m_smoothedparameters.value[smooth_counter]) {
	continue;
      }
      
      //If there are multiple back kinks before the forward kink, then the back kinks must
      //iterate forward until it's just before the forward kink
      //Therefore the forward loop is broken to iterate the smooth_counter
      if ((smooth_counter+1 < (int) m_smoothedparameters.value.size()) ) {
	if (m_forwardparameters.value[forward_counter] > m_smoothedparameters.value[smooth_counter+1] && m_smoothedparameters.coefficient[smooth_counter+1]/sin(m_perigee_Theta) > (MIN_KINK_THRESHOLD/normalisation)) {
	  double_back_boolean=true;
	}
      }
      
      if (double_back_boolean) {
	break;
      }
    
      //If forward kink is just in front of the backward kink then proceed to find the brem value
      else if (m_forwardparameters.value[forward_counter] > m_smoothedparameters.value[smooth_counter]) {
	
	double brem_separation( GetMaximumY(m_combinedparameters, m_smoothedparameters.value[smooth_counter], m_forwardparameters.value[forward_counter])/sin(m_perigee_Theta) );
	

	//Calculates the energy of the brem based on the change in momentum in the 
	//1overP parameter
	double coefficient_average( (m_forwardparameters.coefficient[forward_counter]+m_smoothedparameters.coefficient[smooth_counter])/(2*m_perigee_Theta) );
	double p_before_bremF( Graph_value(m_forwardparameters, m_forwardparameters.value[forward_counter] - 2*m_forwardparameters.width[forward_counter])/m_perigee_Theta );
	double p_before_bremS( Graph_value(m_smoothedparameters, m_smoothedparameters.value[smooth_counter] - 2*m_smoothedparameters.width[smooth_counter])/m_perigee_Theta );
	double p_before_bremAverage( (p_before_bremF + p_before_bremS)/2 );
	double p_after_brem( p_before_bremAverage + 2*coefficient_average );
	double brem_amplitude( 1/p_before_bremAverage - 1/p_after_brem );
	
	
	//Check the strength of the brem and its coefficients
	if (brem_amplitude > (BREM_AMPLITUDE*normalisation) && brem_separation > (BREM_SEPARATION/normalisation) && m_forwardparameters.coefficient[forward_counter]/sin(m_perigee_Theta) > (COEFFICIENT_THRESHOLD/normalisation) && m_smoothedparameters.coefficient[smooth_counter]/sin(m_perigee_Theta) > (COEFFICIENT_THRESHOLD/normalisation)) {
	  
	  m_brem_energy->push_back(brem_amplitude);
	  m_brem_value->push_back( GetMaximumX(m_combinedparameters,m_smoothedparameters.value[smooth_counter],m_forwardparameters.value[forward_counter]) );
	  m_forward_kink->push_back(m_forwardparameters.value[forward_counter]);
	  m_smoothed_kink->push_back(m_smoothedparameters.value[smooth_counter]);
	  
	  break;
	}	
	
      }
	
    }

  }

}

//This function obtains the upper and lower bounds of the brem value by scanning the peak
//till there's a drop in magnitude equal to the desired threshold. If type=1 then, this
//function will return an upperbound. If type=-1, then this function will return the lower
//bound. Threshold must be used as a percentage in drop from the maximum Y value.
double Trk::BremFind::GetBound(double start, int type)
{
  double maximum(Graph_value(m_combinedparameters,start));
  double eval(maximum);
  double value(start);
  
  while (eval > ((100.0-BOUND_THRESHOLD)/100.0)*maximum) {
    eval = Graph_value(m_combinedparameters,value);
    value+=type;

    //if the value is above the end of the inner detector
    if (value > m_smoothedBremFit->LastFitMeasurement()) {
      return m_smoothedBremFit->LastFitMeasurement();
    }
    //if the value is below zero, then return 0 as the lower bound
    if (value < 0.0) {
      return 0.0;
    }
  }
  
  return value;
}

int Trk::BremFind::ClosestMeasurement(double x, QoverPBremFit *BremFit)
{
  double min_distance(1000);
  double distance(1000);
  //double value(x);
  int measurement_no(0);

  for (int counter(0); counter < BremFit->GetNMeasurements(); counter++) {
    distance = fabs(x - BremFit->GetValue(counter));
    if (distance < min_distance) {
      min_distance = distance;
      //value = BremFit->GetValue(counter);
      measurement_no = counter;
    } 
  }
  return measurement_no;
}

double Trk::BremFind::BremSignificance(double x,QoverPBremFit *forwardBremFit, QoverPBremFit *smoothedBremFit)
{
  int measurement_noF( ClosestMeasurement(x, forwardBremFit) );
  double PiF( 1/forwardBremFit->Get1overP(measurement_noF) );

  //Making sure the next measurement is not on the same surface
  int additional_forward(1);
  double value1F( forwardBremFit->GetValue(measurement_noF+1) );
  while (fabs(x-value1F) < 20.0) {
    additional_forward++;
    value1F = forwardBremFit->GetValue(measurement_noF + additional_forward);
  }
    double Pi1F( 1/forwardBremFit->Get1overP(measurement_noF+1) );


  int measurement_noS( ClosestMeasurement(x, smoothedBremFit) );
  double PiS( 1/smoothedBremFit->Get1overP(measurement_noS) );

  int additional_smoothed(1);
  double value1S( smoothedBremFit->GetValue(measurement_noS-1) );
  while (fabs(x-value1S) < 20.0){
    additional_smoothed++;
    value1S = smoothedBremFit->GetValue(measurement_noS - additional_smoothed);
  }
  double Pi1S( 1/smoothedBremFit->Get1overP(measurement_noS-1) );

  double sigf( (1-(Pi1F/PiF))/forwardBremFit->Get1overPerror(measurement_noF) );
  double sigb( (1-(PiS/Pi1S))/smoothedBremFit->Get1overPerror(measurement_noS) );

  double sigsum( sigf + sigb);

  return std::max(sigsum, std::max(sigf,sigb));
}

//This function provides an estimate of the phi position using the value
//The smoothed trajectory is used because it is more accurate in the pixels
//where the majority of brems occur
double Trk::BremFind::GetPhiFromValue(double value, QoverPBremFit* smoothedBremFit)
{
  int closest_measurement_no( ClosestMeasurement(value, smoothedBremFit) );
  
  double value_difference;
  double phi_difference;
  
  if (closest_measurement_no == 0) {
    value_difference = smoothedBremFit->GetValue(closest_measurement_no) - smoothedBremFit->GetValue(closest_measurement_no + 1);
    phi_difference = smoothedBremFit->GetPhi(closest_measurement_no) - smoothedBremFit->GetPhi(closest_measurement_no + 1);
  }

  else {
    value_difference = smoothedBremFit->GetValue(closest_measurement_no) - smoothedBremFit->GetValue(closest_measurement_no - 1);
    phi_difference = smoothedBremFit->GetPhi(closest_measurement_no) - smoothedBremFit->GetPhi(closest_measurement_no - 1);
  }

  double gradient( phi_difference/value_difference );
  double estimated_phi;
  if (closest_measurement_no == 0) 
    estimated_phi = smoothedBremFit->GetPhi(closest_measurement_no+1) + gradient*(value - smoothedBremFit->GetValue(closest_measurement_no + 1));
  else
    estimated_phi = smoothedBremFit->GetPhi(closest_measurement_no-1) + gradient*(value - smoothedBremFit->GetValue(closest_measurement_no - 1)); 
			 
  return estimated_phi;

}

void Trk::BremFind::CalibrateValue()
{     
  double new_value(0);
  double max_value_measurement(m_smoothedBremFit->LastFitMeasurement());
  double min_value_measurement(m_smoothedBremFit->FirstFitMeasurement());
 
        
  for (int counter(0); counter < (int) m_brem_value->size(); counter++) {
      
    double smoothed_kink((*m_smoothed_kink)[counter]);
    double forward_kink((*m_forward_kink)[counter]);
    new_value = ((max_value_measurement - min_value_measurement)-((*m_brem_value)[counter]-min_value_measurement))*smoothed_kink/(max_value_measurement-min_value_measurement);
    new_value += ((*m_brem_value)[counter]-min_value_measurement)*forward_kink/(max_value_measurement-min_value_measurement);
    m_brem_valueCalibrated->push_back( new_value );
  }
}


//This function uses the closest measurement trackparameter and propagates to the brem surface
//and returns the global position at the intersection. It also fills a vector of trackparameters
//at the brem surface.
Amg::Vector3D Trk::BremFind::SurfacePosition(const Trk::TrackParameters& trackparameter, const Trk::Surface& surface, double bremvalue, double bremphi, double bremtheta)
{
  const Trk::TrackParameters* surfaceParameters1;
  const Trk::TrackParameters* surfaceParameters2;

  Amg::Vector3D Pos1 (0, 0, 0);
  Amg::Vector3D Pos2 (0, 0, 0);
  double phidifference1(0.), phidifference2(0.);

  //Pushing back just the global position of the brem not the extrapolated
  double x,y,z,r;
  if (m_Z_mode) {
    x = bremvalue * tan(bremtheta) * cos(bremphi);
    y = bremvalue * tan(bremtheta) * sin(bremphi);
    z = bremvalue;
    r = sqrt(x*x+y*y);
  }
  else {
    x = bremvalue * cos(bremphi);
    y = bremvalue * sin(bremphi);
    z = bremvalue * cos(bremtheta);
    r = bremvalue;
  }
  
  const Amg::Vector3D position(x,y,z);
  const Amg::Vector3D momentum((&trackparameter)->momentum());

  ATH_MSG_DEBUG("x,y,z,r = " << x << "," << y << "," << z << "," << r);
  ATH_MSG_DEBUG("momentum = " << momentum.mag());
  ATH_MSG_DEBUG("charge   = " << (&trackparameter)->charge());

  if (!m_usePropagate && momentum.mag() > 0) {

    Amg::Transform3D *trans = new Amg::Transform3D(trackparameter.associatedSurface().transform());
    Trk::CylinderSurface cylinderSurface(trans, r, 5000.0);
    const Trk::TrackParameters *bremParameters = new AtaCylinder(position, momentum, (&trackparameter)->charge(), cylinderSurface);

    //Broken code - causes FPEs.
    //Trk::CylinderSurface cylinderSurface(new Amg::Transform3D, r, 5000.0);

    m_brem_TrackParameters.push_back(bremParameters);
    return position;
  }

  //function will propagate both ways and return the trackparameter that has the closest phi
  //to the measured brem phi angle
  surfaceParameters1 = m_propagator->propagate(trackparameter,surface,Trk::oppositeMomentum,true,m_fieldProperties,electron);
  if (surfaceParameters1) {
    Pos1 = surfaceParameters1->position();
    double phi1(atan(Pos1.y()/Pos1.x()));
    phidifference1 = UniqueAngle(phi1 - bremphi);
  }
  surfaceParameters2 = m_propagator->propagate(trackparameter,surface,Trk::alongMomentum,true,m_fieldProperties,electron);
  if (surfaceParameters2) {
    Pos2 = surfaceParameters2->position();
    double phi2((Pos2.y()/Pos2.x()));
    phidifference2 = UniqueAngle(phi2 - bremphi);
  }
  if (surfaceParameters1 && !surfaceParameters2) {
    m_brem_TrackParameters.push_back(surfaceParameters1);
    return Pos1;
  }
  else if (!surfaceParameters1 && surfaceParameters2) {
    m_brem_TrackParameters.push_back(surfaceParameters2);
    return Pos2;
  }
  else if (surfaceParameters1 && surfaceParameters2) {
    if (phidifference1 > phidifference2) {
      m_brem_TrackParameters.push_back(surfaceParameters2);
      return Pos2;
    }
    else {
      m_brem_TrackParameters.push_back(surfaceParameters1);
    return Pos1;
    }
    
  }
  
  //backup method. This really shouldn't be called
  const Trk::TrackParameters* surfaceParameters3 = m_propagator->propagate(trackparameter,surface,Trk::anyDirection,true,m_fieldProperties,electron);
  if (surfaceParameters3) {
    m_brem_TrackParameters.push_back(surfaceParameters3);
    return surfaceParameters3->position();
  }
  m_brem_TrackParameters.push_back(0);
  Amg::Vector3D nullPos(0.0,0.0,0.0);
  return nullPos;
}

const Trk::Surface* Trk::BremFind::ClosestSurface(double, double, double, const Trk::TrackParameters& trackparameter)
{
  const Trk::Layer* associatedLayer = m_trackingGeometry->closestMaterialLayer<const Trk::TrackParameters>(trackparameter).object;      
  if (associatedLayer) {
    m_brem_layers.push_back(associatedLayer);
    const Trk::Surface* associatedSurface = &(associatedLayer->surfaceRepresentation());
    return associatedSurface;
  }
  else {
    m_brem_layers.push_back(0);
  }
  
  return 0;
}


StatusCode Trk::BremFind::retrieveTrackingGeometry()
{
  // Retrieve the TrackingGeometry from the DetectorStore
  StatusCode sc = detStore()->retrieve(m_trackingGeometry, m_trackingGeometryName);
  if (sc.isFailure()){
    msg(MSG::FATAL) << "Could not retrieve TrackingGeometry from DetectorStore!" << endmsg;
  }
  return sc;

}

double Trk::BremFind::UniqueAngle(double phi)
{
  if (phi > TMath::Pi()) {
    return UniqueAngle(phi - TMath::Pi());
  }
  else if (phi < 0.0) {
    return UniqueAngle(phi + TMath::Pi());
  }
  return phi;
}

int Trk::BremFind::GetNBrems()
{
  return m_nBrems;
}

void Trk::BremFind::MakeTrackStateOnSurfaces ()
{
 
  for (int make_counter(0); make_counter < m_nBrems; make_counter++) {
    
		std::bitset<TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
		typePattern.set(TrackStateOnSurface::BremPoint);
		const Trk::TrackStateOnSurface* BremTrackStateOnSurface = new Trk::TrackStateOnSurface(0,m_brem_TrackParameters[make_counter],0,m_EstimatedBremOnTrack[make_counter],typePattern);

    if (!m_Z_mode)
      m_brem_trackStateOnSurface.push_back(BremTrackStateOnSurface);
    else
      delete BremTrackStateOnSurface;
    
  }

}

const Trk::TrackStateOnSurface* Trk::BremFind::GetEstimatedBremOnTrack(int n)
{
  return m_brem_trackStateOnSurface[n];
}


//A function that rates the quality of a bremsstrahlung signal based on the separation quality
//of the forward and smoothed fit. A lower score indicates that the measurements are unlikely to
//be the same value. Therefore a lower score means a better fit.

void Trk::BremFind::SeparationQuality(std::vector<double> *backwardkink, std::vector<double> *forwardkink) 
{
  int ForwardHit_Counter(0);
  int SmoothHit_Counter(1);
  std::vector<double> OverlapScore(0);
  std::vector<double> OverlapErr(0);
  
  
  for (int kink_counter(0); kink_counter < (int) backwardkink->size(); kink_counter++) {
    
    OverlapScore.clear();
    OverlapErr.clear();

    ForwardHit_Counter = 0;
    SmoothHit_Counter = 1;

    for (; ForwardHit_Counter < (int) m_forward_1overP->size(); ForwardHit_Counter++,SmoothHit_Counter++) {
      
      
      if ( (*m_forward_value)[ForwardHit_Counter] > (*backwardkink)[kink_counter] && (*m_forward_value)[ForwardHit_Counter] < (*forwardkink)[kink_counter] && (*m_smooth_value)[SmoothHit_Counter] > (*backwardkink)[kink_counter] && (*m_smooth_value)[SmoothHit_Counter] < (*forwardkink)[kink_counter]) {
	
	std::pair<double,double> probabilitypair(ProbabilityScore((*m_forward_1overP)[ForwardHit_Counter],3*(*m_forward_1overPerr)[ForwardHit_Counter],(*m_smooth_1overP)[SmoothHit_Counter],3*(*m_smooth_1overPerr)[SmoothHit_Counter]));
	OverlapScore.push_back(probabilitypair.first);
	OverlapErr.push_back(probabilitypair.second);
	
      }
      
    }

    //Now combing the multiple measurements into one score
    
    double sum1(0);
    double sum2(0);
    double LowestScore(1.0);
    double LowestScoreErr(0.0);

    for(int score_counter(0); score_counter < (int) OverlapScore.size(); score_counter++) {
      
      sum1 += OverlapScore[score_counter]/(pow(OverlapErr[score_counter],2));
      sum2 += 1/(pow(OverlapErr[score_counter],2));

      if (OverlapScore[score_counter] < LowestScore || score_counter==0) {
	LowestScore = OverlapScore[score_counter];
	LowestScoreErr = OverlapErr[score_counter];
      }
    }

    if (sum2==0) {
      m_KinkSeparationScores->push_back(1.0);
      m_KinkSeparationScoresErr->push_back(0.0);
    }
    else {
      m_KinkSeparationScores->push_back(sum1/sum2);
      m_KinkSeparationScoresErr->push_back(LowestScoreErr);
      //m_KinkSeparationScoresErr->push_back(1/sqrt(sum2));
    }
  
  }

}


std::pair<double,double> Trk::BremFind::ProbabilityScore(double Gauss1_mean, double Gauss1_var, double Gauss2_mean, double Gauss2_var) 
{
  double value(exp(-(Gauss1_mean - Gauss2_mean)*(Gauss1_mean - Gauss2_mean)/(2*(Gauss1_var*Gauss1_var + Gauss2_var*Gauss2_var)))*(1/Gauss1_var*Gauss2_var*2*sqrt(TMath::Pi()))*sqrt(2*Gauss1_var*Gauss1_var*Gauss2_var*Gauss2_var/(Gauss1_var*Gauss1_var + Gauss2_var*Gauss2_var)));
  double error(value*(Gauss1_mean - Gauss2_mean)*(Gauss1_var + Gauss2_var)/(Gauss1_var*Gauss1_var + Gauss2_var*Gauss2_var));

  std::pair<double,double> probabilityscore;
  probabilityscore.first = value;
  probabilityscore.second = fabs(error);

  return probabilityscore;
  
}
