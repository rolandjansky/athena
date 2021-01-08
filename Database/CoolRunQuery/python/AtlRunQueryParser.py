#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
from __future__ import print_function
from functools import reduce
import sys,re

from CoolRunQuery.utils.AtlRunQueryLookup import InitDetectorMaskDecoder, DecodeDetectorMask, DQChannels

from DQDefects import DefectsDB

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
                             "streams", "detector", "datasets", "all", "summary", "dqeff", "cosmics", "heavyions" ]
                           
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
                                          'ptag          [format: "ptag data08_cos,data08_cosmag,data09_cos", "ptag data08_cos*,data09_cos" (note: the projectTag in dataset name / denoted "filenamtTag" in COOL)]',"data0*,data1*"),
                           "partition":  ("p(artition)", "partition", self.InterpretString, self.ShowVariable, 
                                          'p(artition)   [format: "partition ATLAS"]', "ATLAS"),
                           "readyforphysics":  ("ready(forphysics)", "readyforphysics", self.InterpretString, self.ShowVariable, 
                                          'ready(forphysics)   [format: "readyforphysics T(rue)"]', ""),
                           "db":         ("db",          "db", self.InterpretString, self.ShowVariable, 
                                          'db            [format: "db <DB>, where <DB> is either DATA, MC, COMP200, CONDBR2, or OFLP200',""),
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
                                          'trigr(ates)   [format: sh trigrates L1_EM*"]" ',""),
                           "dqsumgrl":   ("dqsumgrl",    "dqsumgrl", self.InterpretString, self.ShowVariable,
                                          'dqsumgrl      [format: "dqsumgrl PHYS_StandardGRL_All_Good_25ns", Define tolerable/intolerable defects for DQ summary relative to GRL or other virtual defect, default is PHYS_StandardGRL_All_Good_25ns]', "PHYS_StandardGRL_All_Good_25ns"),
                           "cosmics":    ("cos(mics)", "cosmics", self.InterpretString, self.ShowVariable, 
                                          'cos(mics)     [format: only available as "show" option]', ""),
                           "heavyions":  ("heavy(ions)", "heavyions", self.InterpretString, self.ShowVariable, 
                                          'heavy(ions)   [format: only available as "show" option]', ""),
                           "logictag":   ("logictag",    "logictag", self.InterpretString, self.ShowVariable,
                                          'logictag      [format: "logictag "HEAD", Define logic tag for defect database used for DQ summary relative to GRL or other virtual defect, default is "HEAD"]', "HEAD"),
                           "defecttag":   ("defecttag",  "defecttag", self.InterpretString, self.ShowVariable,
                                          'defecttag     [format: "defecttag "HEAD", Define defect tag for defect database used for DQ summary relative to GRL or other virtual defect, default is "HEAD"]', "HEAD")                     }
        # allowed 'show' arguments

        # init detector mask
        (self.dName, self.NotInAll, self.vetoedbits) = InitDetectorMaskDecoder(run2=True) # needs to be fixed (made run-dependent) - move into Det Selector

    def ParserUsage( self ):
        print (' ')
        print ('Parser usage: python %s <string_argument>' % sys.argv[0])
        print (' ')
        print ('A query starts with the keyword "f(ind)", several queries can be combined with "and", "(and) not"')
        print ('The requested response information is given through the keyword "sh(ow)" (the query must be terminated by a "/")')
        print (' ')
        print ('Query words:')
        for key in self.queryArgs:
            print ("   %s " % self.queryArgs[key][4])
        print (' ' )
        print ('Parts of a keyword written in paranthesis are optional')
        print (' ' )
        print ('Examples: ')
        print (' ' )
        print ('  find run 90272 and runs 90275-91900 and magnet toroid and magnet solenoid / show run and events')
        print ('  f r 90272 and r 90275-91900 and m t and m s / sh r and ev [same as above in short-hand version]')
        print ('  find runs 90275-91900 and not run 90280 and not dq EMEC r and show run and dq EMEC' )
        print (' ')

    def replaceNumbers( self, n ):
        return n.replace('k','000').replace('m','000000')

    def getPtagAndPeriod( self, name ):
        ptag   = 'data11_7TeV'
        if '.' in name:
            ptag, period = name.split('.')
        else:
            period = name
        letter = period[period.find('period')+6:][0]
        return ptag.strip(), period.strip(), letter

    def InterpretPeriods( self, atlqarg, arg, neg ):
        """
        atlqarg: 'run'
        arg:     'run data10_7TeV.periodA' or 'run periodA' (where 'data11_7TeV' is assumed)
        #               or 'data10_7TeV.periodA-periodC' or 'data10_7TeV.periodA,data10_7TeV.periodB,...'
        # This is case sensitive !!
        """
        arg = arg.split(None,1)[1] # remove keyword 'run'

        from CoolRunQuery.AtlRunQueryInterpretDataPeriods import GetRuns

        list_of_runs = GetRuns(arg)
                
        if len(list_of_runs)==0:
            print ("No runs matching pattern")
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
        if atlqarg == 'readyforphysics' and not arg:
            arg = '1'
        
        # default for DQSumGRL flag
        if atlqarg == 'dqsumgrl' and not arg:
            arg = 'PHYS_StandardGRL_All_Good_25ns'
        
        # default for dbbtag flag
        if atlqarg == 'logictag' and not arg:
            arg = 'HEAD'
        if atlqarg == 'defecttag' and not arg:
            arg = 'HEAD'
            
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
                print ('ERROR in argument of command "olc lumi": no argument given' )
                sys.exit()

        # make equal to 'lumi' (for backward compatibility)
        if cmd.lower() == 'lumi':
            atlqarg = 'olclumi'
            if not arg:
                print ('ERROR in argument of command "olc lumi": no argument given' )
                sys.exit()

        # special case for 'lhc'
        if cmd.lower() == 'lhc':
            if 'stablebeams' in arg.lower():
                arg,sep,val = arg.partition(' ')
                val = val.strip().upper()
                if   val == '1' or val == 'T':
                    val = 'TRUE'
                elif val == '0' or val == 'F':
                    val = 'FALSE'
                elif val != 'TRUE' and val != 'FALSE':
                    print ('ERROR in argument of command "lhc": "%s". Value must be boolean (true/false or 1/0)' % val)
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
                if '-' in arg:
                    sign = '-'
                else:
                    sign = '+'
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
        if    arg == "":
            newarg = "off"
        else:
            if arg == 's':
                newarg = "solenoid"
            elif  arg == 't':
                newarg = "toroid"
            else:
                self.ParseError( "cannot interpret of magnetic field: '%s'" % arg )
            if neg:
                newarg += "off"
            else:
                newarg += "on"
                
        return "--" + atlqarg.strip() + ' "' + newarg.strip().replace(' ','') + '"'

    @classmethod
    def DQGetFolder( cls, args, allowWildCards=False ):
        
        #Get logictag and defecttag from sys.argv <--not nice but see no better way
        arglist = sys.argv[1].split(" ")
        logictag = "HEAD"
        defecttag = "HEAD"
        for idx, item in enumerate(arglist):
            if item == "logictag":
                logictag =  arglist[idx+1]
            if item == "defecttag":
                defecttag = arglist[idx+1]
        #Create dbb dummy with HEAD tag
        ddb = DefectsDB()
        #check if defect tag is defined in defect database
        if defecttag not in ['HEAD'] + ddb.defects_tags:
            print('WARNING (DQGetFolder): The defined defect tag "%s" is not defined in defect database. Will use "HEAD" tag instead!' %(defecttag))
            defecttag = "HEAD"
        #check if defect and logic tag is defined in defect database
        if logictag not in ['HEAD'] + ddb.logics_tags:
            print('WARNING (DQGetFolder): The defined logic tag "%s" is not defined in defect database. Will use "HEAD" tag instead!' %(logictag))
            logictag = "HEAD"
        #Now set tags
        ddb = DefectsDB(tag=(defecttag, logictag))
        #Create hierarchical tag
        htag = ""
        if logictag != "HEAD" and defecttag != "HEAD":
            logic_revision = int(logictag.split("-")[-1])
            defect_part = "-".join(defecttag.split("-")[1:])
            htag = "DetStatus-v%02i-%s" % (logic_revision, defect_part)
            #htag = ddb.new_hierarchical_tag(defecttag, logictag)

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
        D is a komma-separated list of defects or DQ-flags or *, each optionally proceeded by a ! or followed by a (defect,defect,defect,...)
        F an folder name [default DEFECTS]
        T a cool tag [default HEAD]

        If no folder is specified the new defects folder is used
        """
        
        d = { 'flag': '', 'folder': '',  'tag': ''}
        if '#' in args:
            if allowWildCards:
                m = re.match(r'(?P<flag>[!.*\w,\-\$\\\]*?)\s*?(?P<folder>\w*)#(?P<tag>[\w-]*)', args)
            else:
                m = re.match(r'(?P<flag>[!\w,\-\$\\\]*?)\s*?(?P<folder>\w*)#(?P<tag>[\w-]*)', args)
        else:
            m = re.match(r'(?P<flag>[!\w,\-\$\\\]*)\s*(?P<folder>[\w-]*)', args)

        if m:
            d.update(m.groupdict())

        flag   = d['flag'].lower()
        folder = d['folder'].upper()
        tag    = d['tag']

        if flag.upper() in cls.dqFolderList:
            folder = flag.upper()
            flag   = ''

        if folder == '':
            folder = 'DEFECTS'
        if flag == '':
            flag = '*'
        
        #Overwrite tag if htag defined
        if htag   != '':
            tag = htag
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
        if useAny:
            args = args[1:]

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
                        if 'LUMI' == a and 'LUMIONL' in tmpdqchans:
                            tmpdqchans.remove('LUMIONL')
                        if 'IDVX' == a and 'MMUIDVX' in tmpdqchans:
                            tmpdqchans.remove('MMUIDVX')
                    if len(tmpdqchans)==0:
                        self.ParseError( 'Unknown DQ channel: "%s"' % a )
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
            self.ParseError( 'ERROR in DQ argument: "%s" --> need <Flag> AND <Status> (len is: %i)' % (atlqarg, len(args)) )
        flag, col, folder = args

        flag = flag.upper() # upper case status flags

        col = col.lower() # lower case color status only        
        trcol = ''
        if not folder.startswith('DEFECTS'):
            if   col[0] == 'n':
                trcol = 'n.a.'
            elif col[0] == 'u':
                trcol = 'unknown'
            elif col[0] == 'g':
                trcol = 'green'
            elif col[0] == 'y':
                trcol = 'yellow'
            elif col[0] == 'r':
                trcol = 'red'
            elif col[0] == 'b':
                trcol = 'black'
            else:
                self.ParseError( 'ERROR in DQ argument: "%s" --> uknown status: %s' % (args, col) )
            if '+' in col:
                trcol+= '+'
            if '-' in col:
                trcol+= '-'
        else:
            if   col[0] == 'n':
                trcol = 'red'
            elif col[0] == 'u':
                trcol = 'red'
            elif col[0] == 'g':
                trcol = 'green'
            elif col[0] == 'y':
                trcol = 'red'
            elif col[0] == 'r':
                trcol = 'red'
            elif col[0] == 'b':
                trcol = 'red'
            elif col[0] == 'x':
                trcol = 'none'
            else:
                self.ParseError( 'ERROR in DQ argument: "%s" --> uknown status: %s' % (atlqarg, col) )


        return "--" + atlqarg.strip() + ' "' + flag + ' ' + trcol + ' ' + folder + '"'
        
    def InterpretDetectorMask( self, atlqarg, arg, neg ):
        arg = (arg.partition(' ')[2]).lower().strip()
        anyflag = ''
        if 'any' in arg:
            arg = arg.replace('any','').strip()
            if not neg:
                anyflag = 'A'
            
        # check who's in
        mask = 0
        for sarg in arg.split(","):
            for i in range(0,len(self.dName)):
                # is vetoed ?
                if i in self.vetoedbits:
                    continue
                
                # sanity
                d = self.dName[i].lower() + self.NotInAll[i]
                if not ('unknown' in d or 'removed' in d):
                    # the actual query
                    if (sarg == 'all' and 'NotInAll' not in d) or sarg in d:
                        mask |= 1 << i

        # sanity check
        if mask == 0:
            print ('ERROR: could not find detector: "%s" in detector list' % arg)
            print (self.dName)
            print ('Note: search is case INSENSITIVE')
            self.ParseError( '' )

        # decide whether detectors are required IN or OUT of partition
        atlqarg.strip()
        if neg:
            atlqarg += 'out'
        else:
            atlqarg += 'in'

        return "--" + atlqarg + ' "%i%s' % (mask,anyflag) + '"'
        
    def ParseError( self, errtext, pos = -1 ):
        print (' ')
        print ('ERROR in argument: "%s"' % self.const_arg)
        print ("%s" % errtext)
        print (' ')
        sys.exit(1)

    def MatchingQueryArg(self, arg):
        arg = arg.split()[0] # first word of arg 'r' in 'r 9000-23100'
        for key in self.queryArgs:
            short = self.queryArgs[key][0]  # e.g. 'r(uns)'
            shortNoPar = short.replace('(','').replace(')','')  # e.g. 'runs'
            if '(' in short:
                short = short[:short.find('(')] # e.g. 'r'
            matchesArg = shortNoPar.startswith(arg.lower()) and arg.lower().startswith(short)
            if matchesArg:
                return key
        return None

    def RetrieveQuery( self, findarg ):
        # check that find part starts with f and remove the f(ind)
        firstword, findarg = findarg.split(None,1)
        if not 'find'.startswith(firstword):
            self.ParseError( "Argument must begin with 'f(ind)'", 0 )

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
            arglist += ['dur', 'allev', 'str', 'dq']
        # show lhc ...
        if any([arg.startswith('lhc') for arg in arglist]):
            arglist += ['olc']
        # show dqsum & dqplots
        tmpArgList = [arg.startswith('dqsum') or arg.startswith('dqpl') for arg in arglist]
        if any(tmpArgList):
            idx = tmpArgList.index(True)
            arglist[idx+1:idx+1] = [ 'ready','lumi', 'lhc stablebeams', 'dq', 'ptag', 'trigrates L1_EM30', 'trigrates L1_EM5', 'trigrates L1_EM12']

        # interpret argument list
        newarg = ""
        for arg in arglist:
            key = self.MatchingQueryArg(arg) # check argument validity
            if key is None:
                self.ParseError( "Could not find predefined keyword for arg: '%s' in 'show' block" % (arg) )

            # interpret argument and append to string
            (short, atlqarg, dummy, function) = self.queryArgs[key][0:4]
            newarg += function( atlqarg, short, arg )

        return newarg


    def ShowVariable( self, atlqarg, short, arg ):
        return '--show ' + atlqarg + ' '

    def ShowWithArg( self, atlqarg, short, arg ):
        if len(arg.split())==1:
            if  atlqarg == 'olc':
                return '--show olclumi --show olclbdata --show olcfillparams '
            elif atlqarg == 'luminosity':
                return '--show "lhc min" --show "%s 0" ' % (atlqarg)
            else:
                return '--show %s ' % atlqarg
        else:
            if atlqarg == 'olc':
                return '--show "olclumi %s" --show olcfillparams --show olclbdata ' % arg.partition(' ')[2]
            elif atlqarg == 'luminosity':
                return '--show "lhc min" --show "%s %s" ' % (atlqarg, arg.partition(' ')[2])
            else:
                return '--show "%s %s" ' % (atlqarg, arg.partition(' ')[2])

    def PostProcessShowArgs( self, showargs ):
        
        # remove special duplicate
        if len(showargs.split('lhc')) >= 3:
            showargs = showargs.replace('--show "lhc min"','')

        # remove duplicates in show block
        duples  = ['--show %s' % d.strip() for d in showargs.split('--show') if d.strip()!='']
        uniques = reduce(lambda l, x: (x not in l and l.append(x)) or l, duples, [])
        showargs = ' '.join(uniques)

        return showargs
        

    def ParseArgument( self, const_arg ):
        self.const_arg = const_arg


        # first separate the three parts ('/' is separator) and strip the spaces around each
        findarg, showarg, otherarg = [x.strip() for x in (self.const_arg.split('/')+['',''])[0:3]]

        # check that show part starts with sh and remove the sh(ow)
        if showarg!='':
            if showarg[0:2] != 'sh':
                self.ParseError( "Show block must begin with 'sh(ow)'", 0 )
            showarg = showarg[showarg.index(' ')+1:]  # remove first word 'sh(ow)'
        else:
            if ' sh ' in findarg or ' sho ' in findarg or ' show ' in findarg:
                self.ParseError( "Show block must be separated by '/' from find condition'", 0 )

        # retrieve the query and show arguments (and check for input errors)
        queryarg = self.RetrieveQuery( findarg.strip() )
        shwarg   = self.RetrieveShow ( showarg.strip() )

        # add 'lhc' to show if 'olclumi' query
        # necessary because of stable beams information
        if '--olclumi' in queryarg and 'stablebeams' not in queryarg:
            queryarg += '--lhc "stablebeams TRUE" '

        shwarg   = self.PostProcessShowArgs( shwarg )

        # interpret other arguments
        nodef_flag = False
        oargs = otherarg.split()
        # always verbose for the time being...
        extraargs = {'verbose' : '--verbose'}
        idx = 0
        while idx < len(oargs):
            oa = oargs[idx]
            if oa == 'v':
                extraargs['verbose'] = '--verbose'
            elif oa == 'noroot':
                extraargs['noroot']  = '--noroot'
            elif oa == 'root':
                extraargs['root']  = '--root'
            elif oa == 'nohtml':
                extraargs['nohtml']  = '--nohtml'
            elif oa == 'html':
                extraargs['html']  = '--html'
            elif oa == 'utc':
                extraargs['utc']  = '--utc'
            elif oa == 'nogrl':
                extraargs['nogrl']  = '--nogrl'
            elif oa == 'grl' or oa == 'xmlfile':
                extraargs['xmlfile']  = '--xmlfile MyLBCollection.xml:MyLBCollection'
            elif oa == 'nodef' :
                nodef_flag = True
            elif idx>0 and (oargs[idx-1]=='grl' or oargs[idx-1]=='xmlfile'):
                extraargs['xmlfile']  = '--xmlfile %s' % oa
            else:
                print ("Extra argument '%s' unknown. Exiting." % oa)
                sys.exit(1)
            idx+=1

        if 'verbose' in extraargs:
            print ("Parser: '%s'" % self.const_arg)
            print ('  find argument: "%s"' % findarg.strip())
            print ('  show argument: "%s"' % showarg.strip())
            print ('  extra argument: "%s"' % otherarg.strip())

        # no defaults for MC
        if self.isMCDB:
            nodef_flag = True

        # add default arguments
        if not nodef_flag:
            for key, argument, defaultvalue in [ (x[0],x[1][1],x[1][5]) for x in self.queryArgs.items()]:
                if argument not in queryarg and defaultvalue != "":
                    queryarg += " --" + argument + ' "' + defaultvalue + '"'

        extraarg = ' '.join(extraargs.values())

        fullarg = '%s %s %s' % (queryarg, shwarg, extraarg)

        return fullarg


# command line driver for convenience
if __name__=='__main__':

    #from CoolRunQuery.AtlRunQueryParser import ArgumentParser
    ap = ArgumentParser()

    if len(sys.argv) <= 1:
        print ('No query argument given')
        sys.exit(1)

    if sys.argv[1].lower() == 'detmask':
        print ('Detector mask %s correspond to:\n%s' % (sys.argv[2], DecodeDetectorMask( int(sys.argv[2] ))))
        sys.exit()

    if sys.argv[1].lower() == 'help':
        ap.ParserUsage()
        sys.exit(1)
 
    atlqueryarg = ap.ParseArgument( ' '.join(sys.argv[1:]) )
    print ("\nAtlRunQuery.py %s\n" % atlqueryarg)

