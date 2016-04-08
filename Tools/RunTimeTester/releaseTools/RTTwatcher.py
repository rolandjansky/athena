# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys
import os.path
import time
from popen2 import Popen4

sys.path.append(os.environ['SCRIPTS'])
from utils import SYSPATH, RTT, ATLAS
SYS  = SYSPATH()
SYS += ['prod_src', 'prod_share']
  
from Evaluate import Evaluate

from  xml.dom.minidom import getDOMImplementation

# ------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------
outputPath = ''
try:
    outputPath = sys.argv[1].strip()
except:
    print 'Please give the directory in which results should be output.'
    sys.exit(1)
    
if not os.path.exists(outputPath):
    print '%s: inexistant' % outputPath
    sys.exit(1)

# ------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------

def makeDOM(docname):
    impl            = getDOMImplementation()
    docType         = impl.createDocumentType(docname, '', '')
    namespace       = None
    document        = impl.createDocument(namespace, docname, docType)
    
    return document

# global dom
theDom = makeDOM('rttStatus')

# useful shortcuts
xmlElementNode = theDom.createElement
xmlTextNode    = theDom.createTextNode

_currentruns = RTT.Current.runs

# ------------------------------------------------------------------------------------
 
def createTextNode(tagName, tagTextContent):
    tag  = xmlElementNode(tagName)
    text = xmlTextNode(tagTextContent)
    tag.appendChild(text)
    return tag

# ------------------------------------------------------------------------------------

def getPopen4Output(cmd, max_lines=-1):
    po = Popen4(cmd)
    out = []
    while po.poll()==-1:
        out.extend(po.fromchild.readlines())
        if max_lines>0 and length(out)>=max_lines: break
    del po
    return out

# ------------------------------------------------------------------------------------
# =====================================================================================
# =====================================================================================
# =====================================================================================

class ClusterHealth:
    def __init__(self):
        self.urlBase = 'http://lemon-gateway.cern.ch/lemon-xml/xml_gateway.php?'
        self.nodes   = self.getnodes()
        self.metrics = {'30045':'exception.no_contact',
                        '20002':'LoadAvg',
                        '6404': 'HostPsNjobs'}

    # ----------------------------------------------------

    def getnodes(self):
        url = self.urlBase + 'clusters=lxbatch/atlasrtt&metrics=20002'
        docEl = self.getLemonXMLResponse(url)
        return [d.getAttribute('node').strip() for d in Evaluate('metric/data', docEl)]
        
    # ----------------------------------------------------

    def makeEntityURLfrag(self):
        entitiesString = 'entities='
        for node in self.nodes: entitiesString += '%s,' % node
        entitiesString = entitiesString[:-1] # remove trailing comma
        return entitiesString

    # ----------------------------------------------------
    
    def makeMetricsURLfrag(self):
        metricsString = 'metrics='
        for metric in self.metrics.values(): metricsString += '%s,' % metric
        metricsString = metricsString[:-1] # remove trailing comma
        return metricsString

    # ----------------------------------------------------
    
    def makeURL(self):
        return self.urlBase + self.makeEntityURLfrag() + '&' + self.makeMetricsURLfrag()
        
    # ----------------------------------------------------

    def getLemonXMLResponse(self, url):
        import urllib
        from xml.dom.minidom import parseString
        handle = urllib.urlopen(url)
        xml_file = handle.read()
        handle.close()
        xml_dom  = parseString(xml_file)
        return xml_dom.documentElement        
        
    # ----------------------------------------------------
    
    def handleNoContactMetric(self, metricEl):
        nodeContactInfo = {}
        for node in self.nodes: nodeContactInfo[node] = 'DOWN' # initialise to "all machines down"
        
        dataEls = metricEl.getElementsByTagName('data')
        
        for dataEl in dataEls:
            nodeName = dataEl.getAttribute('node').strip()
            nodeContactInfo[nodeName] = 'UP'
        return nodeContactInfo    

    # ----------------------------------------------------

    def handleLoadAvg(self, metricEl):
        loadAvgInfo = {}        
        for node in self.nodes: loadAvgInfo[node] = '-1' # initialise to no load found

        dataEls = metricEl.getElementsByTagName('data')
        for dataEl in dataEls:
            # node name
            nodeName = dataEl.getAttribute('node').strip()
            # when load was measured on this node last
            timestamp = Evaluate('r', dataEl)[0].getAttribute('ts').strip()

            # what the load was
            load = Evaluate('r/d/text()', dataEl)
            loadAvgInfo[nodeName] = (load, timestamp)
        return loadAvgInfo

    # ----------------------------------------------------

    def handleNumbBatchJobs(self, metricEl):
        numbJobsInfo = {}
        for node in self.nodes: numbJobsInfo[node] = '-1'
        dataEls = metricEl.getElementsByTagName('data')
        for dataEl in dataEls:
            nodeName = dataEl.getAttribute('node').strip()
            timestamp = Evaluate('r', dataEl)[0].getAttribute('ts').strip()
            # number of jobs running on this node
            numbJobs = Evaluate('r/d/text()', dataEl)
            numbJobsInfo[nodeName] = (numbJobs, timestamp)
        return numbJobsInfo

    # ----------------------------------------------------
    
    def handle(self, metric_name, metricEl):
        dispatcher = {'exception.no_contact': self.handleNoContactMetric,
                      'LoadAvg':self.handleLoadAvg,
                      'HostPsNjobs': self.handleNumbBatchJobs
                      }
        return dispatcher[metric_name](metricEl)
    
    # ----------------------------------------------------
            
    def parse(self, parentEl):
        info = {}
        metricEls = parentEl.getElementsByTagName('metric')
        for metricEl in metricEls:
            metric_name = self.metrics.get(metricEl.getAttribute('id'), None)
            if not metric_name: continue
            info[metric_name] = self.handle(metric_name, metricEl)
        return info
    
    # ----------------------------------------------------

    def constructXMLOutput(self, info):
        lemonEl = xmlElementNode('lemon_info')
        lemonEl.appendChild(createTextNode('numbNodes', str(len(self.nodes))))
        
        for node_name in self.nodes:
            nodeEl = xmlElementNode('node')
            nodeEl.appendChild(createTextNode('nodeName', node_name))
            # contact info i.e. is machine up
            nodeEl.appendChild(createTextNode('up', info['exception.no_contact'][node_name]))
            # load avg info
            load, timestamp = info['LoadAvg'][node_name]
            nodeEl.appendChild(createTextNode('loadAvg', load))
            # number of batch jobs running on the node
            jobs, timestamp = info['HostPsNjobs'][node_name]
            nodeEl.appendChild(createTextNode('numbJobs', jobs))
            lemonEl.appendChild(nodeEl)
        return lemonEl

    def run(self):
        parentNode = self.getLemonXMLResponse(self.makeURL())
        result = self.parse(parentNode)
        return self.constructXMLOutput(result)
        
