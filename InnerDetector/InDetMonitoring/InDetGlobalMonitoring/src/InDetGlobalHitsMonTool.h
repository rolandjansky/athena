/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalHitsMonTool.h
 * Implementation of inner detector global hits monitoring tool
 *
 * @author 
 * Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
 * Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
 * Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
 * Thomas Burgess <Thomas.Burgess@cern.ch>      
 *
 * $Id: InDetGlobalHitsMonTool.h,v 1.45 2009-05-14 22:47:37 kastanas Exp $
 *
 **************************************************************************/

#ifndef InDetGlobalHitsMonTool_H
#define InDetGlobalHitsMonTool_H

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"

//Local
#include "InDetGlobalMotherMonTool.h"
//Framework
#include "IdDictDetDescr/IdDictManager.h"
#include "TrkToolInterfaces/IUpdator.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrkTrack/TrackCollection.h"



//Standard c++
#include <string>
/// Forward declarations
class IInterface;
class StatusCode;
class TH1F_LW;
class TH2F_LW;
class TH1I_LW;
class TH2I_LW;
class TProfile_LW;

namespace Trk {
  class IUpdator;
  class IPropagator;
  class RIO_OnTrack;
  class Track;
  class TrackStateOnSurface;

}

class TrackTruthCollection;

/// Monitoring tool derived from InDetGlobalMotherMonTool
/// Purpose to provide combined hit information for the three ID detector

class InDetGlobalHitsMonTool : public InDetGlobalMotherMonTool
{
    
public:
    ///Constructor
    InDetGlobalHitsMonTool( const std::string & type, 
			    const std::string & name,
			    const IInterface* parent); 
    
    ///Virtual destructor
    virtual ~InDetGlobalHitsMonTool() {}

    virtual StatusCode initialize();
    
    ///@copydoc InDetGlobalMotherMonTool::bookHistograms()
    virtual StatusCode bookHistogramsRecurrent() ;     

    ///@copydoc InDetGlobalMotherMonTool::fillHistograms()
    virtual StatusCode fillHistograms();

    virtual StatusCode procHistograms();

   
private:    

    // Process each hit to determine which detector it belongs to/location and add into hitmap.
    StatusCode  processHit(const Trk::Track *track, const Trk::TrackStateOnSurface* trackState );
    // Process all holes on track and fill the holemap.
    // Determine if track is good w.r.t. each subdetector and if so add to relevant histograms.

    //---------- member variables ------------
   
    /// Updator
    PublicToolHandle<Trk::IUpdator>             m_iUpdator
       {this,"KalmanUpdator","Trk::KalmanUpdator",""};

    ///The track collection
    const TrackCollection* m_tracks;

    //
    // Debug histograms for single detectors 
    //

    TH2F_LW * m_ID_hitmap_x_y;
    TH2F_LW * m_ID_hitmap_x_y_eca;
    TH2F_LW * m_ID_hitmap_x_y_ecc;
    TH2F_LW * m_ID_hitmap_z_x;
    TH2F_LW * m_ID_hitmap_z_r;
    TH2I_LW * m_ID_holes;

    ///@name Pixel histograms
    ///@{

    ///Number of Silicon hits vs TRT hits
    TH2I_LW *  m_Trk_nSihits_nTRThits;

    ///Number of Pixel hits vs SCT hits
    TH2I_LW *  m_Trk_nPIXhits_nSCThits;

    ///Number of SCT hits vs TRT hits
    TH2I_LW *  m_Trk_nSCThits_nTRThits;

    ///Number of PIX hits per track
    TH1I_LW *  m_Trk_nPIXhits;
    TH1I_LW *  m_Trk_nPIXhits_EA;
    TH1I_LW *  m_Trk_nPIXhits_TA;
    TH1I_LW *  m_Trk_nPIXhits_B;
    TH1I_LW *  m_Trk_nPIXhits_TC;
    TH1I_LW *  m_Trk_nPIXhits_EC;
    TProfile_LW *  m_Trk_nPIXhits_eta;
    TProfile_LW *  m_Trk_nPIXhits_phi;
    TProfile2D * m_Trk_nPIXhits_eta_phi;
    TH1I_LW *  m_Trk_nPixhits_SCTTRTPixFid;

    ///Number of PIX hits per track for d0<5 and z0<140 tracks
    TH1I_LW *  m_Trk_nPIXhits_fidusial;

    ///Number of PIX hits per track - only one track per event
    TH1I_LW *  m_Trk_nPIXhits_1trk;
    ///@}

    ///@name SCT histograms
    ///@{

    ///Number of SCT hits per track - only one track per event
    TH1I_LW *  m_Trk_nSCThits_1trk;

    ///Number of SCT hits per track
    TH1I_LW     *  m_Trk_nSCThits;
    TH1I_LW *  m_Trk_nSCThits_EA;
    TH1I_LW *  m_Trk_nSCThits_TA;
    TH1I_LW *  m_Trk_nSCThits_B;
    TH1I_LW *  m_Trk_nSCThits_TC;
    TH1I_LW *  m_Trk_nSCThits_EC;
    TProfile_LW *  m_Trk_nSCThits_eta;
    TProfile_LW *  m_Trk_nSCThits_phi;
    TProfile2D *  m_Trk_nSCThits_eta_phi;
    TH2I_LW     *  m_Trk_nSCThits_pt;
    TH1I_LW     *  m_Trk_nSCThits_PixTRT;

    ///Number of SCT hits for tracks which have at least 1 Pixel hit
    TH1I_LW *  m_Trk_nSCThits_withPix;
    
    ///@}

    ///@name TRT histograms
    ///@{

    ///Number of TRT hits per track
    TH1I_LW *  m_Trk_nTRThits;
    TH1I_LW *  m_Trk_nTRThits_EA;
    TH1I_LW *  m_Trk_nTRThits_TA;
    TH1I_LW *  m_Trk_nTRThits_B;
    TH1I_LW *  m_Trk_nTRThits_TC;
    TH1I_LW *  m_Trk_nTRThits_EC;
    TProfile_LW *  m_Trk_nTRThits_eta;
    TProfile_LW *  m_Trk_nTRThits_phi;
    TProfile2D *  m_Trk_nTRThits_eta_phi;
    TH1I_LW *  m_Trk_nTRThits_PixSCT;
    
    ///Number of high level TRT hits per track
    TH1I_LW *  m_Trk_nTRTHLhits;

    ///Number of low level TRT hits per track
    TH1I_LW *  m_Trk_nTRTLLhits; // hits on track 

    ///Number of TRT hits for tracks which have at least 1 Pixel hit and 1 SCT hit
    TH1I_LW *  m_Trk_nTRThits_withSi;

    ///Number of high level TRT hits per track - only one track per event
    TH1I_LW *  m_Trk_nTRTHLhits_1trk;

    ///Number of low level TRT hits per track - only one track per event
    TH1I_LW *  m_Trk_nTRTLLhits_1trk;


    
    //  Number of hits in each subdetector per track
    int m_TRT_LL;
    int m_TRT_HL;
    int m_SCThits;
    int m_PIXhits;
    int m_PIXhits_fid;
    

    int m_pix_barrel;
    int m_pix_eca;
    int m_pix_ecc;

    int m_sct_barrel;
    int m_sct_eca;
    int m_sct_ecc;

    int m_trt_barrel;
    int m_trt_eca;
    int m_trt_ecc;
    // Monte Carlo parameters (to be removed?)
};

#endif
