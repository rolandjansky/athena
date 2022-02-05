# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import sys
from CoolRunQuery.utils.AtlRunQueryTimer import timer
from CoolRunQuery.utils.AtlRunQueryUtils import GetRanges

from CoolRunQuery.selector.AtlRunQuerySelectorBase import Selector, RunLBBasedCondition, TimeBasedCondition

class LHCSelector(Selector):
    def __init__(self, name, lhc=[], addArg=''):
        super(LHCSelector,self).__init__(name)
        self.condition = LHCCondition( 'lhc', lhc, addArg = addArg )
        self.showarg=addArg
        if not lhc:
            self.condition.applySelection = False

    def __str__(self):
        return str(self.condition)

    def addShowSelector(self, addArg=''):
        self.showarg=addArg
        
    def setShowOutput(self):
        self.condition.setShowOutput(self.showarg)
        
    def select(self, runlist):
        print (self, end='')
        sys.stdout.flush()
        runlist = self.condition.select(runlist)
        return runlist

class LHCCondition(TimeBasedCondition):
    def __init__(self, name, condition=[], channel=0, addArg=''):
        ck = [(1,'lhc:fillnumber',  'FillNumber'),
              (1,'lhc:stablebeams', 'StableBeams'),
              (1,'lhc:beamenergy',  'BeamEnergyGeV')]
        if addArg == 'all':
            ck += [(1,'lhc:beammode',    'BeamMode')]
        elif addArg == 'min':
            ck = [(1,'lhc:stablebeams', 'StableBeams')]
        super(LHCCondition,self).__init__(name=name,
                                          dbfolderkey='COOLOFL_DCS::/LHC/DCS/FILLSTATE',
                                          channelKeys = ck)
    
        # define arguments (after initialising base class!)
        self.lhc = {}
        for c in condition:
            # format: 'fillnumber 891' or 'stablebeams 1', etc
            lhcargs = c.split()
            if len(lhcargs) == 2:
                key = 'lhc:' + lhcargs[0].strip().lower()
                # does it exist?
                if key not in self.ResultKey():
                    print ('ERROR: unknown LHC variable "%s"' % key)
                    sys.exit(1)                

                cond = lhcargs[1].strip()
                if cond:
                    try:
                        float(cond[0])
                        self.lhc[key] = GetRanges( cond )
                    except ValueError:
                        self.lhc[key] = [[cond]]
                        pass
            else:
                print ('ERROR: unknown condition format for LHC: "%s" -> need two arguments separated by blank' % lhcargs)
                sys.exit(1)                                    

    def setShowOutput(self, addArg ):
        ck = ['lhc:fillnumber', 'lhc:stablebeams', 'lhc:beamenergy']
        if addArg == 'all':
            ck += ['lhc:beammode']
        elif addArg == 'min':
            ck = ['lhc:stablebeams']
        super(LHCCondition,self).setShowOutput()
                    
    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking if LHC fill state information %s" % self.lhc
        else:
            return "Retrieving LHC fill state information"

    def passes(self, value, k):
        if k not in self.lhc:
            return True
        if 'n.a.' in value:
            return True
        try:
            v = float(value)
            # special treatment for E-beam = 7864 --> value for ASYNC
            if v >= 7864:
                return False
            
            # iterate over conditions
            for cr in self.lhc[k]:
                if v >= cr[0] and v <= cr[1]:
                    return True            
        except ValueError:
            # value is a string
            # note that it still can happen that the reference of the string is a number. For example, the
            # beam energy can be returned as "ASYNC", when the two beams were in asynchronous mode. Need
            # to cover such a case.
            try:
                float(self.lhc[k][0][0])
                return False # comparing number to string -> don't pass
            except ValueError:
                if value == self.lhc[k][0][0]:
                    return True
        return False
    
