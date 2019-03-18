/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
Summary.h  -  description
-------------------
begin                : Tue Feb 17 2004
email                : edward.moyse@cern.ch
***************************************************************************/


#ifndef TRKTRACKSUMMARY_H
#define TRKTRACKSUMMARY_H

#include <vector>
#include <iostream>
#include <bitset>

class MsgStream;
class TrackSummaryCnv_p1;
class TrackSummaryCnv_p2;

namespace InDet {
    class InDetTrackSummaryHelperTool;
}

namespace Muon {
    class MuonTrackSummaryHelperTool;
}

namespace Trk {


//forward declarations
    class Track;
    class InDetTrackSummary;
    class MuonTrackSummary;

/** enumerates the different types of information stored in Summary. Use in get(const SummaryType type), for for example, summary.get(numOfPixelHits)
When adding a new transient information type, please make sure to increase numberOfTrackSummaryTypes.*/
enum SummaryType {
        // --- Inner Detector
        numberOfContribPixelLayers      =29,  //!< number of contributing layers of the pixel detector
        numberOfBLayerHits              = 0,  //!< these are the hits in the first pixel layer, i.e. b-layer
        numberOfBLayerOutliers          =31,  //!< number of blayer outliers  
        numberOfBLayerSharedHits        =16,  //!< number of Pixel b-layer hits shared by several tracks.
        numberOfBLayerSplitHits         =43,  //!< number of Pixel b-layer hits split by cluster splitting 
	expectBLayerHit                 =42,  //!< Do we expect a b-layer hit for this track?
        expectInnermostPixelLayerHit                 =52,  //!< Do we expect a 0th-layer hit for this track?
	numberOfInnermostPixelLayerHits              =53,  //!< these are the hits in the 0th pixel layer?
        numberOfInnermostPixelLayerOutliers          =54,  //!< number of 0th layer outliers  
        numberOfInnermostPixelLayerSharedHits        =55,  //!< number of Pixel 0th layer hits shared by several tracks.
        numberOfInnermostLayerSplitHits         =56,  //!< number of Pixel 0th layer hits split by cluster splitting 
	expectNextToInnermostPixelLayerHit                 =57,  //!< Do we expect a 1st-layer hit for this track?
	numberOfNextToInnermostPixelLayerHits              = 58,  //!< these are the hits in the 1st pixel layer
        numberOfNextToInnermostPixelLayerOutliers          =59,  //!< number of 1st pixel layer outliers  
        numberOfNextToInnermostPixelLayerSharedHits        =60,  //!< number of Pixel 1st layer hits shared by several tracks.
        numberOfNextToInnermostLayerSplitHits         =61,  //!< number of Pixel 1st layer hits split by cluster splitting 
        numberOfPixelHits               = 2,  //!< these are the pixel hits, including the b-layer
        numberOfPixelOutliers           =41,  //!< these are the pixel outliers, including the b-layer
        numberOfPixelHoles              = 1,  //!< number of pixel layers on track with absence of hits
        numberOfPixelSharedHits         =17,  //!< number of Pixel all-layer hits shared by several tracks.
        numberOfPixelSplitHits          =44,  //!< number of Pixel all-layer hits split by cluster splitting
        numberOfGangedPixels            =14,  //!< number of pixels which have a ganged ambiguity.
        numberOfGangedFlaggedFakes      =32,  //!< number of Ganged Pixels flagged as fakes
        numberOfPixelDeadSensors        =33,  //!< number of dead pixel sensors crossed
        numberOfPixelSpoiltHits         =35,  //!< number of pixel hits with broad errors (width/sqrt(12))
	numberOfDBMHits                 =63,  //!< number of DBM Hits
        numberOfSCTHits                 = 3,  //!< number of hits in SCT
        numberOfSCTOutliers             =39,  //!< number of SCT outliers
        numberOfSCTHoles                = 4,  //!< number of SCT holes
        numberOfSCTDoubleHoles          =28,  //!< number of Holes in both sides of a SCT module
        numberOfSCTSharedHits           =18,  //!< number of SCT hits shared by several tracks.
        numberOfSCTDeadSensors          =34,  //!< number of dead SCT sensors crossed
        numberOfSCTSpoiltHits           =36,  //!< number of SCT hits with broad errors (width/sqrt(12))
        numberOfTRTHits                 = 5,  //!< number of TRT hits
        numberOfTRTOutliers             =19,  //!< number of TRT outliers
        numberOfTRTHoles                =40,  //!< number of TRT holes
        numberOfTRTHighThresholdHits    = 6,  //!< number of TRT hits which pass the high threshold (only xenon counted)
        numberOfTRTHighThresholdHitsTotal= 64,//!< total number of TRT hits which pass the high threshold 
        numberOfTRTHitsUsedFordEdx      = 65, //!< number of TRT hits used for dE/dx computation
        numberOfTRTHighThresholdOutliers=20,  //!< number of TRT high threshold outliers (only xenon counted)
        numberOfTRTDeadStraws           =37,  //!< number of dead TRT straws crossed
        numberOfTRTTubeHits             =38,  //!< number of TRT tube hits
        numberOfTRTXenonHits            =46,  //!< number of TRT hits on track in straws with xenon
	numberOfTRTSharedHits           =62,  //!< number of TRT hits used by more than one track
         
