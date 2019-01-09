/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRACKSUMMARYHELPERTOOL_H
#define INDETTRACKSUMMARYHELPERTOOL_H

#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTrackSummary/TrackSummary.h" // defines the Trk::numberOfDetectorTypes enum
#include "TrkTrackSummary/DetailedHitInfo.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <bitset>
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

class ITRT_StrawSummarySvc;
class PixelID;
class SCT_ID;
class TRT_ID;

namespace Trk
{
   class TrackStateOnSurface;
   class Track;
   class RIO_OnTrack;
   class CompetingRIOsOnTrack;
   class IPRD_AssociationTool;
   class ITrackHoleSearchTool;
   class IPixelToTPIDTool;
}
namespace InDet{
   class IInDetTestBLayerTool;
}

namespace InDet {

  class InDetTrackSummaryHelperTool :  virtual public Trk::ITrackSummaryHelperTool, public AthAlgTool   {
  public:
    
    /** constructor */
    InDetTrackSummaryHelperTool(const std::string&,const std::string&,const IInterface*);
	
    /** destructor */
    virtual ~InDetTrackSummaryHelperTool ();
	
    /** standard AlgTool methods: initialise retrieves Tools, finalize does nothing */
    virtual StatusCode initialize();
    virtual StatusCode finalize();
	

    /** Input : rot, tsos
	Output: Changes in information and hitPattern
	Input quantities rot, tsos are used to increment the counts for hits and outliers in information and to set the proper bits in hitPattern.
    */
    virtual void analyse(const Trk::Track& track,
			 const Trk::RIO_OnTrack* rot,
			 const Trk::TrackStateOnSurface* tsos,
			 std::vector<int>& information, 
			 std::bitset<Trk::numberOfDetectorTypes>& hitPattern,
       Trk::DetailedHitInfo& detailedInfo ) const ;

    virtual void analyse(const Trk::Track& track,
			 const Trk::CompetingRIOsOnTrack* crot,
			 const Trk::TrackStateOnSurface* tsos,
			 std::vector<int>& information, 
			 std::bitset<Trk::numberOfDetectorTypes>& hitPattern,
       Trk::DetailedHitInfo& detailedInfo ) const;

    /** Input : track, partHyp
	Output: Changes in information
	This method first calls the method getListOfHits to isolate the relevant hits on the track before calling the method
	performHoleSearchStepWise which then performs the actual hole search.
	Additionally the Layers of the Pixel Detector which contribute measurements to the track are counted  
	If problems occur, the information counters for Holes and PixelLayers are reset to -1 flagging them as not set.
    */
    void searchForHoles(const Trk::Track& track, 
			std::vector<int>& information ,
			const Trk::ParticleHypothesis partHyp = Trk::pion) const ;

    /** this method simply updaes the shared hit content - it is designed/optimised for track collection merging */
    void updateSharedHitCount(const Trk::Track& track, Trk::TrackSummary& summary) const;
 /** this method simply updaes the electron PID content - it is designed/optimised for track collection merging */
    void updateAdditionalInfo(Trk::TrackSummary& summary,std::vector<float>& eprob,float& dedx, int& nclus, int& noverflowclus) const;
    /** This method updates the expect... hit info*/
    void updateExpectedHitInfo(const Trk::Track& track, Trk::TrackSummary& summary) const;

    /** @copydoc Trk::ITrackSummaryHelperTool::addDetailedTrackSummary(const Trk::Track&, Trk::TrackSummary&)*/

    virtual void addDetailedTrackSummary(const Trk::Track&, Trk::TrackSummary&) const;
 

  private:
    /**ID pixel helper*/
    bool m_usePixel;
    const PixelID* m_pixelId;
    
    /**ID SCT helper*/
    bool m_useSCT;
    const SCT_ID* m_sctId;
    
    /**ID TRT helper*/
    bool m_useTRT;
    const TRT_ID* m_trtId;

    /**Association tool - used to work out which (if any) PRDs are shared between 
       tracks*/
    ToolHandle< Trk::IPRD_AssociationTool >  m_assoTool;
    ToolHandle< Trk::IPixelToTPIDTool >      m_pixeldedxtool; // why in Trk namespace ?
    ToolHandle< Trk::ITrackHoleSearchTool >  m_holeSearchTool;
    ToolHandle< InDet::IInDetTestBLayerTool > m_testBLayerTool;
    ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawSummarySvc; //!< The ConditionsSummaryTool


    bool m_doSharedHits;
    bool m_doSharedHitsTRT;
    bool m_doSplitPixelHits;
    bool m_overwriteidsummary;
    bool m_runningTIDE_Ambi;
    bool m_ITkGeometry;
    
};

}
#endif 

