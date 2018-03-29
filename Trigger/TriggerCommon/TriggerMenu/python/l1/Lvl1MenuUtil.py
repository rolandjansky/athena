# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__all__ = ['log', 'idgen', 'binstr', 'get_smk_psk_Name', 'oldStyle']

from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerConfigL1")

# Old CTP = Old CTP
def oldStyle():
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    oldStylePrescales = 'pp_v4' in TriggerFlags.triggerMenuSetup() or \
                        'HI_v2' in TriggerFlags.triggerMenuSetup() or \
                        'alfa_v2' in TriggerFlags.triggerMenuSetup() or \
                        'DC14' in TriggerFlags.triggerMenuSetup()
    return oldStylePrescales



# unique id generator
class idgen:
    from collections import defaultdict
    a =  defaultdict(int)
    def get(self, kind):
        self.a[kind] += 1
        return str(self.a[kind])
    def reset(self, kind=None):
        if kind:
            self.a[kind] = 0
        else:
            for kind in self.a: self.a[kind] = 0
            
idgen = idgen()


# turns an integer into a binary representation string of given width ( for the reverse just use int('10011',2) )
def binstr(value, width):
    return ''.join([str( value >> i & 1 ) for i in range( width-1, -1, -1 ) ])

def get_smk_psk_Name(menuName):
    import re
    smk_psk_Name = {}
    if "mc_prescale" in menuName:
        form = "(.*)_(.*)_mc_prescale"
        m = re.match(form, menuName)
        (smkName, pskName) = m.groups()
        pskName = pskName+"_mc" 
    elif "prescale" in menuName:
        #eg lumi1e31_simpleL1Calib_no_prescale
        form = "(.*)_(.*)_prescale"
        m = re.match(form,menuName)
        (smkName, pskName) = m.groups()
    else:
        #eg lumi1e31 ps set name can be "default"
        smkName = menuName
        pskName = "default"
    smk_psk_Name["smkName"] = str(smkName)
    smk_psk_Name["pskName"] = str(smkName+"_"+pskName+"_prescale")

    return smk_psk_Name

def getJetWeights(triggerPythonConfig, use_fj=False):
    jw = []
    jthr, fjthr = [], []
    def thr2weights(thrs, factor=1.25):
        n = len(thrs)
        # need to skip any turned-off thresholds
        use = []
        for i in range(n):
            if thrs[i]<1023:
               use.append(1)
            else:
               use.append(0)
        # allow for thresholds being out of order
        index = order(thrs)
        # compute weights
        sum = 0
        weights = [0]*n
        for j in range(n):
            w = 0
            if use[index[j]]>0:
                jnext = j + 1
                while (jnext < n and use[index[jnext]]==0):
                    jnext = jnext + 1
                if (jnext < n):
                    w = int( (thrs[index[j]]+thrs[index[jnext]]+1)/2.0) - sum
                else:  
                    w = int(thrs[index[j]]*factor - sum)
            weights[index[j]]=w
            sum += w
        return weights
#
    def order(thrs): 
        n = len(thrs)
        thr1 = list(thrs)
        thr2 = sorted(thrs)
        index = []
        for i in range(n):
            val = thr2[i]
            pos = thr1.index(val)
            index.append(pos)
            if (thr1.count(val)>1):
                thr1[pos] = -thr1[pos]
        return index
#
    l1_thrs = triggerPythonConfig.Lvl1Thresholds()
    jet_thresholds = l1_thrs.allThresholdsOf('JET')
    fjet_thresholds = l1_thrs.allThresholdsOf('JF')
    log.debug('N jet thresholds: %d (8 expected)' % len(jet_thresholds))
    log.debug('N fjet thresholds: %d (4 expected)' % len(fjet_thresholds))
    jthr = [1023]*8
    fjthr = [1023]*4
    jet_names, fjet_names = ['---']*8, ['---']*4
    for j in jet_thresholds:
        log.debug('jet threshold %s: %d' % (j.name, j.thresholdInGeV()))
        # jthr.append(j.thresholdInGeV())
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
    s = ''
    for j in jet_names:
        s += '%s ' % j
    log.debug('Jet thresholds: %s' % s)
    s = ''
    for j in fjet_names:
        s += '%s ' % j
    log.debug('Fjet thresholds: %s' % s)
    if len(jthr) <= 8:
        w = thr2weights(jthr)
        jw = w
    else:
        jw = [0]*8
    w = [0]*4
    if use_fj:
        if len(fjthr) <= 4:
            w = thr2weights(fjthr)
    jw.extend(w)
    return jw
