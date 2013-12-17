# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#---------------------------------------------------------------------
#
# Code is under development - updates might break your code
#
#---------------------------------------------------------------------

import logging, sys, re
    
def getLog(name, level = logging.INFO):

    f = logging.Formatter("Py:%(name)s: %(levelname)s - %(message)s")
    h = logging.StreamHandler()
    h.setFormatter(f)
    
    log = logging.getLogger(name)
    log.setLevel(level) 
    log.addHandler(h)

    return log

plog = getLog('TrigCostBaseConfig')

#---------------------------------------------------------------------
#
# Module loader class
#
def loadTrigCostBase():
    try:
        import ROOT, PyCintex
        ROOT.gROOT.SetBatch(True)    
    except:
        log.error('Failed to import ROOT')
        sys.exit(-1)
        
    try:
        ROOT.gSystem.Load('libTrigCostBase')
        ROOT.gSystem.Load('libTrigCostBaseDict')
        from ROOT import Anp
    except:
        rlog.error('Failed to load TrigCostBase library')
        sys.exit(-1)

#---------------------------------------------------------------------
#
# Search directory for matching files
#
def SearchInputDir(target, key):
    #
    # Search "target" for files with name matching "key"
    #
    import os
    files = []


    def _matchKey(_val, _key):
        if type(_val) != str:
            return False
        
        if type(_key) == list:
            for i in range(0, len(_key)):
                if type(_key[i]) == str and (len(_key[i]) < 0 or _val.count(_key[i])):
                    return True
        elif type(_key) == str:
            if len(_key) < 0 or _val.count(_key):
                return True
        return False
                    
    
    if os.path.isfile(target):
        #
        # "target" is physical file
        # 
        if _matchKey(target, key):
            files += [target]
    elif os.path.isdir(target):
        #
        # "target" system directory: iterate over directory entries
        #            
        dlist = os.listdir(target)
            
        for ifile in dlist:
            ipath = target.rstrip('/')+'/'+ifile
            if os.path.isfile(ipath) and _matchKey(ifile, key):
                files += [ipath]
            
    elif target.count('castor'):
        #
        # "target" is likely CASTOR directory: iterate over directory entries
        #
        from subprocess import Popen, PIPE
        output = Popen(['nsls',target], stdout=PIPE).communicate()
        
        if len(output) == 0:
            plog.info('Found no files at: '+target)
            return            
        
        flist = output[0].split('\n')
        rpath = 'root://castoratlas/'+target.rstrip('/')
        
        plog.info('There are '+str(len(flist))+' file(s) at: '+target)
        
        for ifile in flist:
            if len(ifile) > 5 and _matchKey(ifile, key):
                files += [rpath+'/'+ifile]
                
    elif re.search('/eos/',target):
        #
        # "target" is likely eos directory: iterate over directory entries
        #
        from subprocess import Popen, PIPE
        output = Popen(['/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select','ls',target], stdout=PIPE).communicate()

        if len(output) == 0:
            plog.info('Found no files at: '+target)
            return            
        
        flist = output[0].split('\n')
        rpath = 'root://eosatlas/'+target.rstrip('/')
        
        plog.info('There are '+str(len(flist))+' file(s) at: '+target)

        for ifile in flist:
            if len(ifile) > 5 and _matchKey(ifile, key):
                files += [rpath+'/'+ifile]
                
    plog.info('Number of included input files: '+str(len(files)))

    for ifile in files:
        plog.info('   %s' %ifile)

    return files

#---------------------------------------------------------------------
#
# Search directory for matching files
#
def SearchRecursiveDir(target, key):
    #
    # Search "target" for files with name matching "key"
    #
    import os
    files = []

    if len(key) < 3:
        plog.warning('Recursive directory search requires key at least 3 characters long')
        return files
    
    if not os.path.isdir(target):
        return files

    #
    # "target" system directory: iterate over directory entries
    #            
    dlist = os.listdir(target)
            
    for ifile in dlist:
        ipath = target.rstrip('/')+'/'+ifile
        if os.path.isfile(ipath):
            if ifile.count(key):
                files += [ipath]
        else:
            files += SearchRecursiveDir(ipath, key)
            
    plog.info('Number of included input files: '+str(len(files)))
    return files

#---------------------------------------------------------------------
#
# Module helper functions - do not use outside this module
#
def _setBoolKey(reg, key, value):
    if value: reg.Set(key, "yes")
    else:     reg.Set(key, "no")

def _getRegistry():
    import ROOT
    ROOT.gROOT.SetBatch(True)    
    from ROOT import Anp
    return ROOT.Anp.Registry()

