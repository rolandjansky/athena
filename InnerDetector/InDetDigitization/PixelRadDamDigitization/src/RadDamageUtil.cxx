/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RadDamageUtil.cxx
//   Implementation file for class RadDamageUtil
//   R. Carney & B Nachman 2018
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
// Details in head file
///////////////////////////////////////////////////////////////////

#include "RadDamageUtil.h"

#include "TGraph.h"
#include "TString.h"
#include "TMath.h"
#include "TFile.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesSvc/SiliconProperties.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

#include "PathResolver/PathResolver.h"
#include <fstream>
#include <cmath>

#include "CLHEP/Random/RandExpZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "TLorentzVector.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "EfieldInterpolator.h"

using namespace std;

// Constructor with parameters:
RadDam::RadDamageUtil::RadDamageUtil(const std::string& type, const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_betaElectrons(4.5e-16),
  m_betaHoles(6.0e-16),
  m_defaultRamo( 1 ),
  m_defaultEField( 1 ),
  m_EfieldInterpolator(nullptr),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngineName("PixelDigitization"),
  m_rndmEngine(0)
{ 
  declareProperty("RndmSvc", m_rndmSvc, "Random Number Service used in RadDamageUtil");
  declareProperty("RndmEngine", m_rndmEngineName, "Random engine name");
  declareProperty("EfieldInterpolator", m_EfieldInterpolator, "Create an Efield for fluence and bias volatge of interest based on TCAD samples");
}

// Destructor:
RadDam::RadDamageUtil::~RadDamageUtil(){}

//=======================================
// I N I T I A L I Z E
//=======================================
StatusCode RadDam::RadDamageUtil::initialize() {
  
  CHECK(AthAlgTool::initialize());
  CHECK(m_EfieldInterpolator.retrieve());
  CHECK(m_rndmSvc.retrieve());
  ATH_MSG_INFO("You are using RadDamageUtil for solid-state silicon detectors.");
  
  return StatusCode::SUCCESS;
}
  
//=======================================
// G E N E R A T E   R A M O   M A P
//=======================================
// The third of the 3 maps should be the most accurate. 
// See doc/RadDamageDefaults.pdf in the Allpix repo for details.
// See ATL-COM-INDET-2018-011 for details.
//=======================================
const StatusCode RadDam::RadDamageUtil::generateRamoMap( TH3F* ramoPotentialMap, InDetDD::PixelModuleDesign* module){

  //TODO: this needs to come from DB
  double pitchX = 0.05;
  double pitchY = 0.25;
  //TODO: from PixelModuleDesign
  double sensorThickness = module->thickness() * 1000.0;//default 200;

  	
  //Ramo potential evaluated up to 2x pitch away 
  //from the center of the primary pixel.  
  double xmin = 0.;
  double xmax = 2*pitchX*1000;
  double ymin = 0.;
  double ymax = 2*pitchY*1000;

  //One bin per 10 microns.
  ramoPotentialMap = new TH3F("hramomap1","hramomap1",((xmax-xmin)/10.), xmin, xmax, ((ymax-ymin)/10.), ymin, ymax, int(sensorThickness*1000)/10, 0., sensorThickness*1000.);
  
  //******************
  //*** Loop in z ***
  //******************
  for (int k=1; k <= ramoPotentialMap->GetNbinsZ(); k++){ 

      //use the lower bin edge.
      double z = ramoPotentialMap->GetZaxis()->GetBinCenter(k) - ramoPotentialMap->GetZaxis()->GetBinWidth(k)/2.; 
      
      //******************
      //*** Loop in x,y ***
      //******************
      for (int i=1; i <= ramoPotentialMap->GetNbinsX(); i++){ //Loop over x
          for (int j=1; j <= ramoPotentialMap->GetNbinsY(); j++){ //loop over y

              double x = ramoPotentialMap->GetXaxis()->GetBinCenter(i) - ramoPotentialMap->GetXaxis()->GetBinWidth(i)/2.;
              double y = ramoPotentialMap->GetYaxis()->GetBinCenter(j) - ramoPotentialMap->GetYaxis()->GetBinWidth(j)/2.;

	      //*******************************
	      //*** Option A: 1D approx. in z
	      //*******************************
              if (m_defaultRamo == -1){ 
                  if (x > (pitchX*1000.* 0.5) || y > (pitchY*1000.*0.5) ){
                      ramoPotentialMap->SetBinContent(i,j,k,0.01);  //outside of the primary pixel. 
		      //TODO what is the last bin value? Is 0.01 the min?
        	  } else {

		      //TODO make sure all of these eta/phi values make sense for non-barrel modules too
        	      //The formula below parameterises the 1D Ramo potential. See ATL-COM-INDET-2018-011 for details.
		      double par_a = 3*sensorThickness / pitchY;                                                                                          
        	      double norm = exp( -par_a ) + exp(-1.);
        	      double val = exp( -par_a*z/(1000.*sensorThickness) ) + exp(-z/(1000*sensorThickness));
        	      val -= norm;
        	      val /= ( 2.-norm ); 
		      ramoPotentialMap->SetBinContent( i,j,k,val );
        	  }
	      //*******************************
	      //*** Option B: 2D approx. in xy
	      //*******************************
              } else if (m_defaultRamo == 0){
                    
		  double par_a = 10.0;
		  double norm = exp( -par_a ) + exp(-1.);
		  double val = exp( -par_a*z/(1000*sensorThickness) ) + exp( -z/(1000*sensorThickness) ); 
		  val -= norm;
		  val /= ( 2.-norm );
		  //From equation 16 in the RadDamageDefaults support note, using solution for weighting potential in 2D
                  double  productSolution = weighting2D(x,z,pitchX,sensorThickness) * weighting2D(y,z,pitchY,sensorThickness) * val / (weighting2D(0,z,pitchX,sensorThickness) * weighting2D(0,z,pitchY,sensorThickness));
		  ramoPotentialMap->SetBinContent(i,j,k, productSolution); 
	      //************************************************
              //*** Option C: Poisson's eqn. w/ simple geometry
	      //************************************************
	      } else if (m_defaultRamo > 0){
                   
                      double fullSolution = weighting3D( x/sensorThickness,y/sensorThickness,z/sensorThickness,m_defaultRamo,m_defaultRamo,4,pitchX/sensorThickness,pitchY/sensorThickness); //N = 4 is arbitrary; just need something bigger than ~1
		      ramoPotentialMap->SetBinContent(i,j,k,fullSolution);

	      }//Ramo option > 0.                                                                                                                                
	  }//loop over y.                                                                                                                                              
      }//loop over x.                                                                                                                                                
  }//loop over z.                                                                                                                                                  
  return StatusCode::SUCCESS;
} //TODO: What about debugging the ramo potential?  I vaguely remember running into issues with this.


//=======================================
// A L P H A 
//=======================================
//Constituent of full poisson solution. 
//Last terms in eqn. 18, 19 in support note
double RadDam::RadDamageUtil::alpha(int n, int Nrep, double a){
	return ( (2*TMath::Pi()*n) / (Nrep*a)); 
}

//=======================================
// W E I G H T I N G   3 D
//=======================================
//Approx, solution to Poisson's eqn. with simplified geometry
//See section 1.3 in support note for details
double RadDam::RadDamageUtil::weighting3D(double x, double y, double z, int n, int m, int Nrep, double a, double b){
 
  //TODO: talk to ben about this comment: 
  //be warned that there is numerical instability if n and m are too large!  Suggest n ~ m ~ 10.                                                                             
  double potential = 0.;
  double pi = TMath::Pi();

  for (int i=-n; i<=n; i++){
	  for (int j=-m; j<=m; j++){
		  double X = 0.;
		  double Y = 0.;
		  double Z = 0.;
		  double factor_x = 0.;
		  double factor_y = 0.;
		  if (i==0 && j==0){
                         factor_x = 1./Nrep;
			 factor_y = factor_x;
			 Z = 1-z;
		  } else {
		          //Equation 18 & 19 in support note
			  factor_x = std::sin( i*pi/Nrep )/ (pi*i);
			  factor_y = std::sin( j*pi/Nrep )/ (pi*j);
		          //Equation 17 in support note
			  double norm = std::sqrt( std::pow( alpha(i,Nrep,a),2) + std::pow( alpha(j,Nrep,b),2) );
			  Z = sinh(norm*(1-z))/sinh(norm);
		  }
		  //Equation 18 & 19 in support note
		  X = factor_x*std::cos( alpha(i,Nrep,a)*x );
		  Y = factor_y*std::cos( alpha(j,Nrep,b)*y );

		  //Equation 20 in support note
		  potential+= Z * X * Y;
	  }
  }
  return potential;
} 

//=======================================
//W E I G H T I N G  2 D
//=======================================
//Solution to Poisson's equation for a 2D inf. strip
//i.e. weighting potential with 2D solution
double RadDam::RadDamageUtil::weighting2D(double x, double z, double Lx, double sensorThickness){

  if (z==0) z=0.00001;//a pathology in the definition.
  double pi = 4.*TMath::ATan(1.);

  //scale to binsize (inputs assumed to be in mm)
  sensorThickness *= 1000.;
  Lx *= 1000;
  
  //val is set according to equation 3 in the radDamageDefaults support note
  double val = (TMath::Sin(pi*z/sensorThickness)*TMath::SinH(0.5*pi*Lx/sensorThickness)/(TMath::CosH(pi*x/sensorThickness)-TMath::Cos(pi*z/sensorThickness)*TMath::CosH(0.5*pi*Lx/sensorThickness)));
  if (val > 0) return TMath::ATan(val)/pi;
  else return TMath::ATan(val)/pi+1;
}

//=========================================
// G E N E R A T E   E - F I E L D   M A P
//=========================================
const StatusCode RadDam::RadDamageUtil::generateEfieldMap( TH1F* eFieldMap, InDetDD::PixelModuleDesign* module  ){

    //TODO: from DB
    double biasVoltage          = 600.;
    double depletionVoltage     = 80.;
    double depletionLength      = 0.2;
    double sensorThickness      = module->thickness(); //default should be 0.2?
    double fluence              = 8.;//*e14 neq/cm^2 
    eFieldMap = new TH1F("hefieldz","hefieldz",200,0,sensorThickness*1e3);
    
    std::string TCAD_list = PathResolver::find_file("ibl_TCAD_EfieldProfiles.txt", "DATAPATH"); //IBL layer 
    if(sensorThickness > 0.2){
        //is blayer
        TCAD_list = PathResolver::find_file("blayer_TCAD_EfieldProfiles.txt", "DATAPATH"); //B layer 
        if(sensorThickness > 0.25){
            ATH_MSG_WARNING("Sensor thickness ("<< sensorThickness << "mm) does not match geometry provided in samples");
            return StatusCode::FAILURE;
        }
    }

    m_EfieldInterpolator->loadTCADlist(TCAD_list);
    eFieldMap = (TH1F*) m_EfieldInterpolator->getEfield(fluence , biasVoltage);		
   /* 
    //Set depletion width
    double electricField=0; //V/mm
    double depletionWidth = sensorThickness; //in mm
    if( biasVoltage < depletionVoltage)
	    depletionWidth *= pow(biasVoltage/depletionVoltage, 0.5);

    //Fill map
    double binSize = depletionLength/eFieldMap->GetNbinsX();
    for (int i=1; i<= eFieldMap->GetNbinsX()+1; i++){

	    double position = binSize*i;
	    if(biasVoltage < depletionVoltage){
		    electricField=( biasVoltage/depletionLength )*( 1.-position/depletionLength );
	    } else {
		    electricField =( depletionVoltage/depletionLength )*( 1.-position/depletionLength);
		    electricField += (biasVoltage-depletionVoltage)/(2.*depletionLength);
	    }
	    //Boundary checking
	    if(position > depletionLength)
		    electricField=0.;

	    eFieldMap->SetBinContent(eFieldMap->GetNbinsX()-i+1,electricField*10); //in V/cm; n in n prior to type inversion.
	    if (m_defaultEField==0) 
		    eFieldMap->SetBinContent(i,10*biasVoltage/(sensorThickness)); //in V/cm
    }
    */
  return StatusCode::SUCCESS;
}

StatusCode RadDam::RadDamageUtil::generateEfieldMap( TH1F* &eFieldMap, InDetDD::PixelModuleDesign* module, double fluence,  double biasVoltage, int layer, std::string TCAD_list, bool interpolate ){

    TString id;
    //TODO adapt saving location for documentation of E field interpolation
    TString predirname ="";
    if(interpolate){
        id = "Interpolation";
    }else{
        id = "TCAD";
    }
    if(interpolate){
        CHECK( m_EfieldInterpolator->loadTCADlist(TCAD_list) );
        eFieldMap = (TH1F*) m_EfieldInterpolator->getEfield(fluence , biasVoltage);		
    }else{
        //retrieve E field directly from file (needs to be .dat file with table)
        CHECK( m_EfieldInterpolator->loadTCADlist(TCAD_list) );
        ATH_MSG_INFO("Load Efield map from " << TCAD_list );
    }
    if(!eFieldMap){
        ATH_MSG_ERROR("E field has not been created!");
        return StatusCode::FAILURE;
    } 
    //For debugging save map
    //TCAD_list.ReplaceAll(".txt","_map.root");
    TString dirname = "layer";
    dirname+=layer;
    dirname+="_fl";
    dirname+=TString::Format("%.1f",fluence/(float)(1e14));
    dirname+="e14_U";
    dirname+=TString::Format("%.0f",biasVoltage);
    dirname+=id;
    dirname.ReplaceAll(".","-");
    dirname = predirname + dirname;
    dirname+=(".root");
    eFieldMap->SaveAs(dirname.Data());
    return StatusCode::SUCCESS;
}

//==================================================
// G E N E R A T E   DISTANCE / TIME / LORENTZ  MAP
//==================================================
//Currently, if one is missing, all 3 have to be regenerated.
//It IS possible to split them up but riht now that means lots of repeated code.
//Might be worth coming back in the future if it needs to be optimised or
//if 
const StatusCode RadDam::RadDamageUtil::generateDistanceTimeMap( TH2F* &distanceMap_e, TH2F* &distanceMap_h, TH1F* &timeMap_e, TH1F* &timeMap_h, TH2F* &lorentzMap_e, TH2F* &lorentzMap_h, TH1F* &eFieldMap, InDetDD::PixelModuleDesign* module ){
    // Implementation for precomputed maps
    //https://gitlab.cern.ch/radiationDamageDigitization/radDamage_athena_rel22/blob/rel22_radDamageDev_master/scripts/SaveMapsForAthena.C
    //TODO: From DB call each time
    double temperature = 300;
    double bField = 2;//Tesla
    //From PixelModuleDesign: TODO
    //FIXME workaround, if PixelModuleDesign not available: retrieve sensor thickness from E field
    //double sensorThickness = module->thickness() * 1000.0;//default is 200;
    double sensorThickness = 0.2; //mm
    //Check if x axis (sensor depth) of E field larger than IBL sensors
    if(eFieldMap->GetXaxis()->GetXmax() > 210 ){ //Efield in um
        sensorThickness = 0.250; 
    }
    if(module){
        sensorThickness = module->thickness() * 1000.0;//default is 200;
    }
    
    //Y-axis is time charge carrier travelled for,
    //X-axis is initial position of charge carrier,
    //Z-axis is final position of charge carrier
    distanceMap_e = new TH2F("edistance","Electron Distance Map",100,0,sensorThickness,1000,0,1000); //mm by ns
    distanceMap_h = new TH2F("hdistance","Holes Distance Map",100,0,sensorThickness,1000,0,1000);
    //Initalize distance maps 
    for (int i=1; i<= distanceMap_e->GetNbinsX(); i++){
      for (int j=1; j<= distanceMap_e->GetNbinsY(); j++){
	distanceMap_h->SetBinContent(i,j,sensorThickness); //if you travel long enough, you will reach the electrode.
        distanceMap_e->SetBinContent(i,j,0.);              //if you travel long enough, you will reach the electrode.
      }
    }

    //From a given place in the sensor bulk, show time-to-electrode
    timeMap_e = new TH1F("etimes","Electron Time Map",100,0,sensorThickness); //mm
    timeMap_h = new TH1F("htimes","Hole Time Map",100,0,sensorThickness); //mm
    
    //X axis is initial position of charge carrier (in z)
    //Y axis is distance travelled in z by charge carrier
    //Z axis is tan( lorentz_angle )
    lorentzMap_e = new TH2F("lorentz_map_e","Lorentz Map e",100,0,sensorThickness,100,0,sensorThickness); //mm by mm
    lorentzMap_h = new TH2F("lorentz_map_h","Lorentz Map h",100,0,sensorThickness,100,0,sensorThickness); //mm by mm
    ATH_MSG_DEBUG ("Did not find time and/or distance maps.  Will compute them from the E-field map..");

    for (int i=1; i<= distanceMap_e->GetNbinsX(); i++){ //Loop over initial position of charge carrier (in z)
	    double z_i = distanceMap_e->GetXaxis()->GetBinCenter(i);
	    double time_e = 0.; //ns
	    double time_h = 0.; //ns
	    double distanceTravelled_e=0; //mm 
	    double distanceTravelled_h=0; //mm
	    double drift_e = 0.; //mm
	    double drift_h = 0.; //mm

	    for (int j=i; j >= 1; j--){ //Lower triangle 
		double dz = distanceMap_e->GetXaxis()->GetBinWidth(j); //mm
		double z_j = distanceMap_e->GetXaxis()->GetBinCenter(j); //mm
                //printf("\n \n distance map bin center i/j: %f/%f width %f (mm) \n sensor thickness: %f \n E field value: %f in bin %f \n ", z_i, z_j, dz, sensorThickness, Ez, z_i*1000);
                //
	        double Ez = eFieldMap->GetBinContent(eFieldMap->GetXaxis()->FindBin(z_j*1000))/1e7; // in MV/mm;  
	        std::pair<double, double> mu = getMobility(Ez, temperature); //mm^2/MV*ns
		if (Ez > 0){
		        
		        //Electrons
		        //double tanLorentzAngle = mu.first*bField*(1.0E-3); //rad, unit conversion; pixelPitch_eta-Field is in T = V*s/m^2
                        double tanLorentzAngle = getTanLorentzAngle(Ez, temperature, bField,  false);
		        time_e += dz/(mu.first*Ez); //mm * 1/(mm/ns) = ns

		        //Fill: time charge carrier travelled for, given staring position (i) and final position (z_j)
		        distanceMap_e->SetBinContent(i,distanceMap_e->GetYaxis()->FindBin(time_e),z_j);
		        
		        drift_e += dz*tanLorentzAngle; //Increment the total drift parallel to plane of sensor
		        distanceTravelled_e += dz; //mm (travelled in z)
		        lorentzMap_e->SetBinContent(i,j,drift_e/distanceTravelled_e); 

			    lorentzMap_h->SetBinContent(i,j,drift_h/distanceTravelled_h);
		    }
	    }
            //Mainly copied from l416 ff changed naming k=>j
            //https://gitlab.cern.ch/radiationDamageDigitization/radDamage_athena_rel22/blob/rel22_radDamageDev_master/scripts/SaveMapsForAthena.C
            for (int j=i; j <= distanceMap_e->GetNbinsX(); j++){ //holes go the opposite direction as electrons.

                double dz = distanceMap_e->GetXaxis()->GetBinWidth(j); //similar to _h
		//double Ez = eFieldMap->GetBinContent(eFieldMap->GetXaxis()->FindBin(z_i*1000))/1e7; // in MV/mm;  
		double z_j= distanceMap_e->GetXaxis()->GetBinCenter(j); //mm //similar to _h
	        double Ez = eFieldMap->GetBinContent(eFieldMap->GetXaxis()->FindBin(z_j*1000))/1e7; // in MV/mm;  
	        std::pair<double, double> mu = getMobility(Ez, temperature); //mm^2/MV*ns
                if (Ez > 0){
                    //Holes
			    //std::pair<double, double> mu = getMobility(Ez, temperature); //mm^2/MV*ns
                    //double tanLorentzAngle = mu.second*bField*(1.0E-3); //rad 
                    double tanLorentzAngle = getTanLorentzAngle(Ez, temperature,bField, true);
                    time_h+=dz/(mu.second*Ez); //mm * 1/(mm/ns) = ns  
                    distanceMap_h->SetBinContent(i,distanceMap_h->GetYaxis()->FindBin(time_h),z_j);
                    
                    drift_h+=dz*tanLorentzAngle;
                    distanceTravelled_h += dz; //mm 
                    lorentzMap_h->SetBinContent(i,j,drift_h/distanceTravelled_h);
                }
	        timeMap_h->SetBinContent(i,time_h);
            }


    }

  return StatusCode::SUCCESS;
  //Finally, we make maps of the average collected charge, in order to make charge chunking corrections later.
  //TODO: talk to Ben about the above comment. Where is code?

}
//=======================================
// G E T   M O B I L I T Y
//=======================================
const std::pair<double,double> RadDam::RadDamageUtil::getMobility(double electricField, double temperature) const{ 
  //Returns the electron/hole mobility *in the z direction*
  //Note, this already includes the Hall scattering factor!
  //These parameterizations come from C. Jacoboni et al., Solid-State Electronics 20 89. (1977) 77.  (see also https://cds.cern.ch/record/684187/files/indet-2001-004.pdf).
  //electrons
  double vsat_e = 15.3*pow(temperature,-0.87);// mm/ns
  double ecrit_e = 1.01E-7*pow(temperature,1.55);// MV/mm
  double beta_e = 2.57E-2*pow(temperature,0.66);//dimensionless
  double r_e = 1.13+0.0008*(temperature-273.);//Hall scaling factor
  //holes
  double vsat_h = 1.62*pow(temperature,-0.52);// mm/ns
  double ecrit_h = 1.24E-7*pow(temperature,1.68);// MV/mm
  double beta_h = 0.46*pow(temperature,0.17);
  double r_h = 0.72 - 0.0005*(temperature-273.);

  double num_e = vsat_e/ecrit_e;
  double den_e = pow(1+pow((electricField/ecrit_e),beta_e),(1/beta_e));
  double mobility_e = r_e*num_e/den_e;

  double num_h = vsat_h/ecrit_h;
  double den_h = pow(1+pow((electricField/ecrit_h),beta_h),(1/beta_h));
  double mobility_h =  r_h*num_h/den_h;

  return std::make_pair( mobility_e, mobility_h );
}

//=======================================
// G E T   L O R E N T Z  A N G L E
//=======================================
//Taken from https://gitlab.cern.ch/radiationDamageDigitization/radDamage_athena_rel22/blob/rel22_radDamageDev_master/scripts/SaveMapsForAthena.C
double RadDam::RadDamageUtil::getTanLorentzAngle(double electricField, double temperature, double bField,  bool isHole){
    double hallEffect = 1.;//already in mobility//= 1.13 + 0.0008*(temperature - 273.0); //Hall Scattering Factor - taken from https://cds.cern.ch/record/684187/files/indet-2001-004.pdf
    if (isHole) hallEffect = 0.72 - 0.0005*(temperature - 273.0);
    std::pair<double,double> mobility = getMobility(electricField, temperature);
    double mobility_object = mobility.first;
    if(isHole) mobility_object = mobility.second;
    double tanLorentz = hallEffect*mobility_object*bField*(1.0E-3);  //unit conversion
    return tanLorentz;
}


//=======================================
// G E T   T R A P P I N G   T I M E
//=======================================
const std::pair<double,double> RadDam::RadDamageUtil::getTrappingTimes( double fluence) const{
 
  double trappingTimeElectrons(0.), trappingTimeHoles(0.);

  if(fluence!=0.0){
    trappingTimeElectrons = 1.0/(m_betaElectrons*fluence); //Make memberVar
    trappingTimeHoles = 1.0/(m_betaHoles*fluence); //ns
  }
  else{//fluence = 0 so do not trap!
    trappingTimeElectrons = 1000; //~infinity
    trappingTimeHoles = 1000;
  }

  return std::make_pair( trappingTimeElectrons, trappingTimeHoles);
}

//=======================================
// F I N A L I Z E
//=======================================
StatusCode RadDam::RadDamageUtil::finalize() {
  ATH_MSG_DEBUG ( "RadDamageUtil::finalize()");
  return StatusCode::SUCCESS;
}

