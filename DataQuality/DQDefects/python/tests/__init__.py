# -*- coding: utf-8 -*-
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from os import unlink, environ
from os.path import exists
from nose import with_setup

from logging import getLogger; log = getLogger("DQDefects.tests")

from DQUtils import init_logger
from DQDefects import DefectsDB
from DQDefects.exceptions import DefectExistsError, DefectUnknownError

TEST_DATABASE = "test_defects.db/COMP200"

if "FORCELOG" in environ:
    init_logger(2)
   
def iov_ranges(iovs):
	return [(i.since, i.until) for i in iovs]

teardown_db = None
def create_database():
    ddb = DefectsDB(TEST_DATABASE, read_only=False, create=True)
    global teardown_db
    teardown_db = ddb

def teardown_database():
    # force garbage collection before database teardown so as not to create
    # lots of .__afs* files.
    global teardown_db
    teardown_db._clear_connections()
    teardown_db = None
    from gc import collect
    collect()
    
    connection_string = "/".join(TEST_DATABASE.split("/")[:-1])
    if connection_string.endswith(".db"):
        log.info("Deleting %s", connection_string)
        unlink(connection_string)
        return
    
    msg = "I don't know how to tear down %s" % TEST_DATABASE
    log.fatal(msg)
    raise NotImplementedError(msg)

@with_setup(create_database, teardown_database)
def test_database_creation():
    assert exists("test_defects.db")
    log.info("Created database %s", TEST_DATABASE)
    
@with_setup(create_database, teardown_database)
def test_database_retrieval():
    ddb = DefectsDB(TEST_DATABASE)
    defects = ddb.retrieve()
    assert len(defects) == 0, "Unexpected records on the database"

def create_defect_type(ddb, n):
    defect_name = "DQD_TEST_DEFECT_%i" % n
    ddb.create_defect(defect_name, "A test defect (%i)" % n)
    return defect_name

def create_defect_type_unicode(ddb, n, desc):
    defect_name = "DQD_TEST_DEFECT_%i" % n
    ddb.create_defect(defect_name, desc)
    return defect_name
    
@with_setup(create_database, teardown_database)
def test_defect_creation():
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    create_defect_type(ddb, 0)
        
    assert len(ddb.defect_ids) == 1


@with_setup(create_database, teardown_database)
def test_virtual_defects_invert():
    "Testing virtual defects involving inversion"
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    create_defect_type(ddb, 2)
    
    # Make a virtual defect whose result is the combination of the above
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", "Comment", 
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1 !DQD_TEST_DEFECT_2")
    
    # Insert an iov into each
    ddb.insert("DQD_TEST_DEFECT_0", 0,   100, "Test", "DQDefects.tests")
    ddb.insert("DQD_TEST_DEFECT_1", 150, 200, "Test", "DQDefects.tests")
    ddb.insert("DQD_TEST_DEFECT_2", 50, 125, "Test", "DQDefects.tests")
    
    # See what the result of the virtual defect is
    iovs = ddb.retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    
    # Tests against the value
    assert len(iovs) == 4, list(iovs)
    first, second, third, fourth = iovs
    assert (first.since, first.until) == (0, 50), first
    assert (second.since, second.until) == (50, 100), second
    assert (third.since, third.until) == (125, 150), third
    assert (fourth.since, fourth.until) == (150, 200), fourth
    assert first.channel == second.channel == third.channel == fourth.channel == "DQD_TEST_VIRTUAL_DEFECT", (first.channel, second.channel, third.channel, fourth.channel)
    del first, second, third, fourth, iovs
    
    # Test evaluate_full=False
    iovs = ddb.retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"], evaluate_full=False)
    assert len(iovs) == 2, list(iovs)

    # Now unset the present bit
    ddb.insert("DQD_TEST_DEFECT_2", 50, 150, "Test", "DQDefects.tests", present=False)
    iovs = ddb.retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])

    # here we are sensitive to details of python ordering ...
    assert len(iovs) == 3, list(iovs)
    first, second, third = iovs
    assert (first.since, first.until) == (0, 100), first
    assert (second.since, second.until) == (100, 150), second
    assert (third.since, third.until) == (150, 200), third
    assert first.channel == second.channel == third.channel == "DQD_TEST_VIRTUAL_DEFECT", (first.channel, second.channel, third.channel)
    del first, second, third, iovs
    