    // --- Muon Spectrometer
        numberOfMdtHits                 = 7,       //!< number of mdt hits
        numberOfTgcPhiHits              = 8,       //!< tgc, rpc and csc measure both phi and eta coordinate
        numberOfTgcEtaHits              = 9, 
        numberOfCscPhiHits              =10,     
        numberOfCscEtaHits              =11,
        numberOfRpcPhiHits              =12,
        numberOfRpcEtaHits              =13,
        numberOfCscEtaHoles             =21,       //! number of CSC Eta measurements missing from the track
        numberOfCscPhiHoles             =22,       //! number of CSC Phi measurements missing from the track
        numberOfRpcEtaHoles             =23,       //! number of RPC Eta measurements missing from the track
        numberOfRpcPhiHoles             =24,       //! number of RPC Phi measurements missing from the track
        numberOfMdtHoles                =25,       //! number of MDT measurements missing from the track
        numberOfTgcEtaHoles             =26,       //! number of TGC Eta measurements missing from the track
        numberOfTgcPhiHoles             =27,       //! number of TGC Phi measurements missing from the track

        // New Small Wheel
        numberOfStgcEtaHits             =67,       //! number of TGC Eta measurements missing from the track
        numberOfStgcPhiHits             =68,       //! number of TGC Phi measurements missing from the track
        numberOfMmHits               =69,       //! number of TGC Eta measurements missing from the track
        numberOfStgcEtaHoles            =70,       //! number of TGC Eta measurements missing from the track
        numberOfStgcPhiHoles            =71,       //! number of TGC Phi measurements missing from the track
        numberOfMmHoles              =72,       //! number of TGC Eta measurements missing from the track

        numberOfCscUnspoiltEtaHits      =45,       //! number of unspoilt CSC eta measurements (all CSC phi measurements are by definition spoilt). See Muon::CscClusterStatus for definitions of 'spoiled' hits.
	numberOfGoodMdtHits             =66,       //!number of non-deweighted MDT hits.  Only here as a placeholder, will be filled only on xAOD::Muon
    // --- all
        numberOfOutliersOnTrack =15,       //!< number of measurements flaged as outliers in TSOS
        standardDeviationOfChi2OS = 30,    //!< 100 times the standard deviation of the chi2 from the surfaces
	
	//reserved: added to keep synchronisation with xAOD::TrackSummary in anticipation of the two being merged
 
	eProbabilityComb_res                = 47, //!< Electron probability from combining the below probabilities [float].
	eProbabilityHT_res                  = 48, //!< Electron probability from  High Threshold (HT) information [float].   
	eProbabilityToT_res                 = 49, //!< Electron probability from Time-Over-Threshold (ToT) information [float].   
	eProbabilityBrem_res                = 50, //!< Electron probability from Brem fitting (DNA) [float]. 
	pixeldEdx_res                       = 51, //!< the dE/dx estimate, calculated using the pixel clusters [?]

