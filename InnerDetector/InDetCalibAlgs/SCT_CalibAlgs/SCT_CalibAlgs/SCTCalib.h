/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCTCalib.h
*
* @Header file for the SCTCalib class
*
* @author Jose E. Garcia, jose.enrique.garcia@cern.ch
* @author Peter Vankov,   peter.vankov@cern.ch
* @author Kazu Hanagaki,  kazunori.hanagaki@cern.ch
* @author Minoru Hirose,  minoru.hirose@cern.ch
* @author Tim Andeen,     timothy.robert.andeen@cern.ch
* @author Junji Tojo,     junji.tojo@cern.ch
**/

#ifndef SCTCalib_H
#define SCTCalib_H
// STL and boost headers
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <queue>   // for HV trip

//shaun added
#include "SCT_CalibAlgs/ISCT_CalibHistoSvc.h"
#include "SCT_CalibAlgs/ISCT_CalibEvtInfo.h"
#include "SCT_CalibAlgs/ISCT_CalibModuleListSvc.h"

#include "TH1.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h" //member
#include "GaudiKernel/IIncidentSvc.h" //template parameter, so not fwd declared
#include "GaudiKernel/IIncidentListener.h" //baseclass
#include "StoreGate/StoreGateSvc.h"

//Athena
#include "AthenaBaseComps/AthAlgorithm.h"  //baseclass
#include "AthenaKernel/IOVTime.h" //member

// Include Event Info
#include "EventInfo/EventID.h"

// RAW data access
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/SCT3_RawData.h"

//InnerDetector
#include "Identifier/IdentifierHash.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"

// SCT Conditions
#include "SCT_ConditionsServices/ISCT_DCSConditionsSvc.h" //template parameter
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h" //template parameter
#include "SCT_ConditionsServices/ISCT_ReadCalibDataSvc.h"  //template parameter
#include "SCT_ConditionsServices/ISCT_DetectorLevelConditionsSvc.h" //template parameter
#include "SCT_ConditionsServices/ISCT_ByteStreamErrorsSvc.h"  //template parameter

// SCT Cabling
#include "SCT_Cabling/ISCT_CablingSvc.h"  //template parameter

// Local
#include "SCT_CalibAlgs/SCTCalibWriteSvc.h" //template parameter


//Forward declarations
class TFile;
class SCT_CalibChain;
class TTreeIndex;
class TH1F;
class TH1I;
class TH2I;
class TH2D;
class TProfile;
class TProfile2D;
class ITHistSvc;
class ISvcLocator;
class StatusCode;
class EventInfo;
class SCT_PlanePosition;
class Identifier;
class Incident;
namespace InDetDD {
class  SCT_DetectorManager;
}


class SCTCalib : public AthAlgorithm {

    public:
        SCTCalib( const std::string &name, ISvcLocator *pSvcLocator ) ;
        ~SCTCalib();
        StatusCode initialize();
        StatusCode beginRun();
        StatusCode execute();
        StatusCode endRun();
        StatusCode finalize();
        //void handle( const Incident& );

    private:
        ServiceHandle<StoreGateSvc>                     p_sgSvc;
        ITHistSvc *                                     m_thistSvc;
        const SCT_ID*                                   m_pSCTHelper;
        const InDetDD::SCT_DetectorManager*             m_pManager;

        ServiceHandle<SCTCalibWriteSvc>                 m_pCalibWriteSvc;
        ServiceHandle<ISCT_DCSConditionsSvc>            m_DCSConditionsSvc;
        ServiceHandle<ISCT_ConfigurationConditionsSvc>  m_ConfigurationConditionsSvc;
        ServiceHandle<ISCT_ReadCalibDataSvc>            m_ReadCalibDataSvc;
        ServiceHandle<ISCT_DetectorLevelConditionsSvc>  m_MajorityConditionsSvc;
        ServiceHandle<ISCT_CablingSvc>                  m_CablingSvc;

        //shaun added
        ServiceHandle<ISCT_CalibHistoSvc>               m_calibHitmapSvc;
        ServiceHandle<ISCT_CalibHistoSvc>               m_calibBsErrSvc;
        ServiceHandle<ISCT_CalibHistoSvc>               m_calibLbSvc;
        ServiceHandle<ISCT_CalibModuleListSvc>          m_calibModuleListSvc;
        ServiceHandle<ISCT_CalibEvtInfo>                m_calibEvtInfoSvc;

        // Book histograms

        // Methods implemented
        StatusCode prepareHV();
        StatusCode doHVPrintXML(const std::pair<int, int> & timeInterval, const std::pair<int, int> & lbRange, Identifier);
        //

        bool       notEnoughStatistics(const int required, const int obtained, const std::string & histogramName="HIST");
        //
        void       checkHVTrips();
        StatusCode getNoisyStrip();
        StatusCode getDeadStrip();
        StatusCode getNoiseOccupancy();
        StatusCode getRawOccupancy();
        StatusCode getEfficiency();
        StatusCode getBSErrors();
        StatusCode getLorentzAngle();

        // To handle XML file for DB
        StatusCode openXML4DB( std::ofstream&, const char*, const char*, IOVTime, IOVTime ) const;
        StatusCode closeXML4DB( std::ofstream& ) const;
        StatusCode addToXML4DB( std::ofstream&, const Identifier&, const char*, float, const char* ) const;

        // To handle XML file for Summary
        StatusCode openXML4MonSummary( std::ofstream&, const char* ) const;
        StatusCode openXML4DeadSummary( std::ofstream& file, const char* type, int n_Module=0, int n_Link=0, int n_Chip=0, int n_Strip=0 )
        const;
        StatusCode wrapUpXML4Summary( std::ofstream&, const char*, std::ostringstream& ) const;
        StatusCode addToSummaryStr( std::ostringstream&, const Identifier&, const char*, const char*, const char* ) const;

        // For retrieving informations of the run for MONP200 rerated things
        //  unsigned long long getDuration(std::string runStart, std::string runEnd);
        unsigned long long getNevents();

        // For ntuple sorting
        typedef std::multimap<int, int> EvtIndex;
        EvtIndex m_chain_index;
        EvtIndex::const_iterator m_evt_iter;

        // For HV trips
        std::vector< std::queue<int> > m_phvtripQueue;
        std::queue<int> m_prevLBN;
        std::vector<int> m_phvtripRunningTotalInt;
        std::vector<int> m_phvtripProcessedEventsInt;
        std::vector<int> m_phvtripHasItTripped;
        std::vector<int> m_phvtripHasItTripped_prev;
        std::vector< std::vector< std::pair<int, int> > > m_summarytrips;
        std::vector< std::vector< std::pair<int, int> > > m_summarytripslb;

        Identifier m_moduleId;
        Identifier m_waferId;
        int m_tq[100];
        UnsignedIntegerProperty m_curr_time;
        //bool newbin;
        std::ofstream m_ofile, m_gofile;

        UnsignedIntegerProperty m_phvtripFirstTime;
        UnsignedIntegerProperty m_phvtripPrevTime;

        // output files (for dead search & MONP200 related things )
        std::ofstream m_outDeadStrips;
        std::ofstream m_outDeadChips;
        std::ofstream m_outDeadSummary;
        std::ofstream m_outNOSummary;
        std::ofstream m_outROSummary;
        std::ofstream m_outEffSummary;
        std::ofstream m_outBSErrSummary;
        std::ofstream m_outBSErrModule;
        std::ofstream m_outLASummary;

        // Properties configurable from jobO
        IntegerProperty m_runNumber;

        std::string m_runStartTime; // from runInfo.txt
        std::string m_runEndTime;   // from runInfo.txt
        std::string m_LBMax;        // from runInfo.txt

        BooleanProperty m_useDCS;
        BooleanProperty m_useConfiguration;
        BooleanProperty m_useCalibration;
        BooleanProperty m_useMajority;
        BooleanProperty m_useBSError;

        StringArrayProperty m_input_bs;     // list of BS
        StringArrayProperty m_input_hist;   // list of HIST (assuming 1 file)

        BooleanProperty m_readBS;         // True if BS is used
        BooleanProperty m_histBefore2010; // True if HIST is from 2009 or earlier

        BooleanProperty m_doHitMaps;
        IntegerProperty m_nLbsMerged;
        BooleanProperty m_readHitMaps;
        BooleanProperty m_doBSErrors;
        BooleanProperty m_doNoisyStrip;
        BooleanProperty m_doNoisyLB;
        BooleanProperty m_doHV;
        BooleanProperty m_doDeadStrip;
        BooleanProperty m_doDeadChip;
        BooleanProperty m_doNoiseOccupancy;
        BooleanProperty m_doRawOccupancy;
        BooleanProperty m_doEfficiency;
        BooleanProperty m_doBSErrorDB;
        BooleanProperty m_doLorentzAngle;
        BooleanProperty m_writeToCool;
        //reinstated 21 May, sroe
        BooleanProperty         m_noisyUpdate;
        BooleanProperty         m_noisyUploadTest;
        FloatProperty           m_noisyModuleAverageInDB; // Average number of modules with at least 1 noisy strip in COOL
        IntegerProperty         m_noisyStripLastRunInDB;  // Number of noisy strips of the last run uploaded to COOL
        FloatProperty			m_noisyStripAverageInDB;  // Number of noisy strips of the last run uploaded to COOL
        IntegerProperty         m_noisyModuleList;
        FloatProperty           m_noisyModuleDiff;
        FloatProperty			m_noisyStripDiff;
        double m_numOfEventsProcessed;
        int    m_numOfLBsProcessed;

        UnsignedIntegerProperty m_noisyMinStat;
        BooleanProperty         m_noisyStripAll;
        BooleanProperty         m_noisyStripThrDef;
        FloatProperty           m_noisyStripThrOffline;
        FloatProperty           m_noisyStripThrOnline;
        BooleanProperty         m_noisyWaferFinder;
        BooleanProperty         m_noisyWaferWrite;
        BooleanProperty         m_noisyWaferAllStrips;
        FloatProperty           m_noisyWaferThrBarrel;
        FloatProperty           m_noisyWaferThrECA;
        FloatProperty           m_noisyWaferThrECC;
        FloatProperty           m_noisyWaferFraction;
        FloatProperty           m_noisyChipFraction;

        IntegerProperty m_maxtbins;
        int             m_maxq;
        double          m_absolutetriplimit;
        double          m_relativetriplimit;
        BooleanProperty m_outputlowhits;
        IntegerProperty m_lowHitCut;

        UnsignedIntegerProperty m_deadStripMinStat;
        UnsignedIntegerProperty m_deadStripMinStatBusy;
        UnsignedIntegerProperty m_deadChipMinStat;
        UnsignedIntegerProperty m_deadStripSignificance;
        UnsignedIntegerProperty m_deadChipSignificance;
        FloatProperty           m_busyThr4DeadFinding;
        FloatProperty           m_noisyThr4DeadFinding;
        BooleanProperty         m_deadChipUploadTest;
        BooleanProperty         m_deadStripUploadTest;


        BooleanProperty         m_noiseOccupancyTriggerAware;
        UnsignedIntegerProperty m_noiseOccupancyMinStat;
        UnsignedIntegerProperty m_rawOccupancyMinStat;
        UnsignedIntegerProperty m_efficiencyMinStat;
        UnsignedIntegerProperty m_BSErrorDBMinStat;
        UnsignedIntegerProperty m_LorentzAngleMinStat;

        BooleanProperty  m_LorentzAngleDebugMode;

        std::string m_tagID4NoisyStrips;
        std::string m_tagID4DeadStrips;
        std::string m_tagID4DeadChips;
        std::string m_tagID4NoiseOccupancy;
        std::string m_tagID4RawOccupancy;
        std::string m_tagID4Efficiency;
        std::string m_tagID4BSErrors;
        std::string m_tagID4LorentzAngle;

        std::string m_badStripsAllFile;     // Output XML for all noisy strips
        std::string m_badStripsNewFile;     // Output XML for newly found noisy strips (i.e. not in config/calib data)
        std::string m_badStripsSummaryFile; // Output XML for summary info from noisy strip search

        std::string m_badModulesFile;       // Output XML for HV trips

        std::string m_deadStripsFile;       // Output XML for dead strips
        std::string m_deadChipsFile;        // Output XML for dead chips
        std::string m_deadSummaryFile;      // Output XML for summary of Dead Finding

        std::string m_noiseOccupancyFile;        // Output XML for noise occupancy
        std::string m_noiseOccupancySummaryFile; // Output XML for summary of noise occupancy
        std::string m_rawOccupancySummaryFile;   // Output XML for summary of raw occupancy
        std::string m_efficiencySummaryFile;     // Output XML for summary of efficiency
        std::string m_efficiencyModuleFile;      // Output XML for efficiency
        std::string m_BSErrorSummaryFile;        // Output XML for summary of BS Errors
        std::string m_BSErrorModuleFile;        // Output XML for summary of BS Errors
        std::string m_LorentzAngleFile;        // Output XML for noise occupancy
        std::string m_LorentzAngleSummaryFile; // Output XML for summary of lorentz angle

        // Event info
        const EventInfo* m_evt;

        // Event related
        unsigned long long m_numberOfEvents;
        unsigned long long m_numberOfEventsHist; // For number of events from HIST
        unsigned long long m_eventCounter;
        unsigned long long m_maxentry;
        int                m_timeStampBegin;
        int                m_timeStampEnd;
        std::string        m_utcBegin;
        std::string        m_utcEnd;
        int                m_LBBegin;
        int                m_LBEnd;
        int                m_LBRange;
        IOVTime            m_iovStart;
        IOVTime            m_iovStop;

        // Input
        TFile*  m_inputHist;    // Monitoring histograms
        bool    m_readHIST;   // True if HIST is used

        // Histograms
        std::vector<TH1F *>       m_phitmapHistoVector;
        std::vector<TH1F *>       m_pLBHistoVector;
        std::vector<TH1F *>       m_pBSErrorsHistoVector;
        std::vector<TProfile2D *> m_pnoiseoccupancymapHistoVector;
        std::vector<TProfile2D *> m_pnoiseoccupancymapHistoVectorECp;
        std::vector<TProfile2D *> m_pnoiseoccupancymapHistoVectorECm;
        std::vector<TProfile *>   m_h_phiVsNstripsSideHistoVector;

        // SCT specific numbers
        enum { nbins       = 6*128, firstStrip     =  0, lastStrip     = nbins-1,
               ntimeBins   =     3, firstTimeBin   =  0, lastTimeBin   =  2,
               n_hitBins   =    50, first_hitBin   =  0, last_hitBin   = 50,
               n_etaBins   =    13, first_etaBin   = -6, last_etaBin   =  6,
               n_phiBins   =    56, first_phiBin   =  0, last_phiBin   = 55,
               n_barrels   =     4, first_barrel   =  0, last_barrel   =  3,
               n_disks     =     9, first_disk     =  0, last_disk     =  8,
               n_etaBinsEC =     3, first_etaBinEC =  0, last_etaBinEC =  2,
               n_phiBinsEC =    52, first_phiBinEC =  0, last_phiBinEC = 51
             };

        // Additional info on SCT specific numbers
        enum { n_chipPerSide = 6, n_chipPerModule = 12, n_stripPerChip = 128, n_etaInBarrel = 12,
               n_phiBinsB0 = 32, n_phiBinsB1 = 40, n_phiBinsB2 = 48, n_phiBinsB3 = 56,
               n_phiBinsECShort = 40, n_phiBinsECMiddle = 40, n_phiBinsECOuter = 52, n_elements=8176,
               n_BSErrorType = 15, firstBSErrorType = 0, lastBSErrorType = 14
             };

        int m_MAXHASH;
        SCT_ID::const_id_iterator m_waferItrBegin;
        SCT_ID::const_id_iterator m_waferItrEnd;
        template<class S>
        bool retrievedService(S & service) {
            //msg(MSG::INFO)<<"Retrieving: "<<service.name()<<endmsg;
            if (service.retrieve().isFailure() ) return msg(MSG::ERROR)<<"Unable to retrieve "<<service.name()<<endmsg, false;
            return true;
        }
        std::string
        xmlChannelNoiseOccDataString(const Identifier & waferId,  const float occupancy, const SCT_SerialNumber & serial);

        std::string
        xmlChannelEfficiencyDataString(const Identifier & waferId,  const float efficiency, const SCT_SerialNumber & serial);

        std::pair<int, bool>
        getNumNoisyStrips( const Identifier& waferId ) const;

        StatusCode
        addStripsToList( Identifier& waferId, std::set<Identifier>& stripIdList, bool isNoisy, bool isNew );

        StatusCode
        writeModuleListToCool( const std::map< Identifier, std::set<Identifier> >& moduleListAll,
                               const std::map< Identifier, std::set<Identifier> >& moduleListNew,
                               const std::map< Identifier, std::set<Identifier> >& moduleListRef ) const;
        std::string
        getStripList( const std::set<Identifier>& stripIdList ) const;

        std::set<Identifier>
        getOverlapStripList( const std::set<Identifier>& stripAllIdList, const std::set<Identifier>& stripRefIdList ) const;

        StatusCode
        noisyStripsToXml( const std::map< Identifier, std::set<Identifier> >& moduleList, const std::string& badStripsFile ) const;

        StatusCode
        noisyStripsToSummaryXml( const std::map< Identifier, std::set<Identifier> >& moduleListAll,
                                 const std::map< Identifier, std::set<Identifier> >& moduleListNew,
                                 const std::map< Identifier, std::set<Identifier> >& moduleListRef,
                                 const std::string& badStripsFile) const;
        /* StatusCode  */
        /* noisyStripsToSummaryXmlFake( const std::string& badStripsFile) const; */
        std::set<int>
        getNoisyChips( const std::set<Identifier>& stripIdList ) const;
        //
        std::pair< std::string, float >
        getNoisyLB( const Identifier& moduleId, int& chipId ) const;

        std::string
        getLBList( const std::set<int>& LBList ) const;
}; // end of class

#endif



