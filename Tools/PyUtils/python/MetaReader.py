# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import absolute_import
import os
import re
from fnmatch import fnmatchcase
from AthenaCommon.Logging import logging

msg = logging.getLogger('MetaReader')

# compile the regex needed in _convert_value() outside it to optimize the code.
regexEventStreamInfo = re.compile(r'^EventStreamInfo(_p\d+)?$')
regexIOVMetaDataContainer = re.compile(r'^IOVMetaDataContainer(_p\d+)?$')
regexByteStreamMetadataContainer = re.compile(r'^ByteStreamMetadataContainer(_p\d+)?$')
regexXAODEventFormat = re.compile(r'^xAOD::EventFormat(_v\d+)?$')
regexXAODTriggerMenu = re.compile(r'^DataVector<xAOD::TriggerMenu(_v\d+)?>$')
regexXAODTriggerMenuAux = re.compile(r'^xAOD::TriggerMenuAuxContainer(_v\d+)?$')
regex_cppname = re.compile(r'^([\w:]+)(<.*>)?$')
# regex_persistent_class = re.compile(r'^([a-zA-Z]+_p\d+::)*[a-zA-Z]+_p\d+$')
regex_persistent_class = re.compile(r'^([a-zA-Z]+(_[pv]\d+)?::)*[a-zA-Z]+_[pv]\d+$')
regex_BS_files = re.compile(r'^(\w+):.*((\.D?RAW\..*)|(\.data$))')