class OLCFillParamsCondition(TimeBasedCondition):
    def __init__(self, name, condition=[], channel=0):
        super(OLCFillParamsCondition,self).__init__(name=name,
                                                    dbfolderkey='COOLONL_TDAQ::/TDAQ/OLC/LHC/FILLPARAMS',
                                                    channelKeys = [(0,'olc:beam1bunches', 'Beam1Bunches'),
                                                                   (0,'olc:beam2bunches', 'Beam2Bunches'),
                                                                   (0,'olc:collbunches',  'LuminousBunches'),
                                                                   (0,'olc:bcidmask',     'BCIDmasks')])
        self.onl = {}

    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking if online lumi is %s" % self.olc
        else:
            return "Retrieving online lumi information /TDAQ/OLC/LHC/FILLPARAMS"

    def passes(self,values,k):
        if k not in self.onl:
            return True
        if 'n.a.' in values:
            return True
        if values ==  self.onl[k]:
            return True
        return False
    
    def runAfterQuery(self,runlist):

        for k in self.ResultKey():
            with timer("olc afterquery blocks prepare for: %s" % k):
                for run in runlist:
                    run.stats[k] = {}
                    if k not in run.data:
                        continue
                    blocks = []
                    for entry in run.data[k]:
                        v = entry.value
                        if k!='olc:bcidmask':
                            v = int(v)
                        if len(blocks) > 0 and blocks[-1][0]==v and blocks[-1][2]==entry.startlb:
                            blocks[-1][2] = entry.endlb
                        else:
                            blocks += [ [v, entry.startlb, entry.endlb] ]
                    run.stats[k] = { "blocks" : blocks, "first" : run.data[k][0].value }

        #f = open("olc.pickle", "w")
        #cPickle.dump([run.data,run.stats],f)
        #f.close()
        
        with timer("olc afterquery rest"):
            from CoolRunQuery.utils.AtlRunQueryUtils import unpackRun1BCIDMask, unpackRun2BCIDMask
            for run in runlist:

                if run.runNr < 151260:
                    continue # OLC information was not in COOL before this run

                # these contain the number of bunches for each IOV [(nb,lbstart,lbend)(...)...]
                xb1 = run.stats['olc:beam1bunches']['blocks']
                xb2 = run.stats['olc:beam2bunches']['blocks']
                xbc = run.stats['olc:collbunches']['blocks']
                
                # loop over BCID mask
                bcidmask = run.stats['olc:bcidmask']['blocks']
                for i in range(len(bcidmask)):
                    (bcidblob,lbstart,lbend) = bcidmask[i]
                    
                    # number of bunches
                    for nb1, b, e in xb1:
                        if lbstart>=b and lbstart<e:
                            break
                    for nb2, b, e in xb2:
                        if lbstart>=b and lbstart<e:
                            break
                    for nbc, b, e in xbc:
                        if lbstart>=b and lbstart<e:
                            break


                    bcidBlobLength = len(bcidblob)
                    if bcidBlobLength == 3564:
                        # run 2
                        beam1, beam2, beam12 = unpackRun2BCIDMask(bcidblob)
                        #print ("BC beam 1: %i, beam 2: %i, coll: %i" % (len(beam1), len(beam2), len(beam12)))
                    else:
                        # unpack BCID mask
                        if len(bcidblob) == 2 * (nb1 + nb2 + nbc):
                            beam1, beam2, beam12 = unpackRun1BCIDMask( bcidblob, nb1, nb2, nbc )
                            #print ("BC beam 1: %i, beam 2: %i, coll: %i" % (len(beam1), len(beam2), len(beam12)))
                        else:
                            print ("WARNING, bcidMask inconsistent",nb1, nb2, nbc,"should add up to half of",len(bcidblob))
                            beam1, beam2, beam12 = ([],[],[])

                    # refill run stats
                    bcidmask[i] = ((nb1, nb2, nbc), (beam1, beam2, beam12), lbstart, lbend)

