# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from JetSimTools.JetSimToolsConf import JetTruthParticleFilter, JetTruthParticleSelectorTool

from JetRec.JetGetters import getStandardInputCollection
from JetRec.JetAlgConfiguration import checkAndUpdateOptions

from RecExConfig.Configured import Configured
from AthenaCommon.AlgSequence import AlgSequence
import logging


def getJetTruthParticleSelectorTool(toolname, **options):
    options = checkAndUpdateOptions(**options)
    selecTool = JetTruthParticleSelectorTool(toolname)
    selecTool.includeMuons = options['includeMuons']
    selecTool.useInteractingParticlesOnly = options['useInteractingOnly']
    return selecTool

class JetTruthParticleFilterGetterBase( Configured ):
    _outputType = "TruthParticleContainer"
    _outputName  = "TruthParticleJetFiltered"
    _output = { _outputType : _outputName }

    _withPileUp = False

    def __init__ (self,
                  disable=False, ignoreExistingDataObject=False, ignoreConfigError=False,
                  seq = AlgSequence(),
                  *args, **kw):
        self._seq = seq
        self.mlog =  logging.getLogger( self.__class__.__name__)        
        Configured.__init__ (self,
                             disable=disable,
                             ignoreExistingDataObject=ignoreExistingDataObject,
                             ignoreConfigError=ignoreConfigError,
                              *args, **kw)
        return


    def configure(self):
        #mlog = logging.getLogger( self.__class__.__name__+'::configure :' )
        mlog = self.mlog
        selecTool = getJetTruthParticleSelectorTool("DefaultTruthJetSelector"+ ("PU" if self._withPileUp else "") )
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += selecTool
        
        theFilter = JetTruthParticleFilter(
            TruthInputContainers = getStandardInputCollection("Truth", disable=False,doTruthPileup=self._withPileUp),
            TruthSelector = selecTool,
            OutputName = self._outputName
            )

        mlog.info ('scheduled to output %s',self.output())

        self._seq += theFilter

        self.alg = theFilter
    
        return True

    def outputName(self):
        return self._outputName

class JetTruthParticleFilterGetter(JetTruthParticleFilterGetterBase):
    _withPileUp = False

class JetTruthPileUpParticleFilterGetter(JetTruthParticleFilterGetterBase):
    _outputName  = "PileUpTruthParticleJetFiltered"
    _withPileUp = True
