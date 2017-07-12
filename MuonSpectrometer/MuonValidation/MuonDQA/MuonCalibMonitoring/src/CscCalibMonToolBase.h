/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:      CscCalibMonTool.h
// PACKAGE:   MuonCalibMonTool.h
//
// AUTHORS:   Caleb Parnell-Lampen <lampen@physics.arizona.edu>
//
// ********************************************************************

#ifndef CSCCALIBMONITORING_CSCCALIBMONTOOLBASE_H
#define CSCCALIBMONITORING_CSCCALIBMONTOOLBASE_H

#include <set>
#include <string>

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "Rtypes.h"

#include "AthenaMonitoring/ManagedMonitorToolBase.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"

//#include "MuonGeoModel/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonCondData/CscCondDataCollection.h"
#include "MuonCondData/CscCondDataContainer.h"
#include "MuonCondInterface/CscICoolStrSvc.h"

#include "CscCalibData/CscCalibResultContainer.h"


/////////////////////////////////////////////////////////////////////////////

/**
 * @class CscCalibMonToolBase  
 * @brief The CscCalibMonToolBase class.

 * This class is designed to be the base for all classes that do monitoring 
 * of CSC calibration constants that are to be stored in COOL. This base
 * class contains the basic operations to compare the constants to what was
 * previously in the COOL database.  
 *
 * It is designed to be a parent class. A derived class will define the exact
 * parameters that will be compared to COOL, as well as do any futher monitoring.
 * 
 * @author lampen@physics.arizona.edu
 */ 

class CscCalibMonToolBase: public ManagedMonitorToolBase  
{

    public:

        /**
          @brief constructor
         */  
        CscCalibMonToolBase (const std::string & type, const std::string & name, 
                const IInterface* parent);

        /**
          @brief destructor

          Doesn't do anything currently. 
         */
        ~CscCalibMonToolBase();

        /**
          @brief initializes services, tools, etc. Also determines the maximum hash id.
         */
        virtual StatusCode initialize();

        /**
          @brief deletes private members
         */
        virtual StatusCode finalize();

        /**
          @brief Standard function for a @c ManagedMonitorToolBase. 

          This doesn't do anything. Meant to be overwritten in a derived class
         */
        virtual StatusCode bookHistograms();

        /**
          @brief Standard function for a @c ManagedMonitorToolBase. 

          Not used. For most @c ManagedMonitorToolBase classes, this would do event-by-event filling
          of histograms. However we let algorithms in @c CscCalibAlgs do this for us.
         */
        virtual StatusCode fillHistograms();

        /**
          @brief At end of run this processes the calibration results and fills the histograms

          fillHistograms() retrieves a CscCalibResultContainer from the TDS, and loops through
          CscCalibResultCollection objects, each holding info for a calibration parameter.

          It calls handleParameter() for each CscCalibResultCollection, which is used in a 
          derived class to determine what is to be done with each parameter.
         */
        virtual StatusCode procHistograms();

        /**
          @brief Standard function for a @c ManagedMonitorToolBase. 

          Not used. Just expected to be here
         */
        virtual StatusCode checkHists(bool fromFinalise);


    protected:
        unsigned int m_maxHashId;
        unsigned int m_maxChamId;
        unsigned int m_numBad;
        unsigned int m_expectedChamberLayer;

        
        /**Data output options*/
        bool  m_makeHashHists, m_makeAllChan1dHists, m_makeAllChan2dHists,  m_makeLayHists, 
              m_makeLaySummHists, m_makeChamProfs, 
              m_makeChamSummHists, m_makeChamHists,
              m_doAllDetailed, m_doBadDetailed;


        
        /**To keep track of channels that should have more data printed to root file*/
        int m_maxDetailedChannels;
        bool * m_detailedHashIds;

        /**Hash Ids one would expect to be in calibration data*/
        bool m_onlyExpectPrecisionHashIds;
        std::set<int> m_expectedHashIdsAll,m_expectedHashIdsPrec;

        /**Naming info*/
        std::string m_hashName, 
          m_hashTitle,
          m_allChan1dName, 
          m_allChan1dTitle, 
          m_allChan2dName, 
          m_allChan2dTitle, 
          m_chamProfName, 
          m_chamProfTitle, 
          m_layHistName, 
          m_layHistTitle, 
          m_chamSummHistName, 
          m_chamSummHistTitle, 
          m_chamHistName, 
          m_chamHistTitle, 
          m_laySummHistName, 
          m_laySummHistTitle;

        /**Hist colors*/
        int m_histCol, m_histColAlert;

        /**Path of histograms*/
        std::string m_generic_path_csccalibmonitoring;
        std::string m_hashPath, m_allChan2dPath, m_chamProfPath, m_layHistPath, 
          m_chamSummHistPath, m_laySummHistPath;