class OLCLBDataCondition(TimeBasedCondition):
    def __init__(self, name, condition=[]):
        super(OLCLBDataCondition,self).__init__(name=name,
                                                dbfolderkey='COOLONL_TDAQ::/TDAQ/OLC/LHC/LBDATA',
                                                channelKeys = [(1,'olc:beam1intensity', 'Beam1Intensity'),
                                                               (1,'olc:beam2intensity', 'Beam2Intensity')])
                                                               #(1,'olc:beam1intensitybct', 'Beam1IntensityAll'),
                                                               #(1,'olc:beam2intensitybct', 'Beam2IntensityAll')])
                                                               #(0,'olc:invlivetime', 'Beam1InvLifetime')])
                                                               #(0,'olc:invlivetime', 'Beam2InvLifetime')])
        self.onl = {}

    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking if online lumi is %s" % self.olc
        else:
            return "Retrieving online lumi information /TDAQ/OLC/LHC/LBDATA"

    def passes(self,values,k):
        if k not in self.onl:
            return True
        if 'n.a.' in values:
            return True
        if values == self.onl[k]:
            return True
        return False
    
    def runAfterQuery(self,runlist):
        for k in self.ResultKey():
            for run in runlist:
                blocks = []
                first = 0
                if len(run.data[k])>0:
                    for entry in run.data[k]:
                        v = entry.value
                        if len(blocks) > 0 and blocks[-1][0]==v and blocks[-1][2]==entry.startlb:
                            blocks[-1][2] = entry.endlb
                        else:
                            blocks += [ [v, entry.startlb, entry.endlb] ]
                    first = run.data[k][0].value
                run.stats[k] = { "blocks" : blocks, "first" : first }

# moved from LUMINOSITY timestamp-based to LB-based folder
#... class OLCLumiCondition(TimeBasedCondition):
class OLCLumiCondition(RunLBBasedCondition):
    def __init__(self, name, condition=[], channel=0):

        self.condition = condition
        name, sep, channel = name.partition(' ')
        try:
            channel = int(channel)
        except ValueError:
            channel = 0
            pass

        # the key
        # interpret condition: reference luminosity: ub-1, default: ub-1
        # format: 1ub, 100nb, 1mb+, 1pb-, etc
        units   = [ 'mb', 'ub', 'nb', 'pb', 'fb', 'ab', 'b' ]
        factoub = [ 1e-3, 1,   1e3,  1e6,  1e9,  1e12, 1e-6 ]
        self.complumi = -1
        self.compsign = +1
        f             = 1
        if self.condition:
            c = self.condition[0].strip()
            for iu,u in enumerate(units):
                if u in c:
                    f = factoub[iu]
                    c = c.replace(u,'')
                    break
            try:
                if '+' in c:
                    c = c.replace('+','').strip()
                    self.complumi = float(c)
                elif '-' in c:
                    self.compsign = -1
                    c = c.replace('-','').strip()
                    self.complumi = float(c)
            except ValueError:
                print ("ERROR: in 'olc' condition: %s" % self.condition)
                sys.exit(1)
        self.complumi *= f # in ub-1

        super(OLCLumiCondition,self).__init__(name=name,
                                              dbfolderkey=( 'COOLONL_TRIGGER::/TRIGGER/LUMI/OnlPrefLumi' if self.isRun2() else 'COOLONL_TRIGGER::/TRIGGER/LUMI/LBLESTONL'),
                                              channelKeys = [(channel,'olc:lumi:%i' % (channel), 'LBAvInstLumi')])
        # -----------------------------------------------------------------------------------------------------------
        # old, timestamp based folder
        # super(OLCLumiCondition,self).__init__(name=name,
        #                                       dbfolderkey='COOLONL_TDAQ::/TDAQ/OLC/LUMINOSITY',
        #                                       channelKeys = [(channel,'olc:lumi:%i' % channel, 'LBAvInstLumPhys')])
        # -----------------------------------------------------------------------------------------------------------
        self.olc = {}
                    
    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking if online lumi is %s" % self.condition
        else:
            return "Retrieving online lumi information /TRIGGER/LUMI/LBLESTONL"

    def passes(self,values,k):
        return True
        if k not in self.olc:
            return True
        if 'n.a.' in values:
            return True

        if values == self.olc[k]:
            return True
        return False

    def runAfterQuery(self, runlist):

        # correct "old" runs for uncalibrated luminosity
        k = self.ResultKey()[0]  # lumi
        kst = 'lhc:stablebeams'
        for ir,run in enumerate(runlist):
            for entry in run.data[k]:
                if entry.startlb == 0:
                    continue
                if entry.value != 'n.a.':
                    if run.runNr <= 158632:
                        entry.value *= 1.13

        if not self.condition:
            return
        
        # compute integrated luminosity
        # given is the delivered luminosity in units of 10^30 cm-2 = 1 ub-1
        # insert events per LB
        vetolist = []
        for ir,run in enumerate(runlist):
            yvecInt = [] # olc per lb in 10^27 cm-2s-1
            xvecStb = []
            for entry in run.data[k]:
                assert entry.startlb != 0, 'entry should not start at LB=0'
                val = 0
                if entry.value != 'n.a.':
                    val = max(0,entry.value) * 1.e3 # unit of val was: 10^30 cm-2 s-1, transform to: 10^27 cm-2s-1
                lbs = range(entry.startlb,entry.endlb)
                yvecInt += len(lbs)*[val]
                    
                # check LBs with stable beam
                print (run.data[kst].atLB(entry.startlb))
                stable_beam = run.data[kst].atLB(entry.startlb)[0].value
                if 'true' in stable_beam.lower():
                    xvecStb += lbs
                            
            # correct for time span
            intlumi    = 0
            for idx,(lbtime,lbendtime) in enumerate(run.lbtimes):
                lb=idx+1                    
                if lb in xvecStb:
                    # unit of yvec was: 10^27 cm-2 s-1 -> 10^30 cm-2 = ub-1 
                    yvecInt[idx] *= (float(lbendtime)-float(lbtime))/1.E9/1000. 
                    intlumi      += yvecInt[idx]            

            if not (intlumi > self.compsign*self.complumi or -1*intlumi > self.compsign*self.complumi):
                vetolist.append(ir)

        # remove vetoed runs
        ic = 0
        vetolist.sort()
        for ir in vetolist:
            del runlist[ir-ic]
            ic += 1
    
    def prettyValue(self, value, key):
        if value=='n.a.':
            return value
        return float(value) # unit is ub-1 s-1
    
