# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import os
## Needed to correct ROOT behavior; see below
CWD = os.getcwd()

import sys
import pdb
import glob

import logging
logger = logging.getLogger('hancoolmod')
logger.addHandler(logging.NullHandler())

import ROOT
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem
os.chdir(CWD)

## Import the ROOT library for reading han results
gSystem.Load('libDataQualityUtils')
from ROOT import dqutils


## LumiBlock length (in minutes)
LBlength = 1.0

## Mapping han<->COOL
folderMap = {
    # ATLAS global
    #"/Global/DQShift": "ATLGL",
    #lar
    "/LAr/EMBC":"EMBC",
    "/LAr/EMBA":"EMBA",
    "/LAr/EMECA":"EMECA",
    "/LAr/EMECC":"EMECC",
    "/LAr/FCALA":"FCALA",
    "/LAr/FCALC":"FCALC",
    "/LAr/HECA" :"HECA",
    "/LAr/HECC" :"HECC",
    # Tile
    "/TileCal/Cell/AnyPhysTrig/LBA": "TILBA",
    "/TileCal/Cell/AnyPhysTrig/LBC": "TILBC",
    "/TileCal/Cell/AnyPhysTrig/EBA": "TIEBA",
    "/TileCal/Cell/AnyPhysTrig/EBC": "TIEBC",
    "/InnerDetector/IDAlignment/BeamSpot/NoTriggerSelection" : "IDBS",
    "/InnerDetector/IDAlignment/CombinedInDetTracks_NoTriggerSelection" : "IDAL",
    "/InnerDetector/IDAlignment_Perf" : "IDPF",
    "/InnerDetector/Global" : "IDGL",
    # CaloGlobal
    "/CaloMonitoring/CaloMonShift/CaloMonBAR" : "CALBA",
    "/CaloMonitoring/CaloMonShift/CaloMonECA" : "CALEA",
    "/CaloMonitoring/CaloMonShift/CaloMonECC" : "CALEC",
    # pixels
    "/InnerDetector/Pixel/DQMF/BLayer" :"PIX0",
    "/InnerDetector/Pixel/DQMF/Barrel" :"PIXB",
    "/InnerDetector/Pixel/DQMF/ECA" :"PIXEA",
    "/InnerDetector/Pixel/DQMF/ECC" :"PIXEC",
    # sct 
    "/InnerDetector/SCT/SCTB" :"SCTB",
    "/InnerDetector/SCT/SCTEA" :"SCTEA",
    "/InnerDetector/SCT/SCTEC" :"SCTEC",
    # trt
    "/InnerDetector/TRT/TRTB" :"TRTB",
    "/InnerDetector/TRT/TRTEA" :"TRTEA",
    "/InnerDetector/TRT/TRTEC" :"TRTEC",
    # rpc
    "/MuonDetectors/RPC/RPCBA" :"RPCBA",
    "/MuonDetectors/RPC/RPCBC" :"RPCBC",
    # mdt
    "/MuonDetectors/MDT/MDTBA" :"MDTBA",
    "/MuonDetectors/MDT/MDTBC" :"MDTBC",
    "/MuonDetectors/MDT/MDTEA" :"MDTEA",
    "/MuonDetectors/MDT/MDTEC" :"MDTEC",
    # tgc
    "/MuonDetectors/TGC/TGCEA" :"TGCEA",
    "/MuonDetectors/TGC/TGCEC" :"TGCEC",
    # csc
    "/MuonDetectors/CSC/CSCEA/Cluster" :"CSCEA",
    "/MuonDetectors/CSC/CSCEC/Cluster" :"CSCEC",
    # l1
    "/CentralTrigger/ErrorSummary": "L1CTP",
    "/L1Calo" : "L1CAL",
    # HLT
    "/HLT/TRCAL" : "TRCAL",
    "/HLT/TRBJT" : "TRBJT",
    "/HLT/TRBPH" : "TRBPH",
    "/HLT/TRELE" : "TRELE",
    "/HLT/TRGAM" : "TRGAM",
    "/HLT/TRJET" : "TRJET",
    "/HLT/TRMET" : "TRMET",
    "/HLT/TRMBI" : "TRMBI",
    "/HLT/TRMUO" : "TRMUO",
    "/HLT/TRTAU" : "TRTAU",
    "/HLT/TRIDT" : "TRIDT",
    # egamma
    "/egamma/EIDB" : "EIDB",
    "/egamma/EIDC" : "EIDCR",
    "/egamma/EIDE" : "EIDE",
    "/egamma/PHOB" : "PIDB",
    "/egamma/PHOC" : "PIDCR",
    "/egamma/PHOE" : "PIDE",
    # tau
    "/Tau/TAUB" : "TAUB",
    }

