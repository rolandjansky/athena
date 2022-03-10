# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# @file POOL2EI_Lib.py
# @purpose provide components to get EventIndex data from pool files
# @author Javier Sanchez
# @date February 2014
# @update January 2022
#
# Some code borrowed from PyAthena.FilePeekerLib
# credits to Sebastien Binet


__version__ = "$Revision: 3.0 $"
__author__ = "Javier Sanchez"
__doc__ = "provide components to POOL2EI"


# imports -----------------------------------------------------------------
import AthenaPython.PyAthena as PyAthena
from .compressB64 import compressB64
from .EI_Lib import IOV
import EventIndexProducer.EIconstants as eic

import struct
from EventIndexProducer.EIPBof import EIPBof
import gzip

import time
StatusCode = PyAthena.StatusCode


def _import_ROOT():
    import ROOT
    ROOT.gROOT.SetBatch(True)
    return ROOT


def toiter(beg, end):
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return


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
    _initialized = False

    _eiversion = 0x00000001
    _eimagic = 0x6e56c8c7

    def __init__(self, name='POOL2EI', **kw):
        # init base class
        super(POOL2EI, self).__init__(name, **kw)

        _info = self.msg.info
        _info("POOL2EI::__init__")

    # ----------------------------------------
    # initialize
    # ----------------------------------------
    def initialize(self):

        import AthenaPython.PyAthena as PyAthena
        _info = self.msg.info
        _info("POOL2EI::initialize")

        # Am I call on every input file ?
        if self._initialized:
            return

        _info("## DoProvenanceRef: {}".format(self.DoProvenanceRef))
        _info("## DoTriggerInfo: {}".format(self.DoTriggerInfo))
        _info("## SendToBroker: {}".format(self.SendToBroker))

        self.eipbof = EIPBof(self._eiversion)         # initialize factory

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
                    dsSource = 'realDatasetsIn'    # dataset name from input
                    if (processingType == 'merge' and
                            transformation != 'POOLtoEI_tf.py'):
                        dsSource = 'realDatasets'  # dataset name from output
                    datasets = newJobDef.job[dsSource].split(',')
                    _info("## {}[0]: {}".format(dsSource, datasets[0]))
                    self._dsname = datasets[0]
                except Exception:
                    _info('## Unable to get dataset name from realDatasetsIn '
                          'or realDatasets')

        # remove _tid and _sub parts from dsname
        import re
        self._dsname_orig = self._dsname
        self._dsname = re.sub('_tid[0-9]{8}_[0-9]{2}', '', self._dsname)
        self._dsname = re.sub('_sub[0-9]{10}', '', self._dsname)
        self._dsname = re.sub('/$', '', self._dsname)

        # token match regex
        import re
        self._re_pool_token = re.compile(r'\[DB=(?P<db>.*?)\]'
                                         r'\[CNT=(?P<cnt>.*?)\]'
                                         r'\[CLID=(?P<clid>.*?)\]'
                                         r'\[TECH=(?P<tech>.*?)\]'
                                         r'\[OID=(?P<oid>.*?)\]').match

        # load our pythonizations:
        for cls_name in ('EventStreamInfo', 'EventType', 'PyEventType'):
            cls = getattr(PyAthena, cls_name)  # noqa: F841

        _info("retrieving various stores...")
        for store_name in ('evtStore', 'inputStore', 'detStore',
                           'tagStore', 'metaStore'):
            _info("retrieving [{}]...".format(store_name))
            o = getattr(self, store_name)  # noqa: F841
            _info("retrieving [{}]... [done]".format(store_name))
        _info("retrieving various stores... [done]")

        # Build output file names
        # self.EiFmt ignored
        # keep self.EiFmt format flag for compatibility and future usage
        if self.Out is not None:
            oname_spb = self.Out
        else:
            oname_spb = "output.ei.spb"

        # open EI output file
        oname_spb = os.path.expanduser(os.path.expandvars(oname_spb))
        self._eifname_spb = oname_spb
        _info('Opening EI SPB file [{}]...'.format(oname_spb))

        if os.path.exists(oname_spb):
            os.remove(oname_spb)

        try:
            self._eif_spb = gzip.open(oname_spb, 'wb')
        except Exception:
            self._eif_spb = None
            self.msg.fatal("Unable to open EI SPB output file {} "
                           "exapnded as {}".format(self.Out, oname_spb))
            raise RuntimeError("Unable to open EI SPB output file")

        self._eif_spb.write(struct.pack('<I', self._eimagic))    # EI SPB magic
        self._eif_spb.write(struct.pack('<I', self._eiversion))  # EI SPB ver

        # get taskid and jobid
        if (hasattr(self, 'TaskID') and hasattr(self, 'JobID') and
                self.TaskID is not None and self.JobID is not None):
            taskID = "{}.T".format(self.TaskID)
            if (hasattr(self, 'AttemptNumber') and
                    self.AttemptNumber is not None):
                jobID = "{}.{}".format(self.JobID, self.AttemptNumber)
            else:
                jobID = "{}.0".format(self.JobID)
        else:
            taskID = "{}.G".format(os.getenv('PanDA_TaskID', 0))
            jobID = "{}.{}".format(os.getenv('PandaID', 0),
                                   os.getenv('PanDA_AttemptNr', 0))

        # initial information SPB
        # write Header
        if self._eif_spb is not None:
            header = self.eipbof.Header()
            header.startProcTime = int(time.time() * 1000)
            header.taskID = taskID
            header.jobID = jobID
            header.inputDsName = self._dsname_orig
            header.provenanceRef = self.DoProvenanceRef
            header.triggerInfo = self.DoTriggerInfo

            spb = header.SerializeToString()
            self._eif_spb.write(struct.pack('<I', eic.EI_PROTO_HEADER << 8 |
                                            eic.EI_PROTO_MSGVER))
            self._eif_spb.write(struct.pack('<I', len(spb)))
            self._eif_spb.write(spb)

        self._initialized = True

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
        except KeyError:
            msg.warning('could not retrieve [{}]'.format(metadata_name))
            return ([], [])
        try:
            msg.info('processing container [{}]'.format(obj.folderName()))
        except Exception:
            msg.info('container does not have folderName')
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
        for ii, payload in zip(range(payloads_sz), payloads):
            if not payload:
                msg.info("**error** null-pointer ?")
                continue
            # names
            chan_names = []
            sz = payload.name_size()
            for idx in range(sz):
                chan = payload.chanNum(idx)
                chan_name = payload.chanName(chan)
                chan_names.append(chan_name)

            # iovs
            sz = payload.iov_size()
            for idx in range(sz):
                chan = payload.chanNum(idx)
                iov_range = payload.iovRange(chan)
                iov_start = iov_range.start()
                iov_stop = iov_range.stop()
                if (idx < 10):
                    msg.info('({}, {}) => ({}, {}) valid={} runEvt={}'.format(
                        iov_start.run(),
                        iov_start.event(),
                        iov_stop.run(),
                        iov_stop.event(),
                        iov_start.isValid(),
                        iov_start.isRunEvent()))
                elif (idx == 10):
                    msg.info("... {} more".format(sz-9))
                iovs.append((iov_start.run(), iov_start.event(),
                             iov_stop.run(), iov_stop.event(),
                             iov_start.isValid(), iov_start.isRunEvent(),
                             chan))

            # attrs
            attrs = []  # can't use a dict as spec.name() isn't unique
            sz = payload.size()
            for idx in range(sz):
                chan = payload.chanNum(idx)
                attr_list = payload.attributeList(chan)
                attr_data = []
                for a in list(toiter(attr_list.begin(), attr_list.end())):
                    spec = a.specification()
                    a_type = spec.typeName()
                    if a_type.find('string') >= 0:
                        a_data = a.data['string']()
                        try:
                            a_data = eval(a_data, {}, {})
                        except Exception:
                            # swallow and keep as a string
                            pass
                    else:
                        a_data = a.data[a_type]()
                    attr_data.append((spec.name(), a_data))
                attrs.append(dict(attr_data))
            if len(attrs) == len(chan_names):
                data.append(dict(zip(chan_names, attrs)))
            else:
                if len(attrs):
                    if len(attrs) == 1:
                        data.append(attrs[0])
                    else:
                        data.append(attrs)
                else:
                    data.append(chan_names)
            pass  # loop over payloads...

        return (data, iovs)

    # ----------------------------------------
    # execute at begin of file
    # ----------------------------------------
    def beginFile(self):

        _info = self.msg.info
        _info("POOL2EI::beginFile")

        # entries for this input file
        self._eif_entries = 0

        # define a new IOV storage
        self._iov = IOV()

        # ======================================================
        # retrieve the GUID
        # ======================================================

        def _get_guid():
            ROOT = _import_ROOT()
            root_files = list(ROOT.gROOT.GetListOfFiles())
            if len(root_files) == 0:
                _info('could not find correct ROOT file')
                return

            root_file = root_files[-1]  # get the last file in the list
            pool = root_file.Get("##Params")
            import re
            # Pool parameters are of the form:
            # '[NAME=somevalue][VALUE=thevalue]'
            pool_token = re.compile(r'\[NAME=(?P<name>.*?)\]'
                                    r'\[VALUE=(?P<value>.*?)\]').match
            params = []
            guids = []
            for i in range(pool.GetEntries()):
                if pool.GetEntry(i) > 0:
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

        # ======================================================
        # trigger menu
        # ======================================================
        if self.DoTriggerInfo:
            l1_menu = None
            l2_menu = None
            ef_menu = None
            hlt_menu = None
            if self.meta_triggermenu:
                _info("Load trigger menu from TriggerMenu")
                for e in self.inputStore['TriggerMenu']:
                    smk = e.smk()
                    l1psk = e.l1psk()
                    hltpsk = e.hltpsk()
                    chainIds = e.chainIds()
                    chainNames = e.chainNames()
                    itemCtpIds = e.itemCtpIds()
                    itemNames = e.itemNames()
                    l1_menu = {name: id for id, name in
                               zip(itemCtpIds, itemNames)}
                    hlt_menu = {name: id for id, name in
                                zip(chainIds, chainNames)}
                    break   # first element is enough

            elif self.meta_triggermenujson_l1 or self.meta_triggermenujson_hlt:
                _info("Load trigger menu from TriggerMenuJson_L1 and "
                      "TriggerMenuJson_HLT")
                import json
                for e in self.inputStore['TriggerMenuJson_L1']:
                    l1_menu_raw = json.loads(e.payload())
                    l1_items = l1_menu_raw['items']
                    l1_menu = {l1_key: l1_items[l1_key]['ctpid']
                               for l1_key in l1_items}
                    break   # first element is enough
                for e in self.inputStore['TriggerMenuJson_HLT']:
                    hlt_menu_raw = json.loads(e.payload())
                    hlt_chains = hlt_menu_raw['chains']
                    hlt_menu = {chain: hlt_chains[chain]['counter']
                                for chain in hlt_chains}
                    break   # first element is enough
            elif self.meta_hlt_menu or self.meta_lvl1_menu:
                _info("Load trigger menu from /TRIGGER/LVL1/Menu and "
                      "/TRIGGER/HLT/Menu")
                # /TRIGGER/LVL1/Menu
                (l1menu_info, l1menu_iovs) = self.process_metadata(
                    self.inputStore, '/TRIGGER/LVL1/Menu')
                # bit mask number is in channel IOV
                channels = [iov[6] for iov in l1menu_iovs]
                names = [e['ItemName'] for e in l1menu_info[0]]
                l1_menu = {name: id for name, id in zip(names, channels)}

                # /TRIGGER/HLT/Menu
                (hltmenu_info, hltmenu_iovs) = self.process_metadata(
                    self.inputStore, '/TRIGGER/HLT/Menu')
                l2_menu = {entry['ChainName']: entry['ChainCounter']
                           for entry in hltmenu_info[0]
                           if entry['ChainName'].startswith("L2_")}
                ef_menu = {entry['ChainName']: entry['ChainCounter']
                           for entry in hltmenu_info[0]
                           if entry['ChainName'].startswith("EF_")}
                hlt_menu = {entry['ChainName']: entry['ChainCounter']
                            for entry in hltmenu_info[0]
                            if entry['ChainName'].startswith("HLT_")}
            else:
                _info("Trigger Menu not found")

        # ======================================================
        # trigger config keys
        # ======================================================
        smk = 0
        l1psk = 0
        hltpsk = 0
        if self.meta_triggermenu:
            _info("Load trigger config keys from TriggerMenu")
            for e in self.inputStore['TriggerMenu']:
                smk = e.smk()
                l1psk = e.l1psk()
                hltpsk = e.hltpsk()
                break  # first element is enough
        if self.meta_hlt_hltconfigkeys:
            _info("Load trigger config keys from /TRIGGER/HLT/HltConfigKeys")
            # /TRIGGER/HLT/HltConfigKeys
            (hltck_info, hltck_iovs) = self.process_metadata(
                self.inputStore, '/TRIGGER/HLT/HltConfigKeys')
            smk_l = [x['MasterConfigurationKey'] for x in hltck_info]
            for val, iov in zip(smk_l, hltck_iovs):
                self._iov.add('SMK', val, iov[:4])
            smk = smk_l[0]
        if self.meta_hlt_prescalekey:
            _info("Load trigger config keys from /TRIGGER/HLT/PrescaleKey")
            # /TRIGGER/HLT/PrescaleKey
            (hltpk_info, hltpk_iovs) = self.process_metadata(
                self.inputStore, '/TRIGGER/HLT/PrescaleKey')
            hltpk_l = [x['HltPrescaleKey'] for x in hltpk_info]
            for val, iov in zip(hltpk_l, hltpk_iovs):
                self._iov.add('HLTPSK', val, iov[:4])
            hltpsk = hltpk_l[0]
        if self.meta_lvl1_lvl1configkey:
            _info("Load trigger config keys from /TRIGGER/LVL1/Lvl1ConfigKey")
            # /TRIGGER/LVL1/Lvl1ConfigKey
            (l1pk_info, l1pk_iovs) = self.process_metadata(
                self.inputStore, '/TRIGGER/LVL1/Lvl1ConfigKey')
            l1pk_l = [x['Lvl1PrescaleConfigurationKey'] for x in l1pk_info]
            for val, iov in zip(l1pk_l, l1pk_iovs):
                self._iov.add('L1PSK', val, iov[:4])
            l1psk = l1pk_l[0]

        # ======================================================
        # AMITag, triggerStreamOfFile and project_name
        # ======================================================
        _info("Load AMITag, triggerStreamOfFile and project_name "
              "from /TagInfo")

        (tginfo, tgiovs) = self.process_metadata(self.inputStore, '/TagInfo')
        amitag = "Unknown"
        trigStream = "Unknown"
        projName = "Unknown"
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

        # ======================================================
        # write BeginGUID
        # ======================================================

        if self._eif_spb is not None:
            beginGUID = self.eipbof.BeginGUID()
            beginGUID.startProcTime = int(time.time() * 1000)
            beginGUID.AMITag = amitag
            beginGUID.trigStream = trigStream
            beginGUID.projName = projName
            beginGUID.guid = self.guid

            spb = beginGUID.SerializeToString()
            self._eif_spb.write(struct.pack('<I', eic.EI_PROTO_BEGINGUID << 8 |
                                            eic.EI_PROTO_MSGVER))
            self._eif_spb.write(struct.pack('<I', len(spb)))
            self._eif_spb.write(spb)

        self._eif_nfiles += 1

        # ======================================================
        # write TriggerMenu
        # ======================================================

        if (self.DoTriggerInfo and self._eif_spb is not None
                and l1_menu is not None):
            _info("Write trigger menu to output SPB")
            tMenu = self.eipbof.TriggerMenu()
            tMenu.SMK = smk
            tMenu.L1PSK = l1psk
            tMenu.HLTPSK = hltpsk

            def d2l(d):
                menu = ["{}:{}".format(k, d[k]) for k in d]
                return ";".join(menu)

            if l1_menu is not None and len(l1_menu) > 0:
                tMenu.L1Menu = d2l(l1_menu)
            if l2_menu is not None and len(l2_menu) > 0:
                tMenu.L2Menu = d2l(l2_menu)
            if ef_menu is not None and len(ef_menu) > 0:
                tMenu.EFMenu = d2l(ef_menu)
            if hlt_menu is not None and len(hlt_menu) > 0:
                tMenu.HLTMenu = d2l(hlt_menu)

            # tMenu.L1Menu is required
            if len(tMenu.L1Menu) > 0:
                spb = tMenu.SerializeToString()
                self._eif_spb.write(struct.pack('<I',
                                                eic.EI_PROTO_TRIGGERMENU << 8 |
                                                eic.EI_PROTO_MSGVER))
                self._eif_spb.write(struct.pack('<I', len(spb)))
                self._eif_spb.write(spb)
            else:
                _info("Unable to write trigger menu to output SPB. "
                      "tMenu.L1Menu is empty")

        return

    # ----------------------------------------
    # execute at end of file
    # ----------------------------------------
    def endFile(self):

        _info = self.msg.info
        _info("POOL2EI::endFile")

        # write EndGUID
        if self._eif_spb is not None:
            endGUID = self.eipbof.EndGUID()
            endGUID.nentries = self._eif_entries
            endGUID.endProcTime = int(time.time() * 1000)

            spb = endGUID.SerializeToString()
            self._eif_spb.write(struct.pack('<I', eic.EI_PROTO_ENDGUID << 8 |
                                            eic.EI_PROTO_MSGVER))
            self._eif_spb.write(struct.pack('<I', len(spb)))
            self._eif_spb.write(spb)

        return

    # ----------------------------------------
    # execute at start of run
    # ----------------------------------------
    def start(self):

        _info = self.msg.info
        _info("POOL2EI::start")

        return StatusCode.Success

    # ----------------------------------------
    # execute event by event
    # ----------------------------------------
    def execute(self):

        if self._eif_totentries < 100:
            _info = self.msg.info
        else:
            _info = lambda *x: None  # noqa: E731
        _error = self.msg.error
        _warning = self.msg.warning

        _info("POOL2EI::execute")

        if self._eif_spb is not None:
            eventPB = self.eipbof.EIEvent()

        # ======================================================
        # Get EventInfo data
        # ======================================================

        store = self.evtStore

        if self.item_xaod_eventinfo:

            _info('=== [xAOD::EventInfo] ===')
            xei = store.retrieve('xAOD::EventInfo', 'EventInfo')
            run_number = xei.runNumber()
            event_number = xei.eventNumber()
            lumi_block = xei.lumiBlock()
            bunch_crossing_id = xei.bcid()
            time_stamp = xei.timeStamp()
            try:
                time_stamp_ns = xei.timeStampNSOffset()
            except Exception:
                _info('## Event does not have xAOD::EventInfo::'
                      'timeStampNSOffset()')
                time_stamp_ns = 0
            evt_type_bit_mask = xei.eventTypeBitmask()
            isSimulation = (evt_type_bit_mask & xei.IS_SIMULATION != 0)
            isTestBeam = (evt_type_bit_mask & xei.IS_TESTBEAM != 0)
            isCalibration = (evt_type_bit_mask & xei.IS_CALIBRATION != 0)
            mc_channel_number = 0
            mc_event_weight = 0.
            if isSimulation:
                try:
                    mc_channel_number = xei.mcChannelNumber()
                    mc_event_weight = xei.mcEventWeight()
                except Exception:
                    pass
            try:
                extendedLevel1ID = xei.extendedLevel1ID()
            except Exception:
                _info('## Event does not have xAOD::EventInfo::'
                      'extendedLevel1ID()')
                extendedLevel1ID = 0
            # there isn't eInfoTrigger in xAOD::EventInfo
            eInfoTrigger = None

            del xei

        elif self.item_eventinfo:

            evt_info_keys = store.keys('EventInfo')
            if len(evt_info_keys) != 1:
                _info('more than one EventInfo: {}'.format(evt_info_keys))
                _info(' ==> we\'ll use [{}]'.format(evt_info_keys[0]))
            sg_key = evt_info_keys[0]
            ei = store.retrieve('EventInfo', sg_key)
            _info('=== [EventInfo#{}] ==='.format(sg_key))
            eid = ei.event_ID()
            run_number = eid.run_number()
            event_number = eid.event_number()
            lumi_block = eid.lumi_block()
            bunch_crossing_id = eid.bunch_crossing_id()
            time_stamp = eid.time_stamp()
            time_stamp_ns = eid.time_stamp_ns_offset()
            eitype = ei.event_type()
            mc_channel_number = eitype.mc_channel_number()
            mc_event_weight = eitype.mc_event_weight()
            bm = list(eitype.bit_mask)
            # IS_SIMULATION / IS_DATA
            isSimulation = True if 'IS_SIMULATION' in bm else False
            # IS_TESTBEAM / IS_FROM_ATLAS_DET
            isTestBeam = True if 'IS_TESTBEAM' in bm else False
            # IS_CALIBRATION / IS_PHYSICS
            isCalibration = True if 'IS_CALIBRATION' in bm else False
            extendedLevel1ID = 0
            eInfoTrigger = ei.trigger_info()
            extendedLevel1ID = eInfoTrigger.extendedLevel1ID()

            del ei

        else:
            # FAIL
            raise RuntimeError('Unable to find neither '
                               'xAOD::EventInfo nor EventInfo')

        _info('## run_number: {:d}'.format(run_number))
        _info('## event_number: {:d}'.format(event_number))
        _info('## bunch_crossing_id: {:d}'.format(bunch_crossing_id))
        _info('## extendedLevel1ID: {:d}'.format(extendedLevel1ID))
        _info('## lumi_block: {:d}'.format(lumi_block))
        _info('## time_stamp: {:d}'.format(time_stamp))
        _info('## time_stamp_ns_offset: {:d}'.format(time_stamp_ns))
        _info('## EventWeight: {:f}'.format(mc_event_weight))
        _info('## McChannelNumber: {:d}'.format(mc_channel_number))
        _info('## isSimulation: {}'.format(isSimulation))
        _info('## isTestBeam: {}'.format(isTestBeam))
        _info('## isCalibration: {}'.format(isCalibration))

        if self._eif_spb is not None:
            eventPB.runNumber = run_number
            eventPB.eventNumber = event_number
            eventPB.lumiBlock = lumi_block
            eventPB.bcid = bunch_crossing_id
            eventPB.timeStamp = time_stamp
            eventPB.timeStampNSOffset = time_stamp_ns
            eventPB.mcEventWeight = mc_event_weight
            eventPB.mcChannelNumber = mc_channel_number
            eventPB.isSimulation = isSimulation
            eventPB.isCalibration = isCalibration
            eventPB.isTestBeam = isTestBeam
            eventPB.extendedLevel1ID = extendedLevel1ID

        # ======================================================
        # Trigger conf keys
        # ======================================================

        if self.item_xaod_TrigConfKeys:
            _info("Retrieve TrigConfKeys from xAOD::TrigConfKeys")
            tck = store.retrieve('xAOD::TrigConfKeys', 'TrigConfKeys')
            evt_smk = tck.smk()
            evt_l1psk = tck.l1psk()
            evt_hltpsk = tck.hltpsk()
            del tck
        elif (self.meta_hlt_hltconfigkeys or self.meta_hlt_prescalekey
              or self.meta_lvl1_lvl1configkey):
            _info("Retrieve TrigConfKeys from /TRIGGER/**")
            evt_smk = self._iov.get('SMK', (run_number, event_number))
            evt_l1psk = self._iov.get('L1PSK', (run_number, lumi_block))
            evt_hltpsk = self._iov.get('HLTPSK', (run_number, lumi_block))
        else:
            _info("Unable to retrieve TrigConfKeys")
            evt_smk = 0
            evt_l1psk = 0
            evt_hltpsk = 0

        _info('## smk: {}'.format(evt_smk))
        _info('## hltpsk: {}'.format(evt_hltpsk))
        _info('## l1psk: {}'.format(evt_l1psk))

        if self._eif_spb is not None:
            eventPB.SMK = evt_smk
            eventPB.HLTPSK = evt_hltpsk
            eventPB.L1PSK = evt_l1psk

        # ======================================================
        # Trigger bit masks
        # ======================================================

        if self.DoTriggerInfo:

            def v2b(v):  # vector elements to binary string
                res = ""
                for e in v:
                    res += "{0:032b}".format(e)[::-1]
                return res

            trigL1 = ""
            trigL2 = ""
            trigEF = ""

            if self.item_xaod_TrigDecision:

                _info("Get trigger bit masks form xAOD::TrigDecision")

                # retrieve xTrigDecision
                xtd = store.retrieve('xAOD::TrigDecision', 'xTrigDecision')

                # L1
                tbp = xtd.tbp()   # vector: 16 elements of 32 bits = 512 bits
                tap = xtd.tap()
                tav = xtd.tav()
                trigL1 = compressB64(v2b(tbp) + v2b(tap) + v2b(tav))

                # L2
                trigL2_PH = xtd.lvl2PassedPhysics()  # 256*32 bits = 8192 bits
                trigL2_PT = xtd.lvl2PassedThrough()
                trigL2_RS = xtd.lvl2Resurrected()
                trigL2 = "{};{};{}".format(
                    compressB64(v2b(trigL2_PH)),
                    compressB64(v2b(trigL2_PT)),
                    compressB64(v2b(trigL2_RS)))

                # EF
                trigEF_PH = xtd.efPassedPhysics()    # 256*32 bits = 8192 bits
                trigEF_PT = xtd.efPassedThrough()
                trigEF_RS = xtd.efResurrected()
                trigEF = "{};{};{}".format(
                    compressB64(v2b(trigEF_PH)),
                    compressB64(v2b(trigEF_PT)),
                    compressB64(v2b(trigEF_RS)))

                del xtd

            else:

                _info("Get trigger bit masks form eventInfo.trigger_info()")

                # get trigger from eventInfo.trigger_info()
                if eInfoTrigger is not None:
                    trigL1 = compressB64(v2b(eInfoTrigger.level1TriggerInfo()))
                    trigL2 = compressB64(v2b(eInfoTrigger.level2TriggerInfo()))
                    trigEF = compressB64(v2b(eInfoTrigger.eventFilterInfo()))

            _info("## trigL1: {}".format(trigL1))
            _info("## trigL2: {}".format(trigL2))
            _info("## trigEF: {}".format(trigEF))

            if self._eif_spb is not None:

                eventPB.L1PassedTrigMask = trigL1
                eventPB.L2PassedTrigMask = trigL2
                eventPB.EFPassedTrigMask = trigEF

        if eInfoTrigger is not None:
            del eInfoTrigger

        # ======================================================
        # Sel reference and Provenance
        # ======================================================

        Pstream_refs = {}  # provenance references
        procTag = None

        # -- Stream references
        dh = store.retrieve('DataHeader')
        procTag = dh.getProcessTag()

        if procTag == "":
            # pacth procTag ATEAM-782
            for el in dh.elements():
                if el.getPrimaryClassID() == 222376821:
                    procTag = el.getKey()
                    break
        _info("## ProcessTag: " + procTag)

        if self.DoProvenanceRef:

            # provenance references
            if dh.sizeProvenance() > 0:
                prv = dh.beginProvenance()
                for i in range(dh.sizeProvenance()):
                    try:
                        tk = prv.getToken().toString()
                        match = self._re_pool_token(tk)
                    except Exception:
                        tk = prv.getToken()
                        match = self._re_pool_token(tk)
                    if not match:
                        _warning('Provenance token can not be parsed: {}'
                                 .format(tk))
                        continue
                    d = match.groupdict()
                    key = prv.getKey()
                    if key.startswith("Output"):
                        _warning('Provenance toke starts with Output: {}'
                                 .format(key))
                        key = key[6:]
                    if key.startswith("Input"):
                        _warning('Provenance toke starts with Input: {}'
                                 .format(key))
                        key = key[5:]
                    # CNT migth be empty. Complete information
                    if key == "StreamRAW":
                        stk = "[DB={}][CNT={}][CLID={}]" \
                              "[TECH={}][OID={}]".format(
                                  d['db'], d['cnt'], d['clid'],
                                  d['tech'], d['oid'])
                    elif key in ("StreamAOD", "StreamESD", "StreamRDO",
                                 "StreamHITS", "StreamEVGEN",
                                 "EmbeddingStream"):
                        if d['cnt'] == "":
                            d['cnt'] = "POOLContainer(DataHeader)"
                        stk = "[DB={}][CNT={}][CLID={}]" \
                              "[TECH={}][OID={}]".format(
                                  d['db'], d['cnt'], d['clid'],
                                  d['tech'], d['oid'])
                    else:
                        _info("provenance {}={}".format(key, tk))
                        _error('Unknown provenance stream: {}'.format(key))
                        raise RuntimeError('Unknown provenance stream')
                    _info("## P" + key + "_ref: " + stk)
                    if key not in Pstream_refs:
                        # keep only the first provenance found for each straam
                        Pstream_refs[key] = stk
                    prv += 1
                del prv

        # stream references
        if self._eif_spb is not None:
            tokenPB0 = eventPB.eitoken.add()

        if dh.size() > 0:
            dhe = dh.begin()
            for i in range(dh.size()):
                key = dhe.getKey()
                if key.startswith('Stream'):
                    _info("## Stream: " + key)
                if key in [procTag, 'StreamAOD']:
                    try:
                        match = self._re_pool_token(dhe.getToken().toString())
                    except Exception:
                        match = self._re_pool_token(dhe.getToken())
                    if not match:
                        continue
                    d = match.groupdict()
                    if d['cnt'] == "":
                        d['cnt'] = "POOLContainer(DataHeader)"
                    stk = "[DB={}][CNT={}][CLID={}][TECH={}][OID={}]".format(
                        d['db'], d['cnt'], d['clid'], d['tech'], d['oid'])
                    _info("## " + key + "_ref: " + stk)
                    if self._eif_spb is not None:
                        if key == tokenPB0.name:
                            _info("Already inserted key {0} in tokenPB0 "
                                  "with value {1}".format(key, stk))
                        tokenPB0.name = key
                        tokenPB0.token = stk
                dhe += 1
            del dhe

        # Update ref token to handle fast merged files.
        try:
            stk = store.proxy(dh).address().par().c_str()
            if self._eif_spb is not None:
                tokenPB0.token = stk
            _info("Updated ref token " + stk)
            del stk
        except Exception:
            pass

        if self._eif_spb is not None:
            for sr in Pstream_refs:
                try:
                    tokenPB = eventPB.eitoken.add()
                    tokenPB.name = sr
                    tokenPB.token = Pstream_refs[sr]
                except Exception:
                    _info("Unable to insert {} in provenance stream "
                          "references with value {}".format(
                              sr, Pstream_refs[sr]))
                    pass

        del dh

        # ======================================================
        # Write event EI
        # ======================================================

        if self._eif_spb is not None:
            spb = eventPB.SerializeToString()
            self._eif_spb.write(struct.pack('<I', eic.EI_PROTO_EIEVENT << 8 |
                                            eic.EI_PROTO_MSGVER))
            self._eif_spb.write(struct.pack('<I', len(spb)))
            self._eif_spb.write(spb)
            del eventPB

        self._eif_entries += 1     # for this input file
        self._eif_totentries += 1  # for all input files

        store.clearStore()

        return StatusCode.Success

    # ----------------------------------------
    # execute at the end of processing
    # ----------------------------------------
    def finalize(self):

        _info = self.msg.info
        _info("POOL2EI::finalize")

        if self._eif_spb is not None:
            trailer = self.eipbof.Trailer()
            trailer.nfiles = self._eif_nfiles
            trailer.nentries = self._eif_totentries
            trailer.endProcTime = int(time.time() * 1000)

            spb = trailer.SerializeToString()
            self._eif_spb.write(struct.pack('<I', eic.EI_PROTO_TRAILER << 8 |
                                            eic.EI_PROTO_MSGVER))
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

        # save algorithm to call on incident
        if 'algo' in kw:
            _info("POOL2EISvc::__init__ algo: {}".format(kw['algo']))
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
        incsvc.addListener(self, 'EndEvtLoop')
        incsvc.release()

        return StatusCode.Success

    def finalize(self):
        _info = self.msg.info
        _info("POOL2EISvc::finalize")
        return StatusCode.Success

    def handle(self, incident):
        _info = self.msg.info
        tp = incident.type()
        if tp == 'EndEvent':
            pass
        elif tp == 'BeginInputFile':
            _info('POOL2EISvc::handle BeginInputFile')
            self.insideInputFile = True
            self.algo.beginFile()
        elif tp == 'EndInputFile':
            _info('POOL2EISvc::handle EndInputFile')
            self.insideInputFile = False
            self.algo.endFile()
        elif tp == 'EndEvtLoop':
            _info('POOL2EISvc::handle EndEvtLoop')
            # when maxEvents is reached, we are still insideInputFile
            if self.insideInputFile:
                self.algo.endFile()
        else:
            _info('POOL2EISvc::handle {}'.format(tp))
        return

    pass  # class POOL2EISvc
