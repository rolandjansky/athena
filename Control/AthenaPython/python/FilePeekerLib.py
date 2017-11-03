# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyAthena.FilePeekerLib
# @purpose provide components to peek into pool files
# @author Sebastien Binet
# @date February 2010

__version__= "$Revision: 668532 $"
__author__ = "Sebastien Binet"
__doc__ = "provide components to peek into pool files"

### imports --------------------------------------------------------------------
import AthenaPython.PyAthena as PyAthena
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


### helper functions ----------------------------------------------------------
def _import_ROOT():
    import sys
    sys.argv.insert(1, '-b')
    import ROOT
    del sys.argv[1]
    return ROOT

### ---
def toiter(beg,end):
    while beg != end:
        yield beg.__deref__()
        beg.__preinc__()
    return

def _create_file_infos():
    """simple helper function to create consistent dicts for the
    fileinfos attribute of AthFile
    """
    d = {
        #'file_md5sum': None,  # provided by AthFile.impl
        #'file_name': None,    # ditto
        #'file_type': None,    # ditto
        #'file_guid': None,    # ditto
        
        'nentries' : 0, # to handle empty files
        'run_number': [],
        'run_type': ['N/A'],
        'evt_type': [],
        'evt_number': [],
        'lumi_block': [],
        'mc_channel_number': [],
        'beam_type':       ['N/A'], # XXX fixme
        'beam_energy':     ['N/A'], # XXX fixme
        'stream_tags': [],
        'metadata_items': [],
        'eventdata_items': [],
        'stream_names': None,
        'geometry': None,
        'conditions_tag': None,
        'det_descr_tags': None,
        ##
        'metadata': {},
        'tag_info': {},
        }
    return d

### classes --------------------------------------------------------------------

class sg_versioned_key(object):
    def __init__(self, sgkey):
        self.raw_key = sgkey
    @property
    def key(self):
        return self.raw_key.split(";")[-1]
        
