/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage TrigIDSCAN Package
@author Nikos.Konstantinidis@cern.ch John.Baines@cern.ch
@section IntroTrigIDSCAN Introduction

This is the main package for the LVL2 algrothm TrigIDSCAN. It comprises TrigIDSCAN (the main HLTAlgo) and the following AlgTools:

   - IDScanZFinder : Trigger/TrigTools/IDScanZFinder
   - HitFilter :     Trigger/TrigTools/IDScanHitFilter
   - TrigInDetTrackFitter : Trigger/TrigTools/TrigInDetTrackFitter
   - TrigTRT_TrackExtensionTool: Trigger/TrigTools/TrigTRT_TrackExtensionTool

All these AlgTools are used via the abstract interfaces defined in the TrigInDetToolInterfaces.

@section IDScanProps TrigIDSCAN Properties:

   - TrueVertexLocation : The StoreGate key for the true primary vertex, needed when the ZFinder mode is set to 1 (-> use the true vertex);
   - PrintDiagnosticMessages : extra diagnostic printing to std::cout ( default false );
   - SpacePointProviderTool : name of the SpacePointProvidor Tool to be retrieved ( default "OnlineSpacePointProviderTool" );
   - TrigZFinder : name of the AlgTool that determines the z-position of the primary pp interaction ( default "IDScanZFinder" );
   - TrigInDetTrackFitter : name of the TrigInDetTrackFitter Tool to be retrieved ( default "TrigInDetTrackFitter" );
   - TrigTRT_TrackExtensionTool : name of the TrigTRT_TrackExtensionTool to be retrieved ( default "TrigTRT_TrackExtensionTool" );
   - TrigToTrkTrackConvertorTool: name of the AlgTool to convert TrigInDetTracks to Trk::Tracks ( default "TrigToTrkTrackTool" );
   - TrigHitFilter : name of the tool that does the main pattern recognition ( default "IDScanHitFilter" );
   - PixelSP_ContainerName : name of the container of Pixel SpacePoints filled by the SpacePointProvidorTool ( default "TrigPixelSpacePoints" );
   - SCT_SP_ContainerName : name of the container of SCT SpacePoints filled by the SpacePointProvidorTool( default "TrigSCT_SpacePoints" );
   - etaHalfWidth : RoI size in eta ( default 0.1 radians );
   - phiHalfWidth : RoI size in phi  ( default 0.1 radians );
   - FullScanMode : false -> RoI guide reconstruction, true -> reconstruction in the whole InDet ( default false );
   - ZFinderMode  : 0 -> find Z of primary vertex,  1 -> use MC truth info for Z of primary vertex ( default 0 -> use the TrigZFinder tool);
   - MinHits : minimum no. SpacePoints needed for IDScan to run ( default 5 )
   - TrkTrackCollName : name of the output TrigInDetTrack collection ( default "IDSCAN_Tracks");
   - trackConvertorMode : 1 - create Trk::Track !=1 - do not create ( default 1 );
   - doTRTpropagation : extend tracks to TRT using TrigTRT_TrackExtensionTool  ( default true );
   - doShift : True - switch on special mode for Cosmics and TB where SP are transformed to point at beampipe ( default m_doShift = false );

@section IDScanMonitorProps TrigIDSCAN Properties used in monitoring:

   - ChiSqd : the chi-squared of the fitted tracks
   - NStraw_TRT : the number of TRT hits per reconstructed track
   - NTR_TRT : the number of high threshold TRT hits per reconstructed track
   - pt : the transverse momentum of reconstructed tracks
   - a0 : the transverse impact parameter of reconstructed tracks
   - z0 : the z coordinate of reconstructed tracks at the point of closest approach to the beam axis
   - phi : the phi of reconstructed tracks at the point of closest approach to the beam axis
   - eta : the eta of reconstructed tracks at the point of closest approach to the beam axis
   - NHit_Si : the number of SiTracker hits per reconstructed track


@htmlinclude used_packages.html

@include requirements


*/