class OLCLumiSelector(Selector):
    def __init__(self, name, olclumi=[]):
        super(OLCLumiSelector,self).__init__(name)
        self.condition = OLCLumiCondition( 'olclumi', olclumi )
        if not olclumi:
            self.condition.applySelection = False

    def setShowOutput(self):
        self.condition.setShowOutput()
        
    def runAfterQuery(self, runlist):
        self.condition.runAfterQuery( runlist )

    def select(self, runlist):
        runlist = self.condition.select(runlist)
        return runlist

class LuminositySelector(RunLBBasedCondition):
    def __init__(self, name, lumi=None):
        
        name, tag = (name.split(None,1) + [""])[:2]

        channel, condtag = self.__interpretTag(tag)

        if self.isRun2():
            self._dbfolderkey='COOLOFL_TRIGGER::/TRIGGER/OFLLUMI/OflPrefLumi'
        else:
            self._dbfolderkey='COOLOFL_TRIGGER::/TRIGGER/OFLLUMI/LBLESTOFL'
        if condtag:
            self._dbfolderkey += "#" + condtag

        self._channelKeys = [(channel,'ofllumi:%i:%s' % (channel,condtag), 'LBAvInstLumi')]

        super(LuminositySelector,self).__init__(name=name,
                                                dbfolderkey = self._dbfolderkey,
                                                channelKeys = self._channelKeys)

        if lumi:
            self.cutRange = GetRanges(lumi)
            self.applySelection = True

    def addShowTag(self, tag):
        channel, condtag = self.__interpretTag(tag)
        if self.isRun2():
            self._dbfolderkey='COOLOFL_TRIGGER::/TRIGGER/OFLLUMI/OflPrefLumi'
        else:
            self._dbfolderkey='COOLOFL_TRIGGER::/TRIGGER/OFLLUMI/LBLESTOFL'
        if condtag:
            self._dbfolderkey += "#" + condtag
        self._channelKeys = [(channel,'ofllumi:%i:%s' % (channel,condtag), 'LBAvInstLumi')]

    def initialize(self):
        print ("Setting channelkeys",self._channelKeys)
        self.setSchemaFolderTag(self._dbfolderkey)
        self.setChannelKeys(self._channelKeys)

    def __interpretTag(self, tag):
        # default settings
        channel = 0  # 'ATLAS_PREFERRED' algorithm
        condtag = None
        
        tt = tag.split()
        if len(tt) == 1:
            # format: "luminosity <channel_number>" OR "luminosity <COOL tag>"
            try:
                channel = int(tt[0])
            except ValueError:
                condtag = tt[0]
        elif len(tt) == 2: 
            # format: "luminosity <channel_number> <COOL tag>"
            try:
                channel = int(tt[0])
            except ValueError:
                channel = 0
            condtag = tt[1]

        if condtag is not None:
            print ("Using channel %i and conditions tag %s" % (channel, condtag))
            return channel, condtag

        if self.isRun2():
            import sys
            from PyCool import cool
            sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
            try:
                from AtlCoolBKLib import resolveAlias

                cur = resolveAlias.getCurrent()
                # cur = resolveAlias.getNext()
                dbSvc = cool.DatabaseSvcFactory.databaseService()
                
                db = dbSvc.openDatabase('oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TRIGGER;dbname=CONDBR2',False)
                
                fld = db.getFolder('/TRIGGER/OFLLUMI/OflPrefLumi')
                
                updLumiTag = fld.resolveTag(cur.replace('*','ST'))
            
                condtag = updLumiTag
            except ImportError as ex:
                print ("WARNING: ImportError, can not read conditions tag (likely an afs permission issue): ",ex)
                condtag = "OflPrefLumi-RUN2-UPD4-10"
            except SyntaxError as ex:
                print ("WARNING: SyntaxError, can not read conditions tag (need to understand where the py3 code is located): ",ex)
                condtag = "OflPrefLumi-RUN2-UPD4-10"
        else:
            condtag = "OflLumi-UPD2-006"
            
        print ("Using channel %i and conditions tag %s" % (channel, condtag))
        return channel, condtag


    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking if number of events matches %r" % self.cutRange
        else:
            return "Retrieving lumi numbers"

    def passes(self,values,key):
        try:
            val = int(values)
        except ValueError: # if n.a.
            self.selDataMissing = True
            return True
        for cr in self.cutRange:
            if val>=cr[0] and val<=cr[1]:
                return True
        return False

