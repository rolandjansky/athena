#! /usr/bin/python
# Q? Send mail to Tomasz dot Bold at cern dot ch

import string
from string import rstrip,atoi,replace
import sys
import commands
import os
import re

teIdtoName = {'0':'0'}
featureCLIDtoName = {}



    

if 'CMTROOT' not in os.environ.keys():
    print 'Can not find out graphviz binaries: ',GraphivizBinPath
    print 'Have you setup CMT environmet? CMTROOT not set'
    raise SystemExit

else:
    # setup lib
    GraphivizPath = os.environ['CMTROOT']+'/../Grafviz/2.2'
    GraphivizBinPath = GraphivizPath+'/bin/'
    GraphivizLibPath = GraphivizPath+'/lib/graphviz'

    if 'LD_LIBRARY_PATH' not in os.environ.keys():
        os.environ['LD_LIBRARY_PATH'] = ''
    os.environ['LD_LIBRARY_PATH'] = os.environ['LD_LIBRARY_PATH']+':'+GraphivizLibPath

    print "Graphviz programs from there: ", GraphivizBinPath

    
# global options driving grph drawing (switched by command line parser)
options =  {'drawFeatures': False,
            'clid': True,
            'convert': False,
            'program': 'dot',
            'event': None,
            'raw': False,
            'regex': re.compile('.*')}


def featureClassName(clid):
    """ converts the CLID to ClassName in C++. If falied then CLID instead.
        The CLIDS to name is taken from CLID Svc dump."""
    if clid in featureCLIDtoName.keys():
        return featureCLIDtoName[clid]
    else:
        if options['clid'] is False:
            return str(clid)
        else: # use CLID DB clid command to get it)
            name = commands.getoutput( 'clid '+clid).split(' ')[1]
            featureCLIDtoName[clid] = name
            return name

def teName(teid):
    if teid in teIdtoName.keys():
        return teIdtoName[teid]
    else:
        print "TE id: ", teid, " unknown"
        return teid
    
class TE:
    def __init__(self, s):
        # print s
        # this si typical line it has to deal with: id:0;active:1;error:0;ptr:0x9606b98;#seeds:4,0x9606ce0 0x9607338 0x96075b0 0x96077e8 #seededBy:0;#RoI:0;#features:1 CLID:242645981 idx:0:0:1 label:"OPI_L2";
        tok = s.split(';')
        #print tok
        self.tename = teName(tok[0].split(':')[1])
        self.active = bool(int(tok[1].split(':')[1]))
        self.error  = bool(int(tok[2].split(':')[1]))
        self.ptr    = tok[3].split(':')[1]
        self.roi    = tok[5].split(':')[1]
        # find out relations
        self.relations = []
        self.terminal = False
        seeds = tok[4].split(',')[0].split(':')[1]
        if seeds is '0':
            self.terminal = True
        else:
            self.relations = tok[4].split(',')[1].split(' ')[0:-1]
        
        # find out features attached
        #print tok
        self.features = []
        global options
        passed=False
        if options['drawFeatures']:
            for part in tok:
                if 'keyprints' in part:
                    passed=True
                clid = 0
                label = ''
                idx=''
                for field in  part.split(" "):
                    if field.find("CLID") is not -1:
                        clid = field.split(":")[1].lstrip(' ').lstrip(' ')
                    elif field.find("label") is not -1:
                        label = field.split(":")[1].rstrip("\"").lstrip("\"")
                    elif field.find("idx") is not -1:
                        idx=field
                if clid is not 0:
                    #print (featureClassName(clid), label, idx, passed)
                    self.features.append( (featureClassName(clid), label, idx, ('','*')[passed]) )
                    
        # print self.features
        
        # print self.tename, self.active, self.ptr, self.relations

    def node(self):
        """ Draws node of the graph. """
        global options
        
        if options['regex'].search(self.tename) == None and self.tename != '0':
            #            print "rejecting TE which of name ", self.tename, ' due to regex'
            return None
        
        nodename = 'X'+ str(self.ptr)
        line =  nodename +' [  label="{TE: ' + self.tename
        
        line += reduce(lambda x,y: x + "|"+y[0]+ "(" +y[1] + ' '+ y[2]+str(y[3])+")", self.features, "")
        line += '}", style=filled, '

        # special nodes 
        if self.tename is '0':
            line += 'fillcolor=pink, '            
        else:
            if self.active:
                line += 'color=green, '
            else:
                line += 'color=plum, '
            
            # error color scheme
            if self.error:
                line += 'fillcolor=red, '
            else:
                line += 'fillcolor=palegreen, '




        line += 'shape=record'
        line += ']\n'
        return line
    
    def edges (self):
        """ Draws node and edges. """
        line = ''
        for rel in self.relations:
            line += 'X'+self.ptr + ' -> X' + rel + '\n'
        return line

    def __str__(self):
        r = self.node()
        if r:
            return r + self.edges()
        else:
            return ""
    