# =====================================================================================
# =====================================================================================
# =====================================================================================

class JobsInBatch:
    def __init__(self):
        self.cmd = 'python /afs/cern.ch/atlas/project/RTT/scripts/prod/list_jobs.py'
    def run(self):
        lines = [l for l in getPopen4Output(self.cmd)[:-2] if l.find('Brinick')==-1] #ignore Brinick jobs
        jobsEl = xmlElementNode('jobsInBatch')
        runs = {}
        for line in lines:
            lsf_info,run_info,job_info = line.strip().split(']')[:-1]
            lsf_info = lsf_info.strip()[1:] # remove '('
            run_info = run_info.strip()[1:] # remove '('
            job_info = job_info.strip()[1:] # remove '('
            if run_info not in runs.keys(): runs[run_info] = []            
            runs[run_info].append((job_info,lsf_info))

        running = pending = 0
        for k,v in runs.items():
            for job_info, lsf_info in v:
                if lsf_info.find('RUN') != -1: running += 1
                if lsf_info.find('PEND') != -1: pending += 1

        runs2 = {}
        for k,jobs in runs.items():
            runs2[k] = {}
            for job_info, lsf_info in jobs:
                batchNode = lsf_info.split(',')[1].strip()
                if batchNode not in runs2[k].keys(): runs2[k][batchNode] = []
                runs2[k][batchNode].append((job_info, lsf_info))
                
        overEl = xmlElementNode('overview')
        overEl.appendChild(createTextNode('jobsTotal',str(len(lines))))
        overEl.appendChild(createTextNode('jobsRunning',str(running)))
        overEl.appendChild(createTextNode('jobsPending',str(pending)))
        jobsEl.appendChild(overEl)

        runsEl = xmlElementNode('runs')
        for run,dict in runs2.items():
            runEl = xmlElementNode('run')
            runEl.appendChild(createTextNode('name',str(run)))
            # v.sort()

            for batch_node, jobs in dict.items():
                batchEl = xmlElementNode('batchNode')
                batchEl.appendChild(createTextNode('name', batch_node))
                for job_info, lsf_info in jobs:
                    jobEl = xmlElementNode('job')
                    jobEl.appendChild(createTextNode('lsf_info',str(lsf_info)))
                    jobEl.appendChild(createTextNode('job_info',str(job_info)))
                    batchEl.appendChild(jobEl)
                runEl.appendChild(batchEl)
            runsEl.appendChild(runEl)
        jobsEl.appendChild(runsEl)
        return jobsEl
        
