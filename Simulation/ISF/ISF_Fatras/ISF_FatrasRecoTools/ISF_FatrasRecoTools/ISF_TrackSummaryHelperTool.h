/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_TRACKSUMMARYHELPERTOOL_H
#define ISF_TRACKSUMMARYHELPERTOOL_H

#include "TrkToolInterfaces/IExtendedTrackSummaryHelperTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <bitset>
#include <stdexcept>

class PixelID;
class SCT_ID;

namespace Trk {
  class TrackStateOnSurface;
  class Track;
  class RIO_OnTrack;
  class CompetingRIOsOnTrack;
  class IPRD_AssociationTool;
}

namespace iFatras {
  class ISF_TrackSummaryHelperTool : public extends<AthAlgTool, Trk::IExtendedTrackSummaryHelperTool> {
  public:
    /** constructor */
    ISF_TrackSummaryHelperTool(const std::string&,const std::string&,const IInterface*);
      
    /** destructor */
    virtual ~ISF_TrackSummaryHelperTool();
      
    /** standard AlgTool methods: initialise retrieves Tools, finalize does nothing */
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    /** Input : rot, tsos
        Output: Changes in information and hitPattern
        Input quantities rot, tsos are used to increment the counts for hits and outliers in information and to set the proper bits in hitPattern.
    */
    using IExtendedTrackSummaryHelperTool::analyse;
    using IExtendedTrackSummaryHelperTool::updateSharedHitCount;
    virtual void analyse(const Trk::Track& track,
                         const Trk::PRDtoTrackMap *prd_to_track_map,
                         const Trk::RIO_OnTrack* rot,
                         const Trk::TrackStateOnSurface* tsos,
                         std::vector<int>& information,
                         std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const override;

    virtual void analyse(const Trk::Track& track,
                         const Trk::PRDtoTrackMap *prd_to_track_map,
                         const Trk::CompetingRIOsOnTrack* crot,
                         const Trk::TrackStateOnSurface* tsos,
                         std::vector<int>& information,
                         std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const override;

    /** Input : rot, tsos
	Output: Changes in information and hitPattern
	Input: quantities rot, tsos are used to increment the counts for hits and outliers in information and to set the proper bits inhitPattern.
    */
    virtual void analyse(const Trk::Track& track,
			 const Trk::RIO_OnTrack* rot,
			 const Trk::TrackStateOnSurface* tsos,
			 std::vector<int>& information,
			 std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const override
    {
      analyse(track,nullptr,rot,tsos,information,hitPattern);
    }

    /** Not used --> running with RIO_OnTrack only
    */
    virtual void analyse(const Trk::Track& track,
			 const Trk::CompetingRIOsOnTrack* crot,
			 const Trk::TrackStateOnSurface* tsos,
			 std::vector<int>& information,
			 std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const override
    {
      analyse(track,nullptr, crot,tsos,information,hitPattern);
    }

    /** Not used --> HoleSearchTool not used
    */
    virtual
    void searchForHoles(const Trk::Track& track,
			std::vector<int>& information ,
			const Trk::ParticleHypothesis partHyp = Trk::pion) const override;

      /** this method simply updaes the shared hit content - it is designed/optimised for track collection merging */
    virtual void updateSharedHitCount(const Trk::Track& track,
                                      const Trk::PRDtoTrackMap *prd_to_track_map,
                                      Trk::TrackSummary& summary) const override;

  /** this method simply updaes the shared hit content - it is designed/optimised for track collection merging */
    virtual
    void updateSharedHitCount(const Trk::Track& /*track*/,
                              Trk::TrackSummary& /*summary*/) const override
    {
      ATH_MSG_DEBUG("updateSharedHitCount not implemented !!");
    }

    /** this method simply updaes the electron PID content - it is designed/optimised for track collection merging */
    virtual
    void updateAdditionalInfo(Trk::TrackSummary& summary,std::vector<float>& eprob,float& dedx, int& nclus, int& noverflowclus) const override;
    /** This method updates the expect... hit info*/
    virtual
    void updateExpectedHitInfo(const Trk::Track& track, Trk::TrackSummary& summary) const override;
    
    /** @copydoc Trk::ITrackSummaryHelperTool::addDetailedTrackSummary(const Trk::Track&, Trk::TrackSummary&)*/
    
    virtual void addDetailedTrackSummary(const Trk::Track&, Trk::TrackSummary&) const override;
    
    
  private:

    inline
    bool isShared(const Trk::PRDtoTrackMap *prd_to_track_map,
                  const Trk::PrepRawData& prd) const {
      if (prd_to_track_map) {
        return prd_to_track_map->isShared(prd);
      }
      else {
        if (!m_assoTool.isEnabled()) {
          throw std::logic_error("Shared hits to be computed but no PRDtoTrack provided "
                                 " (and no association tool configured (deprecated))");
        }
        return m_assoTool->isShared(prd);
      }
    }

    /**ID pixel helper*/
    bool m_usePixel;
    const PixelID* m_pixelId;
    
    /**ID SCT helper*/
    bool m_useSCT;
    const SCT_ID* m_sctId;
    
    /**Association tool - used to work out which (if any) PRDs are shared between 
       tracks*/
    ToolHandle< Trk::IPRD_AssociationTool >   m_assoTool;
    
    bool m_doSharedHits;
    //bool m_doSplitPixelHits;
    bool m_overwriteidsummary;
    //bool m_runningTIDE_Ambi;
  };
  
}

#endif // ISF_TRACKSUMMARYHELPERTOOL_H
