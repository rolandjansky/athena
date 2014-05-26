# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file POOL2EI_Lib.py
# @purpose provide components to get EventIndex data from pool files
# @author Javier Sanchez
# @date February 2010
# 
# Some code borrowed from PyAthena.FilePeekerLib
# credits to Sebastien Binet 

__version__= "$Revision: 000001 $"
__author__ = "Javier Sanchez"
__doc__ = "provide components to POOL2EI"


### imports --------------------------------------------------------------------
#import pdb
import AthenaPython.PyAthena as PyAthena
from compressB64 import compressB64
from EI_Lib import EIrecord, IOV
#from pympler import summary
#from pympler import muppy

import time
StatusCode = PyAthena.StatusCode


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

    def __init__(self, name='POOL2EI', **kw):
        ## init base class
        super(POOL2EI, self).__init__(name, **kw)
        
        _info = self.msg.info
        _info("POOL2EI::__init__")


        
    def initialize(self):

        import AthenaPython.PyAthena as PyAthena
        _info = self.msg.info
        _info("POOL2EI::initialize")

        _info("## DoProvenanceRef: %s" % self.DoProvenanceRef);
        _info("## DoTriggerInfo: %s" % self.DoTriggerInfo);

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
        for store_name in ('evtStore', 'inputStore',
                           'tagStore', 'metaStore'):
            _info('retrieving [%s]...', store_name)
            o = getattr(self, store_name)
            _info('retrieving [%s]... [done]', store_name)
            ## if store_name not in ('evtStore',):
            ##     _info('loading event proxies for [%s]...', store_name)
            ##     status = 'ok' if o.loadEventProxies().isSuccess() else 'err'
            ##     _info('loading event proxies for [%s]... (%s)', store_name, status)
        _info("retrieving various stores... [done]")


        ## open output pkl file
        import os
        oname = self.Out
        oname = os.path.expanduser(os.path.expandvars(oname))
        _info('Opening EI file [%s]...', oname)
        if os.path.exists(oname):
            os.remove(oname)

        import PyUtils.dbsqlite as dbsqlite

        try:
            self._eif = dbsqlite.open(oname,flags='w')
        except:
            pass

        if self._eif is None:
            self.msg.fatal("Unable to open EI output file %s exapnded as %s" % (self.Out, oname))
            raise RuntimeError("Unable to open EI output file")
        
        # initial information
        self._eif['StartProcTime'] = int(time.time() * 1000)
        self._eif['Schema'] = EIrecord().getRecSchema()
        self._eif['Version'] = EIrecord().getVersion()
        self._eif['PandaID'] = os.getenv('PandaID', 0)
        self._eif['PanDA_TaskID'] = os.getenv('PanDA_TaskID', 0)
        
        #processing options
        self._eif['ProvenanceRef'] = self.DoProvenanceRef
        self._eif['TriggerInfo'] = self.DoTriggerInfo


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
    def inputStore(self):
        import AthenaPython.PyAthena as PyAthena
        return PyAthena.py_svc('StoreGateSvc/InputMetaDataStore')

    def process_metadata(self, store, metadata_name):
        msg = self.msg
        try:
            obj = store[metadata_name]
        except KeyError,err:
            msg.warning('could not retrieve [%s]', metadata_name)
            return ([],[])
        msg.info('processing container [%s]', obj.folderName())
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
            #msg.info('==names== (sz: %s)', sz)
            for idx in xrange(sz):
                chan = payload.chanNum(idx)
                chan_name = payload.chanName(chan)
                #msg.info( '--> (%s, %s)', idx, chan_name)
                chan_names.append(chan_name)
                
            # iovs
            sz = payload.iov_size()
            #msg.info('==iovs== (sz: %s)',sz)
            for idx in xrange(sz):
                chan = payload.chanNum(idx)
                iov_range = payload.iovRange(chan)
                iov_start = iov_range.start()
                iov_stop  = iov_range.stop()
                msg.info( '(%s, %s) => (%s, %s) valid=%s runEvt=%s',
                          iov_start.run(),
                          iov_start.event(),
                          iov_stop.run(),
                          iov_stop.event(),
                          iov_start.isValid(),
                          iov_start.isRunEvent())
                iovs.append((iov_start.run(),iov_start.event(),iov_stop.run(),iov_stop.event(),iov_start.isValid(),iov_start.isRunEvent()))
                    
            # attrs
            attrs = [] # can't use a dict as spec.name() isn't unique
            sz = payload.size()
            #msg.info('==attrs== (sz: %s)', sz)
            for idx in xrange(sz):
                chan = payload.chanNum(idx)
                #msg.info("idx: %i chan: %s", idx, chan)
                attr_list = payload.attributeList(chan)
                #pdb.set_trace()
                attr_list = list(toiter(attr_list.begin(),
                                        attr_list.end()))
                attr_data = []
                for a in attr_list:
                    #msg.info((a,dir(a),type(a)))
                    spec   = a.specification()
                    a_type = spec.typeName()
                    a_type = 'std::string' if a_type == 'string' else a_type
                    a_data = getattr(a,'data<%s>'%a_type)()
                    if a_type == 'std::string':
                        try:
                            a_data = eval(a_data,{},{})
                        except Exception:
                            # swallow and keep as a string
                            pass
                    #msg.info("%s: %s", spec.name(), a_data)
                    attr_data.append((spec.name(),
                                      #type(a_data),
                                      a_data))
                attrs.append(dict(attr_data))
                # msg.info(attrs[-1])
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
        #self.inputStore.dump()
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
                _info('=== [EventStreamInfo#%s] ===', sg_key)
                nentries += esi.getNumberOfEvents()
                evt_types = PyAthena.EventStreamInfo.evt_types(esi)
            #print 'nentries ', nentries
            #print 'evt_types ', evt_types


        ##/TRIGGER/HLT/HltConfigKeys
        (hltck_info, hltck_iovs) = self.process_metadata(self.inputStore,'/TRIGGER/HLT/HltConfigKeys')
        hltpsk_l = [ x['HltPrescaleConfigurationKey'] for x in hltck_info ]
        for val, iov in zip(hltpsk_l,hltck_iovs):
            self._iov.add('HLTPSK',val,iov[:4])
        smk_l = [ x['MasterConfigurationKey'] for x in hltck_info ]
        for val, iov in zip(smk_l,hltck_iovs):
            self._iov.add('SMK',val,iov[:4])

        ##/TRIGGER/LVL1/Lvl1ConfigKey
        (l1ck_info, l1ck_iovs) = self.process_metadata(self.inputStore,'/TRIGGER/LVL1/Lvl1ConfigKey')
        l1ck_l = [ x['Lvl1PrescaleConfigurationKey'] for x in l1ck_info ]
        for val, iov in zip(l1ck_l,l1ck_iovs):
            self._iov.add('L1PSK',val,iov[:4])
            

        (tginfo, tgiovs) = self.process_metadata(self.inputStore,'/TagInfo')
        #pdb.set_trace()
        amitag=None
        trigStream=None
        projName=None
        if len(tginfo) > 0:
            for tgi in tginfo:
                if 'AMITag' in tgi:
                    amitag = tgi['AMITag']
                    _info("## AMITag: %s" %amitag)
                if 'triggerStreamOfFile' in tgi:
                    trigStream = tgi['triggerStreamOfFile']
                    _info("## triggerStreamOfFile: %s" % trigStream)
                if 'project_name' in tgi:
                    projName = tgi['project_name']
                    _info("## project_name: %s" % projName)
        

        
        if  self._eif is not None:
            nfile = self._eif_nfiles
            self._eif['StartProcTime_%d' % nfile] = int(time.time() * 1000)
            self._eif['AMITag_%d' % nfile] = amitag
            self._eif['TrigStream_%d' % nfile] = trigStream
            self._eif['ProjName_%d' % nfile] = projName

        self._eif_nfiles += 1
            
        #self._iov.dump()

        return


    ##########################################
    # execute at end of file
    ##########################################
    def endFile(self):
        import AthenaPython.PyAthena as PyAthena
        _info = self.msg.info
        _info("POOL2EI::endFile")

        nfile = self._eif_nfiles-1
        self._eif['Nentries_%d'%nfile] = self._eif_entries
        self._eif['EndProcTime_%d'%nfile] = int(time.time() * 1000)

    ##########################################
    # execute event by event
    ##########################################
    def execute(self):
        import AthenaPython.PyAthena as PyAthena
        _info = self.msg.info
        _error = self.msg.error

        _info("POOL2EI::execute")

        eirec = EIrecord()

        # -- Get EventInfo data
        store = self.evtStore
        evt_info_keys = store.keys('EventInfo')
        if len(evt_info_keys) != 1:
            _info('more than one EventInfo: %s', evt_info_keys)
            _info(' ==> we\'ll use [%s]', evt_info_keys[0])
        sg_key = evt_info_keys[0]
        ei = store.retrieve('EventInfo', sg_key)
        _info('=== [EventInfo#%s] ===', sg_key)
        eid = ei.event_ID()
        _info('## bunch_crossing_id: %d', eid.bunch_crossing_id())
        #_info('## detector_mask: %d', eid.detector_mask())
        #_info('## detector_mask0: %d', eid.detector_mask0())
        #_info('## detector_mask1: %d', eid.detector_mask1())
        _info('## event_number: %d', eid.event_number())
        _info('## lumi_block: %d', eid.lumi_block())
        _info('## run_number: %d', eid.run_number())
        _info('## time_stamp: %d', eid.time_stamp())
        _info('## time_stamp_ns_offset: %d', eid.time_stamp_ns_offset())

        
        # event type
        eitype = ei.event_type()
        bm = list(eitype.bit_mask)
        if 'IS_SIMULATION' in bm:
            eirec['IsSimulation']=1            # IS_SIMULATION
        else:
            eirec['IsSimulation']=0            # IS_DATA

        if 'IS_TESTBEAM' in bm:
            eirec['IsTestBeam']=1              # IS_TESTBEAM
        else:
            eirec['IsTestBeam']=0              # IS_FROM_ATLAS_DET

        if 'IS_CALIBRATION' in bm:
            eirec['IsCalibration']=1           # IS_CALIBRATION
        else:
            eirec['IsCalibration']=0           # IS_PHYSICS
        


        run_number=eid.run_number()
        event_number=eid.event_number()
        lumi_block=eid.lumi_block()
        
        eirec['RunNumber'] = run_number
        eirec['EventNumber'] = event_number
        eirec['LumiBlockN'] = lumi_block
        eirec["BunchId"] = eid.bunch_crossing_id()
        eirec['EventTime'] = eid.time_stamp()
        eirec['EventTimeNanoSec'] = eid.time_stamp_ns_offset() 
        eirec['EventWeight'] = eitype.mc_event_weight()
        eirec['McChannelNumber'] = eitype.mc_channel_number()

        _info('## EventWeight: %f', eitype.mc_event_weight())
        _info('## McChannelNumber: %d', eitype.mc_channel_number())

        # -- trigger processing
        if self.DoTriggerInfo:
            eit = ei.trigger_info()
            _info("## Lvl1ID %s" % eit.extendedLevel1ID())
            Lvl1ID = eit.extendedLevel1ID()
            eirec['Lvl1ID'] = Lvl1ID
            trigL1=""
            trigL2=""
            trigEF=""
            n=0
            for v in eit.level1TriggerInfo():
                trigL1+="{0:032b}".format(v)[::-1]
                if v != 0:
                    _info("L1PassedTrigMask%s%d: %d" % (['TBP','TAP','TAV'][n/8],n%8,v))
                n+=1
            n=0
            for v in eit.level2TriggerInfo():
                trigL2+="{0:032b}".format(v)[::-1]
                if v != 0:
                    _info("L2PassedTrigMask%d: %d"%(n, v))
                n+=1
            n=0
            for v in eit.eventFilterInfo():
                trigEF+="{0:032b}".format(v)[::-1]
                if v != 0:
                    _info("EFPassedTrigMask%d: %d"%(n, v))
                n+=1
            trigL1=compressB64(trigL1)
            trigL2=compressB64(trigL2)
            trigEF=compressB64(trigEF)
            _info("## trigL1: %s"% trigL1)
            _info("## trigL2: %s"% trigL2)
            _info("## trigEF: %s"% trigEF)

            eirec['L1PassedTrigMask'] = trigL1
            eirec['L2PassedTrigMask'] = trigL2
            eirec['EFPassedTrigMask'] = trigEF
        
        
            ## form Metadata
            SMK = self._iov.get('SMK',(run_number,event_number))
            L1PSK = self._iov.get('L1PSK',(run_number,lumi_block))
            HLTPSK = self._iov.get('HLTPSK',(run_number,lumi_block))
            _info('## SMK:    %s'% SMK)
            _info('## L1PSK:  %s'% L1PSK)
            _info('## HLTPSK: %s'% HLTPSK)

            eirec['SMK'] = SMK
            eirec['L1PSK'] = L1PSK
            eirec['HLTPSK'] = HLTPSK
        

        stream_refs = {}   # sreeam reference
        Pstream_refs = {}  # provenance references
        procTag = None
        ## -- Stream references
        dh = store.retrieve('DataHeader')
        procTag = dh.getProcessTag()
        _info("## ProcessTag: " + procTag)

        #pdb.set_trace()
        ## provenance referencess
        if self.DoProvenanceRef:
            if dh.sizeProvenance() > 0:    
                prv = dh.beginProvenance()
                for i in range(0,dh.sizeProvenance()):
                    try:
                        tk=prv.getToken().toString()
                        match = self._re_pool_token(tk)
                    except:
                        tk=prv.getToken()
                        match = self._re_pool_token(tk)
                    if not match:
                        msg.warning('Provenance token can not be parsed: %s', tk)
                        continue
                    d = match.groupdict()
                    key=prv.getKey()
                    # CNT is empty. Complete information
                    if key == "StreamRAW":
                        stk = "[DB=%s][CNT=00000000][CLID=%s][TECH=%s][OID=%s]" % \
                            (d['db'],d['clid'],d['tech'],d['oid'])
                    elif key in ("StreamAOD","StreamESD","StreamRDO","StreamHITS",
                                 "StreamEVGEN","EmbeddingStream"):
                        stk = "[DB=%s][CNT=POOLContainer(DataHeader)][CLID=%s][TECH=%s][OID=%s]" % \
                            (d['db'],d['clid'],d['tech'],d['oid'])
                    else:
                        _info("provenance %s=%s" % (key,tk))
                        _error('Unknown provenance stream: %s', key)
                        raise RuntimeError('Unknown provenance stream')
                    _info("## P"+ key+"_ref: "+stk)
                    Pstream_refs[key]=stk
                    prv += 1
    
        ## stream references
        for dhe in dh:
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
                stk = "[DB=%s][CNT=POOLContainer(DataHeader)][CLID=%s][TECH=%s][OID=%s]" \
                    % (tk_guid,d['clid'],d['tech'],d['oid'])
                _info("## "+ key+"_ref: "+stk)
                ##stream_refs[key+"_ref"]=stk
                if "ProcTag_ref" in stream_refs:
                    _info("Already inserted key ProcTag_ref in stream_refs with value "+stream_refs["ProcTag_ref"])
                stream_refs["Sref0"]=stk
        try:
            # Update ref token to handle fast merged files.
            stk = store.proxy(dh).address().par().c_str()
            stream_refs["Sref0"]=stk
            _info("Updated ref token "+stk)
        except:
            pass
        #print "stream_refs: ", stream_refs
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
                    eirec['Snam%d'%idx] = sr
                    eirec['Sref%d'%idx] = v
                except:
                    _info("Unable to insert " + sr + " in provenance stream references with value "+v)
                    pass
                idx += 1 


            eirec['Snam0'] = procTag
            self._eif['Entry_%d' % self._eif_totentries] = eirec.getRec()
            self._eif_entries += 1     # for this input file
            self._eif_totentries += 1  # for all input fies


        """
        all_objects = muppy.get_objects()
        sum1 = summary.summarize(all_objects)
        summary.print_(sum1)
        """

        return StatusCode.Success

    def finalize(self):
        _info = self.msg.info
        _info("POOL2EI::finalize")

        if  self._eif is not None:
            self._eif['Nentries'] = self._eif_totentries
            self._eif['Nfiles'] = self._eif_nfiles
            self._eif['EndProcTime'] = int(time.time() * 1000)
            self._eif.close()
 

        return StatusCode.Success
        

class POOL2EISvc(PyAthena.Svc):
    """Service
    """

    def __init__(self, name='POOL2EISvc', **kw):
        super(POOL2EISvc, self).__init__(name, **kw)
        _info = self.msg.info
        _info("POOL2EISvc::__init__")

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
        incsvc.release()

        return StatusCode.Success


    def finalize(self):
        _info = self.msg.info
        _info("POOL2EISvc::finalize")
        return StatusCode.Success

    def handle(self, incident):
        _info = self.msg.info
        _info("POOL2EISvc::handle")
        tp = incident.type()
        if tp == 'EndEvent':
            pass
        elif tp == 'BeginInputFile':
            self.msg.info('POOL2EISvc::handle BeginInputFile')
            self.algo.beginFile()
            pass
        elif tp == 'EndInputFile':
            self.msg.info('POOL2EISvc::handle EndInputFile')
            self.algo.endFile()
            pass
        else:
            pass
        return
    
    pass # class POOL2EISvc