## Mapping Status<->Code
codeMap = {
    "Disabled": -1,
    "Undefined":0,
    "Red":1,
    "Yellow":2,
    "Green":3
    }

## Mapping different interval types
intervalType = {
    0 : "_minutes30_",
    1 : "_minutes10_",
    2 : "_medStat_",
    3 : "_lowStat_",
    4 : "ERROR"
    }

## Generates LB from filename
def getLimits( name ):
    try:
        import re, detmaskmod
        runNumber = re.match('run_(\d+)_.*han.root', name).group(1)
        max_hi_limit = detmaskmod.getNumLumiBlocks(int(runNumber))+1
        if (name.find('minutes10_')>-1):
            t=name.split('minutes10_')
            digit = float( ((t[len(t)-1]).split('_'))[0])
            low_limit = int((digit-1.0)*10.0/LBlength+1)-1
            hi_limit = int(digit*10.0/LBlength)
        elif (name.find('minutes30_')>-1):
            t=name.split('minutes30_')
            digit = float( ((t[len(t)-1]).split('_'))[0])
            low_limit = int((digit-1.0)*30.0/LBlength+1)-1
            hi_limit = int(digit*30.0/LBlength)
        elif 'lowStat_' in name:
            t=name.split('lowStat_LB')[-1]
            t=t.split('_han.root')[0]
            digits = t.split('-')
            low_limit = int(digits[0])
            hi_limit = min(int(digits[1])+1, max_hi_limit)
        elif 'medStat_' in name:
            t=name.split('medStat_LB')[-1]
            t=t.split('_han.root')[0]
            digits = t.split('-')
            low_limit = int(digits[0])
            hi_limit = min(int(digits[1])+1, max_hi_limit)
        else:
            low_limit = 1
            hi_limit = max_hi_limit
    except Exception, e:
        logging.warning('Could not determine limits because: %s', e)
        low_limit = 1
        hi_limit = 4294967295

    return (low_limit, hi_limit)

## Looks up the result in the HanOutputFile
def stringGetResult( file, rootFolder ):
    rootFolder =file+":"+rootFolder
    of = dqutils.HanOutputFile( file )
#    result = of.getStatus(rootFolder) #until DataQualityUtils-02-02-00
    result = of.getStringName(rootFolder)
    return result

#/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/tier0/FDR2/NoStream/run_3070_han.root

##path="/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/tier0/physics_HLT_Cosmic_AllTeIDSelected/"
#path="/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/tier0/FDR2/NoStream/"

##-------------------------------------------------------------

def hancool(runNumber=3070,
            filePath="/afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/tier0/FDR2/NoStream/",
            dbConnection="sqlite://;schema=MyCOOL.db;dbname=CONDBR2",dqmfOfl="/GLOBAL/DETSTATUS/DQMFOFL",
            db_tag="HEAD", shiftOfl="", shiftOfl_db_tag="HEAD", isESn=True):

    runlimitscache = {}
    
    logger.info('====> Running hancool_defects')
    hancool_defects(runNumber, filePath, dbConnection, 'HEAD', isESn)
    logger.info('<==== Done with hancool_defects')

