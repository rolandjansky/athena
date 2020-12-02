/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRKSummaryToolStub_H
#define TRKSummaryToolStub_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkToolInterfaces/IExtendedTrackSummaryTool.h" //baseclass

#include <string>
#include <memory> //unique_ptr

namespace Trk{
  class Track;
  class PRDtoTrackMap;
  class TrackSummary;
}
 
 class 
 SummaryToolStub : public extends<AthAlgTool, Trk::IExtendedTrackSummaryTool>{
   public:
   SummaryToolStub(const std::string& t, const std::string& n, const IInterface*  p );
   
   ~SummaryToolStub(){}
   StatusCode initialize() override;
   StatusCode finalize() override;
   //methods inherited from the IExtendedTrackSummaryTool and the ITrackSummaryTool.
   //
   void 
   updateSharedHitCount(Trk::Track& ) const override;
   //
   void 
   computeAndReplaceTrackSummary(const EventContext& ctx, Trk::Track& ,const Trk::PRDtoTrackMap* , bool ) const override;
   //
   void 
   updateTrackSummary(const EventContext& ctx,Trk::Track&) const override;
   //
   void 
   updateTrackSummary(const EventContext& ctx, Trk::Track&, const Trk::PRDtoTrackMap* , bool) const override;
   //
   void 
   updateTrack(const EventContext& ctx,Trk::Track& ) const override;
    //
   std::unique_ptr<Trk::TrackSummary> 
   summaryNoHoleSearch( const EventContext& ctx,const Trk::Track& , const Trk::PRDtoTrackMap* ) const override;
   //
   std::unique_ptr<Trk::TrackSummary> 
   summaryNoHoleSearch(const EventContext& ctx, const Trk::Track&)const override;
   //
   std::unique_ptr<Trk::TrackSummary> 
   summary(const EventContext& ctx,const Trk::Track& ) const override;
   //
   std::unique_ptr<Trk::TrackSummary> 
   summary(const EventContext& ctx,const Trk::Track&, const Trk::PRDtoTrackMap*) const override;
   //
   void 
   updateSharedHitCount(const Trk::Track& ,const Trk::PRDtoTrackMap* , Trk::TrackSummary& ) const override;
   //
   void 
   updateSharedHitCount(Trk::Track& ,const Trk::PRDtoTrackMap* ) const  override;
   //
   void 
   updateAdditionalInfo(const Trk::Track& , Trk::TrackSummary& ) const override;
   //
   void 
   updateAdditionalInfo(Trk::Track& ) const override;
   
   private:
   std::unique_ptr<Trk::TrackSummary> m_pTrackSummaryRef{};
 };
#endif
