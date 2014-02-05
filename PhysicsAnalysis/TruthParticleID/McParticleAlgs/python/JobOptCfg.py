# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: McParticleAlgs/python/JobOptCfg.py
# @purpose: hold a set of customized configurables and factory functions

__doc__    = "hold a set of customized configurables and factory functions"
__author__ = "Sebastien Binet <binet@cern.ch>"
__version__= "$Revision: 1.14 $"

import EventKernel.ParticleDataType
from McParticleAlgsConf import TruthParticleBuilder


class PileUpClassification(object):
    # same order as in McParticleEvent/PileUpClassification.h # PLEASE keep in sync with this header !!
    toString = ["SIGNAL", "ALL", "INTIME", "OUTOFTIME", "RESTOFMB", "CAVERN", "ALLMINBIAS" ]

    @classmethod
    def fromString(cls, putype):
        """ if putype != '' return the corresponding value, else return SIGNAL """
        if putype :
            return getattr(cls,putype)
        return cls.SIGNAL
# add attributes to PileUpClassification
for i,put in enumerate(PileUpClassification.toString):
    setattr(PileUpClassification, put,i)



class McAodBuilder( TruthParticleBuilder ):

    __slots__ = { }

    def __init__( self,
                  name = "McAodBuilder", **kwargs ):
        # have to call base init
        kwargs['name'] = name
        super( McAodBuilder, self ).__init__( **kwargs )

        from AthenaCommon import CfgMgr
        from McAodFlags import jobproperties as jp
        if jp.McAodFlags.doTruthEtIsolations():
            from AthenaCommon.AppMgr import ToolSvc
            if not hasattr(ToolSvc, 'TruthIsolationTool'):
                ToolSvc += CfgMgr.TruthIsolationTool()
        

    def setDefaults(cls, hdl):

        ## continue, only if it is our Configurable
        if not isinstance(hdl, McAodBuilder):
            return

        from McAodFlags import jobproperties as jp
        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.Configurable import Configurable
        
        if not hasattr(hdl, 'FilterTool'):
            from ParticleBuilderOptions.AODFlags import AODFlags

            ##
            ## Default configuration of the filter tool
            ## [ESD-McEventCollection --> AOD-McEventCollection]
            ##
            hdl.FilterTool = CfgMgr.EtaPtFilterTool()
            hdl.FilterTool.McEvents = AODFlags.McEventKey
            hdl.FilterTool.OnlyGenerator  = False
            hdl.FilterTool.DoEtIsolations = jp.McAodFlags.doTruthEtIsolations()
            pass

        if not hasattr(hdl, 'CnvTool'):

            ##
            ## Configuration of the converter tool
            ## [AOD-McEventCollection --> TruthParticleContainer]
            ##
            hdl.CnvTool = CfgMgr.TruthParticleCnvTool()
            hdl.CnvTool.DoEtIsolations = jp.McAodFlags.doTruthEtIsolations()
            pass
        
        if jp.McAodFlags.doTruthEtIsolations():
            mcEvtsOutName = hdl.FilterTool.properties()['McEventsOutput']
            if mcEvtsOutName == Configurable.propertyNoValue:
                mcEvtsOutName = hdl.FilterTool.getDefaultProperty('McEventsOutput')
                pass
            isolTool = hdl.FilterTool.properties()['TruthIsolationTool']
            isolTool.McEventsOutput = mcEvtsOutName


            hdl.FilterTool.TruthIsolationTool = isolTool
            hdl.CnvTool.TruthIsolationTool    = isolTool
            pass
        
        return

    pass # McAodBuilder

def createMcAodBuilder( name = "McAodBuilder",
                        inMcEvtCollection   = None,
                        outMcEvtCollection  = "GEN_AOD",
                        outTruthParticles   = "SpclMC",
                        doTruthEtIsolations = None,
                        filterTool          = None,
                        cnvTool             = None,
                        isolTool            = None,
                        **kw ):
    """
    Factory function to create a fully and correctly configured customized
    TruthParticleBuilder algorithm
    """
    from AthenaCommon import CfgMgr
    if doTruthEtIsolations is None:
        from McAodFlags import jobproperties as jp
        doTruthEtIsolations = jp.McAodFlags.doTruthEtIsolations()
        
    if filterTool is None:
        filterTool = CfgMgr.EtaPtFilterTool( OnlyGenerator=False )

    if cnvTool is None:
        cnvTool = CfgMgr.TruthParticleCnvTool()

    if inMcEvtCollection is None:
        from ParticleBuilderOptions.AODFlags import AODFlags
        inMcEvtCollection = AODFlags.McEventKey

    filterTool.McEvents       =  inMcEvtCollection
    filterTool.McEventsOutput = outMcEvtCollection
    filterTool.DoEtIsolations = doTruthEtIsolations
    
    cnvTool.DoEtIsolations       = doTruthEtIsolations
    cnvTool.McEvents             = outMcEvtCollection
    cnvTool.TruthParticlesOutput = outTruthParticles
    
    if doTruthEtIsolations:
        from AthenaCommon.AppMgr import ToolSvc
        if isolTool is None:
            isolTool = CfgMgr.TruthIsolationTool(name = "TruthIsolationTool_" + name)
            pass
        isolTool.McEventsOutput = outMcEvtCollection
        ToolSvc += isolTool
        filterTool.TruthIsolationTool = isolTool
        cnvTool.TruthIsolationTool    = isolTool

    return McAodBuilder( name,
                         FilterTool = filterTool,
                         CnvTool    = cnvTool,
                         **kw )
    
