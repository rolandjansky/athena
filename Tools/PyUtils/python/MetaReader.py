import os
import logging
msg = logging.getLogger('MetaReader')

import re
import sys

from PyCool import coral


def read_metadata(filenames, file_type=None, mode='lite'):
    """
    This tool is independent of Athena framework and returns the metadata from a given file.
    :param filenames: the input file from which metadata needs to be extracted.
    :param file_type: the type of file. POOL or BS (bytestream: RAW, DRAW) files.
    :param mode: if true, will return all metadata associated with the filename. By default, is false and this will
    return a "lite" version which have only the following keys: 'file_guid', 'file_size', 'file_type', 'nentries'.
    :return: a dictionary of metadata for the given input file.
    """

    # Check if the input is a file or a list of files.
    if isinstance(filenames, basestring):
        filenames = [filenames]

    # Check if fil_type is an allowed value
    if file_type not in ('POOL', 'BS'):
        raise NameError('Allowed values for \'file_type\' parameter are: \'POOL\' or \'BS\'')
    else:
        msg.info('Forced file_type: {0}'.format(file_type))

    # Check the value of mode parameter
    if mode not in ('tiny', 'lite', 'full'):
        raise NameError('Allowed values for \'mode\' parameter are: \'tiny\', \'lite\' or \'full\'')
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
                else:
                    current_file_type = 'BS'
        else:
            current_file_type = file_type

        metaDict[filename] = {
            'file_size': os.path.getsize(filename),
        }

        # ----------------------------------------------------------------------------------------------------------------#
        # retrieves metadata from POOL files.
        if current_file_type == 'POOL':
            import ROOT
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

                # search for metadata only in these keys
                name_filter = ['StreamAOD', '/TagInfo']

                meta_data_srv = __convert_DataHeader(evt.retrieveMetaInput('DataHeader', ';00;MetaDataSvc'))
                # fill metadata_items
                metaDict[filename]['metadata_items'].append(('DataHeader', ';00;MetaDataSvc'))

                metadata = {}
                for name, cls in meta_data_srv:

                    if mode == 'lite' and name not in name_filter:
                        continue
                    try:
                        a = evt.retrieveMetaInput(cls, name)
                    except LookupError:
                        continue

                    # fill metadata_items
                    metaDict[filename]['metadata_items'].append((cls, name))
                    # print("{0} {1}".format(cls, name))

                    if cls == 'IOVMetaDataContainer':
                        metadata[name] = __convert_IOVMetaDataContainer(a)
                    if cls == 'xAOD::EventFormat':
                        metadata[name] = __convert_EventFormat(a)
                    if cls == 'EventStreamInfo':
                        metadata[name] = __convert_EventStreamInfo(a)


                # Aggregate common information who needs to be promoted up one level
                promote_list = {
                    'EventStreamInfo': [
                        ('RunNumbers', 'run_number'),
                        ('ProcessingTags', 'stream_names'),
                        ('LumiBlockNumbers', 'lumi_block'),
                        ('mc_event_number', 'evt_number'),
                        ('mc_channel_number', 'mc_channel_number'),
                        ('mc_event_weight', 'mc_event_weight'),
                        ('evt_type', 'evt_type'),
                        ('detdescr_tags', 'detdescr_tags'),
                    ],
                    'IOVMetaDataContainer': [
                        ('beam_energy', 'beam_energy'),
                        ('beam_type', 'beam_type'),
                        ('GeoAtlas', 'geometry'),
                        ('IOVDbGlobalTag', 'conditions_tag')
                    ]
                }

                for cls, cls_name in metaDict[filename]['metadata_items']:
                    if cls in promote_list:
                        for name_orig, name_new in promote_list[cls]:

                            if name_new not in metaDict[filename]:
                                metaDict[filename][name_new] = []

                            if name_orig in metadata[cls_name]:

                                value = metadata[cls_name][name_orig]

                                if isinstance(value, (list, tuple)):
                                    metaDict[filename][name_new] += value
                                else:
                                    metaDict[filename][name_new].append(value)

                if mode == 'full':
                    metaDict[filename]['metadata'] = metadata
                else:
                    metaDict[filename]['metadata'] = list(metadata.keys())

        # ----------------------------------------------------------------------------------------------------------------#
        # retrieves metadata from bytestream (BS) files (RAW, DRAW)
        elif current_file_type == 'BS':
            import eformat

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


                bs_metadata['runNumber'] = getattr(data_reader, 'runNumber')()
                bs_metadata['lumiblockNumber'] = getattr(data_reader, 'lumiblockNumber')()
                bs_metadata['projectTag'] = getattr(data_reader, 'projectTag')()
                bs_metadata['stream'] = getattr(data_reader, 'stream')()
                bs_metadata['beamType'] = getattr(data_reader, 'beamType')()
                bs_metadata['beamEnergy'] = getattr(data_reader, 'beamEnergy')()

                metaDict[filename]['evt_type'] = bs_metadata.get('evt_type', [])
                metaDict[filename]['geometry'] = bs_metadata.get('geometry', None)
                metaDict[filename]['conditions_tag'] = bs_metadata.get('conditions_tag', None)

                # Promote up one level
                metaDict[filename]['run_number'] = [bs_metadata.get('runNumber', None)]
                metaDict[filename]['lumi_block'] = [bs_metadata.get('lumiblockNumber', None)]
                metaDict[filename]['beam_type'] = [bs_metadata.get('beamType', None)]
                metaDict[filename]['beam_energy'] = [bs_metadata.get('beamEnergy', None)]

                if not data_reader.good():
                    # event-less file...
                    metaDict[filename]['run_number'].append(bs_metadata.get('run_number', 0))
                    metaDict[filename]['lumi_block'].append(bs_metadata.get('LumiBlock', 0))

                ievt = iter(bs)
                evt = ievt.next()
                evt.check()  # may raise a RuntimeError
                stream_tags = [dict(stream_type = tag.type, stream_name = tag.name, obeys_lbk = bool(tag.obeys_lumiblock)) for tag in evt.stream_tag()]
                metaDict[filename]['stream_tags'] = stream_tags
                metaDict[filename]['evt_number'] = [evt.global_id()]
                metaDict[filename]['run_type'] = [eformat.helper.run_type2string(evt.run_type())]


                # fix for ATEAM-122
                if len(bs_metadata.get('evt_type', '')) == 0:  # see: ATMETADATA-6
                    evt_type = ['IS_DATA', 'IS_ATLAS']
                    if bs_metadata.get('stream', '').startswith('physics_'):
                        evt_type.append('IS_PHYSICS')
                    elif bs_metadata.get('stream', '').startswith('calibration_'):
                        evt_type.append('IS_CALIBRATION')
                    elif bs_metadata.get('projectTag', '').endswith('_calib'):
                        evt_type.append('IS_CALIBRATION')
                    else:
                        evt_type.append('Unknown')

                    metaDict[filename]['evt_type'] = evt_type


                if mode == 'full':
                    metaDict[filename]['bs_metadata'] = bs_metadata


        # ----------------------------------------------------------------------------------------------------------------#
        # Thow an error if the user provide other file types
        else:
           msg.error('Unknown filetype for {0} - there is no metadata interface for type {1}'.format(filename, current_file_type))
           return None

    return metaDict

