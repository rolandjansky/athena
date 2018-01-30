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

  @author Kristin Lohwasser, Karsten Koeneke, Felix Buehrer
  @date   January 2013
  */

// STL includes
#include <vector>
#include <string>
#include <array>
#include <map>
//Root fwd declares
class TKey;
class TH1D;
class TH2D;
// ROOT includes
#include "TRandom3.h"
#include "TObjArray.h"
// Core includes
#include "PATCore/PATCoreEnums.h"
#include "AsgTools/AsgMessaging.h"

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

        /** Finalize this class; everything that should be done after the event loop should go here */
        inline int finalize() { return 1; }

        /** The main salculate method: the actual cuts are applied here */
        const std::vector<double> calculate( const PATCore::ParticleDataType::DataType dataType,
                const unsigned int runnumber,
                const double cluster_eta,
                const double et, /* in MeV */
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
        ///Detail Level
        enum detailLevel{simple,medium,detailed,detailLevelEnd};
        /// Set the detail level 
        inline void setDetailLevel (const int input_detailLevel ) { 
            m_detailLevel = input_detailLevel; 
        }
        ///Set the Random Seed
        inline void setSeed( const unsigned long int seed) { 
            m_seed = seed; 
        }

    private:
        // Private methods
        /// Load all histograms from the input file(s)
        int getHistograms();

        int getHistogramInDirectory( TKey *key );

        int setupHistogramsInFolder( const TObjArray& dirNameArray, 
                int lastIdx );

        void calcDetailLevels(const TH1D *eig,
                std::array<int,detailLevelEnd>& sLevel,
                int& nSys) const ;

        std::vector<TObjArray> buildToyMCTable (const TObjArray &sf, 
                const TObjArray &eig, 
                const TObjArray& stat, 
                const TObjArray& uncorr, 
                const std::vector<TObjArray> &corr);

        std::vector<TH2D*> buildSingleToyMC(const TH2D* sf, 
                const TH2D* stat, 
                const TH2D* uncorr, 
                const TObjArray& corr,
                const std::array<int,detailLevelEnd> sLevel,
                int& randomCounter);

        TH2D* buildSingleCombToyMC(const TH2D *sf, 
                const TH2D* stat, 
                const TH2D* uncorr, 
                const TObjArray& corr,
                const std::array<int,detailLevelEnd> sLevel,
                const int nSys,
                int& randomCounter);

        /// Fill and interpret the setup, depending on which histograms are found in the input file(s)
        int setup( const TObjArray& hist,
                std::vector< TObjArray >& histList,
                std::vector< unsigned int >& beginRunNumberList,
                std::vector< unsigned int >& endRunNumberList,
                const int runNumBegin,
                const int runNumEnd) const ;

    private :
        ///Private data members
        bool m_doToyMC;
        bool m_doCombToyMC;
        //// The detail level
        int m_detailLevel;
        //The number of toys
        int m_nToyMC;
        /// The Random seed
        unsigned long int m_seed;
        ///Maximum number of systematics
        int m_nSysMax;
        // The positions of the efficiency scale factor correlated sustematic uncertainties in the result
        std::vector<int> m_position_corrSys; 
        /// The positions of the toy MC scale factors
        std::vector<int> m_position_uncorrToyMCSF; ///Uncorrelated toy systematics
        ///The representation of the prepared toy SF tables
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
        //The vector holding the keys
        std::vector<int> m_keys;
        /// List of histograms for full Geant4 simulation
        std::map<int, std::vector< TObjArray > > m_histList;
        std::vector< std::vector< TObjArray > > m_sysList;
        /// List of histograms for fast simulation
        std::map<int, std::vector< TObjArray > > m_fastHistList;
        std::vector< std::vector< TObjArray > > m_fastSysList;
        //The Randomw generator class   
        TRandom3 m_Rndm;
    }; // End: class definition

} // End: namespace Root

#endif