# =====================================================================================
# =====================================================================================
# =====================================================================================

class ExtractCronInfo:
    def __init__(self):
        self.copier_jobs, (self.rtt_jobs, self.tct_jobs, self.fct_jobs), self.other_jobs = self.load(os.path.join(os.environ['RES'], 'rtt.acrontable'))

    def load(self, fpath):
        h = open(fpath)
        jobs = h.readlines()
        h.close()
        jobs = [j for j in jobs if not j.strip().startswith('#')] # remove comment lines
        
        copier_jobs = [j for j in jobs if j.lower().find('datacopier')!=-1]

        rtt_jobs   = [j for j in jobs if j.find('/afs/cern.ch/atlas/project/RTT/prod/launch/run.rtt.prod.sh')!=-1 and j.find('log/rtt')!=-1]
        fct_jobs   = [j for j in jobs if j.find('/afs/cern.ch/atlas/project/RTT/prod/launch/run.rtt.prod.sh')!=-1 and j.find('log/fct')!=-1]
        tct_jobs   = [j for j in jobs if j.find('/afs/cern.ch/atlas/project/RTT/prod/launch/run.rtt.prod.sh')!=-1 and j.find('log/tct')!=-1]

        all_jobs = copier_jobs + rtt_jobs + tct_jobs + fct_jobs
        other_jobs = [j for j in jobs if j not in all_jobs]
        return (copier_jobs, (rtt_jobs, tct_jobs, fct_jobs), other_jobs)

    def run(self):
        cronJobsEl = xmlElementNode('cronJobs')    
        copierEl = xmlElementNode('copierCronJobs')

        rttEl    = xmlElementNode('rttCronJobs')
        tctEl    = xmlElementNode('tctCronJobs')
        fctEl    = xmlElementNode('fctCronJobs')
        
        otherEl    = xmlElementNode('otherCronJobs')

        for job in self.copier_jobs:
            copierEl.appendChild(createTextNode('job', job.strip()))

        for job in self.rtt_jobs:
            rttEl.appendChild(createTextNode('job', job.strip()))

        for job in self.tct_jobs:
            tctEl.appendChild(createTextNode('job', job.strip()))

        for job in self.fct_jobs:
            fctEl.appendChild(createTextNode('job', job.strip()))

        for job in self.other_jobs:
            otherEl.appendChild(createTextNode('job', job.strip()))

        cronJobsEl.appendChild(copierEl)
        cronJobsEl.appendChild(rttEl)
        cronJobsEl.appendChild(tctEl)
        cronJobsEl.appendChild(fctEl)                        
        cronJobsEl.appendChild(otherEl)
        return cronJobsEl

# ------------------------------------------------------------------------------------

class NightlyFlagAvailability:
    def __init__(self):
        self.todaysDate = self.getTodayDate()

    def buildFlag(self, branch, platform, project):
        projects = {'offline':'AtlasProduction', 'p1hlt':'AtlasP1HLT', 'tier0':'AtlasTier0', 'hlt':'AtlasHLT'}
        base = {'hlt':'/afs/cern.ch/atlas/project/hlt/builds/nightlies'}.get(project, '/afs/cern.ch/atlas/software/builds/nightlies')
        arch, OS, compiler, build = platform.split('-')
        OS     = OS[0].upper()+OS[-1] # e.g S4
        bits   = {'i686':32,'x86_64':64}.get(arch)
        suffix = '%sB%sAll%s' % (bits, OS, build[0].upper()+build[1:])
        return os.path.join(base, branch, projects[project], 'latest_copied_release%s' % suffix)

    def getTodayDate(self):
        today = time.ctime().split()
        todaysDate = {}
        todaysDate['month'] = today[1]
        todaysDate['day']   = today[2]
        todaysDate['hour']  = today[3].split(':')[0]
        todaysDate['mins']  = today[3].split(':')[1]
        return todaysDate

    def run(self):
        nightliesEl = xmlElementNode('nightliesAvailable')

        for _run in _currentruns(): # current runs
            branch, releaseType, platform, project = _run.split('/')
            flag = self.buildFlag(branch, platform, project)

            if not os.path.exists(flag): continue
            
            cmd = 'cd %s;ls -l %s' % (os.path.dirname(flag), os.path.basename(flag))

            nightlyEl = xmlElementNode('nightly')

            platEl = createTextNode('platform', str(platform))
            flagEl = createTextNode('flagLocation', flag)
            flagRelEl = createTextNode('flagRelease', os.readlink(flag).strip())

            nightlyEl.appendChild(platEl)
            nightlyEl.appendChild(flagEl)
            nightlyEl.appendChild(flagRelEl)
        
            out = getPopen4Output(cmd)

            if not out:
                nightlyEl.appendChild(createTextNode('available', 'unable to obtain any information'))
                nightliesEl.appendChild(nightlyEl)
                print '%s: unable to obtain any information' % str(platform)
                continue

            out   = out[0].split()
            month = out[5]
            day   = out[6]
            hour  = out[7].split(':')[0]
            mins  = out[7].split(':')[1]

            theDate = '%s %s@%s:%s' % (day, month, hour, mins)

            isAvailable = False
            if month == self.todaysDate['month'] and day == self.todaysDate['day']:
                if int(hour) < int(self.todaysDate['hour']):
                    isAvailable = True
                elif (int(hour) == int(self.todaysDate['hour']) and (int(mins) < int(self.todaysDate['mins']))):
                    isAvailable = True

            dateEl = createTextNode('date', theDate)

            if isAvailable:
                readyEl = createTextNode('available', 'READY')
                status = 'READY'
            else:
                readyEl = createTextNode('available', 'NOT READY')            
                status = 'NOT READY'
            
            nightlyEl.appendChild(dateEl)
            nightlyEl.appendChild(readyEl)
            nightliesEl.appendChild(nightlyEl)    
        return nightliesEl