# Methods for POOL
def __convert_EventStreamInfo(esi):
    d = {}
    d['RunNumbers'] = list(esi.getRunNumbers())
    d['ProcessingTags'] = list(esi.getProcessingTags())
    d['LumiBlockNumbers'] = list(esi.getLumiBlockNumbers())
    d['mc_event_number'] = list()
    d['mc_channel_number'] = list()
    d['mc_event_weight'] = list()
    d['evt_type'] = list()
    d['detdescr_tags'] = list()

    for evt_type in esi.getEventTypes():
        d['mc_event_number'].append(evt_type.mc_event_number())
        d['mc_channel_number'].append(evt_type.mc_channel_number())
        d['mc_event_weight'].append(evt_type.mc_event_weight())
        d['detdescr_tags'].append(evt_type.get_detdescr_tags())

        d['evt_type'].append('IS_SIMULATION' if evt_type.test(evt_type.IS_SIMULATION) else 'IS_DATA')
        d['evt_type'].append('IS_TESTBEAM' if evt_type.test(evt_type.IS_TESTBEAM) else 'IS_ATLAS')
        d['evt_type'].append('IS_CALIBRATION' if evt_type.test(evt_type.IS_CALIBRATION) else 'IS_PHYSICS')

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
    data = {}

    for cond_attr_list_collection in iovmetadata.payloadContainer():

        for pair_attr_list in cond_attr_list_collection:
            # i = pair_attr_list.first
            attr_list = pair_attr_list.second
            spec = attr_list.specification()

            for i in range(len(spec)):
                # print("{0}: {1} ({2})".format(spec[i].name(), attr_list[i], spec[i].typeName()))
                data[spec[i].name()] = attr_list[i]

    return data


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
    import ROOT

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