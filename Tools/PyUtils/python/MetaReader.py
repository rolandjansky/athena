import os
import logging
msg = logging.getLogger('MetaReader')

import re
import ast
import sys

def read_metadata(filenames, file_type=None, mode='lite'):
    """
    This tool is independent of Athena framework and returns the metadata from a given file.
    :param filename: the input file from which metadata needs to be extracted.
    :param file_type: the type of file. POOL or BS (bytestream: RAW, DRAW) files.
    :param full: if true, will return all metadata associated with the filename. By default, is false and this will
    return a "lite" version which have only the following keys: 'file_guid', 'file_size', 'file_type', 'nentries'.
    :return: a dictionary of metadata for the given input file.
    """

    # Check if the input is a file or a list of files.
    if isinstance(filenames, basestring):
        filenames = [filenames]
    # Check the value of mode parameter
    if mode not in ['tiny', 'lite', 'full']:
        raise NameError('Allowed values for mode are: tiny, lite or full')
    msg.info('Current mode used: {0}'.format(mode))
    # create the storage object for metadata.
    metaDict = {}

    for filename in filenames:
        current_file_type = None
        # Determine the file type of the input.
        if not file_type:
            with open(filename, 'rb') as binary_file:
                magic_file = binary_file.read(4)

                if magic_file == 'root':
                    current_file_type = 'POOL'
                    # check if ROOT module is not imported previously.
                    if 'ROOT' not in sys.modules:
                        global ROOT
                        import ROOT

                else:
                    current_file_type = 'BS'
                    # check if 'eformat' module is not imported previously.
                    if 'eformat' not in sys.modules:
                        global eformat
                        import eformat


        metaDict[filename] = {
            'file_size': os.path.getsize(filename),
        }

        # ----------------------------------------------------------------------------------------------------------------#
        # retrieves metadata from POOL files.
        if current_file_type == 'POOL':
            from CLIDComps.clidGenerator import clidGenerator
            global clidgen
            clidgen = clidGenerator(db = None)

            evt = ROOT.POOL.TEvent()
            evt.readFrom(filename)
            evt.getEntry(0)

            # add the missing keys from the basis "tiny" metadata dictionary
            metaDict[filename]['file_guid'] = __read_guid(filename),
            metaDict[filename]['file_type'] = 'pool'
            metaDict[filename]['nentries'] = evt.getEntries()


            # if the flag is not set to tiny them it will retrieve more metadata
            # ----------------------------------------------------------------------------------------------------------------#
            if mode != 'tiny':
                # this information is duplicated but is used with the AthFile
                metaDict[filename]['file_name'] = filename
                # create a container for class name and instance name for metadata_items
                metaDict[filename]['metadata_items'] = list()

                meta_data_srv = __convert_DataHeader(evt.retrieveMetaInput('DataHeader', ';00;MetaDataSvc'))
                # fill metadata_items
                metaDict[filename]['metadata_items'].append(('DataHeader', ';00;MetaDataSvc'))

                for name, cls in meta_data_srv:
                    try:
                        a = evt.retrieveMetaInput(cls, name)
                    except LookupError:
                        continue

                    # fill metadata_items
                    metaDict[filename]['metadata_items'].append((cls, name))

                    if cls == 'IOVMetaDataContainer':
                        metaDict[filename][name] = __convert_IOVMetaDataContainer(a)
                    if cls == 'xAOD::EventFormat':
                        metaDict[filename][name] = __convert_EventFormat(a)
                    if cls == 'EventStreamInfo':
                        metaDict[filename][name] = __convert_EventStreamInfo(a)

                # if the flag full is set to true then grab all metadata
                if mode == 'lite':
                    for key in list(metaDict[filename]):
                        if key not in ['file_type', 'file_size', 'file_guid', 'nentries', 'run_number']:
                            metaDict[filename].pop(key, None)


        # ----------------------------------------------------------------------------------------------------------------#
        # retrieves metadata from bytestream (BS) files (RAW, DRAW)
        elif current_file_type == 'BS':
            # store the file_type of the input filename
            metaDict[filename]['file_type'] = 'bs'

            # store the number of entries
            bs = eformat.istream(filename)
            metaDict[filename]['nentries'] = bs.total_events

            # store the 'guid' value
            data_reader = eformat.EventStorage.pickDataReader(filename)
            assert data_reader, 'problem picking a data reader for file [%s]' % filename

            if hasattr(data_reader, 'GUID'):
                metaDict[filename]['file_guid'] = getattr(data_reader, 'GUID')()

            # if the flag full is set to true then grab all metadata
            # ----------------------------------------------------------------------------------------------------------------#
            if mode != "tiny":
                bs_metadata = {}

                for md in data_reader.freeMetaDataStrings():
                    if md.startswith('Event type:'):
                        k = 'evt_type'
                        v = []
                        if 'is sim' in md:
                            v.append('IS_SIMULATION')
                        else:
                            v.append('IS_DATA')

                        if 'is atlas' in md:
                            v.append('IS_ATLAS')
                        else:
                            v.append('IS_TESTBEAM')

                        if 'is physics' in md:
                            v.append('IS_PHYSICS')
                        else:
                            v.append('IS_CALIBRATION')

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
                        k, v = md.split('=')
                        bs_metadata[k] = v

                for key_name, fn_name in (
                        ('Stream', 'stream'),
                        ('Project', 'projectTag'),
                        ('LumiBlock', 'lumiblockNumber'),
                        ('run_number', 'runNumber'),
                    ):
                        if key_name in bs_metadata:
                            # no need: already in bs metadata dict
                            continue
                        if hasattr(data_reader, fn_name):
                            bs_metadata[key_name] = getattr(data_reader, fn_name)()

                metaDict[filename]['evt_type'] = bs_metadata.get('evt_type', [])
                metaDict[filename]['geometry'] = bs_metadata.get('geometry', None)
                metaDict[filename]['conditions_tag'] = bs_metadata.get('conditions_tag', None)
                metaDict[filename].update(bs_metadata)

                if not data_reader.good():
                    # event-less file...
                    metaDict[filename]['run_number'].append(bs_metadata.get('run_number', 0))
                    metaDict[filename]['lumi_block'].append(bs_metadata.get('LumiBlock', 0))

                if mode == 'lite':
                    print('This is the lite version for BS files')
                    pass


        # ----------------------------------------------------------------------------------------------------------------#
        # Thow an error if the user provide other file types
        else:
           msg.error('Unknown filetype for {0} - there is no metadata interface for type {1}'.format(filename, current_file_type))
           return None

    return metaDict