def read_metadata(filenames, file_type = None, mode = 'lite', promote = None, meta_key_filter = [],
                  unique_tag_info_values = True):
    """
    This tool is independent of Athena framework and returns the metadata from a given file.
    :param filenames: the input file from which metadata needs to be extracted.
    :param file_type: the type of file. POOL or BS (bytestream: RAW, DRAW) files.
    :param mode: if true, will return all metadata associated with the filename. By default, is false and this will
    return a "tiny" version which have only the following keys: 'file_guid', 'file_size', 'file_type', 'nentries'.
    :return: a dictionary of metadata for the given input file.
    """

    from RootUtils import PyROOTFixes  # noqa F401

    # Check if the input is a file or a list of files.
    if isinstance(filenames, str):
        filenames = [filenames]

    # Check if file_type is an allowed value
    if file_type is not None:
        if file_type not in ('POOL', 'BS'):
            raise NameError('Allowed values for \'file_type\' parameter are: "POOL" or "BS": you provided "' + file_type + '"')
        else:
            msg.info('Forced file_type: {0}'.format(file_type))

    # Check the value of mode parameter
    if mode not in ('tiny', 'lite', 'full', 'peeker'):
        raise NameError('Allowed values for "mode" parameter are: "tiny", "lite", "peeker" or "full"')
    msg.info('Current mode used: {0}'.format(mode))
    msg.info('Current filenames: {0}'.format(filenames))

    if mode != 'full' and len(meta_key_filter) > 0:
        raise NameError('It is possible to use the meta_key_filter option only for full mode')
    if meta_key_filter:
        msg.info('Filter used: {0}'.format(meta_key_filter))

    # create the storage object for metadata.
    meta_dict = {}

    # ----- retrieve metadata from all filename or filenames --------------------------------------------------------#
    for filename in filenames:
        meta_dict[filename] = {}
        current_file_type = None
        # Determine the file_type of the input and store this information into meta_dict
        if not file_type:
            if os.path.isfile(filename):
                with open(filename, 'rb') as binary_file:
                    magic_file = binary_file.read(4)

                    if magic_file == 'root' or magic_file == b'root':
                        current_file_type = 'POOL'
                        meta_dict[filename]['file_type'] = 'POOL'

                    else:
                        current_file_type = 'BS'
                        meta_dict[filename]['file_type'] = 'BS'

                    # add information about the file_size of the input filename
                    meta_dict[filename]['file_size'] = os.path.getsize(filename)

            # determine the file type for the remote input files
            else:
                if regex_BS_files.match(filename):
                    current_file_type = 'BS'
                    meta_dict[filename]['file_type'] = 'BS'
                else:
                    current_file_type = 'POOL'
                    meta_dict[filename]['file_type'] = 'POOL'

                # add information about the file_size of the input filename
                meta_dict[filename]['file_size'] = None  # None -> we can't read the file size for a remote file

        else:
            current_file_type = file_type

        # ----- retrieves metadata from POOL files ------------------------------------------------------------------#
        if current_file_type == 'POOL':
            import ROOT
            # open the file using ROOT.TFile
            current_file = ROOT.TFile.Open( _get_pfn(filename) )

            # open the tree 'POOLContainer' to read the number of entries
            if current_file.GetListOfKeys().Contains('POOLContainer'):
                meta_dict[filename]['nentries'] = current_file.Get('POOLContainer').GetEntriesFast()
            else:
                meta_dict[filename]['nentries'] = None

            # open the tree 'CollectionTree' to read auto flush
            if current_file.GetListOfKeys().Contains('CollectionTree'):
                meta_dict[filename]['auto_flush'] = current_file.Get('CollectionTree').GetAutoFlush()

            # read and add the 'GUID' value
            meta_dict[filename]['file_guid'] = _read_guid(filename)

            # read and add compression level and algorithm
            meta_dict[filename]['file_comp_alg'] = current_file.GetCompressionAlgorithm()
            meta_dict[filename]['file_comp_level'] = current_file.GetCompressionLevel()

            # ----- read extra metadata required for 'lite' and 'full' modes ----------------------------------------#
            if mode != 'tiny':
                # selecting from all tree the only one which contains metadata, respectively "MetaData"
                metadata_tree = current_file.Get('MetaData')
                # read all list of branches stored in "MetaData" tree
                metadata_branches = metadata_tree.GetListOfBranches()
                nr_of_branches = metadata_branches.GetEntriesFast()

                # object to store the names of metadata containers and their corresponding class name.
                meta_dict[filename]['metadata_items'] = {}

                # create a container for the list of filters used for the lite version
                meta_filter = {}

                # set the filters for name
                if mode == 'lite':
                    meta_filter = {
                        '/TagInfo': 'IOVMetaDataContainer_p1',
                        'IOVMetaDataContainer_p1__TagInfo': 'IOVMetaDataContainer_p1',
                        '*': 'EventStreamInfo_p*'
                    }

                # set the filters for name
                if mode == 'peeker':
                    meta_filter = {
                        '/TagInfo': 'IOVMetaDataContainer_p1',
                        'IOVMetaDataContainer_p1__TagInfo': 'IOVMetaDataContainer_p1',
                        '/Simulation/Parameters': 'IOVMetaDataContainer_p1',
                        '/Digitization/Parameters': 'IOVMetaDataContainer_p1',
                        '/EXT/DCS/MAGNETS/SENSORDATA': 'IOVMetaDataContainer_p1',
                        'TriggerMenu': 'DataVector<xAOD::TriggerMenu_v1>',
                        'TriggerMenuAux.': 'xAOD::TriggerMenuAuxContainer_v1',
                        '*': 'EventStreamInfo_p*'
                    }

                if mode == 'full' and meta_key_filter:
                    meta_filter = {f: '*' for f in meta_key_filter}
                # store all persistent classes for metadata container existing in a POOL/ROOT file.
                persistent_instances = {}

                for i in range(0, nr_of_branches):
                    branch = metadata_branches.At(i)
                    name = branch.GetName()


                    class_name = branch.GetClassName()

                    if regexIOVMetaDataContainer.match(class_name):
                        name = name.replace('IOVMetaDataContainer_p1_', '').replace('_', '/')

                    if regexIOVMetaDataContainer.match(class_name):
                        meta_dict[filename]['metadata_items'][name] = 'IOVMetaDataContainer'
                    elif regexByteStreamMetadataContainer.match(class_name):
                        meta_dict[filename]['metadata_items'][name] = 'ByteStreamMetadataContainer'
                    elif regexEventStreamInfo.match(class_name):
                        meta_dict[filename]['metadata_items'][name] = 'EventStreamInfo'
                    else:
                        meta_dict[filename]['metadata_items'][name] = class_name

                    if len(meta_filter) > 0:
                        keep = False
                        for filter_key, filter_class in meta_filter.items():
                            if (filter_key.replace('/', '_') == name.replace('/', '_') or filter_key == '*') and fnmatchcase(class_name, filter_class):
                                keep = True
                                break

                        if not keep:
                            continue

                    # assign the corresponding persistent class based of the name of the metadata container
                    if regexEventStreamInfo.match(class_name):
                        if class_name.endswith('_p1'):
                            persistent_instances[name] = ROOT.EventStreamInfo_p1()
                        elif class_name.endswith('_p2'):
                            persistent_instances[name] = ROOT.EventStreamInfo_p2()
                        else:
                            persistent_instances[name] = ROOT.EventStreamInfo_p3()
                    elif regexIOVMetaDataContainer.match(class_name):
                        persistent_instances[name] = ROOT.IOVMetaDataContainer_p1()
                    elif regexXAODEventFormat.match(class_name):
                        persistent_instances[name] = ROOT.xAOD.EventFormat_v1()
                    elif regexXAODTriggerMenu.match(class_name):
                        persistent_instances[name] = ROOT.xAOD.TriggerMenuContainer_v1()
                    elif regexXAODTriggerMenuAux.match(class_name):
                        persistent_instances[name] = ROOT.xAOD.TriggerMenuAuxContainer_v1()

                    if name in persistent_instances:
                        branch.SetAddress(ROOT.AddressOf(persistent_instances[name]))

                metadata_tree.GetEntry(0)

                # clean the meta-dict if the meta_key_filter flag is used, to return only the key of interest
                if meta_key_filter:
                    meta_dict[filename] = {}

                # read the metadata
                for name, content in persistent_instances.items():
                    key = name

                    if hasattr(content, 'm_folderName'):
                        key = getattr(content, 'm_folderName')

                    aux = None
                    if key == 'TriggerMenu' and 'TriggerMenuAux.' in persistent_instances:
                        aux = persistent_instances['TriggerMenuAux.']
                    elif key == 'DataVector<xAOD::TriggerMenu_v1>_TriggerMenu' and 'xAOD::TriggerMenuAuxContainer_v1_TriggerMenuAux.' in persistent_instances:
                        aux = persistent_instances['xAOD::TriggerMenuAuxContainer_v1_TriggerMenuAux.']
                    elif key == 'TriggerMenuAux.':
                        continue
                    elif key == 'xAOD::TriggerMenuAuxContainer_v1_TriggerMenuAux.':
                        continue

                    meta_dict[filename][key] = _convert_value(content, aux)


            # This is a required workaround which will temporarily be fixing ATEAM-560 originated from  ATEAM-531
            # ATEAM-560: https://its.cern.ch/jira/browse/ATEAM-560
            # ATEAM-531: https://its.cern.ch/jira/browse/ATEAM-531
            # This changes will remove all duplicates values presented in some files due
            # to the improper merging of two IOVMetaDataContainers.
            if unique_tag_info_values:
                msg.info('MetaReader is called with the parameter "unique_tag_info_values" set to True. '
                         'This is a workaround to remove all duplicate values from "/TagInfo" key')
                if '/TagInfo' in meta_dict[filename]:
                    for key, value in meta_dict[filename]['/TagInfo'].items():
                        if isinstance(value, list):
                            unique_list = list(set(value))
                            meta_dict[filename]['/TagInfo'][key] = unique_list[0] if len(unique_list) == 1 else unique_list

            if promote is None:
                promote = mode == 'lite' or mode == 'peeker'

            # Filter the data and create a prettier output for the 'lite' mode
            if mode == 'lite':
                meta_dict = make_lite(meta_dict)

            if mode == 'peeker':
                meta_dict = make_peeker(meta_dict)

            if promote:
                meta_dict = promote_keys(meta_dict)

        # ----- retrieves metadata from bytestream (BS) files (RAW, DRAW) ------------------------------------------#
        elif current_file_type == 'BS':
            import eformat

            # store the number of entries
            bs = eformat.istream(filename)
            meta_dict[filename]['nentries'] = bs.total_events

            # store the 'guid' value
            data_reader = eformat.EventStorage.pickDataReader(filename)
            assert data_reader, 'problem picking a data reader for file [%s]' % filename

            if hasattr(data_reader, 'GUID'):
                meta_dict[filename]['file_guid'] = getattr(data_reader, 'GUID')()

            # if the flag full is set to true then grab all metadata
            # ------------------------------------------------------------------------------------------------------#
            if mode != "tiny":
                bs_metadata = {}

                for md in data_reader.freeMetaDataStrings():
                    if md.startswith('Event type:'):
                        k = 'eventTypes'
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

                bs_metadata['runNumbers'] = getattr(data_reader, 'runNumber')()
                bs_metadata['lumiBlockNumbers'] = getattr(data_reader, 'lumiblockNumber')()
                bs_metadata['projectTag'] = getattr(data_reader, 'projectTag')()
                bs_metadata['stream'] = getattr(data_reader, 'stream')()
                #bs_metadata['beamType'] = getattr(data_reader, 'beamType')()
                beamTypeNbr= getattr(data_reader, 'beamType')()
                #According to info from Rainer and Guiseppe the beam type is
                #O: no beam
                #1: protons
                #2: ions
                if (beamTypeNbr==0): bs_metadata['beamType'] = 'cosmics'
                elif (beamTypeNbr==1 or beamTypeNbr==2):  bs_metadata['beamType'] = 'collisions'
                else: bs_metadata['beamType'] = 'unknown'

                bs_metadata['beamEnergy'] = getattr(data_reader, 'beamEnergy')()

                meta_dict[filename]['eventTypes'] = bs_metadata.get('eventTypes', [])
                meta_dict[filename]['GeoAtlas'] = bs_metadata.get('geometry', None)
                meta_dict[filename]['conditions_tag'] = bs_metadata.get('conditions_tag', None)
                meta_dict[filename]['project_name'] = bs_metadata.get('projectTag', None)

                # Promote up one level
                meta_dict[filename]['runNumbers'] = [bs_metadata.get('runNumbers', None)]
                meta_dict[filename]['lumiBlockNumbers'] = [bs_metadata.get('lumiBlockNumbers', None)]
                meta_dict[filename]['beam_type'] = bs_metadata.get('beamType', None)
                meta_dict[filename]['beam_energy'] = bs_metadata.get('beamEnergy', None)
                meta_dict[filename]['stream'] = bs_metadata.get('stream', None)

                if not data_reader.good():
                    # event-less file...
                    meta_dict[filename]['runNumbers'].append(bs_metadata.get('run_number', 0))
                    meta_dict[filename]['lumiBlockNumbers'].append(bs_metadata.get('LumiBlock', 0))

                ievt = iter(bs)
                evt = next(ievt)
                evt.check()  # may raise a RuntimeError
                processing_tags = [dict(stream_type = tag.type, stream_name = tag.name, obeys_lbk = bool(tag.obeys_lumiblock)) for tag in evt.stream_tag()]
                meta_dict[filename]['processingTags'] = [x['stream_name'] for x in processing_tags]
                meta_dict[filename]['evt_number'] = [evt.global_id()]
                meta_dict[filename]['run_type'] = [eformat.helper.run_type2string(evt.run_type())]

                # fix for ATEAM-122
                if len(bs_metadata.get('eventTypes', '')) == 0:  # see: ATMETADATA-6
                    evt_type = ['IS_DATA', 'IS_ATLAS']
                    if bs_metadata.get('stream', '').startswith('physics_'):
                        evt_type.append('IS_PHYSICS')
                    elif bs_metadata.get('stream', '').startswith('calibration_'):
                        evt_type.append('IS_CALIBRATION')
                    elif bs_metadata.get('projectTag', '').endswith('_calib'):
                        evt_type.append('IS_CALIBRATION')
                    else:
                        evt_type.append('Unknown')

                    meta_dict[filename]['eventTypes'] = evt_type

                if mode == 'full':
                    meta_dict[filename]['bs_metadata'] = bs_metadata

        # ------ Throw an error if the user provide other file types -------------------------------------------------#
        else:
            msg.error('Unknown filetype for {0} - there is no metadata interface for type {1}'.format(filename, current_file_type))
            return None

    return meta_dict


