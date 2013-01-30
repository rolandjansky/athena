#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryParser.py
# Project: AtlRunQuery
# Purpose: Utility to translate a parser language (to be used in 
#          a web browser) into AtlRunQuery arguments
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 13, 2008
# ----------------------------------------------------------------
#
import sys,os,re
import urllib

from utils.AtlRunQueryLookup import InitDetectorMaskDecoder, DecodeDetectorMask, DQChannels

class ArgumentParser:
    dqFolderList = [ 'SHIFTOFL', 'DQCALCOFL', 'DQMFOFL', 'DQMFOFLH', 'DCSOFL', 'TISUMM', 'LBSUMM', 'MUONCALIB',
                     'DQMFONL', 'DQMFONLLB', 'SHIFTONL' ] # these are online folders

    def __init__ ( self ):
        self.const_arg    = ""
        self.default_find = ""
        self.isMCDB = False
        self.default_show = ["run","events","time"]

        # allowed 'query' arguments
        # tuple format: ( "short name", "long-name", "interpret function", "show function", "example", "default value" )
        self.sortedKeys  = [ "run", "time", "duration", "events", "allevents", "projectTag", "partition", "readyforphysics",
                             "trigkeys", "trigger", "release",
                             "ami", "magnets", "larcond", "db", "ctag", "lhc", "lumi", "dq",
                             "streams", "detector", "datasets", "all", "summary", "dqeff" ]
                           
        self.queryArgs = { "run":        ("r(uns)",      "run",    self.InterpretPeriods, self.ShowVariable, 
                                          'r(uns)        [format: "run 91000", "runs 91000-92000", "runs 91000-(+)" (this run and all before (after))]', ""),
                           "all":        ("all",         "all", self.InterpretString, self.ShowVariable, 
                                          'all           [format: only available as "show" option]', ""),
                           "summary":    ("summary",     "summary", self.InterpretString, self.ShowVariable, 
                                          'summary       [format: only available as "show" option]', ""),
                           "dqsummary":  ("dqsum(mary)", "dqsummary", self.InterpretString, self.ShowVariable, 
                                          'dqsum(mary)   [format: only available as "show" option]', ""),
                           "dqplots":    ("dqpl(ots)",   "dqplots", self.InterpretString, self.ShowVariable, 
                                          'dqpl(ots)     [format: only available as "show" option]', ""),
                           "dqeff":      ("dqeff",       "dqeff", self.InterpretString, self.ShowVariable, 
                                          'dqeff         [format: only available as "show" option]', ""),
                           "events":     ("ev(ents)",    "events", self.InterpretRange, self.ShowVariable, 
                                          'ev(ents)      [format: "events 10000+", ... (same as for run number)]', ""),
                           "allevents":  ("alle(vents)", "allevents", self.InterpretRange, self.ShowVariable, 
                                          'alle(vents)   [format: only available as "show" option]', ""),
                           "time":       ("t(ime)",      "time",   self.InterpretRange, self.ShowVariable, 
                                          't(ime)        [format: "time 10.9.2008-30.9.2008", "time 15.10.2008-(+)" (this date and all before (after))]', ""),
                           "duration":   ("dur(ation)",  "duration",  self.InterpretDuration, self.ShowVariable, 
                                          'dur(ation)    [format: "dur 10d/h/m/s+/-", "2000s+" (run duration more than 2000 sec), "dur 3h-" (less than 3 hours)]', ""),
                           "magnets":    ("m(agnet)",    "bfield", self.InterpretMagnets, self.ShowVariable, 
                                          'm(agnet)      [format: "magnet s(olenoid)", "magnet t(oroid)", "not magnet t(oroid)", ...]', ""),
                           "detector":   ("det(ector)",  "detmask", self.InterpretDetectorMask, self.ShowVariable, 
                                          'det(ector)    [format: "detector Pixel B" (for Pixel Barrel), "detector Pixel" (for Pixel B & EC), "all" (for all detectors, detector mask = 72551459979255)', ""),
                           "smk":        ("smk",         "smk",    self.InterpretRange, self.ShowVariable, 
                                          'smk           [format: "smk 398", "smk 398,399 (super-master-key labelling trigger menu, format like for run ranges)]', ""),
                           "trigkeys":   ("trigk(eys)",  "trigkeys",  self.InterpretRange, self.ShowVariable, 
                                          'trigk(eys)    [format: "tkeys" (show all trigger keys of run: SMK, L1 and HLT prescale keys)]', ""),
                           "release":    ("rel(ease)",   "release", self.InterpretString, self.ShowWithArg, 
                                          'rel(ease)     [format: "release 15.1.*" ',""),
                           "projectTag": ("ptag",        "projecttag",self.InterpretString, self.ShowVariable, 
                                          'ptag          [format: "ptag data08_cos,data08_cosmag,data09_cos", "ptag data08_cos*,data09_cos" (note: the projectTag in dataset name / denoted "filenamtTag" in COOL)]',"data08*,data09*,data10*,data11*,data12*,data13*,data14*,data15*,data16*"),
                           "partition":  ("p(artition)", "partition", self.InterpretString, self.ShowVariable, 
                                          'p(artition)   [format: "partition ATLAS"]', "ATLAS"),
                           "readyforphysics":  ("ready(forphysics)", "readyforphysics", self.InterpretString, self.ShowVariable, 
                                          'ready(forphysics)   [format: "readyforphysics T(rue)"]', ""),
                           "db":         ("db",          "db", self.InterpretString, self.ShowVariable, 
                                          'db            [format: "db <DB>, where <DB> is either DATA, MC, COMP200, or OFLP200',""),
                           "ctag":       ("ctag",        "ctag", self.InterpretString, self.ShowVariable, 
                                          'ctag          [format: "ctag COMCOND-HLTC-001-00" ',""),
                           "streams":    ("st(reams)",   "streams", self.InterpretStreams, self.ShowWithArg, 
                                          'st(reams)     [format: "stream RPCwBeam,TGCwBeam", "stream physics*" ',""),
                           "dq":         ("dq",          "dq",     self.InterpretDQ, self.ShowDQ, 
                                          'dq            [format: "dq <FLAG> <status>", where the data quality status is "g(reen)", "y(ellow)", "r(ed)", "u(known)", example: dq PIXB y+ (means yellow or green status)]', ""),
                           "trigger":    ("tr(igger)",   "trigger", self.InterpretString, self.ShowWithArg, 
                                          'tr(igger)     [format: "trigger *Electron*" ',""),
                           "lumi":       ("lu(minosity)","luminosity", self.InterpretWithTwoArgs, self.ShowWithArg, 
                                          'lu(minosity)  [format: "lumi FOLDER", default: LBLESTOFL]" ',""),
                           "olc":        ("olc",         "olc", self.InterpretWithTwoArgs, self.ShowWithArg, 
                                          'olc           [format: "olc" (OLC = Online Luminosity Calculator)]" ',""),
                           "bs":         ("bs",          "beamspot", self.InterpretString, self.ShowWithArg, 
                                          'bs            [format: "bs"]" ',""),
                           "bpm":        ("bpm",         "bpm", self.InterpretString, self.ShowVariable, 
                                          'bpm           [format: "show bpm"]" ',""),
                           "datasets":   ("da(tasets)",  "datasets", self.InterpretString, self.ShowWithArg, 
                                          'da(tasets)    [format: show datasets *NTUP*"]" ',""),
                           "ami":        ("ami",  "ami", self.InterpretString, self.ShowVariable, 
                                          'ami           [format: show ami"]" ',""),
                           "larcond":    ("lar(cond)",   "larcond", self.InterpretWithTwoArgs, self.ShowVariable, 
                                          'lar(cond)     [format: larcond nsamples 7 / show larcond"]" ',""),
                           "lhc":        ("lhc",         "lhc", self.InterpretWithTwoArgs, self.ShowVariable, 
                                          'lhc           [format: lhc fill 7 / show lhc"]" ',""),
                           "trigrates":  ("trigr(ates)", "trigrates", self.InterpretString, self.ShowWithArg, 
                                          'trigr(ates)   [format: sh trigrates L1_EM*"]" ',"")
                           }
        # allowed 'show' arguments

        # init detector mask
        (self.dName, self.NotInAll, self.vetoedbits) = InitDetectorMaskDecoder()

    def ParserUsage( self ):
        print ' '
        print 'Parser usage: python %s <string_argument>' % sys.argv[0]
        print ' '
        print 'A query starts with the keyword "f(ind)", several queries can be combined with "and", "(and) not"'
        print 'The requested response information is given through the keyword "sh(ow)" (the query must be terminated by a "/")'
        print ' '
        print 'Query words:'
        for key in self.queryArgs:
            print "   %s " % self.queryArgs[key][4]
        print ' ' 
        print 'Parts of a keyword written in paranthesis are optional'
        print ' ' 
        print 'Examples: '
        print ' ' 
        print '  find run 90272 and runs 90275-91900 and magnet toroid and magnet solenoid / show run and events'
        print '  f r 90272 and r 90275-91900 and m t and m s / sh r and ev [same as above in short-hand version]'
        print '  find runs 90275-91900 and not run 90280 and not dq EMEC r and show run and dq EMEC' 
        print ' '

    def replaceNumbers( self, n ):
        return n.replace('k','000').replace('m','000000')

    def getPtagAndPeriod( self, name ):
        ptag   = 'data11_7TeV'
        if '.' in name: ptag, period = name.split('.')
        else:           period = name
        letter = period[period.find('period')+6:][0]
        return ptag.strip(), period.strip(), letter

    def InterpretPeriods( self, atlqarg, arg, neg ):
        # format: arg = 'run data10_7TeV.periodA' or 'run periodA' (where 'data11_7TeV' is assumed)
        #               or 'data10_7TeV.periodA-periodC' or 'data10_7TeV.periodA,data10_7TeV.periodB,...'
        # This is case sensitive !!

        available_periods = []
        pfile_extension  = ".runs.list"


        def printPeriods(periods):
            print '\nAvailable periods:\n'
            for i,x in enumerate(periods):
                print x[1],"\t",
                if (i+1)%4==0: print ""
            sys.exit(0)


        def readRunListFromURL( periodname ):
            wwwpath    = "https://atlas.web.cern.ch/Atlas/GROUPS/DATAPREPARATION/DataPeriods/"
            webaddress = wwwpath + periodname + pfile_extension

            from utils.AtlRunQueryUtils import checkURL
            if not checkURL( webaddress ):
                print 'ERROR: Period "%s" not existing - please check name.' % periodname
                print '       Note: case sensitivity must be respected for period names!'
                printPeriods(available_periods)
            return [line.strip() for line in urllib.urlopen( webaddress )]

        def getDataPeriodsWithinRange(period_range):
            pshort  = re.compile("(?:(?:\d{2})(?P<year>\d{2})\.)?(?P<period>[a-zA-Z])(?P<subperiod>\d+)?")
            m1 = pshort.match(period_range[0].upper().replace('PERIOD',''))
            m2 = pshort.match(period_range[1].upper().replace('PERIOD',''))
            if m1==None or m2==None:
                sys.exit(0)
            p1 = (int(m1.group('year')) if m1.group('year') else 11,m1.group('period'),m1.group('subperiod'))
            p2 = (int(m2.group('year')) if m2.group('year') else p1[0],m2.group('period'),m2.group('subperiod'))
            p1c = 10000*p1[0] + 100*(ord(p1[1].upper())-65) + (int(p1[2]) if p1[2] else 0)
            p2c = 10000*p2[0] + 100*(ord(p2[1].upper())-65) + (int(p2[2]) if p2[2] else 99)
            #print "P1",p1,p1c
            #print "P2",p2,p2c
            if p1c>p2c: sys.exit(0)
            list_of_periods = []
            for p,p_name in sorted(available_periods):
                if p[2]==0: continue # no special VdM or AllYear stuff
                if p[2]%100==0: continue # no full periods
                include = (p[2]>=p1c and p[2]<=p2c)
                if include: list_of_periods += [(p[0],p[1],p_name)]  # 
                #print p,("--> include" if include else "")
            return list_of_periods


        def getRunsFromPeriods(list_of_periods):
            """list_of_periods:   periods for which run nr are returned, e.g [('10','B1'),('11','A'),...]"""
            runlist = []
            from CoolRunQuery.AtlRunQueryCOMA import ARQ_COMA
            for year, period, fname in list_of_periods:
                runlist += ARQ_COMA.get_runs(period, 2000+int(year))
                    
            return runlist

        arg = arg.split(None,1)[1]

        pat_last   = re.compile("(?:l|la|las|last) (\d*)$")
        pat_number = re.compile("\d{5,8}[+-]?$")  # simple number with 5-8 digits, possibly followed by a + or -
        pat_range  = re.compile("\d{5,8}-\d{5,8}$")  # simple number with 5-8 digits, possibly followed by a + or -
        pat_short  = re.compile("(?:(?:\d{2})(\d{2})\.)?([a-zA-Z]+\d*)$")
        pat_full   = re.compile("data(\d{2})_.*\.period([a-zA-Z]+\d*)$")

        # final result in here
        list_of_runs = []
                
        # are their any commas?        
        for tag in arg.split(','):

            # last X runs pattern
            m = pat_last.match(arg)
            if m:
                list_of_runs += [ "last%s" % m.group(1) ]
                continue
            
            # run numbers
            if pat_number.match(tag):
                list_of_runs += [tag]
                continue

            # run number range
            if pat_range.match(tag):
                list_of_runs += [tag]
                continue

            from CoolRunQuery.AtlRunQueryCOMA import ARQ_COMA
            available_periods = ARQ_COMA.get_all_periods()

            period_range = tag.split('-')
            if len(period_range)==2:
                list_of_periods = getDataPeriodsWithinRange(period_range)
                list_of_runs += getRunsFromPeriods(list_of_periods)
                continue
            
            m=pat_short.match(tag)
            if m:
                year, period = m.groups()
                if year==None: year="11"
                period = period.upper().replace('PERIOD','')
                if 'ALL' in period: period = 'AllYear'
                list_of_runs += getRunsFromPeriods([(year,period,None)])
                continue

            # backward compatible form: data10_7TeV.periodA
            m=pat_full.match(tag)
            if m:
                year, period = m.groups()
                list_of_runs += getRunsFromPeriods([(year,period,tag)])
                continue


        if len(list_of_runs)==0:
            print "No runs matching pattern"
            sys.exit(0)
                    
        return "--run " + ','.join([str(r) for r in list_of_runs])



    def InterpretRange( self, atlqarg, arg, neg ):
        # special possibility to give run ranges
        rge = self.replaceNumbers(arg.split(None,1)[1])
        rge = rge.replace('last ','l ').replace('las ','l ').replace('la ','l ').replace('l ','last ')
        return "--" + atlqarg.strip() + ' "' + rge.strip().replace(' ','') + '"'
            
    # range interpretation utilities
    def InterpretRangeNew( self, atlqarg, arg, neg ):
        # special possibility to give run ranges
        rge = self.replaceNumbers(arg.split(None,1)[1])
        rge = rge.replace('last ','l ').replace('las ','l ').replace('la ','l ').replace('l ','last ')
        return "--" + atlqarg.strip() + ' "' + rge.strip().replace(' ','') + '"'

    def InterpretString( self, atlqarg, arg, neg ):
        # '%' is translated into '*' wildcard
        arg = arg.partition(' ')[2]
        arg = arg.replace('%','*')
        arg = arg.strip().replace(' ','')
        
        atlqarg = atlqarg.strip()
        if atlqarg=='db' and (arg=='MC' or arg=='OFLP'):
            self.isMCDB = True

        # default for ReadyForPhysics flag
        if atlqarg == 'readyforphysics' and not arg: arg = '1'
            
        return '--%s "%s"' % (atlqarg, arg)

    def InterpretWithTwoArgs( self, atlqarg, arg, neg ):
        # '%' is translated into '*' wildcard
        cmd,sep,arg = arg.partition(' ')
        arg = arg.replace('%','*')
        arg = arg.strip()

        # special case for olc lumi
        if cmd.lower() == 'olc' and 'lumi' in arg.lower():
            atlqarg = 'olclumi'
            a = arg.split()
            if len(a) == 2:
                arg = a[1].strip()
            else:
                print 'ERROR in argument of command "olc lumi": no argument given' 
                sys.exit()

        # make equal to 'lumi' (for backward compatibility)
        if cmd.lower() == 'lumi':
            atlqarg = 'olclumi'
            if not arg:
                print 'ERROR in argument of command "olc lumi": no argument given' 
                sys.exit()

        # special case for 'lhc'
        if cmd.lower() == 'lhc':
            if 'stablebeams' in arg.lower():
                arg,sep,val = arg.partition(' ')
                val = val.strip().upper()
                if   val == '1' or val == 'T': val = 'TRUE'
                elif val == '0' or val == 'F': val = 'FALSE'
                elif val != 'TRUE' and val != 'FALSE':
                    print 'ERROR in argument of command "lhc": "%s". Value must be boolean (true/false or 1/0)' % val
                    sys.exit()
                arg += ' ' + val
        
        atlqarg = atlqarg.strip()
        if atlqarg=='db' and (arg=='MC' or arg=='OFLP'):
            self.isMCDB = True
            
        return '--%s "%s"' % (atlqarg, arg)

    def InterpretStreams( self, atlqarg, arg, neg ):
        # '%' is translated into '*' wildcard
        arg = arg.split()[1:]
        retstr = "--" + atlqarg.strip() + ' "' + arg[0].replace('%','*')
        if len(arg)>1:
            return retstr + " " + self.replaceNumbers(arg[1]) + '"'
        else:
            return retstr + '"'

    def InterpretDuration( self, atlqarg, arg, neg ):
        # sanity check
        key, sep, arg = arg.partition(' ')
        units = {'sec': 1, 's': 1, 'm': 60, 'h': 3600, 'd': 86400 }
        found = False
        for u, fac in units.items():
            if u in arg: 
                arg = arg.replace(u,'')
                if '-' in arg: sign = '-'
                else:          sign = '+'
                arg = "%i%s" % (int(arg.replace(sign,'').strip())*fac,sign)
                found = True
        if not found:
            self.ParseError( "Unit missing in duration tag: '%s' - should be 's', 'm', 'h', or 'd'" % arg )

        return self.InterpretRange( atlqarg, key + ' ' + arg, neg )

    def InterpretMagnets( self, atlqarg, arg, neg ):        
        key, s, arg = arg.partition(' ')

        # if 'arg' is empty, intepret directly
        if arg.strip() == '':
            self.ParseError( 'Keyword "mag(net)" requires argument' )

        # check if 'not' given
        arg = arg.strip()[0]

        # now interpret
        if    arg == "" : newarg = "off"
        else:
            if    arg == 's': newarg = "solenoid"
            elif  arg == 't': newarg = "toroid"
            else:
                self.ParseError( "cannot interpret of magnetic field: '%s'" % arg )
            if    neg: newarg += "off"
            else:      newarg += "on"
                
        return "--" + atlqarg.strip() + ' "' + newarg.strip().replace(' ','') + '"'

    @classmethod
    def DQGetFolder( cls, args, allowWildCards=False ):
        """interpret dq show command
        * args are all arguments after dq codeword

        possible formats for 'dq args':
        1) dq
        2) dq D
        3) dq F
        4) dq F#
        5) dq D F
        6) dq #T
        7) dq D #T
        8) dq F#T
        9) dq D F#T

        where
        D is a komma-separated list of defects or DQ-flags or *, each optionally proceeded by a ! or followed by a \(defect,defect,defect,...)
        F an folder name [default DEFECTS]
        T a cool tag [default HEAD]

        If no folder is specified the new defects folder is used
        """


        d = { 'flag': '', 'folder': '',  'tag': ''}
        if '#' in args:
            if allowWildCards:
                m = re.match('(?P<flag>[!.*\w,\-\$\\\]*?)\s*?(?P<folder>\w*)#(?P<tag>[\w-]*)', args)
            else:
                m = re.match('(?P<flag>[!\w,\-\$\\\]*?)\s*?(?P<folder>\w*)#(?P<tag>[\w-]*)', args)
        else:
            m = re.match('(?P<flag>[!\w,\-\$\\\]*)\s*(?P<folder>[\w-]*)', args)

        if m: d.update(m.groupdict())

        flag   = d['flag'].lower()
        folder = d['folder'].upper()
        tag    = d['tag']

        if flag.upper() in cls.dqFolderList:
            folder = flag.upper()
            flag   = ''

        if folder == '': folder = 'DEFECTS'
        if flag   == '': flag   = '*'
        if tag    != '':
            tag    = '#' + tag
            folder = folder + tag

        return flag, folder, tag
        

    def ShowDQ( self, atlqarg, short, arg ):

        # check if 'arg' has additional arguments (eg, dq PIXB, ...)
        args = arg.partition(' ')[2]

        # check/insert COOL folder
        flags, folder, tag = self.DQGetFolder( args, allowWildCards = True )

        newarg = ""

        if folder.startswith('DEFECTS'):
            # defects DB
            if flags == "*": # no additional arguments given --> show all flags
                return '--show "dq DEFECTS%s" ' % tag
            else:
                # assume some defects are given, split by ","
                return ' '.join(['--show "dq %s DEFECTS%s"' % (fl.upper(), tag) for fl in flags.split(',')])

        # replace 'lar' by appropriate choices
        flags = flags.replace( 'lar','emba,embc,emeca,emecc,heca,hecc,fcala,fcalc' )

        # replace 'tile' by appropriate choices
        flags = flags.replace( 'tile','tigb,tilba,tilbc,tieba,tiebc' )

        # replace 'trig' by appropriate choices
        flags = flags.replace( 'trig','l1cal,l1mub,l1mue,l1ctp,trcal,trbjt,trbph,trcos,trele,trgam,trjet,trmet,trmbi,trmuo,trtau,tridt' )

        dqitems = DQChannels()
        if flags == "*": # no additional arguments given --> show all flags
            # query over all DQ channels and add to show
            for key, dqchan in dqitems:
                newarg += '--show "dq %s %s" ' % (dqchan, folder)
        else:
            # assume some DQ channels are given, split by "," or " "
            argList = flags.split(',')
            for a in argList:
                a = a.strip().lower()
                if self.isVirtualFlag(a):
                    newarg += '--show "dq %s %s" ' % (a.upper(), folder)
                else:
                    for key, dqchan in dqitems:
                        dc = dqchan.lower()
                        if a[0:3]==('trg'): # all trigger dq (tr but not trt)
                            if dc[0:2]=="tr" and dc[2]!='t':
                                newarg += '--show "dq %s %s" ' % (dqchan, folder)
                        else:
                            if a in dc:
                                newarg += '--show "dq %s %s" ' % (dqchan, folder)

        return newarg



    def InterpretDQ( self, atlqarg, args, neg ):
        # the following formats for dq need to work
        # dq [any] sys1[,sys2] value[+|-]   // note: a system must be given, if you want all the specify '*' e.g. 'dq * g'

        # split, and remove first word ('dq'), be reminded that a split automatically removes _all_ whitespaces
        args = args.split()[1:]

        hasColor = len(args)>0 and re.match('(n.a.|u|r|y|g|b)[+-]{0,1}$',args[-1].lower())
        if hasColor:
            # last argument must be dq value
            dqflagVal = args[-1]
            args = args[:-1]
        else:
            dqflagVal = 'x'


        # check for the 'any' identifier (any --> OR, otherwise AND)
        useAny = len(args)>0 and (args[0] == 'any')
        if useAny: args = args[1:]

        # get the cool folders
        flags, folder, tag = self.DQGetFolder( ' '.join(args) )

        newarg = ""

        if folder.startswith('DEFECTS'):
            if flags == "*":
                newarg = "--dq !ANY " + folder
            else:
                # assume some DQ channels are given, split by "," or " "
                dqchans = flags.upper().split(',')
                if useAny:
                    newarg += self.InterpretSingleDQ( atlqarg, (','.join(dqchans), dqflagVal, folder) ) + ' '
                else:
                    for dqchan in dqchans:
                        newarg += self.InterpretSingleDQ( atlqarg, (dqchan, dqflagVal, folder) ) + ' '

        else:
            if not hasColor:
                self.ParseError( 'DQ flags need to be assigned a value ("n.a.", "u", "g", "y", "r"), eg, "dq PIXB,TIL yellow+" or "dq g"' )

            # treat some old aliases
            flags = flags.replace( 'lar','emba,embc,emeca,emecc,heca,hecc,fcala,fcalc' )
            flags = flags.replace( 'tile','tigb,tilba,tilbc,tieba,tiebc' )
            flags = flags.replace( 'trig','l1cal,l1mub,l1mue,l1ctp,trcal,trbjt,trbph,trcos,trele,trgam,trjet,trmet,trmbi,trmuo,trtau,tridt' )

            # loop over DQ channels
            dqitems = DQChannels()
            if flags == "*": # no additional arguments given --> select on all flags
                # query over all DQ channels and add to show
                dqchans = [y for (x,y) in dqitems]
                if useAny:
                    newarg += self.InterpretSingleDQ( atlqarg, (','.join(dqchans), dqflagVal, folder) ) + ' '
                else:
                    for dqchan in dqchans:
                        newarg += self.InterpretSingleDQ( atlqarg, (dqchan, dqflagVal, folder) ) + ' '

            else:
                # assume some DQ channels are given, split by "," or " "
                dqchans = []
                for a in flags.upper().split(','):
                    if self.isVirtualFlag(a):
                        tmpdqchans = [a]
                    else:
                        # wildcard -- DANGEROUS --
                        tmpdqchans = [dqchan for (k,dqchan) in dqitems if a in dqchan]
                        if 'LUMI' == a and 'LUMIONL' in tmpdqchans: tmpdqchans.remove('LUMIONL')
                        if 'IDVX' == a and 'MMUIDVX' in tmpdqchans: tmpdqchans.remove('MMUIDVX')
                    if len(tmpdqchans)==0: self.ParseError( 'Unknown DQ channel: "%s"' % a )
                    dqchans += tmpdqchans

                if useAny:
                    newarg += self.InterpretSingleDQ( atlqarg, (','.join(dqchans), dqflagVal, folder) ) + ' '
                else:
                    for dqchan in dqchans:
                        newarg += self.InterpretSingleDQ( atlqarg, (dqchan, dqflagVal, folder) ) + ' '


        return newarg

    def isVirtualFlag(self, flag):
        return '_' in flag

    def InterpretSingleDQ( self, atlqarg, args ):
        # expand colours
        if len(args) != 3:
            self.ParseError( 'ERROR in DQ argument: "%s" --> need <Flag> AND <Status> (len is: %i)' % (fullarg, len(arg)) )
        flag, col, folder = args

        flag = flag.upper() # upper case status flags

        col = col.lower() # lower case color status only        
        trcol = ''
        if not folder.startswith('DEFECTS'):
            if   col[0] == 'n': trcol = 'n.a.'
            elif col[0] == 'u': trcol = 'unknown'
            elif col[0] == 'g': trcol = 'green'
            elif col[0] == 'y': trcol = 'yellow'
            elif col[0] == 'r': trcol = 'red'
            elif col[0] == 'b': trcol = 'black'
            else:
                self.ParseError( 'ERROR in DQ argument: "%s" --> uknown status: %s' % (args, col) )
            if '+' in col: trcol+= '+'
            if '-' in col: trcol+= '-'
        else:
            if   col[0] == 'n': trcol = 'red'
            elif col[0] == 'u': trcol = 'red'
            elif col[0] == 'g': trcol = 'green'
            elif col[0] == 'y': trcol = 'red'
            elif col[0] == 'r': trcol = 'red'
            elif col[0] == 'b': trcol = 'red'
            elif col[0] == 'x': trcol = 'none'
            else:
                self.ParseError( 'ERROR in DQ argument: "%s" --> uknown status: %s' % (fullarg, col) )


        return "--" + atlqarg.strip() + ' "' + flag + ' ' + trcol + ' ' + folder + '"'
        
    def InterpretDetectorMask( self, atlqarg, arg, neg ):
        arg = (arg.partition(' ')[2]).lower().strip()
        anyflag = ''
        if 'any' in arg:
            arg = arg.replace('any','').strip()
            if not neg: anyflag = 'A'
            
        # check who's in
        mask = 0
        for sarg in arg.split(","):
            for i in range(0,len(self.dName)):
                # is vetoed ?
                if i in self.vetoedbits: continue
                
                # sanity
                d = self.dName[i].lower() + self.NotInAll[i]
                if not ('unknown' in d or 'removed' in d):
                    # the actual query
                    if (sarg == 'all' and not 'NotInAll' in d) or sarg in d:
                        mask |= 1 << i

        # sanity check
        if mask == 0:
            print 'ERROR: could not find detector: "%s" in detector list' % arg
            print self.dName
            print 'Note: search is case INSENSITIVE'
            self.ParseError( '' )

        # decide whether detectors are required IN or OUT of partition
        atlqarg.strip()
        if neg: atlqarg += 'out'
        else: atlqarg += 'in'

        return "--" + atlqarg + ' "%i%s' % (mask,anyflag) + '"'
        
    def ParseError( self, errtext, pos = -1 ):
        print ' '
        print 'ERROR in argument: "%s"' % self.const_arg
        print "%s" % errtext
        print ' '
        sys.exit(1)

    def MatchingQueryArg(self, arg):
        arg = arg.split()[0] # first word of arg 'r' in 'r 9000-23100'
        for key in self.queryArgs:
            short = self.queryArgs[key][0]  # e.g. 'r(uns)'
            shortNoPar = short.replace('(','').replace(')','')  # e.g. 'runs'
            if '(' in short:
                short = short[:short.find('(')] # e.g. 'r'
            matchesArg = shortNoPar.startswith(arg.lower()) and arg.lower().startswith(short)
            if matchesArg: return key
        return None

    def RetrieveQuery( self, findarg ):
        # check that find part starts with f and remove the f(ind)
        if findarg[0] != 'f': 
            self.ParseError( "Argument must begin with 'f(ind)'", 0 )
        findarg = findarg[findarg.index(' ')+1:]  # remove first word 'f(ind)'

        
        argList = [x.strip() for x in findarg.split(" and ")]

        newarg = ""
        for arg in argList:
            # check if negation
            negation = False
            if 'not' in arg:
                arg = arg.replace("not","").strip()
                negation = True

            key = self.MatchingQueryArg(arg)
            if not key:
                self.ParseError( "Could not find predefined keyword for arg: '%s' in 'find' block" % (arg) )
            else:
                short, atlqarg, function = self.queryArgs[key][0:3]
                newarg += function( atlqarg, arg, negation ) + ' ' 

        return newarg

    def RetrieveShow( self, showarg ):
        
        #self.default_show = ["run","events","time"]
        arglist = self.default_show + [a.strip() for a in showarg.split(' and ') if a.strip()!=''] # split at 'and'

        # show all        
        if 'all' in arglist:
            idx = arglist.index('all')
            arglist[idx:idx+1] = ['ready', 'lhc', 'trigk', 'rel', 'streams', 'det'] # slice it out
        # show summary
        if 'summary' in arglist:
            arglist += ['dur', 'allev', 'str', 'dq' ]
        # show lhc ...
        if any([arg.startswith('lhc') for arg in arglist]):
            arglist += ['olc']
        # show dqsum & dqplots
        tmpArgList = [arg.startswith('dqsum') or arg.startswith('dqpl') for arg in arglist]
        if any(tmpArgList):
            idx = tmpArgList.index(True)
            arglist[idx+1:idx+1] = [ 'ready','lumi', 'lhc stablebeams', 'dq', 'ptag', 'trigrates L1_EM30', 'trigrates L1_EM5']

        # interpret argument list
        newarg = ""
        for arg in arglist:
            key = self.MatchingQueryArg(arg) # check argument validity
            if key == None:
                self.ParseError( "Could not find predefined keyword for arg: '%s' in 'show' block" % (arg) )

            # interpret argument and append to string
            (short, atlqarg, dummy, function) = self.queryArgs[key][0:4]
            newarg += function( atlqarg, short, arg )

        return newarg


    def ShowVariable( self, atlqarg, short, arg ):
        return '--show ' + atlqarg + ' '

    def ShowWithArg( self, atlqarg, short, arg ):
        if len(arg.split())==1:
            if   atlqarg == 'olc'       : return '--show olclumi --show olclbdata --show olcfillparams '
            elif atlqarg == 'luminosity': return '--show "lhc min" --show "%s 0" ' % (atlqarg)
            else                        : return '--show %s ' % atlqarg
        else:
            if   atlqarg == 'olc':
                return '--show "olclumi %s" --show olcfillparams --show olclbdata ' % arg.partition(' ')[2]
            elif atlqarg == 'luminosity':
                return '--show "lhc min" --show "%s %s" ' % (atlqarg, arg.partition(' ')[2])
            else:
                return '--show "%s %s" ' % (atlqarg, arg.partition(' ')[2])

    def PostProcessShowArgs( self, showargs ):
        
        # remove special duplicate
        if len(showargs.split('lhc')) >= 3: showargs = showargs.replace('--show "lhc min"','')

        # remove duplicates in show block
        duples  = ['--show %s' % d.strip() for d in showargs.split('--show') if d.strip()!='']
        uniques = reduce(lambda l, x: (x not in l and l.append(x)) or l, duples, [])
        showargs = ' '.join(uniques)

        return showargs
        

    def ParseArgument( self, const_arg ):
        self.const_arg = const_arg
        arg = const_arg.strip()

        # first separate the three parts ('/' is separator) and strip the spaces around each
        findarg, showarg, otherarg = [x.strip() for x in (self.const_arg.split('/')+['',''])[0:3]]

        # check that show part starts with sh and remove the sh(ow)
        if showarg!='':
            if showarg[0:2] != 'sh': 
                self.ParseError( "Show block must begin with 'sh(ow)'", 0 )
            showarg = showarg[showarg.index(' ')+1:]  # remove first word 'sh(ow)'
            #showarg = ' and ' + showarg # and put ' and ' in front
        else:
            if ' sh ' in findarg or ' sho ' in findarg or ' show ' in findarg:
                self.ParseError( "Show block must be separated by '/' from find condition'", 0 )

        # retrieve the query and show arguments (and check for input errors)
        queryarg = self.RetrieveQuery( findarg.strip() )
        shwarg   = self.RetrieveShow ( (showarg).strip() )

        # add 'lhc' to show if 'olclumi' query
        # necessary because of stable beams information
        if '--olclumi' in queryarg and not 'stablebeams' in queryarg: queryarg += '--lhc "stablebeams TRUE" '

        shwarg   = self.PostProcessShowArgs( shwarg )

        
        # interpret other arguments
        nodef_flag = False
        oargs = otherarg.split()
        # always verbose for the time being...
        #if not 'verbose' in queryarg: queryarg += ' --verbose'
        extraargs = {'verbose' : '--verbose'}
        idx = 0
        while idx < len(oargs):
            oa = oargs[idx]
            if oa == 'v':        extraargs['verbose'] = '--verbose'
            elif oa == 'noroot': extraargs['noroot']  = '--noroot'
            elif oa == 'root':   extraargs['root']  = '--root'
            elif oa == 'nohtml': extraargs['nohtml']  = '--nohtml'
            elif oa == 'html':   extraargs['html']  = '--html'
            elif oa == 'utc':    extraargs['utc']  = '--utc'
            elif oa == 'nogrl':  extraargs['nogrl']  = '--nogrl'
            elif oa == 'grl' or oa == 'xmlfile': extraargs['xmlfile']  = '--xmlfile MyLBCollection.xml:MyLBCollection'
            elif oa == 'nodef' : nodef_flag = True
            elif idx>0 and (oargs[idx-1]=='grl' or oargs[idx-1]=='xmlfile'):
                extraargs['xmlfile']  = '--xmlfile %s' % oa
            else:
                print "Extra argument '%s' unknown. Exiting." % oa
                sys.exit(1)
            idx+=1

        if 'verbose' in extraargs:
            print "Parser: '%s'" % self.const_arg
            print '  find argument: "%s"' % findarg.strip()
            print '  show argument: "%s"' % showarg.strip()
            print '  extra argument: "%s"' % otherarg.strip()

        # no defaults for MC
        if self.isMCDB: nodef_flag = True

        # add default arguments
        if not nodef_flag:
            for key, argument, defaultvalue in [ (x[0],x[1][1],x[1][5]) for x in self.queryArgs.items()]:
                if not argument in queryarg and defaultvalue != "":
                    queryarg += " --" + argument + ' "' + defaultvalue + '"'

        extraarg = ' '.join(extraargs.values())

        fullarg = '%s %s %s' % (queryarg, shwarg, extraarg)

        return fullarg


# command line driver for convenience
if __name__=='__main__':

    from CoolRunQuery.AtlRunQueryParser import ArgumentParser
    ap = ArgumentParser()

    if len(sys.argv) <= 1:
        print 'No query argument given'
        sys.exit(1)

    if sys.argv[1].lower() == 'detmask':
        print 'Detector mask %s correspond to:\n%s' % (sys.argv[2], DecodeDetectorMask( int(sys.argv[2] )))
        sys.exit()

    if sys.argv[1].lower() == 'help':
        ap.ParserUsage()
        sys.exit(1)
 
    atlqueryarg = ap.ParseArgument( ' '.join(sys.argv[1:]) )
    print "\nAtlRunQuery.py %s\n" % atlqueryarg

