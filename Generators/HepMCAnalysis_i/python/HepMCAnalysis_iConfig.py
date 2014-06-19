# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from HepMCAnalysis_i.HepMCAnalysis_iConf import HepMCAnalysis

class HepMCAnalysis_i(HepMCAnalysis):
    
    def __init__(self, name = "HepMCAnalysis_i", file = None):
        super( HepMCAnalysis_i, self ).__init__( name )
       
        self.OutputFile = "hepmcanalysis.root"

        from AthenaCommon.Logging import logging
        log = logging.getLogger( 'HepMCAnalysis_i' )

        # set defaults
        if file:
            self.OutputFile = file
            log.info("changing output file name to %s" % self.OutputFile)

        # create THistSvc if not yet done
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr( svcMgr, "THistSvc"):
            log.info("will setup THistSvc and add instance to ServiceMgr")
            from GaudiSvc.GaudiSvcConf import THistSvc
            myHistSvc = THistSvc()
            svcMgr += myHistSvc
        else:
            myHistSvc = svcMgr.THistSvc

        # defining stream and output file
        log.info("here0")
        log.info("Output file: %s" % self.OutputFile)

        myHistSvc.Output += [ ("hepmcanalysis DATAFILE='%s' OPT='RECREATE'" % self.OutputFile)] ####if this line is commented in the segmentation fault appears!!!!!
        myHistSvc.OutputLevel = 1
        #####if the above line is commented out then the segmentation fault appears at event number 5!!!!! STRANGE!!!!!
        #myHistSvc.Output += [ ("hepmcanalysis DATAFILE='test.root' OPT='RECREATE'") ]

        # overriding default IHistSvc in algorithm
        self.ITHistSvc = myHistSvc
        #self.ITHistSvc.Output = [ ("hepmcanalysis DATAFILE='%s' OPT='RECREATE'" % self.OutputFile)] ####if this line is commented in the segmentation fault appears!!!!!
        #####if the above line is commented out then the segmentation fault appears at event number 5!!!!! STRANGE!!!!!
