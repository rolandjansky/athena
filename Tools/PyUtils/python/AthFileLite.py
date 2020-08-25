# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Lightweight and simplified version of AthFile
# As the transform knows which files are bytestream and which are
# POOL files we just have two simple classes and definately avoid
# doing anything fancy here

from __future__ import print_function
import os
import os.path
import re
import subprocess
import sys
import uuid

import PyUtils.dbsqlite as dbsqlite
from PyUtils.Logging import msg

def _create_file_info_template():
    """simple helper function to create consistent dicts for the
    fileinfos attribute of AthFile
    """
    d = {
        'file_md5sum': None,
        'file_name': None,
        'file_size': None,
        'file_type': None,
        'file_guid': None,
        'nentries' : 0, # to handle empty files
        'run_number': [],
        'run_type': [],
        'evt_type': [],
        'evt_number': [],
        'lumi_block': [],
        'beam_energy': [],
        'beam_type':   [],
        'stream_tags': [],
        'metadata_items': None,
        'eventdata_items': None,
        'stream_names': None,
        'geometry': None,
        'conditions_tag': None,
        'det_descr_tags': None,
        ##
        'metadata': None,
        'tag_info': None,
        }
    return d


def _urlType(filename):
    if filename.startswith('dcap:'):
        return 'dcap'
    if filename.startswith('root:'):
        return 'root'
    if filename.startswith('https:'):
        return 'https'
    if filename.startswith('rfio:'):
        return 'rfio'
    if filename.startswith('file:'):
        return 'posix'
    if filename.startswith('davs:'):
        return 'root'
    return 'posix'


def _get_file_size(filename):
    if _urlType(filename) == 'posix':
        try:
            fsize = os.stat(filename)[6]
        except IOError:
            fsize = None
    else:
        from PyUtils.RootUtils import import_root
        root = import_root()
        try:
            msg.debug('Calling TFile.Open for {0}'.format(filename))
            pos = filename.find("?")
            if pos>=0:
                extraparam = '&filetype=raw'
            else:
                extraparam = '?filetype=raw'

            file = root.TFile.Open(filename + extraparam, 'READ')
            fsize = file.GetSize()
            msg.debug('Got size {0} from TFile.GetSize'.format(fsize))
        except ReferenceError:
            msg.error('Failed to get size of {0}'.format(filename))
            fsize = None
    
        file.Close()
        del root
    return fsize


class AthPoolFile(object):
    def __init__(self, filename):
        self._filename = filename
        if self._filename:
            self._stub = os.path.basename(self._filename) + '-' + str(uuid.uuid4())
        else:
            self._stub = str(uuid.uuid4())
        self._jobOptionsFile = self._stub + '-miniAthFile.py'
        self._infoOutputFile = self._stub + '-miniAthFile.db'
        self._logFile = self._stub + '-miniAthFile.log'

        self._metadata = _create_file_info_template()
        self._error = False
        self.fopen()


    def fopen(self):
        self._writeMiniJobOptions()
        self._runMiniAthena()
        self._loadFileInfo()
        self._metadata['file_type'] = 'pool'
        self._metadata['file_size'] = _get_file_size(self._filename)


    @property
    def fileinfo(self):
        return self._metadata

    def _writeMiniJobOptions(self):
        try:
            jo = open(self._jobOptionsFile, "w")

            print(os.linesep.join(("FNAME=['{filename}']",
                                   "import os",
                                   "os.environ.pop('PYTHONINSPECT', None)",
                                   "include('AthenaPython/athfile_peeker.py')",
                                   "from AthenaCommon.AlgSequence import AlgSequence",
                                   "job = AlgSequence()",
                                   "job.peeker.outfname='{picklename}'",
                                   "job.peeker.infname=FNAME[0]",
                                   "import IOVDbSvc.IOVDb",
                                   "theApp.EvtMax = 1")).format(filename=self._filename, picklename=self._infoOutputFile), file=jo)

        except Exception as e:
            print("Exception raised when writing JO file: {0}".format(e), file=sys.stderr)
            self._error = True
            raise

    def _runMiniAthena(self):
        out = open(self._logFile, 'wb')
        try:
            athenv = os.environ.copy()
            subprocess.check_call(['athena.py', self._jobOptionsFile], stdout=out, stderr=out, env=athenv)
        except subprocess.CalledProcessError:
            # Don't delete log files if errors occured
            self._error = True
            raise


    def _loadFileInfo(self):
        db = dbsqlite.open(self._infoOutputFile)
        self._metadata = db['fileinfos']

    def _getSize(self):
        # FIXME Probably need to use ROOT for non-posix fs
        try:
            self._metadata['file_size'] = os.stat(self._filename)[6]
        except IOError:
            self._metadata['file_size'] = None

    def __del__(self):
        if ('AFDEBUG' not in os.environ) and (not self._error):
            for fname in (self._jobOptionsFile, self._infoOutputFile, self._logFile):
                try:
                    os.unlink(fname)
                except (OSError, IOError):
                    pass


