# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS beam spot web page for displaying beam spot job data.
"""

__author__  = 'Juerg Beringer'
__version__ = 'JobBrowser.py atlas/athena'

from BeamSpotWebPage import BeamSpotWebPage
from InDetBeamSpotExample.TaskManager import *
from InDetBeamSpotExample.Utils import blankIfNone
import time
import glob

tableSorter = """\
<script type="text/javascript" src="../js/jquery-latest.js"></script> 
<script type="text/javascript" src="../js/jquery.tablesorter.js"></script>
<script type="text/javascript" id="js">
$(document).ready(function() {
	$("table").tablesorter({
                headers: { 0: { sorter: "shortDate"  } },
		sortList: [[0,1]]
	});
});
</script>
"""

tableHeader = """<table cellspacing="0" class="tablesorter">
<thead>
<tr>
<th>Last update</th>
<th>Dataset</th>
<th>Task</th>
<th>Release</th>
<th>#Jobs</th>
<th>Status</th>
<th>Details</th>
<th>Results</th>
</tr>
</thead>
<tbody>
"""

taskCount = """\
<div class="text">
<h3>%s task(s):</h3>
</div>
"""

taskCountLimit = """\
<div class="text">
<h3>%s task(s), displaying last %i tasks:</h3>
</div>
"""

taskCountForDS = """\
<div class="text">
<h3>%s task(s) for dataset %s:</h3>
</div>
"""

taskCountForRun = """\
<div class="text">
<h3>%s task(s) for run %s:</h3>
</div>
"""


class JobBrowser(BeamSpotWebPage):

    def __init__(self):
        BeamSpotWebPage.__init__(self)
        self.pageConfig['pageTitle'] = 'ATLAS Beam Spot Jobs'
        self.addToPageHeader(tableSorter)

    def content(self,**args):
        with TaskManager(self.globalConfig['taskDb']) as taskman:
            if 'r' in args:
                table = taskCountForRun % (taskman.getNTasks(['where RUNNR =',DbParam(args['r'])]), args['r'])
                taskIter = taskman.taskIterDict('*', ['where RUNNR =',DbParam(args['r']),'order by UPDATED'] )
            elif 'd' in args:
                table = taskCountForDS % (taskman.getNTasks(["where DSNAME like '%%%s%%'" % args['d']]), args['d'])
                taskIter = taskman.taskIterDict('*', ["where DSNAME like '%%%s%%' order by UPDATED" % args['d']] )
            else:
                limit = int(args['limit']) if 'limit' in args else 50
                if not limit:
                    limit = 99999999
                    table = taskCount % taskman.getNTasks()
                    taskIter = taskman.taskIterDict(qual=('order by UPDATED desc',))
                else:
                    table = taskCountLimit % (taskman.getNTasks(),limit)
                    taskIter = taskman.taskIterDict(qual=('order by UPDATED desc',),limit=limit)
        table += tableHeader
        for t in taskIter:
            dsname = t['DSNAME']
            taskname = t['TASKNAME']
            table += "<tr>"
            table += "<td>%s</td>" % (time.ctime(t['UPDATED']))
            table += "<td>%s</td>" % (dsname)
            table += "<td>%s</td>" % (taskname)
            table += "<td>%s</td>" % ('<br>'.join(t['ATLREL'].split(';')))
            table += "<td>%s</td>" % (t['NJOBS'])
            stat = t['STATUS']
            table += '<td class="%s">%s</td>' % (getStatusClass(stat),getKey(TaskManager.StatusCodes,stat))
            table += '<td>'
            table += ' <a href="../details?d=%s&t=%s">Details</a>' %(dsname,taskname)
            table += ' <a href="../files?u=%s/%s">Files</a>' %(dsname,taskname)
            table += '</td>'
            table += "<td>%s</td>" % (blankIfNone(t['RESULTLINKS']))
            table += "</tr>\n"
        table += "</tbody></table>\n"
        return table


# Code to test or run locally
if __name__ == '__main__':
    p = JobBrowser()
    print p.index()