@with_setup(create_database, teardown_database)
def test_defect_insertion_retrieval():
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    TEST_ID = 0
    TEST_DEFECT_NAME = "DQD_TEST_DEFECT_%i" % TEST_ID
    TEST_COMMENT = "First test defect"
    TEST_USER = "DQDefects.tests"
    TEST_SINCE, TEST_UNTIL = 0, 100
    
    create_defect_type(ddb, TEST_ID)
    
    ddb.insert(TEST_DEFECT_NAME, TEST_SINCE, TEST_UNTIL, TEST_COMMENT, TEST_USER)
    
    iovs = ddb.retrieve()
    
    assert len(iovs) == 1
    iov = iovs[0]
    assert iov.since == TEST_SINCE
    assert iov.until == TEST_UNTIL
    assert iov.channel == TEST_DEFECT_NAME
    assert iov.present
    assert not iov.recoverable
    assert iov.user == TEST_USER
    assert iov.comment == TEST_COMMENT

@with_setup(create_database, teardown_database)
def test_defect_insertion_retrieval_unicode():
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    TEST_ID = 0
    TEST_DEFECT_NAME = "DQD_TEST_DEFECT_%i" % TEST_ID
    TEST_COMMENT = u"First test defect with character: ±"
    TEST_USER = u"DQDefécts.tests"
    TEST_SINCE, TEST_UNTIL = 0, 100
    TEST_DEFECT_DESCRIPTION = u"À test defect (%i)" % TEST_ID
    
    create_defect_type_unicode(ddb, TEST_ID, TEST_DEFECT_DESCRIPTION)
    
    ddb.insert(TEST_DEFECT_NAME, TEST_SINCE, TEST_UNTIL, TEST_COMMENT, TEST_USER)
    
    iovs = ddb.retrieve()
    
    assert len(iovs) == 1
    iov = iovs[0]
    assert iov.present
    assert not iov.recoverable
    assert iov.user == TEST_USER
    assert iov.comment == TEST_COMMENT
    assert ddb.all_defect_descriptions[TEST_DEFECT_NAME] == TEST_DEFECT_DESCRIPTION
    
@with_setup(create_database, teardown_database)
def test_defect_empty_retrieval():
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    iovs = ddb.retrieve()
    assert not iovs
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    
    # Make a virtual defect whose result is the combination of the above
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT",
                           "Comment", "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1")
    
    iovs = ddb.retrieve()
    assert not iovs
    
@with_setup(create_database, teardown_database)
def test_virtual_defect_creation():
    
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    
    # Make a virtual defect whose result is the combination of the above
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", 
                           "Comment", "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1")
    
    # First test ensures internals are correct
    assert len(ddb.virtual_defect_ids) == 1
    assert "DQD_TEST_VIRTUAL_DEFECT" in ddb.virtual_defect_names
    
    # Second test to ensure database is in correct state
    ddb = DefectsDB(TEST_DATABASE)
    assert len(ddb.virtual_defect_ids) == 1
    assert "DQD_TEST_VIRTUAL_DEFECT" in ddb.virtual_defect_names

@with_setup(create_database, teardown_database)
def test_nonpresent():
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    create_defect_type(ddb, 0)
    
    ddb.insert("DQD_TEST_DEFECT_0", 0, 100, "Test", "DQDefects.tests")
    ddb.insert("DQD_TEST_DEFECT_0", 0, 100, "Test", "DQDefects.tests", present=False)
    
    iovs = ddb.retrieve()
    
    assert len(iovs) == 0
    
    iovs = ddb.retrieve(nonpresent=True)
    
    assert len(iovs) == 1
    assert not iovs[0].present
    
@with_setup(create_database, teardown_database)
def test_many_sparse_channels():
    "Testing many virtual channels with sparsely placed IoVs"
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    all_defects = []
    for i in xrange(52):
        all_defects.append(create_defect_type(ddb, i*2))
        
    with ddb.storage_buffer:
        for i in all_defects:
            ddb.insert(i, 0, 100, "Test", "DQDefects.tests")
    
    queried_defects = all_defects[:-1]
    result = ddb.retrieve(channels=queried_defects)
    queried_defects_set = set(queried_defects)
    result_defects_set = set(i.channel for i in result)
    
    unwanted_defects_set = result_defects_set - queried_defects_set
    assert not unwanted_defects_set, "Unwanted defects: %r" % unwanted_defects_set
    
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", "Comment", 
                           " ".join(all_defects))
                           
    result = ddb.retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    assert result.channels == set(["DQD_TEST_VIRTUAL_DEFECT"])
    assert len(result) == 1
    # This assumes evaluate_full == True
    assert result[0].comment == " ".join(sorted(all_defects)), result[0].comment
    
    result = ddb.retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"],
                          evaluate_full=False)
                          
    assert result.channels == set(["DQD_TEST_VIRTUAL_DEFECT"])
    assert len(result) == 1

