#! /usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import with_statement

from logging import getLogger; log = getLogger("DQUtils.db")

import re
import sys
from collections import namedtuple
from io import StringIO
from datetime import datetime
from keyword import iskeyword
from os.path import dirname
import six

from CoolConvUtilities.AtlCoolLib import indirectOpen

from .channel_mapping import make_channelselection, get_channel_ids_names
from .selection import make_browse_objects_selection
from .sugar import IOVSet, RunLumi, RunLumiType, TimestampType, make_iov_type

DEFAULT_DBNAME = "CONDBR2"

def make_safe_fields(fields):
    return [(field + "_") if iskeyword(field) else field for field in fields]

def get_query_range(since, until, runs):
    """
    Take `since`, `until` and `runs` and turn them into parameters.
    
    If nothing is specified, an infinite range is used.
    If `runs` is an integer, just that run is used
    If `runs` is a two-tuple, then it is used as a (from_run, to_run)
    """
    if runs and (since is None and until is None):
        from builtins import int
        if isinstance(runs, tuple):
            since, until = (runs[0], 0), (runs[1], 0)
            
        elif isinstance(runs, int):
            since, until = (runs, 0), (runs+1, 0)
        else:
            raise RuntimeError("Invalid type for `runs`, should be int or tuple")
            
    elif runs:
        raise RuntimeError("Specify (since and/or until), OR runs, not both")
        
    else:
        if since is None: since = 0
        if until is None: until = 2**63-1
    
    if isinstance(since, tuple): since = RunLumi(*since)
    if isinstance(until, tuple): until = RunLumi(*until)

    if isinstance(since, str): since = TimestampType.from_string(since)
    if isinstance(until, str): until = TimestampType.from_string(until)

    if isinstance(since, datetime): since = TimestampType.from_date(since)
    if isinstance(until, datetime): until = TimestampType.from_date(until)

    assert since <= until, "Bad query range (since > until?)"
    
    return since, until
    
