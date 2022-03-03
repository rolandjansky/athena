# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from logging import getLogger; log = getLogger("DQDefects.defect_ids")

from ctypes import Structure, c_uint, Union as cUnion

from DQUtils.channel_mapping import get_channel_ids_names

from .exceptions import (DefectUnknownError,
                         InvalidLogicTagError)

import six
from typing import Set, Iterable, Tuple, Union, List
from collections.abc import Mapping, MutableMapping


class DefectIDBitfield(Structure):
    _fields_ = [("defect", c_uint, 31),
                ("is_virtual", c_uint, 1),
                ]
        
    def __repr__(self):
        args = (bool(self.is_virtual), self.defect)
        return "(is_virtual=%s, defect=%i)" % args


class DefectID(cUnion):
    _fields_ = [("as_int", c_uint),
                ("as_bitfield", DefectIDBitfield)]
    
    def __int__(self):
        return self.as_int
    
    def __getattr__(self, what):
        if any(what == field[0] for field in DefectIDBitfield._fields_):
            return getattr(self.as_bitfield, what)
        raise AttributeError(what)
        
    def __setattr__(self, what, value):
        if any(what == field[0] for field in DefectIDBitfield._fields_):
            return setattr(self.as_bitfield, what, value)
        return super(DefectID, self).__setattr__(what, value)
                
    def __repr__(self):
        args = self.as_int, self.as_bitfield
        return '<DefectID value=0x%08x fields=%r>' % args

def choose_new_defect_id(existing_map: Mapping[Union[str, int], Union[str,int]], 
                         defect_name: str, virtual: bool = False) -> int:
    """
    Function to create a new defect ID.

    We're abandoning the mapping of defect names to IDs (Oct 2012).
    Hence here we just ask for the first open gap in ID numbers.
    It works for virtual IDs as well, though via a bit of an edge case for
    when none exist yet.
    """
    existing = sorted(_ for _ in six.itervalues(existing_map) if not isinstance(_, six.string_types))
    if len(existing) == 0:
        newid = 0
    else:
        newid = None
        for i, v in enumerate(existing[1:]):
            if v-existing[i] > 1:
                newid = existing[i]+1
                break
        if newid is None:
            newid = existing[-1] + 1
    did = DefectID(newid)
    did.is_virtual=virtual
    log.debug("Chose new id: %r", did)
    return int(did)