@with_setup(create_database, teardown_database)
def test_virtual_defects():
    "Testing virtual defect basics"
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    
    # Make a virtual defect whose result is the combination of the above
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", "Comment", 
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1")
    
    # Insert an iov into each
    ddb.insert("DQD_TEST_DEFECT_0", 0,   100, "Test", "DQDefects.tests")
    ddb.insert("DQD_TEST_DEFECT_1", 150, 200, "Test", "DQDefects.tests")
    
    # See what the result of the virtual defect is
    iovs = ddb.retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    
    # Tests against the value
    assert len(iovs) == 2
    first, second = iovs
    assert (first.since, first.until) == (0, 100)
    assert (second.since, second.until) == (150, 200)
    assert first.channel == second.channel == "DQD_TEST_VIRTUAL_DEFECT"
    del first, second, iovs
    
    # Now unset the present bit, and test again that the relevant preiod is fixed
    ddb.insert("DQD_TEST_DEFECT_1", 150, 200, "Test", "DQDefects.tests", present=False)
    
    iovs = ddb.retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    
    assert len(iovs) == 1
    first = iovs[0]
    assert (first.since, first.until) == (0, 100)
    assert first.channel == "DQD_TEST_VIRTUAL_DEFECT"
    
    # Now extend the defect and see if we get a contiguous result
    ddb.insert("DQD_TEST_DEFECT_0", 100, 150, "Test", "DQDefects.tests")
    iovs = ddb.retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    
    assert len(iovs) == 1
    first = iovs[0]
    assert (first.since, first.until) == (0, 150)
    assert first.channel == "DQD_TEST_VIRTUAL_DEFECT"
    
@with_setup(create_database, teardown_database)
def test_virtual_defects_deep():
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    
    # Insert an iov into each
    ddb.insert("DQD_TEST_DEFECT_0", 0,   100, "Test", "DQDefects.tests")
    ddb.insert("DQD_TEST_DEFECT_1", 150, 200, "Test", "DQDefects.tests")
    
    # Make a virtual defect whose result is the combination of the above
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT_0", "", "DQD_TEST_DEFECT_0")
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT_1", "", "DQD_TEST_DEFECT_1")
    
    ddb = DefectsDB(TEST_DATABASE)
    
    iovs = ddb.retrieve()
    
@with_setup(create_database, teardown_database)
def test_virtual_defects_stress():
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    STRESS_COUNT = 20
    
    from time import time
    start = time()
    for i in xrange(STRESS_COUNT):
        create_defect_type(ddb, i)
        
    start = time()
    with ddb.storage_buffer:
        for i in xrange(STRESS_COUNT):
            ddb.insert("DQD_TEST_DEFECT_%i" % i, i, i+1, "Test", "DQDefects.tests")
        
    ALL_DEFECTS = " ".join("DQD_TEST_DEFECT_%i" % i for i in xrange(STRESS_COUNT))
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", "", ALL_DEFECTS)
    
    start = time()
    iovs = ddb.retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    
    assert len(iovs) == STRESS_COUNT - 1
    assert (iovs[0].since, iovs[-1].until) == (1, STRESS_COUNT)
    
@with_setup(create_database, teardown_database)
def test_virtual_defect_hiding():
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    
    # Make a virtual defect whose result is the combination of the above
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", "Comment", 
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1")
        
    new_tag = ddb.new_logics_tag()
                       
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT_2", "Comment",
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1")

    ddb2 = DefectsDB(TEST_DATABASE, tag=('HEAD', new_tag), read_only=True)
    assert ddb2.virtual_defect_names == set(['DQD_TEST_VIRTUAL_DEFECT']), 'Only one virtual defect should be in this tag'
    
