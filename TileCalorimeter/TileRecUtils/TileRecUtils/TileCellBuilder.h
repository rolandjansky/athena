/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILECELLBUILDER_H
#define TILERECUTILS_TILECELLBUILDER_H

/********************************************************************
 *
 * NAME:     TileCellBuilder
 * PACKAGE:  offline/TileCalorimeter/TileRecUtils
 *
 * AUTHOR :  A. Solodkov
 * CREATED:  20-Jun-2004
 *
 * PURPOSE:  Create Cells from RawChannels and store them in container
 *
 *  Input: TileRawChannel (one by one or from TileRawChannelContainer)
 *  Output: Container or collection with TileCells
 *  Parameters:
 *    TileRawChannelContainer - Name of input container
 *   
 ********************************************************************/

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Atlas includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/HWIdentifier.h"
#include "AthenaKernel/IOVSvcDefs.h"

// Calo includes
#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloConditions/CaloAffectedRegionInfo.h"

// Tile includes
#include "TileEvent/TileCellContainer.h"
#include "TileIdentifier/TileFragHash.h"
#include "TileIdentifier/TileRawChannelUnit.h"
//#include "TileRecUtils/TileBeamInfoProvider.h"

// forward declarations
class TileID;
class TileTBID;
class TileHWID;
class TileCablingService;
class TileCell;
class TileRawChannel;
class MbtsDetDescrManager;
class TileDetDescrManager;
class TileCellCollection;
class CaloCellContainer;
class ITileBadChanTool;
class TileCondToolEmscale;
class TileCondToolTiming;
class ITileRawChannelTool;
class TileBeamInfoProvider;
class TileDQstatus;


// C++ STL includes
#include <string>
#include <vector>

/**
 @class TileDrawerEvtStatus
 @brief This class keep detailed status info about one drawer in a given event
 
 */
class TileDrawerEvtStatus {
  public:
    int nChannels;
    int nMaskedChannels;
    int nBadQuality;
    int nOverflow;
    int nUnderflow;
    int nSomeSignal;
};

/**
 @class TileDrawerRunStatus
 @brief This class keep detailed status info about one drawer in whole run
 
 */
class TileDrawerRunStatus {
  public:
    int drawerAbsent;
    int drawerMasked;
    int channelsMasked;
};

/**
 @class TileCellBuilder
 @brief This class creates Cells from RawChannels and stores them in a container
 
 */
class TileCellBuilder: public AthAlgTool, virtual public ICaloCellMakerTool {
    friend class DoubleVectorIterator;
  public:
    TileCellBuilder(const std::string& type, const std::string& name, const IInterface* parent); //!< Contructor

    virtual ~TileCellBuilder(); //!< Destructor

    virtual StatusCode initialize();                     //!< initialize mehtod

    //!< Callback added to handle Data-driven GeoModel initialisation
    virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

    void reset(bool fullSizeCont, bool printReset = true); //!< Method to reset the options of the TileCellContainer

    /**
     This method sets the type and unit for the TileRAwChannels. It
     might be called from TileROD_Decoder
     otherwise it isn't needed - type and unit are available from 
     TileRawChannelContainer itself (see TileCellBuilder::process() below)
     */
    void set_type_and_unit(TileFragHash::TYPE type = TileFragHash::Default
        , TileRawChannelUnit::UNIT unit = TileRawChannelUnit::ADCcounts);

    virtual StatusCode finalize(); //!< finalize method

    virtual StatusCode process(CaloCellContainer* theCellContainer); // method to process all raw channels and store them in container

    template<class ITERATOR, class COLLECTION>
    void build(const ITERATOR & begin, const ITERATOR & end, COLLECTION * coll); //!< method to process raw channels from a given vector and store them in collection

    /** method to check if channels are good or bad. Puts zero if both channels are bad
     or recovers from single-channel failure. It returns true if cell was changed, false otherwise
     */
    bool maskBadChannel(TileCell* pCell, HWIdentifier hwid);
    bool maskBadChannels(TileCell* pCell);

    //AlgTool InterfaceID
    static const InterfaceID& interfaceID();
    //static const InterfaceID& interfaceID() { return ICaloCellMakerTool; };

  protected:
    // FIXME: Get rid of this abomination.
    friend class TileHid2RESrcID;

