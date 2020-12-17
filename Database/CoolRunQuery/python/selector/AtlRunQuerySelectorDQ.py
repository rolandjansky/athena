# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from __future__ import print_function
from CoolRunQuery.selector.AtlRunQuerySelectorBase import Selector, RunLBBasedCondition, OOO
from CoolRunQuery.utils.AtlRunQueryIOV    import IOVRange
from CoolRunQuery.utils.AtlRunQueryUtils  import coolDbConn
from CoolRunQuery.utils.AtlRunQueryLookup import DQChannel

from collections import defaultdict, namedtuple

DQDefectPayload = namedtuple("DQDefectPayload", "defect comment user ignore primary tolerable recoverable")
DQDefectPayload.pickled = DQDefectPayload._asdict

DD = namedtuple("DD","description comment since until")

def vfgen(vfobjs):
    for obj in vfobjs:
        yield OOO(obj.channel, (str(obj.Code),obj.Comment), IOVRange(starttime=obj.since, endtime=obj.until), True)

class DQSelector(Selector):
    def __init__(self, name='dataquality'):
        super(DQSelector,self).__init__(name)
        self.selectors = {}

        from CoolRunQuery.AtlRunQuerySelectorWorker import SelectorWorker
        SelectorWorker.getRetrieveSelector('readyforphysics','ReadyForPhysicsSelector')


    def __getChannelAndFolder(self, chanAndfolder):
        cf = chanAndfolder.split()
        if len(cf)==1:
            channel=''
            folder=cf[0]
        else:
            channel, folder = cf
        return (channel,folder)

    def __getCondition(self, folder):
        if folder in self.selectors:
            return self.selectors[folder]
        if folder.startswith('DEFECTS'):
            self.selectors[folder] = DQDefectCondition(self.name+"DEFECTS", folder)
        else:
            self.selectors[folder] = DQCondition(self.name+folder, folder, [])
        return self.selectors[folder]

    def addSelectionChannel(self, dq=[]):
        """
        dq can be e.g. ['EMBA yellow SHIFTOFL', 'FCALA green+ DQMFOFL', 'EMBA,EMBC,EMECA,EMECC yellow+ SHIFTOFL']
        this example should be translated into two selectors, one for each of the folders 'SHIFTOFL' and 'DQMFOFL'
        """
        folderCriteria = defaultdict(list)
        for x in dq:
            s = x.split()
            folderCriteria[s[-1]].append(s[:-1])
        for f in folderCriteria.keys():
            if f.startswith('DEFECTS'):
                self.selectors[f] = DQDefectCondition(self.name+"DEFECTS", f )
                self.selectors[f].addSelectionChannels(folderCriteria[f])
            else:
                self.selectors[f] = DQCondition(self.name+f, f, folderCriteria[f])


    def addShowChannel(self, chanAndfolder):
        (channel,folder) = self.__getChannelAndFolder(chanAndfolder)
        condition = self.__getCondition(folder)
        condition.addShowChannel(folder, channel, 'HEAD')

    def setShowOutput(self):
        for sel in self.selectors.values():
            sel.setShowOutput()


    def __str__(self):
        s = ""
        for sel in self.selectors.values():
            s += "\n%s" % sel
        return s


    def select(self, runlist):
        # garbage collector
        # import gc
        # gcod = gc.get_objects()
        # print ("GC objects",len(gcod))
        # print ("GC object count",gc.get_count())
        
        for sel in self.selectors.values():
            runlist = sel.select(runlist)
        return runlist

    def runAfterQuery(self,runlist):
        for sel in self.selectors.values():
            sel.runAfterQuery(runlist)
        