 // -- numbers...
        numberOfTrackSummaryTypes = 73
    };

// Troels.Petersen@cern.ch:
    enum eProbabilityType {
        eProbabilityComb            = 0,       //!< Electron probability from combining the below probabilities.
            eProbabilityHT              = 1,       //!< Electron probability from High Threshold (HT) information.
            eProbabilityToT             = 2,       //!< Electron probability from Time-Over-Threshold (ToT) information.
            eProbabilityBrem            = 3,       //!< Electron probability from Brem fitting (DNA).
            numberOfeProbabilityTypes   = 4        
        }; 
  // the eProbability vector is abused to store : 
  // [4] TRT local occupancy
  // [5] TRT dE/dx

/** enumerates the various detector types currently accessible from the isHit() method.
\todo work out how to add muons to this*/
enum DetectorType {
        pixelBarrel0 = 0, //!< there are three or four pixel barrel layers (R1/R2)
        pixelBarrel1 = 1,
        pixelBarrel2 = 2,
	pixelBarrel3 = 3,

        pixelEndCap0 = 4, //!< three pixel discs (on each side)
        pixelEndCap1 = 5,
        pixelEndCap2 = 6,

        sctBarrel0   = 7, //!< four sct barrel layers
        sctBarrel1   = 8,
        sctBarrel2   = 9,
        sctBarrel3   = 10,

        sctEndCap0   = 11, //!< and 9 sct discs (on each side)
        sctEndCap1   = 12,
        sctEndCap2   = 13,
        sctEndCap3   = 14,
        sctEndCap4   = 15,
        sctEndCap5   = 16,
        sctEndCap6   = 17,
        sctEndCap7   = 18,
        sctEndCap8   = 19,

        trtBarrel    = 20,
        trtEndCap    = 21,

	DBM0 = 22,
	DBM1 = 23,
	DBM2 = 24,

        numberOfDetectorTypes = 25

    };//not complete yet


/**A summary of the information contained by a track.

If there is information missing, or if I could add useful methods then let me know. I have deliberately tried to keep this as simple as possible, whilst allowing for future expansion.

A recent addition is the isHit() method. This is analogous to the old 'hit patterns' in that you can use it to check if a specific sub-detector element has been hit by the track.

For example, you could do:
if (summary.isHit(Trk::pixelBarrel0) ) callSomething();

Please note that this class does NO error checking - i.e. it would be possible for someone to fill this object in an inconsistent manner and it would not be caught.

@author Edward.Moyse@cern.ch
*/
class TrackSummary
{
public:
    friend class InDet::InDetTrackSummaryHelperTool;
    friend class Muon::MuonTrackSummaryHelperTool;

    /** Value set in the cxx file to -1. If any of the values returned by get(const SummaryType& type)==SummaryTypeNotSet then this means that 
    the information has not been filled. i.e. ID information cannot be filled if the TrackSummaryTool does not have access to the InDetTrackSummaryHelperTool*/
    static const int SummaryTypeNotSet;

    /**needed by POOL*/
    TrackSummary();
    
    /**ctor should probably only be called by TrkSummaryTool.
    @param information The vector passed should be complete 
    i.e. it should be the size required by m_numSummaryTypes, and any undefined
    values should be set to -1. The vector should be filled using SummaryType to select the 
    position of the information.
    @param hitPattern this bitset should be filled using the DetectorType enum. 
    True indicates that a sub-detector was hit*/
    TrackSummary( 
        const std::vector<int>& information, 
        const std::vector<float>& eProbability, 
        std::bitset<numberOfDetectorTypes>& hitPattern,
        float dedx=-1,
        int nhitsuseddedx=-1,
	int nhitsoverflowdedx=-1
    );

    /** copy ctor*/
    TrackSummary( const TrackSummary& rhs );

    virtual ~TrackSummary();

    /** returns the summary information for the passed SummaryType. 
    @param type Use the provided enums to access it, i.e. by summary.get(numberOfPixelHits)
    @return returns -1 if the enum is undefined (i.e. the information was not available in the 
    Track, or (more likely) Trk::TrkTrackSummaryTool is not filling it yet)*/
    int get(const SummaryType& type) const;

    /** returns the probability information for the passed ProbabilityType. 
    @param type Use the provided enums to access it, i.e. by summary.getPID(eProbabilityComb)
    @return returns -1 if the enum is undefined (i.e. the information was not available in the 
    Track, or (more likely) Trk::TrkTrackSummaryTool is not filling it yet)*/
    // Troels.Petersen@cern.ch:
    float getPID(const eProbabilityType& PIDtype) const;

