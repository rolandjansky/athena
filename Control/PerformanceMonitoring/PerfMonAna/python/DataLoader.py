# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: PerfMonAna/python/DataLoader.py
# @purpose: handles various data formats and loads data from these files
# @author: Sebastien Binet <binet@cern.ch>

import shelve,os

class DataFormatHandler(object):
    pass # class DataFormatHandler

class DataHdlr_v000000(object):
    def __init__(self, fileName):
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
        object.__init__(self)
        self._fileName    = fileName
        self._tmpdir      = scratchDir
        self._outFileName = None

    def cnv(self):
        data = {}

        import tempfile,os
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
            tmpFileName = 'foo.bar'
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

        finally:
            os.chdir(wkdir)
        return data

    pass # class DataHdlr_v000100

class DataHdlr_v000200(object):
    """Data handler for mixed ROOT/TTree-python format. v00.02.00
    """
    def __init__(self, fileName, scratchDir):
        object.__init__(self)
        self._fileName    = fileName
        self._tmpdir      = scratchDir

    def cnv(self):
        data = {}
        import os,glob
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
            for k in db.iterkeys():
                data['meta'][k] = db[k]
            db.close()

            from PyRootLib import importRoot
            ROOT = importRoot()
            root = ROOT.fopen(glob.glob("*.root")[0], "read")
            for k in ('ini','evt','fin'):
                data[k] = root.Get("perfmon/%s"%k)
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

