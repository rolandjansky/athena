# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, os, os.path, logging

from DSMConfigReader import DSMConfigReader
from DataSetMaker    import DataSetMaker
from Factory_Action  import ActionFactory
from DataSet         import DataSetToBeCopied
from ReportWriter    import ReportWriter


#================================================================
def usage():
  usg  = '\nUsage: '+sys.argv[0]+' <cfg_file> \n\n'
  usg += '<cfg_file> is an xml config file \n'
  usg += 'specifying configuration parameters needed to \n'
  usg += 'initialise the script\n'
  usg += 'See sample config file at: ...address!'
  print usg
  sys.exit(0)
#=============================================================


class DSMRunner:
    def __init__(self, dsmcfg):
        self.dsmcfg     = dsmcfg
        self.confReader = DSMConfigReader(self.dsmcfg)
        self.cfg        = self.confReader.readDSMcfg()
        
        sys.path.append(self.cfg['rttsrc'])
        

    def getDataSets(self, files):
        datasets= {}   # datasets[logicalName]=datasetObject 
        for file in files:
            #print'reading '+str(file)
            dsmaker = DataSetMaker(file,self.cfg['datasetDir'])
            ds      = dsmaker.makeDatasets()
            for d in ds:
                datasets[d.logicalName]= d 
        return datasets
        
    def run(self):
        from CatalogMaker     import CatalogMaker
        from Factory_USRMaker import USRMakerFactory

        from LoggerSetup      import setupLoggers        
        setupLoggers(os.getcwd())
        logger=logging.getLogger('rtt')
        
        logger.info( 'USR project maker imported')
        usrMaker = USRMakerFactory().create(self.cfg['releaseType'])
        usrMaker = usrMaker(self.cfg['rttcfg'])
        usr      = usrMaker.makeUSR()
        logger.info ('finding package configuration files ')
        packages = usr.userStuff()
        
        logger.debug( 'usr found '+str(len(packages))+' packages')

        self.packageConfigs = [p.pathToTestConfig for p in packages]
        self.datasets       = self.getDataSets(self.packageConfigs)
        self.tobeCopied     = [val for val in self.datasets.values()
                               if not val.isInDestDir]
        
        logger.debug('no of datasets to be copied '+str(len(self.tobeCopied)))

        af     = ActionFactory()
        action = af.create(self.cfg['mode']) #action is to list/copy
        for dataset in self.tobeCopied:      #a file on castor
            dataset.__class__= DataSetToBeCopied
            action(dataset).doIt()
            
        self.copyOK = [d for d in self.tobeCopied if d.copyStatus =='success']
        
        logger.info( 'write report ')
        rw = ReportWriter(self.datasets, self.cfg, self.packageConfigs)
        rw.write()

        logger.info('Making new dataset catalog')
        cm = CatalogMaker(self.cfg['datasetCatalog'],self.cfg['datasetCatalog'])
        cm.update(self.copyOK)
        cm.makeCatalog()



if __name__ =='__main__':

    if len(sys.argv) != 2:
        usage()

    dsmConfigFile = sys.argv[1]
    
    dr = DSMRunner(dsmConfigFile)
    dr.run()


