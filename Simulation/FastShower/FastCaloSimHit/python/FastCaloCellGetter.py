# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# specifies Calo cell making
# so far only handle the RawChannel->CaloCell step
# not all possibility of CaloCellMaker_jobOptions.py integrated yet
from AthenaCommon.Constants import *
from RecExConfig.Configured import Configured


class CaloCellGetter ( Configured )  :
    _outputType = "CaloCellContainer"
    _output = { _outputType : "AllFastCalo" }

        
    def configure(self):
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'CaloCellGetter::configure:' )
        mlog.info ('entering')        

        doStandardCellReconstruction=True
        from CaloRec.CaloCellFlags import jobproperties
        
        if not jobproperties.CaloCellFlags.doFastCaloSim.statusOn:
            doFastCaloSim=False
            mlog.info("doFastCaloSim not set, so not using it")
        else:
            doFastCaloSim=jobproperties.CaloCellFlags.doFastCaloSim()
            if doFastCaloSim:
                mlog.info("doFastCaloSim requested")
                doStandardCellReconstruction=False
                if jobproperties.CaloCellFlags.doFastCaloSimAddCells():
                    doStandardCellReconstruction=True
                    mlog.info("doFastCaloSimAddCells requested: FastCaloSim is added to fullsim calorimeter")
                else:    
                    mlog.info("doFastCaloSimAddCells not requested: Stand alone FastCaloSim is running")
            else:    
                mlog.info("doFastCaloSim explicitly not requested")


        # now configure the algorithm, part of this could be done in a separate class
        # cannot have same name
        try:        
            from CaloRec.CaloRecConf import CaloCellMaker                
        except:
            mlog.error("could not import CaloRec.CaloCellMaker")
            print traceback.format_exc()
            return False

        theCaloCellMaker=CaloCellMaker("FastCaloCellMaker")
        self._CaloCellMakerHandle = theCaloCellMaker ;

        from AthenaCommon.AppMgr import ToolSvc


        if doFastCaloSim:
            mlog.info ('configuring FastCaloSim here')        
            
            try:
                from FastCaloSim.FastCaloSimConf import EmptyCellBuilderTool
                theEmptyCellBuilderTool=EmptyCellBuilderTool()
                ToolSvc += theEmptyCellBuilderTool
                theCaloCellMaker.CaloCellMakerToolNames += [ theEmptyCellBuilderTool ]

                print theEmptyCellBuilderTool
                mlog.info("configure EmptyCellBuilderTool worked")
            except:
                mlog.error("could not get handle to EmptyCellBuilderTool Quit")
                print traceback.format_exc()
                return False
            

            try:
            #if True :
                from FastCaloSim.FastCaloSimFactory import FastCaloSimFactory
                theFastShowerCellBuilderTool=FastCaloSimFactory()

                ToolSvc += theFastShowerCellBuilderTool
                theCaloCellMaker.CaloCellMakerToolNames += [ theFastShowerCellBuilderTool ]
                mlog.info("configure FastShowerCellBuilderTool worked")
            except:
                mlog.error("could not get handle to FastShowerCellBuilderTool Quit")
                print traceback.format_exc()
                return False


        #
        # CaloCellContainerFinalizerTool : closng conainer and seting up iterators
        #
    
        from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool     
        theCaloCellContainerFinalizerTool=CaloCellContainerFinalizerTool()
        ToolSvc += theCaloCellContainerFinalizerTool
        theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerFinalizerTool ]


        # make lots of checks (should not be necessary eventually)
        # to print the check add:

        from CaloRec.CaloRecConf import CaloCellContainerCheckerTool   
        theCaloCellContainerCheckerTool=CaloCellContainerCheckerTool()     
        # FIXME
        # theCaloCellContainerCheckerTool.OutputLevel=DEBUG

        ToolSvc += theCaloCellContainerCheckerTool
        theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerCheckerTool] 


        #

        # sets output key  
        theCaloCellMaker.CaloCellsOutputName=self.outputKey()        


#        # register output in objKeyStore
#        from RecExConfig.ObjKeyStore import objKeyStore
#
#        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # should go just before FastHitConv in topSequence

        mlog.info(" now adding CaloCellMaker to topSequence")        

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()


        list = topSequence.getSequence()
        print list

        print "FastHitConv position = " + str(list.index('FastHitConv/theFastHitConv'))

        topSequence.insert(list.index('FastHitConv/theFastHitConv'),theCaloCellMaker)
        



        return True

    def CaloCellMakerHandle(self):
        return self._CaloCellMakerHandle

   
# would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



