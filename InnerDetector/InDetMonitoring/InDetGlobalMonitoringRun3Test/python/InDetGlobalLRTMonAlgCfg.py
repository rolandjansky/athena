#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

"""
@file InDetGlobalLRTMonAlgCfg.py
@author Leonid Serkin and Per Johansson
@date July 2021
@brief Configuration for Run 3 based on InDetGlobalLRTMonTool.cxx
"""

from math import pi as M_PI

def InDetGlobalLRTMonAlgCfg(helper, alg, **kwargs):

    # values
    m_nBinsEta = 50
    m_nBinsPhi = 50
    m_trackBin = 75
    m_c_etaRange = 3
    m_c_etaRangeTRT = 2.0
    m_c_range_LB = 3000

    # this creates a "lrtGroup" called "alg" which will put its histograms into the subdirectory "Track"
    lrtGroup = helper.addGroup(alg, 'LRT')
    pathtrack = '/InDetGlobal/LRTTrack'
    pathhits = '/InDetGlobal/LRTHits'


    varName = 'm_nBase;nCOMBtrks' #done
    title = 'Track multiplicity (baseline tracks);Track multiplicity;Events'
    lrtGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_trackBin, xmin=-5, xmax=10 + 0.5)


    varName = 'm_d0_perigee;trkD' #done 
    title = 'd_{0} ;d_{0} (mm)'
    lrtGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=20, xmin=-325, xmax=325)

    varName = 'm_z0_perigee;trkZ' #done 
    title = 'z_{0} ;z_{0} (mm);'
    lrtGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=20, xmin=-600, xmax=600)


    varName = 'm_radius_perigee;simon1' #done 
    title = 'Radius of first hit ; R (m);'
    lrtGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=20, xmin=-600, xmax=600)


    varName = 'm_eta_perigee1;trkEta' #done 
    title = '#eta of all tracks;#eta;'
    lrtGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange)


    varName = 'm_phi_perigee1;trkPhi' #done 
    title = ' #varphi of all tracks; #varphi (rad);'
    lrtGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=m_nBinsPhi, xmin=-3.2, xmax=3.2)

    varName = 'm_trkPt;trkPt' #done
    title = 'Track Pt;p_{T} (GeV);'
    lrtGroup.defineHistogram(varName, type='TH1F', path=pathtrack, title=title, xbins=100, xmin=-1, xmax=200)


    # Eta-phi maps
    varName = 'm_eta_perigee,m_phi_perigee;Trk_Base_eta_phi' #done 
    title = 'Distribution of eta vs phi for LRT tracks;#eta,#phi_{0}'
    lrtGroup.defineHistogram(varName, type='TH2F', path=pathtrack, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_track_pass_tight;Trk_Tight_eta_phi_ratio' #done
    title = 'Distribution of eta vs phi for combined tracks passing Tight selection;#eta;#phi_{0}'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathtrack, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_noTRTHits;Trk_noTRText_eta_phi_ratio' #done
    title = 'Distribution of eta vs phi for combined tracks passing with no TRT extension;#eta;#phi_{0}'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathtrack, title=title, xbins=20, xmin=-m_c_etaRangeTRT, xmax=m_c_etaRangeTRT, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_NextToInnermostPixelLayerHit;Trk_noBLhit_eta_phi_ratio' #done
    title = 'Eta-phi of tracks with no b-layer hit but a hit is expected, ratio to total tracks;#eta;#phi_{0}'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathtrack, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_InnermostPixelLayerHit;Trk_noIBLhit_eta_phi_ratio' #done
    title = 'Eta-phi of tracks with no IBL hit but a hit is expected, ratio to total tracks;#eta;#phi_{0}'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathtrack, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfInnermostPixelLayerHits;Trk_nIBLhits_eta_phi' # done
    title = 'Number of IBL hits per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_pixHits;Trk_nPIXhits_eta_phi' #done
    title = 'Number of PIX hits per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_sctHits;Trk_nSCThits_eta_phi' #done
    title = 'Number of SCT hits per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_trtHits;Trk_nTRThits_eta_phi' #done
    title = 'Number of TRT hits per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_lb,m_numberOfInnermostPixelLayerHits;trk_nIBLhits_LB' #done
    title = 'Average number of IBL hits by LB;LB;Average number of hits in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)


    varName = 'm_lb,m_d0_perigee;Simon' #done
    title = 'D0'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_pixHits;trk_nPIXhits_LB' #done
    title = 'Average number of PIX hits by LB;LB;Average number of hits in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_sctHits;trk_nSCThits_LB' #done
    title = 'Average number of SCT hits by LB;LB;Average number of hits in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_trtHits;trk_nTRThits_LB' #done
    title = 'Average number of TRT hits by LB;LB;Average number of hits in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfPixelDeadSensors;Trk_nPIXdisabled_eta_phi' # done
    title = 'Number of PIX disabled detector elements per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfSCTDeadSensors;Trk_nSCTdisabled_eta_phi' #done
    title = 'Number of SCT disabled detector elements per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfTRTDeadStraws;Trk_nTRTdisabled_eta_phi' #done
    title = 'Number of TRT disabled detector elements per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfPixelSharedHits;Trk_nPixShared_eta_phi' #done
    title = 'Number of Pixel shared hits per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfPixelSplitHits;Trk_nPixSplit_eta_phi' #done
    title = 'Number of Pixel split hits per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfSCTSharedHits;Trk_nSCTShared_eta_phi' #done
    title = 'Number of SCT shared hits per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfPixelHoles;Trk_nPixHoles_eta_phi' #done
    title = 'Number of Pixel holes per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_eta_perigee,m_phi_perigee,m_numberOfSCTHoles;Trk_nSCTHoles_eta_phi' #done
    title = 'Number of SCT holes per track, eta-phi profile;#eta;#phi'
    lrtGroup.defineHistogram(varName, type='TProfile2D', path=pathhits, title=title, xbins=m_nBinsEta, xmin=-m_c_etaRange, xmax=m_c_etaRange, ybins=m_nBinsPhi, ymin=-M_PI, ymax=M_PI)

    varName = 'm_lb,m_nBase_LB;Trk_nBase_LB' #done 
    title = 'Average number of baseline tracks per event in LB;LB number;Average number of LRT tracks per event in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)



    varName = 'm_lumiPerBCID,m_nBase_Lumi;Trk_nBase_Lumi' #done 
    title = 'Average number of baseline tracks per event in Pileup;#mu;Average number of loose primary tracks per event in Pileup'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=40, xmin=0, xmax=80)


    varName = 'm_lb,m_nTight_LB;Trk_nTight_LB' #done
    title = 'Average number of tight tracks per event in LB;LB number;Average number of tight tracks per event in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_nNoIBL_LB;Trk_noIBLhits_LB' #done
    title = 'Average number of tracks with missing IBL hit per event in LB;LB number;Average number of tracks with missing IBL hit per event in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_NoIBL_LB;Trk_noIBLhits_frac_LB' #done
    title = 'Fraction of tracks with missing IBL hit per event in LB;LB number;Fraction of tracks with missing IBL hit per event in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_nNoBL_LB;Trk_noBLhits_LB' #done
    title = 'Average number of tracks with missing b-layer hit per event in LB;LB number;Average number of tracks with missing b-layer hit per event in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_NoBL_LB;Trk_noBLhits_frac_LB' #done
    title = 'Fraction of tracks with missing b-layer hit per event in LB;LB number;Fraction of tracks with missing b-layer hit per event in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_nNoTRText_LB;Trk_noTRText_LB' #done
    title = 'Average number of tracks without TRT extension per event in LB;LB number;Average number of tracks with without TRT extension per event in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

    varName = 'm_lb,m_NoTRText_LB;Trk_noTRText_frac_LB' #done
    title = 'Fraction of tracks without TRT extension per event in LB;LB number;Fraction of tracks with without TRT extension per event in LB'
    lrtGroup.defineHistogram(varName, type='TProfile', path=pathtrack, title=title, xbins=m_c_range_LB, xmin=0, xmax=m_c_range_LB)

# end histograms


