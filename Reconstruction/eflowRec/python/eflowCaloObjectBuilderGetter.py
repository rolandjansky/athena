# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
from RecExConfig.Configured import Configured # import base class

class eflowCaloObjectBuilderGetter ( Configured )  :

    def configure(self):

        mlog = logging.getLogger( 'eflowCaloObjectBuilderGetter:configure :' )

        from eflowRec.eflowCaloObjectBuilder import setup_eflowObjectBuilderTools
        return setup_eflowObjectBuilderTools(self, "EM", mlog)

    def eflowObjectBuilderToolsHandle(self):
        return self._eflowObjectBuilderToolsHandle

   
