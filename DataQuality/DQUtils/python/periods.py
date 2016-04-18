#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from __future__ import with_statement

DATA_PERIODS_PATH = "/afs/cern.ch/atlas/www/GROUPS/DATAPREPARATION/DataPeriods/"

from os import listdir, makedirs
from os.path import basename, exists, join as pjoin

from .sugar import define_iov_type, IOVSet, RunLumi, RANGEIOV_VAL

@define_iov_type
def PERIODIOV_VAL(project_tag, period):
    pass

def get_periods_from_file():
    """
    Warning: Being deprecated
    """
    for period_file in listdir(DATA_PERIODS_PATH):
        if not period_file.endswith(".runs.list"):
            continue

def get_period_iovset_from_file(project_tag, period):
    runs_file = "%s.%s.runs.list" % (project_tag, period)
    with open(pjoin(DATA_PERIODS_PATH, runs_file)) as fd:
        period_data = fd.read()
        
    try:
        period_data = map(int, period_data.strip().split("\n"))
    except ValueError:
        raise RuntimeError("Invalid period data")
    
    return IOVSet.from_runs(period_data)

def get_period_iovset_from_ami(ptag, period):
    #from pyAMI.pyAMI import AMI
    #import pyAMI

    # This import needed to ensure correct python environment since pyAMI 4.0.3
    # Yuriy Ilchenko: commented out (not sure needed since ATHENA 17.X)
    #import setup_pyAMI
    import pyAMI
    from pyAMI.client import Client
    amiclient = Client('atlas')
    amiclient.config.read()

    args = ['GetRunsForDataPeriod', 
            '-projectName=%s' % ptag, 
            '-period=%s' % period]
    try:
        runs = amiclient.execute(args, format='dict_object').get_rows()
    except pyAMI.exceptions.AMI_Error:
        raise
        
    #return IOVSet.from_runs(int(x['runNumber']) for x in runs.values())
    return sorted(int(x['runNumber']) for x in runs.values())  

def fetch_project_period_runs():
    """
    Returns a dictionary {project_tag: {period: [runs]}}
    
    retrieved from the coma database.
    """

    from sqlalchemy import (select, create_engine, MetaData, Table, Column, 
        String, Integer, ForeignKey, DateTime, Float, distinct)
        
    from sqlalchemy.sql import func
    from re import compile

    LETTER_PART = compile("^(\D+)")

    from DQUtils.oracle import make_oracle_connection

    engine, metadata = make_oracle_connection("oracle://atlas_dd/atlasdd")

    schema = "ATLAS_TAGS_METADATA"

    coma_runs = Table("coma_runs", metadata,
        Column("run_index", Integer),
        Column("run_number", Integer),
        schema=schema
    )
    run_number = coma_runs.c.run_number

    coma_period_definitions = Table("coma_period_defs", metadata,
        Column("p_index",   Integer),
        Column("p_project_period", String),
        schema=schema
    )
    project_period = coma_period_definitions.c.p_project_period

    coma_period_p1_to_runs = Table("coma_period_p1_to_runs", metadata,
        Column("p2r_index", Integer),
        Column("p_index",   Integer, ForeignKey(coma_period_definitions.c.p_index)),
        Column("run_index", Integer, ForeignKey(coma_runs.c.run_index)),
        schema=schema
    )
    joined = coma_period_p1_to_runs.join(coma_runs).join(coma_period_definitions)

    smt = (select([project_period, coma_runs.c.run_number], from_obj=joined)
           .order_by(coma_runs.c.run_number))
    
    projectperiod_runs = smt.execute().fetchall()
    
    mapping = {}
    for projectperiod, run in projectperiod_runs:
        project, period = projectperiod.split(".")
        period = period.replace("period", "")
        
        project_dict = mapping.setdefault(project, {})
        period_runs = project_dict.setdefault(period, [])
        if not period_runs or period_runs[-1] != run:
            period_runs.append(run)
        
        period_letter = LETTER_PART.search(period)
        if period_letter:
            (period_letter,) = period_letter.groups()
            period_runs = project_dict.setdefault(period_letter, [])
            if not period_runs or period_runs[-1] != run:
                period_runs.append(run)
        
    return mapping
    
def split_grl(project_tag, periods, grl_file, out_directory):
    grl = IOVSet.from_grl(grl_file)
    
    period_info = fetch_project_period_runs()
    if project_tag not in period_info:
        raise RuntimeError("Unknown period {0}".format(project_tag))
    ptag_periods = period_info[project_tag]
    
    if periods is None:
        # If periods are unspecified, use all
        periods = sorted(ptag_periods.keys())
    
    name_left, _, extension = basename(grl_file).rpartition(".")
    assert name_left, "No '.' in filename?"
    
    if not exists(out_directory):
        makedirs(out_directory)
    
    for period in periods:
        iovs = grl_period_iovs = grl & IOVSet.from_runs(ptag_periods[period])
        
        output_name = "{0}_period{1}.{2}".format(name_left, period, extension)
        print "{0:4s} : {1:3} runs, {2:5} lumiblocks" .format(
            period, len(iovs.runs), iovs.lb_counts)
    
        output_path = pjoin(out_directory, output_name)
        iovs.to_grl(output_path)
