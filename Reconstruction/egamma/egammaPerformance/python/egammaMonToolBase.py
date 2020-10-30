# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# base configuration of the egammaMonTool
from AthenaCommon.Logging import logging
import traceback

#import base class
from egammaPerformance.egammaPerformanceConf import egammaMonTool

class egammaMonToolBase ( egammaMonTool ) :

        def __init__(self, name):
                mlog = logging.getLogger(name+'::__init__')
                mlog.info("entering")

                egammaMonTool.__init__(self,name)
                self._name = name

                try:
                        ## add an AthenaMonManager algorithm to the list of algorithms to be ran
                        from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
                        monMan = AthenaMonManager( "MonManager_%s" % name )

                        monMan.AthenaMonTools += [ "egammaMonTool/'%s'" % name ]

                        # FileKey must match that given to THistSvc
                        monMan.FileKey = "'%s'" % name

                        # Set global monitoring parameters: see the AthenaMonManager class
                        # in the Control/AthenaMonitoring package
                        monMan.ManualDataTypeSetup = True
                        monMan.DataType            = "monteCarlo"
                        monMan.Environment         = "user"
                        monMan.ManualRunLBSetup    = True
                        monMan.Run                 = 1
                        monMan.LumiBlock           = 1

                except Exception:
                        mlog.error("could not initialize egammaMonTool/%s", name)
                        print(traceback.format_exc())
                        return False

                # get a handle on the top sequence of algorithms
                from AthenaCommon.AlgSequence import AlgSequence
                topSequence = AlgSequence()
                topSequence += monMan


        def get_name(self) :
                return self._name
