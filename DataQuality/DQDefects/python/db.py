# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
Authors: Peter Waller <peter.waller@cern.ch> and "Peter Onyisi" <peter.onyisi@cern.ch>

Please see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/DQDefects

This file defines DefectsDB and some of its public interface.

To separate out the code into digestable units, this class is split into mixins
which can be found defined in other files in this directory.

The bulk of the core functionality is found in this file.
"""


from logging import getLogger; log = getLogger("DQDefects.db")

from contextlib import contextmanager

from DQUtils import fetch_iovs, IOVSet
from DQUtils.sugar.iovtype import IOVType
from DQUtils.channel_mapping import list_to_channelselection
from DQUtils.sugar import RunLumi

from DQDefects import DEFAULT_CONNECTION_STRING

from .exceptions import DefectExistsError, DefectUnknownError
from .folders import DefectsDBFoldersMixin
from .ids import DefectsDBIDsNamesMixin, choose_new_defect_id
from .tags import DefectsDBTagsMixin, tagtype
from .virtual_mixin import DefectsDBVirtualDefectsMixin
from .virtual_calculator import calculate_virtual_defects
from typing import Union, Tuple, Optional, Iterable, Collection

import six

class DefectsDB(DefectsDBVirtualDefectsMixin, 
                DefectsDBTagsMixin,
                DefectsDBFoldersMixin, 
                DefectsDBIDsNamesMixin,
                object):
    """
    Implementation is split into mixins.
    
        DefectsDBIDsNamesMixin:
            Defines functions for dealing with defect IDs
        
        DefectsDBFolderMixin:
            Logic for loading or creating the COOL folder/database
        
        DefectsDBFoldersMixin:
            Functions for managing tags
        
        DefectsDBVirtualDefectsMixin:
            Internal code for managing and computing virutal defects
            
    Public interface is nominally defined in this class (DefectsDB).
    """

    def __init__(self, connection_string: str = DEFAULT_CONNECTION_STRING,
                 read_only: bool = True, create: bool = False, tag: Union[str, Tuple] = "HEAD") -> None:
        """
        Create a new DefectsDB instance.
        
        The COOL folder resides at `DEFECTS_FOLDER`, which is a module-level
        varaiable in this module.
        
        Parameters:
            `connection_string` : A COOL connection string, or a string ending 
                                  in .db. In the latter case, the database is
                                  assumed to be a sqlite database at that path.
            `read_only` : Indicates whether insertions will be disallowed on
                          this connection. (Default: True)
            `create`    : If True, attempt to create the database if it doesn't
                          exist. (Default: False)
            `tag`       : The COOL tag(s) to use.  Either a single string used
                          for both defects and virtual defect logic, or a
                          2-tuple (defecttag, logictag).  The default is to
                          use HEAD for both defects and logic.  Either
                          folder-level or hierarchical tags can be given.
        """
        self.connection_string = connection_string
        self._read_only = read_only
        self._create = create
        if isinstance(tag, six.string_types):
            self._tag = tagtype(tag, tag) if tag else tagtype("HEAD", "HEAD")
        else:
            try:
                tag = tagtype._make(tag)
            except Exception:
                raise TypeError('tag argument must be a 2-element sequence')
            if len(tag) != 2:
                raise TypeError('tag argument must be a 2-element sequence')
            self._tag = tag
        self._tag = tagtype(self._tag[0],
                            self._tag[1])

        # COOL has no way of emptying a storage buffer. Creating a new storage
        # buffer flushes the old one. Therefore, if an exception happens 
        # halfway through setting up a storage buffer, subsequent operations
        # risk flushing a half-completed operation. In that case, we refuse to 
        # do any more write operations with that database connection.
        self._bad_state = False
        
        super(DefectsDB, self).__init__()
        
        if create:
            # Trigger creation of folders through properties
            self.defects_folder
            self.defect_logic_folder
    
    def __del__(self) -> None:
        """
        Ideally we would use inheritance to call destructors, but this isn't 
        possible in the general case with the way we (ab)use mixins, so we just 
        do it here.
        """
        self._clear_connections()
    
    def create_defect(self, name: str, description: str) -> None:
        """
        Create a new type of defect; tries to figure out system ID from the
        defect name. See also: `create_defect_with_id`, `new_system_defect`.
        
        Parameters:
            `name` : name of the defect
            `description` : a human-readable description of the defect
        Returns:
            the ID number of the new defect
        """        
        sysid = choose_new_defect_id(self.defect_id_map, name)
        log.info("Creating new defect %s: system ID %08x", name, sysid)
        self._create_defect_with_id(sysid, name, description)

    def _create_defect_with_id(self, did: int, name: str, description: str) -> None:
        """
        Create a new type of defect, specifying the defect ID.
        See also: `create_defect`, `new_system_defect`.
        
        Parameters:
            `did` : defect_id
            `name` : name of the defect
            `description` : a human-readable description of the defect
        """
        log.info("Creating new defect %s (0x%08x)", name, did)
        if did in self.defect_ids: raise DefectExistsError(did)
        try:
            oldname = self.normalize_defect_names(name)
            raise DefectExistsError(f'Defect {oldname} already exists')
        except DefectUnknownError:
            pass
        
        self.defects_folder.createChannel(did,
                                          name.encode('ascii'),
                                          description.encode('utf-8'))
        self._new_defect(did, name)
    
    def retrieve(self, since: Optional[Union[int, Tuple[int,int], RunLumi]] = None,
                 until: Optional[Union[int, Tuple[int,int], RunLumi]] = None, 
                 channels: Optional[Iterable[Union[str,int]]] = None, nonpresent: bool = False,
                 primary_only: bool = False, ignore: Collection[str] = None,
                 with_primary_dependencies: bool = False, intersect: bool = False,
                 with_time: bool = False, evaluate_full: bool = True) -> IOVSet:
        """
        Retrieve defects from the database.
        
        Parameters:
            `since`, `until` : IoV range to query (Default: All)
            `channels`  : A list of channels to query. Can contain a mixture of
                         defect names and ids. (Default: None means all 
                        channels, including all virtual)
            `nonpresent` : Only return IoVs which are currently "present"
            `primary_only` : Only return primary IoVs, no virtual ones.
            `ignore` : Set of defects which won't be treated as bad.
            `with_primary_dependencies` : When querying virtual flags, also get
                primary flags which went into the calculation.
            `intersect` : Intersect the result with the query range so that no
                          iov spans outside the query range
            `with_time` : Also retrieves the time the defect was inserted
                          ~2x slower queries, doesn't work for virtual defects
            `evaluate_full` : If specified, also compute the `comment` and
                              `recoverable` fields of virtual defects.
                              Causes a ~0.6x slowdown
        """
        if ignore is not None and not isinstance(ignore, set):
            assert False, "ignore parameter should be set type"
        
        desired_channels = None
        # Figure out the IDs of channels to query and their virtuality
        if channels is not None:
            query_channels = set(self.defect_names_as_ids(channels))
            virtual_channels = query_channels - self.defect_ids
            primary_channels = query_channels & self.defect_ids
        else:
            # Empty channels list means query all channels
            # (including all virtual)
            if primary_only:
                virtual_channels = None
            else:
                virtual_channels = self.virtual_defect_ids
            primary_channels = set(self.defect_ids)
            query_channels = None # (all)
        
        primary_output_names = [self.defect_id_map[pid] for pid in primary_channels]
        virtual_output_names = [] # (optionally populated below)
        
        # Resolve virtual channels to query here, and the primary dependents.
        if virtual_channels:
            assert not primary_only, "Requested virtual channels with primary_only=True"
            assert not with_time, "with_time flag only works for primary defects"
            virtual_output_names = [self.virtual_defect_id_map[vid] 
                                    for vid in virtual_channels]
            
            ordered_logics = self._resolve_evaluation_order(virtual_output_names)
            
            if channels is not None:
                # Since not all channels are being queried, it is necessary to
                # add the desired primary channels to the query
                primary_needed = self.resolve_primary_defects(ordered_logics)
                primary_channels |= set(self.defect_names_as_ids(primary_needed))
                query_channels = primary_channels
                
                if with_primary_dependencies:
                    primary_output_names.extend(sorted(primary_needed))
        
            for logic in ordered_logics:
                logic.set_evaluation(evaluate_full)
        else:
            ordered_logics = []
        
        # Figure out if the set of channels will produce too many ranges for COOL
        if query_channels is not None:
            query_channels = sorted(query_channels)
            query_ranges = list_to_channelselection(query_channels, None, True)
            
            if len(query_ranges) >= 50:
                # We're querying too many ranges. Query everything, filter later.
                desired_channels = set(primary_output_names + virtual_output_names)
                query_channels = None # (everything)
            
        # Retrieve primary IoVs
        primary_iovs = fetch_iovs(self.defects_folder, since, until, 
                                  query_channels, self.defects_tag, 
                                  named_channels=True, unicode_strings=True,
                                  with_time=with_time)
        
        # Calculate virtual defects (if necessary)
        if primary_only or not virtual_channels:
            result = primary_iovs
        else:
            if not primary_iovs:
                return IOVSet()
            args = (primary_iovs, ordered_logics, 
                    virtual_output_names, primary_output_names, 
                    since, until, ignore)
            result = calculate_virtual_defects(*args)
            
        # Filter out results which have their present bit removed 
        # (unless otherwise specified)
        if not nonpresent:
            result = IOVSet(iov for iov in result if iov.present)
        
        # Filter out channels which weren't queried by the user
        # (to get around 50 channelselection COOL limit)
        if desired_channels:
            result = IOVSet(iov for iov in result 
                            if iov.channel in desired_channels)
        
        if intersect:
            result = result.intersect_range((since, until))
        
        return result
    
    @property
    def storage_buffer(self):
        """
        Gives a new context manager for use with the with statement, e.g.:
            with ddb.storage_buffer:
                for d in many_defects:
                    ddb.insert(...defect...)
        """
        assert not self._bad_state, "Please see comment in DefectsDB constructor"
        
        @contextmanager
        def thunk():
            log.debug("setupStorageBuffer()")
            self.defects_folder.setupStorageBuffer()
            try:
                yield
            except Exception:
                log.warning("Exception raised during DefectsDB.storage_buffer. "
                            "Not flushing storage buffer - but COOL has no way "
                            "to empty it. ")
                self._bad_state = True
                raise
            else:
                log.debug("flushStorageBuffer()")
                self.defects_folder.flushStorageBuffer()
        return thunk()
    
    def insert(self, defect_id: Union[str, int], since: int, until: int, comment: str, added_by: str, 
               present: bool = True, recoverable: bool = False) -> None:
        """
        Insert a new defect into the database.
        
        Parameters:
            `defect_id` : The name or channel identifier for the deect
            `since`, `until` : The COOL IoV for the range
            `comment`   : String255 arbitrary text comment
            `added_by`  : The user name or "sys:"-prefixed string of the
                          application that inserted the defect
            `present`   : The state of the flag (Default: True)
            `recoverable` : Indicates whether there is any possibility to set
                            present=False in the future (Default: False)
        """
        return self._insert(defect_id, since, until, comment, added_by,
                            present, recoverable, self.defects_tag)

    def _insert_iov(self, iov: IOVType, tag: str) -> None:
        """
        Helper function for inserting IOV objects, since record order doesn't
        match function argument order
        """
        return self._insert(iov.channel, iov.since, iov.until, iov.comment,
                            iov.user, iov.present, iov.recoverable, tag)

    def _insert(self, defect_id: Union[str, int], since: Union[int, Tuple[int, int], RunLumi], 
                until: Union[int, Tuple[int, int], RunLumi], comment: str, added_by: str,
                present: bool = True, recoverable: bool = False, tag: str = 'HEAD') -> None:
        """
        Implementation of insert, allows tag specification for internal
        functions
        """
        assert not self._read_only, "Insertion on read-only database"
        assert not self._bad_state, "Please see comment in DefectsDB constructor"
        
        # Force load of defects_folder to populate _defect_payload
        store = self.defects_folder.storeObject
        p = self._defect_payload
        
        p["present"] = present
        p["recoverable"] = recoverable
        p["user"] = added_by.encode('utf-8')
        p["comment"] = comment.encode('utf-8')

        defect_id = self.defect_chan_as_id(defect_id, True)
        
        store(since, until, p, defect_id, tag.encode('ascii'),
              (True if tag != 'HEAD' else False))