class BeamspotSelector(RunLBBasedCondition):
    def __init__(self, name, bs=None, args=""):
        self.bs = bs
        args = args.split()
        
        # defaults
        folder       = 'COOLOFL_INDET::/Indet/Beampos'
        # self.condtag = 'IndetBeampos-ES1-UPD2'
        self.condtag = 'IndetBeampos-ES1-UPD2-02' # new default tag (operational since rel 17, run 188902, 7 Sep)
        self.isOffline = True

        if args: # more information: online and/or COOL tag
            # check if online
            if args[0].lower().startswith('onl'):
                # online format: "online MyTag" or "online"
                self.isOffline = False
                folder = 'COOLONL_INDET::/Indet/Onl/Beampos'
                self.condtag = 'IndetBeamposOnl-HLT-UPD1-001-00' # default tag
                if len(args)>1:
                    if args[1]=='live':
                        self.condtag = 'IndetBeamposOnl-LiveMon-001-00'
                    else:
                        self.condtag = args[1]
            else:
                # assume second entry is COOL tag
                self.condtag = args[0]
            
        super(BeamspotSelector,self).__init__(name        = name,
                                              dbfolderkey = folder,
                                              channelKeys = [
                                                  (0,'bs:Status','status'),
                                                  (0,'bs:Pos-X', ('posX','posXErr')),
                                                  (0,'bs:Pos-Y', ('posY','posYErr')),
                                                  (0,'bs:Pos-Z', ('posZ','posZErr')),
                                                  (0,'bs:Sig-X', ('sigmaX','sigmaXErr')),
                                                  (0,'bs:Sig-Y', ('sigmaY','sigmaYErr')),
                                                  (0,'bs:Sig-Z', ('sigmaZ','sigmaZErr')),
                                                  (0,'bs:Sig-XY',('sigmaXY','sigmaXYErr')),
                                                  (0,'bs:Tilt-X',('tiltX','tiltXErr')),
                                                  (0,'bs:Tilt-Y',('tiltY','tiltYErr'))
                                                  ])

        if bs:
            self.cutRange = GetRanges(bs)

    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking if %s beamspot information matches %s" % ("offline" if self.isOffline else "online", self.bs)
        else:
            return "Retrieving %s beamspot information" % ("offline" if self.isOffline else "online",)

    def prettyValue(self, value, key):
        if type(value)==tuple:
            return tuple(map(float,value))
        return float(value)


    def passes(self,values,key):
        try:
            val = int(values)
        except ValueError: # if n.a.
            self.selDataMissing = True
            return True
        for cr in self.cutRange:
            if val>=cr[0] and val<=cr[1]:
                return True
        return False

    def runAfterQuery(self,runlist):
        whatitis = 'offline' if self.isOffline else 'online'
        from CoolRunQuery.AtlRunQueryRun import Run
        Run.BeamspotSource = '%s, COOL tag: %s' % (whatitis, self.condtag)
        for run in runlist:
            run.stats['Beamspot'] = whatitis