class AthBSFile(object):
    def __init__(self, filename):
        self._filename = filename
        self._metadata = _create_file_info_template()
        self.fopen()
    
    def fopen(self):
        self._process_bs_file(self._filename)
        self._metadata['file_type'] = 'bs'
        self._metadata['file_size'] = _get_file_size(self._filename)

    @property
    def fileinfo(self):
        return self._metadata


    def _process_bs_file (self, fname, evtmax=1, full_details=True):
        import eformat as ef

        data_reader = ef.EventStorage.pickDataReader(fname)
        assert data_reader, \
               'problem picking a data reader for file [%s]'%fname

        beam_type   = '<beam-type N/A>'
        try:
            beam_type = data_reader.beamType()
        except Exception:
            msg.warning ("problem while extracting beam-type information")

        beam_energy = '<beam-energy N/A>'
        try:
            beam_energy = data_reader.beamEnergy()
        except Exception:
            msg.warning ("problem while extracting beam-type information")

        bs = ef.istream(fname)

        self._metadata['nentries'] = bs.total_events

        bs_metadata = {}
        
        for md in data_reader.freeMetaDataStrings():
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

        # for bwd/fwd compat...
        # see: https://savannah.cern.ch/bugs/?73208
        # needed for very old BS
        for key_name,fn_name in (
            ('GUID','GUID'),
            ('Stream','stream'),
            ('Project', 'projectTag'),
            ('LumiBlock', 'lumiblockNumber'),
            ('run_number', 'runNumber'),
            ):
            if key_name in bs_metadata:
                # no need: already in bs metadata dict
                continue
            if hasattr(data_reader, fn_name):
                bs_metadata[key_name] = getattr(data_reader, fn_name)()
            
        self._metadata['file_guid'] = bs_metadata.get('GUID', None)
        self._metadata['evt_type']  = bs_metadata.get('evt_type', [])
        self._metadata['geometry']  = bs_metadata.get('geometry', None)
        self._metadata['conditions_tag'] = bs_metadata.get('conditions_tag', None)
        self._metadata['bs_metadata'] = bs_metadata

        if not data_reader.good():
            # event-less file...
            self._metadata['run_number'].append(bs_metadata.get('run_number', 0))
            self._metadata['lumi_block'].append(bs_metadata.get('LumiBlock', 0))
            return
        
        if evtmax == -1:
            evtmax = bs.total_events
            
        ievt = iter(bs)
        for i in range(evtmax):
            try:
                evt = next(ievt)
                evt.check() # may raise a RuntimeError
                stream_tags = [dict(stream_type=tag.type,
                                    stream_name=tag.name,
                                    obeys_lbk=bool(tag.obeys_lumiblock))
                               for tag in evt.stream_tag()]
                self._metadata['run_number'].append(evt.run_no())
                self._metadata['evt_number'].append(evt.global_id())
                self._metadata['lumi_block'].append(evt.lumi_block())
                self._metadata['run_type'].append(ef.helper.run_type2string(evt.run_type()))
                self._metadata['beam_type'].append(beam_type)
                self._metadata['beam_energy'].append(beam_energy)
                self._metadata['stream_tags'].extend(stream_tags)

            except RuntimeError as err:
                print("** WARNING ** detected a corrupted bs-file:\n",err)