def _get_pfn(filename):
    """
    Extract the actuall filename if LFN or PFN notation is used
    """
    pfx = filename[0:4]
    if pfx == 'PFN:':
        return filename[4:]
    if pfx == 'LFN:':
        import subprocess, os
        os.environ['POOL_OUTMSG_LEVEL'] = 'Error'
        output = subprocess.check_output(['FClistPFN','-l',filename[4:]]).split('\n')
        if len(output) == 2:
            return output[0]
        msg.error( 'FClistPFN({0}) returned unexpected number of lines:'.format(filename) )
        msg.error( '\n'.join(output) )
    return filename


def _read_guid(filename):
    """
    Extracts the "guid" (Globally Unique Identfier in POOL files and Grid catalogs) value from a POOL file.
    :param filename: the input file
    :return: the guid value
    """
    import ROOT
    root_file = ROOT.TFile.Open( _get_pfn(filename) )
    params = root_file.Get('##Params')

    regex = re.compile(r'^\[NAME=([a-zA-Z0-9_]+)\]\[VALUE=(.*)\]')
    fid = None

    for i in range(params.GetEntries()):
        params.GetEntry(i)
        # Work around apparent pyroot issue:
        # If we try to access params.db_string directly, we see trailing
        # garbage, which can confuse python's bytes->utf8 conversion
        # and result in an error.
        param = params.GetLeaf('db_string').GetValueString()

        result = regex.match(param)
        if result:
            if result.group(1) == 'FID' :
               # don't exit yet, it's the last FID entry that counts
               fid = result.group(2)

    return fid