#---------------------------------------------------------------------
#
# Python configurable for Anp::AlgEvent class
#
class AlgConfig:
    """AlgConfig - configures single algorithm instance"""
        
    def __init__(self, alg_name, alg_type):
        self._alg_name = alg_name
        self._alg_type = alg_type
        self._target   = ''
        self._reg      = _getRegistry()        
        self._log      = getLog(alg_name)
        
    def Print(self):
        self._log.info('AlgConfig: '+self.GetAlgName())

    def GetAlgName(self): return self._alg_name
    def GetAlgType(self): return self._alg_type

    def GetTarget(self):
        return self._target

    def SetTarget(self, target):
        self._target = target
    
    def GetConfigRegistry(self):
        reg = _getRegistry()
        reg.Merge(self._reg)
        reg.Set("AlgType", self._alg_type)
        return reg

    def SetKey(self, key, value):
        if type(value) == bool:
            _setBoolKey(self._reg, self._alg_type+'::'+key, value)
        else:
            self._reg.Set(self._alg_type+'::'+key, value)

    def SetPar(self, key, value):
        if type(value) == bool:
            _setBoolKey(self._reg, key, value)
        else:
            self._reg.Set(key, value)
        
#---------------------------------------------------------------------
#
# Python configurable for Anp::RunAlg class
#   - configure list of AlgConfig objects
#
class AlgList:
    """AlgList - configures list of algorithms"""
    
    def __init__(self, alg_name):
        self._alg_name = alg_name
        self._alg_list = []
        self._reg      = _getRegistry()
        self._log      = getLog(alg_name)
        
    def Print(self):
        self._log.info('AlgList: name='+self.GetAlgName())
        for alg in self._alg_list: alg.Print()
        
    def GetAlgName(self):  return self._alg_name
    def GetAlgType(self):  return 'RunAlgs'

    def AddAlg(self, alg):

        alg_list = []
        if type(alg) == list:
            for inalg in alg:

                match_alg = False
                for myalg in self._alg_list:
                    if myalg.GetAlgName() == inalg.GetAlgName():
                        match_alg = True
                        break

                if match_alg:
                    self._log.warning('Ignore duplicate algorithm: '+inalg.GetAlgName())
                else:
                    alg_list += [inalg]
                    self._log.info('Adding algorithm: '+inalg.GetAlgName())
        else:
            for myalg in self._alg_list:
                if myalg.GetAlgName() == alg.GetAlgName():
                    self._log.warning('Failed to add duplicate algorithm: '+alg.GetAlgName())
                    return
            alg_list += [alg]

        self._log.debug('Adding list of %d algorithm(s)' % len(alg_list))
        self._alg_list += alg_list

    def SetKey(self, key, value):
        if type(value) == bool:
            _setBoolKey(self._reg, 'RunAlgs::'+key, value)
        else:
            self._reg.Set('RunAlgs::'+key, value)

    def SetPar(self, key, value):
        if type(value) == bool:
            _setBoolKey(self._reg, key, value)
        else:
            self._reg.Set(key, value)

    def SetGlobalPar(self, key, value):
        if type(value) == bool:
            _setBoolKey(self._reg, key, value)
        else:
            self._reg.Set(key, value)
        for myalg in self._alg_list: myalg.SetPar(key, value)

    def GetConfigRegistry(self):
        reg = _getRegistry()
        reg.Merge(self._reg)        

        alg_list = ''
        for myalg in self._alg_list:
            alg_list += myalg.GetAlgName() + ' '
            reg.Set(myalg.GetAlgName(), myalg.GetConfigRegistry())

        reg.Set('RunAlgs::AlgList', alg_list)
        
        return reg

