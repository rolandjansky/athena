# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigHIHypo.TrigHIHypoConf import VnHypo

from AthenaCommon.SystemOfUnits import TeV,GeV
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
class VnMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="VnMonitoring", harmonic=1):
        super(VnMonitoring, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])
        self.Histograms += [ defineHistogram('TotalEt',
                                             type='TH1F',
                                             title="Total ET; ET [MeV]",
                                             xbins = 100, xmin=0.0, xmax=6.5*TeV)]
        self.Histograms += [ defineHistogram('TotalEtPassing',
                                             type='TH1F',
                                             title="Total ET for passing events; ET [MeV]",
                                             xbins = 100, xmin=0.0, xmax=6.5*TeV)]


        self.Histograms += [ defineHistogram('q',
                                             type='TH1F',
                                             title="q%d of checked events" % harmonic,
                                             xbins = 50, xmin=0.0, xmax=0.25)]

        self.Histograms += [ defineHistogram('qPassing',
                                             type='TH1F',
                                             title="q%d of passing events" % harmonic,
                                             xbins = 50, xmin=0.0, xmax=0.25)]
        

        self.Histograms += [ defineHistogram('TotalEt, decision',
                                             type='TH2F',
                                             title="(all row 0) (passing row 1); FCal Et",
                                             xbins = 100, xmin=0.0, xmax=6.5*TeV,
                                             ybins=2, ymin=-0.5, ymax=1.5) ]

class VnBootstrap(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="VnBootstrap", harmonic=1):
        super(VnBootstrap, self).__init__(name)
        self.defineTarget(['Online', 'Validation'])

        self.Histograms += [ defineHistogram('TotalEt, q',
                                             type='TH2F',
                                             title="q%d vs Fcal of all events" % harmonic,
                                             xbins=100, xmin=200*GeV, xmax=5.*TeV,
                                             ybins=100, ymin=0, ymax=0.2) ]            

class QZeroMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="QZeroMonitoring", harmonic=1, qmax=0.3):
        super(QZeroMonitoring, self).__init__(name)        
        self.defineTarget(['Online', 'Validation'])

#        self.Histograms += [ defineHistogram('TotalEt, qnx', type="TH2F", title="TotalEt[TeV];q_{%d x 0};q_{x0}" % harmonic, xbins=100, xmin=0, xmax=4.5*TeV, ybins=30, ymin=-qmax, ymax=qmax) ]
#        self.Histograms += [ defineHistogram('TotalEt, qny', type="TH2F", title="TotalEt[TeV];q_{%d y 0};q_{y0}" % harmonic, xbins=100, xmin=0, xmax=4.5*TeV, ybins=30, ymin=-qmax, ymax=qmax) ]
        self.Histograms += [ defineHistogram('TotalEt, qnx', type="TProfile", title="FCal Event q0x;TotalEt[TeV];q_{%d x 0}" % harmonic, xbins=100, xmin=0, xmax=6.5*TeV) ]
        self.Histograms += [ defineHistogram('TotalEt, qny', type="TProfile", title="FCal Event q0y;TotalEt[TeV];q_{%d y 0}" % harmonic, xbins=100, xmin=0, xmax=6.5*TeV) ]

# _EtCuts=[
#     3.4432, 3.2977, 3.1648, 3.0400, 2.9222, 2.8101, 2.7033, 2.6014, 2.5038, 2.4104, 
#     2.3208, 2.2343, 2.1516, 2.0715, 1.9941, 1.9194, 1.8473, 1.7776, 1.7100, 1.6448, 
#     1.5815, 1.5203, 1.4610, 1.4034, 1.3479, 1.2939, 1.2417, 1.1910, 1.1418, 1.0942, 
#     1.0482, 1.0036, 0.9604, 0.9185, 0.8780, 0.8388, 0.8009, 0.7642, 0.7288, 0.6946, 
#     0.6615, 0.6295, 0.5987, 0.5689, 0.5401, 0.5126, 0.4859, 0.4604, 0.4357, 0.4120, 
#     0.3892, 0.3674, 0.3465, 0.3265, 0.3073, 0.2889, 0.2714, 0.2546, 0.2387, 0.2235,
#     0.2090, 0.1953, 0.1822, 0.1698, 0.1582, 0.1471, 0.1366, 0.1268, 0.1175, 0.1088] 
# #    0.1005, 0.0928, 0.0856, 0.0789, 0.0725, 0.0666, 0.0611, 0.0559, 0.0511, 0.0467, 
# #    0.0425, 0.0386, 0.0350, 0.0316, 0.0285, 0.0256, 0.0229, 0.0203, 0.0180, 0.0158, 
# #    0.0137, 0.0118, 0.0100, 0.0083, 0.0067, 0.0051, 0.0033, -0.0085,    -1,     -2 ]
    
_EtCuts_v2=[ x*1e-3 for x in reversed(range(300, 6050, 50)) ] # equidistant bins
_EtCuts_v3=[ x*1e-3 for x in reversed(range(400, 6050, 50)) ]



########################################################################################
# V2

