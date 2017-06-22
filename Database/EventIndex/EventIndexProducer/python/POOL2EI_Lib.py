# @file POOL2EI_Lib.py
# @purpose provide components to get EventIndex data from pool files
# @author Javier Sanchez
# @date February 2014
#
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# 
# Some code borrowed from PyAthena.FilePeekerLib
# credits to Sebastien Binet 

__version__= "$Revision: 000001 $"
__author__ = "Javier Sanchez"
__doc__ = "provide components to POOL2EI"


### imports --------------------------------------------------------------------
import AthenaPython.PyAthena as PyAthena
from compressB64 import compressB64
from EI_Lib import EIrecord, IOV

import struct
from EventIndexProducer.EIPBof import EIPBof
import gzip

import time
StatusCode = PyAthena.StatusCode

# MN/sss: make Coral.AttributeList work in Coral3/ROOT6/gcc5
from PyCool import coral
_attribute_methods = dir(coral.Attribute)
_methnames = ['data<std::__cxx11::basic_string<char> >',
              'data<std::basic_string<char> >',
              'data<std::string>']
for _m in _methnames:
    if _m in _attribute_methods:
        _attribute_getdata = _m
        break
else:
    raise Exception("Can't find data method in Attribute")
def attr_str_data(attr):
    return getattr(attr, _attribute_getdata) ()


def _import_ROOT():
    import sys
    sys.argv.insert(1, '-b')
    import ROOT
    del sys.argv[1]
    return ROOT

def toiter(beg,end):
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return

class sg_versioned_key(object):
    def __init__(self, sgkey):
        self.raw_key = sgkey
    @property
    def key(self):
        return self.raw_key.split(";")[-1]
        