#---------------------------------------------------------------------
#
# Python configurable for Anp::RunModule class
#   - configure input files
#   - configure algorithms
#   - configure histograms
#
class RunModule:
    """RunModule - configures input files and algorithms"""

    def __init__(self):
        import ROOT
        ROOT.gROOT.SetBatch(True)    
        self._alg   = None
        self._reg   = _getRegistry()
        self._run   = ROOT.Anp.RunModule()
        self._files = []
        self._dirs  = []        
        self._log   = getLog('RunModule')        
        self._hist  = _getRegistry()
        self._hist.AllowNonUniqueKeys()
        self._max_num_files = -1
        
    def Print(self):
        plog.info('RunModule: print children algorithms:')
        self._alg.Print()

    #---------------------------------------------------------------------
    #
    # Functions for python configuration
    #
    def SetKey(self, key, value):
        if type(value) == bool:
            _setBoolKey(self._reg, 'RunModule::'+key, value)
        else:
            self._reg.Set('RunModule::'+key, value)

    def SetPar(self, key, value):
        if type(value) == bool:
            _setBoolKey(self._reg, key, value)
        else:
            self._reg.Set(key, value)

    def SetGlobalPar(self, key, value):
        if type(value) == bool:
            _setBoolKey(self._reg, key, value)
        else:
            self._reg.Set(key, value)
        self._alg.SetGlobalPar(key, value)

    def SetTopAlg(self, alg):
        self._alg = alg

    def AddTopAlg(self, alg_name, alg_list):
        if self._alg != None:
            self._log.warning('Top algorithm is already configured - ignore new configuration')
            return
        
        self._alg = AlgList(alg_name)
        self._alg.AddAlg(alg_list)

    def AddHistFile(self, hfile):
        self._hist.Set('ReadFile', hfile)

    def StoreInputFile(self, file):
        self._log.debug('StoreInputFile: '+file)
        self._files += [file]

    def AddInputDir(self, target, key, max_num_files=-1):
        #
        # Search "target" for files with name matching "key"
        #
        files = SearchInputDir(target, key)
        if max_num_files>-1: self._max_num_files=max_num_files

        if len(files)>self._max_num_files and self._max_num_files>0:
            plog.info('Exceeded the of included input files: %s max: %s' %(len(files), self._max_num_files))
            del files[self._max_num_files:]

        for f in files: self.AddInputFile(f)

    def StoreInputDir(self, target, key, max_num_files=-1):
        #
        # Search "target" for files with name matching "key"
        #
        files = SearchInputDir(target, key)
        for f in files: self._files += [f]
        if max_num_files>-1: self._max_num_files=max_num_files
        
        if len(self._files)>self._max_num_files and self._max_num_files>0:
            plog.info('Exceeded the of included input files: %s max: %s' %(len(self._files), self._max_num_files))
            self._files = sorted(self._files)[:self._max_num_files]
            #del self._files[self._max_num_files:]
            plog.info('Truncated length %s' %(len(self._files,)))
            #print self._files
            
    def StoreRecursiveDir(self, target, key, max_num_files=-1):
        #
        # Search "target" for files with name matching "key"
        #
        files = SearchRecursiveDir(target, key)
        for f in files: self._files += [f]
        if max_num_files>-1: self._max_num_files=max_num_files
        
        if len(self._files)>self._max_num_files and self._max_num_files>0:
            plog.info('Exceeded the of included input files: %s max: %s' %(len(self._files), self._max_num_files))
            del self._files[self._max_num_files:]
                    
    def GetRegistryConfig(self):
        if type(self._alg) == None:
            self._log.warning('Missing top algorithm')
            return None

        reg = _getRegistry()
        
        reg.Merge(self._reg)        
        reg.Set('RunModule::AlgType', self._alg.GetAlgType())
        reg.Set('RunModule::AlgName', self._alg.GetAlgName())
        reg.Set('RunModule::HistMan', self._hist)
        reg.Set(self._alg.GetAlgName(), self._alg.GetConfigRegistry())

        input_reg = _getRegistry()
        input_reg.AllowNonUniqueKeys()

        nfiles=0
        for file in self._files:
            if self._max_num_files>0 and self._max_num_files<nfiles:
                nfiles+=1
                continue
            input_reg.Set('File', file)
            nfiles+=1
        reg.Set('RunModule::InputFiles', input_reg)
        return reg

    def ClearStoredFiles(self):
        self._files = []

    def GetStoredFiles(self,max_num_files=-1):
        local_files=self._files
        if max_num_files>-1: self._max_num_files=max_num_files
        
        if len(self._files)>self._max_num_files and self._max_num_files>0:
            plog.info('Exceeded the of included input files: %s max: %s' %(len(self._files), self._max_num_files))
            del self._files[self._max_num_files:]

        return local_files
    
    #---------------------------------------------------------------------
    #
    # Wrappers for C++ code: RunModule member functions
    #
    def Config(self, reg = None):
        if reg == None:
            self._run.Config(self.GetRegistryConfig())
        else:
            self._run.Config(reg)

    def Init(self):
        self._run.Init()

    def Exec(self):
        self._run.Exec()

    def Execute(self, reg_path):
        self._run.Execute(reg_path)

    def Done(self):
        self._run.Done()

    def AddInputFile(self, file):
        self._run.AddInputFile(file)

    def ClearInputFiles(self):
        self._run.ClearInputFiles()