# Methods for POOL
def __convert_EventStreamInfo(esi):
    d = {}
    d['run_number'] = list(esi.getRunNumbers())
    d['processing_tags'] = list(esi.getProcessingTags())
    d['lumi_block'] = list(esi.getLumiBlockNumbers())
    d['evt_number'] = list()
    d['mc_channel_number'] = list()
    d['mc_event_weight'] = list()
    d['evt_type'] = list()
    d['detdescr_tags'] = list()

    for evt_type in esi.getEventTypes():
        d['evt_number'].append(evt_type.mc_event_number())
        d['mc_channel_number'].append(evt_type.mc_channel_number())
        d['mc_event_weight'].append(evt_type.mc_event_weight())
        d['detdescr_tags'].append(evt_type.get_detdescr_tags())

        d['evt_type'].append( 'IS_SIMULATION' if evt_type.test(evt_type.IS_SIMULATION) else 'IS_DATA')
        d['evt_type'].append( 'IS_TESTBEAM' if evt_type.test(evt_type.IS_TESTBEAM) else 'IS_ATLAS')
        d['evt_type'].append( 'IS_CALIBRATION' if evt_type.test(evt_type.IS_CALIBRATION) else 'IS_PHYSICS')

    d['eventdata_items'] = []
    for e in esi.getItemList():
        clid_name = clidgen.getNameFromClid(e.first)
        if clid_name:
            d['eventdata_items'].append((clid_name, e.second))
        else:
            msg.info('Unable to find a name for clid {0} with value {1}.'.format(e.first, e.second))
            d['eventdata_items'].append(('clid_{0}'.format(e.first), e.second))

    return d


def __convert_IOVMetaDataContainer(iovmetadata):
    return [__convert_CondAttributeList(e) for e in iovmetadata.payloadContainer()]


def __convert_CondAttributeList(condattrlist):
    r = []
    for i in range(0, condattrlist.size()):
        chanNum = condattrlist.chanNum(i)

        d = dict(condattrlist.attributeList(chanNum))

        chanName = condattrlist.chanName(chanNum)
        if chanName is not '':
            d['chanName'] = chanName

        for k, v in d.iteritems():
            try: # to cast the strings into relevant Python types/data structures
                d[k] = ast.literal_eval(v)
            except:
                pass
        r.append(d)
    return r


def __convert_EventFormat(evfmt):
    return [(e.first, e.second.className()) for e in evfmt]


def __convert_DataHeader(dataHeader):
    return [ (x.getKey(), clidgen.getNameFromClid(x.getPrimaryClassID())) for x in dataHeader]

# Currently not used
# def __md5(fname, block_size=2**20, do_fast_md5=True):
#     import hashlib
#     # do_fast_md5 is for compatibility with AthFile.
#     # -- Sebastian Liem
#     hash_md5 = hashlib.md5()
#     with open(fname, 'rb') as f:
#         for chunk in iter(lambda: f.read(block_size), b''):
#             hash_md5.update(chunk)
#             if do_fast_md5:
#                 break
#     return hash_md5.hexdigest()

def __read_guid(filename):
    """
    Extracts the "guid" (Globally Unique Identfier in POOL files and Grid catalogs) value from a POOL file.
    :param filename: the input file
    :return: the guid value
    """
    root_file = ROOT.TFile(filename)
    params = root_file.Get('##Params')

    regex = re.compile(r'^\[NAME\=([a-zA-Z0-9\_]+)\]\[VALUE\=(.*)\]')

    for i in range(params.GetEntries()):
        params.GetEntry(i)
        param = params.db_string

        result = regex.match(param)
        if result:
            name = result.group(1)
            value = result.group(2)

            if name == 'FID':
                return value

    return None