def fetch_iovs(folder_name, since=None, until=None, channels=None, tag="",
               what="all", max_records=-1, with_channel=True, loud=False,
               database=None, convert_time=False, named_channels=False,
               selection=None, runs=None, with_time=False, unicode_strings=False):
    """
    Helper to fetch objects in a pythonic manner
    `folder_name` may be an abbreviated name (DQMFONL) or a fully-qualified name
        (e.g. /GLOBAL/DETSTATUS/DQMFONL)
    `since`, `until` can be (run, lumi) tuples, or standard iov keys
    `channels` can be a cool ChannelSelection object or a list of ids/names
    `tag` COOL folder tag
    `what` is a list of strings specifying which records should be fetched
        if it is the string "all" (not a list), then all records are fetched,
        and naming is turned on.
    `max_records` specifies the maximum number of records to fetch. -1 means all
    `with_channel` specifies whether the channel number should be in the result
        list of each tuple
    `loud` specifies whether quick_retrieve (C++ function) should print its
        status every 1000 objects
    `database` can be used to specify an abbreviated database, or a connection 
        string
    `convert_time` performs a conversion of `since` and `until` from runlumi
        to nanoseconds since the epoch.
    `named_channels` causes the iovs returned to contain strings in the channel 
        identifier
    `selection` [NOT IMPLEMENTED YET] create a cool selection object
    `runs` if it is an integer, it is a run number. If it is a tuple, it is a
        run range.
    `with_time` retrieve insertiontime for iovs
    `unicode_strings` return unicode string objects, assuming database content 
                      is UTF-8
    """
    from .quick_retrieve import quick_retrieve, browse_coracool, get_coracool_payload_spec
    
    if channels == []: return IOVSet()
            
    since, until = get_query_range(since, until, runs)

    channel_mapping = None
    if isinstance(folder_name, str):
        folder = Databases.get_folder(folder_name, database) 
    else:
        try:
            folder = folder_name
            folder_name = folder.fullPath()
        except Exception:
            log.error("Exception when interpreting folder: {0}".format(folder_name))
            raise

    log.info("Querying %s", folder_name)
    log.debug("Query range: [%s, %s]", since, until)
    
    short_folder = folder.fullPath().split("/")[-1]
    
    time_based_folder = "<timeStamp>time</timeStamp>" in folder.description()
    coracool_folder = "<coracool>" in folder.description()
    iov_key_type = TimestampType if time_based_folder else RunLumiType
    
    if time_based_folder and (convert_time or runs):
        # Perform a conversion of the run IoV to a time-based one.
        # Note: probably inadvisable to do this for long ranges since
        #       it has to retrieve all of the luminosity blocks that took place
        #       in the query range.
        
        until = min(until, RunLumi(100000000, 0))
        runrange = fetch_iovs("LBLB", since, until)
        if runrange:
            # If the runrange is empty for some reason, fall back.
            since, until = runrange.first.StartTime, runrange.last.EndTime
            return fetch_iovs(folder_name, since, until, channels, tag, what, 
                              max_records, with_channel, loud, 
                              database, convert_time=False, 
                              named_channels=named_channels, selection=selection, 
                              with_time=with_time, 
                              unicode_strings=unicode_strings)
        else:
            return IOVSet()
    
    detstatus_names = "DQMFOFL", "DCSOFL", "DQMFONL", "SHIFTOFL", "SHIFTONL", "LBSUMM"
    if any(short_folder.endswith(x) for x in detstatus_names):
        channel_mapping = None # get channel mapping from channel_mapping.py
    else:
        _, _, channelmap = get_channel_ids_names(folder)
        cm_reversed = dict((value, key) for key, value in six.iteritems(channelmap))
        channelmap.update(cm_reversed)
        channel_mapping = channelmap
    
    channels = make_channelselection(channels, channel_mapping)    
        
    field_name = "%s_VAL" % short_folder
    
    if not coracool_folder:
        if what == "all":
            what = folder.folderSpecification().payloadSpecification().keys()
        
        channelmap = None
        if named_channels:
            _, _, channelmap = get_channel_ids_names(folder)
                    
        folder.setPrefetchAll(False)
        
        if selection:
            sel = make_browse_objects_selection(folder, selection)
            iterator = folder.browseObjects(since, until, channels, tag, sel)
        else:
            iterator = folder.browseObjects(since, until, channels, tag)
        
        fields = []
        if with_time:
            fields.append("insertion_time")
        if with_channel:
            fields.append("channel")
        fields.extend(what)
        
        record = make_iov_type(field_name, make_safe_fields(fields))
        
        result = quick_retrieve(iterator, record, what, max_records, with_channel, 
                                loud, iov_key_type, channelmap, with_time,
                                unicode_strings)
        
    else:
        args = folder_name, database
        database, folder_path = Databases.resolve_folder_string(*args)
    
        # Coracool
        assert database, "Coracool folder - you must specify a database"
        
        db = Databases.get_instance(database)
        spec =  get_coracool_payload_spec(db, folder_path)
        if what == "all":
            what = spec.keys()
        
        assert isinstance(what, list), ("Coracool folder - you must specify "
            "`what` to fetch (it cannot be inferred, as with non-coracool.)")
        
        record = make_iov_type(field_name, ["channel", "elements"])
        
        element = namedtuple("element", " ".join(make_safe_fields(what)))
    
        result = browse_coracool(db, folder_path, since, until, channels, "", 
                                 what, record, element, iov_key_type)
                
    result = IOVSet(result, iov_type=record, origin=short_folder)
        
    return result

def write_iovs(folder_name, iovs, record, multiversion=True, tag="",
               create=False, storage_buffer=False):    
    args = folder_name, multiversion, record, create
    db, folder, payload = Databases.fetch_for_writing(*args)
    
    if storage_buffer:
        folder.setupStorageBuffer()
        
    total_iovs = len(iovs)
    for i, iov in enumerate(iovs):
        for field_name, field_value in zip(iov._fields[3:], iov[3:]):
            payload[field_name] = field_value
        
        folder.storeObject(iov.since, iov.until, payload, iov.channel, tag)
        if not i % 1000:
            log.debug("Wrote %5i / %5i records", i, total_iovs)

    if storage_buffer:
        log.debug("Flushing records to database...")
        folder.flushStorageBuffer()
        log.debug("... done")