    // properties
    std::string m_rawChannelContainer;
    std::string m_infoName;
    std::string m_MBTSContainer;
    std::string m_E4prContainer;
    std::string m_dspRawChannelContainer;

    float m_eneForTimeCut;        //!< keep time for channels with energy above cut
    float m_eneForTimeCutMBTS;    //!< similar cut for MBTS in pC
    float m_zeroEnergy;           //!< energy to store in every PMT if both PMT are bad
    int m_qualityCut;           //!< cut on channel quality (set energy to m_zeroEnergy for them)
    bool m_correctTime;          //!< should time be corrected (deltat added from CondDB)
    bool m_correctAmplitude; //!< If true, amplitude is corrected by parabolic function (needed for OF without iterations)
    bool m_of2;              //!< If true, assume OF2 method for amplitude correction, otherwise - OF1
    bool m_mergeChannels;        //!< If true, missing raw channels are taken from DSP container

    // thresholds for parabolic amplitude correction
    float m_ampMinThresh;   //!< correct amplitude if it's above amplitude threshold (in ADC counts)
    float m_timeMinThresh;                //!< correct amplitude is time is above time min threshold
    float m_timeMaxThresh;                //!< correct amplitude is time is below time max threshold

    float m_minEneChan[3];        //!< channel energy thresholds for masking (normal,gap,mbts)
    float m_eThreshold;           //!< cell energy threshold to consider the cell
    float m_maxTimeDiff;          //!< maximum time difference between the PMTs in the cell
    float m_maxTime;              //!< maximum time for the PMTs in the cels
    float m_minTime;              //!< minimum time for the PMTs in the cels
    float m_maxChi2;              //!< maximum chi2 for the PMTs in the cels
    float m_minChi2;              //!< minimum chi2 for the PMTs in the cels
    bool m_thresholdNotSet;      //!< bool variable to check whether some threshold have been set
    bool m_fullSizeCont;
    bool m_maskBadChannels;      //!< if true=> bad channels are masked
    bool m_fakeCrackCells;       //!< if true=> fake E3/E4 cells added
    int  m_skipGain;             //!< for two-gain calib runs skip one of two gains

    const TileID* m_tileID;   //!< Pointer to TileID
    const TileTBID* m_tileTBID; //!< Pointer to TileTBID
    const TileHWID* m_tileHWID; //!< Pointer to TileHWID
    const TileCablingService* m_cabling; //!< TileCabling instance
    const TileDQstatus* m_DQstatus;

    ToolHandle<ITileBadChanTool> m_tileBadChanTool; //!< Tile Bad Channel tool
    ToolHandle<TileCondToolEmscale> m_tileToolEmscale; //!< main Tile Calibration tool
    ToolHandle<TileCondToolTiming> m_tileToolTiming;  //!< Tile Timing Calibration tool
    ToolHandle<TileBeamInfoProvider> m_beamInfo; //!< Beam Info tool to get the DQ Status object
    ToolHandleArray<ITileRawChannelTool> m_noiseFilterTools; //!< noise filter tools to apply

    const TileDetDescrManager* m_tileMgr; //!< Pointer to TileDetDescrManager
    const MbtsDetDescrManager* m_mbtsMgr; //!< Pointer to MbtsDetDescrManager

    std::vector<TileCell*> m_allCells;  //!< vector to of pointers to TielCells
    TileCellContainer* m_MBTSCells;     //!< Pointer to MBTS cell container
    TileCellContainer* m_E4prCells;     //!< Pointer to E4'  cell container

    TileFragHash::TYPE m_RChType;        //!< Type of TileRawChannels (Fit, OF2, etc.)
    TileRawChannelUnit::UNIT m_RChUnit;  //!< Unit for TileRawChannels (ADC, pCb, etc.)
    //unsigned int m_bsflags;              //!< other flags stored in TileRawChannelContainer
    float m_maxTimeCorr;                 //!< max possible time when time correction is applied

    TileDrawerEvtStatus m_drawerEvtStatus[5][64]; //!< status of every drawer in every event
    TileDrawerRunStatus m_drawerRunStatus[5][64]; //!< overall status of drawer in whole run
    int m_eventErrorCounter[4]; //!< number of events with no errors(0), warnings(1), error(2), total(3)

    std::vector<CaloAffectedRegionInfo> m_affectedRegionInfo_global;
    std::vector<CaloAffectedRegionInfo> m_affectedRegionInfo_current_run;