# Nothing that follows applies for Run 2 ...
#    if ( filePath.rfind("/")!=(len(filePath)-1) ):
#        filePath+="/"
#
#    def nocomment_insert(mod, flag, code, comment):
#        mod.insert(flag, code, 0, 0, db_tag)
#
#    def comment_insert(mod, flag, code, comment):
#        mod.insert(flag, code, 0, 0, comment, db_tag)
#
#    def codelogic(since, until, flag, code, comment=None):
#        if comment is None:
#            inserter = nocomment_insert
#        else:
#            inserter = comment_insert
#        mod.setIOV(runNumber, since, runNumber, until)
#        wasModified = True
#        oldcode=mod.dumpCode(flag, db_tag)
#        if (oldcode == -1):
#            # this situation shouldn't happen
#            inserter(mod, flag, code, comment)
#        elif (code==3 and oldcode in (sys.maxint, 0)):
#            inserter(mod, flag, code, comment)
#        elif (code==2 and (oldcode in (sys.maxint, 0, 3))):
#            inserter(mod, flag, code, comment)
#        elif (code==0 and oldcode==sys.maxint):
#            inserter(mod, flag, code, comment)
#        elif ((code==1 or code==-1) and code != oldcode):
#            inserter(mod, flag, code, comment)
#        else:
#            wasModified = False
#        if wasModified:
#            print '%d-%d, %s --> %s' % (since, until, flag, code)
#        else:
#            print 'Not changed for %s' % flag
#
#    def filelogic(globname, filename, pair):
#        result=stringGetResult(globname, pair[0]+"/Results/Status")
#        code = codeMap[result]
#        if filename not in runlimitscache:
#            runlimitscache[filename] = getLimits(filename)
#        since, until = runlimitscache[filename]
#        print filename + ':',
#        codelogic(since, until, pair[1], code)
#        #print filename + ": " + pair[1]+" --> "+result+" ("+str(code)+")"
#        
#    ## pdb.set_trace()
#    mod = dqutils.StatusFlagCOOL(dbConnection,dqmfOfl,0,0,0,0)
#    filename="run_"+str(runNumber)+"_han.root"
#    runNumber = int(runNumber)
#    for pair in folderMap.iteritems():
#        i = 0
#        number = 0
#        while ((os.path.isfile(filePath+filename)) and (i<2)):
#            while os.path.isfile(filePath+filename):
#                ##print "--------------------------------------"
#                filelogic(filePath+filename, filename, pair)
#                ##print "in database: "+str(mod.dumpCode(pair[1]))
#                number = number+1
#                filename="run_"+str(runNumber)+intervalType[i]+str(number)+"_han.root"
#            number = 1
#            i = i+1
#            filename="run_"+str(runNumber)+intervalType[i]+str(number)+"_han.root"
#        for i in [2,3]:
#            for globname in glob.glob(os.path.join(filePath, 'run_%d%s*_han.root' % (runNumber, intervalType[i]))):
#                filename = os.path.basename(globname)
#                ##print "--------------------------------------"
#                filelogic(globname, filename, pair)
#                ##print "in database: "+str(mod.dumpCode(pair[1]))
#        filename="run_"+str(runNumber)+"_han.root"
#    ##print "--------------------------------------"
#
#    import detmaskmod
#    blacks = detmaskmod.decodeBlack(detmaskmod.getRunMask(runNumber))
#    for flag in blacks:
#        print 'Detector mask:',
#        codelogic(1, 4294967295, flag, -1)
#
#    if shiftOfl != '':
#        print 'Exporting black flags to SHIFTOFL'
#        db_tag = shiftOfl_db_tag
#        mod = dqutils.StatusFlagCommentCOOL(dbConnection,shiftOfl,0,0,0,0)
#        for flag in blacks:
#            print 'Detector mask:',
#            codelogic(1, 4294967295, flag, -1, 'Automatically filled by hancool')
#        
def detmask_defects(runNumber, ddb):
    import detmaskmod
    blacks = detmaskmod.decodeBlack(detmaskmod.getRunMask(runNumber),
                                    defects=True)
    nlbs = detmaskmod.getNumLumiBlocks(runNumber)
    for defect in blacks:
        ddb.insert(defect, since=(runNumber << 32 | 1),
                   until=(runNumber << 32 | (nlbs+1)),
                   comment='Automatically added by hancool',
                   added_by='sys:hancool')

import collections
defect_val = collections.namedtuple('defect_val',
                                    'defect, comment, recoverable')
defect_iov = collections.namedtuple('defect_iov',
                                    'defect, comment, recoverable, since, until')

def ctp_defects(d, i, runNumber):
    mapping = { 1: 'TRIG_L1_CTP_CTP_ROD_bcid',
                2: 'TRIG_L1_CTP_bcid',
                3: 'TRIG_L1_CTP_CTP_MuCTPI_bcid',
                4: 'TRIG_L1_CTP_candnumber',
                5: 'TRIG_L1_CTP_multpt',
                6: 'TRIG_L1_CTP_roiNum',
                7: 'TRIG_L1_CTP_roiCand',
                8: 'TRIG_L1_CTP_bcidrange',
                9: 'TRIG_L1_CTP_lumiblockrange',
                10: 'TRIG_L1_CTP_lumiblocktime',
                11: 'TRIG_L1_CTP_nanosectime',
                12: 'TRIG_L1_CTP_TAPnoTBP',
                13: 'TRIG_L1_CTP_TAVnoTAP',
                14: 'TRIG_L1_CTP_CTPsim',
                15: 'TRIG_L1_CTP_incompletefragment',
                #TODO: add missing-orbit bin here, eventually (i.e. 15: 'TRIG_L1_CTP_missingorbit')
                }

    rv = []
    when = d.Get('CentralTrigger/ErrorSummary/errorSummaryPerLumiBlock')
    if not when:
        return None
    bad_lbs = {}
    overflow_bad_lbs = {}
    for key in mapping:
        bad_lbs[key] = []

    # loop over error bin numbers defined in dict above
    for errorBin in mapping:
        bad_lbs[errorBin] = [bin for bin in xrange(1, when.GetNbinsX()+1) if when.GetBinContent(bin, errorBin) > 0] # fix this line, slicing in Y? is that it?
        overflow_bad_lbs[errorBin] = (when.GetBinContent(when.GetNbinsX()+1, errorBin) > 0)
        
    message = 'Automatically set'
    
    for defect in mapping:
        for lb in bad_lbs[defect]:
            rv.append(defect_iov(mapping[defect], message, False, lb, lb+1))
        if overflow_bad_lbs[defect]:
            message += '; defect occurred past end of monitoring histogram, marking end of run as bad'
            import detmaskmod # ugly: could happen for more than one defect - should be cheap though
            nlbs = detmaskmod.getNumLumiBlocks(runNumber)
            rv.append(defect_iov(defect, message, False, when.GetNbinsX(), nlbs+1))
    #print "The following defects were extracted: " # TODO: remove this line?
    #print rv # TODO: remove this line?
    return rv