class Databases(object):
    """
    Databases helper class. Used as a singleton. (don't instantiate)
    Specifies abbreviations for database connection strings and folders
    """
    
    FOLDERS = {
        "DQMFONL"       : "COOLONL_GLOBAL::/GLOBAL/DETSTATUS/DQMFONL",
        "DQMFONLLB"     : "COOLONL_GLOBAL::/GLOBAL/DETSTATUS/DQMFONLLB",
        "SHIFTONL"      : "COOLONL_GLOBAL::/GLOBAL/DETSTATUS/SHIFTONL",
        
        "DQMFOFL"       : "COOLOFL_GLOBAL::/GLOBAL/DETSTATUS/DQMFOFL",
        "DCSOFL"        : "COOLOFL_GLOBAL::/GLOBAL/DETSTATUS/DCSOFL",
        "DQCALCOFL"     : "COOLOFL_GLOBAL::/GLOBAL/DETSTATUS/DQCALCOFL",
        "SHIFTOFL"      : "COOLOFL_GLOBAL::/GLOBAL/DETSTATUS/SHIFTOFL",
        "LBSUMM"        : "COOLOFL_GLOBAL::/GLOBAL/DETSTATUS/LBSUMM",
        "VIRTUALFLAGS"  : "COOLOFL_GLOBAL::/GLOBAL/DETSTATUS/VIRTUALFLAGS",
        
        "DEFECTS"       : "COOLOFL_GLOBAL::/GLOBAL/DETSTATUS/DEFECTS",
        
        "SOR_Params"    : "COOLONL_TDAQ::/TDAQ/RunCtrl/SOR_Params",
        "EOR_Params"    : "COOLONL_TDAQ::/TDAQ/RunCtrl/EOR_Params",
        "SOR"           : "COOLONL_TDAQ::/TDAQ/RunCtrl/SOR",
        "EOR"           : "COOLONL_TDAQ::/TDAQ/RunCtrl/EOR",
        
        "LBLB"          : "COOLONL_TRIGGER::/TRIGGER/LUMI/LBLB",
        "LBTIME"        : "COOLONL_TRIGGER::/TRIGGER/LUMI/LBTIME",
        "LBLESTONL"     : "COOLONL_TRIGGER::/TRIGGER/LUMI/LBLESTONL",
        "LVL1COUNTERS"  : "COOLONL_TRIGGER::/TRIGGER/LUMI/LVL1COUNTERS",
        "HLTCOUNTERS"   : "COOLOFL_TRIGGER::/TRIGGER/LUMI/HLTCOUNTERS",
        "HLT/Menu"      : "COOLONL_TRIGGER::/TRIGGER/HLT/Menu",
        "LVL1/Menu"     : "COOLONL_TRIGGER::/TRIGGER/LVL1/Menu",
        "Prescales"     : "COOLONL_TRIGGER::/TRIGGER/LVL1/Prescales",
    }
    
    @classmethod
    def resolve_folder_string(cls, folder_name, db_override=None):
        """
        Resolves a simplified folder URI.
        
        Examples:   
            folder_name == "SHIFTOFL" 
                will give a connection to COOLOFL_GLOBAL/COMP200
                folder /GLOBAL/DETSTATUS/SHIFTOFL
            folder_name == "test.db::SHIFTOFL"
                will give a connection to an sqlite database test.db
        
        Priority:
            * Database specified in db_override
            * Database specified in `folder_name`
            * Database specified in cls.FOLDERS[folder_name]
        """
        res_database = db_override
        
        # First check - is a database specified in the folder name?
        if "::" in folder_name:
            assert folder_name.count("::") == 1, "Bad folder format"
            database, folder_name = folder_name.split("::")
            
            # If res_database hasn't already been set, do so
            res_database = database if not res_database else res_database
        
        if folder_name in cls.FOLDERS:
            database, res_folder = cls.FOLDERS[folder_name].split("::")
            res_database = database if not res_database else res_database
        else:
            res_folder = folder_name
        
        return res_database, res_folder
    
    @classmethod
    def get_folder(cls, folder_name, db_override=None, read_only=True, 
                   create_function=None, also_db=False):
        """
        Retrieve a folder. The `db_override` parameter allows over-riding the
        database which comes from the folder string.
        
        Parameters:
            `folder_name` : Folder name or alias to load
            `db_override` : If specified, causes an alternate database string
                            to be used.
            `read_only`   : Specifies if a read-only database connection should
                            be used.
            `create_function` : If specified, function to be called in case the
                                folder doesn't exist. It is passed the database
                                connection.
            `also_db`     : Make the return value (db, folder)
        """
        
        if read_only:
            assert not create_function, "`read_only` specified with `create`"
        
        database, folder = cls.resolve_folder_string(folder_name, db_override)
        assert database, ("Unable to resolve database for (%s, %s)"
                          % (folder_name, db_override))
        
        create = bool(create_function)
        db = cls.get_instance(database, read_only, create)
        
        try:
            cool_folder = db.getFolder(folder)
        except Exception as error:
            log.debug('HELP! %s', error.args)
            args = str(error.args[0] if not isinstance(error.args, str) else error.args)
            log.debug('THIS IS %s', type(args))
            log.debug('Value of boolean: %s', ("not found" in args))
            if not ("cannot be established" in args or
                     "not found" in args
                    ):
                log.exception("Unknown error encountered whilst opening "
                              "database connection to '%s'",
                              database)
                raise
                
            if not create_function:
                log.exception("Database does not exist, `create_function` not "
                              "specified")
                raise
                
            cool_folder = create_function(db)
        if also_db:
            return db, cool_folder
        return cool_folder
        
    @classmethod
    def resolve_db_string(cls, db_string, read_only=True):
        """
        Take a database string - if it looks like a filename ending in ".db"
        then assume it is a sqlite database with that name.
        
        If the `db_string` is prepended with "WRITE|" then a writable connection
        is requested.
        
        If the db_string doesn't contain a "/", then "/" + DEFAULT_DBNAME is
        appended.
        """
        if "://" in db_string:
            # Assume that the string is already resolved
            return db_string, read_only
        
        if db_string.startswith("WRITE|"):
            assert db_string.count("|") == 1, "Bad db_string format"
            db_string = db_string.split("|")[1]
            read_only = False
        
        sqlite_regex = re.compile(r"(?P<filename>.*?\.db)(?:/?(?P<dbname>[^/]+))?$")
        matched = sqlite_regex.match(db_string)
        if matched:
            filename, dbname = matched.groups()
            dbname = DEFAULT_DBNAME if not dbname else dbname
            db_string = "sqlite://schema=%s;dbname=%s" % (filename, dbname)            
        else:
            if "/" not in db_string:
                return db_string + "/" + DEFAULT_DBNAME, read_only
            
        return db_string, read_only
    
    @classmethod
    def get_instance(cls, db_string, read_only=True, create=False):
        """
        Open a database connection
        """
        res_db_string, read_only = cls.resolve_db_string(db_string, read_only)
            
        try:
            prev_stdout = sys.stdout
            sys.stdout = StringIO()
            try:
                connection = indirectOpen(res_db_string, readOnly=read_only,
                                          oracle=True)
            finally:
                sys.stdout = prev_stdout
        except Exception as e:
            if "The database does not exist" in e.args[0] and not create:
                log.info("Failed trying to connect to '%s'", res_db_string)
                raise
            from PyCool import cool
            dbService = cool.DatabaseSvcFactory.databaseService()
            connection = dbService.createDatabase(res_db_string)
        except Exception:
            log.error(sys.exc_info()[0])
            raise
        return connection

    @classmethod
    def build_folder(cls, db, folder_name, multiversion, record):
        """
        Create `folder_name` on database instance `db`, with recordspecification
        `record`.
        
        Also creates folderset to which folder_name belongs if necessary.
        """
        from PyCool import cool
        
        folderset_path = dirname(folder_name)
        try:
            db.getFolderSet(folderset_path)
        except Exception as error:
            caught_error = "Folder set %s not found" % folderset_path
            if caught_error not in error.args[0]:
                raise

            log.debug("Folderset doesn't exist - creating it.")
            db.createFolderSet(folderset_path, "", True)

        if not isinstance(record, cool.RecordSpecification):
            record_spec = cool.RecordSpecification()
            for field in record:
                record_spec.extend(*field)
        else:
            record_spec = record
        
        FV = cool.FolderVersioning
        versioning = FV.MULTI_VERSION if multiversion else FV.SINGLE_VERSION
        folder_spec = cool.FolderSpecification(versioning, record_spec)
        folder = db.createFolder(folder_name, folder_spec)
        payload = cool.Record(record_spec)
        
        return folder, payload

    @classmethod
    def fetch_for_writing(cls, orig_folder_name, multiversion=True, record=None,
                          create=False, db_override=None):
        """
        Retrieve a folder for writing. Creates it if it doesn't exist.
        
        `folder_name` specifies folder to be queried
        `multiversion` specifies COOL versioning mode
        `record` is a list of fields to be created in the form:
            [("<field name>", cool.StorageType.<field type>), ...]
            or if None, defaults to one Code record,
            or if isinstance(record, cool.RecordSpecification), uses this.
        `create` should the database be created if it doesn't
        `db_override` overrides automatic detection of database string
        """
        from PyCool import cool
        if record is None:
            record = [("Code", cool.StorageType.Int32)]        

        database, folder_name = cls.resolve_folder_string(orig_folder_name)
        if db_override:
            database = db_override
        
        try:
            db = cls.get_instance(database, False)
            
        except Exception as error:
            if not create or "The database does not exist" not in error.args[0]:
                raise
            
            from PyCool import cool
            dbService = cool.DatabaseSvcFactory.databaseService()
            
            resolved_database, _ = cls.resolve_db_string(database)
            
            log.info("Database doesn't exist - creating it.")
            db = dbService.createDatabase(resolved_database)

        try:
            folder = db.getFolder(folder_name)
            payload = cool.Record(folder.payloadSpecification())
        except Exception as error:
            if not create or "Folder %s not found" % folder_name not in error.args[0]:
                raise
                
            log.debug("Folder doesn't exist - creating it.")
            args = db, folder_name, multiversion, record
            folder, payload = cls.build_folder(*args)

        return db, folder, payload
