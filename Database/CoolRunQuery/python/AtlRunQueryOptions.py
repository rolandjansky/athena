# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from optparse import OptionParser

class AtlRunQueryOptions:
    def __init__(self):
        self.parser = OptionParser(usage="usage: %prog options")
        self.parser.add_option( "-r",
                                "--run",
                                action = 'append',
                                dest = "runlist",
                                help = "A list of run ranges 90923,89500-89600,92050-" )
        
        self.parser.add_option( "-t",
                                "--time",
                                action="append",
                                dest = "timelist",
                                help = "A list of time ranges 10.9.2008-30.9.2008,15.10.2008-" )

        self.parser.add_option( "-p",
                                "--partition",
                                dest = "partition",
                                help = "The partition name [default: %DEFAULT]" )

        self.parser.add_option( "--readyforphysics",
                                dest = "readyforphysics",
                                help = "ReadyForPhysics flag" )

        self.parser.add_option( "--db",
                                dest = "database",
                                default = 'CONDBR2', # default doesn't make sense, the DB should be picked based on run number
                                choices = ['COMP200', 'CONDBR2', 'OFLP200', 'DATA', 'MC'],
                                help = "The detabase [default: %DEFAULT]" )
        
        self.parser.add_option( "-c",
                                "--ctag",
                                dest = "condtag",
                                help = "The top conditions tag" )
        
        self.parser.add_option( "-l",
                                "--duration",
                                dest = "duration",
                                help = "The duration limit of a run in seconds e.g.: -l 36000 (longer than 10 hours) or -l 300- (less than 5 minutes)" )
        
        self.parser.add_option( "-e",
                                "--events",
                                dest = "events",
                                help = "Number of events (EF accepts) in run e.g. 10000-20000" )

        self.parser.add_option( "--allevents",
                                dest = "allevents",
                                help = "Shows number of EFA/SFO/L1A/L2A events in selected runs e.g. 10000-20000" )

        self.parser.add_option( "-k",
                                "--smk",
                                dest = "smklist",
                                help = "A list of smks 359,365,372-" )
        
        self.parser.add_option( "-b",
                                "--bfield",
                                action = "append",
                                choices = ['on', 'off', 'solenoidon', 'solenoidoff', 'toroidon', 'toroidoff'],
                                dest = "bfield",
                                help = "Bfield ['on', 'off', 'solenoidon', 'solenoidoff', 'toroidon', 'toroidoff']" )
        
        self.parser.add_option( "-q",
                                "--dq",
                                action = "append",
                                dest = "dqchannels",
                                help = "Data quality flags, e.g. --dq 'PIXB green SHIFTOFL'" )
        
        self.parser.add_option( "--detmaskin",
                                action = "append",
                                dest = "detmaskin",
                                help = "Detector mask, e.g. --detmaskin 0x3123 " )

        self.parser.add_option( "--detmaskout",
                                action = "append",
                                dest = "detmaskout",
                                help = "Detector mask of absent detectors, e.g. --detmaskout 0x2174 " )
        
        self.parser.add_option( "--filenametag",
                                dest = "projecttag",
                                help = "Filename tag, e.g. --filenametag !data_test " )
        
        self.parser.add_option( "--projecttag",
                                dest = "projecttag",
                                help = "Project tag, e.g. --projecttag !data_test " )

        self.parser.add_option( "-s",
                                "--show",
                                action = "append",
                                dest = "show",
                                help = "List of items to query" )
        
        self.parser.add_option( "--streams",
                                action = "append",
                                dest = "streams",
                                help = 'Streams, --streams "phy*" ' )
        
        self.parser.add_option( "--trigger",
                                action = "append",
                                dest = "trigger",
                                help = 'Triggers, --trigger "EF_Electron*" ' )
        
        self.parser.add_option( "--release",
                                action = "append",
                                dest = "release",
                                help = 'Release, --release "14.2.5-14.3.1" ' )

        self.parser.add_option( "--luminosity",
                                action = "append",
                                dest = "luminosity",
                                help = 'Luminosity, --luminosit' )

        self.parser.add_option( "--olclumi",
                                action = "append",
                                dest = "olclumi",
                                help = 'olclumi, --olclumi "1ub+"' )

        self.parser.add_option( "--olcfillparams",
                                action = "append",
                                dest = "olcfillparams",
                                help = 'olcfillparams, --olcfillparams "channel"' )

        self.parser.add_option( "--beamspot",
                                action = "append",
                                dest = "beamspot",
                                help = 'Beamspot, --beamspot' )

        self.parser.add_option( "--bpm",
                                action = "append",
                                dest = "bpm",
                                help = 'BPM, --bpm' )

        self.parser.add_option( "--datasets",
                                action = "append",
                                dest = "datasets",
                                help = 'Datasets, --datasets *NTUP*' )

        self.parser.add_option( "--ami",
                                action = "append",
                                dest = "ami",
                                help = 'Ami, --ami' )

        self.parser.add_option( "--larcond",
                                action = "append",
                                dest = "larcond",
                                help = 'Larcond, --larcond "nsamples 7"' )

        self.parser.add_option( "--lhc",
                                action = "append",
                                dest = "lhc",
                                help = 'LHC information, --lhc "fillnumber 851"' )

        self.parser.add_option( "--summary",
                                action = "count",
                                dest = "summary",
                                help = 'Print summary, --summary ' )

        self.parser.add_option( "--dqeff",
                                action = "count",
                                dest = "dqeff",
                                help = 'Print DQ efficiency summary, --dqeff ' )

        self.parser.add_option( "-v",
                                "--verbose",
                                action = "count",
                                dest = "verbose",
                                help = "Verbosity level" )

        self.parser.add_option( "--noroot",
                                action = "store_false",
                                dest = "dictroot",
                                help = "Do not create root files and python dict" )

        self.parser.add_option( "--root",
                                action = "store_true",
                                dest = "dictroot",
                                help = "Do create root files and python dict" )

        self.parser.add_option( "--nohtml",
                                action = "store_false",
                                dest = "html",
                                help = "Text output instead of html" )

        self.parser.add_option( "--html",
                                action = "store_true",
                                dest = "html",
                                help = "Text output instead of html" )

        self.parser.add_option( "--selectOnUnknown",
                                action = "store_true",
                                dest = "selectonunknown",
                                default = True,
                                help = "Select when information is not known (only for #evt and partition name)" )

        self.parser.add_option( "--skipUnknown",
                                action = "store_false",
                                dest = "selectonunknown",
                                help = "Select when information is not known (only for #evt and partition name)" )

        self.parser.add_option( "--xmlfile",
                                action = "store",
                                type = "string",
                                dest = "xmlfile",
                                help = "Give filename and label (format: filename:label) for XML GoodRunList file" )

        self.parser.add_option( "--nogrl",
                                action = "store_false",
                                dest = "prodgrl",
                                help = "Do not produce a good run list" )

        self.parser.add_option( "--utc",
                                action = "store_true",
                                dest = "utc",
                                help = "Use utc for everything, default is local time (CET/CEST)" )
        
        self.parser.add_option( "--dqsumgrl",
                                action = "store",
                                dest = "dqsumgrl",
                                #default = "PHYS_StandardGRL_All_Good_25ns",
                                help = "Define tolerable/intolerable defects for DQ summary relative to GRL or other virtual defect, default is PHYS_StandardGRL_All_Good_25ns")
        
        self.parser.add_option( "--defecttag",
                                action = "store",
                                dest = "defecttag",
                                #default = "HEAD",
                                help = 'Define defect tag for defect database used for DQ summary relative to GRL or other virtual defect, default is "HEAD"')
        
        self.parser.add_option( "--logictag",
                                action = "store",
                                dest = "logictag",
                                #default = "HEAD",
                                help = 'Define logic tag for defect database used for DQ summary relative to GRL or other virtual defect, default is "HEAD"')
    
    def splitCmdline(self,argstring):
        arg = argstring.split()
        m = list(zip([x for x in range(len(arg)) if arg[x][0]=='"'],[x for x in range(len(arg)) if arg[x][-1]=='"']))
        m.reverse()
        for p in m:
            if p[0]==p[1]:
                arg[p[0]] = arg[p[0]].strip('"')
            elif p[1]-p[0] == 2:
                arg[p[0]:p[1]+1]=[("%s %s %s" % (arg[p[0]],arg[p[0]+1],arg[p[1]])).strip('"')]
            else:
                arg[p[0]:p[1]+1]=[("%s %s" % (arg[p[0]],arg[p[1]])).strip('"')]
        return arg


    def parse(self,atlqueryarg=None):
        if atlqueryarg:
            sys.argv = self.splitCmdline(atlqueryarg)
        return self.parser.parse_args()
