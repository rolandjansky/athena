#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
                                defineHistogram('deltaEta0',title='Precision#Delta#eta0', path='EXPERT',xbins=40, xmin=-0.01,xmax=0.01),
                                defineHistogram('deltaEta1',title='Precision#Delta#eta1', path='EXPERT',xbins=40, xmin=-0.01,xmax=0.01),
                                defineHistogram('deltaEta2',title='Precision#Delta#eta2', path='EXPERT',xbins=40, xmin=-0.01,xmax=0.01),
                                defineHistogram('deltaEta3',title='Precision#Delta#eta3', path='EXPERT',xbins=40, xmin=-0.01,xmax=0.01),
                                defineHistogram('deltaPhi0',title='Precision#Delta#eta0', path='EXPERT',xbins=40, xmin=-0.01,xmax=0.01),
                                defineHistogram('deltaPhi1',title='Precision#Delta#phi1', path='EXPERT',xbins=40, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhi2',title='Precision#Delta#phi2', path='EXPERT',xbins=40, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhi3',title='Precision#Delta#phi3', path='EXPERT',xbins=40, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhiRescaled0',title='Precision#Delta#phiRescaled0', path='EXPERT',xbins=40, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhiRescaled1',title='Precision#Delta#phiRescaled1', path='EXPERT',xbins=40, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhiRescaled2',title='Precision#Delta#phiRescaled2', path='EXPERT',xbins=40, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhiRescaled3',title='Precision#Delta#phiRescaled3', path='EXPERT',xbins=40, xmin=-0.1,xmax=0.1),
                                defineHistogram('deltaPhiLast',title='Precision#Delta#phiLastMeasurement', path='EXPERT',xbins=40, xmin=-0.1,xmax=0.1),
                                defineHistogram('d0',title='Precision#d0', path='EXPERT',xbins=40, xmin=-1,xmax=1),

                           #Shower Shapes variables monitoring
                                defineHistogram('Reta', type='TH1F', path='EXPERT',title="Reta",xbins=15, xmin=0.4, xmax=1.2),
                                defineHistogram('Rphi', type='TH1F', path='EXPERT',title="Rphi",xbins=15, xmin=0.4, xmax=1.2),
                                defineHistogram('Rhad', type='TH1F', path='EXPERT',title="Rhad",xbins=35, xmin=-0.3, xmax=0.3 ), 
                                defineHistogram('Rhad1', type='TH1F', path='EXPERT',title="Rhad",xbins=30, xmin=-0.3, xmax=0.3 ),  
                                defineHistogram('ethad',   type='TH1F', path='EXPERT',title="Transverse energy in the first sampling of the hadronic calorimeters behind the cluster calculated from ehad1 ; E [MeV]",xbins=20, xmin=-1, xmax=1 ),
                                defineHistogram('ethad1',   type='TH1F', path='EXPERT',title="ET leakage into hadronic calorimeter with exclusion of energy in CaloSampling; E [MeV]",xbins=20, xmin=-1, xmax=1 ),
                                defineHistogram('weta1',    type='TH1F', path='EXPERT',title="Precision Reconstruction corrected width in 3 strips in the 1st samp. ; Width",xbins=12, xmin=0.4, xmax=1 ), 
                                defineHistogram('weta2',    type='TH1F', path='EXPERT',title="Precision Reconstruction corrected width in 3 strips in the 2st samp. ; Width",xbins=20, xmin=0, xmax=0.02 ),
                                defineHistogram('wtots1',   type='TH1F', path='EXPERT',title="Precision Reconstruction total width in em sampling 1 in 20 strips; Width",xbins=20, xmin=0, xmax=20 ), 
                                defineHistogram('f1',    type='TH1F', path='EXPERT',title="Fraction of energy found in 1st em sampling;Fraction",xbins=11, xmin=0, xmax=1 ),
                                defineHistogram('f3',    type='TH1F', path='EXPERT',title="Fraction of energy reconstructed in 3rd sampling ;Fraction",xbins=21, xmin=-0.05, xmax=0.1 ),
                                defineHistogram('eratio',    type='TH1F', path='EXPERT',title="Eratio ;Count",xbins=20, xmin=0, xmax=2 ),
                           
                           #Isolation variables monitoring
                                defineHistogram('ptcone20',    type='TH1F', path='EXPERT',title="count;ptcone20",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('ptcone30',    type='TH1F', path='EXPERT',title="count;ptcone30",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('ptcone40',   type='TH1F', path='EXPERT', title="count;ptcone40",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('ptvarcone20',    type='TH1F', path='EXPERT',title="count;ptvarcone20",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('ptvarcone30',    type='TH1F', path='EXPERT',title="count;ptvarcone30",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('ptvarcone40',   type='TH1F', path='EXPERT', title="count;ptvarcone40",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('topoetcone20',    type='TH1F', path='EXPERT',title="count;topoetcone20[GeV]",xbins=100, xmin=-10, xmax=50 ),
                                defineHistogram('topoetcone40_shift',    type='TH1F', path='EXPERT',title="count;topoetcone40[GeV]",xbins=100, xmin=-10, xmax=50 )]
    
        return monTool_electron
    else:
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
        monTool_photon = GenericMonitoringTool("MonTool_"+name)
        monTool_photon.Histograms = [ 
                           
                           #Shower Shapes variables monitoring
                                defineHistogram('Reta', type='TH1F', path='EXPERT',title="Reta",xbins=15, xmin=0.4, xmax=1.2),
                                defineHistogram('Rphi', type='TH1F', path='EXPERT',title="Rphi",xbins=15, xmin=0.4, xmax=1.2),
                                defineHistogram('Rhad', type='TH1F', path='EXPERT',title="Rhad",xbins=35, xmin=-0.3, xmax=0.3 ), 
                                defineHistogram('Rhad1', type='TH1F', path='EXPERT',title="Rhad",xbins=30, xmin=-0.3, xmax=0.3 ),  
                                defineHistogram('ethad',   type='TH1F', path='EXPERT',title="Transverse energy in the first sampling of the hadronic calorimeters behind the cluster calculated from ehad1 ; E [MeV]",xbins=20, xmin=-1, xmax=1 ),
                                defineHistogram('ethad1',   type='TH1F', path='EXPERT',title="ET leakage into hadronic calorimeter with exclusion of energy in CaloSampling; E [MeV]",xbins=20, xmin=-1, xmax=1 ),
                                defineHistogram('weta1',    type='TH1F', path='EXPERT',title="Precision Reconstruction corrected width in 3 strips in the 1st samp. ; Width",xbins=12, xmin=0.4, xmax=1 ), 
                                defineHistogram('weta2',    type='TH1F', path='EXPERT',title="Precision Reconstruction corrected width in 3 strips in the 2st samp. ; Width",xbins=20, xmin=0, xmax=0.02 ),
                                defineHistogram('wtots1',   type='TH1F', path='EXPERT',title="Precision Reconstruction total width in em sampling 1 in 20 strips; Width",xbins=20, xmin=0, xmax=20 ), 
                                defineHistogram('f1',    type='TH1F', path='EXPERT',title="Fraction of energy found in 1st em sampling;Fraction",xbins=11, xmin=0, xmax=1 ),
                                defineHistogram('f3',    type='TH1F', path='EXPERT',title="Fraction of energy reconstructed in 3rd sampling ;Fraction",xbins=21, xmin=-0.05, xmax=0.1 ),
                                defineHistogram('eratio',    type='TH1F', path='EXPERT',title="Eratio ;Count",xbins=20, xmin=0, xmax=2 ),
                                
                           
                           #Isolation variables monitoring
                                defineHistogram('ptcone20',    type='TH1F', path='EXPERT',title="count;ptcone20",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('ptcone30',    type='TH1F', path='EXPERT',title="count;ptcone30",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('ptcone40',   type='TH1F', path='EXPERT', title="count;ptcone40",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('ptvarcone20',    type='TH1F', path='EXPERT',title="count;ptvarcone20",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('ptvarcone30',    type='TH1F', path='EXPERT',title="count;ptvarcone30",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('ptvarcone40',   type='TH1F', path='EXPERT', title="count;ptvarcone40",xbins=50, xmin=0, xmax=5 ),
                                defineHistogram('topoetcone20',    type='TH1F', path='EXPERT',title="count;topoetcone20[GeV]",xbins=100, xmin=-10, xmax=50 ),
                                defineHistogram('topoetcone40_shift',    type='TH1F', path='EXPERT',title="count;topoetcone40[GeV]",xbins=100, xmin=-10, xmax=50 )]
    
        return monTool_photon


def egammaMonitorSuperClusterCfg(name):

     from math import pi
    
     from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
     monTool = GenericMonitoringTool("MonTool_"+name)
     monTool.Histograms = [ 
                           #calo cluster variables monitoring
                              defineHistogram('et',             type='TH1D', path='EXPERT', title="E_{T};              E_{T} [GeV];        Entries", xbins= 150, xmin= 0.0, xmax= 200.0),
                              defineHistogram('eta',            type='TH1D', path='EXPERT', title="#eta;               #eta;               Entries", xbins=  50, xmin=-3.0, xmax=   3.0) ,
                              defineHistogram('phi',            type='TH1D', path='EXPERT', title="#phi;               #phi;               Entries", xbins=  64, xmin= -pi, xmax=    pi),
                              defineHistogram('container_size', type='TH1D', path='EXPERT', title="Container Size;     Number of Clusters; Entries", xbins=2001, xmin=-0.5, xmax=2000.5),
                              defineHistogram('clusterSize',    type='TH1D', path='EXPERT', title="Cluster Size;       Type;               Entries", xbins=  13, xmin= 0.5, xmax=  13.5),
                              defineHistogram('signalState',    type='TH1D', path='EXPERT', title="Signal State;       Signal State;       Entries", xbins=   4, xmin=-1.5, xmax=   2.5)] 
     
     return monTool
    
