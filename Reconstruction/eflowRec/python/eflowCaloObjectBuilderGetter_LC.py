# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
from RecExConfig.Configured import Configured # import base class

class eflowCaloObjectBuilderGetter_LC ( Configured )  :

    def configure(self):

        mlog = logging.getLogger( 'eflowCaloObjectBuilderGetter_LC:configure :' )

        from eflowRec.eflowCaloObjectBuilder import setup_eflowObjectBuilderTools
        return setup_eflowObjectBuilderTools(self, "LC", mlog)

    def eflowObjectBuilderToolsHandle(self):
        return self._eflowObjectBuilderToolsHandle

   
