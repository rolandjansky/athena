# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def readFileSimplified(filename):
    import os.path
    if not os.path.exists(filename): return None
    lines = []
    for l in open(filename):
        if '#' in l: l = l[0:l.index('#')] #Remove comments
        l=' '.join(l.split())#simplify whitespace
        if l: lines += [l]
    return lines

def filterEntry(parts):
    #parts is either [evt] or [run,evt]:
    assert len(parts) in [1,2]
    filter_run = int(parts[0]) if len(parts)==2 else None
    filter_evt = int(parts[1]) if len(parts)==2 else int(parts[0])
    assert (filter_run==None or filter_run>=0) and filter_evt>=0
    return (filter_run,filter_evt)

def readFiltersFromFile(filename):
    lines = readFileSimplified(filename)
    if lines==None: return None
    filts=[]
    for l in lines:
        filts+=[filterEntry(l.replace(':',' ').split())]
    return filts

def parseFilterString(s):
    import os.path
    excludemode=False
    if s.startswith('%'):
        excludemode=True
        s=s[1:]
    try:
        filters=[]
        tmp=s
        for filt in tmp.split(','):
            if os.path.exists(filt):
                #This is a file...
                for filefilt in readFiltersFromFile(filt):
                    filters += [filefilt]
                continue
            filters += [filterEntry(filt.split(':'))]
    except:
        filters=[]
        def vp1CfgErr(s): print "VP1 CONFIGURATION ERROR: %s" % s#fixme: should go in common utils module
        vp1CfgErr("Badly formatted filter string '%s'" % str(s))
        raise
    return (excludemode,filters)

def installEventFilter(filters):
    def vp1filter(run,evt):
        print "vp1filter(%i,%i) called" %(run,evt)
        for filter in filters[1]:
            if evt==filter[1] and (filter[0]==None or filter[0]==run): return not filters[0]
        return filters[0]
    from AthenaCommon.AlgSequence import AthSequencer
    seq = AthSequencer('AthMasterSeq')
    from GaudiSequencer.PyComps import PyEvtFilter
    seq += PyEvtFilter ('vp1filteralg')
    seq.vp1filteralg.filter_fct = vp1filter
    seq.vp1filteralg.evt_info = ''#To trigger keyless retrieve of the EventInfo