        /**Keeps track of MonGroups**/
        DataVector<MonGroup> * m_monGroupVec;
        std::map<std::string, MonGroup*> m_monGroupMap;

        /**Calibration result storegate key*/
        std::string m_calibResultKey;

        /** Muon Detector Descriptor*/
        const MuonGM::MuonDetectorManager * m_muon_mgr;

        /** CSC identifier helper*/
        const CscIdHelper * m_cscIdHelper;

        /**Access to COOL database*/
        MuonCalib::CscICoolStrSvc* m_cscCoolSvc;



        /**
          @struct ProcSetupInfo

          Contains info to be passed to procParameter.
         */
        struct ProcSetupInfo
        {

          ProcSetupInfo() : badHist(NULL), overDiff(NULL), overChi2(NULL),
          missingChans(NULL),
          badBin(0), missingBadBin(0), 
          chi2BadBin(0), maxDiff(0.0), chi2Max(9999.), expectedVal(9999.), doChi2(false),
          dbName(""),vals(NULL),errors(NULL),oldVals(NULL),
          diffs(NULL),chi2s(NULL)
          {}

          TH1I *badHist;
          TH1F *overDiff, *overChi2 ;
          TH1F *missingChans;
          int badBin, missingBadBin, chi2BadBin;
          float maxDiff, chi2Max, expectedVal;
          std::set<int> expectedChannels;
          bool doChi2;
          std::string dbName;
          //float *vals,*errors,*oldVals,*diffs,*chi2s;
          std::vector<float> *vals, *errors, *oldVals, *diffs,* chi2s;
        };

        /**Contains an array with data on a per-channel basis, as well as various histograms that 
          give different views of the data in the array.
         */
        struct HistCollection{
          HistCollection(): 
            ignoreY(false), 
          hashHist(NULL), allChan1dHistX(NULL), allChan1dHistY(NULL), allChan2dHist(NULL), chamProf(NULL),  
          layHistVect(NULL), laySummVect(NULL), chamSummVect(NULL), chamHistVect(NULL), abs2d(true)
          {}

          HistCollection(int arraySize, int errorSize = 0): 
            ignoreY(false),
            hashHist(NULL), allChan1dHistX(NULL), allChan1dHistY(NULL), allChan2dHist(NULL), chamProf(NULL),
            layHistVect(NULL), laySummVect(NULL), chamSummVect(NULL), chamHistVect(NULL), abs2d(true)
          {
            data.resize(arraySize);
            if(errorSize)
              errors.resize(errorSize);
          }

          ~HistCollection() {
            if (layHistVect != NULL)
              delete layHistVect;
            if (laySummVect != NULL)
              delete laySummVect;
            if (chamSummVect)
              delete chamSummVect;                
          }
          bool ignoreY;
          std::vector<float>  data;
          std::vector<float> errors;
          std::vector<TH1F *> cscSpec;
          TH1F * hashHist;
          TH1F * allChan1dHistX;
          TH1F * allChan1dHistY;
          TH2F * allChan2dHist;
          TProfile * chamProf;
          std::vector<TH1F*> * layHistVect;
          std::vector<TH1F*> * laySummVect; 
          std::vector<TH1F*> * chamSummVect;
          std::vector<TH1F*> * chamHistVect;
          bool abs2d;
        };

        //Stat bit from database
        bool m_doStatDb;
        HistCollection * m_statDbColl;

    protected:
        /** Initializes the histograms in a histCollection and books them. This and the other booking
          function are used to enforce a central naming convention to the histograms. The @c Name parameters
          are strings that will be input into the names of the historams, and the @c Title parameters into
          the titles.
          @param dataTypeName indicates the dataType, like pedestal, noise, etc.
          @param dataTypeTitle indicates the dataType, like pedestal,noise, etc.
          @param categoryName indicates a category. Useful to specify new or COOL data.
          @param categoryTitle indicates a category. Useful to specify new or COOL data.
          @param axisLabel What the axis corresponding to the data type should be named. The other axis is determined depending on histogram type.
          @param numBins For those histograms whose x-axis is the datatype, and not channels or similar.
          @param lowBound For those histograms whose x-axis is the datatype, and not channels or similar.
          @param highBound For those histograms whose x-axis is the datatype, and not channels or similar.
          @param parDir is a parDirectory to store the resulting histograms in.
          @param toSkip is a bit word that specifies when to allow certain histograms
         */
        virtual StatusCode bookHistCollection(HistCollection *histCollection,
            std::string dataTypeName, std::string dataTypeTitle, std::string categoryName, std::string categoryTitle,
            std::string axisLabel, int numBins, float lowBound, float highBound, std::string parDir ="", 
            uint16_t toSkip =0);

