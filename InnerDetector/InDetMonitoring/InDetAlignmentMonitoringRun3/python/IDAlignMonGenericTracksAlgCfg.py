#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

"""
@file IDAlignMonGenericTracksAlgCfg.py
@author Per Johansson
@date 2021
@brief Configuration for Run 3 based on IDAlignMonGenericTracks.cxx
"""

from math import pi as M_PI

def IDAlignMonGenericTracksAlgCfg(helper, alg, **kwargs):

    # values
    m_pTRange = 100
    m_NTracksRange = 200
    m_rangePixHits = 10
    m_rangeSCTHits = 20
    m_rangeTRTHits = 60
    m_etaRange = 3
    m_d0BsNbins = 100
    m_d0Range = 2
    m_z0Range = 250
    m_d0BsRange = 0.5
     
    # this creates a "genericTrackGroup" called "alg" which will put its histograms into the subdirectory "GenericTracks"
    genericTrackGroup = helper.addGroup(alg, 'Tracks')
    pathtrack = '/IDAlignMon/ExtendendTracks_NoTriggerSelection/GenericTracks'

    varName = 'm_lb;LumiBlock'
    title = 'Lumiblock'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=1024, xmin=-0.5, xmax=1023.5)

    varName = 'm_beamSpotX,m_beamSpotY;YBs_vs_XBs'
    title = 'BeamSpot Position: y vs x'
    genericTrackGroup.defineHistogram(varName, type='TH2F', path=pathtrack, title=title, xbins=100, xmin=-0.9, xmax=-0.1, ybins=100, ymin=-0.9, ymax=-0.1) 

    varName = 'm_beamSpotZ,m_beamSpotY;YBs_vs_ZBs'
    title = 'BeamSpot Position: y vs z'
    genericTrackGroup.defineHistogram(varName, type='TH2F', path=pathtrack, title=title, xbins=100, xmin=-25, xmax=-5, ybins=100, ymin=-0.9, ymax=-0.1)

    varName = 'm_beamSpotX,m_beamSpotZ;XBs_vs_XZs'
    title = 'BeamSpot Position: x vs z'
    genericTrackGroup.defineHistogram(varName, type='TH2F', path=pathtrack, title=title, xbins=100, xmin=-25, xmax=-5, ybins=100, ymin=-0.9, ymax=-0.1)

    varName = 'm_nhits_per_track;Nhits_per_track'
    title = 'Number of hits per track;Number of Good Tracks;Number of Events'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_NTracksRange + 1, xmin=-0.5, xmax=m_NTracksRange + 0.5)

    varName = 'm_npixelhits_per_track;Npixhits_per_track'
    title = 'Number of pixhits per track;Number of Good Tracks;Number of Tracks'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_rangePixHits, xmin=-0.5, xmax=m_rangePixHits - 0.5)

    varName = 'm_nscthits_per_track;Nscthits_per_track'
    title = 'Number of scthits per track;Number of SCT hits per Tracks;Number of Tracks'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_rangeSCTHits, xmin=-0.5, xmax=m_rangeSCTHits - 0.5)

    varName = 'm_ntrthits_per_track;Ntrthits_per_track'
    title = 'Number of trthits per track;Number of TRT hits per Tracks;Number of Tracks'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_rangeTRTHits, xmin=-0.5, xmax=m_rangeTRTHits - 0.5)

    varName = 'm_chi2oDoF;chi2oDoF'
    title = 'chi2oDoF;Track in #chi^{2} / NDoF;Number of Tracks'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=100, xmin=0, xmax=10)

    varName = 'm_eta;eta'
    title = 'eta;Track #eta;Number of Tracks'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=80, xmin=-m_etaRange, xmax=m_etaRange)

    varName = 'm_eta_pos;eta_pos'
    title = 'eta for positive tracks; #eta(+);Number of Tracks'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=25, xmin=-m_etaRange, xmax=m_etaRange)

    varName = 'm_eta_neg;eta_neg'
    title = 'eta for negative tracks; #eta(-);Number of Tracks'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=25, xmin=-m_etaRange, xmax=m_etaRange)

    varName = 'm_phi;phi'
    title = 'phi;Track #phi;Number of Tracks'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=80, xmin=0, xmax= 2 * M_PI)

    varName = 'm_z0;z0'
    title = 'z0;[mm]'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_d0BsNbins, xmin=-m_z0Range, xmax=m_z0Range)

    varName = 'm_z0sintheta;z0sintheta'
    title = 'z0sintheta;[mm]'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_d0BsNbins, xmin=-m_z0Range, xmax=m_z0Range)

    varName = 'm_d0;d0'
    title = 'd0;[mm]'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_d0BsNbins, xmin=-m_d0Range, xmax=m_d0Range)

    varName = 'm_d0_bscorr;d0_bscorr'
    title = 'd0 (corrected for beamspot);d0 [mm]'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_d0BsNbins, xmin=-m_d0BsRange, xmax=m_d0BsRange)

    varName = 'm_pT;pT'
    title = 'pT;Signed Track pT [GeV];Number of Tracks'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=200, xmin=-m_pTRange, xmax=m_pTRange)

    varName = 'm_p;P'
    title = 'Track Momentum P;Signed Track P [GeV];Number of Tracks'
    genericTrackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=200, xmin=-m_pTRange, xmax=m_pTRange)
   
    # end histograms


