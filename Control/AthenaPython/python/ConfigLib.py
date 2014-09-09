# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file AthenaPython/ConfigLib.py
# @purpose functions to ease the configuration of reading/copying files
# @date January 2010

__doc__ = "functions to ease the configuration of reading/copying files"
__version__ = "$Revision: 285924 $"
__author__ = "Sebastien Binet <binet@cern.ch>"

__all__ = [
    'read_file',
    'copy_file',
    #'merge_files',
    'AutoCfg',
    ]

### imports -------------------------------------------------------------------
import collections
#import AthenaCommon.AlgSequence as Acas

### functions -----------------------------------------------------------------

def _read_file_impl(cfg, hints):
    cfg.disable_extra_steps()
    cfg.disable_read_steps()
    cfg.disable_write_steps()

    fname = cfg.input_files[0]
    import PyUtils.AthFile as af
    f = af.fopen(fname)

    if f.infos['file_type'] == 'pool':
        if cfg.is_evgen():
            pass

        elif cfg.is_rdo():
            cfg.rec['readRDO'] = True
            pass

        elif cfg.is_esd():
            cfg.rec['readESD'] = True
            pass

        elif cfg.is_aod():
            cfg.rec['readAOD'] = True
            pass

        elif cfg.is_tag():
            cfg.rec['readTAG'] = True
            pass
        
        pass
    
    elif f.infos['file_type'] == 'bs':
        cfg.rec['readRDO'] = True
        pass
    
    else:
        raise RuntimeError('unknown AthFile.file_type [%s]' % (f.infos['file_type'],))
    
    return cfg

def _copy_file_impl(cfg, hints):

    cfg = _read_file_impl(cfg, hints=hints)

    # now try to configure the writing...
    # get the file infos
    import PyUtils.AthFile
    af = PyUtils.AthFile.fopen(cfg.input_files[0])
    stream_names = af.infos['stream_names']
    if stream_names is None:
        stream_names = []

    def any_startswith(pattern, lst):
        return any(map(lambda x:x.startswith(pattern), lst))

    dst = cfg.output_file
    
    if af.infos['file_type'] == 'pool':
        do_write = None

        if (cfg.is_rdo() or
            cfg.is_esd() or
            cfg.is_aod() or
            cfg.is_tag()):

            if cfg.is_rdo():
                do_write = 'RDO'
                if hints.get('writeBS', False):
                    do_write = 'BS'
                    cfg.acf['BSRDOOutput'] = dst
                else:
                    cfg.acf['PoolRDOOutput'] = dst
                    
            elif cfg.is_esd():
                do_write = 'ESD'
                cfg.acf['PoolESDOutput'] = dst
                
            elif cfg.is_aod():
                do_write = 'AOD'
                cfg.acf['PoolAODOutput'] = dst

            elif cfg.is_tag():
                do_write = 'TAG'
                cfg.acf['PoolTAGOutput'] = dst

            else:
                raise RuntimeError('logic error')
            
        else: #if cfg.is_evgen():
            # FIXME - create an outputstream with the same stream name
            #       - configure it correctly
            import AthenaPoolCnvSvc.WriteAthenaPool as wap

            outstream = wap.AthenaPoolOutputStream(af.infos['stream_names'][0], dst)
            outstream.ForceRead=True
            outstream.TakeItemsFromInput=True

            ## tweak the default commit interval
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.AthenaPoolCnvSvc.CommitInterval = 100

            pass
        
        if do_write is not None:
            try:
                cfg.rec["doWrite"+do_write] = True
            except AttributeError:
                cfg.msg.info(err)
                pass
            
            pass
        
    else: # a bytestream
        do_write = None
        assert cfg.is_rdo()
        do_write = 'RDO'
        if hints.get('writeBS', False):
            do_write = 'BS'
            cfg.acf['BSRDOOutput'] = dst
        else:
            cfg.acf['PoolRDOOutput'] = dst
        try:
            cfg.rec["doWrite"+do_write] = True
        except AttributeError:
            cfg.msg.info(err)
            pass
        pass 

    return cfg

def read_file(src, hints=None, cfgobj=None):
    """ helper function to configure an athena job to read a file (or a list
    of files) `src`

    this triggers the autoconfiguration framework from:
    https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonAutoConfiguration

    Note: this function disables all writing RecFlags.
    Note: one can provide a dict of hints to help the framework find the
          right configuration or do the right thing

    example:

    >>> import AthenaPython.ConfigLib as apcl
    >>> apcl.read_file(src='ESD.pool.root')
    >>> apcl.read_file(src=['esd.001.pool', 'esd.002.pool'])
    >>> apcl.read_file(src='esd.pool', hints={'input_type':'esd'})
    >>> apcl.read_file(src='mc.pool', hints={'input_type':'evgen'})
    """
    if hints is None:
        hints = {}
        pass

    if cfgobj is None:
        cfg = AutoCfg('read-file-cfg', input_files=src)
    else:
        cfg = cfgobj
        cfg.input_files = src
        
    cfg = _read_file_impl(cfg, hints=hints)

    # triggers the auto-configuration
    cfg._apply_cfg()
    
    return cfg

