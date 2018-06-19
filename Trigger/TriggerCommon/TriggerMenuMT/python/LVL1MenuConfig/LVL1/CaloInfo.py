# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Lvl1MenuUtil import log
from Lvl1MenuUtil import oldStyle
from Limits import CaloLimits

class Isolation:

    class Parametrization:
        def __init__(self, isobit, offset, slope, mincut, upperlimit, etamin, etamax, priority):
            self.isobit     = int(isobit)
            self.offset     = int(offset)
            self.slope      = int(slope)
            self.upperlimit = int(upperlimit)
            self.etamin     = int(etamin)
            self.etamax     = int(etamax)
            self.priority   = int(priority)
            self.mincut     = int(mincut)  # minimum value for the isolation cut
            if self.isobit not in range(1,6):
                raise RuntimeError("Isolation bit is '%i' must be between 1 and 5" % self.isobit)

        def xml(self, ind=1, step=2):
            s = ind * step * ' ' + '<Parametrization isobit="%i" offset="%i" slope="%i" mincut="%i" upperlimit="%i" etamin="%i" etamax="%i" priority="%i"/>\n' % \
                ( self.isobit, self.offset, self.slope, self.mincut, self.upperlimit, self.etamin, self.etamax, self.priority )
            return s

    class IsoGroup:
        def __init__(self):
            self.parameterizations = []

        def __iter__(self):
            for x in self.parameterizations:
                yield x

        def addIsolation(self, isobit, offset, slope, mincut=0, upperlimit=CaloLimits.ClusterOff, etamin=-49, etamax=49, priority=0):
            self.parameterizations += [ Isolation.Parametrization(isobit=isobit, offset=offset, slope=slope, mincut=mincut, upperlimit=upperlimit, etamin=etamin, etamax=etamax, priority=priority) ]
            return self


    def __init__(self):
        self.isolation = {'EMIsoForEMthr': Isolation.IsoGroup(), 'HAIsoForEMthr': Isolation.IsoGroup(), 'EMIsoForTAUthr': Isolation.IsoGroup()}

    def __getitem__(self,k):
        if not k in self.isolation:
            raise RuntimeError("Isolation group %s doesn't exist. Should be one of %s" % (k, ','.join(self.isolation.keys())) )
        return self.isolation[k]
        
    def xml(self, ind=1, step=2):
        s=""
        for thrtype in self.isolation.keys():
            s += ind * step * ' ' + '<Isolation thrtype="%s">\n' % thrtype
            for p in self.isolation[thrtype]:
                s += p.xml(ind+1,step)
            s += ind * step * ' ' + '</Isolation>\n'
        return s



class MinimumTOBPt(object):
    def __init__(self, thrtype, ptmin, window=8, etamin=-49, etamax=49, priority=0):
        self.thrtype    = str(thrtype).upper()
        self.ptmin      = int(ptmin)
        self.etamin     = int(etamin)
        self.etamax     = int(etamax)
        self.priority   = int(priority)
        self.window     = int(window)
        if not self.thrtype in [ "JETS", "JETL", "EM", "TAU"]:
            raise RuntimeError("Unknown type %s for Minimum TOB PT, must be JETS, JETL, EM, or TAU")
        

    def xml(self, ind=1, step=2):
        window = ' window="%i"' % self.window if self.thrtype.startswith("JET") else ""
        s = ind * step * ' ' + '<MinimumTOBPt thrtype="%s"%s ptmin="%i" etamin="%i" etamax="%i" priority="%i"/>\n' % \
             ( self.thrtype, window, self.ptmin, self.etamin, self.etamax, self.priority )
        return s

