# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
from AthenaCommon.JobProperties import jobproperties

from eflowRec.eflowRecConf import eflowRecoverSplitShowersTool

class eflowRecoverSplitShowersToolDefault_LC ( eflowRecoverSplitShowersTool )  :  # python way to define inheritance

    def __init__(self,name="eflowRecoverSplitShowersToolDefault_LC"):

        eflowRecoverSplitShowersTool.__init__(self,name)

        mlog = logging.getLogger( 'eflowRecoverSplitShowersToolDefault::__init__ ' )
        mlog.info("entering")

        from eflowRec.eflowRecoverSplitShowersSetup import setup_eflowRecoverSplitShowers
        return setup_eflowRecoverSplitShowers(self,"LC",mlog)
