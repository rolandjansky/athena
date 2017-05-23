# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS beam spot web page for letting users create their customized beam spot plots.
"""

__author__  = 'Juerg Beringer'
__version__ = 'MyPlots.py atlas/athena'

from WebPage import *
from BeamSpotWebPage import BeamSpotWebPage
from InDetBeamSpotExample.TaskManager import *

import os
import time
import glob
from math import floor
import commands
from cgi import escape

cmdoutput = """\
<div class="boldtext">
Command:
</div>
<pre class="text">
%s
</pre>
<div class="boldtext">
Output (status %s):
</div>
<div class="text">
<pre>
%s
</pre>
</div>
"""

image = """\
<p class="text">
<a href="%s">Link to generated plot</a>
(<a href="%s">eps</a>, 
<a href="%s">pdf</a>)   (valid for 3 days)
<p>
<img id="plot" src="%s" />
"""

def error(msg):
    """Utility to render an error message."""
    return htmlText(msg,'class="errormsg"',escapeText=True)


class MyPlots(BeamSpotWebPage):

    def __init__(self):
        BeamSpotWebPage.__init__(self)
        self.pageConfig['pageTitle'] = 'ATLAS Beam Spot Plots'

    def content(self,**args):

        # Configuration and defaults
        ntDir = self.globalConfig['ntDir']
        periodDir = ntDir+'/DataPeriods'
        cmd = self.globalConfig['wwwDir']+'/run-beamspotnt.sh'
        if not os.path.exists(cmd):
            return error('Configuration error: %s does not exist or unreadable' % cmd)
        if not 'plotType' in args:
            args['plotType'] = 'plot'

        # Start form
        form = htmlPara('Select data and what to plot:', attr = 'class ="example"')

        # Project tag selection
        projectList = sorted([ os.path.basename(p) for p in glob.glob(periodDir+'/data*')])
        form += htmlSelect('Project tag','project', args, projectList, 'all data',
                           attr = 'onchange = "this.form.submit()"')

        # Period selection (only if project selected)
        if 'project' in args and args['project']:
            periodList = glob.glob(periodDir+'/'+args['project']+'/*')
            periodList = sorted([ os.path.basename(p)[:-10] for p in periodList ])
            if 'AllYear' in periodList:
                periodList.remove('AllYear')
                periodList.insert(0,'AllYear')
            if not 'period' in args and periodList:
                args['period'] = periodList[0]
            form += htmlSelect('Period','period', args, periodList, attr='multiple size=5')

        # Fill and run number selection (only if no project selected)
        if not args.get('project'):
            form += htmlTextInput('or select fill', 'fillnr', args,6,6)
            form += htmlTextInput('or run', 'runnr', args,6,6)

        # Ntuple selection
        form += htmlPara()
        form += htmlSelect('Ntuple', 'nt', args,
                           ['initial T0 processing::t0/beamspotnt-IndetBeampos-ES1-UPD2.root',
                            'latest data from COOL::cool/beamspotnt-COOL-Current.root'])

        # Plot type selection
        form += htmlPara()
        form += htmlSelect('Type of plot','plotType', args,
                           ['time evolution::plot', 'summary (slow)::summary', 'histogram::hist'],
                           attr = 'onchange = "this.form.submit()"')

        # Plot variable selection (except for summary plot)
        if args['plotType']!='summary':
            form += htmlSelect('for','varName', args,
                               ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ','tiltX','tiltY','rhoXY','k'])

        # Plot options
        form += htmlPara('Options:', attr='class ="example"')
        if args['plotType']!='hist':
            form += htmlCheckbox('Use LB instead of time on x axis','lbaxis',args)
            form += htmlCheckbox('Predefined axis range','ydef', args)
            form += htmlPara()
            form += htmlTextInput('y axis scale expansion factor','yscale',args,6,6)
            if args['plotType']=='plot':
                form += htmlPara()
                form += htmlTextInput('y axis minimum','ymin',args,6,6)
                form += htmlTextInput('y axis maximum','ymax',args,6,6)
        else:
            form += htmlTextInput('x axis minimum','xmin',args,6,6)
            form += htmlTextInput('x axis maximum','xmax',args,6,6)
            form += htmlPara()
            form += htmlCheckbox('Log y axis', 'logy', args)

        # Command line output option and submit button
        form += htmlPara('Generate plot (depending on what parameters you choose, this may take a while.. please be patient):', attr='class ="example"')
        form += htmlCheckbox('Show command line output','verbose',args)
        form += htmlSubmit('Plot', 'doplot', attr = 'class="submit"')

        # Render selection form
        page = htmlForm(form, attr='class="params"')

        # Output parameters for debugging
        if 'debug' in args:
            page += escape(str(args))

        # Plotting
        if 'doplot' in args:

            # Unique file name
            t = time.time()
            ms = 1000.*(t-floor(t))
            varName = args.get('varName', 'all')
            baseName = 'beamspot-%s-%s' % (args['plotType'], varName)
            baseName = time.strftime(baseName+'-%Y%m%d-%H%M%S',time.localtime())
            baseName += '%03i' % ms

            # Assemble command and execute
            cmd += ' -b'
            cmd += ' -f %s/%s' % (ntDir,args['nt'])
            cmd += ' -o %s/tmp/%s.gif,%s/tmp/%s.eps,%s/tmp/%s.pdf' % (self.globalConfig['wwwDir'],baseName,
                                                                      self.globalConfig['wwwDir'],baseName,
                                                                      self.globalConfig['wwwDir'],baseName)
            if args.get('project'):
                # NOTE: due to the defaults above, period will always be in args
                selPeriodList = args['period'] if isinstance(args['period'],list) else [args['period']]
                selPeriodString = ','.join([args['project']+'.'+p for p in selPeriodList])
                cmd += ' --perioddef %s --period %s' % (periodDir,selPeriodString)
            if args.get('fillnr'):
                try:
                    cmd += ' --fill %i' % int(args['fillnr'])
                except:
                    return page+error('Fill number must be an integer')
            if args.get('runnr'):
                try:
                    cmd += ' --run %i' % int(args['runnr'])
                except:
                    return page+error('Run number must be an integer')
            if not 'lbaxis' in args:
                cmd += ' --timeaxis'
            if 'logy' in args:
                cmd += ' --logy'
            if 'ydef' in args:
                cmd += ' --ydef'
            if  args.get('yscale','')!='':
                try:
                    dummy = float(args['yscale'])
                    cmd += ' --yscale %s' % args['yscale']
                except:
                    return page+error('y axis scale expansion factor must be a number')
            if  args.get('ymin','')!='':
                try:
                    dummy = float(args['ymin'])
                    cmd += ' --ymin %s' % args['ymin']
                except:
                    return page+error('y axis minimum must be a number')
            if  args.get('ymax','')!='':
                try:
                    dummy = float(args['ymax'])
                    cmd += ' --ymax %s' % args['ymax']
                except:
                    return page+error('y axis maximum must be a number')
            if  args.get('xmin','')!='':
                try:
                    dummy = float(args['xmin'])
                    cmd += ' --xmin %s' % args['xmin']
                except:
                    return page+error('x axis minimum must be a number')
            if  args.get('xmax','')!='':
                try:
                    dummy = float(args['xmax'])
                    cmd += ' --xmax %s' % args['xmax']
                except:
                    return page+error('x axis maximum must be a number')
            if args['plotType']=='summary':
                cmd += ' summary'
            else:
                cmd += ' %s %s' % (args['plotType'], args['varName'])
            (status,output) = commands.getstatusoutput(cmd)

            # Plot
            if not status:
                imageUrl = '%s/tmp/%s.gif' % (self.globalConfig.baseUrl,baseName)
                epsUrl = '%s/tmp/%s.eps' % (self.globalConfig.baseUrl,baseName)
                pdfUrl = '%s/tmp/%s.pdf' % (self.globalConfig.baseUrl,baseName)
                page += image % (imageUrl,epsUrl,pdfUrl,imageUrl)

            # Command line output
            if status or ('verbose' in args):
                page += cmdoutput % (escape(cmd),status,escape(output))

        return page
 

# Code to test or run locally
if __name__ == '__main__':
    page = MyPlots()
    print p.index()
