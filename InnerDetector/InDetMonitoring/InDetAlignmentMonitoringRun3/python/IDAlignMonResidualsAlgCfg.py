#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file InDetAlignMonResidualsAlgCfg.py
@author PerJohansson
@date 2021
@brief Configuration for Run 3 based on InDetAlignMonResiduals.cxx
'''
def IDAlignMonResidualsAlgCfg(helper, alg, **kwargs):
    '''Function to configures some algorithms in the monitoring system.'''

    #Values
    m_nBinsMuRange        = 101
    m_muRangeMin          = -0.5
    m_muRangeMax          = 100.5
    m_minSiResFillRange   = -0.08
    m_maxSiResFillRange   = 0.08
    m_RangeOfPullHistos   = 6
    m_minPIXResYFillRange = -0.4
    m_maxPIXResYFillRange = 0.4
      
    # this creates a "residualGroup" called "alg" which will put its histograms into the subdirectory "Residuals"
    residualGroup = helper.addGroup(alg, 'Residuals')
    pathResiduals = '/IDAlignMon/ExtendedTracks_NoTriggerSelection/Residuals'

    # Histograms for the Alignment Residual monitoring:    
    varName = 'm_mu;mu_perEvent'
    title = 'mu_perEvent;<#mu> per event;Events'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=m_nBinsMuRange, xmin=m_muRangeMin, xmax=m_muRangeMax)

    varName = 'm_detType;sirescalcfailure'
    title = 'Hits with ResidualPullCalculator problem;Events;DetType'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=2, xmin=0, xmax=2)

    #Common for Pixel and SCT
    varName = 'm_si_residualx;si_residualx'
    title = 'Silicon UnBiased X Residual;Events;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_si_b_residualx;si_b_residualx'
    title = 'Silicon Barrel Only UnBiased X Residual;Events;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    #Pixel Barrel Plots
    varName = 'm_pix_b_residualx;pix_b_residualx'
    title = 'UnBiased X Residual Pixel Barrel;Events;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_pix_b_biased_residualx;pix_b_biasedresidualx'
    title = 'Biased X Residual Pixel Barrel;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_pix_b_residualy;pix_b_residualy'
    title = 'UnBiased Y Residual Pixel Barrel;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minPIXResYFillRange, xmax=m_maxPIXResYFillRange)

    varName = 'm_pix_b_biased_residualy;pix_b_biasedresidualy'
    title = 'Biased Y Residual Pixel Barrel;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minPIXResYFillRange, xmax=m_maxPIXResYFillRange)

    varName = 'm_pix_b0_residualsx;pix_b0_residualx' 
    title = 'UnBiased X Residual Pixel Barrel 0'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_pix_b1_residualsx;pix_b1_residualx' 
    title = 'UnBiased X Residual Pixel Barrel 1'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_pix_b2_residualsx;pix_b2_residualx' 
    title = 'UnBiased X Residual Pixel Barrel 2'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_pix_b3_residualsx;pix_b3_residualx' 
    title = 'UnBiased X Residual Pixel Barrel 3'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_pix_b0_residualsy;pix_b0_residualy' 
    title = 'UnBiased Y Residual Pixel Barrel 0'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minPIXResYFillRange, xmax=m_maxPIXResYFillRange)

    varName = 'm_pix_b1_residualsy;pix_b1_residualy' 
    title = 'UnBiased Y Residual Pixel Barrel 1'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minPIXResYFillRange, xmax=m_maxPIXResYFillRange)

    varName = 'm_pix_b2_residualsy;pix_b2_residualy' 
    title = 'UnBiased Y Residual Pixel Barrel 2'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minPIXResYFillRange, xmax=m_maxPIXResYFillRange)

    varName = 'm_pix_b3_residualsy;pix_b3_residualy' 
    title = 'UnBiased Y Residual Pixel Barrel 3'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minPIXResYFillRange, xmax=m_maxPIXResYFillRange)

    varName = 'm_pix_b0_pullsx;pix_b0_pullx' 
    title = 'UnBiased X Pull Pixel Barrel 0'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_pix_b1_pullsx;pix_b1_pullsx' 
    title = 'UnBiased X Pull Pixel Barrel 1'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_pix_b2_pullsx;pix_b2_pullx' 
    title = 'UnBiased X Pull Pixel Barrel 2'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_pix_b3_pullsx;pix_b3_pullx' 
    title = 'UnBiased X Pull Pixel Barrel 3'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_pix_b0_pullsy;pix_b0_pully' 
    title = 'UnBiased Y Pull Pixel Barrel 0'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_pix_b1_pullsy;pix_b1_pully' 
    title = 'UnBiased X Pull Pixel Barrel 1'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_pix_b2_pullsy;pix_b2_pully' 
    title = 'UnBiased Y Pull Pixel Barrel 2'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_pix_b3_pullsy;pix_b3_pully' 
    title = 'UnBiased Y Pull Pixel Barrel 3'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    #Pixel EndCap A plots
    varName = 'm_pix_eca_residualx;pix_eca_residualx'
    title = 'UnBiased X Residual Pixel EndCap A;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_pix_eca_residualy;pix_eca_residualy'
    title = 'UnBiased Y Residual Pixel EndCap A;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minPIXResYFillRange, xmax=m_maxPIXResYFillRange)

    varName = 'm_pix_eca_pullx;pix_eca_pulllx'
    title = 'UnBiased X Pull Pixel EndCap A'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_pix_eca_pully;pix_eca_pullly'
    title = 'UnBiased Y Pull Pixel EndCap A'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    #Pixel EndCap C plots
    varName = 'm_pix_ecc_residualx;pix_ecc_residualx'
    title = 'UnBiased X Residual Pixel EndCap C;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_pix_ecc_residualy;pix_ecc_residualy'
    title = 'UnBiased Y Residual Pixel EndCap C;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minPIXResYFillRange, xmax=m_maxPIXResYFillRange)

    varName = 'm_pix_ecc_pullx;pix_ecc_pulllx'
    title = 'UnBiased X Pull Pixel EndCap C'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_pix_ecc_pully;pix_ecc_pullly'
    title = 'UnBiased Y Pull Pixel EndCap C'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    #SCT
    #Barrel Plots
    varName = 'm_sct_b_residualx;sct_b_residualx'
    title = 'UnBiased X Residual SCT Barrel;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_sct_b_biased_residualx;sct_b_biasedresidualx'
    title = 'Biased X Residual SCT Barrel;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_sct_b0_residualsx;sct_b0_residualx' 
    title = 'UnBiased X Residual SCT Barrel 0'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_sct_b1_residualsx;sct_b1_residualx' 
    title = 'UnBiased X Residual SCT Barrel 1'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_sct_b2_residualsx;sct_b2_residualx' 
    title = 'UnBiased X Residual SCT Barrel 2'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_sct_b3_residualsx;sct_b3_residualx' 
    title = 'UnBiased X Residual SCT Barrel 3'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_sct_b0_pullsx;sct_b0_pullx' 
    title = 'UnBiased X Pull SCT Barrel 0'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_sct_b1_pullsx;sct_b1_pullx' 
    title = 'UnBiased X Pull SCT Barrel 1'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_sct_b2_pullsx;sct_b2_pullx' 
    title = 'UnBiased X Pull SCT Barrel 2'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    varName = 'm_sct_b3_pullsx;sct_b3_pullx' 
    title = 'UnBiased X Pull SCT Barrel 3'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)

    #SCT EndCap A plots
    varName = 'm_sct_eca_residualx;sct_eca_residualx'
    title = 'UnBiased X Residual SCT EndCap A;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_sct_eca_pullx;sct_eca_pulllx'
    title = 'UnBiased X Pull SCT EndCap A'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)
    
    #SCT EndCap C plots
    varName = 'm_sct_ecc_residualx;sct_ecc_residualx'
    title = 'UnBiased X Residual SCT EndCap C;Residual [mm]'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=m_minSiResFillRange, xmax=m_maxSiResFillRange)

    varName = 'm_sct_ecc_pullx;sct_ecc_pulllx'
    title = 'UnBiased X Pull SCT EndCap C'
    residualGroup.defineHistogram(varName, type='TH1F', path=pathResiduals, title=title, xbins=100, xmin=-m_RangeOfPullHistos, xmax=m_RangeOfPullHistos)
    
    # end histograms