def _extract_fields(obj):
    result = {}

    for meth in dir(obj):
        if not meth.startswith('_'):
            if meth.startswith('m_'):

                field_name = str(meth)[2:]
                field_value = getattr(obj, meth)

                result[field_name] = _convert_value(field_value)

    return result


def _convert_value(value, aux = None):
    cl=value.__class__
    if hasattr(cl, '__cpp_name__'):

        result = regex_cppname.match(cl.__cpp_name__)

        if result:
            cpp_type = result.group(1)
            if cpp_type == 'vector':
                return [_convert_value(val) for val in value]

            elif cpp_type == 'pair':
                return _convert_value(value.first), _convert_value(value.second)

            # elif cpp_type == 'long':
            #   return int(value)

            elif cl.__cpp_name__ == "_Bit_reference":
                return bool(value)

            # special case which extracts data in a better format from IOVPayloadContainer_p1 class
            elif cl.__cpp_name__ == 'IOVMetaDataContainer_p1':
                return _extract_fields_iovmdc(value)

            elif cl.__cpp_name__ == 'IOVPayloadContainer_p1':
                return _extract_fields_iovpc(value)

            elif cl.__cpp_name__ == 'xAOD::EventFormat_v1':
                return _extract_fields_ef(value)

            elif cl.__cpp_name__ == 'DataVector<xAOD::TriggerMenu_v1>' :
                return _extract_fields_triggermenu(interface=value, aux=aux)

            elif (cl.__cpp_name__ == 'EventStreamInfo_p2' or
                  cl.__cpp_name__ == 'EventStreamInfo_p3'):
                return _extract_fields_esi(value)

            elif (cl.__cpp_name__ == 'EventType_p1' or
                  cl.__cpp_name__ == 'EventType_p3'):
                return _convert_event_type_bitmask(_extract_fields(value))

            elif regex_persistent_class.match(cl.__cpp_name__):
                return _extract_fields(value)

    return value


