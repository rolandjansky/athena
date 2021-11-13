#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""
Periods are assigned an ordinate by which they can be sorted
"""

from functools import cache
import re,sys

pat_last   = re.compile(r"(?:l|la|las|last) (\d*)$")  # l(ast) NN runs
pat_number = re.compile(r"\d{5,8}[+-]?$")  # run number (5-8 digits), possibly followed by a + or -
pat_range  = re.compile(r"\d{5,8}-\d{5,8}$")  # range of run numbers (each 5-8 digits)
pat_short  = re.compile(r"(?:(?:\d{2})(\d{2})\.)?([a-zA-Z]+\d*)$")
pshort     = re.compile(r"(?P<first>(data|20)?(?P<year>\d{2})(_.*)?\.)?(period)?(?P<period>[a-zA-Z])(?P<subperiod>\d+)?$",re.I) # 2015_periodA5




def getCurrentYear():
    """
    returns the last two digits of current year 
    """
    from time import gmtime
    return gmtime().tm_year - 2000



def getRunsFromPeriods(list_of_periods):
    """
    find the run numbers for given period(s)
    
    list_of_periods   periods in format  [('10','B1'),('11','A'),...]
    """
    runlist = []
    from CoolRunQuery.AtlRunQueryCOMA import ARQ_COMA
    for year, period, fname in list_of_periods:
        runlist += ARQ_COMA.get_runs(period, 2000+int(year))
    return runlist


@cache
def getSortedAvailablePeriods():
    from CoolRunQuery.AtlRunQueryCOMA import ARQ_COMA
    available_periods = ARQ_COMA.get_all_periods()
    available_periods.sort()
    #print ('\nAvailable periods:\n')
    #for i,p in enumerate(available_periods):
    #    print (p)
    return available_periods


def getListOfPeriodsFromOrdinateRange(begin, end, requiredProjectName, specialData=None ):
    """
    requiredProjectName e.g. data15_13TeV, data15_cos, data15_hip, etc. . If None then it is required that the project name ends in TeV
    """
    if begin>end:
        sys.exit(0)
    list_of_periods = []

    for p,full_name in getSortedAvailablePeriods():
        (year, period, ordcode, projname) = p

        if requiredProjectName is None:
            if not projname.endswith("TeV"):
                continue
        else:
            if projname != requiredProjectName:
                continue
        
        if ordcode == 0: # no special VdM or AllYear stuff periods
            if specialData is not None:
                (vdmyear, vdmperiod, vdmsubperiod) = specialData
                if year==vdmyear and (vdmperiod+vdmsubperiod)==period:
                    list_of_periods += [ (year, period, full_name) ]
                    print (p,"--> include")
            continue

        if ordcode % 100 == 0:  # no full periods as they are already listed in the numbered periods
            continue

        if ordcode < begin or ordcode>end:
            continue

        list_of_periods += [ (year, period, full_name) ]
        print (p,"--> include")
    return list_of_periods



def getDataPeriodsWithinRange( period_range ):

    m1 = pshort.match(period_range[0])
    m2 = pshort.match(period_range[1])

    if m1 is None or m2 is None:
        if m1 is None:
            print ("Invalid specification of begin of range",period_range[0])
        if m2 is None:
            print ("Invalid specification of end of range",  period_range[1])
        sys.exit(0)

    m1 = m1.groupdict()
    m2 = m2.groupdict()

    # year
    m1['year'] = int(m1['year']) if m1['year'] else getCurrentYear()
    m2['year'] = int(m2['year']) if m2['year'] else m1['year']
    # sub-period
    m1['subperiod'] = int(m1['subperiod']) if m1['subperiod'] else 0
    m2['subperiod'] = int(m2['subperiod']) if m2['subperiod'] else 99

    #print ("Interpret run range: %r - %r" % (m1,m2))

    # ordinate
    p1c = 10000*m1['year'] + 100*(ord(m1['period'].upper())-65) + m1['subperiod']
    p2c = 10000*m2['year'] + 100*(ord(m2['period'].upper())-65) + m2['subperiod']

    return getListOfPeriodsFromOrdinateRange( p1c, p2c, None )



def GetRuns( arg ):
    """
    arg:     'run data10_7TeV.periodA' or 'run periodA' (where 'data11_7TeV' is assumed)
    #               or 'data10_7TeV.periodA-periodC' or 'data10_7TeV.periodA,data10_7TeV.periodB,...'
    # This is case sensitive !!
    """

    # final result in here
    list_of_runs = []

    # are their any commas?        
    for tag in arg.split(','):

        # last X runs pattern
        m = pat_last.match(arg)
        if m:
            #print ("Pattern 'Last N'")
            list_of_runs += [ "last%s" % m.group(1) ]
            continue

        # run numbers
        if pat_number.match(tag):
            #print ("Pattern 'List of runs'")
            list_of_runs += [tag]
            continue

        # run number range
        if pat_range.match(tag):
            #print ("Pattern 'Range of runs'")
            list_of_runs += [tag]
            continue


        if '-' in tag: # range
            #print ("Pattern 'Range of periods'")
            list_of_periods = getDataPeriodsWithinRange( tag.split('-') )
            list_of_runs += getRunsFromPeriods(list_of_periods)
            continue


        # various AllYear versions
        if '.' not in tag or tag.endswith(".All") or tag.endswith(".AllYear") or tag.endswith(".periodAllYear"):
            #print ("Pattern 'AllYear'")
            allyear = 0
            projectName = None
            # no tag means AllYear
            m = re.match(r"20(?P<year>\d{2})(.All|.AllYear|.periodAllYear)?$", tag, re.I)
            if m:
                allyear = int(m.groupdict()['year'])
            else:
                m = re.match(r"(?P<proj>data(?P<year>\d{2})_.*?)(.All|.AllYear|.periodAllYear)?$", tag, re.I)
                if m:
                    allyear = int(m.groupdict()['year'])
                    projectName = m.groupdict()['proj']

            if allyear==0:
                raise RuntimeError("Can't interpret run period %s" % tag)

            print ("Interpret period: AllYear for %i" % (2000+allyear))

            # ordinate
            p1c = 10000*allyear
            p2c = 10000*(allyear+1) - 1

            list_of_periods = getListOfPeriodsFromOrdinateRange(p1c, p2c, projectName)

            print (list_of_periods)

            list_of_runs += getRunsFromPeriods(list_of_periods)
            continue



        m = re.match( r"(?P<first>(data|20)?(?P<year>\d{2})(_.*)?\.)?(period)?(?P<period>[a-zA-Z])(?P<subperiod>\d+)?$", tag, re.I )

        #projPeriod = re.compile(r"(?P<first>(data|20)?(?P<year>\d{2})(_.*)?\.)?(period)?(?P<period>[a-zA-Z])(?P<subperiod>\d+)?$",re.I) # 2015_periodA5


        d = { 'projname' : None }
        m = re.match( r"20(?P<year>\d{2})\.(period)?(?P<period>[a-zA-Z]|VdM)(?P<subperiod>\d+)?$", tag, re.I )
        if m:
            #print ("Pattern '2015.(period)A1'")
            d.update( m.groupdict() )
        else:
            m = re.match( r"(?P<projname>data(?P<year>\d{2})_.*?)\.(period)?(?P<period>[a-zA-Z]|VdM)(?P<subperiod>\d+)?$", tag, re.I )
            if m:
                #print ("Pattern 'dataYY_TTT.(period)A1'")
                d.update( m.groupdict() )
            else:
                d = None

        if d:
            d['subperiod'] = int(d['subperiod']) if d['subperiod'] else 0

            print ("Interpret period: %r" % d)
            if len(d['period'])==1:
                # ordinate
                p1c = 10000*int(d['year']) + 100*(ord(d['period'].upper())-65) + d['subperiod']
                if d['subperiod'] != 0:
                    p2c = p1c
                else:
                    p2c = p1c+99
                list_of_periods = getListOfPeriodsFromOrdinateRange(p1c, p2c, d['projname'])
            else:
                # VdM
                sp = '' if d['subperiod']==0 else str(d['subperiod'])
                list_of_periods = getListOfPeriodsFromOrdinateRange( 0, 0, d['projname'], (int(d['year']), d['period'],sp) )
                
            list_of_runs += getRunsFromPeriods(list_of_periods)
            continue

    if len(list_of_runs)==0:
        print ("No runs matching pattern")

    return list_of_runs

        



# command line driver for convenience
if __name__=='__main__':


    if len(sys.argv) <= 1:
        print ('No data period argument given, try\n%s data15_13TeV.A')
        sys.exit(1)


    print (GetRuns( sys.argv[1] ))

