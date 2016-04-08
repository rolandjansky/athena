# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import time, xml.dom.minidom, os.path, getopt, sys
sys.path.append('/afs/cern.ch/atlas/project/RTT/prod/Tools/RunTimeTester/src')
from Evaluate    import Evaluate
from exc2string2 import exc2string2
from makeRTTDOM  import makeRTTDOM


# ----- global stuff -----

daysecs = 24*3600
# value of each entry is the date of the first run (DD/MM/YYYY)

runIdentifiers = [('i686-slc4-gcc34-opt', 'bugfix'),
                  ('i686-slc4-gcc34-opt', 'dev'),
                  ('i686-slc4-gcc34-opt', 'val'),
                  ('i686-slc4-gcc34-dbg', 'bugfix'),
                  ('i686-slc3-gcc323-opt', 'pcache'),
                  ('i686-slc3-gcc323-opt', 'bugfix'),
                  ]

runTypeIDtags = ['originalBranch', 'targetCMTCONFIG']

numberOfDaysToGoBackInTime = 6

prodResultsBase = '/afs/cern.ch/atlas/project/RTT/Results'

# --------------------------------------------------------------

def getReports(fn):
    sortedReports = {}
    reportsDoc = None
    reports = []

    if os.path.exists(fn):
        reportsDoc = xml.dom.minidom.parse(fn)
        reports = Evaluate('report', reportsDoc.documentElement)
    else:
        print 'no reports file', fn
        return sortedReports

    for r in reports:
        
        rtime     = Evaluate('startTime1/text()', r).strip()
        platform  = Evaluate('CMTconfig/text()', r).strip()
        branch    = Evaluate('branch/text()', r).strip()
        dayOfWeek = Evaluate('humanDate/dayOfWeek/text()', r).strip()
        date      = Evaluate('humanDate/date/text()', r).strip()
        month     = Evaluate('humanDate/month/text()', r).strip()
        year      = Evaluate('humanDate/year/text()', r).strip()

            
        if date and int(date) < 10 and len(date)==1:
            date = '0' + date
        day = '%s %s %s %s' % (dayOfWeek, date, month, year)
        sortedReports[(branch, platform, day, rtime)] = r

    return sortedReports

# --------------------------------------------------------------

def getEarliestTime():
    startsecs = time.time()-numberOfDaysToGoBackInTime*daysecs
    
    start = time.strftime('%y:%m:%d', time.localtime(startsecs))
    start = time.strptime(start+':00:00:00', '%y:%m:%d:%H:%M:%S')
    startsecs = time.mktime(start)
    return startsecs

# --------------------------------------------------------------

def makeByDayTable():
    '''Return an empty grid of all possible runs. Later we fill
    in the runs that did occur.'''
    def includeEntry(loopDate, entryDate):
        eday,emonth,eyear = entryDate.split('/')
        dayName, lday, lmonth, lyear = loopDate.split()
        lmonth = {'January':1,'February':2,'March':3,'April':4,
                  'May':5,'June':6,'July':7,'August':8,
                  'September':9,'October':10,'November':11,'December':12}.get(lmonth)
        if int(eyear) < int(lyear): return False
        if int(eyear) > int(lyear): return True
        if int(emonth) < int(lmonth): return False
        if int(emonth) > int(lmonth): return True
        if int(eday) < int(lday): return False
        if int(eday) >= int(lday): return True
        
    startsecs = getEarliestTime()
    
    startL = [startsecs+d*daysecs for d in range(numberOfDaysToGoBackInTime+1)]
    startD = [time.strftime('%A %d %B %Y', time.localtime(s)) for s in startL]

    byDay = {}
    for d in startD:
        byDay[d] = {}
        for r in runIdentifiers:
            byDay[d][r] = []

    return (startD, byDay)

# --------------------------------------------------------------

def sortOverViewNodesByDay(ovNodes):
    # runsByDay is a dictionary of dictionaries.
    orderedKeys, runsByDay = makeByDayTable()

    for o in ovNodes:
        stime  = float(Evaluate('startTime2/text()', o))

        branch = Evaluate(runTypeIDtags[0] + '/text()', o).strip()
        config = Evaluate(runTypeIDtags[1] + '/text()', o).strip()
        
        day = time.strftime('%A %d %B %Y', time.localtime(stime))
        if (config, branch) in  runsByDay[day]:
            runsByDay[day][(config,branch)].append(o)
    return orderedKeys, runsByDay

# --------------------------------------------------------------

def getOverviewNodes(fn, weekDom):
    fns = open(fn, 'r').readlines()
    fns = [f.strip() for f in fns]
    fns = [f for f in fns if os.path.exists(f)]
    overviewNodes = []

    for f in fns:
        try:
            d = xml.dom.minidom.parse(f)
        except:
            print 'error parsing ', f
            print exc2string2()
            continue # ignore unparseable files

        on = Evaluate('overview', d.documentElement)[0]
        domStart = float(Evaluate('startTime2/text()', on))

        earliestTime = getEarliestTime()
        if domStart > earliestTime:    
            overviewNodes.append(weekDom.importNode(on, True))
            
        d.unlink()
    return overviewNodes

# --------------------------------------------------------------    

def getStartTimeFromOverViewNode(on):
    return Evaluate('startTime1/text()', on)

# --------------------------------------------------------------

def sortByStartDate(overV1, overV2):
    startTime1 = getStartTimeFromOverViewNode(overV1)
    startTime2 = getStartTimeFromOverViewNode(overV2)
    if startTime1 < startTime2: return -1
    if startTime1 > startTime2: return 1
    return 0

# --------------------------------------------------------------