# ------------------------------------------------------------------------------------

class CheckQuotas:
    def __init__(self):
        self.resBase        = '/afs/cern.ch/atlas/project/RTT/Results'
        self.bigfilesBase0  = '/afs/cern.ch/atlas/project/RTT/Results/tempfiles/0/nightly'
        self.bigfilesBase1  = '/afs/cern.ch/atlas/project/RTT/Results/tempfiles/1/nightly'        
        self.bigfile_quotas = []
        self.admin_quotas   = ['/afs/cern.ch/user/r/rtt/',
                               self.resBase,
                               '/afs/cern.ch/atlas/project/RTT/data',
                               '/afs/cern.ch/atlas/project/RTT/prod',
                               '/afs/cern.ch/atlas/project/RTT/prod/log']

    def getQuotaUsage(self, path):
        out = getPopen4Output('fs lq %s' % path)
        volume = out[1].split()[0]
        quota  = out[1].split()[1]
        used   = out[1].split()[2]
        frac   = out[1].split()[3]
        frac   = frac[:frac.find('%')]
        return (path, volume, quota, used, frac)

    def getDateStamp(self, thing):
        from stat import ST_CTIME
        summ = os.path.join(thing, 'RTTSummary.xml')
        if not os.path.exists(summ): return 'n/a'        
        return time.strftime('%d%b@%H:%M', time.localtime(os.lstat(summ)[ST_CTIME]))
    
    def uniquifyThenSort(self, what):
        from sets import Set
        what = list(Set([b for b in what if os.path.exists(b)]))
        what.sort()
        return what

    def run(self):
        runs_dict = {}

        # big file quotas
        for each in _currentruns(): # current runs
            bigfiles1 = os.path.join(self.bigfilesBase0, '%s') % each
            bigfiles2 = os.path.join(self.bigfilesBase1, '%s') % each
            self.bigfile_quotas.extend([bigfiles1, bigfiles2])

            for rel in ['rel_0','rel_1','rel_2','rel_3','rel_4','rel_5','rel_6']:
                resDir = os.path.join(self.resBase, '%s/%s' % (rel, each))
                runs_dict.setdefault(rel, []).append(resDir)

        self.bigfile_quotas = self.uniquifyThenSort(self.bigfile_quotas)

        quotasEl = xmlElementNode('quotas')
    
        admin_quotas = [self.getQuotaUsage(path) for path in self.admin_quotas]
        bigfile_quotas = [self.getQuotaUsage(path) for path in self.bigfile_quotas]

        for k,v in runs_dict.items():
            runs_dict[k] = [(self.getQuotaUsage(path), self.getDateStamp(path)) for path in v if os.path.exists(path)]

        def createQuotaNode(quota_results, date_stamp=None):
            path, volName, tot, used, frac  = quota_results
            error = None
            try:
                if int(tot) < 10000:
                    return None # not in use quota
            except:
                error = 'Problem retrieving!'
            
            quotaEl = xmlElementNode('quota')
            quotaEl.appendChild(createTextNode('dir', path))
            quotaEl.appendChild(createTextNode('volume', volName))

            if not error:
                quotaEl.appendChild(createTextNode('total', tot))
                quotaEl.appendChild(createTextNode('used', used))
                quotaEl.appendChild(createTextNode('frac', frac))
                if date_stamp:
                    quotaEl.appendChild(createTextNode('date', date_stamp))
            else:
                quotaEl.appendChild(createTextNode('total', error))
                quotaEl.appendChild(createTextNode('used', 'n/a'))
                quotaEl.appendChild(createTextNode('frac', 'n/a'))
                
            return quotaEl

        def wrap(parentNodeName, els):
            parentNode = xmlElementNode(parentNodeName)
            parentNode.appendChild(createTextNode('quota_cat_name', parentNodeName))
            [parentNode.appendChild(qel) for qel in els if qel]
            quotasEl.appendChild(parentNode)

        wrap('admin_quotas', [createQuotaNode(qr) for qr in admin_quotas])
        wrap('bigfile_quotas', [createQuotaNode(qr) for qr in bigfile_quotas])

        # run quotas are a bit different in structure
        parentNode = xmlElementNode('runs_quotas')
        parentNode.appendChild(createTextNode('quota_cat_name', 'runs_quotas'))
        for rel, runs in runs_dict.items():
            rel_node = xmlElementNode('release')
            rel_node.appendChild(createTextNode('release_name', rel))
            for quota, date_stamp in runs:
                qr_node = createQuotaNode(quota, date_stamp)
                if qr_node:
                    rel_node.appendChild(qr_node)
            parentNode.appendChild(rel_node)

        quotasEl.appendChild(parentNode)
        return quotasEl

