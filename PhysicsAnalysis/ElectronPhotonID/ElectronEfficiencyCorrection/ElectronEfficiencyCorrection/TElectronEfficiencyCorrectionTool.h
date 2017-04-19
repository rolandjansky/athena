/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TELECTRONEFFICIENCYCORRECTIONTOOL__
#define __TELECTRONEFFICIENCYCORRECTIONTOOL__

/**
   @class TElectronEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in pure ROOT
   For each scale factor declare a tool and use addFileName to add root files containing scale factors. 
   After adding root files call initialize()
   In the event loop use calculate(const PATCore::ParticleDataType::DataType dataType, const unsigned int runnumber, const double cluster_eta, const double et)
   This returns a TResult. The scale factor and its uncertainty are obtained by calling getScaleFactor() or getTotalUncertainty (See header file of TResult in package PATCore)
   For a short documentation see the included README file

   @author Karsten Koeneke, Felix Buehrer
   @date   January 2013
*/


#include <sstream>

// STL includes
#include <vector>
#include <string>

// ROOT includes
#include <TString.h>
#include "TKey.h"
#include "TObjArray.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"
// Include the return object and the base class
#include "PATCore/TResult.h"
#include "PATCore/TCalculatorToolBase.h"
#include "PATCore/PATCoreEnums.h"
#include "AsgTools/AsgMessaging.h"


namespace Root {
  class TElectronEfficiencyCorrectionTool : public Root::TCalculatorToolBase,public asg::AsgMessaging
    {

    public: 
      /** Standard constructor */
      TElectronEfficiencyCorrectionTool( const char* name="TElectronEfficiencyCorrectionTool" );

      /** Standard destructor */
      ~TElectronEfficiencyCorrectionTool();

      // Main methods
      /** Initialize this class */
      int initialize();

      /** Finalize this class; everything that should be done after the event loop should go here */
      inline int finalize() { return 1; }

      /** The main salculate method: the actual cuts are applied here */
      const Root::TResult& calculate( const PATCore::ParticleDataType::DataType dataType,
				      const unsigned int runnumber,
				      const double cluster_eta,
				      const double et /* in MeV */
				      );


      /// Add an input file
      inline void addFileName ( const std::string& val ) 
      { m_corrFileNameList.push_back(val); }
        
      /// Set the prefix of the result name
      inline void setResultPrefix ( const std::string& val ) { m_resultPrefix = val; }

      /// The string for the result
      inline void setResultName ( const std::string& val ) { m_resultName = val; }

      inline void bookToyMCScaleFactors(const int nToyMC) {
	m_doToyMC = kTRUE;
	m_nToyMC = nToyMC;
      }

      inline void bookCombToyMCScaleFactors(const int nToyMC) {
	m_doCombToyMC = kTRUE;
	m_nToyMC = nToyMC;
      }
  
      /// helper functions to retrieve the position of the first/last toy MC scale factors and correlated systematics in the result
      int getFirstToyMCPosition();
      int getLastToyMCPosition();
      int getFirstCorrSysPosition();
      int getLastCorrSysPosition();
      int getGlobalBinNumberPosition();
      int getNbins(std::map<float, std::vector<float> >&); 
      int getNSyst() {return m_nSysMax;};

      void printResultMap();

      enum detailLevel{simple,medium,detailed};

      /// Set the detail level 
      inline void setDetailLevel ( const int input_detailLevel ) { m_detailLevel = input_detailLevel; }

      inline void setSeed( const int seed) { m_seed = seed; }

      /** Get the name of the class instance */
      inline const char* getName() const { return m_name; }

      // Private methods
    private:
      /// Load all histograms from the input file(s)
      int getHistograms();
      int getHistogramInDirectory( TKey *key );
      int setupHistogramsInFolder( const TObjArray& dirNameArray, int lastIdx );

      void calcDetailLevels(TH1D *eig) ;

      std::vector<TObjArray> buildToyMCTable (const TObjArray &sf, const TObjArray &eig, 
					      const TObjArray& stat, const TObjArray& uncorr, const std::vector<TObjArray> &corr);

      std::vector<TH2D*> buildSingleToyMC(TH2D *sf, TH2D* stat, TH2D* uncorr, const TObjArray& corr);

      TH2D* buildSingleCombToyMC(TH2D *sf, TH2D* stat, TH2D* uncorr, const TObjArray& corr);

      /// Fill and interpret the setup, depending on which histograms are found in the input file(s)
      int setup( const TObjArray& hist,
		 std::vector< TObjArray >& histList,
		 std::vector< unsigned int >& beginRunNumberList,
		 std::vector< unsigned int >& endRunNumberList );
    
      int setupSys( std::vector<TObjArray> & hist,
		    std::vector< std::vector< TObjArray>> & histList);

      void printDefaultReturnMessage(TString reason, int line);

      TRandom3 m_Rndm;
      int m_randomCounter;
      bool m_isInitialized;
      /// The detail level
      int m_detailLevel;
      int m_toyMCSF;
      //
      /// The seed
      int m_seed;
      bool m_doToyMC;
      bool m_doCombToyMC;
      int m_nToyMC;
      int m_sLevel[3];
      int m_nSys;
      int m_nSysMax;
      int m_runNumBegin;
      int m_runNumEnd;

      /// The prefix string for the result
      std::string m_resultPrefix;

      /// The string for the result
      std::string m_resultName;

      /// The position of the efficiency scale factor in the result
      int m_position_eff; 

      /// The position of the efficiency scale factor uncertainty in the result
      int m_position_err; 

      /// The position of the efficiency scale factor statistical uncertainty in the result
      int m_position_statErr; 

      /// The position of the number of systematic uncertainties in the result
      int m_position_nSys; 

      /// The position of the efficiency scale factor uncorrelated systematic uncertainty in the result
      int m_position_uncorrSys; 

      /// total number of bins in the efficiency tables
      int m_nbins; 

      int m_nSimpleUncorrSyst;

      /// The position of the efficiency scale factor uncorrelated systematic uncertainty in the result
      int m_position_globalBinNumber; 


      //description here ?
      std::vector< std::vector<TObjArray>> m_uncorrToyMCSystFull;
      std::vector< std::vector<TObjArray>> m_uncorrToyMCSystFast;
      /// The list of file name(s)
      std::vector< std::string > m_corrFileNameList;
      /// List of run numbers where histgrams become valid for full simulation
      std::vector< unsigned int > m_begRunNumberList;
      /// List of run numbers where histgrams stop being valid for full simulation
      std::vector< unsigned int > m_endRunNumberList;
      /// List of run numbers where histgrams become valid for fast simulation
      std::vector< unsigned int > m_begRunNumberListFastSim;
      /// List of run numbers where histgrams stop being valid for fast simulation
      std::vector< unsigned int > m_endRunNumberListFastSim;    
      /// List of histograms for full Geant4 simulation
      std::map<int, std::vector< TObjArray > > m_histList;
      std::vector< std::vector< TObjArray > > m_sysList;
      /// List of histograms for fast simulation
      std::map<int, std::vector< TObjArray > > m_fastHistList;
      std::vector< std::vector< TObjArray > > m_fastSysList;
      /// The positions of the efficiency scale factor correlated sustematic uncertainties in the result
      std::vector<int> m_position_corrSys; 
      /// The positions of the toy MC scale factors
      std::vector<int> m_position_uncorrToyMCSF; 
      ///The vector holding the keys
      std::vector<int> m_keys;
    
    }; // End: class definition
  
} // End: namespace Root

#endif

