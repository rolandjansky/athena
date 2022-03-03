#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# ----------------------------------------------------------------
# Script : AtlRunQueryAMI.py
# Project: AtlRunQuery
# Purpose: Utility to retrieve information from AMI
# Authors: Andreas Hoecker (CERN), Joerg Stelzer (DESY)
# Created: Nov 10, 2009
# ----------------------------------------------------------------
#

from __future__ import print_function
import re

class ARQ_COMA:

    __store = {}
    all_periods = None

    __cursor = None
    __schema = "ATLAS_TAGS_METADATA"

    __tables = {
        'PR'  : 'COMA_V_PERIOD_RUNS',
        }

    @classmethod
    def getUsedTables(cls, output, condition):
        usedtables = set( [o.split('.')[0] for o in output] ) # all tables specified in output fields

        for c in condition:
            for p in c.split():
                if '.' in p and '\'' not in p:
                    usedtables.add(p.split('.')[0].lstrip('(')) # all tables specified in conditions

        return ["%s.%s %s" % (cls.__schema,cls.__tables[t],t) for t in usedtables] # prefix schema name to table name


    @classmethod
    def query(cls, output, condition, bindvars, verbose=False):
        query = 'select distinct %s from %s' % (', '.join(output),
                                                ', '.join(cls.getUsedTables(output, condition)))
        if condition:
            query += ' where ' + ' and '.join(condition)
        if verbose:
            print ("="*80,"\n",query)
        c = cls.cursor()
        c.execute(str(query),bindvars)
        return c.fetchall()


    @classmethod
    def cursor(cls):
        """get COMA connection
        any connection to ATLR is fine, we use Trigger DB
        """
        if not cls.__cursor:

            from CoolRunQuery.utils.AtlRunQueryTriggerUtils import interpretConnection
            connectionParameters = interpretConnection("TRIGGERDB")
            
            from cx_Oracle import connect
            connection = connect ( connectionParameters["user"],
                                   connectionParameters["passwd"],
                                   connectionParameters["server"], threaded=True)
            cls.__cursor = connection.cursor()

        return cls.__cursor





    @classmethod
    def get_periods_for_run(cls, run):
        try:
            run = int(run)
        except ValueError:
            return []
        if run not in cls.__store:
            output = ['PR.P_LEVEL', 'PR.P_PERIOD', 'PR.P_PROJECT' ]
            condition = [ "PR.RUN_INDEX = :run" ]
            bindvars = { "run": run }

            cls.__store[run] = sorted( cls.query(output, condition, bindvars) )

        return [x[1] for x in cls.__store[run]]


    @classmethod
    def get_periods(cls, year=0, level=0):
        output = ['PR.P_PERIOD', 'PR.P_PROJECT' ]
        condition = []
        bindvars = {}

        if level>0:
            condition += [ "PR.P_LEVEL=:lvl" ]
            bindvars["lvl"] = level

        if year>2000:
            project = 'data%02i%%' % (year-2000)
            condition += [ "PR.P_PROJECT like :proj" ]
            bindvars["proj"] = project

        return sorted( cls.query(output, condition, bindvars) )


    @classmethod
    def get_all_periods(cls):
        if cls.all_periods is not None:
            return cls.all_periods
        cls.all_periods = []
        p = re.compile(r"(?P<periodletter>[A-Za-z]+)(?P<periodnumber>\d+)?$")
        try:
            result = cls.get_periods(0, 0)
            for period, projectName in result:
                year = int(projectName[4:6])
                m = p.match(period)
                if not m:
                    print ("Period '%s'does not match pattern  [A-Za-z]+\\d+" % period)
                    continue

                period_letter = m.group('periodletter')
                period_number = int(m.group('periodnumber')) if m.group('periodnumber') else 0
                if len(period_letter)!=1:
                    pc = 0
                else:
                    pc = 10000*year + 100*(ord(period_letter.upper())-65) + period_number

                cls.all_periods += [ ((year, period, pc, projectName), projectName+".period" + period) ]

            cls.all_periods.sort()
        except Exception:
            import traceback
            traceback.print_exc()
            pass
        return cls.all_periods
    

    @classmethod
    def get_runs(cls, period, year):
        output = ['PR.RUN_INDEX', 'PR.P_LEVEL', 'PR.P_PERIOD', 'PR.P_PROJECT' ]

        condition = [ "PR.P_PERIOD=:period" ]
        bindvars = { "period" : period }
        if year>2000:
            project = 'data%02i%%' % (year-2000)
            condition += [ "PR.P_PROJECT like :proj" ]
            bindvars["proj"] = project
        result = cls.query(output, condition, bindvars)

        tmpstore = {}

        for record in result:
            run = record[0]
            info = record[1:4]
            if run not in tmpstore:
                tmpstore[run] = []
            tmpstore[run] += [info]

        cls.__store.update(tmpstore)
        
        return sorted( [r[0] for r in result] )



# for testing purpose
if __name__ == "__main__":

    choice = 1
    while choice != 0:
        print ("\n1 - periods for run")
        print ("2 - runs for period (and year)")
        print ("3 - periods (by year and/or level)")
        print ("4 - all periods (different format)")
        print ("\n0 - exit\n")

        choice = input("  enter your choice: ")
        choice = int(choice) if choice.isdigit() else 0
        if choice==1:
            run = int(input("  run number: "))
            print (ARQ_COMA.get_periods_for_run( run ))
        elif choice==2:
            period = input("  period           : ")
            year   = input("  year <RET> = all : ")
            year   = int(year) if year.isdigit() else 0
            print (', '.join([str(x) for x in ARQ_COMA.get_runs(period, year)]))
        elif choice==3:
            year   = input("  year <RET> = all           : ")
            year   = int(year) if year.isdigit() else 0
            level  = input("  level [1|2|3] <RET> = all : ")
            level  = int(level) if level.isdigit() else 0
            print (ARQ_COMA.get_periods(year, level))
        elif choice==4:
            print (ARQ_COMA.get_all_periods())