# ------------------------------------------------------------------------------------

class CheckRTTsummaries:
    def __init__(self):
        self.resBase = '/afs/cern.ch/atlas/project/RTT/Results'
        self.summFile = os.path.join(self.resBase, 'summaryFilePaths.txt')

    def readFile(self, fpath):
        h = open(fpath)
        cont = h.readlines()
        h.close()
        return cont

    def getSummTextFiles(self, lines):
        def file_exists(l):
            return os.path.exists(os.path.join(self.resBase, os.path.dirname(l.strip()), 'RTTSummary.txt'))

        from sets import Set
        lines = [l for l in lines if l.find('rel_')!=-1]
        lines = list(Set(lines)) # uniquify
        lines = [l for l in lines if l.split('rel_')[1][2:].strip().split('/RTTSummary.xml')[0] in _currentruns()] # get only current runs summaries

        summaries = [os.path.join(self.resBase, os.path.dirname(l.strip()), 'RTTSummary.txt') for l in lines if file_exists(l)]
        summaries.sort()
        return summaries

    def run(self):
        summFile  = os.path.join(self.resBase, self.summFile)
        summaries = self.getSummTextFiles(self.readFile(summFile))

        summsEl   = xmlElementNode('runSummaries')
    
        for s in summaries:
            cont = self.readFile(s)
            totRTT = [l for l in cont if l.find('totRTT')!=-1]
            if len(totRTT)!=1:
                continue

            toks = totRTT[0].split()
            toks = toks[2:]

            summEl = xmlElementNode('summary')
            summEl.appendChild(createTextNode('runID', s.strip()))
            summEl.appendChild(createTextNode('jobsTot', toks[0].strip()))
            summEl.appendChild(createTextNode('jobsSubmit', toks[1].strip()))
            summEl.appendChild(createTextNode('jobSucc', toks[2].strip()))
            summEl.appendChild(createTextNode('jobsFail', toks[3].strip()))
            summEl.appendChild(createTextNode('jobsTout', toks[4].strip()))
            summEl.appendChild(createTextNode('jobsBerr', toks[5].strip()))
            summEl.appendChild(createTextNode('jobsOpShut', toks[6].strip()))
            summEl.appendChild(createTextNode('jobsDone', toks[-1].strip()))

            summsEl.appendChild(summEl)

        return summsEl

# ------------------------------------------------------------------------------------

def xmlToFile():
    xml_file = os.path.join(outputPath, 'RTTstatus.xml')
    of = open(xml_file, 'w')    
    of.write(theDom.toprettyxml('   ','\n'))
    of.close

# ------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------
# ------------------------------------------------------------------------------------
    
if __name__ == '__main__':
    toDo = [ClusterHealth, ExtractCronInfo, NightlyFlagAvailability, CheckQuotas, CheckRTTsummaries, JobsInBatch]
    [theDom.documentElement.appendChild(xmlSnippet) for xmlSnippet in [callable().run() for callable in toDo]]
    xmlToFile()

    lastRun = os.path.join(outputPath, 'rttStatusLastRun')
    handle  = open(lastRun, 'w')
    handle.write(time.ctime())
    handle.close()