if __name__ == "__main__":

    from CoolRunQuery.selector.AtlRunQuerySelectorBase import Selector
    from CoolRunQuery.selector.AtlRunQuerySelectorRuntime import RunTimeSelector

    runNumbers =  "251103,251106,251363,251367,251371,251663,251666,251667,251669,266904,"
    runNumbers += "266919,267073,251863,251869,251873,251876,251880,252009,252044,252072,"
    runNumbers += "252099,252115,267148,267152,267162,252179,252186,252194,252198,252207,"
    runNumbers += "252220,252222,252223,252226,252233,252376,252380,252390,267167,252402,"
    runNumbers += "252404,252589,252604,252608,252662,252663,252838,252840,252844,252854,"
    runNumbers += "253009,253010,253014,267638,267639"

    #runNumbers = "251103,251106"

    Selector._conddb = "CONDBR2" # set Run 2 for entire query

    print(Selector.condDB())


    sel = LuminositySelector(name = 'lumiSelector 0 OflPrefLumi-RUN2-UPD4-10')
    sel.applySelection = False

    # the run selector
    rtSel = RunTimeSelector(name = 'runtime', runlist = runNumbers.split(","))

    # find the begin and end of each interesting run
    runlist = rtSel.select()

    from CoolRunQuery.AtlRunQuerySelectorWorker import SelectorWorker

    SelectorWorker.addSelector(selector=sel, priority=1)
    sd = SelectorWorker.findSelectorDescriptor(sel.name)
    sd.doesSelect = False

    selectionOutput = []
    # Selectors can implement an initialize function which runs after any constructor and setShow 
    for sd in SelectorWorker.getOrderedSelectorList():
        s = sd.selector
        if hasattr(s, 'initialize'):
            with timer("initializing selector '%s'" % s.name):
                s.verbose = True
                s.initialize()

    # apply the selectors to initial run list
    for s in SelectorWorker.selectors():
        with timer("run selector '%s'" % s.name):
            s.verbose = True
            runlist = s.select(runlist)
            selectionOutput += ["%s" % s.__str__()]
        with timer("run AfterQuery for selector '%s'" % s.name):
            s.runAfterQuery(runlist)

    #for r in runlist:
    #    print(r.__dict__)