def sct_lowstat_defect(d, i, runNumber):
    histogram = d.Get('InnerDetector/SCT/Summary/tracksPerRegion')
    if not histogram:
        return None
    if histogram.GetEntries() < 200:
        return [defect_val('SCT_GLOBAL_LOWSTAT', 'Low statistics', False)]
    else:
        return []

def sct_conf_defects(d, i, runNumber):
    def sct_conf_defects_core(d, i, runNumber, histname, mapping):
        rv = []
        histogram = d.Get(histname)
        if not histogram: 
            return None
        for bin in mapping:
            threshold = 40
            if mapping[bin] == 'SCT_MOD_OUT_GT40':
                threshold = 80
            if histogram.GetBinContent(bin) > threshold:
                rv.append(defect_val(mapping[bin], '%.1d modules affected' % histogram.GetBinContent(bin), False))
        return rv

    rv1 = sct_conf_defects_core(d, i, runNumber, 'InnerDetector/SCT/Summary/SCTConfOutM',
                                { 1: 'SCT_MOD_OUT_GT40' })
    rv2 = sct_conf_defects_core(d, i, runNumber, 'InnerDetector/SCT/Summary/SCTConfNew',
                                { 3: 'SCT_MOD_ERR_GT40',
                                  5: 'SCT_MOD_NOISE_GT40' })
    if rv1 is None and rv2 is None:
        return None
    else:
        return (rv1 if rv1 is not None else [])+(rv2 if rv2 is not None else [])

def sct_perlb_defects(d, i, runNumber):
    pairs = [('InnerDetector/SCT/Summary/SCT_LinksWithLinkLevelErrorsVsLbs', 
              'SCT_PERIOD_ERR_GT40', lambda _: _ > 80),
             ('InnerDetector/SCT/Summary/SCT_LinksWithRODLevelErrorsVsLbs', 
              'SCT_ROD_OUT', lambda _: _ >= 1)]

    rv = []
    bad_lbs = {}
    overflow_bad_lbs = {}
    message = 'Automatically set'
    foundany = False

    for hname, dname, policy in pairs:
        when = d.Get(hname)
        if not when:
            continue
        foundany = True

    # extract bad bins
        bad_lbs[dname] = [bin for bin in xrange(1, when.GetNbinsX()+1) if policy(when.GetBinContent(bin))]
        overflow_bad_lbs[dname] = policy(when.GetBinContent(when.GetNbinsX()+1))
        
        for lb in bad_lbs[dname]:
            rv.append(defect_iov(dname, message, False, lb, lb+1))
        if overflow_bad_lbs[dname]:
            message += '; defect occurred past end of monitoring histogram, marking end of run as bad'
            import detmaskmod # ugly: could happen for more than one defect - should be cheap though
            nlbs = detmaskmod.getNumLumiBlocks(runNumber)
            rv.append(defect_iov(dname, message, False, when.GetNbinsX(), nlbs+1))
    if foundany:
        return rv
    else:
        return None

def iovs_merge(l):
    l.sort(key=lambda x: x.since)
    rl = []
    i = 0
    previous = None; until = -1
    while i < len(l):
        if not previous:
            previous = l[i]
            until = previous.until
        else:
            if l[i].since != until or previous.comment != l[i].comment:
                # we have no more to merge for this range
                rl.append(previous._replace(until=until))
                previous = l[i]; until = previous.until
            else:
                until = l[i].until
        i += 1
    if previous:
        rl.append(previous._replace(until=until))
    return rl