class DQCondition(RunLBBasedCondition):

    code  = {'unknown': 0, 'red': 1, 'yellow': 2, 'green': 3, 'black': -1}
    color = {'n.a.': 'n.a.', '0': 'U', '1': 'R', '2': 'Y', '3': 'G', '-1': 'B'}
    invert = {'n.a.': -2, 'B': -1, 'U': 0, 'R': 1, 'Y': 2, 'G': 3 }
    
    def __init__(self, name, foldername, dq):

        self.foldername = foldername
        self.channelNames = []
        self.channelNamesFlat = []
        self.channels = []
        self.channelsflat = []
        self.flags = []
        self.useVirtualFlags = False

        dbname = 'COOLOFL_GLOBAL'
        if 'ONL' in self.foldername:
            dbname = 'COOLONL_GLOBAL'

        # set up virtual flag logic folder and expand wild cards for VF in dq
        self.GetVirtualFlagsExpanded(dq, Selector.compDB(), dbname)

        for x in dq:
            ch = x[0].split(',')
            self.channelNames     += [["%s:%s" % (self.foldername,chn) for chn in ch]]
            self.channels         += [[self.DQChannel(chn) for chn in ch]]
            self.flags            += [(x+[''])[1]]

        for chN in self.channelNames:
            self.channelNamesFlat += chN
        for ch in self.channels:
            self.channelsflat += ch

        self.channelCommentsFlat = [n+"_m" for n in self.channelNamesFlat]

        # choose DB based on foldername

        # out of 'SHIFTOFL', 'DQCALCOFL', 'DQMFOFL', 'DQMFOFLH', 'DCSOFL', 'TISUMM', 'LBSUMM', 'MUONCALIB', 'DQMFONL', 'SHIFTONL'
        # these ones have a comment field

        self.folderHasComment = ( (self.foldername == "SHIFTOFL") or (self.foldername.startswith("SHIFTOFL#")) or
                                  (self.foldername == "LBSUMM")   or (self.foldername.startswith("LBSUMM#")) or
                                  (self.foldername == "DQCALCOFL")   or (self.foldername.startswith("DQCALCOFL#")) or
                                  (self.foldername.startswith("MUONCALIB#")) )

        # channel keys:
        # [Code (Int32) : 1], [deadFrac (Float) : 0], [Thrust (Float) : 0], [Comment (String255) : "warm start"]
        if self.folderHasComment:
            channelKeys  = zip(self.channelsflat, self.channelNamesFlat, len(self.channelsflat)*[('Code','Comment')])
        else:
            channelKeys  = zip(self.channelsflat, self.channelNamesFlat, len(self.channelsflat)*['Code'])
                                         
        super(DQCondition,self).__init__(name=name,
                                         dbfolderkey='%s::/GLOBAL/DETSTATUS/%s' % (dbname, self.foldername),
                                         channelKeys = channelKeys)

        self.flagInterpreter = {}
        for flag in self.flags:
            self.flagInterpreter[flag] = {}
            d = self.flagInterpreter[flag]
            d['refVal']      = None
            d['passFnc']     = None
            d['passFncName'] = ''

            if flag=='n.a.':
                d['passFnc']     = lambda x: x==-2
                d['passFncName'] = "x=='n.a.'"
            elif flag[-1] in '+-':
                cd = self.code[flag[:-1].lower()]
                d['refVal'] = cd
                if flag[-1]=='+':
                    if cd == -1:
                        d['passFnc'] = lambda x: int(x)>=-1
                    elif cd == 0:
                        d['passFnc'] = lambda x: int(x)>=0
                    elif cd == 1:
                        d['passFnc'] = lambda x: int(x)>=1
                    elif cd == 2:
                        d['passFnc'] = lambda x: int(x)>=2
                    elif cd == 3:
                        d['passFnc'] = lambda x: int(x)>=3
                    d['passFncName'] = "x>='%i'" % d['refVal']
                else:
                    if cd == -1:
                        d['passFnc'] = lambda x: int(x)<=-1
                    elif cd == 0:
                        d['passFnc'] = lambda x: int(x)<=0
                    elif cd == 1:
                        d['passFnc'] = lambda x: int(x)<=1
                    elif cd == 2:
                        d['passFnc'] = lambda x: int(x)<=2
                    elif cd == 3:
                        d['passFnc'] = lambda x: int(x)<=3
                    d['passFncName'] = "x<='%i'" % d['refVal']
            else:
                cd = self.code[flag.lower()]
                d['refVal'] = cd
                if cd == -1:
                    d['passFnc'] = lambda x: int(x)==-1
                elif cd == 0:
                    d['passFnc'] = lambda x: int(x)==0
                elif cd == 1:
                    d['passFnc'] = lambda x: int(x)==1
                elif cd == 2:
                    d['passFnc'] = lambda x: int(x)==2
                elif cd == 3:
                    d['passFnc'] = lambda x: int(x)==3
                d['passFncName'] = "x=='%i'" % d['refVal']
                
        self.passSpecs = {}
        for chgrp, flag in zip(self.channelNames,self.flags):
            for ch in chgrp:
                self.passSpecs[ch] = self.flagInterpreter[flag]

    def _getFolder(self):
        from CoolRunQuery.AtlRunQueryRun import RunData
        RunData.DQLogic = self.channelNames
        RunData.DQKeys = self.channelNamesFlat

        f = coolDbConn.GetDBConn(schema = self.schema,db=Selector.condDB()).getFolder(self.folder)
        if self.useVirtualFlags:
            f = self.VirtualFlagFolder(f)
        else:
            if f.versioningMode()==0:
                self.tagname=""
            if self.tagname not in ["HEAD", ""]:
                self.tagname = f.resolveTag(self.tagname)
        return f

    def _retrieve(self, iovmin, iovmax, f, sortedRanges):
        if self.useVirtualFlags:
            channels = [x.lower().split(':')[-1] for x in self.channelNamesFlat]
            objs = vfgen(f.browseObjects( iovmin, iovmax, channels, self.tagname ))
        else:
            objs = super(DQCondition,self)._retrieve(iovmin, iovmax, f, sortedRanges)
        return objs

    def DQChannel(self,dqfullname):
        dqname = dqfullname.split(':')[-1].split('#')[0]
        if self.useVirtualFlags and dqname in self.vfl.get_logic_list().keys():
            try:
                return self.vfl.get_logic_list()[dqname].record.channel
            except AttributeError:
                return self.vfl.get_logic_list()[dqname].channel
        return DQChannel(dqname)

    def GetVirtualFlagsExpanded(self, dqlist, db, schema):
        for i in range(len(dqlist)):
            dqs = dqlist[i][0].split(',')
            newdqs = []
            for c in dqs:
                if '_' in c:
                    newdqs += self.ExpandVFlag(c,db,schema)
                else:
                    newdqs.append(c)
            dqlist[i][0] = ','.join(newdqs)

    def ExpandVFlag(self, cpflag, db, schema):
        vfl = self.GetVirtualFlagLogic(db, schema)
        vflags = vfl.get_logic_list().keys()
        useprimaries = cpflag[-1]=='+'
        cpflag=cpflag.rstrip('+')
        expflags = []

        if cpflag in vflags:
            self.AddVFHeaderData(cpflag)
            expflags += [cpflag]
            if useprimaries:
                expflags += self.getVFDef(cpflag)
        else:
            for vf in vflags:
                if vf.startswith(cpflag):
                    self.AddVFHeaderData(vf)
                    expflags += [vf]
                    if useprimaries:
                        expflags += self.getVFDef(vf)
                    
        if len(expflags)==0:
            raise RuntimeError("Virtual Flag pattern %s does not match any virtual flag: %r" % (cpflag, vfl.get_logic_list().keys()))

        return expflags


    def GetVirtualFlagLogic(self, db, schema):
        if self.useVirtualFlags:
            return self.vfl
        try: # import
            from VirtualFlags import VirtualFlagLogicFolder, VirtualFlagFolder
        except ImportError:
            print ("Can't import virtual flags")
            import traceback
            traceback.print_exc()

        self.VirtualFlagFolder = VirtualFlagFolder
        self.vfl               = VirtualFlagLogicFolder(coolDbConn.GetDBConn(schema=schema, db=db))
        self.logics            = self.vfl.get_logic_list()
        self.useVirtualFlags   = True
        return self.vfl

    def AddVFHeaderData(self,cpflag):
        from CoolRunQuery.AtlRunQueryRun import Run
        if cpflag in Run.Fieldinfo:
            return
        Run.Fieldinfo[cpflag] = '<strong><b>%s</b></strong><br><table width="300"><tr><td>%s</td></tr></div>' % \
                                (self.vfl.get_logic_list()[cpflag].comment,
                                 ", ".join(self.getVFDef(cpflag)) )

    def getVFDef(self,cpflag):
        return self.vfl.resolve_primary_flags(self.vfl.resolve_dependancies([cpflag]))
    
    
        
    def ApplySelection(self,key):
        if key in self.passSpecs:
            return True
        return False

    def addShowChannel(self, folder, channelname, tag):
        tmplist = [[channelname,'']]
        self.GetVirtualFlagsExpanded(tmplist, Selector.compDB(), self.schema)

        for shch in tmplist[0][0].split(','):
            self.addSingleShowChannel(folder+':'+shch)
        
    def addSingleShowChannel(self, channelname):
        ch = self.DQChannel(channelname)
        ssr = self.DoSelectShowRetrieve()
        if ch in self.channelsflat: # channel exists already, just need to set showoutput to true
            idx = self.channelsflat.index(ch) # location of the channel in the flat list
            self.channelNamesFlat[idx] = channelname
            ssr[idx] += 2
        else:
            self.channelNamesFlat += [channelname]
            self.channelsflat += [self.DQChannel(channelname)]
            ssr += [2]
        # re-assign
        self.channels = self.channelsflat
        
        if self.folderHasComment:
            channelKeys = zip(self.channelsflat, self.channelNamesFlat, len(self.channelsflat)*[('Code','Comment')])
        else:
            channelKeys = zip(self.channelsflat, self.channelNamesFlat, len(self.channelsflat)*['Code'])

        self.setChannelKeys(channelKeys,ssr)

    def __str__(self):
        if self.applySelection:
            s = "SELOUT Checking in the DQ folder %s" % self.foldername
            flagCh = defaultdict(list)
            for flag,ch in zip(self.flags,self.channelNames):
                flagCh[flag].append('(' + ' or '.join(ch) + ')')
            for flag in flagCh:
                chlist = ' and '.join(flagCh[flag])
                s += "\nSELOUT %s is %r" % (chlist, flag)
            return s
        else:
            return "Retrieving DQ for channels %r from folder %s" % (self.channelNamesFlat, self.foldername)

    def passes(self,values, key):
        passfnc = self.passSpecs[key]['passFnc']
        if isinstance(values,tuple):
            v = values[0]
        else:
            v = values
        if v=='n.a.':
            v=-2
        try:
            if passfnc(v):  # passed this DQ flag in the OR group?
                return True
        except ValueError:
            # happens when comparing 'n.a.' with 0,1,2,3 (for 'U','R','Y','G') ... and the black flag? (AH)
            # should never set pass=True
            pass
        
        return False

    def rejectRun(self,run):
        for k in self.ResultKey():
            passfnc = self.passSpecs[k]['passFnc']
            for entry in run.data[k]:
                v = entry.value
                dqres = v[0] if type(v)==tuple else v
                dqcode = self.invert[dqres]
                entry.rejected = not passfnc( dqcode )
        return super(DQCondition,self).rejectRun(run)



    def prettyValue(self, value, key):
        if type(value)==tuple:
            return (self.color[value[0]], value[1])
        else:
            return self.color[value]
                
    def runAfterQuery(self,runlist):
        dqs = ['G','Y','R','B','U','n.a.']
        for run in runlist:
            for k in self.ResultKey():
                n={}
                run.stats[k] = {}
                for dq in dqs:
                    n[dq] = 0
                blocks = []
                for entry in run.data[k]:
                    if self.folderHasComment:
                        if entry.value == 'n.a.':
                            dq, dqcomment = ('n.a.','')
                        else:
                            dq, dqcomment = entry.value
                    else:
                        dq, dqcomment = (entry.value,None)
                    
                    n[dq] += len(entry)
                    if entry.startlb == 0:
                        n[dq] -= 1 # CAUTION: problem with LB-1 = 0 ==> needs to be corrected
                    if len(blocks) > 0 and blocks[-1][0]==(dq,dqcomment) and blocks[-1][2]==entry.startlb:
                        blocks[-1][2] = entry.endlb
                    else:
                        blocks += [ [(dq, dqcomment), entry.startlb, entry.endlb] ]

                maxn = max(n.values())
                # find maximum DQ flag
                dqmax = dqs[0]
                for dq in dqs:
                    if n[dq]==maxn:
                        dqmax=dq
                run.result[k]=dqmax
                run.stats[k] = { "counts" : n, "max": dqmax, "blocks": blocks }


