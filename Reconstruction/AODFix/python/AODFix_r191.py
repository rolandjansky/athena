# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging

logAODFix_r191 = logging.getLogger( 'AODFix_r191' )

from AODFix_base import AODFix_base

class AODFix_r191(AODFix_base):
    ''' This class just performs AODFix on 19.1.X releases

    Instance variables
      addMetadata (bool) (inherited): whether to add metaData
      doAODFix (bool) (inherited): whether to do AODFix
      prevAODFix (string): previous AODFix applied
      newAODFix (string): new AODFix to apply
    '''

    @staticmethod
    def latestAODFixVersion():
        """The latest version of the AODFix."""
        return "r191v1"
    
    # inside RecExCommon_topOptions.py:
    def preInclude(self):
        if self.doAODFix:
            logAODFix_r191.debug("executing preIncude")

            # run trigger fix
            from AthenaCommon.AppMgr import ToolSvc
            from AthenaCommon.AlgSequence import AlgSequence
            topSequence=AlgSequence()

            #fix xAOD trigger object by scheduling cnv alg again

            from xAODTriggerCnv.xAODTriggerCnvConf import xAODMaker__TrigDecisionCnvAlg
            topSequence += xAODMaker__TrigDecisionCnvAlg("TrigDecisionCnvAlg")
            topSequence.TrigDecisionCnvAlg.xAODKey = 'xTrigDecision'

            from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
            ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

