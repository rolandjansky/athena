# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: STVFMETGetter.py 525597 2012-11-13 12:54:50Z ssnyder $
#
# This configurable can schedule the STVF MET reconstruction if it's needed
# by the D3PDMaker job.

# Gaudi/Athena import(s):
from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import AlgSequence
from RecExConfig.Configured import Configured

# D3PDMaker import(s):
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

##
# @short Class decorator for implementing the singleton pattern
#
# I just copy-pasted this code from the TauDiscriGetter code.
# The idea is the same. The STVF MET reconstruction should only be
# scheduled once into a job.
#
def singleton( cls ):

    __log = logging.getLogger( "%s::__init__" % cls.__name__ )
    __instances = {}

    def getinstance( *args, **kwargs ):

        # Check if the singleton has already been created:
        if cls in __instances:
            __log.debug( "STVF MET reconstruction already configured" )
            return __instances[ cls ]

        # Create the singleton:
        obj = cls( *args, **kwargs )
        __instances[ cls ] = obj
        return obj

    return getinstance

##
# @short Singleton class setting up the STVF MET reconstruction
#
# This class is responsible for setting up the STVF MET reconstruction for
# D3PDMaker jobs.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 525597 $
# $Date: 2012-11-13 13:54:50 +0100 (Tue, 13 Nov 2012) $
@singleton
class STVFMETGetter( Configured ):

    def __init__( self,
                  name = "STVFMETGetter",
                  sequence = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() ),
                  **kw):

        # Remember the parameter(s):
        self.__name = name
        self.__sequence = sequence
        Configured.__init__( self, **kw )

    def configure( self ):

        # Let the user know what we're doing:
        __log = logging.getLogger( "STVFMETGetter" )
        __log.info( "Configuring STVF MET reconstruction" )

        # Sort out the sequence first. This is not pretty, but is needed
        # in order to be able to use the default configurable nicely outside
        # of D3PDMaker jobs.
        topSeq = AlgSequence()
        if self.__sequence != topSeq and \
           not hasattr( topSeq, self.__sequence._name ):
            topSeq += self.__sequence
            pass

        # Turn on MET reconstruction:
        from RecExConfig.RecAlgsFlags import recAlgs
        recAlgs.doMissingET.set_Value_and_Lock( True )

        # Schedule the reconstruction of the STVF objects:
        from MissingET.METRefGetter_newplup import make_METRefAlg
        from AthenaCommon.SystemOfUnits import GeV
        METalg_STVF = make_METRefAlg( _suffix = '_STVF' )
        METalg_STVF.sequence                  = self.__sequence
        METalg_STVF.jet_JetInputCollectionKey = 'AntiKt4LCTopoJets'
        METalg_STVF.jet_JetPtCut              = 20.0 * GeV
        METalg_STVF.jet_ApplyJetScale         = "Yes"
        METalg_STVF.jet_UseJetMomentForScale  = True
        METalg_STVF.jet_JetMomentForScale     = "LCJES"
        METalg_STVF.jet_ApplyJetJVF           = "Yes"
        METalg_STVF.jet_RunSoftJetsTool       = False
        METalg_STVF.jet_calibType             = 'LocHad'
        METalg_STVF.ele_calibType             = 'RefCalib'
        METalg_STVF.gamma_calibType           = 'EmScale'
        METalg_STVF.plupSuppCorr              = 'STVF'
        METalg_STVF.celloutCorrection         = 'STVF'
        METalg_STVF.cellout_calibType         = 'Eflow'
        METalg_STVF.tau_calibType             = 'ExclRefCalib'
        METalg_STVF.cryo_ApplyCorrection      = "Off"
        METalg_STVF.muon_algorithm            = "Staco"
        METalg_STVF.muon_isolationAlg         = "dRJet"

        # Only run this if MET_RefFinal_STVF doesn't exist already.
        METalg_STVF._output = {'MissingET' : ['MET_RefFinal_STVF']}

        METalg_STVF()

        # Signal that everything went okay:
        return True