class FilePeeker(PyAthena.Alg):
    """utility algorithm to inspect a file's content
    """
    def __init__(self, name='FilePeeker', **kw):
        ## init base class
        super(FilePeeker, self).__init__(name, **kw)
        self.infname = kw.get('infname', 'not-there.pool')
        self.outfname= kw.get('outfname', None)

        # flag to enable the bwd compat fallback mechanism...
        self._old_file_flag = False
        
        # flag to enable event less files without beginRun...
        self._begin_run_flag = False

        # all data, collected over the events analyzed
        self._peeked_events = []
        
    def initialize(self):

        import AthenaPython.PyAthena as PyAthena
        self.peeked_data = _create_file_infos()
        
        # load our pythonizations:
        for cls_name in ('EventStreamInfo',
                         'EventType',
                         'PyEventType',
                         ):
            cls = getattr(PyAthena, cls_name)

        _info = self.msg.info
        _info("retrieving various stores...")
        for store_name in ('evtStore', 'inputStore',
                           'tagStore', 'metaStore',):
            _info('retrieving [%s]...', store_name)
            o = getattr(self, store_name)
            _info('retrieving [%s]... [done]', store_name)
            ## if store_name not in ('evtStore',):
            ##     _info('loading event proxies for [%s]...', store_name)
            ##     status = 'ok' if o.loadEventProxies().isSuccess() else 'err'
            ##     _info('loading event proxies for [%s]... (%s)', store_name, status)
            
        _info("retrieving various stores... [done]")

        import os
        self.infname = os.path.basename(self.infname)

        import AthenaPython.PyAthena as PyAthena
        _info = self.msg.info
        return StatusCode.Success
        
    def beginRun(self):
        self._begin_run_flag = True
        # retrieving data available at start...
        self.peeked_data.update(self._do_peeking(peek_evt_data=False))
        self.print_summary()
        return StatusCode.Success

    def endRun(self):
        if not self._begin_run_flag:
            # retrieving data for event less jobs, where no beginRun is called
            self.peeked_data.update(self._do_peeking(peek_evt_data=False))
            self.print_summary()

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
    
    def execute(self):
        _info = self.msg.info
        self.peeked_data.update(self._do_peeking(peek_evt_data=True))
        self.print_summary()
        self._peeked_events.append(dict(self.peeked_data))
        return StatusCode.Success

    def process_metadata(self, store, metadata_name):
        msg = self.msg
        try:
            obj = store[metadata_name]
        except KeyError,err:
            msg.warning('could not retrieve [%s]', metadata_name)
            return
        msg.info('processing container [%s]', obj.folderName())
        data = []
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
            # print "-->",ii,payload,type(payload),'\n' 
            if not payload:
                msg.info ("**error** null-pointer ?")
                continue
            # names
            chan_names = []
            sz = payload.name_size()
            msg.info('==names== (sz: %s)', sz)
            for idx in xrange(sz):
                chan = payload.chanNum(idx)
                chan_name = payload.chanName(chan)
                #msg.info( '--> (%s, %s)', idx, chan_name)
                chan_names.append(chan_name)
                
            if 1: # we don't really care about those...
                # iovs
                sz = payload.iov_size()
                msg.info('==iovs== (sz: %s)',sz)
                for idx in xrange(sz):
                    chan = payload.chanNum(idx)
                    iov_range = payload.iovRange(chan)
                    iov_start = iov_range.start()
                    iov_stop  = iov_range.stop()
                    if 0:
                        msg.info( '(%s, %s) => (%s, %s) valid=%s runEvt=%s',
                                  iov_start.run(),
                                  iov_start.event(),
                                  iov_stop.run(),
                                  iov_stop.event(),
                                  iov_start.isValid(),
                                  iov_start.isRunEvent())
        
            # attrs
            attrs = [] # can't use a dict as spec.name() isn't unique
            sz = payload.size()
            msg.info('==attrs== (sz: %s)', sz)
            for idx in xrange(sz):
                chan = payload.chanNum(idx)
                #msg.info("idx: %i chan: %s", idx, chan)
                attr_list = payload.attributeList(chan)
                attr_data = []
                for a in list(toiter(attr_list.begin(), attr_list.end())):
                    #msg.info((a,dir(a),type(a)))
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
                        a_data = getattr(a,'data<%s>'%a_type)()
                    #msg.info("%s: %s  %s", spec.name(), a_data, type(a_data) )
                    attr_data.append( (spec.name(), a_data) )
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
        
        #payload.dump()
        ##  if len(data)>1 and obj.folderName() == "/TagInfo":
        ##      print "="*80,metadata_name
        ##      for d in data:
        ##        print "==",d
        return data

    def finalize(self):
        _info = self.msg.info
        peeked_data = dict(self.peeked_data)
        if self.outfname:
            oname = self.outfname
            import os
            oname = os.path.expanduser(os.path.expandvars(oname))
            _info('storing peeked file infos into [%s]...', oname)
            if os.path.exists(oname):
                os.remove(oname)
            try:
                import cPickle as pickle
            except ImportError:
                import pickle
            import PyUtils.dbsqlite as dbsqlite
            db = dbsqlite.open(oname,flags='w')
            
            # merge and collect data from all processed events (if any)
            if self._peeked_events:
                peeked_data = self._peeked_events.pop(0)
                for d in self._peeked_events:
                    for k in ('run_number',
                              'evt_number',
                              'lumi_block',
                              'run_type',
                              'beam_type',
                              'beam_energy',
                              'stream_tags',):
                        v = d[k]
                        if isinstance(v, list) and len(v)>0:
                            v = v[0]
                            peeked_data[k].append(v)
            #_info('peeked_data:')
            #_info(str(peeked_data))
            db['fileinfos'] = peeked_data
            db.close()
            _info('storing peeked file infos into [%s]... [done]', oname)
        self.print_summary(peeked_data)
        return StatusCode.Success

    def print_summary(self, data=None):
        if data is None:
            data = self.peeked_data
        _info = self.msg.info

        ## -- summary
        _info(':::::: summary ::::::')
        _info(' - nbr events:  %s', data['nentries'])
        _info(' - run numbers: %s', data['run_number'])
        _info(' - evt numbers: %s', data['evt_number'])
        _info(' - lumiblocks: %s', data['lumi_block'])
        _info(' - evt types: %s', data['evt_type'])
        _info(' - item list: %s', len(data['eventdata_items']))
        _info(' - stream names: %s', data['stream_names'])
        _info(' - stream tags: %s', data['stream_tags'])
        _info(' - geometry: %s', data['geometry'])
        _info(' - conditions tag: %s', data['conditions_tag'])
        _info(' - metadata items: %s', len(data['metadata_items']))
        _info(' - tag-info: %s', data['tag_info'].keys())
        return
    
    def _do_peeking(self, peek_evt_data=False):
        """ the real function doing all the work of peeking at the input file
        @return a dict of peeked-at data
        """
        peeked_data = {}
        import AthenaPython.PyAthena as PyAthena
        _info = self.msg.info
        _error= self.msg.error
        
        def _get_detdescr_tags(evt_type):
            ddt = evt_type.get_detdescr_tags().split()
            # det_descr_tags is of the form:
            # [ 'key1', 'val1', 'key2', 'val2', ... ]
            ddt = dict(zip(ddt[0::2],  # select 'key?'
                           ddt[1::2])) # select 'val?'
            return ddt
        
        ## input-store
        # self.inputStore.dump()
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
            if len(evt_types) > 0:
                evt_type = evt_types[0]
                peeked_data['evt_type'] = evt_type.bit_mask
                ddt = _get_detdescr_tags(evt_type)
                peeked_data['det_descr_tags'] = ddt
                peeked_data['mc_channel_number'] = [long(evt_type.mc_channel_number())]
                
            def _make_item_list(item):
                sgkey= item[1]
                clid = item[0]
                _typename = store._pyclidsvc.typename
                return (_typename(clid) or str(clid), # str or keep the int?
                        sgkey)
            item_list = esi.item_list()
            item_list = map(_make_item_list, item_list)
            peeked_data['eventdata_items'] = item_list
            # print "======",len(item_list)
            peeked_data['lumi_block'] = esi.lumi_blocks()
            peeked_data['run_number'] = esi.run_numbers()
            #peeked_data['evt_number'] = esi.event_number()
            peeked_data['stream_names'] = esi.processing_tags()
            # handle event-less files
            if not peeked_data['stream_names']:
                stream_names = [sg_versioned_key(s).key for s in stream_names]
                peeked_data['stream_names'] = list(set(stream_names))
                pass
            # more event-less files handling - POOL-from-BS-event-less files
            # see bug#98568
            if len(esi.run_numbers()) == 0:
                bsmd_keys = store.keys("ByteStreamMetadataContainer")
                if len(bsmd_keys) == 1:
                    bsmd = store[bsmd_keys[0]][0]
                    peeked_data['lumi_block'] = [bsmd.getLumiBlock()]
                    peeked_data['run_number'] = [bsmd.getRunNumber()]
                    bs_metadata = {}
                    for md in bsmd.getFreeMetaDataStrings():
                        if md.startswith('Event type:'):
                            k = 'evt_type'
                            v = []
                            if 'is sim' in md:   v.append('IS_SIMULATION')
                            else:                v.append('IS_DATA')
                            if 'is atlas' in md: v.append('IS_ATLAS')
                            else:                v.append('IS_TESTBEAM')
                            if 'is physics' in md: v.append('IS_PHYSICS')
                            else:                  v.append('IS_CALIBRATION')
                            bs_metadata[k] = tuple(v)
                        elif md.startswith('GeoAtlas:'):
                            k = 'geometry'
                            v = md.split('GeoAtlas:')[1].strip()
                            bs_metadata[k] = v
                        elif md.startswith('IOVDbGlobalTag:'):
                            k = 'conditions_tag'
                            v = md.split('IOVDbGlobalTag:')[1].strip()
                            bs_metadata[k] = v
                        elif '=' in md:
                            k,v = md.split('=')
                            bs_metadata[k] = v
                        pass
                    peeked_data['evt_type']  = bs_metadata.get('evt_type', [])
                    peeked_data['geometry']  = bs_metadata.get('geometry', None)
                    peeked_data['conditions_tag'] = bs_metadata.get('conditions_tag', None)
                    peeked_data['bs_metadata'] = bs_metadata
                    pass
                pass
            
        # hack to retrieve the number of events if no EventStreamInfo
        # was present in the input file
        if nentries is None:
            ROOT = _import_ROOT()
            import os
            root_files = list(ROOT.gROOT.GetListOfFiles())
            root_files = [root_file for root_file in root_files
                          if root_file.GetName().count(self.infname)]
            if len(root_files)==1:
                root_file = root_files[0]
                data_hdr = root_file.Get("POOLContainer")
                if data_hdr == None:
                    data_hdr = root_file.Get("POOLContainer_DataHeader")                
                nentries = data_hdr.GetEntriesFast() if data_hdr is not None \
                           else None
            else:
                _info('could not find correct ROOT file (looking for [%s])',
                      self.infname)
                nentries = None
            del root_files
            del data_hdr
        peeked_data['nentries'] = nentries

        # retrieve the GUID
        def _get_guid():
            guid = None
            ROOT = _import_ROOT()
            import os
            root_files = list(ROOT.gROOT.GetListOfFiles())
            root_files = [root_file for root_file in root_files
                          if root_file.GetName().count(self.infname)]
            if len(root_files)==0:
                _info('could not find correct ROOT file (looking for [%s])',
                      self.infname)
                return
            
            root_file = root_files[0]
            pool = root_file.Get("##Params")
            import re
            # Pool parameters are of the form:
            # '[NAME=somevalue][VALUE=thevalue]'
            pool_token = re.compile(r'[[]NAME=(?P<name>.*?)[]]'\
                                    r'[[]VALUE=(?P<value>.*?)[]]').match
            params = []
            for i in xrange(pool.GetEntries()):
                if pool.GetEntry(i)>0:
                    match = pool_token(pool.db_string)
                    if not match:
                        continue
                    d = match.groupdict()
                    params.append((d['name'], d['value']))
                    if d['name'].lower() == 'fid':
                        guid = d['value']
            return guid
        guid = _get_guid()
        if guid:
            peeked_data['file_guid'] = guid
        
        ## -- metadata
        metadata_items = [(self.inputStore._pyclidsvc.typename(p.clID()),
                           p.name())
                          for p in self.inputStore.proxies()]
        peeked_data['metadata_items'] = metadata_items
        metadata = {}
        def maybe_get(o, idx, default=None):
            try:
                return o[idx]
            except IndexError:
                return default
        def maybe_float(o):
            try:
                return float(o)
            except ValueError:
                return o

        def mergeMultipleDict(inDicts):
            outDict={}
            for d in inDicts:
                for k,o in d.iteritems():
                    if not outDict.has_key(k):
                        outDict[k]=o
            if len(outDict)==0:
                return None
            else:
                return outDict
                          

            
        for k in self.inputStore.keys('IOVMetaDataContainer'):
            v = self.process_metadata(self.inputStore, k)
            metadata[k] = maybe_get(v, -1)
        peeked_data['metadata'] = metadata

        ## -- taginfo
        taginfo = {}
        if self.metaStore.contains('IOVMetaDataContainer','/TagInfo'):
            v = self.process_metadata(self.metaStore, '/TagInfo')
            taginfo = mergeMultipleDict(v)
        else:
            if '/TagInfo' in metadata:
                taginfo = metadata['/TagInfo'].copy()
            else:
                self._old_file_flag = True
                # no tag info whatsoever...
                # try detdescr_tags ?
        if ddt:
            peeked_data['det_descr_tags'] = ddt
            peeked_data['geometry'] = ddt.get('GeoAtlas', None)
            peeked_data['conditions_tag'] = ddt.get('IOVDbGlobalTag', None)
        peeked_data['tag_info'] = taginfo
        if taginfo:
            peeked_data['det_descr_tags'] = taginfo
            peeked_data['geometry'] = taginfo.get('GeoAtlas', None)
            peeked_data['conditions_tag'] = taginfo.get('IOVDbGlobalTag', None)
            peeked_data['beam_type'] = [taginfo.get('beam_type','N/A')]
            peeked_data['beam_energy']= [maybe_float(taginfo.get('beam_energy',
                                                                 'N/A'))]

        if not 'geometry' in peeked_data:
            peeked_data['geometry'] = None
        if not 'conditions_tag' in peeked_data:
            peeked_data['conditions_tag'] = None
        if not 'det_descr_tags' in peeked_data:
            peeked_data['det_descr_tags'] = {}
            
        # eventless simulated DAOD files
        if not self._begin_run_flag:
            if taginfo and taginfo.get('project_name',None) == 'IS_SIMULATION':
                peeked_data['evt_type'] = ('IS_SIMULATION', 'IS_ATLAS', 'IS_PHYSICS')
            if '/Simulation/Parameters' in metadata:
                peeked_data['run_number'] = [metadata.get('/Simulation/Parameters').get('RunNumber',None)]
        #######################################################################
        ## return early if no further processing needed...
        if peek_evt_data == False:
            return peeked_data
        #######################################################################

        
        # access directly the EventInfo
        store = self.evtStore
        evt_info_keys = store.keys('EventInfo')
        if len(evt_info_keys) != 1:
            _info('more than one EventInfo: %s', evt_info_keys)
            _info(' ==> we\'ll use [%s]', evt_info_keys[0])
        sg_key = evt_info_keys[0]
        ei = store.retrieve('EventInfo', sg_key)
        _info('=== [EventInfo#%s] ===', sg_key)
        eid = ei.event_ID()

        dh_keys = [k for k in store.keys('DataHeader')
                   # remove DH-keys for backnavigation
                   if not k.startswith('[DB=')]
        if len(dh_keys) != 1:
            _error('more than 1 DataHeader key after filtering: %s', dh_keys)
            _error('content of store: %s', store.keys('DataHeader'))
            raise RuntimeError('more than one DataHeader key')

        sg_key = dh_keys[0]
        _info('=== [DataHeader#%s] ===', sg_key)
        dh = store.retrieve('DataHeader', sg_key)
        dh_cls = type(dh)
        if not dh.checkStatus(dh_cls.Primary):
            _err = 'DataHeader#%s is NOT the primary one !' % (sg_key,)
            _error(_err)
            raise RuntimeError(_err)
        
        def _make_item_list(dhe):
            sgkey= dhe.getKey()
            clid = dhe.getPrimaryClassID()
            _typename = store._pyclidsvc.typename
            return (_typename(clid) or str(clid), # str or keep the int?
                    sgkey)
        dhes = []
        if hasattr(dh, 'at'):
            # HACK for bug #77976
            for ii in range(len(dh)):
                dhes.append(dh.at(ii))
            pass
        else:
            dhes = list(dh.elements())
        #item_list = map(_make_item_list, dh)
        item_list = []
        for i,dhe in enumerate(dhes):
            sgkey = dhe.getKey()
            clid =  dhe.getPrimaryClassID()
            _typename = store._pyclidsvc.typename
            try:
                clid_name = _typename(clid)
                if clid_name:
                    clid = clid_name
            except Exception,err:
                msg.info("no typename for clid [%s] (%s)", clid, err)
            item_list.append((str(clid), sgkey))

        # -- event-type
        evt_type = ei.event_type()
        det_descr_tags = _get_detdescr_tags(evt_type)

        peeked_data.update({
            'run_number':      [eid.run_number()],
            'evt_number':      [eid.event_number()],
            'run_type':        ['N/A'],
            'evt_type':        evt_type.bit_mask,
            'det_descr_tags':  det_descr_tags,
            'geometry':        det_descr_tags.get('GeoAtlas', None),
            'conditions_tag':  det_descr_tags.get('IOVDbGlobalTag', None),
            'lumi_block':      [eid.lumi_block()],
            'stream_names':    [dh.getProcessTag()],
            'eventdata_items': item_list,
            'beam_type':       [det_descr_tags.get('beam_type','N/A')],
            'beam_energy':     [maybe_float(det_descr_tags.get('beam_energy',
                                                               'N/A'))],
            })
        
        trigger_info= ei.trigger_info()
        stream_tags = trigger_info.streamTags() if trigger_info else []
        stags = []
        for st in stream_tags: # don't use list-comprehensions b/c of ROOT bugs
            st_type = st.type()
            st_name = st.name()
            obeys_lbk=bool(st.obeysLumiblock())
            stags.append(dict(stream_type=st_type,
                              stream_name=st_name,
                              obeys_lbk=obeys_lbk))
        peeked_data['stream_tags'] = stags

        # -- old files compat
        if self._old_file_flag or 1:

            ## -- metadata
            metadata_items = [(self.inputStore._pyclidsvc.typename(p.clID()),
                               p.name())
                              for p in self.inputStore.proxies()]
            peeked_data['metadata_items'] = metadata_items
            metadata = {}
            for k in self.inputStore.keys('IOVMetaDataContainer'):
                v = self.process_metadata(self.inputStore, k)
                metadata[k] = maybe_get(v, -1)
            peeked_data['metadata'] = metadata

            ## -- taginfo
            taginfo = {}
            if self.metaStore.contains('IOVMetaDataContainer','/TagInfo'):
                v = self.process_metadata(self.metaStore, '/TagInfo')
                taginfo = mergeMultipleDict(v)
            if taginfo:
                # we want to keep the AtlasRelease from when the file was produced
                atlas_release = metadata.get('/TagInfo', taginfo)
                atlas_release = atlas_release.get('AtlasRelease',
                                                  taginfo['AtlasRelease'])
                taginfo['AtlasRelease'] = atlas_release
                peeked_data['det_descr_tags'] = taginfo
            peeked_data['tag_info'] = taginfo

            ## -- geometry + conditions-tag
            # update these values which may have changed b/c of sync' w/ taginfo
            ddt = peeked_data['det_descr_tags']
            peeked_data['geometry'] = ddt.get('GeoAtlas', None)
            peeked_data['conditions_tag'] = ddt.get('IOVDbGlobalTag', None)

            ## -- beam energy/type
            peeked_data['beam_type']= [ddt.get('beam_type','N/A')]
            beam_ene = maybe_float(ddt.get('beam_energy','N/A'))
            peeked_data['beam_energy']=[beam_ene]
            
            pass # old files compat

        return peeked_data
        
    pass # class FilePeeker

class FilePeekerSvc(PyAthena.Svc):
    """a service to spy for file meta-data and store this collected data into
    the pool file, in a python-pickle friendly format
    """

    def __init__(self, name='FilePeekerSvc', **kw):
        kw['name'] = name
        super(FilePeekerSvc, self).__init__(**kw)


    def initialize(self):
        # register with the incident svc
        svc = PyAthena.py_svc('IncidentSvc', iface='IIncidentSvc')
        if not svc:
            self.msg.error('unable to get the incident svc')
            return StatusCode.Failure

        for incident in ('EndEvent',
                         'BeginInputFile',):
            svc.addListener(self, incident)
            pass
        
        return StatusCode.Success

    def finalize(self):
        return StatusCode.Success

    def handle(self, incident):
        tp = incident.type()
        if tp == 'EndEvent':
            pass
        elif tp == 'BeginInputFile':
            self.msg.info('input file name: [%s]', incident.fileName())
            pass
        else:
            pass
        return
    
    pass # class FilePeekerSvc
