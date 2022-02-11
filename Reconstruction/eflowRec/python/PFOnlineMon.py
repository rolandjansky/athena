# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def getMonTool_eflowTrackCaloExtensionTool():
  from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
  monTool = GenericMonitoringTool('MonTool_TrackCaloExtension')

  monTool.defineHistogram( 'TIME_execute', path='EXPERT', type='TH1F', title='Extension tool - execution time; Execution time [us]; Counts',
                             xbins=100, xmin=0., xmax=50. )
  monTool.defineHistogram( 'TIME_extrapolation', path='EXPERT', type='TH1F', title='Extension tool - extrapolation time; Extrapolation time [us]; Counts',
                             xbins=100, xmin=0., xmax=25. )
  monTool.defineHistogram( 'track_pt, TIME_extrapolation', path='EXPERT', type='TH2F', title='Extension tool - extrapolation time vt. track p_{T}; Track p_{T} [GeV]; Extrapolation time [us]  ; Counts',
                             xbins=100, xmin=0., xmax=15., ybins=100, ymin=0., ymax=25.)
  return monTool


def getMonTool_PFTrackSelector():
  from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
  monTool = GenericMonitoringTool('MonTool')

  monTool.defineHistogram( 'TIME_execute', path='EXPERT', type='TH1F', title='Track selector - execution time; Execution time [ms]; Counts',
                           xbins=60, xmin=0., xmax=20. )
  monTool.defineHistogram( 'TIME_track', path='EXPERT', type='TH1F', title='Track selector - extrapolation time per track; Time per track [us]; Counts',
                           xbins=100, xmin=0., xmax=50. )
  monTool.defineHistogram( 'N_tracks', path='EXPERT', type='TH1F', title='Track selector - number of selected tracks per event; Selected tracks/event; Counts',
                           xbins=25, xmin=0., xmax=1000. )
  monTool.defineHistogram( 'eta_track', path='EXPERT', type='TH1F', title='Track selector - track #eta; #eta; Counts',
                           xbins=120, xmin=-3., xmax=3. )
  monTool.defineHistogram( 'pt_track', path='EXPERT', type='TH1F', title='Track selector - track p_{T}; Track p_{T} [GeV]; Counts',
                           xbins=100, xmin=0., xmax=15. )

  monTool.defineHistogram( 'eta_track,TIME_track', path='EXPERT', type='TH2F', title='Track selector - track #eta vs. extrapolation time;#eta; Extrapolation time per track [us]',
                           xbins=30, xmin=-3., xmax=3., ybins=50, ymin=0., ymax=50. )
  monTool.defineHistogram( 'pt_track,TIME_track', path='EXPERT', type='TH2F', title='Track selector - track p_{T} vs. extrapolation time; Track p_{T} [GeV]; Extrapolation time per track [us]',
                           xbins=25, xmin=0., xmax=15., ybins=50, ymin=0., ymax=50. )
  return monTool

def getMonTool_PFTrackClusterMatching():
  from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
  monTool = GenericMonitoringTool('MonTool_ClusterMatching')

  monTool.defineHistogram( 'matched_tracks_pt', path='EXPERT', type='TH1F', title='Number of matched tracks with given p_{T}; p_{T} [GeV]; Counts',
                          xbins=40, xmin=0., xmax=20. )
  monTool.defineHistogram( 'matched_clusters_e', path='EXPERT', type='TH1F', title='Number of matched clusters with given energy; E [GeV]; Counts',
                          xbins=40, xmin=0., xmax=20. )
  monTool.defineHistogram( 'matched_clusters_eta', path='EXPERT', type='TH1F', title='Number of matched clusters with given eta; #eta; Counts',
                          xbins=40, xmin=-4., xmax=4. )
  monTool.defineHistogram( 'matched_clusters_phi', path='EXPERT', type='TH1F', title='Number of matched clusters with given phi; #phi; Counts',
                          xbins=40, xmin=-3.14159265, xmax=3.14159265 )

  return monTool

def getMonTool_PFAlgorithm():
  from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
  monTool = GenericMonitoringTool('MonTool')

  monTool.defineHistogram( 'TIME_execute', path='EXPERT', type='TH1F', title='PFlow algorithm - execution time; Time [ms]; Counts',
                           xbins=100, xmin=0., xmax=500. )
  monTool.defineHistogram( 'TIME_subtract', path='EXPERT', type='TH1F', title='PFlow algorithm - subtraction time; Time [ms]; Counts',
                           xbins=100, xmin=0., xmax=500. )
  monTool.defineHistogram( 'N_efrClusters', path='EXPERT', type='TH1F', title='PFlow algorithm - number of clusters per events; Number of clusters per events; Counts',
                           xbins=40, xmin=0., xmax=4000. )
  return monTool

