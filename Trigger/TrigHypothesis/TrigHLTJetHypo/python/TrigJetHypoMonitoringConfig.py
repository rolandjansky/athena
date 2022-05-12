# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def TrigJetHypoToolMonitoring(histPath, histFlags):
    montool = GenericMonitoringTool("MonTool", HistPath = histPath)
    # Always make these.
    montool.defineHistogram('Et', title='Jet E_{T};E_{T} (GeV)', xbins=100, xmin=0, xmax=500, path='EXPERT', type='TH1F' )
    montool.defineHistogram('TIME_jetHypo,NJetsIn', title='JetHypo time vs input jets;time (ms) ;N(jets)', xbins=50, xmin=0, xmax=10, ybins=40, ymin=-0.5, ymax=39.5, path='EXPERT', type='TH2F' )
    montool.defineHistogram('TIME_jetHypo,NJetsOut', title='JetHypo time vs jets;time (ms) ;N(jets)', xbins=50, xmin=0, xmax=10, ybins=30, ymin=-0.5, ymax=29.5, path='EXPERT', type='TH2F' )
    # Conditional histograms: monitor the mass for largeR jets (anything but a4), and etaphi for simple smallR
    if 'a4' not in histFlags:  montool.defineHistogram('Mass', title='Jet mass;m (GeV)', xbins=100, xmin=0, xmax=200, path='EXPERT', type='TH1F' )
    if ('simple' in histFlags) and ('a4' in histFlags) and all("HT" not in flag for flag in histFlags): 
        montool.defineHistogram('Eta,Phi', title='Jet #eta vs #phi;#eta;#phi', xbins=40, xmin=-5, xmax=5, ybins=25, ymin=-3.145, ymax=3.145, path='EXPERT', type='TH2F' )
    # Conditional histograms: hypoScenarios like HTXXXX will match.
    if any("HT" in flag for flag in histFlags) : 
        montool.defineHistogram('HT', title='Event H_{T};H_{T} (GeV)', xbins=100, xmin=0, xmax=3000, path='EXPERT', type='TH1F' )
        montool.defineHistogram('NJet', title='Jet multiplicity;N(jets)', xbins=20, xmin=-0.5, xmax=19.5, path='EXPERT', type='TH1F' )   
    return montool
