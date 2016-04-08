from FileGrepper import FileGrepper
import os.path

class LogFileGrepper(FileGrepper):
    def __init__(self, argdict={}):
        argdict['inputFilePattern']  = argdict['JobDescriptor'].log
        argdict['outputFile'] = '%s.grep_output' % argdict['JobDescriptor'].log
        FileGrepper.__init__(self, argdict)

    def run(self):
        return FileGrepper.run(self)

if __name__ == '__main__':
    class Paths:
        def __init__(self):
            self.RTTSrcDir = '/afs/cern.ch/atlas/project/RTT/data/brinick/Tools.CURRENT/RunTimeTester/src'

    class Desc:
        def __init__(self):
            self.paths = Paths()
            self.log  = 'log.log'
            self.runPath = '/afs/cern.ch/atlas/project/RTT/data/brinick/Tools.CURRENT/RunTimeTester/share'
            
    d = {'JobDescriptor': Desc(),
         'searchList': ['leaving with code 0', 'ErrorCode=0 \(OK\)', 'successful run'],
         'vetoList'  : ['FATAL', 'Core dump', 'stack trace', 'Shortened traceback']}
    
    fg = LogFileGrepper(d)
    print fg.run()