q2xshift=0.0036
q2yshift=-0.00034

class V2Hypo(VnHypo):
    __slots__ = []
    def __init__(self, name):
        super( VnHypo, self ).__init__( name )
        self.FlowHarmonic = 2
        self.CentralityBins=_EtCuts_v2
        self.QxShifts = [q2xshift]*len(_EtCuts_v2)
        self.QyShifts = [q2yshift]*len(_EtCuts_v2)
        self.AthenaMonTools += [VnMonitoring(name="VnMonitoring", harmonic=self.FlowHarmonic)]
        
V2_th16 = V2Hypo("V2_th16")
V2_th16.AthenaMonTools += [QZeroMonitoring(name="QZeroMonitoring", harmonic=2, qmax=2), VnBootstrap(name="V2Bootstrap", harmonic=2)]
V2_th16.CentralityBins=[-0.5]
V2_th16.QThresholds=[1e3]
V2_th16.QxShifts=[q2xshift]
V2_th16.QyShifts=[q2yshift]


def generateVnThresholds(a,b,c, etbins):
    return [ a - b*et +c*pow(et, 2)  for et in etbins]
    
# thresholds mappint
#      %v2     %v3
#15    2.5     2.5
#14    5       5 
#13    10      10
#10    1       1 
#5     0.5     20

V2_th15 = V2Hypo("V2_th15")            
V2_th15.QThresholds = generateVnThresholds(0.144638, 0.0239043, -0.000368144, _EtCuts_v2)

V2_th14 = V2Hypo("V2_th14")            
V2_th14.QThresholds = generateVnThresholds(0.134306, 0.0213092, -0.00069103, _EtCuts_v2)

V2_th13 = V2Hypo("V2_th13")
V2_th13.QThresholds = generateVnThresholds(0.12048, 0.0172576, -0.0011478, _EtCuts_v2)


V2_th10 = V2Hypo("V2_th10")
V2_th10.QThresholds = generateVnThresholds(0.158364, 0.0281745, 0.000147842, _EtCuts_v2)

V2_th5 = V2Hypo("V2_th5")
V2_th5.QThresholds =  generateVnThresholds(0.165639, 0.0295911, 0.00035603, _EtCuts_v2)


V2_th = {16: V2_th16, 15: V2_th15, 14: V2_th14, 13: V2_th13, 10: V2_th10, 5: V2_th5 }
def V2(names, threshold):
    return V3_th[threshold]


V2_th1_veto = V2Hypo("V2_th1_veto")
V2_th1_veto.UpperLimit=True
V2_th1_veto.QThresholds = generateVnThresholds(0.0330088, -0.00256366, -0.00233638, _EtCuts_v2)


########################################################################################
# V3
q3xshift=-0.0017
q3yshift=0.0013



class V3Hypo(VnHypo):
    __slots__ = []
    def __init__(self, name):
        super( VnHypo, self ).__init__( name )
        self.FlowHarmonic = 3
        self.CentralityBins = _EtCuts_v3
        self.QxShifts = [q3xshift]*len(_EtCuts_v3)
        self.QyShifts =  [q3yshift]*len(_EtCuts_v3)
        self.AthenaMonTools += [VnMonitoring(name="VnMonitoring", harmonic=self.FlowHarmonic)]
        
V3_th16 =V3Hypo("V3_th16")
V3_th16.CentralityBins=[-0.5]
V3_th16.QThresholds=[1e3]
V3_th16.QxShifts=[q3xshift]
V3_th16.QyShifts=[q3yshift]

V3_th16.AthenaMonTools += [QZeroMonitoring(name="QZeroMonitoring", harmonic=3, qmax=2), VnBootstrap(name="V3Bootstrap", harmonic=3)]

V3_th15 = V3Hypo("V3_th15")
V3_th15.QThresholds = generateVnThresholds(0.0680338, 0.0128576, 0.000901204, _EtCuts_v3)

V3_th14 = V3Hypo("V3_th14")
V3_th14.QThresholds = generateVnThresholds(0.0632529, 0.0135102, 0.00102213, _EtCuts_v3)

V3_th13 = V3Hypo("V3_th13")
V3_th13.QThresholds = generateVnThresholds(0.0549295, 0.0111602, 0.0006531, _EtCuts_v3)

V3_th10 = V3Hypo("V3_th10")
V3_th10.QThresholds = generateVnThresholds(0.0777345, 0.0161675, 0.00145098, _EtCuts_v3)


V3_th5 = V3Hypo("V3_th5")
V3_th5.QThresholds = generateVnThresholds(0.0456977, 0.00745393, -2.59157e-05,  _EtCuts_v3)

V3_th = {16: V3_th16, 15: V3_th15, 14: V3_th14, 13: V3_th13, 10: V3_th10, 5: V3_th5 }
def V3(names, threshold):
    return V3_th[threshold]


V3_th1_veto = V3Hypo("V3_th1_veto")
V3_th1_veto.UpperLimit=True
V3_th1_veto.QThresholds = generateVnThresholds(0.0118349, 0.00241723, 0.000276767, _EtCuts_v3)
