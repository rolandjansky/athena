######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannot shelve the objects                #
# Instead, unload the necessary variables in from    #
# more complicated classes in __init__               #
#                                                    #
######################################################



import sys, os, os.path, glob, re, types, string
from Logger import Logger
from sets import Set

class FileGrepper:
    
    def __init__(self, paramDict={}):
        jDescriptor = paramDict['JobDescriptor']

        self.runPath = jDescriptor.runPath
        self.srcDir  = jDescriptor.paths.RTTSrcDir
        if self.srcDir not in sys.path: sys.path.append(self.srcDir)

        self.success   =  0
        self.error     = -1
        self.logger    = Logger()
                
        self.iFilePat = paramDict.get('inputFile', '').strip()
        if not self.iFilePat: return        

        self.illegalOutputFileName = False
        self.outFile       = paramDict.get('outputFile', '')
        if self.outFile:
            ok_chars = string.digits + string.ascii_letters + '_-.+'
            if [t for t in self.outFile if t not in ok_chars]:
                self.illegalOutputFileName = True
                return
            self.outFile = os.path.join(self.runPath, self.outFile)

        search = paramDict.get('searchList', [])
        if search: search = [s.strip() for s in search.split(',')]

        veto = paramDict.get('vetoList', [])
        if veto: veto = [v.strip() for v in veto.split(',')]

        search.extend(paramDict.get('searchStrings', []))
        veto.extend(paramDict.get('vetoStrings', []))

        self.searchDict    = self.dictify(search)
        self.vetoDict      = self.dictify(veto)
        self.findAllSearch = self.boolify(paramDict.get('mustFindAllSearchTerms', True))
        self.sendToFile    = paramDict.get('outputLinesMatching', '')
        self.verbose       = self.boolify(paramDict.get('verbose', True))
        
    # ========================================================================
    # ========================================================================
    # ========================================================================

    def dictify(self, list_):
        if type(list_)==type(''):
            list_ = list(list_)
        d = {}
        [d.setdefault(l, []) for l in list_] # uniquify search/veto strings (dict keys)
        return d

    def boolify(self, string_):
        if type(string_)!=type(''):
            string_ = str(string_)
        return {'False':False}.get(string_, True)
        
    def run(self):
        if not self.iFilePat:
            m = '%s ==> no input file pattern to match was given. Abandoning.' % self.__class__.__name__
            self.logger.error(m)
            return self.error

        if not self.outFile:
            m = "%s ==> no output file was given. Abandoning." % self.__class__.__name__
            self.logger.error(m)
            return self.error

        if self.illegalOutputFileName:
            m = '%s ==> Illegal output file name: %s' % (self.__class__.__name__, self.outFile)
            self.logger.error(m)
            return self.error
        
        def print_(file_, fileBad, failureReason, dict, type_):
            matches = [(n,m) for n,m in dict.items() if m]
            if not matches: return ''
            output = ''
            if self.verbose:
                output  = '--------------------------------------------------------------------\n'
                output += 'File: %s\n' % file_
                output += '--------------------------------------------------------------------\n'
                output += '%s\n' % {False: 'STATUS: PASS', True: 'STATUS: FAIL'}[fileBad]
                if fileBad:
                    output += 'This file FAILed for the following reason:\n'
                    output += '%s\n\n' % failureReason
                output += '%d lines match a %s term. '  % (reduce(lambda x,y:x+y, [len(m) for n,m in matches],0), type_)
                output += '%d (of %d total) %s terms were matched.\n' % (len(matches), len(dict.keys()), type_)
                # output += 'All search terms must be matched: %s\n' % self.findAllSearch
                output += '--------------------------------------------------------------------\n'
            for n, m in matches:
                if self.verbose:
                    output += '%s term %s matched the following %d lines:\n' % (type_, n, len(m))
                    for mm in m: output += '\t%s\n' % mm.strip()
                else:
                    for mm in m: output += '%s\n' % mm.strip()
                        
            return output
                
        self.outFile = os.path.join(self.runPath, self.outFile)
        inFiles = self.getInFiles()
        if not inFiles: return self.error

        success = True
        output  = ''        
        
        for file_, ((fileBad, failureReason), searchDict, vetoDict) in [(inf, self.processInfile(inf)) for inf in inFiles]:

            success = success and not fileBad
            if self.sendToFile == 'veto':
                output += print_(file_, fileBad, failureReason, vetoDict, 'veto')
            elif self.sendToFile == 'search':
                output += print_(file_, fileBad, failureReason, searchDict, 'search')
            else: # send both veto and search matches to file
                output += print_(file_, fileBad, failureReason, vetoDict, 'veto')
                output += print_(file_, fileBad, failureReason, searchDict, 'search')

        h = open(self.outFile, 'w')
        h.write(output)
        h.close()

        return {True:self.success, False:self.error}[success]

    # ========================================================================
    # ========================================================================
    # ========================================================================
        
    def getInFiles(self):
        pattern = os.path.join(self.runPath, self.iFilePat)
        matches = [m for m in glob.glob(pattern) if os.path.isfile(m)]
        if not matches:
            m = '%s => No matches found for the in-file pattern: %s\n' % (self.__class__.__name__, self.iFilePat)
            self.logger.warning(m)
        return matches

    # ========================================================================
    # ========================================================================
    # ========================================================================

    def processInfile(self, inFile): 
        """Return list of matching lines from the in file."""
        def readfile():
            handle = open(inFile)
            for line in handle.readlines(): yield line
            handle.close()

        def matchline(sdict, vdict, line):
            line = line.strip()
            slist = [s for s in sdict.keys() if re.search(s, line)]
            vlist = [v for v in vdict.keys() if re.search(v, line)]
            [sdict[s].append(line) for s in slist]
            [vdict[v].append(line) for v in vlist]            
            return slist or vlist
        def foundAllSearchTerms(sdict):
            return len([sv for sv in sdict.values() if sv])==len(sdict.keys())
        def foundSomeSearchTerms(sdict):
            return len([sv for sv in sdict.values() if sv])>0
        def fileBad(sdict,vdict):
            vetofound = [vv for vv in vdict.values() if vv]
            if len(vetofound)>0:
                return (True, '%d veto terms were found' % len(vetofound))
            if self.findAllSearch and not foundAllSearchTerms(sdict):
                return (True, 'Not all search terms matched, and they should have been.')
            if not foundSomeSearchTerms(sdict):
                return (True, 'No search terms matched at all.')
            return (False, '')
            
        s = {}
        v = {}
        s.update(self.searchDict)
        v.update(self.vetoDict)

        # fill the dicts
        [matchline(s,v,line) for line in readfile()]
        return (fileBad(s,v), s, v)

    
if __name__ == '__main__':
    class Paths:
        def __init__(self):
            self.RTTSrcDir = '/afs/cern.ch/atlas/project/RTT/data/brinick/Tools.CURRENT/RunTimeTester/src'

    class Desc:
        def __init__(self):
            self.paths = Paths()
            self.runPath = '/afs/cern.ch/atlas/project/RTT/data/brinick/Tools.CURRENT/RunTimeTester/share'
            
    d = {'JobDescriptor': Desc(),
         'inputFile' : 'a ',# 'bl?h/log.log',
         'outputFile': 'log.log.output', 
         'searchStrings': ['leaving with code 0', 'ErrorCode=0 \(OK\)', 'successful run'],
         'searchList'   : 'ErrorCode=0 \(OK\),    leaving with code 0',
         'vetoStrings'  : ['FATAL', 'Core dump', 'stack trace', 'Shortened traceback'],
         'mustFindAllSearchTerms': 'bugger',
         #'outputLinesMatching': 'veto',
         # 'verbose': 'True'
         }
    
    fg = FileGrepper(d)
    print fg.run()
    print fg.logger.errorL
    print fg.logger.debugL
    print fg.logger.infoL
    print fg.logger.warningL