class POOL2EI(PyAthena.Alg):
    """Algorithm
    """
    _iov = IOV()
    _eif = None
    _eif_entries = 0
    _eif_totentries = 0
    _eif_nfiles = 0
    _eifname = None
    _eifname_spb = None

    _eiversion = 0x00000001
    _eimagic   = 0x6e56c8c7

    def __init__(self, name='POOL2EI', **kw):
        ## init base class
        super(POOL2EI, self).__init__(name, **kw)
        
        _info = self.msg.info
        _info("POOL2EI::__init__")


        
    def initialize(self):

        import AthenaPython.PyAthena as PyAthena
        _info = self.msg.info
        _info("POOL2EI::initialize")

        _info("## DoProvenanceRef: {}".format(self.DoProvenanceRef))
        _info("## DoTriggerInfo: {}".format(self.DoTriggerInfo))
        _info("## HaveHlt: {}".format(self.HaveHlt))
        _info("## HaveXHlt: {}".format(self.HaveXHlt))
        _info("## RUN1: {}".format(self.RUN1))
        _info("## SendToBroker: {}".format(self.SendToBroker))

        self.eipbof = EIPBof(self._eiversion) # initialize factory

        self._dsname = "Unknown.Input.Dataset.Name"   # default fake value
        if self.EiDsName is not None:
            _info("## EiDsName: {}".format(self.EiDsName))
            self._dsname = self.EiDsName
        else:
            # try to get dataset name from pathena INDS environment variable
            import os
            inds = os.getenv('INDS')
            if inds is not None:
                _info("## INDS: {}".format(inds))
                self._dsname = inds
            else:
                # else, try to use job definition
                try:
                    import newJobDef
                    processingType = newJobDef.job['processingType']
                    transformation = newJobDef.job['transformation']
                    dsSource ='realDatasetsIn'    # take dataset name from input
                    if processingType == 'merge' and transformation != 'POOLtoEI_tf.py':
                        dsSource = 'realDatasets' # take dataset name from output
                    datasets = newJobDef.job[dsSource].split(',')
                    _info("## {}[0]: {}".format(dsSource,datasets[0]))
                    self._dsname = datasets[0]
                except:
                    _info('## Unable to get dataset name from realDatasetsIn or realDatasets')

        # remove _tid and _sub parts from dsname
        import re
        self._dsname_orig = self._dsname
        self._dsname = re.sub('_tid[0-9]{8}_[0-9]{2}', '', self._dsname)
        self._dsname = re.sub('_sub[0-9]{10}', '', self._dsname)
        self._dsname = re.sub('\/$', '', self._dsname)
                

        # token match regex
        import re
        self._re_pool_token = re.compile(r'[[]DB=(?P<db>.*?)[]]' \
                                        r'[[]CNT=(?P<cnt>.*?)[]]' \
                                        r'[[]CLID=(?P<clid>.*?)[]]' \
                                        r'[[]TECH=(?P<tech>.*?)[]]' \
                                        r'[[]OID=(?P<oid>.*?)[]]').match

        # load our pythonizations:
        for cls_name in ('EventStreamInfo', 'EventType', 'PyEventType'):
            cls = getattr(PyAthena, cls_name)

        _info("retrieving various stores...")
        for store_name in ('evtStore', 'inputStore', 'detStore',
                           'tagStore', 'metaStore'):
            _info("retrieving [{}]...".format(store_name))
            o = getattr(self, store_name)
            _info("retrieving [{}]... [done]".format(store_name))
        _info("retrieving various stores... [done]")


        if self.HaveHlt:
            # load trigger decision tool
            from TriggerJobOpts.TriggerFlags import TriggerFlags
            TriggerFlags.configurationSourceList = ['ds']
            import AthenaPython.PyAthena as PyAthena
            self.trigDec = PyAthena.py_tool('Trig::TrigDecisionTool/TrigDecisionTool')
            self.trigDec.ExperimentalAndExpertMethods().enable()
            if self.HaveXHlt:
                self.trigDec.setProperty("ConfigTool","TrigConf::xAODConfigTool");
                self.trigDec.setProperty("TrigDecisionKey","xTrigDecision")
            else:
                self.trigDec.setProperty("ConfigTool","TrigConf::AODConfigTool");
                self.trigDec.setProperty("TrigDecisionKey","TrigDecision")
            # to detect changes in SMK
            self.SMK_saved = -1

        ## Build output file names
        if self.EiFmt <= 0:
            if self.Out is not None:
                oname = self.Out
            else:
                oname = "output.ei.pkl"
            oname_spb = None
        elif self.EiFmt == 1:
            if self.Out is not None:
                oname_spb = self.Out
            else:
                oname_spb = "output.ei.spb"
            oname = None
        else:
            # both outputs
            if self.Out is None:
                oname     = "output.ei.pkl"
                oname_spb = "output.ei.spb"
            else:
                if self.Out.endswith(".pkl"):
                    oname     = self.Out
                    oname_spb = oname[:-4] + ".spb"
                elif self.Out.endswith(".spb"):
                    oname_spb = self.Out
                    oname     = oname_spb[:-4] + ".pkl"
                else:
                    oname_spb = self.Out + ".spb"
                    oname     = self.Out
        
        ## open output pkl file
        import os
        if oname is not None:
            oname = os.path.expanduser(os.path.expandvars(oname))
            self._eifname = oname
            _info('Opening EI file [{}]...'.format(oname))

            if os.path.exists(oname):
                os.remove(oname)

            import PyUtils.dbsqlite as dbsqlite
            try:
                self._eif = dbsqlite.open(oname,flags='w')
            except:
                self._eif = None

            if self._eif is None:
                self.msg.fatal("Unable to open EI output file {} exapnded as {}".format(self.Out, oname))
                raise RuntimeError("Unable to open EI output file")
        else:
            self._eif = None


        if oname_spb is not None:
            oname_spb = os.path.expanduser(os.path.expandvars(oname_spb))
            self._eifname_spb = oname_spb
            _info('Opening EI SPB file [{}]...'.format(oname_spb))

            if os.path.exists(oname_spb):
                os.remove(oname_spb)

            try:
                self._eif_spb = gzip.open(oname_spb, 'wb')
            except:
                self._eif_spb = None

            if self._eif_spb is None:
                self.msg.fatal("Unable to open EI SPB output file {} exapnded as {}".format(self.Out, oname_spb))
                raise RuntimeError("Unable to open EI SPB output file")

            self._eif_spb.write(struct.pack('<I', self._eimagic ))      # EI SPB magic
            self._eif_spb.write(struct.pack('<I', self._eiversion ))    # EI SPB versin
        else:
            self._eif_spb = None
                        

        taskID0 = ""
        jobID0 = ""
        # get taskid and jobid
        if hasattr(self,'TaskID') and hasattr(self,'JobID') and self.TaskID is not None and self.JobID is not None:
            taskID0 = "{}.T".format(self.TaskID)
            if hasattr(self,'AttemptNumber') and self.AttemptNumber is not None:
                jobID0 = "{}.{}".format(self.JobID,self.AttemptNumber)
            else:
                jobID0 = "{}.0".format(self.JobID)
        else:
            # get them from job info 
            try:
                import newJobDef
                taskID0 = "{}.G".format(newJobDef.job['taskID'])
                jobID0  = "{}.{}".format(newJobDef.job['PandaID'], newJobDef.job['attemptNr'])
            except:
                taskID0 = "{}.G".format(os.getenv('PanDA_TaskID', 0))
                jobID0  = "{}.0".format(os.getenv('PandaID', 0))

            
        # initial information
        if self._eif is not None:
            self._eif['StartProcTime'] = int(time.time() * 1000)
            self._eif['TaskID']        = taskID0
            self._eif['JobID']         = jobID0
            self._eif['Schema']        = EIrecord().getRecSchema()
            self._eif['Version']       = EIrecord().getVersion()
            self._eif['InputDsName']   = self._dsname

            #processing options
            self._eif['ProvenanceRef'] = self.DoProvenanceRef
            self._eif['TriggerInfo']   = self.DoTriggerInfo

        # initial information SPB
        if self._eif_spb is not None:
            header = self.eipbof.Header()
            header.startProcTime  = int(time.time() * 1000)
            header.taskID         = taskID0
            header.jobID          = jobID0
            header.inputDsName    = self._dsname_orig
            header.provenanceRef  = self.DoProvenanceRef
            header.triggerInfo    = self.DoTriggerInfo

            spb = header.SerializeToString()
            self._eif_spb.write(struct.pack('<I', 1<<8 | 0x01 ))
            self._eif_spb.write(struct.pack('<I', len(spb)))
            self._eif_spb.write(spb)

        return StatusCode.Success

    @property
    def evtStore(self):
        import AthenaPython.PyAthena as PyAthena
        return PyAthena.py_svc('StoreGateSvc/StoreGateSvc')
    
    @property
    def metaStore(self):
        import AthenaPython.PyAthena as PyAthena
        return PyAthena.py_svc('StoreGateSvc/MetaDataStore')
    
    @property
    def tagStore(self):
        import AthenaPython.PyAthena as PyAthena
        return PyAthena.py_svc('StoreGateSvc/TagMetaDataStore')
    
    @property
    def detStore(self):
        import AthenaPython.PyAthena as PyAthena
        return PyAthena.py_svc('StoreGateSvc/DetectorStore')
    
    @property
    def inputStore(self):
        import AthenaPython.PyAthena as PyAthena
        return PyAthena.py_svc('StoreGateSvc/InputMetaDataStore')

    def process_metadata(self, store, metadata_name):
        msg = self.msg
        try:
            obj = store[metadata_name]
        except KeyError,err:
            msg.warning('could not retrieve [{}]'.format(metadata_name))
            return ([],[])
        msg.info('processing container [{}]'.format(obj.folderName()))
        data = []
        iovs = []
        payloads = obj.payloadContainer()
        payloads_sz = payloads.size()
        if hasattr(payloads, 'at'):
            # HACK for bug #77976
            _tmp = payloads
            payloads = []
            for ii in range(payloads_sz):
                payloads.append(_tmp.at(ii))
            pass
        for ii,payload in zip(range(payloads_sz), payloads):
            if not payload:
                msg.info ("**error** null-pointer ?")
                continue
            # names
            chan_names = []
            sz = payload.name_size()
            for idx in xrange(sz):
                chan = payload.chanNum(idx)
                chan_name = payload.chanName(chan)
                chan_names.append(chan_name)
                
            # iovs
            sz = payload.iov_size()
            for idx in xrange(sz):
                chan = payload.chanNum(idx)
                iov_range = payload.iovRange(chan)
                iov_start = iov_range.start()
                iov_stop  = iov_range.stop()
                msg.info( '({}, {}) => ({}, {}) valid={} runEvt={}'.format(
                        iov_start.run(),
                        iov_start.event(),
                        iov_stop.run(),
                        iov_stop.event(),
                        iov_start.isValid(),
                        iov_start.isRunEvent()))
                iovs.append((iov_start.run(),iov_start.event(),iov_stop.run(),iov_stop.event(),iov_start.isValid(),iov_start.isRunEvent()))
                    
            # attrs
            attrs = [] # can't use a dict as spec.name() isn't unique
            sz = payload.size()
            for idx in xrange(sz):
                chan = payload.chanNum(idx)
                attr_list = payload.attributeList(chan)
                attr_data = []
                for a in list(toiter(attr_list.begin(), attr_list.end())):
                    spec   = a.specification()
                    a_type = spec.typeName()
                    if a_type.find('string') >= 0:
                        a_data = attr_str_data(a)
                        try:
                            a_data = eval(a_data,{},{})
                        except Exception:
                            # swallow and keep as a string
                            pass
                    else:
                        a_data = getattr(a,'data<{}>'.format(a_type))()
                    attr_data.append((spec.name(), a_data))
                attrs.append(dict(attr_data))
            if len(attrs) == len(chan_names):
                data.append(dict(zip(chan_names,attrs)))
            else:
                if len(attrs):
                    if len(attrs) == 1:
                        data.append(attrs[0])
                    else:
                        data.append(attrs)
                else:
                    data.append(chan_names)
            pass # loop over payloads...
        
        return (data,iovs)

    ##########################################
    # execute at begin of file
    ##########################################
    def beginFile(self):
        import AthenaPython.PyAthena as PyAthena
        _info = self.msg.info
        _info("POOL2EI::beginFile")
        
        # entries for this input file
        self._eif_entries = 0

        # define a new IOV storage
        self._iov = IOV()

        ## inputStore
        store = self.inputStore
        esi_keys = store.keys('EventStreamInfo')
        nentries = None
        ddt = None
        if len(esi_keys) >= 1:
            sg_key = esi_keys[-1]
            nentries = 0
            stream_names = esi_keys[:]
            for sg_key in esi_keys:
                esi = store.retrieve('EventStreamInfo', sg_key)
                _info('=== [EventStreamInfo#{}] ==='.format(sg_key))
                nentries += esi.getNumberOfEvents()
                evt_types = PyAthena.EventStreamInfo.evt_types(esi)


        # retrieve the GUID
        def _get_guid():
            guid = None
            ROOT = _import_ROOT()
            import os
            root_files = list(ROOT.gROOT.GetListOfFiles())
            if len(root_files)==0:
                _info('could not find correct ROOT file')
                return
            
            root_file = root_files[-1]  # get the last file in the list
            pool = root_file.Get("##Params")
            import re
            # Pool parameters are of the form:
            # '[NAME=somevalue][VALUE=thevalue]'
            pool_token = re.compile(r'[[]NAME=(?P<name>.*?)[]]'\
                                    r'[[]VALUE=(?P<value>.*?)[]]').match
            params = []
            guids = []
            for i in xrange(pool.GetEntries()):
                if pool.GetEntry(i)>0:
                    pool_string = pool.db_string
                    # take string until \0 is found
                    n = pool_string.find('\0')
                    if n != -1:
                        pool_string = pool_string[:n]
                    match = pool_token(pool_string)
                    if not match:
                        continue
                    d = match.groupdict()
                    params.append((d['name'], d['value']))
                    if d['name'].lower() == 'fid':
                        guids.append(d['value'])
            return guids

        self.guids = _get_guid()
        if len(self.guids) > 0:
            self.guid = self.guids.pop()
        else:
            self.guid = None

        if self.DoTriggerInfo and self.HaveHlt:
            # forget old SMK
            self.SMK_saved = -1

            ##/TRIGGER/HLT/HltConfigKeys
            (hltck_info, hltck_iovs) = self.process_metadata(self.inputStore,'/TRIGGER/HLT/HltConfigKeys')
            #hltpsk_l = [ x['HltPrescaleConfigurationKey'] for x in hltck_info ]
            #for val, iov in zip(hltpsk_l,hltck_iovs):
            #    self._iov.add('HLTPSK',val,iov[:4])
            smk_l = [ x['MasterConfigurationKey'] for x in hltck_info ]
            for val, iov in zip(smk_l,hltck_iovs):
                self._iov.add('SMK',val,iov[:4])

            ##/TRIGGER/HLT/PrescaleKey
            (l1ck_info, l1ck_iovs) = self.process_metadata(self.inputStore,'/TRIGGER/HLT/PrescaleKey')
            l1ck_l = [ x['HltPrescaleKey'] for x in l1ck_info ]
            for val, iov in zip(l1ck_l,l1ck_iovs):
                self._iov.add('HLTPSK',val,iov[:4])

            ##/TRIGGER/LVL1/Lvl1ConfigKey
            (l1ck_info, l1ck_iovs) = self.process_metadata(self.inputStore,'/TRIGGER/LVL1/Lvl1ConfigKey')
            l1ck_l = [ x['Lvl1PrescaleConfigurationKey'] for x in l1ck_info ]
            for val, iov in zip(l1ck_l,l1ck_iovs):
                self._iov.add('L1PSK',val,iov[:4])
            

        (tginfo, tgiovs) = self.process_metadata(self.inputStore,'/TagInfo')
        amitag     = "Unknown"
        trigStream = "Unknown"
        projName   = "Unknown"
        if len(tginfo) > 0:
            for tgi in tginfo:
                if 'AMITag' in tgi:
                    amitag = tgi['AMITag']
                    _info("## AMITag: {}".format(amitag))
                if 'triggerStreamOfFile' in tgi:
                    trigStream = tgi['triggerStreamOfFile']
                    _info("## triggerStreamOfFile: {}".format(trigStream))
                if 'project_name' in tgi:
                    projName = tgi['project_name']
                    _info("## project_name: {}".format(projName))


        
        if  self._eif is not None:
            nfile = self._eif_nfiles
            self._eif['StartProcTime_{:d}'.format(nfile)] = int(time.time() * 1000)
            self._eif['AMITag_{:d}'.format(nfile)] = amitag
            self._eif['TrigStream_{:d}'.format(nfile)] = trigStream
            self._eif['ProjName_{:d}'.format(nfile)] = projName
            self._eif['GUID_{:d}'.format(nfile)] = self.guid

        if  self._eif_spb is not None:
            beginGUID = self.eipbof.BeginGUID()
            beginGUID.startProcTime = int(time.time() * 1000)
            beginGUID.AMITag        = amitag
            beginGUID.trigStream    = trigStream
            beginGUID.projName      = projName
            beginGUID.guid          = self.guid

            spb = beginGUID.SerializeToString()
            self._eif_spb.write(struct.pack('<I', 3<<8 | 0x01))
            self._eif_spb.write(struct.pack('<I', len(spb)))
            self._eif_spb.write(spb)

        self._eif_nfiles += 1
            
        return


    ##########################################
    # execute at end of file
    ##########################################
    def endFile(self):
        import AthenaPython.PyAthena as PyAthena
        _info = self.msg.info
        _info("POOL2EI::endFile")

        nfile = self._eif_nfiles-1

        if  self._eif is not None:
            self._eif['Nentries_{:d}'.format(nfile)] = self._eif_entries
            self._eif['EndProcTime_{:d}'.format(nfile)] = int(time.time() * 1000)

        if  self._eif_spb is not None:
            endGUID = self.eipbof.EndGUID()
            endGUID.nentries    = self._eif_entries
            endGUID.endProcTime = int(time.time() * 1000)

            spb = endGUID.SerializeToString()
            self._eif_spb.write(struct.pack('<I', 4<<8 | 0x01 ))
            self._eif_spb.write(struct.pack('<I', len(spb)))
            self._eif_spb.write(spb)

        return 

    #################################################
    # Get trigger list and chain counters or ctpIds
    #################################################
    def getChainCounter(self, level):
        _info = self.msg.info

        triggers = self.trigDec.getChainGroup(level)
        names = triggers.getListOfTriggers()
        _info("POOL2EI::getChainCounter got {} triggers for level {}".format( names.size(),level))

        if level.startswith("L1"):
            level1 = True
        else:
            level1 = False

        ccname = {}
        ccmax = 0
        gtd_method = True                         # by default, use getChainDetails for L2,EF,HLT
        try:
            for i in xrange(names.size()):
                name = names.at(i)
                if level1:
                    cc = self.trigDec.ExperimentalAndExpertMethods().getItemConfigurationDetails(name).ctpId()
                else:
                    if gtd_method:
                        try:
                            cc = self.trigDec.ExperimentalAndExpertMethods().getChainDetails(name).getChainCounter()
                        except:
                            gtd_method = False    # disable this method for next triggers 
                    if not gtd_method:
                        cc = self.trigDec.ExperimentalAndExpertMethods().getChainConfigurationDetails(name).chain_counter()
                ccname[cc]=name
                if cc > ccmax:
                    ccmax = cc

            if len(ccname) == 0:
                cclen = 0
            else:
                cclen = ccmax + 1
        except:
            _info("POOL2EI::getChainCounter Unable to get trigger chains for {}".format(level))
            ccname = {}
            cclen = 0

        return ( cclen, ccname )
        
 
    ##########################################
    # execute at begin of run
    ##########################################
    def beginRun(self):
        import AthenaPython.PyAthena as PyAthena
        _info = self.msg.info
        _info("POOL2EI::beginRun")

        # get trigger chains
        if self.DoTriggerInfo and self.HaveHlt:
            if self.RUN1:
                ( self.cclenL1, self.ccnameL1 ) = self.getChainCounter("L1_.*")
                ( self.cclenL2, self.ccnameL2 ) = self.getChainCounter("L2_.*")
                ( self.cclenEF, self.ccnameEF ) = self.getChainCounter("EF_.*")
            else:
                ( self.cclenL1, self.ccnameL1 ) = self.getChainCounter("L1_.*")
                ( self.cclenL2, self.ccnameL2 ) = self.getChainCounter("HLT_.*")
                ( self.cclenEF, self.ccnameEF ) = ( self.cclenL2, self.ccnameL2 )

        return StatusCode.Success

    ##########################################
    # get chain counters from detector Store
    ##########################################
    def getChainCountersFromDetStore(self):
        _info = self.msg.info

        # try to get L1 trigger menu from detStore if not loaded in BeginRun
        if self.cclenL1 == 0:
            _info("POOL2EI::getChainCountersFromDetStore Trying to get L1 trigger names from detStore")
            try:
                self.cclenL1 = 0
                self.ccnameL1 = {}
                lvl1menu = self.detStore['/TRIGGER/LVL1/Menu']
                for (ichan, entry) in lvl1menu:
                    self.ccnameL1[ichan] = entry['ItemName']
                    if ichan > self.cclenL1:
                        self.cclenL1 = ichan
                _info("POOL2EI::getChainCountersFromDetStore got {} triggers for level L1".format(self.cclenL1))
                if len(self.ccnameL1) == 0:
                    self.cclenL1 = 0
                else:
                    self.cclenL1 += 1
            except:
                _info("POOL2EI::getChainCountersFromDetStore Unable to get L1 trigger names from detStore")
                self.cclenL1 = 0
                self.ccnameL1 = {}
                pass

        # try to get HLT trigger menu from detStore if not loaded in BeginRun
        if self.cclenL2 == 0 or self.cclenEF == 0:
            _info("POOL2EI::getChainCountersFromDetStore Trying to get HLT trigger names from detStore")
            if self.RUN1:
                try:
                    self.cclenL2 = 0
                    self.ccnameL2 = {}
                    self.cclenEF = 0
                    self.ccnameEF = {}
                    hltmenu = self.detStore['/TRIGGER/HLT/Menu']
                    for (ichan, entry) in hltmenu:
                        ccounter = entry['ChainCounter']
                        ccname   = entry['ChainName']
                        if ccname.startswith("L2_"):
                            self.ccnameL2[ccounter] = ccname
                            if ccounter > self.cclenL2:
                                self.cclenL2 = ccounter
                        if ccname.startswith("EF_"):
                            self.ccnameEF[ccounter] = ccname
                            if ccounter > self.cclenEF:
                                self.cclenEF = ccounter
                    _info("POOL2EI::getChainCountersFromDetStore got {} triggers for level L2".format(self.cclenL2))
                    _info("POOL2EI::getChainCountersFromDetStore got {} triggers for level EF".format(self.cclenEF))
                    if len(self.ccnameL2) == 0:
                        self.cclenL2 = 0
                    else:
                        self.cclenL2 += 1
                    if len(self.ccnameEF) == 0:
                        self.cclenEF = 0
                    else:
                        self.cclenEF += 1
                except:
                    _info("POOL2EI::getChainCountersFromDetStore Unable to get L2 & EF trigger names from detStore")
                    self.cclenL2 = 0
                    self.ccnameL2 = {}
                    self.cclenEF = 0
                    self.ccnameEF = {}
                    pass
            else:
                try:
                    self.cclenL2 = 0
                    self.ccnameL2 = {}
                    hltmenu = self.detStore['/TRIGGER/HLT/Menu']
                    for (ichan, entry) in hltmenu:
                        ccounter = entry['ChainCounter']
                        ccname   = entry['ChainName']
                        if ccname.startswith("HLT_"):
                            self.ccnameL2[ccounter] = ccname
                            if ccounter > self.cclenL2:
                                self.cclenL2 = ccounter
                    _info("POOL2EI::getChainCountersFromDetStore got {} triggers for level HLT".format(self.cclenL2))
                    if len(self.ccnameL2) == 0:
                        self.cclenL2 = 0
                    else:
                        self.cclenL2 += 1
                    ( self.cclenEF, self.ccnameEF ) = ( self.cclenL2, self.ccnameL2 )
                except:
                    _info("POOL2EI::getChainCountersFromDetStore Unable to get HLT trigger names from detStore")
                    self.cclenL2 = 0
                    self.ccnameL2 = {}
                    pass

 
    def writeTrigMenutoSPB(self, SMK, L1PSK, HLTPSK):

        L1Menu = []
        for pos,name in self.ccnameL1.items():
            L1Menu.append("{:d}:{}".format(pos,name))

        L2Menu = []
        for pos,name in self.ccnameL2.items():
            L2Menu.append("{:d}:{}".format(pos,name))

        if not self.HaveXHlt:
            EFMenu = []
            for pos,name in self.ccnameL2.items():
                EFMenu.append("{:d}:{}".format(pos,name))

        if self._eif_spb is not None:
            tMenu = self.eipbof.TriggerMenu()
            tMenu.SMK    = ( SMK    if SMK    is not None else 0 )
            tMenu.L1PSK  = ( L1PSK  if L1PSK  is not None else 0 )
            tMenu.HLTPSK = ( HLTPSK if HLTPSK is not None else 0 )
            tMenu.L1Menu = ";".join(L1Menu)
            if self.HaveXHlt:
                tMenu.HLTMenu = ";".join(L2Menu)
            else:
                tMenu.L2Menu = ";".join(L2Menu)
                tMenu.EFMenu = ";".join(EFMenu)

            spb = tMenu.SerializeToString()
            self._eif_spb.write(struct.pack('<I', 5<<8 | 0x01 ))
            self._eif_spb.write(struct.pack('<I', len(spb)))
            self._eif_spb.write(spb)
        

    ##########################################
    # execute event by event
    ##########################################
    def execute(self):
        import AthenaPython.PyAthena as PyAthena

        if self._eif_totentries < 100:
            _info = self.msg.info
        else:
            _info = lambda *x: None
        _error = self.msg.error

        _info("POOL2EI::execute")

        if self._eif is not None:
            eirec   = EIrecord()
        if self._eif_spb is not None:
            eventPB = self.eipbof.EIEvent()

        # -- Get EventInfo data
        store = self.evtStore
        evt_info_keys = store.keys('EventInfo')
        if len(evt_info_keys) != 1:
            _info('more than one EventInfo: {}'.format(evt_info_keys))
            _info(' ==> we\'ll use [{}]'.format(evt_info_keys[0]))
        sg_key = evt_info_keys[0]
        ei = store.retrieve('EventInfo', sg_key)
        _info('=== [EventInfo#{}] ==='.format(sg_key))
        eid = ei.event_ID()
        _info('## bunch_crossing_id: {:d}'.format(eid.bunch_crossing_id()))
        _info('## event_number: {:d}'.format(eid.event_number()))
        _info('## lumi_block: {:d}'.format(eid.lumi_block()))
        _info('## run_number: {:d}'.format(eid.run_number()))
        _info('## time_stamp: {:d}'.format(eid.time_stamp()))
        _info('## time_stamp_ns_offset: {:d}'.format(eid.time_stamp_ns_offset()))

        
        # event type
        eitype = ei.event_type()
        bm = list(eitype.bit_mask)
        if 'IS_SIMULATION' in bm:
            if self._eif is not None:
                eirec['IsSimulation']  = 1            # IS_SIMULATION
            if self._eif_spb is not None:
                eventPB.isSimulation   = True         # IS_SIMULATION
        else:
            if self._eif is not None:
                eirec['IsSimulation']  = 0            # IS_DATA
            if self._eif_spb is not None:
                eventPB.isSimulation   = False        # IS_DATA

        if 'IS_TESTBEAM' in bm:
            if self._eif is not None:
                eirec['IsTestBeam']    = 1            # IS_TESTBEAM
            if self._eif_spb is not None:
                eventPB.isTestBeam     = True         # IS_TESTBEAM
        else:
            if self._eif is not None:
                eirec['IsTestBeam']    = 0            # IS_FROM_ATLAS_DET
            if self._eif_spb is not None:
                eventPB.isTestBeam     = False        # IS_FROM_ATLAS_DET

        if 'IS_CALIBRATION' in bm:
            if self._eif is not None:
                eirec['IsCalibration'] = 1            # IS_CALIBRATION
            if self._eif_spb is not None:
                eventPB.isCalibration  = True         # IS_CALIBRATION
        else:
            if self._eif is not None:
                eirec['IsCalibration'] = 0            # IS_PHYSICS
            if self._eif_spb is not None:
                eventPB.isCalibration  = False        # IS_PHYSICS
        


        run_number   = eid.run_number()
        event_number = eid.event_number()
        lumi_block   = eid.lumi_block()
        
        if self._eif is not None:
            eirec['RunNumber']        = run_number
            eirec['EventNumber']      = event_number
            eirec['LumiBlockN']       = lumi_block
            eirec["BunchId"]          = eid.bunch_crossing_id()
            eirec['EventTime']        = eid.time_stamp()
            eirec['EventTimeNanoSec'] = eid.time_stamp_ns_offset() 
            eirec['EventWeight']      = eitype.mc_event_weight()
            eirec['McChannelNumber']  = eitype.mc_channel_number()

        if self._eif_spb is not None:
            eventPB.runNumber         = run_number
            eventPB.eventNumber       = event_number
            eventPB.lumiBlock         = lumi_block
            eventPB.bcid              = eid.bunch_crossing_id()
            eventPB.timeStamp         = eid.time_stamp()
            eventPB.timeStampNSOffset = eid.time_stamp_ns_offset()
            eventPB.mcEventWeight     = eitype.mc_event_weight()
            eventPB.mcChannelNumber   = eitype.mc_channel_number()

        _info('## EventWeight: {:f}'.format(eitype.mc_event_weight()))
        _info('## McChannelNumber: {:d}'.format(eitype.mc_channel_number()))

        # xAOD
        if 'xAOD::EventInfo' in store.keys():
            xei = store.retrieve('xAOD::EventInfo', 'EventInfo')
            _info('#2# bunch_crossing_id: {:d}'.format(xei.bcid()))
            _info('#2# event_number: {:d}'.format(xei.eventNumber()))
            _info('#2# lumi_block: {:d}'.format(xei.lumiBlock()))
            _info('#2# run_number: {:d}'.format(xei.runNumber()))
            _info('#2# time_stamp: {:d}'.format(xei.timeStamp()))
            _info('#2# time_stamp_ns_offset: {:d}'.format(xei.timeStampNSOffset()))

        # -- trigger processing
        if self.DoTriggerInfo:
            eit = ei.trigger_info()
            _info("## Lvl1ID {}".format(eit.extendedLevel1ID()))
            if 'xAOD::EventInfo' in store.keys():
                _info("#2# Lvl1ID {}".format(xei.extendedLevel1ID()))
            Lvl1ID = eit.extendedLevel1ID()
            if self._eif is not None:
                eirec['Lvl1ID'] = Lvl1ID
            if self._eif_spb is not None:
                eventPB.extendedLevel1ID = Lvl1ID
            trigL1 = ""
            trigL2 = ""
            trigEF = ""
            _info("LEN_L1: {}".format(32*len(eit.level1TriggerInfo())))
            for v in eit.level1TriggerInfo():
                trigL1 += "{0:032b}".format(v)[::-1]
            _info("LEN_L2: {}".format(32*len(eit.level2TriggerInfo())))
            for v in eit.level2TriggerInfo():
                trigL2 += "{0:032b}".format(v)[::-1]
            _info("LEN_EF: {}".format(32*len(eit.eventFilterInfo())))
            for v in eit.eventFilterInfo():
                trigEF += "{0:032b}".format(v)[::-1]
            trigL1 = compressB64(trigL1)
            trigL2 = compressB64(trigL2)
            trigEF = compressB64(trigEF)
            _info("## trigL1: {}".format(trigL1))
            _info("## trigL2: {}".format(trigL2))
            _info("## trigEF: {}".format(trigEF))

            if self._eif is not None:
                eirec['L1PassedTrigMask'] = trigL1
                eirec['L2PassedTrigMask'] = trigL2
                eirec['EFPassedTrigMask'] = trigEF
                
            if self._eif_spb is not None:
                eventPB.L1PassedTrigMask = trigL1
                eventPB.L2PassedTrigMask = trigL2
                eventPB.EFPassedTrigMask = trigEF
        
        
            # if trigger chains were not read at BeginRun try now from detStore
            if self.cclenL1 == 0 or self.cclenL2 == 0 or self.cclenEF == 0:
                _info("triggers were not read  at BeginRun try now from detStore")
                self.getChainCountersFromDetStore()

            ## from Metadata
            SMK = self._iov.get('SMK',(run_number,event_number))
            L1PSK = self._iov.get('L1PSK',(run_number,lumi_block))
            HLTPSK = self._iov.get('HLTPSK',(run_number,lumi_block))
            _info('## SMK:    {}'.format(SMK))
            _info('## L1PSK:  {}'.format(L1PSK))
            _info('## HLTPSK: {}'.format(HLTPSK))

            if SMK is None:
                # try read keys from detector store
                try:
                    SMK    = self.detStore['/TRIGGER/HLT/HltConfigKeys']['MasterConfigurationKey']
                    L1PSK  = self.detStore['/TRIGGER/LVL1/Lvl1ConfigKey']['Lvl1PrescaleConfigurationKey']
                    if '/TRIGGER/HLT/PrescaleKey' in self.detStore.keys():
                        # LB-wise 
                        HLTPSK = self.detStore['/TRIGGER/HLT/PrescaleKey']['HltPrescaleKey']
                    else:
                        # Run-wise
                        HLTPSK = self.detStore['/TRIGGER/HLT/HltConfigKeys']['HltPrescaleConfigurationKey']
                except:
                    pass
                _info('## SMK*:    {}'.format(SMK))
                _info('## L1PSK*:  {}'.format(L1PSK))
                _info('## HLTPSK*: {}'.format(HLTPSK))

            if self._eif is not None:
                eirec['SMK']    = SMK
                eirec['L1PSK']  = L1PSK
                eirec['HLTPSK'] = HLTPSK

            if self._eif_spb is not None:
                eventPB.SMK     = ( SMK    if SMK    is not None else 0 )
                eventPB.L1PSK   = ( L1PSK  if L1PSK  is not None else 0 )
                eventPB.HLTPSK  = ( HLTPSK if HLTPSK is not None else 0 )
            
                if SMK != self.SMK_saved:
                	self.SMK_saved = SMK
                	self.writeTrigMenutoSPB(SMK,L1PSK,HLTPSK)

        # update trigger if TrigDecision info is available
        if self.DoTriggerInfo and self.HaveHlt:

            L1_isPassedAfterPrescale  = 0x1 << 16
            L1_isPassedBeforePrescale = 0x1 << 17
            L1_isPassedAfterVeto      = 0x1 << 18
            nlvl1=self.cclenL1
            _info("LEN_L1*: {}".format(3*nlvl1))
            trigL1X = list(3*nlvl1*"0")
            for pos,name in self.ccnameL1.items():
                passedBits = self.trigDec.isPassedBits(name)
                if passedBits & L1_isPassedBeforePrescale != 0:
                    trigL1X[pos] = "1"
                if passedBits & L1_isPassedAfterPrescale != 0:
                    trigL1X[pos+nlvl1] = "1"
                if passedBits & L1_isPassedAfterVeto != 0:
                    trigL1X[pos+2*nlvl1] = "1"
            trigL1=compressB64("".join(trigL1X))


            L2_passedRaw   = 0x1 << 8
            L2_passThrough = 0x1 << 9
            L2_prescaled   = 0x1 << 10
            L2_resurrected = 0x1 << 11
            nlvl2=self.cclenL2
            _info("LEN_L2*: {}".format(nlvl2))
            trigL2_PH = list(nlvl2*"0")
            trigL2_PT = list(nlvl2*"0")
            trigL2_RS = list(nlvl2*"0")
            for pos,name in self.ccnameL2.items():
                passedBits    = self.trigDec.isPassedBits(name)
                passedPhysics = self.trigDec.isPassed(name)
                passedPT      = ( passedBits & L2_passThrough != 0 )
                passedRes     = ( passedBits & L2_resurrected != 0 )
                if passedPhysics :
                    trigL2_PH[pos] = "1"
                if passedPT:
                    trigL2_PT[pos] = "1"
                if passedRes:
                    trigL2_RS[pos] = "1"


            trigL2_PH = compressB64("".join(trigL2_PH))
            trigL2_PT = compressB64("".join(trigL2_PT))
            trigL2_RS = compressB64("".join(trigL2_RS))
            trigL2 = trigL2_PH+";"+trigL2_PT+";"+trigL2_RS


            EF_passedRaw   = 0x1
            EF_passThrough = 0x1 << 1
            EF_prescaled   = 0x1 << 2
            EF_resurrected = 0x1 << 3
            nlvlEF = self.cclenEF
            _info("LEN_EF*: {}".format(nlvlEF))
            trigEFX    = list(nlvlEF*"0")
            trigEF_PH  = list(nlvlEF*"0")
            trigEF_PT  = list(nlvlEF*"0")
            trigEF_RS  = list(nlvlEF*"0")
            trigEF_INC = list(nlvlEF*"0")
            trigEF_RAW = list(nlvlEF*"0")
            for pos,name in self.ccnameEF.items():
                passedBits    = self.trigDec.isPassedBits(name)
                passedPhysics = self.trigDec.isPassed(name)
                passedPT      = ( passedBits & EF_passThrough != 0 )
                passedRes     = ( passedBits & EF_resurrected != 0 )
                passedRaw     = ( passedBits & EF_passedRaw != 0 )
                if passedRaw:
                    trigEF_RAW[pos] = "1"
                if passedPhysics :
                    trigEF_PH[pos]  = "1"
                if passedPT:
                    trigEF_PT[pos]  = "1"
                if passedRes:
                    trigEF_RS[pos]  = "1"
                if ( passedPhysics or passedPT or passedRes ):
                    trigEF_INC[pos] = "1"

            trigEF_RAW = compressB64("".join(trigEF_RAW))
            trigEF_PH  = compressB64("".join(trigEF_PH))
            trigEF_PT  = compressB64("".join(trigEF_PT))
            trigEF_RS  = compressB64("".join(trigEF_RS))
            trigEF_INC = compressB64("".join(trigEF_INC))
            trigEF  = trigEF_PH+";"+trigEF_PT+";"+trigEF_RS
            trigEF2 = trigEF_PH+";"+trigEF_INC

            _info("## trigL1*: {}".format(trigL1))
            _info("## trigL2*: {}".format(trigL2))
            _info("## trigEF*: {}".format(trigEF))
            _info("## trigRAW*: {}".format(trigEF_RAW))

            # overwrite 
            if self._eif is not None:
                eirec['L1PassedTrigMask'] = trigL1
                eirec['L2PassedTrigMask'] = trigL2
                eirec['EFPassedTrigMask'] = trigEF

            if self._eif_spb is not None:
                eventPB.L1PassedTrigMask = trigL1
                eventPB.L2PassedTrigMask = trigL2
                eventPB.EFPassedTrigMask = trigEF

        stream_refs  = {}  # sreeam reference
        Pstream_refs = {}  # provenance references
        procTag = None
        ## -- Stream references
        dh = store.retrieve('DataHeader')
        procTag = dh.getProcessTag()
        _info("## ProcessTag: " + procTag)

        ## provenance referencess
        if self.DoProvenanceRef:
            if dh.sizeProvenance() > 0:    
                prv = dh.beginProvenance()
                for i in range(0,dh.sizeProvenance()):
                    try:
                        tk = prv.getToken().toString()
                        match = self._re_pool_token(tk)
                    except:
                        tk = prv.getToken()
                        match = self._re_pool_token(tk)
                    if not match:
                        msg.warning('Provenance token can not be parsed: {}'.format(tk))
                        continue
                    d = match.groupdict()
                    key = prv.getKey()
                    # CNT is empty. Complete information
                    if key == "StreamRAW":
                        stk = "[DB={}][CNT=00000000][CLID={}][TECH={}][OID={}]".format(
                            d['db'],d['clid'],d['tech'],d['oid'])
                    elif key in ("StreamAOD","StreamESD","StreamRDO","StreamHITS",
                                 "StreamEVGEN","EmbeddingStream"):
                        stk = "[DB={}][CNT=POOLContainer(DataHeader)][CLID={}][TECH={}][OID={}]".format(
                            d['db'],d['clid'],d['tech'],d['oid'])
                    else:
                        _info("provenance {}={}".format(key,tk))
                        _error('Unknown provenance stream: {}'.format(key))
                        raise RuntimeError('Unknown provenance stream')
                    _info("## P"+ key+"_ref: "+stk)
                    Pstream_refs[key]=stk
                    prv += 1
    
        ## stream references
        if self._eif_spb is not None:
            tokenPB0 = eventPB.eitoken.add()
        if dh.size() > 0:
            dhe = dh.begin()
            for i in range(0,dh.size()):
                key = dhe.getKey()
                if key.startswith('Stream'):
                    _info("## Stream: "+key)
                if key in [ procTag, 'StreamAOD' ]:
                    try:
                        match = self._re_pool_token(dhe.getToken().toString())
                    except:
                        match = self._re_pool_token(dhe.getToken())
                    if not match:
                        continue
                    d = match.groupdict()
                    tk_guid = d['db']
                    stk = "[DB={}][CNT=POOLContainer(DataHeader)][CLID={}][TECH={}][OID={}]".format(
                        tk_guid,d['clid'],d['tech'],d['oid'])
                    _info("## "+ key+"_ref: "+stk)
                    if "ProcTag_ref" in stream_refs:
                        _info("Already inserted key ProcTag_ref in stream_refs with value "+stream_refs["ProcTag_ref"])
                    stream_refs["Sref0"] = stk
                    if self._eif_spb is not None:
                        if key == tokenPB0.name:
                            _info("Already inserted key {0} in tokenPB0 with value {1}".format(key,stk))
                        tokenPB0.name  = key
                        tokenPB0.token = stk
                dhe += 1

        # Update ref token to handle fast merged files.
        try:
            stk = store.proxy(dh).address().par().c_str()
            stream_refs["Sref0"] = stk
            if self._eif_spb is not None:
                tokenPB0.token = stk
            _info("Updated ref token "+stk)
        except:
            pass

        del dh

        if  self._eif is not None:
            for sr,v in stream_refs.iteritems():
                try:
                    eirec[sr] = v
                except:
                    _info("Unable to insert " + sr + " in stream references with value "+v)
                    pass
            idx=1
            for sr,v in Pstream_refs.iteritems():
                try:
                    eirec['Snam{:d}'.format(idx)] = sr
                    eirec['Sref{:d}'.format(idx)] = v
                except:
                    _info("Unable to insert " + sr + " in provenance stream references with value "+v)
                    pass
                idx += 1 


            eirec['Snam0'] = procTag
            self._eif['Entry_{:d}'.format(self._eif_totentries)] = eirec.getRec()


        if  self._eif_spb is not None:
            for sr,v in Pstream_refs.iteritems():
                try:
                    tokenPB = eventPB.eitoken.add()
                    tokenPB.name  = sr
                    tokenPB.token = v
                except:
                    _info("Unable to insert " + sr + " in provenance stream references with value "+v)
                    pass

            tokenPB0.name = procTag

            spb = eventPB.SerializeToString()
            self._eif_spb.write(struct.pack('<I', 6<<8 | 0x01 ))
            self._eif_spb.write(struct.pack('<I', len(spb)))
            self._eif_spb.write(spb)


        self._eif_entries += 1     # for this input file
        self._eif_totentries += 1  # for all input fies

        return StatusCode.Success

    def finalize(self):
        _info = self.msg.info
        _info("POOL2EI::finalize")

        if  self._eif is not None:
            self._eif['Nentries'] = self._eif_totentries
            self._eif['Nfiles'] = self._eif_nfiles
            self._eif['EndProcTime'] = int(time.time() * 1000)
            self._eif.close()

        if  self._eif_spb is not None:
            trailer = self.eipbof.Trailer()
            trailer.nfiles = self._eif_nfiles
            trailer.nentries = self._eif_totentries
            trailer.endProcTime = int(time.time() * 1000)

            spb = trailer.SerializeToString()
            self._eif_spb.write(struct.pack('<I', 2<<8 | 0x01 ))
            self._eif_spb.write(struct.pack('<I', len(spb)))
            self._eif_spb.write(spb)

            self._eif_spb.close()
            
        return StatusCode.Success
        

