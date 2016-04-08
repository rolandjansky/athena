# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,os.path, time, logging
logger = logging.getLogger('rtt')

class ReportWriter:
    def __init__(self, datasets, configs, packageConfFiles):
        self.datasets = datasets
        self.configs  = configs
        self.pcf      = packageConfFiles

    def write(self, reportDir):
        #print 'Datasets :'+str(self.datasets)
        self.reportDir  = reportDir
        
        self.notToCopy  = [d for d in self.datasets if  d.isInDestDir]
        self.tobeCopied = [d for d in self.datasets if not d.isInDestDir]
        
        logger.debug( 'to be copied :'+str(self.tobeCopied))
        
        self.copyFailed = [d for d in self.tobeCopied if d.copyStatus !='success']
        logger.debug( 'copy Failed '+str(self.copyFailed))
        
        self.copyOK     = [d for d in self.tobeCopied if d.copyStatus =='success']
        logger.debug( 'copyOK '+str(self.copyOK))

        report      = 'DatasetManager.report'
        self.report = os.path.join(self.reportDir, report)
        
        try:
            print 'Working dir : '+str(os.getcwd())
            #os.chdir(os.getcwd())
            fh = open(self.report, 'w')
            #fh.write('\n************************************************************')
            fh.write('\nDataSetManager Report\t\t' + time.ctime())
            fh.write('\n************************************************************\n\n')

            #fh.write('\nDate \t\t\t\t\t: '+ time.ctime())

            fh.write('\n\nRelease      \t\t\t\t: '+str(self.configs.get('release')))
            fh.write('\nRelease Type \t\t\t\t: '+str(self.configs.get('releaseType')))

            #fh.write('\n\n************************************************************')
            fh.write('\n\nTotal number of packages found by RTT \t: '+str(len(self.pcf)))
            fh.write('\n\nTotal number of datasets requested    \t: '+str(len(self.datasets)))

            fh.write('\nNo of datasets to be copied:          \t: '+str(len(self.tobeCopied)))

            fh.write('\nNo of datasets where copyOK:          \t: '+str(len(self.copyOK)))
            fh.write('\nNo of datasets where copyFailed:      \t: '+str(len(self.copyFailed)))

            #fh.write('\n\nDETAILS')
            #fh.write('\n****************************************************************\n')
            fh.write('\n\n\nDetails on Datasets where copy failed:')
           # fh.write('\n****************************************************************')
            for ds in self.copyFailed:
                fh.write('\n====================================')
                fh.write('\nName         :'+str(ds.logicalName))
                fh.write('\nCopy Status  :'+str(ds.copyStatus))
                fh.write('\nFailure Reason :\n')
                fh.write(str(ds.copyInfo))
                fh.write('\nPackageCfg : '+str(ds.packageCfg))
                fh.write('\n====================================\n')    
            #fh.write('\n****************************************************************\n\n')
            fh.write('\n\nDetails on Datasets where copy succeeded:')
            #fh.write('\n****************************************************************')
            for ds in self.copyOK:
                fh.write('\n====================================')
                fh.write('\nName         :'+str(ds.logicalName))
                fh.write('\nCopy Status  :'+str(ds.copyStatus))
                fh.write('\nPhysicalName :'+str(ds.physicalName))
                fh.write('\n====================================\n\n')    
            fh.write('\n****************************************************************\n\n')

            fh.write('\nRequested Datasets already at UCL')
            fh.write('\n****************************************************************')
            for ds in self.notToCopy:
                fh.write('\n'+str(ds.logicalName))
                #if ds.files:
                if ds.dsSource =='dq2':
                    for file in ds.files:
                        fh.write('\n\t'+str(file))
        
            fh.close()

        except Exception ,e :
            logger.error('Could not create a report file '+str(self.report))
            print str(e)
class WriteTest:
    def __init__(self):
        pass

    def write(self):
        fh=open('testReport.results', 'w')
        fh.write('\nThe test file ')


if __name__ =='__main__':

    rt = WriteTest()
    rt.write()
                           
    
                            
    
