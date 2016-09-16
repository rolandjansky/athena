/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************************
                      QoverPBremFit.cxx * description

completed   : Tuesday 18th November 2008
author      : Tony Shao
email       : Qi.Tao.Shao@cern.ch
description : Class called by BremFind to fit 1overp vs R plots using a sum of Tanh graphs
**********************************************************************************/
#include "QoverPBremFit.h"

#include "TrkGaussianSumFilter/IMultiComponentStateCombiner.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkFitterUtils/FitterTypes.h"

#include <math.h>
#include <algorithm>
#include "TMath.h"



Trk::QoverPBremFit::QoverPBremFit(int type, bool Z_mode):
  m_stateCombiner("Trk::MultiComponentStateCombiner")
{  
  m_potentialvalue = new std::vector<double>;
  m_potentialwidth = new std::vector<double>;
  m_1overPvalue = new std::vector<double>;
  m_1overPvalueerror = new std::vector<double>;
  m_PhiValue = new std::vector<double>;
  m_PhiValueerror = new std::vector<double>;
  m_ThetaValue = new std::vector<double>;
  m_ThetaValueerror = new std::vector<double>;
  m_RadiusValue = new std::vector<double>;
  m_Zvalue = new std::vector<double>;
  m_FitValue = new std::vector<double>;
  m_trackParameters = new std::vector<const Trk::TrackParameters*>;
  m_type = type;
  m_Z_mode = Z_mode;
  m_surfaceCounter = 0;  
  m_charge =0;
}

StatusCode Trk::QoverPBremFit::initialize()
{
  // Request the state combiner
  if ( m_stateCombiner.retrieve().isFailure() ){
    printf("Failed to retrieve m_stateCombiner");
    return StatusCode::FAILURE;
  }  
  return StatusCode::SUCCESS;
}


Trk::QoverPBremFit::~QoverPBremFit()
{
	for (unsigned int d(0);d < m_trackParameters->size(); d++) {
		delete (*m_trackParameters)[d];
	}
	
  delete m_potentialvalue;
  delete m_potentialwidth;
  delete m_1overPvalue;
  delete m_1overPvalueerror;
  delete m_PhiValue;
  delete m_PhiValueerror;
  delete m_ThetaValue;
  delete m_ThetaValueerror;
  delete m_RadiusValue;
  delete m_Zvalue;
  delete m_FitValue;
	delete m_trackParameters;
}



Trk::GraphParameter Trk::QoverPBremFit::GetParameters(const DataVector <const Trk::TrackStateOnSurface>& Trajectory)
{
  FillVariables(Trajectory);
  
  for (int copy_counter(0); (int) copy_counter < GetNMeasurements(); copy_counter++) {
    if (m_Z_mode)
      m_FitValue->push_back(fabs((*m_Zvalue)[copy_counter]));
    else
      m_FitValue->push_back((*m_RadiusValue)[copy_counter]);
  }
    

  FindPotentials();
  
  m_graphparameter=FindParameters();
  
  m_mergedparameter = MergeParameters(m_graphparameter);

   
  return m_mergedparameter;
}