def sct_eff_defect(d, i, runNumber):
    h1 = d.Get('InnerDetector/SCT/Summary/SctTotalEffBCID_/Results/Status')
    h2 = d.Get('InnerDetector/SCT/Summary/SctTotalEff_/Results/Status')
    if not h1 or not h2: return None
    badstatuses = set(['Yellow', 'Red'])
    statuscheck = []
    for h in h1, h2:
        status = set(x.GetName() for x in h.GetListOfKeys())
        if len(badstatuses & status) > 0:
            assert len(status) == 1, 'Status must be length one or the file is corrupt'
            statuscheck.append(True)
        else:
            statuscheck.append(False)
    if all(statuscheck):
        return [defect_val('SCT_EFF_LT99', 'Automatically set for whole run', False)]
    else:
        return []

def dqmf_node_defect(node, defect, badstatuses=['Red']):
    badstatuses = set(badstatuses)
    def dqmf_node_defect_core(d, i, runNumber):
        filenode = d.Get(node + '_/Results/Status')
        if not filenode: return None
        status = set(x.GetName() for x in filenode.GetListOfKeys())
        if len(badstatuses & status) > 0:
            assert len(status) == 1, 'Status must be length one or the file is corrupt'
            return [defect_val(defect, node + ' DQMF color ' + status.pop(), False)]
        else:
            return []
    return dqmf_node_defect_core

def hancool_defects(runNumber, filePath="./", dbConnection="", db_tag='HEAD', isESn=True):
    import pix_defect
    analyzers = []
    if isESn:
        # CTP
        analyzers += [ctp_defects]
        # SCT
        analyzers += [sct_eff_defect,
                      sct_lowstat_defect,
                      sct_conf_defects,
                      sct_perlb_defects,
                      ]

    if ( len(filePath) == 0 or filePath[-1] != '/'):
        filePath+="/"
    if (len(dbConnection)<1): dbConnection = "/afs/cern.ch/user/a/atlasdqm/dqmdisk1/cherrypy-devel/defectstest.db/COMP200"

    import ROOT
    # Conflict logic: shorter intervals override longer ones
    defects_by_function = {}

    # empty list for missing high stat, reserved for future use

    fnames = ([ [filePath+"run_"+str(runNumber)+"_han.root"], [] ]
              + [glob.glob(os.path.join(filePath, 'run_%s%s*_han.root' % (runNumber, intervalType[i]))) for i in [2, 3]])

    for i, itype in enumerate(fnames):
        ldefects_by_function = {}
        for globname in itype:
            filename = os.path.basename(globname)
            ##print "--------------------------------------"
            since, until = getLimits(filename)
            default_iov = defect_iov(*([0]*5))
            default_iov = default_iov._replace(since=since, until=until)
            #print filename + ':', since, until
            fobj = ROOT.TFile.Open(globname)
            ldefects = None
            for func in analyzers:
                 rv = func(fobj, i, runNumber)
                 if rv is not None:
                     rvt = [default_iov._replace(**(i._asdict())) for i in rv]
                     if func not in ldefects_by_function:
                         ldefects_by_function[func] = rvt
                     else:
                         ldefects_by_function[func] += rvt
        defects_by_function.update(ldefects_by_function)
    defects = sum(defects_by_function.values(), [])

    if isESn:
        globname = fnames[0][0]
        filename = os.path.basename(globname)    
        since, until = getLimits(filename)
        try:
            defects += pix_defect.execute(runNumber, globname, until-1)
        except Exception, e:
            logging.warning('Unable to execute pixel hancool code')
            logging.warning('--> %s: %s' % (type(e).__name__, e))

    from DQDefects import DefectsDB
    ddb = DefectsDB(dbConnection, read_only=False)
    if isESn:
        logging.info('Running detmask_defects')
        detmask_defects(runNumber, ddb)
    with ddb.storage_buffer:
        for defect in iovs_merge(defects):
            logger.debug('Uploading %s', defect)
            ddb.insert(defect.defect, since=(runNumber << 32 | defect.since),
                       until=(runNumber << 32 | defect.until),
                       comment=defect.comment,
                       recoverable=defect.recoverable,
                       added_by='sys:hancool')


    # Assign Pixel defect
    #globname_pix = filePath+"run_"+str(runNumber)+"_han.root"
    #filename_pix = "run_"+str(runNumber)+"_han.root"
    #from DQDefects import DefectsDB
    #ddb = DefectsDB(dbConnection, read_only=False, tag="HEAD")
    #import pix_defect
    #pix_defect.execute(runNumber, globname_pix, until-1, 'sys:hancool', ddb)
