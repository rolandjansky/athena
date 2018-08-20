# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger 
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class

class eflowCaloObjectBuilderGetter_HLLHC ( Configured )  :

    def configure(self):

        mlog = logging.getLogger( 'eflowCaloObjectBuilderGetter:configure :' )

        from eflowRec.eflowCaloObjectBuilder import setup_eflowObjectBuilderTools
        return setup_eflowObjectBuilderTools(self, "EM_HLLHC", mlog)

    def eflowObjectBuilderToolsHandle(self):
        return self._eflowObjectBuilderToolsHandle

   