void Trk::QoverPBremFit::FillVariables(const DataVector <const Trk::TrackStateOnSurface>& Trajectory) {

  DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurface;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator Trajectory_end; 
  

  if (m_type==1) {
    trackStateOnSurface = Trajectory.begin();
    Trajectory_end = Trajectory.end(); 
  }
  else if (m_type==-1) {
    trackStateOnSurface = Trajectory.end();
    trackStateOnSurface--;
    Trajectory_end = Trajectory.begin();
    Trajectory_end--;
  }
  

  m_surfaceCounter=0;
  
  
  for ( ; trackStateOnSurface != Trajectory_end; )  {
    const Trk::MultiComponentState *TrajectoryMultiState=0;
    const Trk::MultiComponentStateOnSurface *TrajectoryMultiStateOnSurface = dynamic_cast<const Trk::MultiComponentStateOnSurface*>(*trackStateOnSurface);

    if (m_surfaceCounter < TRKFGSF_VALSURFACES) {
      
      if (!TrajectoryMultiStateOnSurface) {
	// Create new multiComponentState from a single state
	Trk::ComponentParameters componentParameters( (*trackStateOnSurface)->trackParameters(), 1.0);
	TrajectoryMultiState = new Trk::MultiComponentState(componentParameters);
      }
      else
	TrajectoryMultiState = TrajectoryMultiStateOnSurface->components();

      const Amg::Vector3D posOnSurf = TrajectoryMultiStateOnSurface->trackParameters()->position();
      

      //Fill the radius values
      m_RadiusValue->push_back( posOnSurf.perp() );
      m_Zvalue->push_back( posOnSurf.z() );

      //Specify in joboptions whether to use mode or mean
      const Trk::TrackParameters* CombinedMultiState = m_stateCombiner->combine(*TrajectoryMultiState,true);
      
      if (CombinedMultiState->parameters()[Trk::qOverP] > 0.0) 
	m_charge = 1.0;    
      else 
	m_charge = -1.0;

      //Fill the 1overPvalues in GeV
      m_trackParameters->push_back( CombinedMultiState );
      m_1overPvalue->push_back( 1000.0*fabs(CombinedMultiState->parameters()[Trk::qOverP]) );
      m_PhiValue->push_back( CombinedMultiState->parameters()[Trk::phi] );
      m_ThetaValue->push_back( CombinedMultiState->parameters()[Trk::theta] );

      const AmgSymMatrix(5)* measuredCov = CombinedMultiState->covariance();

      if (measuredCov) {
	m_1overPvalueerror->push_back( 1000.0*sqrt((*measuredCov)(Trk::qOverP,Trk::qOverP)));
	m_PhiValueerror->push_back( sqrt((*measuredCov)(Trk::phi,Trk::phi)));
	m_ThetaValueerror->push_back( sqrt((*measuredCov)(Trk::theta,Trk::theta)));
      }
      else {
	m_1overPvalueerror->push_back( 1000.0*fabs(CombinedMultiState->parameters()[Trk::qOverP]) );
	m_PhiValueerror->push_back( fabs(CombinedMultiState->parameters()[Trk::phi]) );
	m_ThetaValueerror->push_back( fabs(CombinedMultiState->parameters()[Trk::theta]) );
      }
          
    }
    
    if (!TrajectoryMultiStateOnSurface) { 
      delete TrajectoryMultiState;
    }
    m_surfaceCounter++;
    if (m_type==1) {
      trackStateOnSurface++;
    }
    else if (m_type==-1) {
      trackStateOnSurface--;
    }
  }

}


void Trk::QoverPBremFit::FindPotentials()
{
  double width;
  double oneoverPgradient;

  //Search for potential brem locations
  for (int datapoint=0; datapoint < m_surfaceCounter-1; datapoint++) {
    //Calculates gradient between adajacent points
    oneoverPgradient=(1 - (*m_1overPvalue)[datapoint]/(*m_1overPvalue)[datapoint+1])/((*m_FitValue)[datapoint+1]-(*m_FitValue)[datapoint]);

    if (oneoverPgradient > POTENTIAL_BREM_THRESHOLD) {
      
      m_potentialvalue->push_back(((*m_FitValue)[datapoint+1]+(*m_FitValue)[datapoint])/2);
      width=((*m_FitValue)[datapoint+1]-(*m_FitValue)[datapoint])/2;

      if (width < 20) {
	width=20;
      }
      
      m_potentialwidth->push_back(width);
      
    }
  }   
}