def _extract_fields_iovmdc(value):
    return _convert_value(value.m_payload)


def _extract_fields_iovpc(value):
    result = {}

    for attr_idx in value.m_attrIndexes:
        name_idx = attr_idx.nameIndex()
        type_idx = attr_idx.typeIndex()
        obj_idx = attr_idx.objIndex()

        attr_name = value.m_attrName[name_idx]
        attr_value = None

        if type_idx == 0:
            attr_value = bool(value.m_bool[obj_idx])
        elif type_idx == 1:
            attr_value = int(value.m_char[obj_idx])
        elif type_idx == 2:
            attr_value = int(value.m_unsignedChar[obj_idx])
        elif type_idx == 3:
            attr_value = int(value.m_short[obj_idx])
        elif type_idx == 4:
            attr_value = int(value.m_unsignedShort[obj_idx])
        elif type_idx == 5:
            attr_value = int(value.m_int[obj_idx])
        elif type_idx == 6:
            attr_value = int(value.m_unsignedInt[obj_idx])
        elif type_idx == 7:
            attr_value = int(value.m_long[obj_idx])
        elif type_idx == 8:
            attr_value = int(value.m_unsignedLong[obj_idx])
        elif type_idx == 9:
            attr_value = int(value.m_longLong[obj_idx])
        elif type_idx == 10:
            attr_value = int(value.m_unsignedLongLong[obj_idx])
        elif type_idx == 11:
            attr_value = float(value.m_float[obj_idx])
        elif type_idx == 12:
            attr_value = float(value.m_double[obj_idx])
        elif type_idx == 13:
            # skipping this type because is file IOVPayloadContainer_p1.h (line 120) is commented and not considered
            pass
        elif type_idx == 14:
            attr_value = str(value.m_string[obj_idx])
            # Cleaning class name from value
            if attr_value.startswith('IOVMetaDataContainer_p1_'):
                attr_value = attr_value.replace('IOVMetaDataContainer_p1_', '')
            if attr_value.startswith('_'):
                attr_value = attr_value.replace('_', '/')
            # Now it is clean
        elif type_idx == 15:
            attr_value = int(value.m_date[obj_idx])
        elif type_idx == 16:
            attr_value = int(value.m_timeStamp[obj_idx])
        else:
            raise ValueError('Unknown type id {0} for attribute {1}'.format(type_idx, attr_name))

        if attr_name not in result:
            result[attr_name] = []

        result[attr_name].append(attr_value)

    max_element_count = 0
    for name, content in result.items():
        if len(content) > max_element_count:
            max_element_count = len(content)

    if max_element_count <= 1:
        for name, content in result.items():
            if len(content) > 0:
                result[name] = content[0]
            else:
                result[name] = None

    return result