class POOL2EISvc(PyAthena.Svc):
    """Service
    """

    def __init__(self, name='POOL2EISvc', **kw):
        super(POOL2EISvc, self).__init__(name, **kw)
        _info = self.msg.info
        _info("POOL2EISvc::__init__")
        
        # whether we are inside beginFile ... endFile
        self.insideInputFile = False

        #save algorithm to call on incident
        if 'algo' in kw:
            self.algo = kw['algo']

    def initialize(self):
        # register with the incident svc
        _info = self.msg.info
        _info("POOL2EISvc::initialize")
        incsvc = PyAthena.py_svc('IncidentSvc', iface='IIncidentSvc')
        if not incsvc:
            self.msg.error('unable to get the incident svc')
            return StatusCode.Failure

        incsvc.addListener(self, 'BeginInputFile')
        incsvc.addListener(self, 'EndInputFile')
        #incsvc.addListener(self, 'BeginRun', 10)
        #incsvc.addListener(self, 'EndRun', 10)
        incsvc.addListener(self, 'EndEvtLoop')
        incsvc.release()

        return StatusCode.Success


    def finalize(self):
        _info = self.msg.info
        _info("POOL2EISvc::finalize")
        return StatusCode.Success

    def handle(self, incident):
        _info = self.msg.info
        #_info("POOL2EISvc::handle")
        tp = incident.type()
        if tp == 'EndEvent':
            pass
        elif tp == 'BeginInputFile':
            _info('POOL2EISvc::handle BeginInputFile')
            self.insideInputFile = True
            self.algo.beginFile()
            pass
        elif tp == 'EndInputFile':
            _info('POOL2EISvc::handle EndInputFile')
            self.insideInputFile = False
            self.algo.endFile()
            pass
        elif tp == 'EndEvtLoop':
            _info('POOL2EISvc::handle EndEvtLoop')
            # when maxEvents is reached, we are still insideInputFile
            if self.insideInputFile:
                self.algo.endFile()
            pass
        #elif tp == 'BeginRun':
        #    _info('POOL2EISvc::handle BeginRun')
        #    self.algo.beginRun()
        #    pass
        else:
            _info('POOL2EISvc::handle {}'.format(tp))
            pass
        return
    
    pass # class POOL2EISvc