def createTruthParticlesBuilder( name = "TruthParticlesBuilder",
                                 inMcEvtCollection   = None,
                                 outTruthParticles   = "SpclMC",
                                 selectSignalType    = 0,
                                 cnvTool             = None,
                                 **kw ):
    """
    Factory function to create a fully and correctly configured customized
    TruthParticleBuilder algorithm, only producing a TruthParticleContainer
    from a McEventCollection (without any filtering involved)
    """
    from AthenaCommon import CfgMgr

    if cnvTool is None:
        cnvTool = CfgMgr.TruthParticleCnvTool()

    if inMcEvtCollection is None:
        from ParticleBuilderOptions.AODFlags import AODFlags
        inMcEvtCollection = AODFlags.McEventKey

    cnvTool.DoEtIsolations       = False
    cnvTool.McEvents             = inMcEvtCollection
    cnvTool.TruthParticlesOutput = outTruthParticles
    cnvTool.SelectSignalType     = selectSignalType
    kw['DoFiltering' ] = kw.get('DoFiltering', False)
    kw['CnvTool']      = cnvTool

    if kw['DoFiltering']:
        return McAodBuilder(name, **kw)
    
    alg = CfgMgr.TruthParticleBuilder(name, **kw)
    alg.CnvTool = cnvTool
    return alg
    






# -----------------------------------------------
# define some Getter (Configured) class for the default TruthParticleContainers
# to be used during ESD creation.
from RecExConfig.Configured import Configured
from AthenaCommon.AlgSequence import AlgSequence
from RecExConfig.ObjKeyStore import objKeyStore
import logging

class TruthParticleContainerGetter(Configured):
    _truthType    = PileUpClassification.SIGNAL
    _outputKey    = "INav4MomTruthEvent" # redefined according to _truthType
    _inputMcEvent = "TruthEvent"
    
    def __init__ (self,
                  disable=False, ignoreExistingDataObject=False, ignoreConfigError=False,
                  seq = AlgSequence(),
                  *args, **kw):        
        self._seq = seq        
        if self._truthType != PileUpClassification.SIGNAL:
            self._outputKey = self._outputKey+PileUpClassification.toString[ self._truthType ] 
        self._output = { "TruthParticleContainer" : self._outputKey } # for configured class
        Configured.__init__ (self,
                             disable=disable,
                             ignoreExistingDataObject=ignoreExistingDataObject,
                             ignoreConfigError=ignoreConfigError,
                              *args, **kw)



    def configure(self):
        builder = createTruthParticlesBuilder(
            name = self._outputKey+"Builder",
            inMcEvtCollection = self._inputMcEvent,
            outTruthParticles = self._outputKey,
            selectSignalType  = self._truthType
            )
        self._seq += builder
        mlog =  logging.getLogger( self.__class__.__name__)        
        mlog.info(" Recorded the %s ESD TruthParticle shadow collection - will be recreated on the fly "%(self.outputKey(),))
        objKeyStore.addStreamESD("TruthParticleContainer",self.outputKey())
        
        return True

    def outputKey(self):
        return self._outputKey

class TruthParticleContainerGetterSIGNAL(TruthParticleContainerGetter):
    # can not invoke directly the base class otherwise, derived class may not work !
    pass
class TruthParticleContainerGetterINTIME(TruthParticleContainerGetter):
    _truthType = PileUpClassification.INTIME
class TruthParticleContainerGetterOUTOFTIME(TruthParticleContainerGetter):
    _truthType = PileUpClassification.OUTOFTIME
class TruthParticleContainerGetterRESTOFMB(TruthParticleContainerGetter):
    _truthType = PileUpClassification.RESTOFMB
class TruthParticleContainerGetterCAVERN(TruthParticleContainerGetter):
    _truthType = PileUpClassification.CAVERN

def invokeTruthParticleGetter(puType, disable=False, seq=None):
    """Helper fonction to invoke the right TruthParticleContainerGetter for puType """
    getter = { PileUpClassification.SIGNAL     : TruthParticleContainerGetterSIGNAL,
               PileUpClassification.INTIME     : TruthParticleContainerGetterINTIME,
               PileUpClassification.OUTOFTIME  : TruthParticleContainerGetterOUTOFTIME,
               PileUpClassification.RESTOFMB   : TruthParticleContainerGetterRESTOFMB,
               PileUpClassification.CAVERN     : TruthParticleContainerGetterCAVERN,
               }[puType]
    if seq is None: seq = AlgSequence()
    return getter(disable,seq=seq)