@with_setup(create_database, teardown_database)
def test_update_virtual_defect():
    
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    create_defect_type(ddb, 2)
    
    ddb.insert("DQD_TEST_DEFECT_0",   0, 100, "", "")
    ddb.insert("DQD_TEST_DEFECT_1", 100, 200, "", "")
    ddb.insert("DQD_TEST_DEFECT_2", 200, 300, "", "")
    
    defects_tag = ddb.new_defects_tag("dqd-test", "New iov tag")
    
    # Make a virtual defect whose result is the combination of the above
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", "", 
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1")
    
    original_tag = ddb.new_logics_tag()
    original_htag = ddb.new_hierarchical_tag(defects_tag, original_tag)
                       
    ddb.update_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", 
                              "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_2")
    
    new_tag = ddb.new_logics_tag()
    new_htag = ddb.new_hierarchical_tag(defects_tag, new_tag)
        
    what = {"channels": ["DQD_TEST_VIRTUAL_DEFECT"]}
    
    orig_iovs = DefectsDB(TEST_DATABASE, tag=original_htag).retrieve(**what)
    new_iovs  = DefectsDB(TEST_DATABASE, tag=new_htag).retrieve(**what)
    head_iovs = DefectsDB(TEST_DATABASE).retrieve(**what)
    
    assert head_iovs == new_iovs
    assert head_iovs != orig_iovs
    
    assert len(head_iovs) == 2
    assert (head_iovs[0].since, head_iovs[0].until) == (  0, 100)
    assert (head_iovs[1].since, head_iovs[1].until) == (200, 300)

    from DQUtils.db import Databases
    db3 = Databases.get_instance(TEST_DATABASE, read_only=True)
    pfs = db3.getFolderSet('/GLOBAL/DETSTATUS')
    assert pfs.tagDescription(new_htag) != '', 'Hierarchical tag description not created'
    
@with_setup(create_database, teardown_database)
def test_rename_defects():
    
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    
    ddb.insert("DQD_TEST_DEFECT_0",   0, 100, "", "")
    ddb.insert("DQD_TEST_DEFECT_1", 100, 200, "", "")
    
    defects_tag = ddb.new_defects_tag("dqd-test", "New iov tag")
    
    # Make a virtual defect whose result is the combination of the above
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", "", 
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1")
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT_2", "", 
                           "DQD_TEST_VIRTUAL_DEFECT")
    # Make ones with inversion logic
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT_3", "", 
                           "DQD_TEST_DEFECT_0 !DQD_TEST_DEFECT_1")
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT_4", "", 
                           "!DQD_TEST_VIRTUAL_DEFECT")
    
    original_tag = ddb.new_logics_tag()
    original_htag = ddb.new_hierarchical_tag(defects_tag, original_tag)

    # The following is no longer considered an error condition
    ## exc_raised = False
    ## try:
    ##     ddb.rename_defect("DQD_TEST_DEFECT_0", "DQD_TEST_0")
    ## except AssertionError:
    ##     exc_raised = True
    ## assert exc_raised, "Bad defect rename did not raise exception"

    # first rename a primary defect
    ddb.rename_defect("DQD_TEST_DEFECT_0", "DQD_TEST_DEFECT_3")
    assert ddb.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT'].clauses == ['DQD_TEST_DEFECT_3', 'DQD_TEST_DEFECT_1'], ddb.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT'].clauses
    assert ddb.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT_3'].clauses == ['DQD_TEST_DEFECT_3', '!DQD_TEST_DEFECT_1'], ddb.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT_3'].clauses

    # also valid in other tags?
    ddb2 = DefectsDB(TEST_DATABASE, tag=original_htag)
    assert ddb2.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT'].clauses == ['DQD_TEST_DEFECT_3', 'DQD_TEST_DEFECT_1'], ddb2.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT'].clauses

    # Now try to rename a virtual defect
    ddb.rename_defect("DQD_TEST_VIRTUAL_DEFECT", "DQD_TEST_VIRTUAL_DEFECT_A")

    assert ddb.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT_2'].clauses == ['DQD_TEST_VIRTUAL_DEFECT_A'], ddb.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT_2'].clauses
    assert ddb.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT_4'].clauses == ['!DQD_TEST_VIRTUAL_DEFECT_A'], ddb.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT_4'].clauses

    # also valid in other tags?
    ddb2 = DefectsDB(TEST_DATABASE, tag=original_htag)
    assert ddb2.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT_2'].clauses == ['DQD_TEST_VIRTUAL_DEFECT_A'], ddb2.virtual_defect_logics['DQD_TEST_VIRTUAL_DEFECT_2'].clauses

    ddb._virtual_defect_consistency_check()