class CaloInfo:
            
    AverageOfThr = 1
    LowerThr = 2
    HigherThr = 3
    def __init__(self, name='standard', globalEmScale=2, globalJetScale=1, jetWeights=[0]*12):
        self.name        = name
        self.globalEmScale  = globalEmScale
        self.globalJetScale = globalJetScale
        self.jetWeights  = jetWeights
        self.xsParams    = { 'xsSigmaScale' : 0,
                             'xsSigmaOffset': 0,
                             'xeMin'        : 0,
                             'xeMax'        : 0,
                             'teSqrtMin'    : 0,
                             'teSqrtMax'    : 0 }
        self.isolation = Isolation()
        self.minTOBPt = []

    def setName(self, name):
        self.name = name

    def setGlobalEmScale(self, x):
        self.globalEmScale = x

    def setGlobalJetScale(self, x):
        self.globalJetScale = x

    def setJetWeights(self, weights):
        self.jetWeights = weights

    def setXsParams( self, **args ):
        for k in args:
            if not k in self.xsParams:
                raise RuntimeError, "'%s' is not a MET significance parameter" %  k
            self.xsParams[k]  = args[k]

    def xml(self, ind=1, step=2):
        s = ind * step * ' ' + '<CaloInfo name="CaloInfo" global_em_scale="%g" global_jet_scale="%g">\n' % (self.globalEmScale, self.globalJetScale)
        if oldStyle():
            for i, jw in enumerate(self.jetWeights):
                s += (ind+1) * step * ' ' + '<JetWeight num="%i" weight="%i">%i</JetWeight>\n' % (i+1, jw, jw)
        s += (ind+1) * step * ' ' + '<METSignificance xeMin="11" xeMax="63" teSqrtMin="4" teSqrtMax="63" xsSigmaScale="1150" xsSigmaOffset="1640"/>\n'
        if not oldStyle():
            s += self.isolation.xml(ind+1)
            for minObjPt in self.minTOBPt:
                s += minObjPt.xml(ind+1)

        s += ind * step * ' ' + '</CaloInfo>\n'
        return s



    @staticmethod
    def calcJetWeightsFromThresholds(triggerConfigLvl1, use_fj=False):

        def thr2weights(thrs, factor=1.25):
            # thrs = [10.0, 15.0, 1023.0, 20.0, 30.0, 50.0, 75.0, 350.0]

            # allow for thresholds being out of order
            # need to skip any turned-off thresholds (thr=1023)
            sthrs = sorted( filter(lambda x: x<1023, thrs)) + [0]
            
            # k = [(10.0, 15.0), (15.0, 20.0), (20.0, 30.0), (30.0, 50.0), (50.0, 75.0), (75.0, 350.0), (350.0, 0)]
            k = zip(sthrs,sthrs[1:])

            # compute weights
            # TODO: this formula description, copied from LVL1.py doesn't seem to match the implementation (need to check with L1Calo - Joerg
            # -------------------------------------------------------------------
            # Jet thresholds: [5, 10, 18, 23, 35, 42, 70, 100 ][10, 18, 26, 65 ]
            # Jet weights(w):  [ 7, 14, 20, 29, 38, 56, 85, 100 ][9, 6, 23, 19]
            # Jet weights(W):[ 5,  6, 6, 8, 9, 17, 29, 15 ][9, 6, 23, 19]
            # W_i=0.5*(thr_{i+1}-thr_{i-1}),
            #     thr_{0}=0 and thr_{n+1}=thr_{n}+(thr_{n}-thr_{n-1})
            # --------------------------------------------------------------------
            sum = 0
            weights = dict([(t,0) for t in thrs])
            for thr,thrNext in k:
                if thrNext != 0:
                    w = int( (thr + thrNext + 1)/2.0) - sum
                else:  
                    w = int( thr * factor - sum)
                weights[ thr ] = w
                sum += w

            return [ weights[thr] for thr in thrs ]

    
        l1_thrs = triggerConfigLvl1.menu.thresholds
        jet_thresholds  = l1_thrs.allThresholdsOf('JET')
        fjet_thresholds = l1_thrs.allThresholdsOf('JF')
        log.debug('N jet thresholds: %d (8 expected)' % len(jet_thresholds))
        log.debug('N fjet thresholds: %d (4 expected)' % len(fjet_thresholds))

        jthr, fjthr           = [1023]*8, [1023]*4
        jet_names, fjet_names = ['---']*8, ['---']*4

        for j in jet_thresholds:
            log.debug('jet threshold %s: %d' % (j.name, j.thresholdInGeV()))
            jthr[j.mapping] = j.thresholdInGeV()
            jet_names[j.mapping] = j.name

        for j in fjet_thresholds:
            tvalues = j.thresholdValues
            priority = 0
            threshold_value = 0
            for tv in tvalues:
                if tv.priority > priority:
                    threshold_value = tv.value
            log.debug('fjet threshold %s: %d' % (j.name, threshold_value))
            fjthr.append(threshold_value)
            fjthr[j.mapping] = threshold_value
            fjet_names[j.mapping] = j.name
        #

        log.debug('Jet thresholds: %s' % ' '.join(jet_names))
        log.debug('Fjet thresholds: %s' % ' '.join(fjet_names))

        if len(jthr) <= 8:
            w = thr2weights(jthr)
            jw = w
        else:
            jw = [0]*8

        w = 4 * [0]

        if use_fj and len(fjthr) <= 4:
            w = thr2weights(fjthr)
        jw.extend(w)
        return jw


