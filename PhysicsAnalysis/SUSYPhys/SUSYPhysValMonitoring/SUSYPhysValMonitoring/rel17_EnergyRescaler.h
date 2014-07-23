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

#include <TRandom3.h>



class EnergyRescaler {


  

   public:



      EnergyRescaler();
      ~EnergyRescaler();

      ////read text file containing calib constants
      bool readCalibConstants(std::string fname);
      
    
//      typedef enum { NOMINAL=1, ERR_DOWN=2, ERR_UP=3 } CorrType;
      typedef enum { NOMINAL=0, ERR_DOWN=1, ERR_UP=2 } CorrType;


      //take eta/phi and uncorrected energy of electron, return  corrected energy, 
      //last argurment is to choose central/down/up energy corrections, default is nominal/central value
     
      double applyEnergyCorrection(double cl_eta, double cl_phi, double uncorr_energy, double et, int value=NOMINAL /* NOMINAL=0, ERROR_DOWN==1, ERROR_UP==2*/, std::string part_type="ELECTRON" );


      //if can't use the above method then use this method to read the default constants(note they are not egamma default constants
      //but for private use only!)

      bool useDefaultCalibConstants();


      ///set random seed to some fix value(would need for MC comparison )
      
      void SetRandomSeed(unsigned seed=0 );

      
      ////get smearing correction
      double getSmearingCorrection(double eta, double energy, int value=NOMINAL, bool mc_withCT=true);


      ////print the constants
      bool printMap();

      //get systematics error, user should not call this method. Use it via applyEnergyCorrection
      void getError(double cl_eta,double cl_et, double &er_up, double &er_do, std::string part_type="ELECTRON",bool withXMAT=true);
      

   private:
      

      TRandom3   m_random3;

      struct calibMap { 
 
            double eta; 
            double phi; 
            double etaBinSize; 
            double phiBinSize; 
            double alpha; 
            double alphaErr; 
      }; 


      std::vector< calibMap > corrVec;
 
      
};

#endif