def iov_result_depends(iovs):
    caused_bad = set()
    for iov in iovs:
        if iov.user == "sys:virtual":
            caused_bad |= set(iov.comment.split())
    return caused_bad

@with_setup(create_database, teardown_database)
def test_ignore_defects():
    
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    create_defect_type(ddb, 2)
    
    ddb.insert("DQD_TEST_DEFECT_0",   0, 100, "", "")
    ddb.insert("DQD_TEST_DEFECT_1", 100, 200, "", "")
    ddb.insert("DQD_TEST_DEFECT_2", 200, 300, "", "")
    
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", "", 
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1 DQD_TEST_DEFECT_2")

    ignored_defects = set(["DQD_TEST_DEFECT_1"])
    
    iovs = ddb.retrieve()
    assert iov_result_depends(iovs) & ignored_defects, "test is broken"
    
    iovs = ddb.retrieve(ignore=ignored_defects)
    assert not iov_result_depends(iovs) & ignored_defects, "result depends on ignored defects"
    
@with_setup(create_database, teardown_database)
def test_query_with_primary_dependencies():
    
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    defect_names = [create_defect_type(ddb, i) for i in xrange(3)]
    
    for i, defect in enumerate(defect_names):
        ddb.insert(defect, i*100, (i+1)*100, "", "")
    
    ddb.new_virtual_defect("DQD_TEST_VDEFECT", "", " ".join(defect_names))

    iovs = ddb.retrieve(channels=["DQD_TEST_VDEFECT"])
    assert iovs.channels == set(["DQD_TEST_VDEFECT"])
    assert len(iovs) == len(defect_names)

    iovs = ddb.retrieve(channels=["DQD_TEST_VDEFECT"],
                        with_primary_dependencies=True)
    assert iovs.channels == set(["DQD_TEST_VDEFECT"] + defect_names), (
        "Check failed.")
        
    assert len(iovs) == len(defect_names)*2

@with_setup(create_database, teardown_database)
def test_independent_tags():
    
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    create_defect_type(ddb, 2)
    
    ddb.insert("DQD_TEST_DEFECT_0",   0, 100, "", "")
    ddb.insert("DQD_TEST_DEFECT_1", 100, 200, "", "")
    ddb.insert("DQD_TEST_DEFECT_2", 200, 300, "", "")
    
    dtag1 = ddb.new_defects_tag("dqd-test", "New iov tag")
    
    ddb.insert("DQD_TEST_DEFECT_0",   0, 100, "", "", present=False)
    
    dtag2 = ddb.new_defects_tag("dqd-test-2", "New iov tag")
    
    # Make a virtual defect whose result is the combination of the above
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", "", 
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1")
    
    ltag1 = ddb.new_logics_tag()
                       
    ddb.update_virtual_defect("DQD_TEST_VIRTUAL_DEFECT", 
                              "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_2")
    
    ltag2 = ddb.new_logics_tag()
    
    iovs = DefectsDB(TEST_DATABASE, tag=("HEAD", ltag1)).retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    assert iov_ranges(iovs) == [(100, 200)], str(iov_ranges(iovs))
    
    iovs = DefectsDB(TEST_DATABASE, tag=("HEAD", ltag2)).retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    assert iov_ranges(iovs) == [(200, 300)], str(iov_ranges(iovs))
    
    iovs = DefectsDB(TEST_DATABASE, tag=(dtag1, ltag1)).retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    assert iov_ranges(iovs) == [(0, 100), (100, 200)], str(iov_ranges(iovs))
    
    iovs = DefectsDB(TEST_DATABASE, tag=(dtag2, ltag2)).retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    assert iov_ranges(iovs) == [(200, 300)], str(iov_ranges(iovs))

    htag = ddb.new_hierarchical_tag(dtag2, ltag2)

    iovs = DefectsDB(TEST_DATABASE, tag=(htag, htag)).retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    assert iov_ranges(iovs) == [(200, 300)], str(iov_ranges(iovs))
    
    iovs = DefectsDB(TEST_DATABASE, tag=htag).retrieve(channels=["DQD_TEST_VIRTUAL_DEFECT"])
    assert iov_ranges(iovs) == [(200, 300)], str(iov_ranges(iovs))
    
