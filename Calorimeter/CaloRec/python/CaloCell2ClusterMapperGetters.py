# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloCell2ClusterMapperGetters.py,v 1.7 2008-09-12 09:08:56 menke Exp $
#
# File: CaloRec/python/caloCell2ClusterMapperGetters.py
# Created: Mar 2007, sss
# Purpose: Create Getter's for CaloCell2ClusterMapper
#


from AthenaCommon.Logging    import logging
from RecExConfig.Configured  import Configured
from CaloRec.CaloRecMakers   import make_CaloCell2ClusterMapper
from RecExConfig.ObjKeyStore import objKeyStore
import traceback


class CaloCell2ClusterMapperGetterBase (Configured):
    # The primary output class we produce.
    _outputType = "CaloCell2ClusterMap"

    # Must be set
    #_name = "EMCell2TopoClusterMapper"
    #_inputGetter = "CaloRec.CaloClusterTopoEMGetters.CaloClusterTopoEMGetter633"
    #_outputKey = "EMCell2TopoCluster"

    def configure (self):
        # Make a logger.
        clsname = self.__class__.__name__
        mlog = logging.getLogger ('%s.configure' % clsname)

        input_getter = self.getInputGetter (self._inputGetter)
        if input_getter == None: return False

        # Create the configurable.
        try:
            self._handle = \
                     make_CaloCell2ClusterMapper (self._name,
                                                  input_getter.outputKey(),
                                                  self._outputKey)
        except:
            mlog.error("could not construct CaloCell2ClusterMapper Quit")
            print traceback.format_exc()
            return False

        # register output in objKeyStore
        objKeyStore.addTransient (self.outputType(), self.outputKey())

        # now add algorithm to topSequence
        # this should always come at the end
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        topSequence += self._handle

        return True

    def handle(self):
        return self._handle

    def outputKey(self):
        return self._outputKey

    def outputType(self):
        return self._outputType

    def output(self):
        if self._output == {}:
            self._output = {self._outputType : self._outputKey}
        return self._output

def make_CaloCell2ClusterMapperGetter (name,
                                       inputGetter,
                                       outputKey):
    cls = type (name + "MapperGetter", (CaloCell2ClusterMapperGetterBase,), {})
    cls._name = name
    cls._inputGetter = inputGetter
    cls._outputKey = outputKey
    return cls

def _make_mappergetter (name, input_getter):
    g = make_CaloCell2ClusterMapperGetter (name, input_getter, name)
    globals()[g.__name__] = g
    return g

_igprefix = "CaloRec.CaloClusterTopoEMGetters.CaloClusterTopoEM"
_make_mappergetter ("EMCell2TopoCluster",   _igprefix + "633Getter")
_make_mappergetter ("EMCell2TopoCluster630",_igprefix + "630Getter")
_make_mappergetter ("EMCell2TopoCluster422",_igprefix + "422Getter")
_make_mappergetter ("EMCell2TopoCluster420",_igprefix + "420Getter")
_make_mappergetter ("EMCell2TopoCluster420Lowcut",_igprefix +"420LowcutGetter")
_make_mappergetter ("EMCell2TopoCluster430",_igprefix + "430Getter")
_make_mappergetter ("CaloCell2TopoCluster",  "CaloRec.CaloClusterTopoGetter.CaloClusterTopoGetter")
