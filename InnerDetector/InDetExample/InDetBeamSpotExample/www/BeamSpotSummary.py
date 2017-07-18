# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS beam spot web page for displaying beam spot job data.
"""

__author__  = 'Juerg Beringer'
__version__ = '$Id $'

from BeamSpotWebPage import BeamSpotWebPage
from InDetBeamSpotExample.TaskManager import *
from InDetBeamSpotExample.Utils import blankIfNone
import time
import glob

# TODO: default sorting
tableSorter = """\
<script type="text/javascript" src="../js/jquery-latest.js"></script> 
<script type="text/javascript" src="../js/jquery.tablesorter.js"></script>
<script type="text/javascript" id="js">
$(document).ready(function() {
	$("table").tablesorter({
                headers: { 3: { sorter: "shortDate"  } },
		sortList: [[0,1]]
	});
});
</script>
"""

tableHeader = """<table cellspacing="0" class="tablesorter">
<thead>
<tr>
<th>Run</th>
<th>Stream</th>
<th>Latest %s Task</th>
<th>Updated</th>
<th>Status</th>
<th>Results</th>
<th>Data in COOL</th>
<th>Validation<br>Job Status</th>
<th>Validation<br>Job Results</th>
<th>Links</th>
</tr>
</thead>
<tbody>
"""

runCount = """\
<div class="text">
<h3>%s runs(s) with tasks of type %s:</h3>
</div>
"""

runCountLimit = """\
<div class="text">
<h3>%s runs(s) with tasks of type %s, displaying last %i runs:</h3>
</div>
"""


class BeamSpotSummary(BeamSpotWebPage):

    def __init__(self):
        BeamSpotWebPage.__init__(self)
        self.pageConfig['pageTitle'] = 'ATLAS Beam Spot Summary'
        self.addToPageHeader(tableSorter)

    def content(self,**args):
        if 'type' in args:
            type = args['type']
        else:
            type = ''
        limit = int(args['limit']) if 'limit' in args else 50
        with TaskManager(self.globalConfig['taskDb']) as taskman:
            if not limit:
                limit = 99999999
                table = runCount % (taskman.getCount('distinct(runnr)',[ "where TASKNAME like '%s%%'" % type]), type)
            else:
                table = runCountLimit % (taskman.getCount('distinct(runnr)',[ "where TASKNAME like '%s%%'" % type]), type,limit)
            table += tableHeader % type
            for r in taskman.taskIterDict('distinct(DSNAME),RUNNR',["where TASKNAME like '%s%%' order by RUNNR desc" % type], limit):
                runnr = r['RUNNR']
                dsname = r['DSNAME']
                try:
                    stream = dsname.split('.')[-1].split('_')[-1]
                except:
                    stream = ''
                if not runnr: continue
                table += "<tr>"
                table += '<td><a href="http://atlas-runquery.cern.ch/query.py?q=find+r+%s+/+sh+lhc+all+and+r+and+t+and+mag+and+dq+idbs,pix,sct">%s</a></td>' % (runnr,runnr)
                table += '<td>%s</td>' % stream
                try:
                    t = taskman.taskIterDict('*',['where RUNNR =',DbParam(runnr),'and DSNAME =',DbParam(dsname),"and TASKNAME like '%s%%' order by UPDATED desc" % type]).next()
                    taskName = t['TASKNAME']
                    datatag = taskName.split('.')[-1].split('_')[0]
                    if taskName[:11] == 'DB_BEAMSPOT.':
                        # Special naming convention for T0 beam spot jobs
                        monTaskName = 'MON.%s.%s' % (taskName,datatag)
                    elif taskName[:-1] == 'REPROHIMAR2011_BEAMSPOT.r2074.v':
                        # Kludge to fix inconsistent naming for Mar 2011 HI reprocessing jobs
                        monTaskName = 'MON.REPROHIMAR2011_BEAMSPOT.r2074'
                    else:
                        # Other monitoring jobs
                        monTaskName = 'MON.%s%%' % (taskName)
                    try:
                        m = taskman.taskIterDict('*',['where RUNNR =',DbParam(runnr),'and DSNAME =',DbParam(dsname),'and TASKNAME like ',DbParam(monTaskName),'order by UPDATED desc']).next()
                        stat = m['STATUS']
                        monStatus = '<td class="%s"><a href="../details?d=%s&t=%s">%s</a></td>' % (getStatusClass(stat),t['DSNAME'],m['TASKNAME'],getKey(TaskManager.StatusCodes,stat))
                        monResults = "<td>%s</td>" % (blankIfNone(m['RESULTLINKS']))
                    except:
                        monStatus = '<td></td>'
                        monResults = '<td></td>'
                    table += '<td><a href="../details?d=%s&t=%s">%s</a></td>' % (t['DSNAME'],taskName,taskName)
                    table += "<td>%s</td>" % (time.ctime(t['UPDATED']))
                    stat = t['STATUS']
                    table += '<td class="%s">%s</td>' % (getStatusClass(stat),getKey(TaskManager.StatusCodes,stat))
                    table += "<td>%s</td>" % (blankIfNone(t['RESULTLINKS']))
                    table += "<td>"
                    cooltags = t['COOLTAGS']
                    if not cooltags:
                        cooltags = ''   # make sure it is not None
                    for tag in cooltags.split():
                        table += '<a href="http://atlas-runquery.cern.ch/query.py?q=find+run+%s+/+show+bs+%s">%s</a> ' % (runnr,tag,tag)
                        table += '<br>'
                    table += "</td>"
                except Exception, e:
                    table += "<td>%s</td>" % str(e)
                    table += "<td></td>"
                    table += "<td></td>"
                    table += "<td></td>"
                    table += "<td></td>"
                    monStatus = '<td></td>'
                    monResults = '<td></td>'
                table += monStatus
                table += monResults
                table += '<td><a href="../jobs?r=%s">all tasks</a></td>' % runnr
                table += "</tr>\n"
        table += "</tbody></table>\n"
        return table


# Code to test or run locally
if __name__ == '__main__':
    p = BeamSpotSummary()
    print p.index()