@with_setup(create_database, teardown_database)
def test_query_with_primary_dependencies_with_ignore():
    """
    Checking with_primary_dependencies + ignore
    
    Checks that the set of channels and iovs returned contains the correct 
    number of channels and iovs when specifying both with_primary_dependencies 
    and ignore to DQDefects.Retrieve
    """
    
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    defect_names = [create_defect_type(ddb, i) for i in xrange(3)]
    
    for i, defect in enumerate(defect_names):
        ddb.insert(defect, i*100, (i+1)*100, "", "")
    
    ignored = set(defect_names[:1])
    
    ddb.new_virtual_defect("DQD_TEST_VDEFECT", "", " ".join(defect_names))

    iovs = ddb.retrieve(channels=["DQD_TEST_VDEFECT"], ignore=ignored)
    assert iovs.channels == set(["DQD_TEST_VDEFECT"])
    assert len(iovs) == len(defect_names)-1

    iovs = ddb.retrieve(channels=["DQD_TEST_VDEFECT"], ignore=ignored,
                        with_primary_dependencies=True)
    assert iovs.channels == set(["DQD_TEST_VDEFECT"] + defect_names) - ignored, (
        "Check failed.")
        
    assert len(iovs) == len(defect_names[1:])*2
    
def test_virtual_defect_consistency():
    """
    Checking that virtual flags don't depend on non-existent flags
    """
    DefectsDB('oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_GLOBAL;dbname=COMP200')._virtual_defect_consistency_check()
    
@with_setup(create_database, teardown_database)
def test_iov_tag_defects():
    log.info('IOV Tags')
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    create_defect_type(ddb, 2)
    
    ddb.insert("DQD_TEST_DEFECT_0",   0, 100, "comment1", "user1", present=False)
    ddb.insert("DQD_TEST_DEFECT_1", 100, 200, "", "")
    ddb.insert("DQD_TEST_DEFECT_2", 200, 300, "comment2", "user2", recoverable=True)
    
    defects_tag = ddb.new_defects_tag("dqd-test", "New iov tag",
                                      iovranges=[(0, 51),
                                                 ((0,210), (0,306))])
    ddb2 = DefectsDB(TEST_DATABASE, tag='DetStatusDEFECTS-dqd-test')
    iovs = ddb2.retrieve(nonpresent=True)
    assert len(iovs) == 2
    assert ((iovs[0].channel, iovs[0].since, iovs[0].until,
             iovs[0].present, iovs[0].recoverable, iovs[0].user,
             iovs[0].comment)
            ==
            ('DQD_TEST_DEFECT_0', 0, 51,
             False, False, 'user1',
             'comment1'))       
    assert ((iovs[1].channel, iovs[1].since, iovs[1].until,
             iovs[1].present, iovs[1].recoverable, iovs[1].user,
             iovs[1].comment)
            ==
            ('DQD_TEST_DEFECT_2', 210, 300,
             True, True, 'user2',
             'comment2'))       

@with_setup(create_database, teardown_database)
def test_intersect():
    
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    create_defect_type(ddb, 2)
    
    ddb.insert("DQD_TEST_DEFECT_0",   0,    1000, "", "")
    ddb.insert("DQD_TEST_DEFECT_0",   1000, 2000, "", "")
    
    iovs = ddb.retrieve(50, 100, intersect=True)
    assert len(iovs) == 1
    assert (iovs[0].since, iovs[0].until) == (50, 100)

    iovs = ddb.retrieve(999, 1001, intersect=True)
    assert len(iovs) == 2
    first, second = iovs
    assert (first.since,  first.until)  == (999,  1000)
    assert (second.since, second.until) == (1000, 1001)

