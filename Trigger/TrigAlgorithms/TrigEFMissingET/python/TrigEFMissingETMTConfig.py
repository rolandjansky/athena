# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def getMETMonTool(name = "MonTool"):
   """
   Return configured GenericMonitoringTool instance for EFMissingETAlgMT
   """
   mon = GenericMonitoringTool("MonTool")
   mon.defineHistogram( "TIME_Total", path='EXPERT', title="Time spent Alg",           xbins=100, xmin=0, xmax=100 )
   mon.defineHistogram( "TIME_Loop",  path='EXPERT', title="Time spent in Tools loop", xbins=100, xmin=0, xmax=100 )

   # signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
   # central bin is +/- 0.075 and contains values whose abs < 1.189 GeV
   # limits are +/- 1.189 TeV
   mon.defineHistogram('EF_MEx_log', path='EXPERT', title="EF Missing E_{x};sgn(ME_{x}) log_{10}(ME_{x}/GeV)", xbins=41, xmin=-3.075, xmax=3.075)
   mon.defineHistogram('EF_MEy_log', path='EXPERT', title="EF Missing E_{y};sgn(ME_{y}) log_{10}(ME_{y}/GeV)", xbins=41, xmin=-3.075, xmax=3.075)
   mon.defineHistogram('EF_MEz_log', path='EXPERT', title="EF Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)", xbins=41, xmin=-3.075, xmax=3.075)

   # signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
   # bin centered on zero is +/- 0.075 and contains values whose abs < 1.189 GeV
   # limits are -74.99 GeV and 2.371 TeV
   mon.defineHistogram('EF_SumEt_log', path='EXPERT', title="EF Sum E_{T};log_{10}(SumE_{T}/GeV)", xbins=40, xmin=-1.875, xmax=4.125)
   mon.defineHistogram('EF_SumE_log',  path='EXPERT', title="EF Sum E;log_{10}(SumE/GeV)",         xbins=40, xmin=-1.875, xmax=4.125)

   # unsigned (i.e. normal) log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
   # limits are 708 MeV and 2.818 TeV
   mon.defineHistogram('EF_MET_log', path='EXPERT', title="EF |Missing E_{T}|;log_{10}(ME_{T}/GeV)", xbins=24, xmin=-0.15, xmax=3.45)
   mon.defineHistogram('EF_ME_log',  path='EXPERT', title="EF |Missing Energy|;log_{10}(ME/GeV)",    xbins=24, xmin=-0.15, xmax=3.45)

   # linear-scale
   mon.defineHistogram('EF_MET_lin',   path='EXPERT', title="EF |Missing E_{T}|;ME_{T} (GeV)", xbins=205, xmin=-13.5,   xmax=601.5)
   mon.defineHistogram('EF_ME_lin',    path='EXPERT', title="EF |Missing Energy|;ME (GeV)",    xbins=205, xmin=-13.5,   xmax=601.5)
   mon.defineHistogram('EF_SumEt_lin', path='EXPERT', title="EF Sum E_{T};SumE_{T} (GeV)",     xbins=305, xmin=-27.,    xmax=1803.)
   mon.defineHistogram('EF_XS',        path='EXPERT', title="EF Significance; (XS/GeV^{1/2})", xbins=40,  xmin=-0.025,  xmax=20.025)
   mon.defineHistogram('EF_MET_phi',   path='EXPERT', title="EF MET #phi;MET #phi (rad)",      xbins=32,  xmin=-3.1416, xmax=3.1416)
   mon.defineHistogram('EF_SumE_lin',  path='EXPERT', title="EF Sum E;SumE (GeV)",             xbins=305, xmin=-27.,    xmax=3500.)
   mon.defineHistogram('EF_MEx_lin',   path='EXPERT', title="EF Missing E_{x};ME_{x} (GeV)",   xbins=199, xmin=-298.5,  xmax=298.5)
   mon.defineHistogram('EF_MEy_lin',   path='EXPERT', title="EF Missing E_{y};ME_{y} (GeV)",   xbins=199, xmin=-298.5,  xmax=298.5)
   mon.defineHistogram('EF_MEz_lin',   path='EXPERT', title="EF Missing E_{z};ME_{z} (GeV)",   xbins=199, xmin=-298.5,  xmax=298.5)

   return mon