Trk::GraphParameter Trk::QoverPBremFit::FindParameters()
{
  
  GraphParameter graphparameter;
  int nparameters = (int) m_potentialvalue->size() + 1;
  int totalparameters = nparameters;
  int nsurfaces = m_1overPvalue->size();


  std::vector<Element> elements;
  elements.clear();
  Element thePoint;

  //Fill in the elements
  for (int elementfill=0; elementfill < totalparameters-1; elementfill++) {

    thePoint.value=(*m_potentialvalue)[elementfill];
    thePoint.width=(*m_potentialwidth)[elementfill];
    thePoint.sign=true;
    elements.push_back(thePoint);
  }

  while (true) {
    //printf("Starting the matrix iterations\n");
    Amg::VectorX theta(nparameters); theta.setZero();
    Amg::VectorX ymatrix(nsurfaces); ymatrix.setZero();
    Amg::MatrixX Amatrix (nsurfaces,nparameters); Amatrix.setZero();
    Amg::MatrixX Gy (nsurfaces,nsurfaces); Gy.setZero();

    //temporary matrices to store the operations
    Amg::MatrixX ATranspose(nparameters,nsurfaces); ATranspose.setZero();
    Amg::MatrixX AGA(nparameters,nparameters); AGA.setZero();
    Amg::MatrixX AGAinverse(nparameters,nparameters); AGAinverse.setZero();

    //printf("nparameters=%i\n",nparameters);


    //Filling each of the matrices --- y matrix first
    for (int yfill=0; yfill < nsurfaces; yfill++) {
      ymatrix[yfill]=(*m_1overPvalue)[yfill];
    }


    //Filling the A matrix.
    for (int afill=0; afill < nsurfaces; afill++) {

      int afill3(0);

      //afill2 starts at -1 because we want the elements vector to start at zero.
      //The first iteration is taken away by the constant parameters.
      for (int afill2=-1; afill2 < totalparameters-1; afill2++) {

        //First column of the A matrix is a constant
        if (afill3==0){
          Amatrix(afill,afill3)=1;
          afill3++;
        }

        //Only the terms with positive coefficients are kept
        else if (elements[afill2].sign) {

          Amatrix(afill,afill3)=TMath::TanH((*m_FitValue)[afill]/elements[afill2].width-elements[afill2].value/elements[afill2].width);
          afill3++;
        }

      }
    }


    //Fill the Gy matrix (the error matrix)
    for (int gfill(0); gfill < nsurfaces; gfill++) {

      for (int gfill2(0); gfill2 < nsurfaces; gfill2++) {

        if (gfill==gfill2) {
          Gy(gfill,gfill2)=1/(*m_1overPvalueerror)[gfill];
        }
        else {
          Gy(gfill,gfill2)=0;
        }	  
      }
    }



    //int inv;
    ATranspose=Amatrix.transpose();
    AGA=ATranspose*(Gy*(Amatrix));
    AGAinverse=AGA.inverse();
    //printf("Inv=%i\n",inv);

    theta=AGAinverse*(ATranspose*(Gy*ymatrix));


    //Check the signs of the calculated coefficients. Negative coefficients will be taken away
    //on the next iteration until all coefficients are >= 0

    int nNegative(0);

    int signcheck2(1);  //Iterations start at 1 because the constant parameter doesn't matter
    for (int signcheck(0); signcheck < totalparameters-1; signcheck++) {

      if (!elements[signcheck].sign) { //if the term is already taken away, then ignore it
        continue;
      }
      else if (theta[signcheck2] < 0) { //checks to the sign of the calculated coefficient
        elements[signcheck].sign=false;
        nNegative++;  //counts the number of negative coefficients
        signcheck2++;
      }
      else {
        signcheck2++;
      }
    }

    if (nNegative==0) {//Ends the iterations because all terms are positive

      int graphfill2(0);
      for(int graphfill(-1); graphfill < totalparameters-1; graphfill++) {
        if (graphfill2==0) { //Fills the constant with the corresponding theta element
          graphparameter.constant=theta[graphfill2];
          if (isnan(graphparameter.constant))
            graphparameter.constant=0.0;
          graphfill2++;
        }

        else if (!elements[graphfill].sign) { 
          //if the parameter is negative then its coefficient is assigned zero
          graphparameter.value.push_back(elements[graphfill].value);
          graphparameter.coefficient.push_back(0.0);
          graphparameter.width.push_back(20.0);
        }

        else {
          //fills the graphparameter with the coefficients, radii and width
          if (isnan(theta[graphfill2])) {
            graphparameter.coefficient.push_back(0.0);
            graphparameter.value.push_back(elements[graphfill].value);
            graphparameter.width.push_back(elements[graphfill].width);
          }
          else {
            graphparameter.value.push_back(elements[graphfill].value);
            graphparameter.coefficient.push_back(theta[graphfill2]);
            graphparameter.width.push_back(elements[graphfill].width);
          }
          graphfill2++;
        }
      }

      break;

    }
    else {
      nparameters-=nNegative; //resets the matrices with less degrees of freedom
    }
  }

  return graphparameter;
}     