@with_setup(create_database, teardown_database)
def test_normalize_defect_names():
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create two defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    
    # Make a virtual defect whose result is the combination of the above
    ddb.new_virtual_defect("DQD_TEST_Virtual_DEFECT", "Comment", 
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1")

    assert ddb.normalize_defect_names('dQd_tEsT_DeFeCt_0')=='DQD_TEST_DEFECT_0'
    assert ddb.normalize_defect_names(['dQd_tEsT_DeFeCt_0', 'DqD_TeSt_dEfEcT_1'])==['DQD_TEST_DEFECT_0', 'DQD_TEST_DEFECT_1']
    assert ddb.normalize_defect_names('dqd_test_virtual_defect')=='DQD_TEST_Virtual_DEFECT'
    try:
        ddb.normalize_defect_names('does_not_exist')
        wasthrown = False
    except DefectUnknownError:
        wasthrown = True
    assert wasthrown, 'normalize_defect_names should throw when the defect does not exist'
    
@with_setup(create_database, teardown_database)
def test_reject_duplicate_names():
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    ddb.create_defect('A_TEST_DEFECT', 'A test defect')
    
    wasthrown = False
    try:
        ddb.create_defect('A_TEST_DEFECT', 'A test defect 2')
    except DefectExistsError:
        wasthrown = True
    assert wasthrown, 'Creation of duplicate defects should throw'

    wasthrown = False
    try:
        ddb.create_defect('A_test_defect', 'A test defect 2')
    except DefectExistsError:
        wasthrown = True
    assert wasthrown, 'Creation of duplicate defects should throw, even with different case'
    
@with_setup(create_database, teardown_database)
def test_get_intolerable_defects():
    '''
    Check that the intolerable defect listing function works
    '''
    ddb = DefectsDB(TEST_DATABASE, read_only=False)
    
    # Create some primary defects
    create_defect_type(ddb, 0)
    create_defect_type(ddb, 1)
    create_defect_type(ddb, 2)
    create_defect_type(ddb, 3)
    
    # Make some virtual defects
    ddb.new_virtual_defect("DQD_TEST_Virtual_DEFECT", "Comment", 
                           "DQD_TEST_DEFECT_0")
    ddb.new_virtual_defect("PRIMARY", "Comment",
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1")
    ddb.new_virtual_defect("PHYS_test", "Comment",
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1 DQD_TEST_DEFECT_2")
    ddb.new_virtual_defect("TIGHT", "Comment",
                           "DQD_TEST_DEFECT_0 DQD_TEST_DEFECT_1 DQD_TEST_DEFECT_2 DQD_TEST_DEFECT_3")

    # old_primary_only should be True here, so PRIMARY gives the set of defects
    assert sorted(ddb.get_intolerable_defects()) == ["DQD_TEST_DEFECT_0", "DQD_TEST_DEFECT_1"], "default params/old_primary_only check failed"
    assert sorted(ddb.get_intolerable_defects(old_primary_only=False, exclude=['TIGHT'])) == ["DQD_TEST_DEFECT_0", "DQD_TEST_DEFECT_1", "DQD_TEST_DEFECT_2"], 'old_primary_only=False check failed'
    assert sorted(ddb.get_intolerable_defects(old_primary_only=False)) == ["DQD_TEST_DEFECT_0", "DQD_TEST_DEFECT_1"], 'regex handling check failed'
    
@with_setup(create_database, teardown_database)
def test_list_empty_tag():
    """
    Checking that listing a tag works even if the tag is empty
    """
    ddb = DefectsDB(TEST_DATABASE, read_only=False, tag="nominal")
    ddb.virtual_defect_names
    
@with_setup(create_database, teardown_database)
def test_noncontiguous_defectid_creation():
    """
    Test that defects are inserted correctly into non-contiguous ranges
    """
    ddb = DefectsDB(TEST_DATABASE, read_only=False)

    create_defect_type(ddb, 0)
    ddb._create_defect_with_id(2, "TEST_DEFECT_DQD_2", "")
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT",
                           "Comment", "DQD_TEST_DEFECT_0 TEST_DEFECT_DQD_2")
    ddb.new_virtual_defect("DQD_TEST_VIRTUAL_DEFECT_2",
                           "Comment", "DQD_TEST_DEFECT_0 TEST_DEFECT_DQD_2")
    create_defect_type(ddb, 1)
    create_defect_type(ddb, 3)
    # assert primary and virtual defects are right
    ids, names, m = ddb.get_channels()
    assert m == {0L: 'DQD_TEST_DEFECT_0', 1L: 'DQD_TEST_DEFECT_1', 2: 'TEST_DEFECT_DQD_2', 3L: 'DQD_TEST_DEFECT_3', 'DQD_TEST_DEFECT_3': 3L, 'DQD_TEST_DEFECT_0': 0L, 'DQD_TEST_DEFECT_1': 1L, 'TEST_DEFECT_DQD_2': 2}, 'Primary defect problem'
    ids, names, m = ddb.get_virtual_channels()
    assert m == {2147483648L: 'DQD_TEST_VIRTUAL_DEFECT', 2147483649L: 'DQD_TEST_VIRTUAL_DEFECT_2', 'DQD_TEST_VIRTUAL_DEFECT': 2147483648L, 'DQD_TEST_VIRTUAL_DEFECT_2': 2147483649L}, 'Virtual defect problem'
