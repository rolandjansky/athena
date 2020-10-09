// Dear emacs, this is -*- c++ -*-
/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TELECTRONEFFICIENCYCORRECTIONTOOL__
#define __TELECTRONEFFICIENCYCORRECTIONTOOL__

/**
  @class TElectronEfficiencyCorrectionTool
  @brief Calculate the egamma scale factors 
  Implementation class for the e/gamma Efficiency Scale Factors. This code implements
  the underlying logic of accessign the ROOT files containing the recommendations.
  @authors Kristin Lohwasser, Karsten Koeneke, Felix Buehrer
  @date   January 2013
  @updated by Christos Anastopoulos 2017-2018
  */

// STL includes
#include <vector>
#include <string>
#include <array>
#include <unordered_map>
#include <map>
//Root fwd declares
class TKey;
class TH1;
class TH2;
// ROOT includes
#include "TRandom3.h"
#include "TObjArray.h"
// Core includes
#include "PATCore/PATCoreEnums.h"
#include "AsgMessaging/AsgMessaging.h"

namespace Root {
class TElectronEfficiencyCorrectionTool : public asg::AsgMessaging
{
public: 
  /**
   * The position of each result
   */
  enum struct Position{
    SF=0,
    Total=1,
    Stat=2,
    NSys=3,
    UnCorr=4,
    GlobalBinNumber=5,
    End=6
  };

  /** Standard constructor */
  TElectronEfficiencyCorrectionTool( const char* name="TElectronEfficiencyCorrectionTool" );

  /** Standard destructor */
  ~TElectronEfficiencyCorrectionTool();

  // Main methods
  /** Initialize this class */
  int initialize();

  /** The main calculate method: the actual cuts are applied here */
  int calculate( const PATCore::ParticleDataType::DataType dataType,
                 const unsigned int runnumber,
                 const double cluster_eta,
                 const double et, /* in MeV */
                 std::vector<double>& result,
                 size_t& index_of_corr,
                 size_t& index_of_toys) const;

  /// Add an input file
  inline void addFileName ( const std::string& val ) { 
    m_corrFileNameList.push_back(val); 
  }
  ///MC Toys Helper functions
  inline void bookToyMCScaleFactors(const int nToyMC) {
    m_doToyMC = true;
    m_nToyMC = nToyMC;
  }
  inline void bookCombToyMCScaleFactors(const int nToyMC) {
    m_doCombToyMC = true;
    m_nToyMC = nToyMC;
  }

  ///Helpers to get the binning of the uncertainties
  int getNbins(std::map<float, std::vector<float> >&) const; 
  inline int getNSyst() const {
    return m_nSysMax;
  }
  
  ///Set the Random Seed
  inline void setSeed( const unsigned long int seed) { 
    m_seed = seed; 
  }

private:
  // Private methods
  /// Load all histograms from the input file(s)
  int getHistograms();

  int setupHistogramsInFolder( const TObjArray& dirNameArray, 
                               int lastIdx );

  bool setupUncorrToySyst(std::unordered_map<int, TObjArray>& objs,
                          std::vector<TObjArray>& sysObjs,
                          std::vector< std::vector<TObjArray>>& uncorrToyMCSyst);

   std::vector<TObjArray> buildToyMCTable (const TObjArray &sf, 
                                          const TObjArray &eig, 
                                          const TObjArray& stat, 
                                          const TObjArray& uncorr, 
                                          const std::vector<TObjArray> &corr);

  std::vector<TH2*> buildSingleToyMC(const TH2* sf, 
                                     const TH2* stat, 
                                     const TH2* uncorr, 
                                     const TObjArray& corr,
                                     int& randomCounter);

  TH2* buildSingleCombToyMC(const TH2 *sf, 
                            const TH2* stat, 
                            const TH2* uncorr, 
                            const TObjArray& corr,
                            const int nSys,
                            int& randomCounter);

  void setupTempMapsHelper(TObject* obj,
                           std::unordered_map<int, TObjArray>& objs,
                           std::vector<TObjArray >& sysObjs,int& seenSystematics) ;

  /// Fill and interpret the setup, depending on which histograms are found in the input file(s)
  int setup( const TObjArray& hist,
             std::vector< TObjArray >& histList,
             std::vector< unsigned int >& beginRunNumberList,
             std::vector< unsigned int >& endRunNumberList,
             const int runNumBegin,
             const int runNumEnd) const ;

private :
  ///Flag to control Toys
  bool m_doToyMC;
  bool m_doCombToyMC;
  ///The number of toys
  int m_nToyMC;
  /// The Random seed
  unsigned long int m_seed;
  ///Maximum number of systematics
  int m_nSysMax;
  //The representation of the prepared toy SF tables
  std::vector< std::vector<TObjArray>> m_uncorrToyMCSystFull;
  std::vector< std::vector<TObjArray>> m_uncorrToyMCSystFast;
  /// The list of file name(s)
  std::vector< std::string > m_corrFileNameList;
  /// List of run numbers where histograms become valid for full simulation
  std::vector< unsigned int > m_begRunNumberList;
  /// List of run numbers where histograms stop being valid for full simulation
  std::vector< unsigned int > m_endRunNumberList;
  /// List of run numbers where histograms become valid for fast simulation
  std::vector< unsigned int > m_begRunNumberListFastSim;
  /// List of run numbers where histograms stop being valid for fast simulation
  std::vector< unsigned int > m_endRunNumberListFastSim;    
 /// List of histograms for full Geant4 simulation
  std::unordered_map<int, std::vector< TObjArray > > m_histList;
  std::vector< std::vector< TObjArray > > m_sysList;
  /// List of histograms for fast simulation
  std::unordered_map<int, std::vector< TObjArray > > m_fastHistList;
  std::vector< std::vector< TObjArray > > m_fastSysList;
  //The Random generator class   
  TRandom3 m_Rndm;
}; // End: class definition
} // End: namespace Root

#endif
