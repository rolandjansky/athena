#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TrigEgammaMonitoring.egammaMonitorPrecisionConfig") 

def egammaMonitorPrecisionCfg(name):


    if ('Electron' in name): 
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
        monTool_electron = GenericMonitoringTool("MonTool_"+name)
        monTool_electron.Histograms = [ 
                           #track variables monitoring
                                defineHistogram('deltaEta0',title='Precision#Delta#eta0', path='EXPERT',xbins=80, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaEta1',title='Precision#Delta#eta1', path='EXPERT',xbins=80, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaEta2',title='Precision#Delta#eta2', path='EXPERT',xbins=80, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaEta3',title='Precision#Delta#eta3', path='EXPERT',xbins=80, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhi0',title='Precision#Delta#eta0', path='EXPERT',xbins=80, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhi1',title='Precision#Delta#phi1', path='EXPERT',xbins=80, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhi2',title='Precision#Delta#phi2', path='EXPERT',xbins=80, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhi3',title='Precision#Delta#phi3', path='EXPERT',xbins=80, xmin=-0.1,xmax=0.1),
                           
                           #Shower Shapes variables monitoring
                                defineHistogram('e237',type='TH1F', path='EXPERT',title="Precision Reconstruction uncor energy in 3x7 cells in em sampling 2; E [MeV]",xbins=50, xmin=-15000, xmax=150000 ),
                                defineHistogram('e277', type='TH1F', path='EXPERT',title="Precision Reconstruction  uncor energy in 7x7 cells in em sampling 2;E [MeV]",xbins=50, xmin=-15000, xmax=150000 ),
                                defineHistogram('Rhad', type='TH1F', path='EXPERT',title="Rhad",xbins=50, xmin=-1, xmax=1 ), 
                                defineHistogram('Rhad1', type='TH1F', path='EXPERT',title="Rhad",xbins=50, xmin=-1, xmax=1 ),  
                                defineHistogram('ethad',   type='TH1F', path='EXPERT',title="Transverse energy in the first sampling of the hadronic calorimeters behind the cluster calculated from ehad1 ; E [MeV]",xbins=50, xmin=-15000, xmax=150000 ),
                                defineHistogram('ethad1',   type='TH1F', path='EXPERT',title="ET leakage into hadronic calorimeter with exclusion of energy in CaloSampling; E [MeV]",xbins=50, xmin=-15000, xmax=150000 ),
                                defineHistogram('weta1',    type='TH1F', path='EXPERT',title="Precision Reconstruction corrected width in 3 strips in the 1st samp. ; Width",xbins=20, xmin=0, xmax=1 ), 
                                defineHistogram('weta2',    type='TH1F', path='EXPERT',title="Precision Reconstruction corrected width in 3 strips in the 2st samp. ; Width",xbins=20, xmin=0, xmax=0.05 ),
                                defineHistogram('wtots1',   type='TH1F', path='EXPERT',title="Precision Reconstruction total width in em sampling 1 in 20 strips; Width",xbins=20, xmin=0, xmax=20 ), 
                                defineHistogram('f1',    type='TH1F', path='EXPERT',title="Fraction of energy found in 1st em sampling;Fraction",xbins=50, xmin=-0.1, xmax=1.1 ),
                                defineHistogram('f3',    type='TH1F', path='EXPERT',title="Fraction of energy reconstructed in 3rd sampling ;Fraction",xbins=50, xmin=-0.1, xmax=1.1 ),
                                defineHistogram('eratio',    type='TH1F', path='EXPERT',title="Eratio ;Count",xbins=50, xmin=0, xmax=2 ),
                                defineHistogram('fracs1',   type='TH1F', path='EXPERT', title="Reconstruction energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",xbins=50, xmin=-5000, xmax=10000 ),
                           
                           #Isolation variables monitoring
                                defineHistogram('ptcone20',    type='TH1F', path='EXPERT',title="count;ptcone20",xbins=50, xmin=0, xmax=10 ),
                                defineHistogram('ptcone30',    type='TH1F', path='EXPERT',title="count;ptcone30",xbins=50, xmin=0, xmax=10 ),
                                defineHistogram('ptcone40',   type='TH1F', path='EXPERT', title="count;ptcone40",xbins=50, xmin=0, xmax=10 ),
                                defineHistogram('topoetcone20',    type='TH1F', path='EXPERT',title="count;topoetcone20[GeV]",xbins=50, xmin=-10, xmax=50 )]
    
        return monTool_electron
    else:
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
        monTool_photon = GenericMonitoringTool("MonTool_"+name)
        monTool_photon.Histograms = [ 
                           
                           #Shower Shapes variables monitoring
                                defineHistogram('e237',type='TH1F', path='EXPERT',title="Precision Reconstruction uncor energy in 3x7 cells in em sampling 2; E [MeV]",xbins=50, xmin=-15000, xmax=150000 ),
                                defineHistogram('e277', type='TH1F', path='EXPERT',title="Precision Reconstruction  uncor energy in 7x7 cells in em sampling 2;E [MeV]",xbins=50, xmin=-15000, xmax=150000 ),
                                defineHistogram('Rhad', type='TH1F', path='EXPERT',title="Rhad",xbins=50, xmin=-1, xmax=1 ), 
                                defineHistogram('Rhad1', type='TH1F', path='EXPERT',title="Rhad",xbins=50, xmin=-1, xmax=1 ),  
                                defineHistogram('ethad',   type='TH1F', path='EXPERT',title="Transverse energy in the first sampling of the hadronic calorimeters behind the cluster calculated from ehad1 ; E [MeV]",xbins=50, xmin=-15000, xmax=150000 ),
                                defineHistogram('ethad1',   type='TH1F', path='EXPERT',title="ET leakage into hadronic calorimeter with exclusion of energy in CaloSampling; E [MeV]",xbins=50, xmin=-15000, xmax=150000 ),
                                defineHistogram('weta1',    type='TH1F', path='EXPERT',title="Precision Reconstruction corrected width in 3 strips in the 1st samp. ; Width",xbins=20, xmin=0, xmax=1 ), 
                                defineHistogram('weta2',    type='TH1F', path='EXPERT',title="Precision Reconstruction corrected width in 3 strips in the 2st samp. ; Width",xbins=20, xmin=0, xmax=0.05 ),
                                defineHistogram('wtots1',   type='TH1F', path='EXPERT',title="Precision Reconstruction total width in em sampling 1 in 20 strips; Width",xbins=20, xmin=0, xmax=20 ), 
                                defineHistogram('f1',    type='TH1F', path='EXPERT',title="Fraction of energy found in 1st em sampling;Fraction",xbins=50, xmin=-0.1, xmax=1.1 ),
                                defineHistogram('f3',    type='TH1F', path='EXPERT',title="Fraction of energy reconstructed in 3rd sampling ;Fraction",xbins=50, xmin=-0.1, xmax=1.1 ),
                                defineHistogram('eratio',    type='TH1F', path='EXPERT',title="Eratio ;Count",xbins=50, xmin=0, xmax=2 ),
                                defineHistogram('fracs1',   type='TH1F', path='EXPERT', title="Reconstruction energy outside core (E(+-7)-E(+-3))/E(+-7); E [MeV]",xbins=50, xmin=-5000, xmax=10000 ),
                           
                           #Isolation variables monitoring
                                defineHistogram('ptcone20',    type='TH1F', path='EXPERT',title="count;ptcone20",xbins=50, xmin=0, xmax=10 ),
                                defineHistogram('ptcone30',    type='TH1F', path='EXPERT',title="count;ptcone30",xbins=50, xmin=0, xmax=10 ),
                                defineHistogram('ptcone40',   type='TH1F', path='EXPERT', title="count;ptcone40",xbins=50, xmin=0, xmax=10 ),
                                defineHistogram('topoetcone20',    type='TH1F', path='EXPERT',title="count;topoetcone20[GeV]",xbins=50, xmin=-10, xmax=50 )]
        return monTool_photon