        /** Books a vector of histograms where each one corresponds to a layer of the CSC secbers
          @param histTypeDir The subdirectory where the histogram type should be stored (e.g "layerOverview")
          @param parTypeDir The subdirectory where the parameter type  (e.g ped, noise,etc)
          @param namePrefix Something to put in front of the name. Rest is based on layer.
          @param titlePrefix String to append to the front of the title. Rest is based on layer.
          @param xaxis Title for x-axis.
          @param yaxis Title for y-axis.
          @param chanView If true, x-axis num-bins will be number of channels of a given layer. numbins, highBound, and lowBound will be ignored.
          @param ignoreY Do not initialize histograms from the transverse direction.
          @param numBins number of bins for each histoto
          @param lowBound lower bound for histogram x-axis
          @param highBound higher bound for histogram x-axis
         */
        virtual StatusCode bookLayHists(std::string histTypeDir, std::string parTypeDir,
            std::vector<TH1F*> &vector, 
            std::string namePrefix, std::string titlePrefix,
            std::string xaxis, std::string yaxis,  bool chanView = true, 
            bool ignoreY = false, unsigned int numBins = 100, 
            float lowBound = 0, float highbound = 100);


        /** Books a vector of histograms where each one corresponds to a secber of the CSC secbers
          @param histTypeDir The subdirectory where the histogram type should be stored (e.g "layerOverview")
          @param parTypeDir The subdirectory where the parameter type  (e.g ped, noise,etc)
          @param namePrefix Something to put in front of the name. Rest is based on secber.
          @param titlePrefix String to append to the front of the title. Rest is based on secber.
          @param xaxis Title for x-axis.  @param yaxis Title for y-axis.  @param chanView If true, x-axis num-bins will be number of channels of a given layer. numbins, highBound, and lowBound will be ignored.  @param ignoreY Do not initialize histograms from the transverse direction.
          @param numBins number of bins for each histoto
          @param lowBound lower bound for histogram x-axis
          @param highBound higher bound for histogram x-axis
         */
        virtual StatusCode  bookChamHists(std::string histTypeDir, std::string parTypeDir, 
            std::vector<TH1F*>&  histVector,
            std::string namePrefix, std::string titlePrefix,
            std::string xaxis = "", std::string yaxis = "", bool chanView = true, bool ignoreY = false, 
            unsigned int numBins = 100, float lowBound = 0, float highbound = 100);

        /**Should be written by user in derived class. This is called during procHistograms() and is 
          meant to be used to determine course of action for each parameter type. Doesn't do anything
          in base class. 
          @param parVals Contains all the parameter values of a given type. For example, the pedestal for each channel.
         */
        virtual StatusCode handleParameter(const CscCalibResultCollection *parVals);

        /**Processors a parameter type. Typically called by derived classes' handleParameter. 
          It does several things

          -Fills the histograms in simpleSet
          -Determines if a given channel deviates too much from an expected value (expected
          value is from COOL or specified in simpleSet).
          -Optionally tests the chi^2 value in parVals
          -Fills the std::vectors in simpleSet. These vectors contain info such as the per-channel
          values, difference from old COOL values, and the COOL values themselves.
          -Looks for any expected channels missing in parVals. If so, it mentions it.
          @param parVals Collection of values for a given parameter
          @param simpleSet contains a set of histograms and vectors to be filled.
         */
        virtual StatusCode procParameter(const CscCalibResultCollection *parVals,
            ProcSetupInfo* simpleSet);
        /**Fills the histograms in the histCollection based on the data in the std::vector in
          the HistCollection

         */
        virtual StatusCode copyDataToHists(HistCollection * histCollection);

        /**Run after handleParameter. The derived class can use this to retrive additional calibration
          mode specific histogram from the calibration procedure.
         */
        virtual StatusCode postProc();

        /**produces a unique number for every layer in the detector*/
        virtual int getLayIndex(int measuresPhi, int stationEta, int sector, int layer);
        /**Produces a unique number for every secber in the detector.*/
        virtual int getChamIndex(int measuresPhi, int stationEta, int sector);
        /**Produces a sector number from the phi and size of a station.*/
        virtual int getSector(int stationPhi, int stationSize);
        /** Gets the letter endcap (A or C) for a given eta index (1,-1)*/
        std::string getEndCap(int eta);

        /**
          getGeoPath creates a directory structure for a histogram based upon where it is in the CSC geometry.
          The user fills the data down as far as they like. For example, just filling in eta and sector will provide
          a path for a sector-wide histogram.
          This provides a centralized location to change the layout of the root output file.
         */
        std::string getGeoPath( int eta = -9999, int sector = -9999,  
            int wireLayer = -9999,int measuresPhi = -9999 , int channel =-9999);

        /**
          Geo path creates a directory structure for a histogram based upon its geoPath, 
          the hist type dir (spectrum of values, per-channel values, etc), and the parTypeDir (pedestal, etc).
          This provides a centralized, consistent way of generating the paths
         */
        std::string getFullPath(std::string geoPath, std::string histTypeDir, std::string parTypeDir);

};

#endif
