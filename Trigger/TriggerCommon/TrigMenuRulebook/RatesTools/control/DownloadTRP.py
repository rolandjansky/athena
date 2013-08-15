# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from model.basics.TRPFile import TRPFile


import os
user = os.environ["USER"]

amiclient = None

def ami_init():
    global amiclient
    if amiclient == None:
        from pyAMI.pyAMI import AMI
        amiclient = AMI(False)
        
def get_runs(year, period):
    ami_init()
    global amiclient
    
    cmd = ['GetRunsForDataPeriod', '-period=%s' % period]
    if year>2000:
        cmd += [ '-projectName=data%02i%%' % (year-2000) ]
    result = amiclient.execute(cmd)
    return sorted([ int(e['runNumber']) for e in result.getDict()['Element_Info'].values() ])


def get_periods(year):
    ami_init()
    global amiclient
    
    cmd = ['ListDataPeriods', '-createdSince=2009-01-01 00:00:00' ]
    if year>2000:
        cmd += [ '-projectName=data%02i%%' % (year-2000) ]
        
    result = amiclient.execute(cmd)
    return sorted( [ str(e['period']) for e in result.getDict()['Element_Info'].values() ])


def download_trp(year, period):    
    failed = []
    for i in get_runs(year, period):
        try:
            print i,
            trp = TRPFile(i)
            if not trp.is_local():
                print "remote" 
                trp.make_local()
            else:
                print "local"
        except:
            failed.append(i)
            
    if len(failed):
        print "Failed to load the following runs from period %s, %i" % (period, year)
        
    return failed



if __name__ == "__main__":
    import subprocess 
    
    year = 2011
    period = "L"    
    
    print year, period
    download_trp(year, period)    
    for runNumber in get_runs(year, period):
        cmd = "cp -u -f %s /tmp/%s/to_p1/" % (TRPFile(runNumber).get_local_path(), user)
        subprocess.call(cmd, shell = True)
    
    """
    for year in [2011, 2010, 2009]:
        periods = []
        for period in sorted(get_periods(year), reverse=True): 
            if len(period) == 1: periods.append(period)
        
        for period in periods: 
            print "[+]", year, period
            download_trp(year, period)
            
            for runNumber in get_runs(year, period):
                cmd = "mv %s /tmp/ggalster/to_p1/" % TRPFile(runNumber).get_local_path()
                subprocess.call(cmd, shell = True)            

    """