Trk::GraphParameter Trk::QoverPBremFit::MergeParameters(GraphParameter graphparameter) 
{
  GraphParameter MergedParameters;
  MergedParameters.constant=graphparameter.constant;

  double joinscore(0);
  std::vector<int> nonzero_coefficients;  //contains the coefficient numbers that are non-zero


  for (int coeff_test(0); coeff_test < (int) graphparameter.coefficient.size(); coeff_test++) {
    if ( graphparameter.coefficient[coeff_test] != 0) {
      nonzero_coefficients.push_back(coeff_test);
    }
  }

  //Returns the singular case
  if ( (int) nonzero_coefficients.size() == 1 ) {
    MergedParameters.coefficient.push_back(graphparameter.coefficient[nonzero_coefficients[0]]);
    MergedParameters.value.push_back(graphparameter.value[nonzero_coefficients[0]]);
    MergedParameters.width.push_back(graphparameter.width[nonzero_coefficients[0]]);
    return MergedParameters;
  }

  std::vector<bool> joinboolean ((int) nonzero_coefficients.size()-1,false);
  double kinkdistance; //distance between adjacent kinks
  double maxmeasure;   //the maximum distance between measurements between the adjacent kinks
  //int npotential;     //number of potential brem points between kinks
  int nmeasurement;   //number of measurements between kinks
  double regionbias;   //takes into account the joining bias of the region based on value  

  //The first element of the joinboolean array will represent whether 
  //kink #1 and kink#2 should be joined 

  for (int join_counter(0); join_counter < (int) nonzero_coefficients.size()-1; join_counter++) {

    joinscore=0;

    kinkdistance = graphparameter.value[nonzero_coefficients[join_counter+1]] - graphparameter.value[nonzero_coefficients[join_counter]];
    maxmeasure = maxvalue(graphparameter.value[nonzero_coefficients[join_counter]],graphparameter.value[nonzero_coefficients[join_counter+1]]);
    //npotential = npotentials(graphparameter.value[nonzero_coefficients[join_counter]],graphparameter.value[nonzero_coefficients[join_counter+1]]);
    nmeasurement = nmeasurements(graphparameter.value[nonzero_coefficients[join_counter]],graphparameter.value[nonzero_coefficients[join_counter+1]]);
    regionbias = (LastFitMeasurement()/2-graphparameter.value[nonzero_coefficients[join_counter+1]]+LastFitMeasurement()/2-graphparameter.value[nonzero_coefficients[join_counter]])/5;

    joinscore+= -10*nmeasurement;
    joinscore+= 2*maxmeasure;
    joinscore+= 50-kinkdistance;
    joinscore+= m_type*regionbias;
    //joinscore+= 20*npotential;

    //printf("Join Score%i=%f\n",join_counter,joinscore);

    //Scoring system thresholds
    if (kinkdistance > 350) {
      joinboolean[join_counter] = false;
      continue;
    }

    if (joinscore > 0) {
      joinboolean[join_counter] = true;
    }

    else {
      joinboolean[join_counter] = false;
    }

  }

  //Use the joinboolean array to join multiple smaller kinks into one larger one

  int nkinks_merged(0);

  std::vector<double> coefficient_vector;
  std::vector<double> width_vector;
  std::vector<double> value_vector;

  for (int merge_counter(0); merge_counter < (int) nonzero_coefficients.size(); merge_counter++) {

    if (merge_counter < (int) nonzero_coefficients.size()-1) {

      if (joinboolean[merge_counter]) {
        coefficient_vector.push_back(graphparameter.coefficient[nonzero_coefficients[merge_counter]]);
        width_vector.push_back(graphparameter.width[nonzero_coefficients[merge_counter]]);
        value_vector.push_back(graphparameter.value[nonzero_coefficients[merge_counter]]);
        nkinks_merged++;
      }

      else if (!joinboolean[merge_counter]) {
        coefficient_vector.push_back(graphparameter.coefficient[nonzero_coefficients[merge_counter]]);
        value_vector.push_back(graphparameter.value[nonzero_coefficients[merge_counter]]);
        width_vector.push_back(graphparameter.width[nonzero_coefficients[merge_counter]]);
        MergedParameters.coefficient.push_back(coefficient_sum(coefficient_vector));
        MergedParameters.value.push_back(value_sum(value_vector,coefficient_vector));
        MergedParameters.width.push_back(width_sum(width_vector,coefficient_vector,value_vector));
        coefficient_vector.clear();
        value_vector.clear();
        width_vector.clear();
        nkinks_merged=0;
      }
    }

    else {
      coefficient_vector.push_back(graphparameter.coefficient[nonzero_coefficients[merge_counter]]);
      value_vector.push_back(graphparameter.value[nonzero_coefficients[merge_counter]]);
      width_vector.push_back(graphparameter.width[nonzero_coefficients[merge_counter]]);
      MergedParameters.coefficient.push_back(coefficient_sum(coefficient_vector));
      MergedParameters.value.push_back(value_sum(value_vector,coefficient_vector));
      MergedParameters.width.push_back(width_sum(width_vector,coefficient_vector,value_vector));
    }

  }

  return MergedParameters;
}


