# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from math import pi as PI
#================================================================================
# FTK Online
#================================================================================
class FtkHltEfficiencyMonitoring(TrigGenericMonitoringToolConfig):
  def __init__ (self, name="FtkHltEfficiencyMonitoring"):
    super(FtkHltEfficiencyMonitoring, self).__init__(name)
    self.defineTarget( ["Online", "Validation"] )
    
    EfficiencyPars      = ['pt'          ,'eta'  ,'phi'         ,'d0'         ,'z0'         ,'nPix'      ,'nSct'          ,'nSi'           ,'chi2Ndf'       ,'eta_phi'              , 'eta_z0']
    EffParNames         = ['p_{t} [GeV]' ,'#eta' ,'#phi [rad]'  ,'d_{0} [mm]' ,'z_{0} [mm]' ,'#hits PIX' ,'#hits SCT'     ,'#hits PIX+SCT' ,'#chi^{2}/NDF'  ,'#phi + 2#pi i_{#eta}' , 'z_{0} + #Delta z_{0} i_{#eta}']
    EffParRanges        = [[0,10]        ,[-3,3] ,[-PI,PI]      ,[-3,3]       ,[-200,200]   ,[-.5,9.5]   ,[-.5,19.5]      ,[-.5,29.5]      ,[0,10]          ,[-PI, (7)*PI]          , [-100, 700]]
    EffParNBins         = [10            ,10     ,10            ,10           ,10           ,10          ,20              ,30              ,10              ,40                     , 40]

    ResolutionPars      = ['pt'          ,'eta'        ,'phi'         ,'d0'         ,'z0'       ,'R']
    ResParNames         = ['p_{t} [GeV]' ,'#eta'       ,'#phi [rad]'  ,'d_{0} [mm]' ,'z_{0}'    ,'R']
    ResParRanges        = [[-.5,.5]      ,[-0.02,0.02] ,[-0.02,0.02]  ,[-0.5,0.5]   ,[-1,1]     ,[0,0.05]]
    ResParNBins         = [15            ,15           ,15            ,15           ,15         ,15]
       
    ProfileParsX        = ['phi'                    ,'eta'              ]
    ProfileParsY        = ['d0'                     ,'z0'              ]
    ProfileRangesX      = [[-PI, PI]                ,[-3,3]             ]
    ProfileAxisNames    = ['#phi [rad];d_{0} [mm]'  ,'#eta;z_{0} [mm]'  ]
    ProfileNBinsX       = [10                       ,10                 ]
   
    nBins = 0

    print "FtkHltEfficiencyMonitoring: booking histos"
    for i_var in range(len(EfficiencyPars)):
      nBins += 6*EffParNBins[i_var]
       # hlt track parameters
      self.Histograms += [ defineHistogram(
          'HLT_trk_'+EfficiencyPars[i_var], 
          type='TH1F',
          title='HLT tracks;'+EffParNames[i_var]+';#events',
          xbins=EffParNBins[i_var], xmin=EffParRanges[i_var][0], xmax=EffParRanges[i_var][1])]
      # ftk assigned track parameters
      self.Histograms += [ defineHistogram(
          'FTK_match_trk_'+EfficiencyPars[i_var],
          type='TH1F',
          title='FTK matched tracks;'+EffParNames[i_var]+';#events',
          xbins=EffParNBins[i_var], xmin=EffParRanges[i_var][0], xmax=EffParRanges[i_var][1])]
      # ftk all track parameters
      self.Histograms += [ defineHistogram(
          'FTK_trk_'+EfficiencyPars[i_var],
          type='TH1F',
          title='FTK all tracks;'+EffParNames[i_var]+';#events',
          xbins=EffParNBins[i_var], xmin=EffParRanges[i_var][0], xmax=EffParRanges[i_var][1])]
      # ftk fake track parameters
      self.Histograms += [ defineHistogram(
          'FTK_fake_trk_'+EfficiencyPars[i_var],
          type='TH1F',
          title='FTK fake tracks;'+EffParNames[i_var]+';#events',
          xbins=EffParNBins[i_var], xmin=EffParRanges[i_var][0], xmax=EffParRanges[i_var][1])]
      # ftk track efficiency
      self.Histograms += [ defineHistogram(
          'HLT_trk_'+EfficiencyPars[i_var]+',FTKHLT_eff_weight',
          type='TProfile',
          title='FTK vs HLT track efficiency;'+EffParNames[i_var]+';#FTK_{matched} / #HLT' ,
          xbins=EffParNBins[i_var], xmin=EffParRanges[i_var][0], xmax=EffParRanges[i_var][1])]
      # ftk track fake rate
      self.Histograms += [ defineHistogram(
          'FTK_trk_'+EfficiencyPars[i_var]+',FTKHLT_fake_weight',
          type='TProfile',
          title='FTK vs HLT track fake rate;'+EffParNames[i_var]+';#FTK_{unmatched} / #FTK',
          xbins=EffParNBins[i_var], xmin=EffParRanges[i_var][0], xmax=EffParRanges[i_var][1])]


    for i_var in range(len(ResolutionPars)):
      nBins += ResParNBins[i_var]
      # resolutions  
      self.Histograms += [ defineHistogram(
          'delta_'+ResolutionPars[i_var],
          type='TH1F',
          title='FTK vs HLT track resolution;#Delta '+ResParNames[i_var]+';#events',
          xbins=ResParNBins[i_var], xmin=ResParRanges[i_var][0], xmax=ResParRanges[i_var][1])]


    for i_var in range(len(ProfileParsX)):
      nBins += 2*ProfileNBinsX[i_var]        
      self.Histograms += [ defineHistogram(
          'HLT_trk_'+ProfileParsX[i_var]+', HLT_trk_'+ProfileParsY[i_var],
          type='TProfile',
          title='HLT tracks;'+ProfileAxisNames[i_var],
          xbins=ProfileNBinsX[i_var], xmin=ProfileRangesX[i_var][0], xmax=ProfileRangesX[i_var][1])]
      
      self.Histograms += [ defineHistogram(
          'FTK_trk_'+ProfileParsX[i_var]+', FTK_trk_'+ProfileParsY[i_var],
          type='TProfile',
          title='FTK all tracks;'+ProfileAxisNames[i_var],
          xbins=ProfileNBinsX[i_var], xmin=ProfileRangesX[i_var][0], xmax=ProfileRangesX[i_var][1])]


    # efficiency weights
    nBins += 2 + 2
    self.Histograms += [ defineHistogram(
        'FTKHLT_eff_weight', 
        type='TH1F',
        title="FTK vs HLT efficiency weight",
        xbins=2, xmin=-.5, xmax=1.5) ]
    self.Histograms += [ defineHistogram(
        'FTKHLT_fake_weight',
        type='TH1F',
        title="FTK vs HLT fake weight",
        xbins=2, xmin=-.5, xmax=1.5) ]

    # others
    nBins += 5 + 5
    self.Histograms += [ defineHistogram(
        'numb_HLT_trks',
        type='TH1F',
        title='number of HLT tracks;#tracks;#events',
        xbins=50, xmin=0, xmax=500)]
    self.Histograms += [ defineHistogram(
        'numb_FTK_trks',
        type='TH1F',
        title='number of FTK tracks;#tracks;#events',
        xbins=50, xmin=0, xmax=500)]

    print "FtkHltEfficiencyMonitoring: booked", nBins, " histogram bins"

