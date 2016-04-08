# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from listFiles import listFiles
import os.path, sys, getopt
from findMostRecentLogDir import findMostRecentLogDir

def exceptionTypes():
    import exceptions
    return [e for e in dir(exceptions) if e.find('__')==-1]

def stripstr(s):
    return s.strip()[:-1]

class Checker:
    def __init__(self, strings):
        self.strings = strings
        self.dict = {}
        self.fdict = {}
        self.sdict = {}
        for s in self.strings:
            self.dict[s]=0
            self.fdict[s]=[]

        self.ncalls = 0
        
    def checkForStrings(self,f):
        self.ncalls += 1
        lines = open(f,'r').readlines()
        self.sdict[f] = []
        for s in self.strings:
            for l in lines:
                # print "\n looking for:\n%s\n in line\n %s" %(s,l)
                if l.find(s)>= 0:
                    self.dict[s]+=1
                    if f not in self.fdict[s]: self.fdict[s].append(f)
                    self.sdict[f].append(s)



def logFileChecker(root, globpar, strings):

    res =  'looking for files below %s matching %s\n' % (root, globpar)
    logfiles = listFiles(root, globpar)
    res +=  'Found %d log files\n' % len(logfiles)

    checker = Checker(strings)

    [checker.checkForStrings(f) for f in logfiles]

    res +=  '\nerror strings:\n'
    dict = checker.dict

    res += 'Occurences   text\n'

    for s in dict:
        if dict[s]>0: res += '%10d   %s\n' % (dict[s], s)

    res += '\n Strings searched for:\n'
    for s in dict.keys(): res += s+'\n'

    for s in checker.fdict.keys():
        if len(checker.fdict[s]):
            res +=  '\n\n"%s" was found in %d files:\n' % (s, len(checker.fdict[s]))
            for f in checker.fdict[s]: res +=  f+'\n'
        else:
            res +=  '\n"%s" was found in no files\n\n' % s

    res +=  '\n\n%d files with no strings:\n\n' % (checker.sdict.values()).count([])

    for f in checker.sdict.keys():
        if not checker.sdict[f]: res += f+'\n'

    return res

def readConfig(config):

    lines  = open(config, 'r').readlines()

    toRemove = [l for l in lines if l.startswith('#')]
    [lines.remove(l) for l in toRemove]
    lines = [stripstr(l) for l in lines]
    toRemove = [s for s in lines if len(s)==0]
    [lines.remove(l) for l in toRemove]

    return lines

if __name__ == '__main__':

    def usage():
        print
        print 'logFileChecker -- checks for strings in files'
        print
        print 'usage: python logFileChecker [options]'
        print
        print 'Options'
        print '   -c filename name of file with dir, glob parameters and strings. Mandatory'
        print '   -l use the most recent version of the log files. must supply arg branch or branch:os'
        print '      possible branch = ("bugfix", "dev", "val"), possibe os = ("slc3", "slc4")'
        print '      example: -l dev or -l dev:slc3'
        print '      if ommitted, the path to the log files will be taken from the config file.'
        print '   -f triggers writing to a file. Filename is written to stdout'
        print '   -u set the user, eg  brinick, eric, peter, prod, Default prod'
        print '   -h print this message and exit'
        return
    
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'c:b:l:fu:h')
        print opts
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    useLatest = False
    config    = None
    slc       = None
    output    = None
    branch    = None
    user      = 'prod'
    for o, a in opts:
        if o == '-l':
            tokens = a.split(':')
            
            branch = tokens[0]
            if branch not in ['bugfix', 'dev', 'val']:
                
                usage()
                sys.exit(2)
                
            if len(tokens)>1:
                opsys = tokens[1]
            if opsys not in ['slc3', 'slc4']:
                usage()
                sys.exit(2)
                
            if opsys == 'slc3':
                opsys = 'SLC3'
            elif opsys == 'slc4':
                opsys = 'SLC4_32'


        elif o == '-u':
            user = a+'/logs'
            if user != 'prod': user = 'data/'+user
        elif o == '-h':
            usage()
            sys.exit(0)
        if o == '-c':
            config = a
        if o == '-f':
            output = '/afs/cern.ch/user/r/rtt/Scratch'

    if not config:
        print '-c option required'
        usage()
        sys.exit(2)
    if not os.path.exists(config):
        print
        print 'unknown config file',config,' exiting'
        sys.exit(2)

    # config = open(sys.argv[1], 'r')
    lines = readConfig(config)
    if branch:
        root = '/afs/cern.ch/atlas/project/RTT/%s/logs/%s/%s/logs' % (user, branch, opsys)

        print 'looking for most recent log file directory below %s' %  root
        root = findMostRecentLogDir(root)
        print '  ... found %s' %  root
    else:
        root = lines[0]


    globpar = lines[1]
    strings = lines[2:]

    if not os.path.exists(root):
        print 'Balking as root directory does not exist:\n%s' % root
        sys.exit(0)


    if output:
        cname = config.split('.')[0]
        cname += '_'+os.path.basename(root)+'.log'
        output = os.path.join(output, cname)
        of = open(output,'w')
        print 'writing results to ', output
    else:
        of = sys.stdout


    of.write(logFileChecker(root, globpar, strings))
