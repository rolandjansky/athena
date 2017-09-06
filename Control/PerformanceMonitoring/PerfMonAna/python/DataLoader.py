# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PerfMonAna/python/DataLoader.py
# @purpose: handles various data formats and loads data from these files
# @author: Sebastien Binet <binet@cern.ch>

import shelve,os,tempfile,glob,atexit,shutil
import cppyy

class DataFormatHandler(object):
    pass # class DataFormatHandler

class DataHdlr_v000000(object):
    def __init__(self, fileName):
        import cppyy
        object.__init__(self)
        self._fileName    = fileName
        self._outFileName = None

    def cnv(self):
        return
    
    pass # class DataHdlr_v000000

class DataHdlr_v000100(object):
    """Data handler for pure python format. v00.01.00
    """
    def __init__(self, fileName, scratchDir):
        import cppyy
        object.__init__(self)
        self._fileName    = fileName
        self._tmpdir      = scratchDir
        self._outFileName = None

    def cnv(self):
        data = {}

        import tempfile,os,glob
        import gzip
        import shutil

        wkdir  = os.getcwd()
        tmpdir = tempfile.mkdtemp()
        global _holder
        _holder.toRemove += [tmpdir]
        try:
            os.chdir(tmpdir)
            f = open(os.path.abspath(self._fileName), 'r')
            if self._fileName.endswith('.gz'):
                f = gzip.GzipFile(fileobj=f)
            f.seek(0)
            tmpFile = open(tmpFileName, 'w')
            shutil.copyfileobj(f, tmpFile)
            f.close()
            tmpFile.close()

            import imp, inspect
            tmpFile = open(tmpFileName, 'r')
            mod = imp.load_source( '_data_loader', '_data_loader.py', tmpFile)
            tmpFile.close()
            os.remove( tmpFileName )
            def _fctLoader(obj):
                if inspect.isfunction(obj):
                    return obj.__name__ == 'getData'
            _data = inspect.getmembers(mod, _fctLoader)[0][1]
            _data = _data()

            data['meta'] = {
                'components'   : _data['meta://components'],
                'iocontainers' : _data['meta://iocontainers'],
                }
            compNames = set(_data['components'].keys() +
                            _data['iocontainers'])

            from PyRootLib import importRoot
            ROOT = importRoot()
            root = ROOT.fopen(
                os.path.join([tmpdir,self._fileName+".root"]),
                "recreate"
                )
            
##         dataSetName = self.name
## ##         print ">>>",len(compNames),len(data.keys())
##         _data_keys = data.keys()
##         for compName in compNames:
## ##             print ":::::::::",compName
##             monComp = MonitoredComponent(compName, dataSetName)
##             monData = monComp.data[dataSetName]
##             for storeName in storeNames:
## ##                 print compName,storeName
##                 if not monData.has_key(storeName):
##                     monData[storeName] = {}
##                 compNameHdr1 = compName + '://'  + storeName
##                 compNameHdr2 = compName + ':///' + storeName
##                 for k in _data_keys:
##                     if k.startswith( compNameHdr1 ) or \
##                        k.startswith( compNameHdr2 ):
##                         monKey = k[k.find(storeName+'/')+len(storeName)+1:]
##                         monData[storeName][monKey] = numpy.array(data[k])
##                 if storeName == 'evt' and  monData[storeName].has_key('evtNbr'):
##                     self.bins = monData[storeName]['evtNbr']
##                 pass
##             pass
##         _monitor('5')

##         _compsDb = data['meta://components'  ]
##         _comps   = data['meta://components'  ].keys()
##         _ioconts = data['meta://iocontainers']
##         for monComp in MonitoredComponent.instances.values():
##             if monComp.type != None:
##                 continue
##             monName = monComp.name
##             if   monName in _comps   : monComp.type = _compsDb[monName]
##             elif monName in _ioconts : monComp.type = 'io'
##             else                     : monComp.type = 'usr'
##             pass

        finally: os.chdir(wkdir)
        return data
    
    pass # class DataHdlr_v000100

class DataHdlr_v000200(object):
    """Data handler for mixed ROOT/TTree-python format. v00.02.00
    """
    def __init__(self, fileName, scratchDir):
        import cppyy
        object.__init__(self)
        self._fileName    = fileName
        self._tmpdir      = scratchDir

    def cnv(self):
        data = {}
        import tempfile,os,glob
        origdir = os.getcwd()
        tmpdir  = self._tmpdir
        try:
            os.chdir(tmpdir)
            import tarfile
            tar = tarfile.open(os.path.abspath(self._fileName), 'r')
            tar.extractall(path=tmpdir)
            tar.close()

            fname = glob.glob("*.dat")[0]
            if open(fname, 'r').read(1024).startswith('SQLite format'):
                import PyUtils.dbsqlite as dbs
                db = dbs.open(fname, 'r')
            else:
                db = shelve.open(fname)
            data['meta'] = {}
            for k in db.iterkeys(): data['meta'][k] = db[k]
            db.close()
            
##             print "version:",data['meta']['version_id']

            from PyRootLib import importRoot
            ROOT = importRoot()
            root = ROOT.fopen(glob.glob("*.root")[0], "read")
            for k in ('ini','evt','fin'): data[k] = root.Get("perfmon/%s"%k)
            data['meta']['rootFile'] = root
        finally:
            os.chdir(origdir)
            
        return data
    
    pass # class DataHdlr_v000200


class DataLoader(object):
    def __init__(self, fileName):
        super(DataLoader, self).__init__()
        self.fileName  = os.path.abspath(fileName)
        self._dataHdlr = None

    def load(self):
        import PerfMonComps.PerfMonSerializer as pmon_ser
        infos, data = pmon_ser.pmon_load(self.fileName)
        return {'meta':infos,
                'data':data}
    
    pass # class DataLoader

