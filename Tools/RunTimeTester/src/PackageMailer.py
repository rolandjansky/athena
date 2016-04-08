# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from Tools                    import sendMail
from Tools2                   import deSpamifyEmailAddress
import os.path

class PackageMailer:

    def __init__(self, packageName, mailTo, release, runType, originalBranch,
                 cmtConfig, pathSegment, packageTag, resultsDirs,
                 resultsBaseURL, confFile, disabled, logger):
        self.name            = packageName
        self.mailTo          = mailTo
        self.release         = release
        self.runType         = runType
        self.originalBranch  = originalBranch
        self.cmtConfig       = cmtConfig
        self.pathSegment     = pathSegment
        self.packageTag      = packageTag
        self.resultsDirs     = resultsDirs
        self.resultsBaseURL  = resultsBaseURL
        self.confFile        = confFile
        self.disabled        = disabled

        self.logger          = logger
        self.emailStats      = {}
        
    def sendMailToPackageDevelopers(self):

        if self.disabled: return

        if not self.packageRequestsMailOnFailure():
            self.logger.debug('Package did not request mail via <mailto> tag. None sent.')
            return

        if not self.emailStats.keys(): return

        emailAddresses = deSpamifyEmailAddress(self.mailTo)
        
        self.logger.debug('Package requests mail be sent to: %s' % str(emailAddresses))

        release  = self.release
        runType  = self.runType
        branch   = self.originalBranch
        platform = self.cmtConfig
        majMinProj  = self.pathSegment
        packageTag = self.packageTag
        
        packageResDir = os.path.join(self.resultsDirs['packages'], self.name)
        baseResURL    = self.resultsBaseURL

        failedJobs = [k for k,v in self.emailStats.items() if not v]
        okJobsFailedTests = [(k,v) for k,v in self.emailStats.items() if v]

        if not okJobsFailedTests:
            text = '%d job(s) failed' % len(failedJobs)
        elif not failedJobs:
            text = '%d successful job(s) had one or more failed test' % len(okJobsFailedTests)
        else:
            text = '%d job(s) failed and %d successful jobs had failed tests' % (len(failedJobs), len(okJobsFailedTests))
        
        message  = '**************************************************************\n'
        message += 'PLEASE DO NOT REPLY TO THIS EMAIL ADDRESS.\n'
        message += '**************************************************************\n\n'
        message += 'Dear RTT user,\n\n'
        message += 'The RTT is sending you this automated email because %s\n' % text
        message += 'in package %s and, in such cases, you requested to be email alerted.\n' % self.name
        message +=  '--- you did this using the <mailto> XML tag in your package configuration file:\n\n'
        message += str(self.confFile) + '\n\n'
        message += 'To stop future emails, simply remove the tag.\n\n'
        message += 'Questions? Contact: rtt@hep.ucl.ac.uk\n\n'
        message += '----------------\n'
        message += 'Run      : %s/%s/%s\n' % (release, branch, runType)
        message += 'Platform : %s\n' % platform
        message += 'Package  : %s\n\n' % self.name
        message += 'CVS tag  : %s\n\n' % packageTag
        message += 'Number of jobs that failed: %d\n' % len(failedJobs)
        message += 'Number of successful jobs but with >=1 failed tests: %d\n' % len(okJobsFailedTests)
        message += '----------------\n\n'

        index = 1


        if failedJobs:
            message += '**** JOBS THAT FAILED ****\n\n'
            for (jobName, jobID, jobGroup) in failedJobs:
                m  = 'Job #%s\n' % str(index)
                m += '   - Name  : %s\n' % jobName
                m += '   - Group : %s\n' %  jobGroup
                m += '   - AFS   : %s\n' % os.path.join(packageResDir, jobGroup, jobName, jobID)
                jobURL = os.path.join(baseResURL, 'page2.php')
                jobURL += '?xml=%s/%s/%s/%s/%s/RTTSummary.xml&package=%s&job=%s&id=%s' % (release,
                                                                                          branch,
                                                                                          runType,
                                                                                          platform,
                                                                                          majMinProj,
                                                                                          self.name,
                                                                                          jobName,
                                                                                          jobID)
                m += '   - URL   : %s\n\n' % jobURL
                message += m
                index   += 1

        if okJobsFailedTests:    
            message += '**** JOBS THAT SUCCEEDED BUT THAT HAD FAILED TESTS ****\n\n'
            for (jobName, jobID, jobGroup), failedTests in okJobsFailedTests:
                m  = 'Job #%s\n' % str(index)
                m += '   - Name  : %s\n' % jobName
                m += '   - Group : %s\n' %  jobGroup
                m += '   - AFS   : %s\n' % os.path.join(packageResDir, jobGroup, jobName, jobID)
                jobURL = os.path.join(baseResURL, 'page2.php')
                jobURL += '?xml=%s/%s/%s/%s/%s/RTTSummary.xml&package=%s&job=%s&id=%s' % (release,
                                                                                          branch,
                                                                                          runType,
                                                                                          platform,
                                                                                          majMinProj,
                                                                                          self.name,
                                                                                          jobName,
                                                                                          jobID)
                m += '   - URL   : %s\n\n' % jobURL

                m += '   - Tests that failed in this job:\n'
                for ft in failedTests: # ft = RTTTestRunner instance
                    m += '\t%s\n' % ft
                m += '\n'
                message += m
                index   += 1
        message += '--------------------------\n'    
        message += 'AFS base results dir for your entire package:\n'
        message += '   %s\n' % packageResDir
        message += 'URL for your entire package:\n'
        message += '   %s?xml=%s/%s/%s/%s/%s/RTTSummary.xml&package=%s' % (os.path.join(baseResURL, 'page1.php'),
                                                                           release,
                                                                           branch,
                                                                           runType,
                                                                           platform,
                                                                           majMinProj,
                                                                           self.name)
            
        message += '\n\nRegards,\n'
        message += '     The RTT team\n\n'
        message += '--------------------------------------------------------\n'
        message += 'RTT results: http://cern.ch/atlasrtt/page0.php\n'
        message += 'RTT home   : http://www.hep.ucl.ac.uk/atlas/AtlasTesting\n'

        subject = 'Failures in %s (%s/%s/%s: %s)' % (str(self.name), release, branch, runType, platform)
        sendMail(emailAddresses,subject, message)
        self.logger.debug("Sent mail to: %s" % str(emailAddresses))
            
# ------------------------------------------------------------------------- 

    def packageRequestsMailOnFailure(self):
        return len(self.mailTo.strip()) > 0

# -------------------------------------------------------------------------

    def collectStats(self, minders):
        def jobFailed(m):
            jobStatus = m.status()
            return jobStatus == 'error' and not 'Production Manager Veto' in m.errorMessages
        def jobSucceeded(m):
            return m.status()!='error'            
        def testsFailed(m):
            return [t.testId for t in m.finishedTests if t.result!=0]

        if not self.packageRequestsMailOnFailure(): return
        failed = {}
        for m in minders:
            jobName   = m.name
            jobID     = m.jobSerialNumber
            jobGroup  = m.jobGroup
            if jobFailed(m):
                failed[(jobName,str(jobID),jobGroup)] = []
            elif jobSucceeded(m):
                tf = testsFailed(m)
                if tf:
                    failed[(jobName,str(jobID),jobGroup)] = tf

        self.emailStats.update(failed)
   
