# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloClusterGetterBase.py,v 1.9 2008-02-25 22:23:19 droussea Exp $
#
# File: CaloRec/python/CaloClusterGetterBase.py
# Created: Mar 2007, sss
# Purpose: Base class for calorimeter cluster Getter's.
#

from AthenaCommon.Logging     import logging
from RecExConfig.Configured   import Configured
from CaloRec.CaloRecMakers    import make_CaloClusterMaker
from RecExConfig.ObjKeyStore  import objKeyStore
from AthenaCommon.AlgSequence import AlgSequence
import traceback


#
# This class factors out the common behavior for Getter's
# for calorimeter clustering algorithms.  Derive from this to produce
# the concrete Getter's.  In the derived classes, you should
# define the following class variables:
#
#   _name:        The name of the algorithm being constructed.
#   _outputKey:   The StoreGate key for the cluster container which
#                 the algorithm will produce.
#   _inputGetter: Name of the Getter class producing the input
#                 for this clustering algorithm.  Should be a string
#                 of the form "MODULE.NAME".
#   _towrite:     Cluster is to be written to ESD/AOD.
#
# Optionally, _keepEachCorrection may be set to True to save results
# after each correction is applied.
#
# In addition, the derived class must define two methods:
#
#   def makeMakerTools (self, input_key):
#     This method should return a list of the maker tools for this
#     clustering algorithm.  INPUT_KEY is the key from the specified
#     _inputGetter.
#
#   def makeCorrectionTools (self):
#     This method should return a list of the correction tools for this
#     clustering algorithm.
#
class CaloClusterGetterBase (Configured):
    # The primary output class we produce.
    _outputType = "xAOD::CaloClusterContainer"

    # This defaults to false.
    _keepEachCorrection = False

    def __init__ (self, seq = AlgSequence(), *args, **kw):
        self.seq = seq
        Configured.__init__ (self, *args, **kw)
        return

    def configure (self):
        # Make a logger.
        clsname = self.__class__.__name__
        mlog = logging.getLogger ('%s.configure' % clsname)

        input_getter = self.getInputGetter (self._inputGetter)

        # Create the configurable.
        try:
            maker = make_CaloClusterMaker (\
                self._name,
                self._outputKey,
                self._keepEachCorrection,
                maker_tools = self.makeMakerTools (input_getter.outputKey()),
                correction_tools = self.makeCorrectionTools ())
        except:
            mlog.error("could not construct CaloClusterMaker Quit")
            print traceback.format_exc()
            return False

        self._handle = maker

        objKeyStore.addManyTypesTransient(self.output())
        if self._towrite:
            # register output in objKeyStore
            # write everything in ESD
            # 2014-01-15 W.L. Remove objs from output streams  b/c of xAOD migration

            #objKeyStore.addManyTypesStreamESD(self.output())
            ## only write cluster class in AOD but not the LArClusterEM (only the eg one goes)
            #if not self.outputKey().startswith("LArClusterEM"):
            #    objKeyStore.addStreamAOD(self.outputType(),self.outputKey())
            pass
        self.seq += maker

        return True

    def caloClusterMakerHandle(self):
        return self._handle

    @classmethod
    def outputKey(cls):
        return cls.output()[cls._outputType]

    @classmethod
    def outputType(cls):
        return cls._outputType

    def handle(self):
        return self._handle

    @classmethod
    def output (cls):
        if cls._output == {}:
            # The full list of output objects that we produce.
            cls._output = { cls._outputType : cls._outputKey ,
                            "CaloClusterAuxContainer" : cls._outputKey+"Aux.",
                            "CaloClusterCellLinkContainer" : cls._outputKey+"_links"}
        return cls._output

