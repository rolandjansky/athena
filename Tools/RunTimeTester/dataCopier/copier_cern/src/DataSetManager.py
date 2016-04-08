# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, os, os.path, logging

from DSMConfigReader import DSMConfigReader
#from DataSetMaker    import DataSetMaker
#from Factory_Action  import ActionFactory,DQ2ActionFactory
#from DataSet         import DataSetToBeCopied
#from ReportWriter    import ReportWriter

#from validateXMLFile import validateXMLFile
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
        print str(self.cfg)
        sys.path.append(self.cfg['rttsrc'])
        print 'Sys.path ....'
        print str(sys.path)

    def getDataSets(self, files, catalog):
        from DataSetMaker    import DataSetMaker
      
        nonDQ2data = []
        dq2data    = []
        validFiles = [file for file in files if os.path.exists(file)]
        for file in validFiles:
            #print'reading '+str(file)
            dsmaker = DataSetMaker(file,
                                   self.cfg['datasetDir'],
                                   catalog,
                                   self.cfg['stageArea'] )
            ds      = dsmaker.makeDatasets()
            ds      = [d for d in ds if d != None]
            
            nonDQ2      = [d for d in ds[0] if d != None]
            dq2         = [d for d in ds[1] if d != None]
        
            nonDQ2data.extend(nonDQ2)
            dq2data.extend(dq2)
            #sys.exit(0)
        return (nonDQ2data, dq2data)
        
    def run(self):
        from DataSetMaker    import DataSetMaker
        from Factory_Action  import ActionFactory,DQ2ActionFactory
        from DataSet         import DataSetToBeCopied
        from ReportWriter    import ReportWriter

        from validateXMLFile import validateXMLFile

      
        from CatalogMaker     import CatalogMaker
        from Factory_USRMaker import USRMakerFactory

        from LoggerSetup      import setupLoggers        
        setupLoggers(os.getcwd())
        logger=logging.getLogger('rtt')
        
        logger.info('Making UserStuffRetriever')
        usrMaker = USRMakerFactory().create(self.cfg['releaseType'])
        usrMaker = usrMaker(self.cfg['rttcfg'], logger, self.cfg)
        usr      = usrMaker.makeUSR()
        
        logger.info ('USR made succesfully, finding package configuration files ')  
        packages            = usr.userStuff()
        rttPackages         = [p for p in packages if p.wantsRTT]
        logger.debug( 'usr found '+str(len(packages))+' packages')
        self.packageConfigs = [p.pathToTestConfig for p in rttPackages]
        logger.info(str(self.packageConfigs))
        
        #self.packageConfigs=['/home/en/testArea/toAddToCatalog.xml']

        self.cm                = CatalogMaker(#self.cfg['datasetCatalog'],
                                              usrMaker.paths,
                                              self.cfg['datasetCatalog'],
                                              logger)
        self.catalog           = self.cm.catalog
        self.datasets,self.dq2 = self.getDataSets(self.packageConfigs, self.catalog)  
        self.tobeCopied        = [ds for ds in  self.datasets if ds.isToCopy]
        self.dq2TobeCopied     = [ds for ds in self.dq2 if ds.isToCopy]
       
        logger.debug( 'No of dq2 datasets to be copied: '+str(len(self.dq2TobeCopied)))
        
        logger.debug('no of nonDQ2 datasets to be copied '+str(len(self.tobeCopied)))
        logger.debug('no of DQ2 datasets to be copied '+str(len(self.dq2TobeCopied)))

        af     = ActionFactory()
        action = af.create(self.cfg['mode'], self.cfg['site']) #action is to list/copy
        for dataset in self.tobeCopied:                        #a file on castor
            dataset.__class__= DataSetToBeCopied
            action(dataset).doIt()
            
        dqf     = DQ2ActionFactory()
        action  = dqf.create(self.cfg['mode']) #action is to list/copy
        for data in self.dq2TobeCopied:
          print ' copying of files using dq2'
          action(data).doCopy()
            
        self.copyOK = [d for d in self.tobeCopied if d.copyStatus =='success']
        logger.info(str(len(self.copyOK)) +' dataset(s) copied successfully')

        self.dq2CopyOk =[d for d in self.dq2TobeCopied
                         if d.copyStatus == 'success' ]

        self.datasets.extend(self.dq2)
        self.copyOK.extend(self.dq2CopyOk)
        
        logger.info( 'writing report ')
        rw = ReportWriter(self.datasets, self.cfg, self.packageConfigs)
        rw.write(self.cfg['reportDir'])
        logger.info('report written to '+str(rw.report))

        logger.info('Making new dataset catalog')
        self.cm.update(self.copyOK, self.datasets )
        self.cm.makeCatalog()

      

if __name__ =='__main__':

    if len(sys.argv) != 2:
        usage()

    dsmConfigFile = sys.argv[1]
    #dsmConfigFile ='../cfg/dsm_cfg_nightly.xml'
    
    dr = DSMRunner(dsmConfigFile)
    dr.run()


