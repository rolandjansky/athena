"""get_release: returns the path to a build provided that such a build
exists, and that it is recent."""

import os
import re
from time import sleep
from datetime import (datetime,
                      time,
                      timedelta)
import PathConstants

fn_re = re.compile(r'^(?P<year>20\d\d)-(?P<month>\d\d)-(?P<day>\d\d)T\d\d\d\d$')
build_day_start = 20

def fn2dt(fn): return datetime.strptime(fn, "%Y-%m-%dT%H%M")

def fn2asetup_date(fn):
    m = fn_re.match(fn)
    return 'r%s-%s-%s' % (m.group('year'), m.group('month'), m.group('day'))
             
def _get_build_in_period(branch, period_start, period_end):
    """
    period_end - end of build persiod under conssideration:
                 either build_day_start for today or for tomorrow
                 depending on the start time of the RTT.

    full dir eg 
    /cvmfs/atlas-nightlies.cern.ch/repo/sw/master/2017-03-09T2245
    """

    # find if any branchs are recent
    basedir = PathConstants.branchdir_2017
    found = [fn for fn in os.listdir(os.path.join(basedir, branch))
             if fn_re.match(fn)]

    def is_recent(fn):
        fn_dt = fn2dt(fn)
        return  fn_dt >= period_start and fn_dt < period_end 


    # print found
    recent = [f for f in found if is_recent(f)]
    if not recent:
        print 'no file available for period %s - %s in %s' % (
            str(period_start),
            str(period_end),
            os.path.join(PathConstants.branchdir_2017, branch)
        )
        return ''
    # examine build for matching cmt config paramters
    # start search form the most recent, to back in time.

    print 'recent', recent
    recent = sorted(
        recent,
        cmp=lambda x,y: cmp(fn2dt(x), fn2dt(y)),
        reverse=True)
    return recent[0]


def _get_release(release_datetime):
    """return the release (rel_X) for a datetime within the current
    build persiod. The current build period starts at the build_start_time
    of the same day if the rtt starts up at a time after the build_start_time,
    otherwise the build_start_time of the previous day"""

    
    # python calls Sunday 6, Monday 0, ATLAS: Sunday 0, Monday 1
    rel = release_datetime.weekday() + 1
    if rel == 7: rel = 0

    # but if the build starts after build_day_start, consider
    # it to be the next day

    # so, a build starting after build_day_start on sunday
    # will be assigned 6 by python, converted to 0 above.
    # further it will be reassigned to 1 as it starts after
    # build_day_start
    if release_datetime.hour >= build_day_start: rel += 1
    if rel == 7: rel = 0

    return 'rel_%d' % rel

def get_period_start(dt):
    
    result = dt.replace(
        hour=build_day_start,
        minute=0,
        second=0,
        microsecond=0)

    if dt.hour < build_day_start:
        result = result - timedelta(days=1)

    return result


def get_release(branch, cmtconfig, check_datetime=None):
    """Return a release (string matching rel_[0,6]) if a build occurs
    during a period of interest, '' otherwise.
    If the RTT check time is after build_day_start, the period of
    interest starts at the same day at build_day_start, otherwise
    at build_day_start of the previous day. It terminates 24 hours later.

    If no such build exists, the function waits for it to arrive, or
    the current time exits the persiod of interest.
    """
    if check_datetime is None: check_datetime = datetime.now()
    
    period_start = get_period_start(check_datetime)
    print period_start
    period_end = period_start + timedelta(days=1)

    while True:
        fn  = _get_build_in_period(branch,
                                   period_start,
                                   period_end)
        if not fn:
            print 'get_release_2017 no file available, sleeping'
            sleep(60*5) # no file available, sleep 5 mins
        else:
            return (fn, _get_release(fn2dt(fn)), fn2asetup_date(fn))
    
if __name__ == '__main__':
    import sys
    branch = sys.argv[1]
    cmtconfig = sys.argv[2]

    # eg python get_release_2017.py master x86_64-slc6-gcc49-opt



    print 'calling _get_build_in_period', branch, cmtconfig
    period_start = get_period_start(datetime.now())
    period_end = period_start + timedelta(days=1)
    
    fn = _get_build_in_period(branch, period_start, period_end)
    print 'release file: -->%s<--' % fn

    # the call to get release waits for a suitable build to appear,
    # so may hang.
    # fn, rel = get_release(datetime.now(), branch, cmtconfig)
    # print 'ran get_release', branch, cmtconfig
    # print fn, rel

    print 'calling _get_release'

    early = build_day_start - 1
    late = build_day_start + 1

    assert early in range(24)
    assert late in range(24)
    
    sunday_early = datetime(year=2017,
                              month=3,
                              day=12,
                              hour=early)

    sunday_late = sunday_early.replace(hour=late)

    assert _get_release(sunday_early) == 'rel_0'
    assert _get_release(sunday_late) == 'rel_1'


    print '\ntemp debug\n'
    now = datetime.now()
    now_1 = now - timedelta(days=1)
    now_2 = now - timedelta(days=2)
    now_3 = now - timedelta(days=3)

    conf = 'x86_64-slc6-gcc62-opt'
    branch = 'master'

    print branch, conf
    print get_release(branch, conf)
    print get_release(branch, conf, now)
    print get_release(branch, conf, now_1)
    print get_release(branch, conf, now_2)
    print get_release(branch, conf, now_3)


    branch = '21.0'
    print 
    print branch, conf
    print
    print get_release(branch, conf)
    print get_release(branch, conf, now)
    print get_release(branch, conf, now_1)
    print get_release(branch, conf, now_2)
    print get_release(branch, conf, now_3)

    print 'ok'

    
    