class DQDefectCondition(RunLBBasedCondition):

    def __init__(self, name, folder):
        self.foldername   = folder # DEFECTS#<TAG>
        self.primaries    = {} # self.primaries[runnr] is the list of primary defects that are set in each run
        self.channelNames = []
        self.selectionChannelNames = []
        self.global_ignore = set() # set of all primary defects that should be ignored in all defects
        super(DQDefectCondition,self).__init__(name=name,
                                               dbfolderkey = 'COOLOFL_GLOBAL::%s' % self.foldername, # folder name handled internally by DQDefects
                                               channelKeys = [('DQDEFECT', 'DQ',('Code','Comment'))])

        self.data_keys[0]._second_internal_key = self.tagname

    def _getFolder(self):
        from CoolRunQuery.AtlRunQueryRun import RunData
        RunData.DefectSelector = self._pass
        self.__db = coolDbConn.GetDBConn('DEFECTS',self.tagname)
        return self.__db # return the DB, not the folder

    def _pass(self,data):
        these_defects = []
        if data is not None:
            for x in data:
                if x.value.ignore is None: # no ignore
                    these_defects += [x.value.defect]
                else:
                    these_defects += [x.value.defect+'\\'+x.value.ignore]
        for orGroup in self.selectionChannelNames:
            any_passes = False
            for k in orGroup:
                if k[0]=='\\':
                    continue # "\Defect" are modifiers not requirements so we ignore it in the check
                if k[0]=='!':  # !DEFECT
                    kk = k[1:]
                    passes = kk not in these_defects
                else:
                    passes = k in these_defects
                if passes:
                    any_passes = True
                    break
            if not any_passes: # none pass in the OR-group
                return False
        return True

    def _fix_channel_names(self, db):
        newl = []
        for litem in self.selectionChannelNames:
            newlitem = []
            for k in litem:
                if k[0]=='\\':
                    newlitem.append(k)
                elif k[0] == '!':
                    newlitem.append('!' + db.normalize_defect_names(k[1:]))
                else:
                    newlitem.append(db.normalize_defect_names(k))
            newl.append(newlitem)
        self.selectionChannelNames = newl
        self.global_ignore = set(db.normalize_defect_names(self.global_ignore))


    def __getChannels(self, db, channel_name_patterns):
        """ based on the patterns in the list channel_name_patterns create a
        list of pairs of patterns and matching defects ( pattern, set(matching defects) )
        and attach it to the global Run.FieldInfo dictionary under 'DQ'
        """
        from re import compile
        from CoolRunQuery.AtlRunQueryRun import Run
        if 'DQ' not in Run.Fieldinfo:
            Run.Fieldinfo['DQ'] = {}
        if 'DefMatch' not in Run.Fieldinfo['DQ']:
            Run.Fieldinfo['DQ']['DefMatch'] = []
        Run.Fieldinfo['DQ']['IntolerableDefects'] = db.get_intolerable_defects(old_primary_only=False)
        matches = set()
        for pattern in channel_name_patterns:
            if pattern.upper() != 'DET':
                cpattern = compile(pattern)
                channelnames = set(filter(cpattern.match,db.defect_names))
                channelnames.update(filter(cpattern.match,db.virtual_defect_names))
            else:
                channelnames = set([d for d in db.defect_names if '_' not in d])
                channelnames.update([d for d in db.virtual_defect_names if '_' not in d])
            matches.update(channelnames)
            Run.Fieldinfo['DQ']['DefMatch'] += [(pattern,channelnames)]
        return matches


    def _retrieve(self, iovmin, iovmax, db, sortedRanges):
        runmin = iovmin>>32
        runmax = iovmax>>32
        since  = (runmin,iovmin&0xFFFFFFFF)
        until  = (runmax,iovmax&0xFFFFFFFF)

        # add the channels for showing (those can be wildcarded, so call __getChannels to get exact matches)
        channels = self.__getChannels(db,self.channelNames)

        # Rewrite defect names to get cases to be correct
        self._fix_channel_names(db)

        # add the channels for selection (those have to match exactly, they are simply added)  
        channels_with_ignore = {} # unique
        for selChans in self.selectionChannelNames:
            channels.update([x.lstrip('!') for x in selChans if '\\' not in x])
            for x in selChans: # for the defects with ignore condition
                if '\\' not in x:
                    continue
                channel, ignore_str = self.sort_uniq_ignores(x).split('\\',1)
                if ignore_str not in channels_with_ignore:
                    channels_with_ignore[ignore_str] = []
                channels_with_ignore[ignore_str] += [channel.lstrip('!')]

        #print ("CHANNELS",channels)
        if len(channels) + len(channels_with_ignore)==0:
            return []

        # we need to remove the special case ANY from the set
        if 'ANY' in channels:
            channels.remove('ANY')

        res = [] if len(channels)==0 else [ (db.retrieve(since=since, until=until, channels=channels, ignore=self.global_ignore, with_primary_dependencies=True).trim_iovs, None) ]

        # and also treat the defects with 'ignore' condition
        for _ignore_str, _channels in channels_with_ignore.items():
            ignore = set(_ignore_str.split('\\'))
            ignore.update(self.global_ignore)
            ## https://svnweb.cern.ch/trac/atlasoff/browser/DataQuality/DQDefects/tags/DQDefects-00-00-24/python/db.py#L154
            res += [ (db.retrieve(since=since, until=until, channels=_channels, ignore=ignore, with_primary_dependencies=True).trim_iovs, _ignore_str) ]

        return self.defgen(db, res, channels, channels_with_ignore)

    

    def defgen(self, db, defects_with_ignore, channels, channels_with_ignore):
        """
        defects: list of defects returned by query
        channels: explicit list of channels that matched the pattern
        """
        intolerableDefects = db.get_intolerable_defects(old_primary_only=False)
        for defects, ignore in defects_with_ignore:
            chanlist = channels if ignore is None else channels_with_ignore[ignore]
            for d in defects:
                if not d.present:
                    continue
                isVirtual = (d.user == 'sys:virtual') # db.defect_is_virtual(d.channel)
                if not isVirtual:
                    run = d.since.run
                    # fill list of primaries defects for this run (this is needed for the comments, but not for the selection)
                    if run not in self.primaries:
                        self.primaries[run] = []
                    self.primaries[run] += [d]
                if d.channel not in chanlist:
                    continue

                defPayload = DQDefectPayload(defect = d.channel, comment = d.comment,
                                             user = d.user, primary = not isVirtual,
                                             ignore = ignore,
                                             tolerable = (d.channel not in intolerableDefects),
                                             recoverable = d.recoverable)
                # note that the comment is either the user's comment,
                # a comment that the defect is auto-generated, or the
                # list of source defects in case of virtual defects
                
                #o = O("DQDEFECT", (d.channel, d.comment, ignore), IOVRange(starttime=d.since.real, endtime=d.until.real), True)
                o = OOO("DQDEFECT", defPayload, IOVRange(starttime=d.since.real, endtime=d.until.real), True)
                yield o


        
    def ApplySelection(self,key):
        return key=='DQ' and len(self.selectionChannelNames)!=0


    def sort_uniq_ignores(self,requirement):
        if '\\' not in requirement:
            return requirement
        x = requirement.split('\\')
        new_req = x[0] + '\\' + '\\'.join(sorted(list(set(x[1:]))))
        return new_req
    

    def addSelectionChannels(self, dq):
        for channels,flag in dq:
            a = []
            prefix = '!' if flag == 'green' else ''
            for x in channels.split(','):
                if x[0]=='\\':
                    self.global_ignore.update( x.lstrip('\\').split('\\') ) # "\Defect" are modifiers not a selection requirement
                else:
                    a += [ prefix+self.sort_uniq_ignores(x) ]
            if len(a)>0:
                self.selectionChannelNames += [ a ]

        
    def addShowChannel(self, folder, channelname, tag):
        self.channelNames += [channelname]
        from CoolRunQuery.AtlRunQueryRun import Run
        if 'DQ' not in Run.Fieldinfo:
            Run.Fieldinfo['DQ'] = {}
        if 'DefChannels' not in Run.Fieldinfo['DQ']:
            Run.Fieldinfo['DQ']['DefChannels'] = []
        Run.Fieldinfo['DQ']['DefChannels'] += [channelname]

    
    def __str__(self):
        if self.applySelection:
            return "SELOUT Checking the DQ defects %s" % ', '.join([', '.join(cn) for cn in self.selectionChannelNames])
        else:
            return "Retrieving DQ defects %s" % ', '.join([', '.join(cn) for cn in self.selectionChannelNames])

    def passes(self,values, key):
        return True

    def rejectRun(self,run):
        return super(DQDefectCondition,self).rejectRun(run)

    def mergeRanges(self,it):
        """ two adjacent defect entries are merged into one"""
        doMerge = False
        if doMerge:
            mergedByDefect = []
            x = it.next()
            (openedat,openuntil) = (x.startlb,x.endlb)
            for x in it:
                if x.startlb == openuntil:
                    openuntil = x.endlb
                else:
                    mergedByDefect += [(openedat, openuntil)]
                    openedat,openuntil = x.startlb,x.endlb
            mergedByDefect += [(openedat, openuntil)]
            return mergedByDefect
        else:
            return [(x.startlb,x.endlb) for x in it]



    def runAfterQuery(self,runlist):
        """
        collects, sorts, and groups defects with LBs and comments
        """
        from itertools import groupby
        from operator import attrgetter

        for run in runlist:

            for k in self.ResultKey():
                # format of run.data[k] is
                # [ <DataEntry with value = (defect_name, defect_comment or defect_composition)>, ...]

                #for x in run.data[k]:
                #    print ("               %r" % (x,))
                if k not in run.data.keys():
                    run.result[k] = {}
                run.stats[k] = {}

                all_defects = sorted(list(set([x.value[0] for x in run.data[k]])))
                
                grouped_defects = groupby(sorted(run.data[k],key=lambda x: x.value[0]),key = lambda x: x.value[0]) # first sort by defect name, then group by defect name

                desired_defects = [ (defect_name, self.mergeRanges(lblist)) for defect_name, lblist in grouped_defects ]

                run.stats[k] = { "defects": desired_defects,
                                 "primaries": {} }

                primaries = {}  # defect : list of primaries with lb-range
                if run.runNr in self.primaries:
                    primgroups = groupby(sorted([p for p in self.primaries[run.runNr]],key=attrgetter('channel')),key=attrgetter('channel'))
                    primaries = dict([(p,list(l)) for p,l in primgroups])
                    
                for defect in all_defects:
                    if self.__db.defect_is_virtual(defect):
                        reps = []
                        self.find_primaries(DD, self.__db.virtual_defect_logics[defect], primaries, defect, reps)
                        run.stats[k]["primaries"][defect] = reps
                    elif defect in primaries:
                        run.stats[k]["primaries"][defect] = [ DD("->%s" % def_obj.channel, def_obj.comment, def_obj.since.lumi, def_obj.until.lumi) for def_obj in primaries[defect] ]


                #import pickle
                #f = open("dev.pickle","w")
                #pickle.dump(run.stats[k]["primaries"],f)
                #f.close()

                #for x,l in run.stats[k]["primaries"].items():
                #    print (x)
                #    for y in sorted(l):
                #        print ("    ",y)


    def find_primaries(self, DD, defect_logic, primaries, curpath, reps):
        for pdef in defect_logic.primary_defects:
            if pdef not in primaries:
                continue
            pdefects = primaries[pdef]
            for pdeflb in pdefects:
                reps += [  DD("%s->%s" % (curpath, pdeflb.channel), pdeflb.comment, pdeflb.since.lumi, pdeflb.until.lumi) ]
        for dep in defect_logic.dependencies:
            self.find_primaries(DD, dep, primaries, curpath+"->"+dep.name, reps)