def _extract_fields_esi(value):
    result = {}

    result['eventTypes'] = []
    for eventType in value.m_eventTypes:
        result['eventTypes'].append(_convert_value(eventType))

    result['numberOfEvents'] = value.m_numberOfEvents
    result['runNumbers'] = list(value.m_runNumbers)
    result['lumiBlockNumbers'] = list(value.m_lumiBlockNumbers)
    result['processingTags'] = [str(v) for v in value.m_processingTags]
    result['itemList'] = []

    # Get the class name in the repository with CLID <clid>
    from CLIDComps.clidGenerator import clidGenerator
    cgen = clidGenerator("")
    for clid, sgkey in value.m_itemList:
        result['itemList'].append((cgen.getNameFromClid(clid), sgkey))

    return result


def _extract_fields_ef(value):
    result = {}

    for ef_element in value:
        result[ef_element.first] = ef_element.second.className()

    return result


def _extract_fields_triggermenu(interface, aux):
    L1Items = []
    HLTChains = []

    try:
        interface.setStore( aux )
        if interface.size() > 0:
            # We make the assumption that the first stored SMK is
            # representative of all events in the input collection.
            firstMenu = interface.at(0)
            L1Items = [ item for item in firstMenu.itemNames() ]
            HLTChains = [ chain for chain in firstMenu.chainNames() ]
    except Exception as err: # noqa: F841
        msg.warn('Problem reading xAOD::TriggerMenu:')

    result = {}
    result['L1Items'] = L1Items
    result['HLTChains'] = HLTChains

    return result


def _convert_event_type_bitmask(value):

    types = None
    for key in value:
        if key == 'bit_mask':
            val = value[key]

            bitmask_length = len(val)

            is_simulation = False
            is_testbeam = False
            is_calibration = False

            if bitmask_length > 0:  # ROOT.EventType.IS_SIMULATION
                is_simulation = val[0]

            if bitmask_length > 1:  # ROOT.EventType.IS_TESTBEAM
                is_testbeam = val[1]

            if bitmask_length > 2:  # ROOT.EventType.IS_CALIBRATION:
                is_calibration = val[2]

            types = [
                'IS_SIMULATION' if is_simulation else 'IS_DATA',
                'IS_TESTBEAM' if is_testbeam else 'IS_ATLAS',
                'IS_CALIBRATION' if is_calibration else 'IS_PHYSICS'
            ]

    value['type'] = types
    return value


def make_lite(meta_dict):
    for filename, file_content in meta_dict.items():
        for key in file_content:
            if key in meta_dict[filename]['metadata_items'] and regexEventStreamInfo.match(meta_dict[filename]['metadata_items'][key]):
                keys_to_keep = ['lumiBlockNumbers', 'runNumbers', 'mc_event_number', 'mc_channel_number', 'eventTypes', 'processingTags']

                for item in list(meta_dict[filename][key]):
                    if item not in keys_to_keep:
                        meta_dict[filename][key].pop(item)

        if '/TagInfo' in file_content:
            keys_to_keep = ['beam_energy', 'beam_type', 'GeoAtlas', 'IOVDbGlobalTag', 'AODFixVersion']

            for item in list(meta_dict[filename]['/TagInfo']):
                if item not in keys_to_keep:
                    meta_dict[filename]['/TagInfo'].pop(item)
    return meta_dict


