# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS beam spot web page for displaying details of beam spot jobs.
"""

__author__  = 'Juerg Beringer'
__version__ = 'JobDetails.py atlas/athena'

from BeamSpotWebPage import BeamSpotWebPage
from InDetBeamSpotExample.TaskManager import *
from InDetBeamSpotExample.Utils import fileListSnippet
from InDetBeamSpotExample.Utils import blankIfNone
import time

error = """\
<div class="errormsg">
%s
</div>
"""

tableRow = '<tr><td class="header">%s</td><td>%s</td></tr>\n'
tableRowStatus = '<tr><td class="header">%s</td><td class="%s">%s</td></tr>\n'

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
<th>Job</th>
<th>Status</th>
<th>Exit code</th>
<th>Files</th>
</tr>
</thead>
<tbody>
"""


class JobDetails(BeamSpotWebPage):

    def __init__(self):
        BeamSpotWebPage.__init__(self)
        self.pageConfig['pageTitle'] = 'ATLAS Beam Spot Job Details'
        self.addToPageHeader(tableSorter)

    def content(self,**args):
        if not 'd' in args:
            return error % ('CONFIGURATION ERROR: No data set name specified')
        if not 't' in args:
            return error % ('CONFIGURATION ERROR: No task name specified')
        with TaskManager(self.globalConfig['taskDb']) as taskman:
            t = taskman.getTaskDict(args['d'],args['t'])
        if not t:
            return error % ('ERROR: Task %s/%s not found' % (args['d'],args['t']))

        p = '<div class="text"><h3>Summary for task %s/%s (task id %s):</h3></div>\n' % (args['d'],args['t'],t['TASKID'])
        p += '<table>\n'
        p += tableRow % ('Dataset name',t['DSNAME'])
        p += tableRow % ('Run number',blankIfNone(t['RUNNR']))
        p += tableRow % ('Task name',t['TASKNAME'])
        stat = t['STATUS']
        p += tableRowStatus % ('Task status',getStatusClass(stat),getKey(TaskManager.StatusCodes,stat))
        p += tableRow % ('On-disk status',getKey(TaskManager.OnDiskCodes,t['ONDISK']))
        p += tableRow % ('Template',t['TEMPLATE'])
        p += tableRow % ('Postprocessing steps',t['TASKPOSTPROCSTEPS'])
        p += tableRow % ('ATLAS release',t['ATLREL'])
        p += tableRow % ('# result files',t['NRESULTFILES'])
        p += tableRow % ('Result files',fileListSnippet(t['RESULTFILES'],t['DSNAME'],t['TASKNAME'],self.globalConfig['jobDir']))
        p += tableRow % ('Result links',t['RESULTLINKS'])
        cooltags = t['COOLTAGS'] or ''
        p += tableRow % ('COOL tags','<br>'.join(cooltags.split()))
        p += tableRow % ('# jobs',t['NJOBS'])
        p += tableRow % ('# jobs submitted',t['NJOBS_SUBMITTED'])
        p += tableRow % ('# jobs running',t['NJOBS_RUNNING'])
        p += tableRow % ('# jobs postprocessing',t['NJOBS_POSTPROC'])
        p += tableRow % ('# jobs failed',t['NJOBS_FAILED'])
        p += tableRow % ('# jobs completed',t['NJOBS_COMPLETED'])
        p += tableRow % ('Comment',t['TASKCOMMENT'])
        p += tableRow % ('Created','%s by %s on host %s' % (time.ctime(t['CREATED']),t['CREATED_USER'],t['CREATED_HOST']))
        p += tableRow % ('Last update',time.ctime(t['UPDATED']))
        p += '</table>\n'

        p += '<div class="text"><h3>Job details (may be more up-to-date than summary data above):</h3></div>\n'
        p += tableHeader;
        a = JobAnalyzer(self.globalConfig['jobDir'],t['DSNAME'],t['TASKNAME'])
        for j in a.jobList():
            p += "<tr>"
            p += "<td>%s</td>" % (j)
            (stat,exitcode) = a.jobStatus(j)
            p += '<td class="%s">%s</td>' % (getStatusClass(stat),getKey(TaskManager.StatusCodes,stat))
            p += "<td>%s</td>" % (exitcode)
            p += '<td><a href="../files?u=%s/%s/%s">Files</a></td>' %(t['DSNAME'],t['TASKNAME'],j)
            p += "</tr>\n"
        p += "</tbody></table>\n"
        return p


# Code to test or run locally
if __name__ == '__main__':
    p = JobDetails()
    print p.index()
