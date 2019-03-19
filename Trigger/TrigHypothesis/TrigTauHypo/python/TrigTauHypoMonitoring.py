# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" TrigTauRec  Monitoring """

__author__  = 'O.Igonkina'
__version__=""
__doc__="Implementation of TrigTauRec  Monitoring"
from AthenaCommon.SystemOfUnits import GeV

################ Validation, DQ checks
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigTauHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self,  name):
        super(TrigTauHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = []

    def setDefaults(self, handle):
        super(TrigTauHypoOnlineMonitoring, self).setDefaults( handle)
        if not hasattr( handle, 'name' ):
           myName = "SomeAlgo"
        else:
           myName=handle.getName()

        if myName.find("T2TauHypo") > -1:
            cuts=['Input','EmRadius3S 1P', 'etOverPtLeadTrk','trkAvgDist', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        elif myName.find("T2IDTauHypo") > -1:
            cuts=['Input','has tracksInfo','nTrks','ldTrkPt','sumPtRatio', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        elif myName.find("T2IDCoreTauHypo") > -1:
            cuts=['Input','has tracksInfo','nTrks','leading tk. p_{T}', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        elif myName.find("T2IDIsoTauHypo") > -1:
            cuts=['Input','has tracksInfo','sumPtRatio', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        elif myName.find("T2CaloTauHypo") > -1:
            cuts=['Input','has TrigTauCluster vector','has exactly one TrigTauCluster','TrigTauCluster is ok','E_{T} Raw','EMRadius3S','Core Fraction', ' ', ' ', ' ', ' ', ' ', ' ' ]

        elif myName.find("EFTauInvHypo") > -1:
            cuts=['Input','has tau details','E_{T} calib','tk. number','EM radius', 'dr trk avg','Et ov leadPt', ' ', ' ', ' ', ' ', ' ', ' ']

        elif (myName.find("tracktwo") > -1 or myName.find("loose1") > -1 or myName.find("medium1") > -1 or myName.find("tight1") > -1 or myName.find("FTK") > -1):
            cuts=['Input','has tau details','E_{T} calib','tk. number','score cut', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        elif (myName.find("kaon") > -1 or myName.find("pion") > -1):
            cuts=['Input', 'tau cand' ,'E_{T} calib','tk. number','lead tr pt','massTrkSys','massTrkSysKaon','massTrkSysKaonPi','EMPOverTrkSysP', 'etOverPtLeadTrk', 'dRmax', ' ', ' ', ' ', ' ']

        elif myName.find("ditauM") > -1 or myName.find("ditauL") > -1 or myName.find("ditauT") > -1:
            cuts=['Input','good vtx/trk', 'match', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        elif myName.find("tsf") > -1:
            cuts=['Input','dR', '', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        elif myName.find("EFTauTopo") > -1:
            cuts=['Input','dR', 'passed', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        elif myName.find("mVis") > -1:
            cuts=['Input','mVis', '', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        elif myName.find("HLTCaloTauHypo") > -1:
            cuts=['Input', 'E_{T} calib', 'Calo Cuts', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        elif myName.find("HLTTrackTauHypo") > -1:
            cuts=['Input', 'Track Cuts', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']

        else:
            cuts=[]
        

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'

        if hasattr( handle, 'Histograms' ) and not 'CutCounter' in handle.Histograms:
           handle.Histograms += [ defineHistogram('CutCounter', type='TH1F', title=myName+" cut counter;cut; nevents", xbins=13, xmin=-0.5, xmax=12.5,  
           opt="kCumulative", 
           labels=labelsDescription) ]

##########   Create instance for validation  - same as online here
class TrigTauHypoValidationMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(TrigTauHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

##########   Create instance for cosmics  - same as online here
class TrigTauHypoCosmicMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(TrigTauHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")


##########  setTauMonTools  ########## 

def setTauMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3] 

    nameOnl = algoName+"_Onl"
    onlTool = TrigTauHypoOnlineMonitoring(nameOnl )

    nameVal = algoName+"_Val"
    valTool = TrigTauHypoValidationMonitoring(nameVal)

    nameCos = algoName+"_Cos"
    cosTool = TrigTauHypoCosmicMonitoring(nameCos)

    algoObject.AthenaMonTools = [ time, onlTool, valTool, cosTool ]



#########   Additional T2IDTau specific histograms

class T2IDTauHypoOnlineMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(T2IDTauHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")


class T2IDTauHypoValidationMonitoring(TrigTauHypoValidationMonitoring):
    def __init__ (self, name):
        super(T2IDTauHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = []

    def setDefaults(self, handle):
        super(T2IDTauHypoValidationMonitoring, self).setDefaults(handle)

        if not hasattr( handle, 'name' ):
           myName = "SomeAlgo"
        else:
           myName=handle.getName()

        if hasattr( handle, 'Histograms' ) and not 'nTrks'      in handle.Histograms:
            handle.Histograms += [ defineHistogram('nTrks',     type='TH1F', title = myName + " Core tracks;Core tracks; nevents", xbins=10,  xmin=-0.5, xmax=9.5) ]

        if hasattr( handle, 'Histograms' ) and not 'sumPtRatio' in handle.Histograms:
            handle.Histograms += [ defineHistogram('sumPtRatio',type='TH1F', title = myName + " SumPtRatio; sumPt ratio; nevents", xbins=40,  xmin=0.,   xmax=2.) ]

        if hasattr( handle, 'Histograms' ) and not 'ldTrkPt'    in handle.Histograms:
            handle.Histograms += [ defineHistogram('ldTrkPt',   type='TH1F', title = myName + " LdTrkPt; LdTrkPt; nevents",        xbins=100, xmin=0.,   xmax=100000.) ]



######################################################

def setTauT2IDMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig    
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3] 

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"

    valTool     = T2IDTauHypoValidationMonitoring( nameVal )
    onlTool     = T2IDTauHypoOnlineMonitoring( nameOnl )
    algoObject.AthenaMonTools = [ time, onlTool, valTool ]



#########   Additional T2Tau specific histograms

class T2TauHypoOnlineMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(T2TauHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")


class T2TauHypoValidationMonitoring(TrigTauHypoValidationMonitoring):
    def __init__ (self, name):
        super(T2TauHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = []

    def setDefaults(self, handle):
        super(T2TauHypoValidationMonitoring, self).setDefaults(handle)

        if not hasattr( handle, 'name' ):
           myName = "SomeAlgo"
        else:
           myName=handle.getName()

        if hasattr( handle, 'Histograms' ) and not 'nTrks2011'       in handle.Histograms:
            handle.Histograms += [ defineHistogram('nTrks2011',      type='TH1F', title = myName + " Core+Iso tracks;Core+Iso tracks; nevents",  xbins=10,  xmin=-0.5, xmax=9.5) ]

        if hasattr( handle, 'Histograms' ) and not 'nTrks2012'       in handle.Histograms:
            handle.Histograms += [ defineHistogram('nTrks2012',      type='TH1F', title = myName + " Core tracks;Core tracks; nevents",          xbins=10,  xmin=-0.5, xmax=9.5) ]

        if hasattr( handle, 'Histograms' ) and not 'etOverPtLeadTrk' in handle.Histograms:
            handle.Histograms += [ defineHistogram('etOverPtLeadTrk',type='TH1F', title = myName + " etOverPtLeadTrk; etOverPtLeadTrk; nevents", xbins=50,  xmin=0.,   xmax=20.) ]

        if hasattr( handle, 'Histograms' ) and not 'trkAvgDist'      in handle.Histograms:
            handle.Histograms += [ defineHistogram('trkAvgDist',     type='TH1F', title = myName + " trkAvgDist; trkAvgDist; nevents",           xbins=100, xmin=0.,   xmax=2.) ]



######################################################

def setTauT2TauMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig    
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3] 

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"

    valTool     = T2TauHypoValidationMonitoring( nameVal )
    onlTool     = T2TauHypoOnlineMonitoring( nameOnl )
    algoObject.AthenaMonTools = [ time, onlTool, valTool ]





######################################################
## CORE
class T2IDCoreTauHypoValidationMonitoring(TrigTauHypoValidationMonitoring):
    def __init__ (self, name):
        super(T2IDCoreTauHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = []

    def setDefaults(self, handle):
        super(T2IDCoreTauHypoValidationMonitoring, self).setDefaults(handle)

        if not hasattr( handle, 'name' ):
           myName = "SomeAlgo"
        else:
           myName=handle.getName()

        if hasattr( handle, 'Histograms' ) and not 'nTrks'      in handle.Histograms:
            handle.Histograms += [ defineHistogram('nTrks',     type='TH1F', title = myName + " Core tracks;Core tracks; nevents", xbins=10,  xmin=-0.5, xmax=9.5) ]

        if hasattr( handle, 'Histograms' ) and not 'ldTrkPt'    in handle.Histograms:
            handle.Histograms += [ defineHistogram('ldTrkPt',   type='TH1F', title = myName + " LdTrkPt; LdTrkPt; nevents",        xbins=100, xmin=0.,   xmax=100000.) ]



def setTauT2CoreMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig    
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3] 

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"

    valTool     = T2IDCoreTauHypoValidationMonitoring( nameVal )
    onlTool     = T2IDTauHypoOnlineMonitoring( nameOnl )
    algoObject.AthenaMonTools = [ time, onlTool, valTool ]



######################################################
## ISO
class T2IDIsoTauHypoValidationMonitoring(TrigTauHypoValidationMonitoring):
    def __init__ (self, name):
        super(T2IDIsoTauHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = []

    def setDefaults(self, handle):
        super(T2IDIsoTauHypoValidationMonitoring, self).setDefaults(handle)

        if not hasattr( handle, 'name' ):
           myName = "SomeAlgo"
        else:
           myName=handle.getName()

        if hasattr( handle, 'Histograms' ) and not 'nTrks'      in handle.Histograms:
            handle.Histograms += [ defineHistogram('nTrks',     type='TH1F', title = myName + " Core tracks;Core tracks; nevents", xbins=10,  xmin=-0.5, xmax=9.5) ]

        if hasattr( handle, 'Histograms' ) and not 'sumPtRatio' in handle.Histograms:
            handle.Histograms += [ defineHistogram('sumPtRatio',type='TH1F', title = myName + " SumPtRatio; sumPt ratio; nevents", xbins=40,  xmin=0.,   xmax=2.) ]


def setTauT2IsoMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig    
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3] 

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"

    valTool     = T2IDIsoTauHypoValidationMonitoring( nameVal )
    onlTool     = T2IDTauHypoOnlineMonitoring( nameOnl )
    algoObject.AthenaMonTools = [ time, onlTool, valTool ]




#########   Additional EF specific histograms

class L2TauTopoHypoOnlineMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(L2TauTopoHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        
        self.Histograms += [defineHistogram('DROfAccepted',type='TH1F',title='dR Passed', xbins=50, xmin=0.,xmax=4.)]

class EFTauTopoHypoOnlineMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(EFTauTopoHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [defineHistogram('DROfProcessed',type='TH1F',title='dR Processed', xbins=100, xmin=0.,xmax=4.)]
        self.Histograms += [defineHistogram('DROfAccepted',type='TH1F',title='dR Accepted', xbins=100, xmin=0.,xmax=4.)]

class L2TauTopoHypoValidationMonitoring(L2TauTopoHypoOnlineMonitoring):
    def __init__ (self, name):
        super(L2TauTopoHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

class EFTauTopoHypoValidationMonitoring(EFTauTopoHypoOnlineMonitoring):
    def __init__ (self, name):
        super(EFTauTopoHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

class EFTauInvHypoOnlineMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(EFTauInvHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

class EFTauInvHypoValidationMonitoring(TrigTauHypoValidationMonitoring):
    def __init__ (self, name):
        super(EFTauInvHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

class EFTauMVHypoOnlineMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(EFTauMVHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        
        self.Histograms += [defineHistogram('EtCalib',type='TH1F',title='Tau Pt Passed', xbins=50, xmin=0.,xmax=100.)]
        self.Histograms += [defineHistogram('NTrack',type='TH1F',title='Tau nTrack Passed', xbins=10, xmin=-0.5,xmax=9.5)]
        self.Histograms += [defineHistogram('NWideTrack',type='TH1F',title='Tau nWideTrack Passed', xbins=10, xmin=-0.5,xmax=9.5)]
        self.Histograms += [defineHistogram('NInputTaus',type='TH1F',title='# Input Taus', xbins=20, xmin=-0.5,xmax=19.5)]

class EFTauMVHypoValidationMonitoring(EFTauMVHypoOnlineMonitoring):
    def __init__ (self, name):
        super(EFTauMVHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

class EFPhotonTauHypoOnlineMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(EFPhotonTauHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [defineHistogram('MvisOfAccepted',type='TH1F',title='mVis Passed', xbins=50, xmin=0.,xmax=150.)]

class EFPhotonTauHypoValidationMonitoring(EFPhotonTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(EFPhotonTauHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")


class EFTauDiKaonHypoOnlineMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(EFTauDiKaonHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [defineHistogram('EtCalib',type='TH1F',title='Tau Pt Passed', xbins=50, xmin=0.,xmax=100.)]
        self.Histograms += [defineHistogram('NTrack',type='TH1F',title='Tau nTrack Passed', xbins=10, xmin=-0.5,xmax=9.5)]
        self.Histograms += [defineHistogram('NWideTrack',type='TH1F',title='Tau nWideTrack Passed', xbins=10, xmin=-0.5,xmax=9.5)]
        self.Histograms += [defineHistogram('MassTrkSys',type='TH1F',title='Tau massTrkSys Passed', xbins=50, xmin=0.,xmax=2000.)]
        self.Histograms += [defineHistogram('MassTrkSysKaon',type='TH1F',title='Tau massTrkSysKaon Passed', xbins=50, xmin=0.,xmax=2000.)]
        self.Histograms += [defineHistogram('MassTrkSysKaonPi',type='TH1F',title='Tau massTrkSysKaonPi Passed', xbins=50, xmin=0.,xmax=2000.)]
        self.Histograms += [defineHistogram('LeadTrkPt',type='TH1F',title='Tau leadTrkPt Passed', xbins=50, xmin=0.,xmax=50000.)]
        self.Histograms += [defineHistogram('DR',type='TH1F',title='Tau dRmax Passed', xbins=40, xmin=0.,xmax=0.4)]
        self.Histograms += [defineHistogram('EtOverPtLeadTrk',type='TH1F',title='Tau etOverPtLeadTrk Passed', xbins=50, xmin=0.,xmax=5.)]
        self.Histograms += [defineHistogram('EMOverTrkSysP',type='TH1F',title='Tau EMOverTrkSysP Passed', xbins=50, xmin=0.,xmax=5.)]  

class EFTauDiKaonHypoValidationMonitoring(EFTauDiKaonHypoOnlineMonitoring):
    def __init__ (self, name):
        super(EFTauDiKaonHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

class HLTVertexPreselHypoOnlineMonitoring(TrigTauHypoOnlineMonitoring):
    def __init__ (self, name):
        super(HLTVertexPreselHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [defineHistogram('Z0DistanceAll',type='TH1F',title='Z0 distance All', xbins=50, xmin=0.,xmax=10.)]
        self.Histograms += [defineHistogram('Z0DistanceAccepted',type='TH1F',title='Z0 distance Accepted', xbins=50, xmin=0.,xmax=10.)]

class HLTVertexPreselHypoValidationMonitoring(HLTVertexPreselHypoOnlineMonitoring):
    def __init__ (self, name):
        super(HLTVertexPreselHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

######################################################
def setTauEFMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig    
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3] 

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"
    valTool = EFTauInvHypoValidationMonitoring(nameVal)
    onlTool = EFTauInvHypoOnlineMonitoring(nameOnl)
     
    algoObject.AthenaMonTools = [ time, onlTool, valTool ]

def setL2TauTopoMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3]

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"
    valTool = L2TauTopoHypoValidationMonitoring(nameVal)
    onlTool = L2TauTopoHypoOnlineMonitoring(nameOnl)

    algoObject.AthenaMonTools = [ time, onlTool, valTool]

def setEFTauTopoMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3]

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"
    valTool = EFTauTopoHypoValidationMonitoring(nameVal)
    onlTool = EFTauTopoHypoOnlineMonitoring(nameOnl)

    algoObject.AthenaMonTools = [ time, onlTool, valTool]

def setTauEFMVMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3]

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"
    valTool = EFTauMVHypoValidationMonitoring(nameVal)
    onlTool = EFTauMVHypoOnlineMonitoring(nameOnl)

    algoObject.AthenaMonTools = [ time, onlTool, valTool]

def setPhotonTauMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3]

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"
    valTool = EFPhotonTauHypoValidationMonitoring(nameVal)
    onlTool = EFPhotonTauHypoOnlineMonitoring(nameOnl)

    algoObject.AthenaMonTools = [ time, onlTool, valTool]


def setTauEFDiKaonMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3]

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"
    valTool = EFTauDiKaonHypoValidationMonitoring(nameVal)
    onlTool = EFTauDiKaonHypoOnlineMonitoring(nameOnl)

    algoObject.AthenaMonTools = [ time, onlTool, valTool]

def setHLTVertexPreselMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3]

    nameOnl = algoName+"_Onl"
    nameVal = algoName+"_Val"
    onlTool = HLTVertexPreselHypoOnlineMonitoring(nameOnl)
    valTool = HLTVertexPreselHypoValidationMonitoring(nameVal)

    algoObject.AthenaMonTools = [ time, onlTool, valTool]

    
######################################################
#   HadCalib triggers monitoring
######################################################
class HadCalibHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self,  name):
        super(HadCalibHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = []

    def setDefaults(self, handle):
        super(HadCalibHypoOnlineMonitoring, self).setDefaults( handle)
        if not hasattr( handle, 'name' ):
           myName = "SomeAlgo"
        else:
           myName=handle.getName()

        cuts=['All','pT','pixHits','sctHits','D0','Z0','eta','isolation']

        labelsDescription = ''
        for c in cuts:
            labelsDescription +=  c+':'


        if hasattr( handle, 'Histograms' ) and not 'CutCounter' in handle.Histograms:
           handle.Histograms += [ defineHistogram('CutCounter', type='TH1F', title=myName+" cut counter;cut; nevents", xbins=8, xmin=-0.5, xmax=7.5, 
           opt="kCumulative", 
           labels=labelsDescription) ]

        if hasattr( handle, 'Histograms' ) and not 'nTracksInIso' in handle.Histograms:
           handle.Histograms  += [ defineHistogram('nTracksInIso', type='TH1F', title=myName+" Number of tracks in isolation cone;Number of tracks; nevents", xbins=10, xmin=-0.5, xmax=9.5) ]

        if hasattr( handle, 'Histograms' ) and not 'HTnPixHit' in handle.Histograms:
           handle.Histograms  += [ defineHistogram('HTnPixHit', type='TH1F', title=myName+" Number of Pixel hits from leading track;Number of pixel hits; nentries", xbins=10, xmin=-0.5, xmax=9.5) ]

        if hasattr( handle, 'Histograms' ) and not 'HTnSCTHit' in handle.Histograms:
           handle.Histograms  += [ defineHistogram('HTnSCTHit', type='TH1F', title=myName+" Number of SCT hits from leading track;Number of SCT hits; nentries", xbins=10, xmin=-0.5, xmax=9.5) ]

        if hasattr( handle, 'Histograms' ) and not 'HTpT' in handle.Histograms:
           handle.Histograms += [ defineHistogram('HTpT', type='TH1F', title=myName+" Pt of leading track;PT [MeV]; nentries", xbins=100, xmin=0., xmax=50000.) ]

        if hasattr( handle, 'Histograms' ) and not 'HTz0' in handle.Histograms:
           handle.Histograms += [ defineHistogram('HTz0', type='TH1F', title=myName+" Z0 of leading track;z0 [mm]; nentries", xbins=100, xmin=-50., xmax=50.) ]

        if hasattr( handle, 'Histograms' ) and not 'HTd0' in handle.Histograms:
           handle.Histograms += [ defineHistogram('HTd0', type='TH1F', title=myName+" D0 of leading track;d0 [mm]; nentries", xbins=100, xmin=-50., xmax=50.) ]

        if hasattr( handle, 'Histograms' ) and not 'HTeta,HTphi' in handle.Histograms:
           handle.Histograms += [ defineHistogram('HTeta,HTphi', type='TH2F', title="Eta vs phi of leading track;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=80, ymin=-4., ymax=4.) ]


class HadCalibHypoValidationMonitoring(HadCalibHypoOnlineMonitoring):
    def __init__ (self, name):
        super(HadCalibHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")


######################################################
def setHadCalibEFMonTools( algoObject):    

    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3]

    nameVal = algoName+"_Val"
    nameOnl = algoName+"_Onl"
    valTool = HadCalibHypoValidationMonitoring(nameVal)
    onlTool = HadCalibHypoOnlineMonitoring(nameOnl)

    algoObject.AthenaMonTools = [ time, onlTool, valTool ]

class T2TauTauCombValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2TauTauCombValidationMonitoring"):
        """ defines histograms for validation """
        super(TrigGenericMonitoringToolConfig, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('dPhiOfAll', type='TH1F', title="Tau-Tau dPhi",
                                             xbins=50, xmin=0., xmax=5.) ]       
        self.Histograms += [ defineHistogram('dROfAll', type='TH1F', title="Tau-Tau dR",
                                             xbins=50, xmin=0., xmax=5.) ]       
        self.Histograms += [ defineHistogram('massOfAll', type='TH1F', title="Tau-Tau Mass ",
                                             xbins=60, xmin=0.*GeV, xmax=400.*GeV) ]
        self.Histograms += [ defineHistogram('dEtaOfAll', type='TH1F', title="Tau-Tau dEta",
                                             xbins=50, xmin=0., xmax=5.) ]       
        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=8, xmin=-0.5, xmax=7.5) ]
        self.Histograms += [ defineHistogram('combtype', type='TH1F', title="Tau combination",
                                             xbins=4, xmin=-0.5, xmax=3.5,labels='null:tautau:taumu:taue') ]
                             
class T2TauTauCombOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2TauTauCombOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigGenericMonitoringToolConfig, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('dPhiOfAll', type='TH1F', title="Tau-Tau dPhi",
                                             xbins=50, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram('dROfAll', type='TH1F'  , title="Tau-Tau dR",
                                             xbins=50, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram('massOfAll', type='TH1F', title="Tau-Tau Mass ",
                                             xbins=50, xmin=0.*GeV, xmax=400.*GeV) ]
        self.Histograms += [ defineHistogram('dEtaOfAll', type='TH1F', title="Tau-Tau dEta",
                                             xbins=50, xmin=0., xmax=5.) ]       
        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=8, xmin=-0.5, xmax=7.5) ]
        self.Histograms += [ defineHistogram('combtype', type='TH1F', title="Tau combination",
                                             xbins=4, xmin=-0.5, xmax=3.5,labels='null:tautau:taumu:taue') ]

class EFTauTauCombValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFTauTauCombValidationMonitoring"):
        """ defines histograms for validation """
        super(TrigGenericMonitoringToolConfig, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('dPhiOfAll', type='TH1F', title="Tau-Tau dPhi",
                                             xbins=50, xmin=0., xmax=5.) ]       
        self.Histograms += [ defineHistogram('dROfAll', type='TH1F', title="Tau-Tau dR",
                                             xbins=50, xmin=0., xmax=5.) ]       
        self.Histograms += [ defineHistogram('massOfAll', type='TH1F', title="Tau-Tau Mass ",
                                             xbins=60, xmin=0.*GeV, xmax=400.*GeV) ]
        self.Histograms += [ defineHistogram('dEtaOfAll', type='TH1F', title="Tau-Tau dEta",
                                             xbins=50, xmin=0., xmax=5.) ]       
        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=8, xmin=-0.5, xmax=7.5) ]
        self.Histograms += [ defineHistogram('combtype', type='TH1F', title="Tau combination",
                                             xbins=4, xmin=-0.5, xmax=3.5,labels='null:tautau:taumu:taue') ]

class EFTauTauCombOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFTauTauCombOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigGenericMonitoringToolConfig, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('dPhiOfAll', type='TH1F', title="Tau-Tau dPhi",
                                             xbins=50, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram('dROfAll', type='TH1F'  , title="Tau-Tau dR",
                                             xbins=50, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram('massOfAll', type='TH1F', title="Tau-Tau Mass ",
                                             xbins=50, xmin=0.*GeV, xmax=400.*GeV) ]
        self.Histograms += [ defineHistogram('dEtaOfAll', type='TH1F', title="Tau-Tau dEta",
                                             xbins=50, xmin=0., xmax=5.) ]       
        self.Histograms += [ defineHistogram('cut', type='TH1F', title="Cut counter",
                                             xbins=8, xmin=-0.5, xmax=7.5) ]
        self.Histograms += [ defineHistogram('combtype', type='TH1F', title="Tau combination",
                                             xbins=4, xmin=-0.5, xmax=3.5,labels='null:tautau:taumu:taue') ]



######################################################
class HLTCaloPreselHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self,  name):
        super(HLTCaloPreselHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = []

    def setDefaults(self, handle):
        super(HLTCaloPreselHypoValidationMonitoring, self).setDefaults( handle)
        if not hasattr( handle, 'name' ):
           myName = "SomeAlgo"
        else:
           myName=handle.getName()


        if hasattr( handle, 'Histograms' ) and not 'seedPt' in handle.Histograms:
           handle.Histograms  += [ defineHistogram('seedPt', type='TH1F', title=myName+" P_{T} of seed taus;P_{T} of cadidate [MeV]; nevents", xbins=100, xmin=0., xmax=50000.) ]

        if hasattr( handle, 'Histograms' ) and not 'fCore' in handle.Histograms:
           handle.Histograms  += [ defineHistogram('fCore', type='TH1F', title=myName+" Fractional central E_{T};Fractional core E_{T}; nentries", xbins=100, xmin=-0.2, xmax=1.5) ]

        if hasattr( handle, 'Histograms' ) and not 'hadRadius' in handle.Histograms:
           handle.Histograms  += [ defineHistogram('hadRadius', type='TH1F', title=myName+" Average transverse-hadronic-energy weighted cell radius;Average energy-weighted cell radius [mm]; nentries", xbins=100, xmin=0.0, xmax=0.5) ]

        if hasattr( handle, 'Histograms' ) and not 'dRBaryCenterRoI' in handle.Histograms:
           handle.Histograms  += [ defineHistogram('dRBaryCenterRoI', type='TH1F', title=myName+" Delta R between tau barycenter and RoI;DeltaR tau barycenter - RoI; nentries", xbins=100, xmin=0.0, xmax=1.0) ]


######################################################
class HLTTrackPreselHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self,  name):
        super(HLTTrackPreselHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = []

    def setDefaults(self, handle):
        super(HLTTrackPreselHypoValidationMonitoring, self).setDefaults( handle)
        if not hasattr( handle, 'name' ):
           myName = "SomeAlgo"
        else:
           myName=handle.getName()


        if hasattr( handle, 'Histograms' ) and not 'nTracksInCore' in handle.Histograms:
           handle.Histograms += [ defineHistogram('nTracksInCore', type='TH1F', title=myName+" Number of tracks in core region;Number of tracks in core; nentries", xbins=10,  xmin=-0.5, xmax=9.5) ]

        if hasattr( handle, 'Histograms' ) and not 'nTracksInIso' in handle.Histograms:
           handle.Histograms += [ defineHistogram('nTracksInIso', type='TH1F', title=myName+" Number of tracks in outer region;Number of tracks in outer region; nentries", xbins=10,  xmin=-0.5, xmax=9.5) ]

######################################################
def setHLTCaloPreselMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig    
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3] 

    nameVal = algoName+"_Val"
    valTool = HLTCaloPreselHypoValidationMonitoring(nameVal)

    algoObject.AthenaMonTools = [ time, valTool ]

######################################################
def setHLTTrackPreselMonTools( algoObject ):
    algoName=algoObject.getName()

    from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig    
    time = TrigTimeHistToolConfig(algoName+"Time")
    time.TimerHistLimits = [0,3] 

    nameVal = algoName+"_Val"
    valTool = HLTTrackPreselHypoValidationMonitoring(nameVal)

    algoObject.AthenaMonTools = [ time, valTool ] 
