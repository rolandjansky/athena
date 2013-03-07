#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from sqlalchemy import (select, create_engine, MetaData, Table, Column, String, 
    Integer)
from sqlalchemy.sql import and_

from DQUtils.sugar import IOVSet
from xml.dom.minidom import parse
from os.path import exists
from urllib import splithost

def parse_auth_file(file_name, connection):

    dom = parse(file_name)
    
    connections = dom.getElementsByTagName("connection")
    desired_conn = lambda c: c.attributes.get("name").value == connection
    
    connections = filter(desired_conn, connections)
    
    if len(connections) < 1:
        return None
    
    info = {}
    for node in connections[0].childNodes:
        if node.nodeName == "parameter":
            info[node.getAttribute("name")] = str(node.getAttribute("value"))
                
    authentication = info["user"], info["password"]
    return authentication

def get_authentication(connection="oracle://ATLAS_COOLPROD/ATLAS_COOLONL_GLOBAL"):
    """
    Retrieves authentication information from CORAL_AUTH_PATH authentication.xml
    """
    
    from os import environ
    from os.path import join as pjoin
    assert "CORAL_AUTH_PATH" in environ, "CORAL_AUTH_PATH environment var not set"
    
    auth_paths = environ["CORAL_AUTH_PATH"].split(":")
    
    for auth_path in auth_paths + ["."]:
        file_name = pjoin(auth_path, "authentication.xml")
        if exists(file_name):
            authentication = parse_auth_file(file_name, connection)
            if authentication:
                return authentication
    
    raise RuntimeError("Unable to locate credentials for %s." 
                          % connection)

def make_oracle_connection(connection_string):
    "oracle://ATLAS_COOLPROD/ATLAS_COOLONL_GLOBAL"
    assert connection_string.startswith("oracle://"), "Not a connection string"
    host, user = splithost(connection_string[len("oracle:"):])
    username, password = get_authentication(connection_string)
    conn_str = "oracle://%s:%s@%s" % (username, password, host)
    engine = create_engine(conn_str, pool_recycle=10*60)
    metadata = MetaData(bind=engine)
    return engine, metadata

#conn_str = "oracle://%s:%s@ATLAS_COOLPROD" % get_authentication()

# Recycle the connection every 10 minutes
#engine = create_engine(conn_str, pool_recycle=10*60)
#metadata = MetaData(bind=engine)

engine, metadata = make_oracle_connection("oracle://ATLAS_COOLPROD"
                                          "/ATLAS_COOLONL_GLOBAL")

run_table = Table("ATLAS_RUN_NUMBER.RUNNUMBER", metadata,
    Column("NAME",          String),
    Column("RUNNUMBER",     String),
    Column("STARTAT",       String),
    Column("DURATION",      Integer),
    Column("CREATEDBY",     String),
    Column("HOST",          String),
    Column("PARTITIONNAME", String),
    Column("CONFIGSCHEMA",  Integer),
    Column("CONFIGDATA",    String),
    Column("COMMENTS",      String),
    quote=False
)
    
ONE_WEEK = 7*24*3600
    
def fetch_recent_runs(how_recent=ONE_WEEK, ascending=False):
    """
    Retrieve a list of ATLAS runs from the database, since first_run
    """
    from time import time, strftime, gmtime
    t = run_table
    
    ordering = t.c.RUNNUMBER.asc() if ascending else t.c.RUNNUMBER.desc()
    
    this_recent = strftime("%Y%m%dT%H%M%S", gmtime(time()-how_recent))
    condition = and_(t.c.STARTAT >= this_recent, t.c.PARTITIONNAME == "ATLAS")
    rows = select([run_table]).where(condition).order_by(ordering)
    return rows.execute().fetchall()

def fetch_runs_since(first_run=140000, ascending=False):
    """
    Retrieve a list of ATLAS runs from the database, since first_run
    """
    t = run_table
    
    ordering = t.c.RUNNUMBER.asc() if ascending else t.c.RUNNUMBER.desc()
    
    condition = and_(t.c.RUNNUMBER > first_run, t.c.PARTITIONNAME == "ATLAS")
    rows = select([run_table]).where(condition).order_by(ordering)
    return rows.execute().fetchall()

def make_atlas_partition_query():
    return (select([run_table.c.RUNNUMBER])
                  .where(run_table.c.PARTITIONNAME == "ATLAS")
                  .order_by(run_table.c.RUNNUMBER))

def fetch_last_n_atlas_runs(n=10):
    rows = (select([run_table.c.RUNNUMBER])
            .where(run_table.c.PARTITIONNAME == "ATLAS")
            .order_by(run_table.c.RUNNUMBER.desc()).limit(n))
    
    return [row.RUNNUMBER for row in reversed(rows.execute().fetchall())]

def fetch_atlas_runs():
    rows = make_atlas_partition_query()
    return rows.execute().fetchall()

def atlas_runs_set():
    return set(x.RUNNUMBER for x in fetch_atlas_runs())

def atlas_runs_between(first, last):

    rows = make_atlas_partition_query()
    rows = rows.where(first <= run_table.c.RUNNUMBER <= last)
    
    return [row.RUNNUMBER for row in rows.execute().fetchall()]

def filter_atlas_runs(iovs):

    iov_runs = set(iov.since.run for iov in iovs)
    first, last = min(iov_runs), max(iov_runs)
    
    rows = make_atlas_partition_query()
    rows = rows.where(first <= run_table.c.RUNNUMBER <= last)
    
    atlas_runs = set(row.RUNNUMBER for row in rows.execute().fetchall())
    keep_runs = atlas_runs.intersection(iov_runs)
    
    return IOVSet(iov for iov in iovs if iov.since.run in keep_runs)
    
