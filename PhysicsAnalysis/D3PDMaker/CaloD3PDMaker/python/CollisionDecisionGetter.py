# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file CaloD3PDMaker/python/CollisionDecisionGetter.py
# @author scott snyder <snyder@bnl.gov>
# @date Mar, 2010
# @brief Configure algorithm to calculate collision decision flags.
#


from RecExConfig.Configured import Configured
from AthenaCommon.AlgSequence import AlgSequence

class CollisionDecisionGetter ( Configured )  :
    _outputType = "SkimDecisionCollection"
    _output = { _outputType : "DESD_COLLCAND_SkimDecisionsContainer" }


    def __init__ (self,
                  seq = AlgSequence(),
                  *args, **kw):
        self.seq = seq
        Configured.__init__ (self, *args, **kw)
        return

        
    def configure(self):
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'CollisionDecision::configure:' )
        mlog.info ('entering')

        import traceback
        try:
            from AthenaCommon.AppMgr             import ToolSvc

            from PrimaryDPDMaker.PrimaryDPDMakerConf import MBTSTimeFilterTool
            ToolSvc += MBTSTimeFilterTool \
                       (ChargeThreshold   = 60.0/222.0,
                        MinHitsPerSide    = 2,
                        MaxTimeDifference = 10.0)

            from EventBookkeeperTools.BookkeepingInfoWriter \
                 import SkimDecisionsWriter
            skimmer = SkimDecisionsWriter ('D3PDSkimDecisionWriter')
            skimmer.SkimDecisionsContainerName = 'DESD_COLLCAND_SkimDecisionsContainer'

            from PrimaryDPDMaker.PrimaryDPDMakerConf import CollisionFilterAlg
            from RecExConfig.ObjKeyStore import objKeyStore
            if objKeyStore.isInInput ('CaloCellContainer', 'AllCalo'):
                self.seq += CollisionFilterAlg ('CaloTimeFilterInCollisionStream',
                                                DoCaloTimeFilter = True,
                                                DoMBTSTimeFilter = False)
                skimmer.addAcceptAlg ('CaloTimeFilterInCollisionStream')

            self.seq += CollisionFilterAlg ('MBTSTimeFilterInCollisionStream',
                                            DoCaloTimeFilter = False,
                                            DoMBTSTimeFilter = True)
            skimmer.addAcceptAlg ('MBTSTimeFilterInCollisionStream')

            from PrimaryDPDMaker.TriggerFilter import TriggerFilter
            self.seq += TriggerFilter ('TriggerFilterInCollissionStream',
                                  trigger = 'L1_MBTS_1_1')
            skimmer.addAcceptAlg ('TriggerFilterInCollissionStream')

            self.seq += skimmer

            # register output in objKeyStore
            objKeyStore.addTransient (self.outputType(),self.outputKey())

            self._handle = skimmer

        except:
            mlog.error ("Error configuring LArCollisionTimeAlg.")
            traceback.print_exc()

        return True


    def handle (self):
        return self._handle

    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType
