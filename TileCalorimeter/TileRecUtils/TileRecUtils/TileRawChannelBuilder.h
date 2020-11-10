/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_ITILERAWCHANNELBUILDER_H
#define TILERECUTILS_ITILERAWCHANNELBUILDER_H

/********************************************************************
 *
 * NAME:     TileRawChanelBuilder
 * PACKAGE:  offline/TileCalorimeter/TileRecUtils
 *
 * AUTHOR :  K. Gellerstedt
 * CREATED:  Nov 17 2003
 *
 * PURPOSE:  Interface for builder algorithms
 *
 *  Input: TileDigitsContainer
 *  Output: Container with TileRawChannels
 *  Parameters:
 *    TileRawChannelContainer - Name of output container
 *   
 ********************************************************************/

// Tile includes
#include "TileIdentifier/TileFragHash.h"
#include "TileIdentifier/TileRawChannelUnit.h"
#include "TileEvent/TileMutableRawChannelContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsCollection.h"
#include "TileEvent/TileDQstatus.h"
#include "TileRecUtils/ITileRawChannelTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondIdTransforms.h"
#include "TileConditions/TileCablingSvc.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"

//Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/WriteHandleKey.h"

#include <utility>
#include <vector>
#include <memory>

class TileDigits;
class TileRawChannel;
class TileID;
class TileHWID;
class TileInfo;
class StoreGateSvc;

typedef std::vector<std::pair<TileRawChannel*, const TileDigits*> > Overflows_t;

class TileRawChannelBuilder: public AthAlgTool {
  public:
    TileRawChannelBuilder(const std::string& type, const std::string& name,
        const IInterface* parent);

    virtual ~TileRawChannelBuilder();

    // Algorithm tool virtual methods
    virtual StatusCode initialize();
    //virtual StatusCode execute();
    virtual StatusCode finalize();

    /**
     * Create container in SG with name given by
     * parameter (m_rawChannelContainerKey)
     */
    virtual StatusCode createContainer();

    /**
     * Builder virtual method to be implemented by subclasses
     * @param digits Pointer to TileDigitsContainer
     *
     */
    virtual TileRawChannel* rawChannel(const TileDigits* digits);

    /**
     * Commit RawChannelContiner in SG and make const
     */
    virtual StatusCode commitContainer();

    void initLog();
    void endLog();

    // process one digit and store result in internal container
    StatusCode build (const TileDigits* digits) {
      ATH_CHECK( m_rawChannelCnt->push_back(std::unique_ptr<TileRawChannel>(rawChannel(digits))) );
      return StatusCode::SUCCESS;
    }

    // process all digits from one collection and store results in internal container
    StatusCode build (const TileDigitsCollection* collection);

    // process digits from a given vector and store results in internal container
    template<class ITERATOR>
    StatusCode build (const ITERATOR & begin, const ITERATOR & end) {
      for (ITERATOR rawItr = begin; rawItr != end; ++rawItr) {
        ATH_CHECK( m_rawChannelCnt->push_back(rawChannel((*rawItr))) );
      }
      return StatusCode::SUCCESS;
    }

    // process digits from a given vector and store results in collection
    template<class ITERATOR, class COLLECTION>
    StatusCode build (const ITERATOR & begin, const ITERATOR & end, COLLECTION * coll) {
      initLog();
      for (ITERATOR rawItr = begin; rawItr != end; ++rawItr) {
        ATH_CHECK( coll->push_back(rawChannel((*rawItr))) );
      }
      endLog();
      return StatusCode::SUCCESS;
    }

    /**
     * AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID();

    static double correctAmp(double phase, bool of2 = true); //!< Amplitude correction factor according to the time when using weights for tau=0 without iterations

    static double correctTime( double phase, bool of2 = true); //!< Time correction factor

    static int CorruptedData(int ros, int drawer, int channel, int gain,
			     const std::vector<float> & digits, float &dmin, float &dmax, float ADCmaxMinusEps, float ADCmaskValueMinusEps);

    static const char* BadPatternName(float ped);

    void resetDrawer();
    void resetOverflows(void);
    Overflows_t& getOverflowedChannels(void);
    std::string getTileRawChannelContainerID(void);

  protected:
    // FIXME: Get rid of this abomination.
    friend class TileHid2RESrcID;

    // properties
    // DQ status.
    SG::ReadHandleKey<TileDQstatus> m_DQstatusKey{this, "TileDQstatus", 
                                                  "TileDQstatus", 
                                                  "TileDQstatus key"};

    // DSP container.  Only used of m_useDSP is set.
    SG::ReadHandleKey<TileRawChannelContainer> m_DSPContainerKey
      {this, "DSPContainer", "",  "DSP Container key"};

  // name of TDS container with output TileRawChannels
    SG::WriteHandleKey<TileRawChannelContainer> m_rawChannelContainerKey{this,"TileRawChannelContainer","TileRawChannelFiltered",
                                                                         "Output Tile raw channels container key"};

    // RawChannelContainer
    std::unique_ptr<TileMutableRawChannelContainer> m_rawChannelCnt;

    // parameters for RawChannelContainer
    TileFragHash::TYPE m_rChType;
    TileRawChannelUnit::UNIT m_rChUnit;
    unsigned int m_bsflags;
    // parameter to determine which sample to start with the analysis
    int m_firstSample;
    // Should energy be calibrated
    bool m_calibrateEnergy;

    // Should time be calibrated (delta added from laser run)
    bool m_correctTime;


    // Use DSP noise correction for incomplete containers
    bool m_useDSP;

    // thresholds for parabolic amplitude correction (used in optimal filter without iterations)
    float m_ampMinThresh;   //!< correct amplitude if it's above amplitude threshold (in ADC counts)
    float m_timeMinThresh;   //!< correct amplitude is time is above time min threshold
    float m_timeMaxThresh;   //!< correct amplitude is time is below time max threshold

    // runType used to override value of trigType in event
    int m_runType;

    // Tile objects
    const TileID* m_tileID;
    const TileHWID* m_tileHWID;

    ToolHandleArray<ITileRawChannelTool> m_noiseFilterTools{this,
        "NoiseFilterTools", {}, "Tile noise filter tools"};

    ToolHandle<TileCondToolEmscale> m_tileToolEmscale{this,
        "TileCondToolEmscale", "TileCondToolEmscale", "Tile EM scale calibration tool"};

    ToolHandle<TileCondToolTiming> m_tileToolTiming{this,
        "TileCondToolTiming", "TileCondToolTiming", "Tile timing tool"};

    ToolHandle<TileCondIdTransforms> m_tileIdTransforms{this,
        "TileCondIdTransforms", "TileCondIdTransforms",
        "Tile tool to tranlate hardware identifier to the drawerIdx, channel, and adc"};

    /**
     * @brief Name of Tile cabling service
     */
    ServiceHandle<TileCablingSvc> m_cablingSvc{ this,
        "TileCablingSvc", "TileCablingSvc", "The Tile cabling service"};

    int m_trigType;
    bool m_idophys;   // Phys fitting
    bool m_idolas; // Laser fitting
    bool m_idoped; // Ped run, phys fitting
    bool m_idocis; // CIS fitting
    int m_cischan; // which channel is fired by CIS
    double m_capdaq; // Capacitor

    unsigned int m_evtCounter;
    unsigned int m_chCounter;

    int m_nChL;
    int m_nChH;
    double m_RChSumL;
    double m_RChSumH;

    Overflows_t m_overflows;
    int m_dataPoollSize;

    static const int MAX_CHANNELS = 48;
    static const int MAX_DMUS = 16;
    int m_error[MAX_CHANNELS];
    int m_lastDrawer = -1;
    bool m_badDrawer = false;
    
    bool m_notUpgradeCabling;

    // TileInfo
    std::string m_infoName;
    const TileInfo* m_tileInfo;
    int m_i_ADCmax;
    float m_f_ADCmax;
    int m_i_ADCmaxPlus1;
    float m_f_ADCmaxPlus1;
    float m_ADCmaxMinusEps;
    float m_ADCmaskValueMinusEps; //!< indicates channels which were masked in background dataset

private:
    // find all bad patterns in a drawer and fill internal static arrays
    void fill_drawer_errors(const EventContext& ctx,
                            const TileDigitsCollection* collection);

};

#endif