    /** returns the dE/dx estimate, calculated using the pixel clusters */
    float getPixeldEdx() const;

    int numberOfUsedHitsdEdx() const;

    int numberOfOverflowHitsdEdx() const;

    unsigned long getHitPattern() const;

    /** returns true if the detector type 'type' is hit. 
    @param type Please use the DetectorType enum to access the information  (the internal
    positions may change!)
    @return true if sub-detector 'type' is hit*/
    bool isHit(const DetectorType& type) const ;

    /** returns a pointer to the InDetTrackSummary if available */
    const InDetTrackSummary* indetTrackSummary() const;

    /** returns a pointer to the MuonTrackSummary if available */
    const MuonTrackSummary* muonTrackSummary() const;

        /**return number of parameters currently created*/
    static unsigned int numberOfInstantiations() ;

    /** adds the values of the passed TrackSummary to this TrackSummary. Mainly intended for
    adding Muon and ID TrackSummaries, when making a combined Track for example
    WARNING: may not make much sense for e.g. Trk::numberOfContribPixelLayers*/
    TrackSummary& operator+=(const TrackSummary& ts);

    /** assignment operator */
    TrackSummary& operator=(const TrackSummary&);   

   /** Update unset summary information.
    * @param type the type of the summary information to be updated.
    * @param new_value the value to be set for the given type.
    * @return false in case the information was set alread.
   */
   bool update(Trk::SummaryType type, int new_value);

private: // data members
    friend class ::TrackSummaryCnv_p1;
    friend class ::TrackSummaryCnv_p2;

    /** vector containing the persistent summary information. */
    std::vector<int>  m_information;

    /** vector containing the persistent summary information. */
    // Troels.Petersen@cern.ch:
    std::vector<float>  m_eProbability;

    /** contains the dE/dx information, calculated using the pixel clusters */
    float m_dedx;

    int m_nhitsdedx;

    int m_nhitsoverflowdedx;

    /**contains the 'hit pattern'*/
    unsigned long m_idHitPattern;

    /** number of objects of this type in memory */
    static unsigned int s_numberOfInstantiations;

    /** pointer to the InDetTrackSummary */
    const InDetTrackSummary* m_indetTrackSummary;

    /** pointer to the MuonTrackSummary */
    const MuonTrackSummary* m_muonTrackSummary;
};

inline int Trk::TrackSummary::get(const Trk::SummaryType& type) const 
{
    return m_information.at(type);
}

// Troels.Petersen@cern.ch:
inline float Trk::TrackSummary::getPID(const Trk::eProbabilityType& PIDtype) const 
{
  return (PIDtype < m_eProbability.size() ? m_eProbability[PIDtype] : 0.);
}
        
inline bool Trk::TrackSummary::update(Trk::SummaryType type, int new_value)
{     
    if  (m_information.at(type) != SummaryTypeNotSet) return false;                                                                                                                                                                                               
    m_information[type]=new_value;
    return true;
}                                                                                                                                                                                                                                                               

inline float Trk::TrackSummary::getPixeldEdx() const
{
    return m_dedx;
}

inline int Trk::TrackSummary::numberOfUsedHitsdEdx() const
{
    return m_nhitsdedx;
}

inline int Trk::TrackSummary::numberOfOverflowHitsdEdx() const
{
    return m_nhitsoverflowdedx;
}

inline bool Trk::TrackSummary::isHit(const Trk::DetectorType& type) const 
{
    // no range checking because people should be using enums
    return (m_idHitPattern&(1<<static_cast<unsigned int>(type)));
}

inline const Trk::InDetTrackSummary* Trk::TrackSummary::indetTrackSummary() const {
    return m_indetTrackSummary;
}

inline const Trk::MuonTrackSummary* Trk::TrackSummary::muonTrackSummary() const {
    return m_muonTrackSummary;
}

/**output. This dumps the values of each of the possible summary enums*/
MsgStream& operator<<(MsgStream& out, const TrackSummary& trackSum);

/**output. This dumps the values of each of the possible summary enums*/
std::ostream& operator<<(std::ostream& out, const TrackSummary& trackSum);

}

inline unsigned long Trk::TrackSummary::getHitPattern() const
{
  return m_idHitPattern;
}

#endif