def nav2dot(nav, eventId, level):
    """ converts"""
    global options
    inputTEs = nav.split('\n')
    stringTEs = []
    for te in inputTEs:
        if te is not '':
            stringTEs.append(te.lstrip(' |\_'))
        
    # unique
    uniqueTEs = []
    for ste in stringTEs:
        if ste not in uniqueTEs:
            uniqueTEs.append(ste)
    # print "LEN :" + str(len(stringTEs)) + "-->" + str(len(uniqueTEs))
    
    objectTEs = map( lambda x: TE(x), uniqueTEs)
    dot = 'digraph Menu  {\n'\
          +'\n'\
          +'graph [ rankdir = "TR"];'\
          +'node [ shape=polygon, fontname=Helvetica ]\n'\
          +'edge [ fontname=Helvetica ]\n'
    dot += reduce(lambda x,y: x+str(y), objectTEs, '')
    dot += '\n}'

    fname = 'navigationEvent-'+level+'-'+str(eventId)
    if options['event'] is None or str(options['event']) == str(eventId):
        print 'writing file: '+fname+'.dot'
        f = file('temp.dot','w')
        f.write(dot)
        f.close()
        #print commands.getoutput(GraphivizBinPath+'unflatten -l4 -f -c2 -o '+ fname+ '.dot temp.dot')
        print commands.getoutput('mv -f temp.dot '+fname+'.dot')
        print commands.getoutput('/bin/rm -f temp.dot')
        if options['convert']:
            print 'converting file: '+fname+'.dot to graphics'
            global GraphivizBinPath
            print commands.getoutput(GraphivizBinPath+options['program']+' ' + fname +'.dot -Tpng -o '+fname+'.png')
            # delete dot file
            print commands.getoutput('rm -f '+fname+'.dot')
# nav2dot(navex)

def parseLog(file):
    eventNumber = -1
    navigationStarted=False
    nav = ""
    step = 0
    stepToLevel =   ["L2", "EFunpacking", "EF"]     

    for line in file:
        if line.find("- --") is not -1:
            # print line
            if line.find("inputTEs") is not -1: # we spoted input TE in sequence printout
                try:
                    teId = line.split(" ")[5]
                    teId = string.strip(teId, "(), \"\n")
                    teId = rstrip(teId, ",")
                    teName = line.split(" ")[4]
                    teName = string.strip(teName, "(), \"\n")
                    teIdtoName[teId] = teName
                except:
                    pass
            if line.find(" outputTE   : (") is not -1:
                teId = line.split(" ")[7]
                teId = string.strip(teId, "() \n")
                teName = line.split(" ")[6]
                teName = string.strip(teName, "()\",\n")
                teIdtoName[teId] = teName

        # catch the navigation printout
        if line.find("start processing event #") is not -1:
            eventId = line.split("#")[1].split(",")[0]
            step=0
            print "scanning event: ",eventId


        if line.find("Start of HLT Processing in L2") is not -1:
            step = 0

        if line.find("Start of HLT Processing in EF") is not -1:
            step = 1

        if line.find("TrigSteer_EF.ResultBuilder") is not -1:
            step = 2
            
        # catch navigation block
        if line.find("\\_") is not -1:
            if navigationStarted is False:
                navigationStarted=True
            nav += line
        else:
            if navigationStarted is True:
              navigationStarted=False
              nav2dot(nav, eventId, stepToLevel[step] )

              nav = ""


def parseCLIDs():

    if options['clid'] is False:
        clids = file("CLIDDBout.txt")
        for line in clids:
            clid = line.split(" ")[0]
            name = line.split(" ")[1].rstrip("\n")
            featureCLIDtoName[clid] = name
    # print featureCLIDtoName



def parseOpt():
    global options
    stat = True
        
    if '-f' in sys.argv:
        options['drawFeatures'] = True
        print "OPTION: will draw features"

    if '-d' in sys.argv:
        options['clid'] = True
        print "OPTION: will use clid command to get class names (slower)"
    if '-x' in sys.argv:
        options['regex'] = re.compile(sys.argv[sys.argv.index('-x')+1])
        print "OPTION: will use only TEs accepted by regex", sys.argv[sys.argv.index('-x')+1]
    if '-e' in sys.argv:
        import string
        options['event'] = string.atoi(sys.argv[sys.argv.index('-e')+1])
        print "OPTION: will only draw event: ", options["event"]

    if '-c' in sys.argv:
        options['convert'] = True;
        print "OPTION: will convert to graphics on the fly"

    if '-p' in sys.argv:
        options['program'] = sys.argv[sys.argv.index('-p')+1]
        print "OPTION: will convert to graphics on the fly using ", options['program']

    if '-r' in sys.argv:
        options['raw'] = True
        options['configdump'] = sys.argv[sys.argv.index('-r')+2]
        options['navigationdump'] = sys.argv[sys.argv.index('-r')+1]
        
                
    if '-h' in sys.argv:
        print "OPTION: Help needed? Here it is:"
        print """
        Files generated by this utility can be viewed by programs like 'dotty'.
        Thay can be converted to graphics using probram 'dot':
        dot nav_event_123.dot -o nav123.png -Tpng
        
        -h               -- this help
        -f               -- draw freatures atteched to TE (with lables)
        -d               -- use clid command instead of CLIDDBout.txt to get human class names rather than CLIDs
        -e X             -- writeout only event (X)
        -x regex         -- use the regex to select only wanted TEs
        -c               -- convert graphs to png on the fly using dot program
        -p prog          -- use other program(neato, twopi, circo, fdp)
        -r navigationfile configfile              -- take files  with the navigation dump and the configuration dump (rather than plain log)
        """
        stat = False
    return stat
# main script

if parseOpt():
    if not options['raw']:
        logname = sys.argv[-1]
        log = file(logname)
        parseCLIDs()
        parseLog(log)
    else:
        conflog = file(options['configdump'])
        parseLog(conflog)
        navlog = file(options['navigationdump'])
        parseLog(navlog)
        
    #print teIdtoName