//This function calculates the number of measurements between kinks
int Trk::QoverPBremFit::nmeasurements (double start, double end)
{
  int npoints(0);
  for (int i=0; i < (int) m_FitValue->size(); i++) {
    if ((*m_FitValue)[i] > start && (*m_FitValue)[i] < end) {
      npoints++;
    }
  }

  return npoints;
} 

//This function calculates the number of potential brem points between kinks
int Trk::QoverPBremFit::npotentials(double start, double end) 
{
  int potentialcount(0);
  for (int i=0; i < (int) m_potentialvalue->size(); i++) {
    if ( (*m_potentialvalue)[i] > start && (*m_potentialvalue)[i] < end) {
      potentialcount++;    
    }
  
  }
  return potentialcount;
}


//This function calculates the maximum distance between measurements within adjacent kinks
//Note: the measured points considered can be outside the radii of the kinks, hence the use
//of "if" conditions #2 and #4 below.
double Trk::QoverPBremFit::maxvalue(double start, double end)
{
  double max(-1);
  double max2(-1);
  int endcounter(0);
  for (int value_counter(0); value_counter < (int) m_FitValue->size(); value_counter++) {
    
    if ((*m_FitValue)[value_counter] < start) {
      continue;
    }
        
    if ((*m_FitValue)[value_counter] > end && endcounter > 0) {
      continue;
    }

    //A measurement outside the value of the kink will counter if it is the first and only one
    else if ((*m_FitValue)[value_counter] > end && endcounter==0) {
      max2 = (*m_FitValue)[value_counter] - (*m_FitValue)[value_counter - 1];
      endcounter++;
    }

    else {
      max2 = (*m_FitValue)[value_counter] - (*m_FitValue)[value_counter - 1];
    }

    if (max2 > max) {
      max = max2;
    }
    
  }
  return max;

}


double Trk::QoverPBremFit::coefficient_sum (std::vector<double> coefficients)
{
  double sum(0);
  for (int sum_counter(0); sum_counter < (int) coefficients.size(); sum_counter++) {
    sum+=coefficients[sum_counter];
  }

  return sum;

}

