/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////
//DATED: October 23, 2010 
//V0.1
//Author Ashfaq Ahmad <Ashfaq.Ahmad@cern.ch>
//Class to rescale/weight energy of the cluster by applying 
//Calibration Constants.
//for more information please go here
//https://twiki.cern.ch/twiki/bin/view/AtlasProtected/EnergyRescaler
// Jan 26, 2010
//include Nikola Makovec smearing function
//Feb 4, 2010
// include systematics
///////////////////////////////////////////////////////////
#ifndef EnergyRescaler_h
#define EnergyRescaler_h

#include <vector>
#include <string>
using std::size_t;
#include <TRandom3.h>


namespace eg2011 {

// Taken from CLHEP/Units/SystemOfUnits.h
static const double GeV = 1.e+3;


class EnergyRescaler {


   public:


      EnergyRescaler();
      virtual ~EnergyRescaler();

      ////read text file containing calib constants
      bool readCalibConstants(std::string fname);
      
    
//      typedef enum { NOMINAL=1, ERR_DOWN=2, ERR_UP=3 } CorrType;
      typedef enum { NOMINAL=0, ERR_DOWN=1, ERR_UP=2 } CorrType;


      //take eta/phi and uncorrected energy of electron, return  corrected energy, 
      //last argurment is to choose central/down/up energy corrections, default is nominal/central value
     
      double applyEnergyCorrectionGeV(double cl_eta, double cl_phi, double uncorr_energy, double et, 
				      int value=NOMINAL /* NOMINAL=0, ERROR_DOWN==1, ERROR_UP==2*/, std::string part_type="ELECTRON" ) const;

      double applyEnergyCorrectionMeV(double cl_eta, double cl_phi, double uncorr_energy, double et, 
				      int value=NOMINAL /* NOMINAL=0, ERROR_DOWN==1, ERROR_UP==2*/, std::string part_type="ELECTRON" ) const;
			
		// take std::vectors from D3PD
      std::vector<float> applyEnergyCorrectionMeV(
         std::vector<float> cl_eta, std::vector<float> cl_phi, std::vector<float> cl_E, 
         std::vector<float> eta, /* cl_eta for photons, tracketa for electrons */
         std::vector<int> ph_isConv = std::vector<int>(), /* LEAVE BLANK FOR ELECTRONS */
         int value=NOMINAL /* NOMINAL=0, ERROR_DOWN==1, ERROR_UP==2*/ ) const;


      //if can't use the above method then use this method to read the default constants(note they are not egamma default constants
      //but for private use only!)

      bool useDefaultCalibConstants(std::string corr_version="2011");


      ///set random seed to some fix value(would need for MC comparison )
      
      void SetRandomSeed(unsigned seed=0 );

      
      /// smearing corrections

      double getSmearingCorrectionGeV(double eta, double energy, int value=NOMINAL, bool mc_withCT=false,std::string corr_version="2011" ) ;
      double getSmearingCorrectionMeV(double eta, double energy, int value=NOMINAL, bool mc_withCT=false,std::string corr_version="2011" ) ;
      
      // take std::vectors from D3PD
      std::vector<float> getSmearingCorrectionMeV(
         std::vector<float> eta_s2, 
         std::vector<float> cl_E, 
         int value=NOMINAL, bool mc_withCT=false,std::string corr_version="2011" ) ;

      /// MC calibration corrections

      double applyMCCalibrationGeV(double eta, double ET, std::string ptype);
      double applyMCCalibrationMeV(double eta, double ET, std::string ptype);

      double applyAFtoG4(double eta);

      ////print the constants
      bool printMap() const;

      //get systematics error

      // ... total error with some optional subtractions

      void getErrorGeV(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type="ELECTRON",
		       bool withXMAT=true,bool withPS=true) const;
      void getErrorMeV(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type="ELECTRON",
		       bool withXMAT=true,bool withPS=true) const;
      
      // ... dominant subsets : PS, Material, Rest

      void getErrorGeV_PS(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type="ELECTRON") const;
      void getErrorMeV_PS(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type="ELECTRON") const;
      
      void getErrorGeV_MAT(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type="ELECTRON") const;
      void getErrorMeV_MAT(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type="ELECTRON") const;
      
      void getErrorGeV_REST(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type="ELECTRON") const;
      void getErrorMeV_REST(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type="ELECTRON") const;
      

      // new functions (MB)
      // INTERNAL ONLY - DO NOT USE

      double mcSamplingTerm(double cl_eta);
      double mcSamplingTermRelError( double cl_eta );
      double mcNoiseTerm( double cl_eta );
      double mcConstantTerm( double cl_eta );
      double dataConstantTerm( double cl_eta );
      double dataConstantTermError( double cl_eta );
      double dataConstantTermUpError( double cl_eta );
      double dataConstantTermDownError( double cl_eta );
      double dataZPeakResolution( double cl_eta );
      double mcZPeakResolution( double cl_eta );
      double dataConstantTermCorError( double cl_eta ); 
      double fcn_sigma(double energy, double Cdata, double Cdata_er, double S, double S_er);
      void   resolutionError( double energy, double cl_eta, double& errUp, double& errDown );
      double resolution( double energy, double cl_eta, bool withCT );
     
      // end new functions (MB)

      class calibMap { 
          public:
     
      calibMap()
        : eta(0),
          phi(0),
          etaBinSize(0),
          phiBinSize(0),
          alpha(0),
          alphaErr(0)
          { }
               virtual ~calibMap() { }
      
                double eta; 
                double phi; 
                double etaBinSize; 
                double phiBinSize; 
                double alpha; 
                double alphaErr; 
  
                #ifdef ROOTCORE
                ClassDef(calibMap,1)
                #endif
      }; 
     
      #ifdef ROOTCORE
      ClassDef(EnergyRescaler,1)
      #endif

   private:
      
      mutable TRandom3   m_random3;

      std::vector< calibMap > m_corrVec;

      std::string m_corr_version;
      
};




// INLINES

inline double EnergyRescaler::applyEnergyCorrectionMeV(double cl_eta, double cl_phi, double uncorr_energy, 
						       double et, int value, std::string part_type) const
{ 
  return applyEnergyCorrectionGeV(cl_eta, cl_phi, uncorr_energy/GeV, et/GeV, value, part_type) * GeV;
}

inline double EnergyRescaler::getSmearingCorrectionMeV(double eta, double energy, int value, bool mc_withCT,std::string corr_version) 
{
  return getSmearingCorrectionGeV(eta, energy/GeV, value, mc_withCT, corr_version);
}

inline double EnergyRescaler::applyMCCalibrationMeV(double eta, double ET, std::string ptype)
{
  return applyMCCalibrationGeV(eta, ET/GeV, ptype);
}

inline void EnergyRescaler::getErrorMeV(double cl_eta,double cl_et, double &er_up, double &er_do, 
					std::string part_type, bool withXMAT,bool withPS) const
{
  getErrorGeV(cl_eta, cl_et/GeV, er_up, er_do, part_type, withXMAT, withPS);
}

inline void EnergyRescaler::getErrorMeV_PS(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type) const
{
  getErrorGeV_PS(cl_eta, cl_et/GeV, er_up, er_do, part_type);
}

inline void EnergyRescaler::getErrorMeV_MAT(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type) const
{
  getErrorGeV_MAT(cl_eta, cl_et/GeV, er_up, er_do, part_type);
}

inline void EnergyRescaler::getErrorMeV_REST(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type) const
{
  getErrorGeV_REST(cl_eta, cl_et/GeV, er_up, er_do, part_type);
}


} // end of namespace
#endif