def makeWeeklyViewDom(overviewNodes, reportTimes, weekDom):
    dtags  = ['release', 'startTime1', 'startTime2', 'endTime1','statusText','topProject','nPackages', 'nPackagesAllJobsOK']

    weekDom = makeRTTDOM('weeklyReport')

    orderedDays, runsByDay = sortOverViewNodesByDay(overviewNodes)
    orderedDays.reverse()

    # day is of format: Tuesday 05 May 2007
    for day in orderedDays:

        el = weekDom.createElement('day')
        el2 = weekDom.createElement('name')
        el2.appendChild(weekDom.createTextNode(day))
        el.appendChild(el2)

        runsEl = weekDom.createElement('runs')
        
        for runID in runsByDay[day]:
            # is there an actual run for this platform/branch ?
            on = runsByDay[day][runID]
            
            if not on:
                runEl = weekDom.createElement('run')
                
                config = runID[0]
                branch = runID[1]
                branchEl  = weekDom.createElement('originalBranch')
                branchEl.appendChild(weekDom.createTextNode(branch))
                
                configEl  = weekDom.createElement('targetCMTCONFIG')
                configEl.appendChild(weekDom.createTextNode(config))
                
                # now append these two to the <run> parent element
                runEl.appendChild(branchEl)
                runEl.appendChild(configEl)

                # is there a report for this?
                startTime = '---'
                possibleReportKey = (branch, config, day, startTime)
                r = reportTimes.get(possibleReportKey, None)
                if r: runEl.appendChild(r)

                runsEl.appendChild(runEl)
                continue


            for o in on:
                runEl = weekDom.createElement('run')

                config = runID[0]
                branch = runID[1]
                branchEl  = weekDom.createElement('originalBranch')
                branchEl.appendChild(weekDom.createTextNode(branch))
                
                configEl  = weekDom.createElement('targetCMTCONFIG')
                configEl.appendChild(weekDom.createTextNode(config))
                
                # now append these two to the <run> parent element
                runEl.appendChild(branchEl)
                runEl.appendChild(configEl)
                
                startTime = Evaluate('startTime1/text()', o)
                startTime = startTime.strip()
            
                nodes = []
                for t in dtags:
                    e = Evaluate(t, o)
                    if e:
                        e = e[0]
                        nodes.append(e)
                nodes = [n.cloneNode(n) for n in nodes] # allow the on to be unlinked
            
                o.unlink()
                
                # add a report if there is one
                possibleReportKey = (branch, config, day, startTime)
                report = reportTimes.get(possibleReportKey, None)
                if report: nodes.append(report)
            
                # append all these extra nodes to the <run> parent element
                [runEl.appendChild(n) for n in nodes]
        
                runsEl.appendChild(runEl)

        el.appendChild(runsEl)
        weekDom.documentElement.appendChild(el)

    return weekDom

# --------------------------------------------------------------

def display(weeklyDom, longReport):
    orderedKeys, runsByDay = ns(weeklyDom)

    tags  = ['releaseName', 'targetCMTCONFIG', 'startTime1', 'endTime1','statusText']
    rtags = ['reportPerson', 'afsReport', 'buildReport','runStatus', 'clusterStatus', 'commentReport']

    format = '%14s %20s %14s %14s %s'
    for d in orderedKeys:
        print '\n------------%s-------------\n\n' % d
        rpd = runsByDay[d]
        if rpd:
            print format %  tuple(tags)
            print
        for r in rpd:
            vals = []
            for t in tags:
                val = Evaluate(t+'/text()', r)
                vals.append(val.strip())
            print format % tuple(vals)

            if longReport:
                reports = Evaluate('report', r)
                if reports:
                    report = reports[0]
                    print
                    for t in rtags: 
                        val = Evaluate(t+'/text()', report)
                        val = val.strip()
                        print '   %20s  %s' % (t, val)

# --------------------------------------------------------------

def doit(longReport, writeOut, disp):
    fn = '/afs/cern.ch/atlas/project/RTT/prod/webPageCollector/src/summaryFilePaths.txt'

    weeklyViewDom = makeRTTDOM('weeklyReport')

    overviewNodes = getOverviewNodes(fn, weeklyViewDom)
    # print 'found %d doms' % len(doms)

    fn = 'RTTreports.xml'
    sortedReports = getReports(fn)
    
    weeklyViewDom = makeWeeklyViewDom(overviewNodes, sortedReports, weeklyViewDom)
    # print weeklyViewDom.toxml()        

    # print weekDom.toprettyxml()
    # if disp: display(weeklyViewDom, longReport)

    if writeOut: open('weekly.xml', 'w').write(weeklyViewDom.toxml())
    print 'Done.'

# --------------------------------------------------------------    

def usage():
    print
    print 'dailyRTT -- displays summaries of all RTT runs for the last week and optionally the checker reports'
    print
    print 'usage: python dailyRTT [options]'
    print
    print 'Options to *turn off* features'
    print '   -w do not write out summary file'
    print '   -l do not display checker reports  (condensed output)'
    print '   -d do not display on standard out'
    print '   -h print this message and exit'
    print
    print 'The script should be run in a dictory containg the files summaryFilePaths.txt (a list of paths to'
    print 'the RTT summary files, typically created y webpagecollector) and RTTreports.xml, a file containing'
    print ' the checkers reports'
    
    return

# --------------------------------------------------------------

if __name__ == '__main__':
    
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'wldh')
        print 'opts',opts
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    disp     = True
    longDisp = True
    writeOut = True

    for o, a in opts:
        if o == '-l': longDisp = False
        if o == '-w': writeOut = False
        if o == '-d': disp     = False
        if o == '-h':
            usage()
            sys.exit(0)


    doit(longDisp, writeOut, disp)