def make_peeker(meta_dict):
    for filename, file_content in meta_dict.items():
        for key in file_content:
            if key in meta_dict[filename]['metadata_items'] and regexEventStreamInfo.match(meta_dict[filename]['metadata_items'][key]):
                keys_to_keep = [
                    'lumiBlockNumbers',
                    'runNumbers',
                    'mc_event_number',
                    'mc_channel_number',
                    'eventTypes',
                    'processingTags',
                    'itemList'
                ]
                for item in list(meta_dict[filename][key]):
                    if item not in keys_to_keep:
                        meta_dict[filename][key].pop(item)

        if '/TagInfo' in file_content:
            keys_to_keep = [
                'beam_energy',
                'beam_type',
                'GeoAtlas',
                'IOVDbGlobalTag',
                'AODFixVersion',
                'AMITag',
                'project_name',
                'triggerStreamOfFile',
                'AtlasRelease'
            ]
            for item in list(meta_dict[filename]['/TagInfo']):
                if item not in keys_to_keep:
                    meta_dict[filename]['/TagInfo'].pop(item)

        if '/Simulation/Parameters' in file_content:
            keys_to_keep = [
                'TruthStrategy',
                'SimBarcodeOffset',
                'TRTRangeCut',
            ]
            for item in list(meta_dict[filename]['/Simulation/Parameters']):
                if item not in keys_to_keep:
                    meta_dict[filename]['/Simulation/Parameters'].pop(item)

        if '/Digitization/Parameters' in file_content:
            keys_to_keep = [
                'numberOfCollisions',
                'intraTrainBunchSpacing',
                'BeamIntensityPattern'
            ]
            for item in list(meta_dict[filename]['/Digitization/Parameters']):
                if item not in keys_to_keep:
                    meta_dict[filename]['/Digitization/Parameters'].pop(item)

    return meta_dict


def promote_keys(meta_dict):
    for filename, file_content in meta_dict.items():
        md = meta_dict[filename]
        for key in file_content:
            if key in md['metadata_items'] and regexEventStreamInfo.match(md['metadata_items'][key]):
                md.update(md[key])

                if 'eventTypes' in md and len(md['eventTypes']):
                    et = md['eventTypes'][0]
                    md['mc_event_number'] = et.get('mc_event_number', md['runNumbers'][0])
                    md['mc_channel_number'] = et.get('mc_channel_number', 0)
                    md['eventTypes'] = et['type']

                if 'lumiBlockNumbers' in md[key]:
                    md['lumiBlockNumbers'] = md[key]['lumiBlockNumbers']
                
                if 'processingTags' in md[key]:
                    md['processingTags'] = md[key]['processingTags']

                meta_dict[filename].pop(key)
                break

        if '/TagInfo' in file_content:
            md.update(md['/TagInfo'])
            md.pop('/TagInfo')

        if '/Simulation/Parameters' in file_content:
            md.update(md['/Simulation/Parameters'])
            md.pop('/Simulation/Parameters')

        if '/Digitization/Parameters' in file_content:
            md.update(md['/Digitization/Parameters'])
            md.pop('/Digitization/Parameters')

    return meta_dict


def convert_itemList(metadata, layout):
    """
    This function will rearrange the itemList values to match the format of 'eventdata_items', 'eventdata_itemsList'
    or 'eventdata_itemsDic' generated with AthFile
    :param metadata: a dictionary obtained using read_metadata method.
                     The mode for read_metadata must be 'peeker of 'full'
    :param layout: the mode in which the data will be converted:
                * for 'eventdata_items' use: layout= None
                * for 'eventdata_itemsList' use: layout= '#join'
                * for 'eventdata_itemsDic' use: layout= 'dict'
    """

    # Find the itemsList:
    item_list = None

    if 'itemList' in metadata:
        item_list = metadata['itemList']
    else:

        current_key = None

        for key in metadata:
            if key in metadata['metadata_items'] and metadata['metadata_items'][key] == 'EventStreamInfo_p3':
                current_key = key
                break
        if current_key is not None:
            item_list = metadata[current_key]['itemList']

    if item_list is not None:

        if layout is None:
            return item_list

        elif layout == '#join':
            return [k + '#' + v for k, v in item_list if k]


        elif layout == 'dict':
            from collections import defaultdict
            dic = defaultdict(list)

            for k, v in item_list:
                dic[k].append(v)

            return dict(dic)
