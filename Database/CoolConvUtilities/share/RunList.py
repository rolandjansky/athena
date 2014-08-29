#!/usr/bin/env python
# RunList.py
# list run information from ATLAS COOL online information
# Richard Hawkings, started 02/9/07
#
# syntax: RunList.py {<options>}
# do RunList.py --help for instructions

import sys,getopt
from CoolConvUtilities import AtlCoolLib
from CoolConvUtilities.RunLister import coolRunLister

class runListDriver:
    def __init__(self):
        # defaults for optional parameters
        # database instances for M4 onwards
        self.tdaqdbname='COOLONL_TDAQ/COMP200'
        self.trigdbname='COOLONL_TRIGGER/COMP200'
        self.statusdbname='COOLOFL_GLOBAL/COMP200'
        self.oracle=False
        self.runmin=0
        self.runmax=0x7FFFFFFF
        self.usetime=False
        self.tsmin=0
        self.tsmax=(1 << 63)-1
        self.loglevel=1
        self.listErr=False
        self.reconly=False
        self.detmask=0
        self.runtype=""
        self.detstatus=""
        self.detstatustag="HEAD"
        self.tag=""
        self.webfile=""
        self.format=""
        self.name=""
        self.reverse=False
        # process command line options and switches
        try:
            longopts=['tdaqdb=','trigdb=','statusdb=','r=','rs=','ru=','ts=','tu=','reconly','detmask=','runtype=','detstatus=','detstatustag=','filenametag=','webfile=','format=','name=','readoracle','reverse','listerr','debug','quiet','help']
            opts,args=getopt.getopt(sys.argv[1:],'',longopts)
        except getopt.GetoptError,e:
            print e
            self.usage()
            sys.exit(-1)
        self.procopts(opts)
        self.execute()

    def usage(self):
        print
        print "usage: RunList {<options>}"
        print "Options are:"
        print "--tdaqdb=<COOL DB instance> : Set DB instance for TDAQ info"
        print "--trigdb=<COOL DB instance> : Set DB instance for TRIGGER info"
        print "--statusdb=<COOL DB instance> : Set DB instance for detStatus info"
        print "--readoracle: Force queries to use Oracle replica"
        print "--r=<run> : Set limits for a single run"
        print "--rs=<run> : Set lower run limit"
        print "--ru=<run> : Set upper run limit"
        print "--ts=<time> : Initial timestamp (seconds or 2007-14-13:14:31:01)"
        print "--tu=<time> : Final timestamp (seconds or 2007-14-13:14:31:01)"
        print "--reconly : Select runs with recording enabled only"
        print "--detmask=<mask> : Select runs with specifed mask bits (in hexidecimal)"
        print "--runtype=<runtype> : Select runs of specified runtype only"
        print "--filenametag=<tag> : Select runs of specified filename tag only"
        print "--detstatus=\"<status string>\" : Make detector status cuts"
        print "--detstatustag=tagval : Make detector status cuts"
        print "--format=<format>: format code for output (from 'acdert')"
        print "--listerr : List out data errors/inconsistencies"
        print "--webfile=<file> : Output html on file instead of screen"
        print "--name=<name> : Name for output information on web"
        print "--reverse : Reverse sorting of runs (latest run first)"
        print "--debug : Produce debug printout"
        print "--quiet : Produce minimum printout (can then pipe output)"
        print
        print "Lists run information from COOL online data produced by Run Control"
        print "and central trigger processor, to text or html file"

    def procopts(self,opts):
        "Process the command line parameters"
        for o,a in opts:
            if (o=='--tdaqdb'): self.tdaqdbname=str(a)
            if (o=='--trigdb'): self.trigdbname=str(a)
            if (o=='--statusdb'): self.statusdbname=str(a)
            if (o=='--readoracle'): self.oracle=True
            if (o=='--r'):
                self.runmin=int(a)
                self.runmax=self.runmin
            if (o=='--rs'): self.runmin=int(a)
            if (o=='--ru'): self.runmax=int(a)
            if (o=='--ts'):
                self.tsmin=AtlCoolLib.timeVal(a)*1000000000L
                self.usetime=True
            if (o=='--tu'):
                self.tsmax=AtlCoolLib.timeVal(a)*1000000000L
                self.usetime=True
            if (o=='--reconly'):
                self.reconly=True
            if (o=='--detmask'):
                self.detmask=int(a,16)
            if (o=='--runtype'):
                self.runtype=str(a)
            if (o=='--detstatus'):
                self.detstatus=str(a)
            if (o=='--detstatustag'):
                self.detstatustag=str(a)
            if (o=='--filenametag'):
                self.tag=str(a)
            if (o=='--webfile'):
                self.webfile=str(a)
            if (o=='--format'):
                self.format=str(a)
            if (o=='--reverse'):
                self.reverse=True
            if (o=='--name'):
                self.name=str(a)
            if (o=='--listerr'): self.listErr=True
            if (o=='--debug'): self.loglevel=2
            if (o=='--quiet'): self.loglevel=0
            if (o=='--help'):
                self.usage()
                sys.exit(0)
                
    def execute(self):
        # if status is not being used, reset DBname to avoid opening connection
        if (self.detstatus==""): self.statusdbname=""
        runlisttool=coolRunLister(self.tdaqdbname,self.trigdbname,self.statusdbname,self.oracle,self.loglevel)
        runlisttool.setSelection(self.reconly,self.detmask,self.runtype,self.tag,self.detstatus,self.detstatustag)
        if (self.usetime):
            runlisttool.listFromTime(self.tsmin,self.tsmax)
            runlisttool.close()
        else:
            runlisttool.listFromRuns(self.runmin,self.runmax)
            runlisttool.close()
        if len(self.webfile)>0:
            runlisttool.makeWebSummary(self.webfile,self.name,self.reverse)
        else:
            runlisttool.listRuns(self.format,self.reverse)
        if (self.listErr):
            runlisttool.listErrors()
            
        
# main code
if __name__=='__main__':
    runListDriver()
    