    int m_useDemoCabling;
    
    void correctCell(TileCell* pCell, int correction, int pmt, int gain, float ener, float time,
        unsigned char iqual, unsigned char qbit, int ch_type); //!< Compute calibrated energy, time, etc. for TileCell and adjust it.

    unsigned char iquality(float qual)  {//!< method to compute the cell quality
         return std::min(255, abs((int) qual));
    } // keep quality within 8 bits make it "unsigned char"

    unsigned char qbits(int ros, int drawer, bool count_over, bool good_time, bool good_ener,
        bool overflow, bool underflow, bool good_overflowfit); //!< method to compute the cell quality bits

    template<typename T, typename V>
    class DoubleVectorIterator {
        T* first;
        TileFragHash::TYPE typ1;
        TileRawChannelUnit::UNIT uni1;
        float cut1;
        bool amp1;
        bool tim1;
        bool of21;
        T* second;
        TileFragHash::TYPE typ2;
        TileRawChannelUnit::UNIT uni2;
        float cut2;
        bool amp2;
        bool tim2;
        bool of22;
        TileCellBuilder* ptr;
        int pos;
        typedef typename T::iterator itr_type;
        itr_type itr;

      public:

        DoubleVectorIterator(T* f, TileFragHash::TYPE y1, TileRawChannelUnit::UNIT u1, float c1, bool a1, bool t1, bool o1
                           , T* s, TileFragHash::TYPE y2, TileRawChannelUnit::UNIT u2, float c2, bool a2, bool t2, bool o2
                           , TileCellBuilder* b, int p)
            : first(f), typ1(y1), uni1(u1), cut1(c1), amp1(a1), tim1(t1), of21(o1)
            , second(s), typ2(y2), uni2(u2), cut2(c2), amp2(a2), tim2(t2), of22(o2)
            , ptr(b), pos(p) {

          if (first->begin() != first->end() && pos < 1) {
            pos = 0;
            itr = first->begin();
          } else if (second->begin() != second->end() && pos < 2) {
            pos = 1;
            itr = second->begin();
            // set parameters for second vector
            ptr->m_RChType = typ2;
            ptr->m_RChUnit = uni2;
            ptr->m_maxTimeCorr = cut2;
            ptr->m_correctAmplitude = amp2;
            ptr->m_correctTime = tim2;
            ptr->m_of2 = of22;
          } else {
            pos = 2;
            itr = second->end();
          }
        }

        DoubleVectorIterator(const DoubleVectorIterator& i) = default;
        DoubleVectorIterator& operator=(const DoubleVectorIterator& other) = default;


        bool operator!=(const DoubleVectorIterator& i) {
          if (pos != i.pos || itr != i.itr) return true;
          else return false;
        }

        V& operator*() const { return (*itr); }
        V* operator->() const { return (*itr); }

        DoubleVectorIterator& operator++() {
          switch (pos) {
            case 0:
              if (itr != first->end()) ++itr;
              if (itr != first->end()) break;
              itr = second->begin();
              pos = 1;
              // set parameters for second vector
              ptr->m_RChType = typ2;
              ptr->m_RChUnit = uni2;
              ptr->m_maxTimeCorr = cut2;
              ptr->m_correctAmplitude = amp2;
              ptr->m_correctTime = tim2;
              ptr->m_of2 = of22;
              if (itr != second->end()) break;
              pos = 2;
              // recover parameters for first vector
              ptr->m_RChType = typ1;
              ptr->m_RChUnit = uni1;
              ptr->m_maxTimeCorr = cut1;
              ptr->m_correctAmplitude = amp1;
              ptr->m_correctTime = tim1;
              ptr->m_of2 = of21;
              break;
            case 1:
              if (itr != second->end()) ++itr;
              if (itr != second->end()) break;
              pos = 2;
              // recover parameters for first vector
              ptr->m_RChType = typ1;
              ptr->m_RChUnit = uni1;
              ptr->m_maxTimeCorr = cut1;
              ptr->m_correctAmplitude = amp1;
              ptr->m_correctTime = tim1;
              ptr->m_of2 = of21;
              break;
            default:
              break;
          }
          return *this;
        }
    };

    enum CELL_CHANNEL {E1_CHANNEL = 12};
    enum CELL_TOWER {E1_TOWER = 10};
    bool m_run2;
};

#endif
