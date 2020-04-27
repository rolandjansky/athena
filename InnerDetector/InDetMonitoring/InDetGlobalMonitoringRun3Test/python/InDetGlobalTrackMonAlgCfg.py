#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

"""
@file InDetGlobalTrackMonAlgCfg.py
@author Leonid Serkin and Per Johansson
@date April 2020
@brief Configuration for Run 3 based on InDetGlobalTrackMonTool.cxx
"""

from math import pi as M_PI

def InDetGlobalTrackMonAlgCfg(helper, alg, **kwargs):

    # values	
    m_doIBL = True
    m_nBinsEta = 50
    m_nBinsPhi = 50
    m_trackBin = 75
    m_c_etaRange = 2.5
    m_c_etaTrackletsMin = 2.4
    m_c_etaTrackletsMax = 2.7
    m_c_etaRangeTRT = 2.0
    m_c_range_LB = 3000
    m_trackMax = 75
    #M_PI = math.pi

    # this creates a "trackGroup" called "alg" which will put its histograms into the subdirectory "Track"
    trackGroup = helper.addGroup(alg, 'Track')
    pathtrack = '/InDetGlobal/Track'
    pathhits = '/InDetGlobal/Hits'


    varName = 'm_nBase;nCOMBtrks' #done
    title = 'Track multiplicity (baseline tracks);Track multiplicity;Events'
    trackGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_trackBin, xmin=0.5, xmax=m_trackMax + 0.5)

    # Eta-phi maps
    varName = 'm_eta_perigee,m_phi_perigee;Trk_Base_eta_phi' #done
    title = 'Distribution of eta vs phi for combined tracks passing Loose Primary selection;#eta,#phi_{0}'
    trackGroup.defineHistogram(varName, type='TH2F', path=pathtrack, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_track_pass_tight;Trk_Tight_eta_phi_ratio' #done
    title = 'Distribution of eta vs phi for combined tracks passing Tight selection;#eta;#phi_{0}'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathtrack, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_noTRTHits;Trk_noTRText_eta_phi_ratio' #done
    title = 'Distribution of eta vs phi for combined tracks passing with no TRT extension;#eta;#phi_{0}'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathtrack, title=title, xbins=20, xmin=-m_c_etaRangeTRT, xmax=m_c_etaRangeTRT, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_NextToInnermostPixelLayerHit;Trk_noBLhit_eta_phi_ratio' #done
    title = 'Eta-phi of tracks with no b-layer hit but a hit is expected, ratio to total tracks;#eta;#phi_{0}'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathtrack, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_InnermostPixelLayerHit;Trk_noIBLhit_eta_phi_ratio' #done
    title = 'Eta-phi of tracks with no IBL hit but a hit is expected, ratio to total tracks;#eta;#phi_{0}'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathtrack, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfInnermostPixelLayerHits;Trk_nIBLhits_eta_phi' # done
    title = 'Number of IBL hits per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_pixHits;Trk_nPIXhits_eta_phi' #done
    title = 'Number of PIX hits per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_sctHits;Trk_nSCThits_eta_phi' #done
    title = 'Number of SCT hits per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_trtHits;Trk_nTRThits_eta_phi' #done
    title = 'Number of TRT hits per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_lb,m_numberOfInnermostPixelLayerHits;trk_nIBLhits_LB' #done
    title = 'Average number of IBL hits by LB;LB;Average number of hits in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_pixHits;trk_nPIXhits_LB' #done
    title = 'Average number of PIX hits by LB;LB;Average number of hits in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_sctHits;trk_nSCThits_LB' #done
    title = 'Average number of SCT hits by LB;LB;Average number of hits in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_trtHits;trk_nTRThits_LB' #done
    title = 'Average number of TRT hits by LB;LB;Average number of hits in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfPixelDeadSensors;Trk_nPIXdisabled_eta_phi' # done
    title = 'Number of PIX disabled detector elements per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfSCTDeadSensors;Trk_nSCTdisabled_eta_phi' #done
    title = 'Number of SCT disabled detector elements per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfTRTDeadStraws;Trk_nTRTdisabled_eta_phi' #done
    title = 'Number of TRT disabled detector elements per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfPixelSharedHits;Trk_nPixShared_eta_phi' #done
    title = 'Number of Pixel shared hits per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfPixelSplitHits;Trk_nPixSplit_eta_phi' #done
    title = 'Number of Pixel split hits per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfSCTSharedHits;Trk_nSCTShared_eta_phi' #done
    title = 'Number of SCT shared hits per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfPixelHoles;Trk_nPixHoles_eta_phi' #done
    title = 'Number of Pixel holes per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfSCTHoles;Trk_nSCTHoles_eta_phi' #done
    title = 'Number of SCT holes per track, eta-phi profile;#eta;#phi'
    trackGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_lb,m_nBase_LB;Trk_nBase_LB' #done
    title = 'Average number of baseline tracks per event in LB;LB number;Average number of loose primary tracks per event in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_nTight_LB;Trk_nTight_LB' #done
    title = 'Average number of tight tracks per event in LB;LB number;Average number of tight tracks per event in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_nNoIBL_LB;Trk_noIBLhits_LB' #done
    title = 'Average number of tracks with missing IBL hit per event in LB;LB number;Average number of tracks with missing IBL hit per event in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_NoIBL_LB;Trk_noIBLhits_frac_LB' #done
    title = 'Fraction of tracks with missing IBL hit per event in LB;LB number;Fraction of tracks with missing IBL hit per event in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_nNoBL_LB;Trk_noBLhits_LB' #done
    title = 'Average number of tracks with missing b-layer hit per event in LB;LB number;Average number of tracks with missing b-layer hit per event in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_NoBL_LB;Trk_noBLhits_frac_LB' #done
    title = 'Fraction of tracks with missing b-layer hit per event in LB;LB number;Fraction of tracks with missing b-layer hit per event in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_nNoTRText_LB;Trk_noTRText_LB' #done
    title = 'Average number of tracks without TRT extension per event in LB;LB number;Average number of tracks with without TRT extension per event in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_NoTRText_LB;Trk_noTRText_frac_LB' #done
    title = 'Fraction of tracks without TRT extension per event in LB;LB number;Fraction of tracks with without TRT extension per event in LB'
    trackGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

# end histograms