double Trk::QoverPBremFit::value_sum (std::vector<double> radii, std::vector<double> coefficients)
{
  double value(0);
  double mod_squared(0);
  for (int value_counter(0); value_counter < (int) radii.size(); value_counter++) {
    value+=coefficients[value_counter]*coefficients[value_counter]*radii[value_counter];
    mod_squared+=coefficients[value_counter]*coefficients[value_counter];
  }
  value/=mod_squared;
  return value;
}

double Trk::QoverPBremFit::width_sum (std::vector<double> widths, std::vector<double> coefficients, std::vector<double> radii)
{
  double width(0);
  double coefficient_average(0);
  double mod_squared(0);

  //singular case
  if ( (int)coefficients.size() == 1)
    {
      return widths[0];
    }


  for (int coefficient_counter(0); coefficient_counter < (int)coefficients.size(); coefficient_counter++) {
    coefficient_average+=coefficients[coefficient_counter]/coefficients.size();
    mod_squared+=coefficients[coefficient_counter]*coefficients[coefficient_counter];
  }
  
  for (int width_counter(0); width_counter < (int) widths.size()-1 ; width_counter++) {
    width+=std::min(coefficients[width_counter]*coefficients[width_counter]*(radii[width_counter+1]-radii[width_counter]),coefficients[width_counter+1]*coefficients[width_counter+1]*(radii[width_counter+1]-radii[width_counter]));
  }
  width/=mod_squared;

  for (int width_counter2(0); width_counter2 < (int) widths.size(); width_counter2++) {
    width+=coefficients[width_counter2]*coefficients[width_counter2]*widths[width_counter2]/(coefficients[width_counter2]*coefficients[width_counter2]+coefficient_average*coefficient_average);
  }

  return width;
}

double Trk::QoverPBremFit::GetPerigee_1overP()
{
  return (*m_1overPvalue)[0];
}

double Trk::QoverPBremFit::GetPerigee_Phi()
{
  return (*m_PhiValue)[0];
}

double Trk::QoverPBremFit::GetPerigee_Theta()
{
  return (*m_ThetaValue)[0];
}

double Trk::QoverPBremFit::GetPerigee_d0()
{
  return (*m_RadiusValue)[0];
}

double Trk::QoverPBremFit::GetPerigee_z0()
{
  return (*m_Zvalue)[0];
}

double Trk::QoverPBremFit::Get1overP(int n)
{
  return (*m_1overPvalue)[n];
}

double Trk::QoverPBremFit::GetPhi(int n)
{
  return (*m_PhiValue)[n];
}

double Trk::QoverPBremFit::GetTheta(int n)
{
  return (*m_ThetaValue)[n];
}

double Trk::QoverPBremFit::GetRadius(int n)
{
  return (*m_RadiusValue)[n];
}

double Trk::QoverPBremFit::GetZ(int n)
{
  return (*m_Zvalue)[n];
}

double Trk::QoverPBremFit::Get1overPerror(int n)
{
  return (*m_1overPvalueerror)[n];
}

double Trk::QoverPBremFit::GetPhierror(int n)
{
  return (*m_PhiValueerror)[n];
}

double Trk::QoverPBremFit::GetThetaerror(int n)
{
  return (*m_ThetaValueerror)[n];
}

int Trk::QoverPBremFit::GetNMeasurements()
{
  return (int) (*m_1overPvalue).size();
}

double Trk::QoverPBremFit::GetCharge()
{
  return m_charge;
}

const Trk::TrackParameters* Trk::QoverPBremFit::GetTrackParameters(int n)
{
  return (*m_trackParameters)[n];
}

int Trk::QoverPBremFit::GetnSurfaces()
{
  return m_surfaceCounter;
}

double Trk::QoverPBremFit::GetValue(int n)
{
  return (*m_FitValue)[n];
}


double Trk::QoverPBremFit::LastFitMeasurement()
{
  int lastmeasurement(GetNMeasurements() - 1);
  return (*m_FitValue)[lastmeasurement];
}

double Trk::QoverPBremFit::FirstFitMeasurement()
{
  if (m_type == 1)
    return (*m_FitValue)[0];
  else if (m_type == -1)
    return (*m_FitValue)[1];
  return (*m_FitValue)[0];
}