def copy_file(src, dst=None, hints=None, cfgobj=None):
    """
    """
    if hints is None:
        hints = {}
        pass

    if cfgobj is None:
        cfg = AutoCfg("copy-file-cfg")
    else:
        cfg = cfgobj
    cfg.input_files = src
        
    if dst is None:
        cfg.msg.info('generating output file name...')
        import os
        dst = 'copy_'+os.path.basename(cfg.input_files[0])
        cfg.msg.info('generating output file name: [%s]', dst)

    cfg.output_file = dst
    
    cfg = _copy_file_impl(cfg, hints=hints)

    # triggers the auto-configuration
    cfg._apply_cfg()
    return cfg

### classes -------------------------------------------------------------------

class AutoCfg(object):
    """a simple class to hold the result of an auto-cfg procedure
    """

    def __init__(self, name="autocfg", **kwargs):
        object.__init__(self)
        self.name = name

        self.rec = kwargs.get('rec', {})
        self.acf = kwargs.get('acf', {})
        self._input = kwargs.get('input_files', [])
        self._output= kwargs.get('output_file', None)
        # number of files to test the is_xyz()
        self._nfiles= kwargs.get('nfiles', 1) 

        cfg = kwargs.get('cfg', ['everything'])
        if isinstance(cfg, basestring):
            cfg = cfg.split()
        if not isinstance(cfg, (list, tuple)):
            raise TypeError(
                'cfg argument must be a list, tuple or space-separated string'
                )
        self.cfgopts = list(cfg)[:]
        
        import AthenaCommon.Logging as L
        self.msg.setLevel(L.logging.INFO)
        return

    def configure_job(self, cfg=None):
        if not (cfg is None):
            if isinstance(cfg, basestring):
                cfg = cfg.split()
            if not isinstance(cfg, (list, tuple)):
                raise TypeError(
                    'cfg arg must be a list, tuple or space-separated string'
                    )
            self.cfgopts = list(cfg)[:]
        
        o = None
        if self.output_file:
            o = copy_file(src=self.input_files,
                          dst=self.output_file,
                          cfgobj=self)
        else:
            o = read_file(src=self.input_files,
                          cfgobj=self)
        return o
    
    def _apply_cfg(self):
        cfg = self.cfgopts[:]
        
        self.acf['FilesInput'] = self.input_files[:]
        
        from AthenaCommon.AthenaCommonFlags import jobproperties as jp
        acf = jp.AthenaCommonFlags
        for k,v in self.acf.iteritems():
            getattr(acf, k).set_Value_and_Lock(v)

        from RecExConfig.RecFlags import rec
        for k,v in self.rec.iteritems():
            globals()[k] = False # FIXME: backward compat...
            getattr(rec, k).set_Value_and_Lock(v)
            
        rec.AutoConfiguration = cfg
        import RecExConfig.AutoConfiguration as auto
        if (self.is_rdo() or
            self.is_esd() or
            self.is_aod() or
            self.is_tag()):
            # FIXME: autocfg fails when input is Stream1-evgen !
            return auto.ConfigureFromListOfKeys(rec.AutoConfiguration())
        else:
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            import AthenaPoolCnvSvc.ReadAthenaPool
            svcMgr.EventSelector.InputCollections = acf.FilesInput()

    @property
    def msg(self):
        import AthenaCommon.Logging as L
        msg = L.logging.getLogger(self.name)
        return msg
        
    def _set_input(self, src):
        if isinstance(src, basestring):
            src = [src]
        self._input = src[:]

    input_files = property(lambda self: self._input,
                           _set_input,
                           "the list of input (pool/bs) files")
    del _set_input

    def _set_output(self, src):
        if not isinstance(src, (type(None), basestring)):
            raise ValueError('output file must be None or a string')
        self._output = src

    output_file = property(lambda self: self._output,
                           _set_output,
                           "output (pool/bs) file name")
    del _set_output


    def disable_extra_steps(self):
        for item in ('doCBNT',
                     'doESD',
                     'doAOD',
                     'doDPD',
                     ):
            self.rec[item] = False

        # disable the time consuming stuff
        for item in ('doDumpTDS', 'doDumpTES',
                     'doMonitoring',
                     'doHist',
                     'doNameAuditor', 'doDetailedAuditor',
                     'doSGAuditor',
                     'doPerfMon', 'doDetailedPerfMon',
                     ):
            self.rec[item] = False
        return

    def disable_read_steps(self):

        for item in ('readRDO', #'readBS',
                     'readESD', 'readAOD', 'readTAG',
                     ):
            self.rec[item] = False
            pass
        return
        
    def disable_write_steps(self):
        for item in ('doWriteBS',
                     'doWriteRDO', 'doWriteESD',
                     'doWriteAOD', 'doWriteTAG', 'doWriteTAGCOM',
                     ):
            self.rec[item] = False
        return
    

    def is_evgen(self, input_files=None):
        if input_files is None:
            input_files=self.input_files
        if isinstance(input_files, basestring):
            input_files=[input_files]
        for fname in input_files[:self._nfiles]:
            import PyUtils.AthFile as af
            try:
                infos = af.fopen(fname).infos
                for stream_name in infos['stream_names']:
                    if stream_name.startswith(('StreamEvGen', 'StreamEVGEN')):
                        return True
                    if stream_name.startswith('Stream1'):
                        evtdata=collections.defaultdict(list)
                        for k,v in infos['eventdata_items']:
                            evtdata[k].append(v)
                        evtdata = dict(evtdata)
                        genevt = evtdata.get('McEventCollection', [])
                        if 'GEN_EVENT' in genevt:
                            return True
            except Exception, err:
                self.msg.info('caught:\n%s', err)
        return False

    def is_hits(self, input_files=None):
        if input_files is None:
            input_files=self.input_files
        if isinstance(input_files, basestring):
            input_files=[input_files]
        for fname in input_files[:self._nfiles]:
            import PyUtils.AthFile as af
            try:
                infos = af.fopen(fname).infos
                for stream_name in infos['stream_names']:
                    if stream_name.startswith((
                        'StreamHITS',
                        )):
                        return True
            except Exception, err:
                self.msg.info('caught:\n%s', err)
        return False
        
    def is_rdo(self, input_files=None):
        if input_files is None:
            input_files=self.input_files
        if isinstance(input_files, basestring):
            input_files=[input_files]
        for fname in input_files[:self._nfiles]:
            import PyUtils.AthFile as af
            try:
                infos = af.fopen(fname).infos
                if infos['file_type'] == 'bs':
                    return True
                for stream_name in infos['stream_names']:
                    if stream_name.startswith('StreamRDO'):
                        return True
                    if stream_name.startswith('Stream1'):
                        evtdata=collections.defaultdict(list)
                        for k,v in infos['eventdata_items']:
                            evtdata[k].append(v)
                        evtdata= dict(evtdata)
                        for k in evtdata.keys():
                            if k.endswith(('RDO_Container',
                                           'RawChannelContainer',
                                           'RdoContainer',)):
                                return True
            except Exception, err:
                self.msg.info('caught:\n%s', err)
        return False

    def is_esd(self, input_files=None):
        if input_files is None:
            input_files=self.input_files
        if isinstance(input_files, basestring):
            input_files=[input_files]
        for fname in input_files[:self._nfiles]:
            import PyUtils.AthFile as af
            try:
                infos = af.fopen(fname).infos
                for stream_name in infos['stream_names']:
                    if stream_name.startswith((
                        'StreamESD',
                        'DESD',
                        'StreamDESD',
                        'StreamDESDM',
                        'StreamD2ESD',
                        'StreamD2ESDM',
                        )):
                        return True
            except Exception, err:
                self.msg.info('caught:\n%s', err)
        return False

    def is_aod(self, input_files=None):
        if input_files is None:
            input_files=self.input_files
        if isinstance(input_files, basestring):
            input_files=[input_files]
        for fname in input_files[:self._nfiles]:
            import PyUtils.AthFile as af
            try:
                infos = af.fopen(fname).infos
                for stream_name in infos['stream_names']:
                    if stream_name.startswith((
                        'StreamAOD',
                        'DAOD',
                        'DPD',
                        'StreamDAOD',
                        'StreamDAODM',
                        'StreamD2AOD',
                        'StreamD2AODM',
                        )):
                        return True
            except Exception, err:
                self.msg.info('caught:\n%s', err)
        return False

    def is_tag(self, input_files=None):
        if input_files is None:
            input_files=self.input_files
        if isinstance(input_files, basestring):
            input_files=[input_files]
        for fname in input_files[:self._nfiles]:
            import PyUtils.AthFile as af
            try:
                infos = af.fopen(fname).infos
                for stream_name in infos['stream_names']:
                    if stream_name.startswith(('StreamTAG', 'TAG')):
                        return True
            except Exception, err:
                self.msg.info('caught:\n%s', err)
        return False

    def is_usr(self, input_files=None):
        if input_files is None:
            input_files=self.input_files
        if isinstance(input_files, basestring):
            input_files=[input_files]
        for fname in input_files[:self._nfiles]:
            import PyUtils.AthFile as af
            try:
                infos = af.fopen(fname).infos
                for stream_name in infos['stream_names']:
                    if stream_name.startswith((
                        'StreamUSR',
                        )):
                        return True
            except Exception, err:
                self.msg.info('caught:\n%s', err)
        return False
        
    pass # AutoCfgObject