class AthTagFile(object):
    def __init__(self, filename):
        self._filename = filename
        self._metadata = _create_file_info_template()

        self.fopen()

    def fopen(self):
        self._process_tag_file()
        self._metadata['file_type'] = 'tag'
        self._metadata['file_size'] = _get_file_size(self._filename)

    @property
    def fileinfo(self):
        return self._metadata

    def _process_tag_file(self, evtmax=1):
        tag_guid=None
        nentries = 0
        runs=[]
        evts=[]
       
        try:
            from PyUtils.RootUtils import import_root
            root = import_root()
            f = root.TFile.Open(self._filename, 'READ')

            metadata= f.Get('CollectionMetadata') if f else None
            if metadata:
                metadata.GetEntry(0)
                # note: we used to use .rstrip('\0') b/c of the change in
                # semantics in PyROOT (char[] and const char* may not mean
                # the same thing)
                # see https://savannah.cern.ch/bugs/?100920 for the gory details
                # but in the end, we use ctypes...
                # see https://savannah.cern.ch/bugs/?101200 for the gory details
                # 
                # make sure it is what we think it is
                import ctypes
                key_name = str(ctypes.c_char_p(metadata.Key).value)
                assert key_name == 'POOLCollectionID' 
                tag_guid = str(ctypes.c_char_p(metadata.Value).value)
            del metadata
            coll_tree = f.Get('POOLCollectionTree') if f else None
            if coll_tree:
                nentries = coll_tree.GetEntries()
                if evtmax in (-1, None):
                    evtmax = nentries
                evtmax = int(evtmax)
                for row in range(evtmax):
                    if coll_tree.GetEntry(row) < 0:
                        break
                    runnbr = coll_tree.RunNumber
                    runs.append(runnbr)
                    evtnbr = coll_tree.EventNumber
                    evts.append(evtnbr)
            del coll_tree
            f.Close()
            del f

            self._metadata['stream_names'] = ['TAG']
            self._metadata['file_guid'] = tag_guid
            self._metadata['nentries'] = nentries
            self._metadata['run_number'] = runs
            self._metadata['evt_number'] = evts
        except Exception as e:
            print("Exception raised when processing TAG file {0}: {1}".format(self._filename, e), file=sys.stderr)
            raise

    def _getSize(self):
        # FIXME Probably need to use ROOT for non-posix fs
        try:
            self._metadata['file_size'] = os.stat(self._filename)[6]
        except IOError:
            self._metadata['file_size'] = None


class AthInpFile(object):
    def __init__(self, filename):
        self._filename = filename
        self._metadata = _create_file_info_template()

        self.fopen()

    def fopen(self):
        self._process_inp_file()
        self._metadata['file_type'] = 'pool'
        self._metadata['file_size'] = _get_file_size(self._filename)

    @property
    def fileinfo(self):
        return self._metadata

    def _process_inp_file(self):
        pool_guid = None
        nentries = 0
        try:
            from PyUtils.RootUtils import import_root
            root = import_root()
            f = root.TFile.Open(self._filename, 'READ')

            if f:
                # FIXME EventStreamInfo is more authoritative source for nentries
                tree = f.Get('POOLContainer')
                if not tree: # support for old files
                    tree = f.Get("POOLContainer_DataHeader")
                if tree:
                    nentries = tree.GetEntriesFast()
                del tree

                # _get_guid() code from FilePeeker class by Sebastian Binet
                pool = f.Get('##Params')
                if pool:
                    pool_token = re.compile(r'\[NAME=(?P<name>.*?)\]'\
                                            r'\[VALUE=(?P<value>.*?)\]').match
                    params = []
                    for i in range(pool.GetEntries()):
                        if pool.GetEntry(i)>0:
                            # Work around apparent pyroot issue:
                            # If we try to access pool.db_string directly,
                            # we see trailing garbage, which can confuse
                            # python's bytes->utf8 conversion
                            # and result in an error.
                            param = pool.GetLeaf('db_string').GetValueString()
                            match = pool_token(param)
                            if not match:
                                continue
                            d = match.groupdict()
                            params.append((d['name'], d['value']))
                            if d['name'].lower() == 'fid':
                                pool_guid = d['value']
                del pool
            f.Close()
            del f

            self._metadata['file_guid'] = pool_guid
            self._metadata['nentries'] = nentries
        except Exception as e:
            print("Exception raised when processing POOL file {0}: {1}".format(self._filename, e), file=sys.stderr)
            raise

    def _getSize(self):
        # FIXME Probably need to use ROOT for non-posix fs
        try:
            self._metadata['file_size'] = os.stat(self._filename)[6]
        except IOError:
            self._metadata['file_size'] = None