class DefectsDBIDsNamesMixin(object):
    """
    Contains the logic for storing knowledge of which defects exist, and their
    channel names and IDs
    """
    def __init__(self) -> None:
        self._initialized = False
        self._virtual_initialized = False
        self._defect_id_map: MutableMapping[Union[str,int], Union[str,int]] = {}
        self._defect_ids: Set[int] = set()
        self._defect_names: Set[str] = set()
        self._virtual_defect_id_map: MutableMapping[Union[str,int], Union[str,int]] = {}
        self._virtual_defect_ids: Set[int] = set()
        self._virtual_defect_names: Set[str] = set()
        super(DefectsDBIDsNamesMixin, self).__init__()
    
    def _populate_defect_ids(self) -> None:
        """
        Called the first time any of defect_{ids,names,id_map,etc} is called,
        and populates internal variables to store the channel ids/names for the
        life of the DefectsDB instance.
        """
        ids, names, mapping = get_channel_ids_names(self.defects_folder)
        self._defect_ids = set(ids)
        self._defect_names = set(names)
        self._defect_id_map = mapping
        
    def _populate_virtual_defect_ids(self) -> None:
        """
        Called the first time any of virtual_defect_{ids,names,id_map,etc} is called,
        and populates internal variables to store the channel ids/names for the
        life of the DefectsDB instance.
        """
        _, _, mapping = get_channel_ids_names(self.defect_logic_folder)
        try:
            self._virtual_defect_names = set(self.virtual_defect_logics.keys())
            self._virtual_defect_ids = set(mapping[name] for name in self._virtual_defect_names)
            all_defects = self._virtual_defect_names | self._virtual_defect_ids
            self._virtual_defect_id_map = dict(item for item in mapping.items() 
                                               if item[0] in all_defects)
        except InvalidLogicTagError:
            # Presumably this tag doesn't exist yet, which is ok if we're about
            # to create some new virtual defects. ?
            self._virtual_defect_names = set()
            self._virtual_defect_ids = set()
            self._virtual_defect_map = {}
    
    def _new_defect(self, did: int, dname: str) -> None:
        """
        Internal function used to keep defect IDs/names uptodate.
        """
        self.defect_ids.add(did)
        self.defect_names.add(dname)
        self.defect_id_map[did] = dname
        self.defect_id_map[dname] = did
        
    def _new_virtual_defect(self, did: int, dname: str) -> None:
        """
        Internal function used to keep defect IDs/names uptodate.
        """
        self.virtual_defect_ids.add(did)
        self.virtual_defect_names.add(dname)
        self.virtual_defect_id_map[did] = dname
        self.virtual_defect_id_map[dname] = did
        # Reset this so we will reload logic later
        self._virtual_defect_logics = None
    
    @property
    def defect_ids(self) -> Set[int]:
        """
        Gives the set of defect IDs that exist in COOL
        """
        if not self._initialized: 
            self._populate_defect_ids()
            self._initialized = True
        return self._defect_ids
    
    @property
    def defect_names(self) -> Set[str]:
        """
        Gives the set of defect names that exist in COOL
        """
        if not self._initialized: 
            self._populate_defect_ids()
            self._initialized = True
        assert self._defect_names is not None, self._initialized
        return self._defect_names
    
    @property
    def defect_id_map(self) -> MutableMapping[Union[str,int], Union[str,int]]:
        """
        Gives the dictionary relating COOL channel ids to defect names and vice
        versa, retrieving them from the database if necessary.
        """
        if not self._initialized: 
            self._populate_defect_ids()
            self._initialized = True
        return self._defect_id_map
        
    @property
    def virtual_defect_ids(self) -> Set[int]:
        """
        Returns the set of existing virtual defect IDs
        """
        if not self._virtual_initialized: 
            self._populate_virtual_defect_ids()
            self._virtual_initialized = True
        return self._virtual_defect_ids
    
    @property
    def virtual_defect_names(self) -> Set[str]:
        """
        Returns the set of existing virtual defect names
        """
        if not self._virtual_initialized: 
            self._populate_virtual_defect_ids()
            self._virtual_initialized = True
        return self._virtual_defect_names
    
    @property
    def virtual_defect_id_map(self) -> MutableMapping[Union[str,int], Union[str,int]]:
        """
        Returns a dict() mapping virtual defect names to IDs and vice-versa.
        """
        if not self._virtual_initialized: 
            self._populate_virtual_defect_ids()
            self._virtual_initialized = True
        return self._virtual_defect_id_map
        
    def defect_chan_as_id(self, channel: Union[str, int], primary_only: bool = False) -> int:
        """
        Returns the defect ID for a virtual defect.
        Accepts a `channel` as an integer/string and returns it as an integer.
        
        Will raise DefectUnknownError if `channel` is an unknown ID or string.
        
        This function first checks against non-virtual defects, then virutal 
        defects. Thus virtual-defects are lazily loaded.        
        """
        from builtins import int
        if isinstance(channel, int):
            if (channel not in self.defect_ids and 
                (not primary_only and channel not in self.virtual_defect_ids)):
                raise DefectUnknownError(channel)
            return channel
        elif isinstance(channel, six.string_types):
            if channel in self.defect_names:
                return self.defect_id_map[channel]
            if not primary_only and channel in self.virtual_defect_names:
                return self.virtual_defect_id_map[channel]
            raise DefectUnknownError(channel)
        raise RuntimeError("Invalid `channel` type, got %s, expected integer"
                              " or string" % type(channel))
    
    def defect_names_as_ids(self, channels: Iterable[Union[str, int]]) -> List[int]:
        """
        Returns a list of channels as IDs
        """
        return [self.defect_chan_as_id(chan) for chan in channels]
    
    def get_channels(self) -> Tuple[Set[int], Set[str], Mapping[Union[str, int], Union[str, int]]]:
        """
        Return channel IDs, names, and dict relating the two
        """
        return self.defect_ids, self.defect_names, self.defect_id_map
    
    def get_virtual_channels(self) -> Tuple[Set[int], Set[str], Mapping[Union[str, int], Union[str, int]]]:
        """
        Return channel IDs, names, and dict relating the two
        """
        return self.virtual_defect_ids, self.virtual_defect_names, self.virtual_defect_id_map
    
    def get_channel_descriptions(self, channels: Iterable[Union[str, int]]) -> MutableMapping[Union[str, int], str]:
        """
        For the list of channel IDs "channels", return dict mapping ID to
        description
        """
        get_desc = self.defects_folder.channelDescription
        return {channel: get_desc(self.defect_chan_as_id(channel))
                     for channel in channels}
                     
    def get_virtual_channel_descriptions(self, channels: Iterable[Union[str, int]]) -> MutableMapping[Union[str, int], str]:
        """
        For the list of channel IDs "channels", return dict mapping ID to
        descriptiondefect_id
        """
        get_desc = self.defect_logic_folder.channelDescription
        return {channel: get_desc(self.defect_chan_as_id(channel))
                    for channel in channels}

    @property
    def all_defect_descriptions(self) -> Mapping[Union[str, int], str]:
        """
        A dictionary of all (virtual and primary) defect descriptions
        """
        result = self.get_channel_descriptions(self.defect_names)
        gvcd = self.get_virtual_channel_descriptions
        result.update(gvcd(self.virtual_defect_names))
        return result
    
    def set_channel_description(self, channel: Union[str, int], description: str) -> None:
        """
        Set a defect description
        """
        chan_id = self.defect_chan_as_id(channel)
        if self.defect_is_virtual(chan_id):
            folder = self.defect_logic_folder
        else:
            folder = self.defects_folder
        folder.setChannelDescription(chan_id, description.encode('utf-8'))
    
    def defect_is_virtual(self, defect_id: Union[str, int]) -> bool:
        """
        Returns True if the `defect_id` represents a virtual defect, False if it 
        is not and raises if it doesn't exist
        
        Parameters:
            `defect_id` : defect channel id or name
        """
        from builtins import int
        if isinstance(defect_id, int):
            return DefectID(defect_id).is_virtual
        
        if not isinstance(defect_id, six.string_types):
            raise RuntimeError("Invalid defect_id, expected int or string")
        
        if defect_id in self.defect_names:
            return False
            
        if defect_id in self.virtual_defect_names:
            return True
        
        raise DefectUnknownError(defect_id)

    def normalize_defect_names(self, defect_id: Union[str, Iterable[str]]) -> Union[str, List[str]]:
        """
        Returns correct name(s) of defects, given name(s) that possibly differ
        from the correct ones by case.  Raises if an input name doesn't map to
        any existing defect.  You can pass either a string or an iterable
        object as `defect_id`.
        """
        wasstring = False
        if isinstance(defect_id, six.string_types):
            defect_id = [defect_id]
            wasstring = True
        if any(not isinstance(i, six.string_types) for i in defect_id):
            raise ValueError('All input values must be strings')

        uppered_defects = dict((i.upper(), i) for i in self.defect_names)
        uppered_defects.update(dict((i.upper(), i) for i in self.virtual_defect_names))

        rv = []
        for i in defect_id:
            up = i.upper()
            if up in uppered_defects:
                rv.append(uppered_defects[up])
            else:
                raise DefectUnknownError(i)

        if wasstring:
            return rv[0]
        else:
            return rv
