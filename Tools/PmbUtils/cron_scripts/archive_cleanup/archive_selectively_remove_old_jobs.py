# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#for now this only cleans up RTT jobs. We should probably cleanup custom cron jobs as well.

import os,glob,datetime,shutil,sys

#definitions:
age_old_ndays=120
age_old_ndays_recperftests=366
base='/afs/cern.ch/atlas/project/pmb/archive/rtt'

test=True
if 'really_do_it' in sys.argv[1:]:
    test=False
quiet=False
if 'quiet' in sys.argv[1:]:
    quiet=True

pattern=os.path.join(base,'*/*/*/*/*/*/*')
for d in glob.glob(pattern):
    parts=d[len(base)+1:].split('/')
    day,month,year,rel,cmtcfg,type,package=parts
    dt=datetime.date(day=int(day),month=int(month),year=int(year))
    age=(datetime.date.today()-dt).days
    is_monday=(dt.weekday()==0)
    is_saturday=(dt.weekday()==5)
    #always keep rel1, all of dev and "recent" jobs 
    age_lim =  age_old_ndays
    if package=='RecPerfTests': age_lim=age_old_ndays_recperftests

    #Under what conditions do we keep the job?
    if is_monday:
        #always keep rel_1
        continue
    if is_saturday and rel=='dev':
        #also always keep rel_6 for dev
        continue
    if age<age_lim:
        continue
    if test:
        print 'WOULDREMOVE',d
    else:
        if not quiet:
            print "removing",d
        shutil.rmtree(d)



