# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# System import(s):
import copy
import unittest
import re

# ATLAS import(s):
from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.DualUseConfig import createAlgorithm

class AnaAlgSequence( AlgSequence ):
    """Analysis algorithm sequence

    This is a thin layer above a generic algorithm sequence, which helps
    with setting up the analysis algorithms for the job.

    Note that this class is specifically meant for setting up the algorithms
    provided centrally for analysis. The private analysis algorithms of the
    users do not need to use this sequence type.
    """

    def __init__( self, name = "AnalysisSequence" ):
        """Analysis algorithm sequence constructor

        Nothing special, it just initialises the base class, and taking the
        name of the input container of the sequence.

        Keyword arguments:
          name -- The name of the analysis algorithm sequence
        """

        # Initialise the base class:
        super( AnaAlgSequence, self ).__init__( name )

        # Set up the sequence's member variables:
        self._inputPropNames = []
        self._outputPropNames = []
        self._affectingSystematics = []

        return

    def configure( self, inputName, outputName ):
        """Perform a post-configuration on the analysis algorithm sequence

        This function needs to be called once the sequence is configured to
        hold the right algorithms, in the right order. It sets the I/O
        properties of the algorithms to make sure that:
          - The first one takes the object/container specified
          - The output of one algorithm is picked up correctly by the next
          - The last one outputs the object/container specified

        Keyword arguments:
          inputName  -- The name of the input object/container to process
          outputName -- The name of the output object/container to produce
        """

        # Make sure that all internal variables are of the same size:
        nAlgs = len( self )
        if len( self._inputPropNames ) != nAlgs or \
                len( self._outputPropNames ) != nAlgs or \
                len( self._affectingSystematics ) != nAlgs:
            raise RuntimeError( 'Analysis algorithm sequence is in an ' \
                                'inconsistent state' )

        if isinstance( inputName, dict ):
            inputNameDict = inputName
        else:
            inputNameDict = { "default" : inputName }
            pass
        if isinstance( outputName, dict ):
            outputNameDict = outputName
        else:
            outputNameDict = { "default" : outputName }
            pass

        # Iterate over the algorithms:
        currentInputs = copy.deepcopy( inputNameDict )
        inputRegex = {}
        tmpIndex = {}
        systematicsUsed = False
        for alg, inName, outName, syst in zip( self, self._inputPropNames,
                                               self._outputPropNames,
                                               self._affectingSystematics ):

            # If there is no input defined for the algorithm (because it may
            # be a public tool), then skip doing anything with it:
            if not inName:
                continue

            # Set the input name(s):
            for inputLabel, inputPropName in inName.iteritems():
                setattr( alg, inputPropName, currentInputs[ inputLabel ] )
                if inputLabel not in inputRegex.keys():
                    inputRegex[ inputLabel ] = '(^$)'
                    pass
                setattr( alg, inputPropName + 'Regex', inputRegex[ inputLabel ] )
                pass

            # Set up the output name(s):
            if outName:
                for outputLabel, outputPropName in outName.iteritems():
                    if outputLabel not in tmpIndex.keys():
                        tmpIndex[ outputLabel ] = 1
                        pass
                    if outputLabel in inputNameDict.keys():
                        currentInputs[ outputLabel ] = \
                          '%s_tmp%i_%%SYS%%' % ( inputNameDict[ outputLabel ],
                                                     tmpIndex[ outputLabel ] )
                    elif outputLabel in outputNameDict.keys():
                        currentInputs[ outputLabel ] = \
                          '%s_tmp%i_%%SYS%%' % ( outputNameDict[ outputLabel ],
                                                     tmpIndex[ outputLabel ] )
                    else:
                        currentInputs[ outputLabel ] = \
                          '%s_tmp%i_%%SYS%%' % ( outputLabel,
                                                     tmpIndex[ outputLabel ] )
                        pass

                    tmpIndex[ outputLabel ] += 1
                    setattr( alg, outputPropName, currentInputs[ outputLabel ] )

                    # Make sure that this (possibly intermediate) label is known
                    # in the regex dictionary.
                    if outputLabel not in inputRegex.keys():
                        inputRegex[ outputLabel ] = '(^$)'
                        pass

                    # Assume that the variation on *all* of the inputs affect
                    # all of the outputs.
                    for label in inName.keys():
                        if label in inputRegex.keys():
                            # If it starts with '(^$)' (it should), then remove
                            # that for the following operations.
                            pattern = inputRegex[ label ]
                            if pattern.find( '(^$)|' ) == 0:
                                pattern = pattern[ 5 : ]
                                pass
                            # Check that we don't have this already in the
                            # regular expression.
                            if inputRegex[ outputLabel ].find( pattern ) == -1:
                                # If we don't, then let's add it now.
                                inputRegex[ outputLabel ] += '|%s' % pattern
                                pass
                            pass
                        pass

                    # And of course variations applied by this algorithm itself
                    # do affect all outputs.
                    if syst and outputLabel in syst.keys():
                        inputRegex[ outputLabel ] += '|%s' % syst[ outputLabel ]
                        pass

                    pass
                pass

            # Set up the systematic behaviour of the algorithm:
            if syst:
                alg.systematicsRegex = '|'.join( syst.values() )
                systematicsUsed = True
                pass

            pass

        # Add a '_%SYS%' postfix to the output name(s) if the user didn't put
        # this string into the name somewhere. In case the sequence uses
        # systematic variations...
        if systematicsUsed:
            for outputKey in outputNameDict.keys():
                if not '%SYS%' in outputNameDict[ outputKey ]:
                    outputNameDict[ outputKey ] += '_%SYS%'
                    pass
                pass
            pass

        # Set the output name(s) of the last algorithm (that provides output)
        # to the requested value:
        currentOutputs = copy.deepcopy( outputNameDict )
        for alg, inName, outName in reversed( zip( self, self._inputPropNames,
                                                   self._outputPropNames ) ):

            # Stop the loop if we're already done.
            if len( currentOutputs ) == 0:
                break

            # If the algorithm has (an) output(s), set them up appropriately.
            # Remembering which "final" output still needs to be set.
            if outName:
                for outputLabel, outputKey in outName.iteritems():
                    if outputLabel in currentOutputs.keys():
                        setattr( alg, outputKey, currentOutputs[ outputLabel ] )
                        del currentOutputs[ outputLabel ]
                        pass
                    pass
                pass

            # Set up the input name(s) of the algorithm correctly, in case this
            # is needed...
            for inputLabel, inputKey in inName.iteritems():
                if inputLabel in currentOutputs.keys():
                    setattr( alg, inputKey, currentOutputs[ inputLabel ] )
                    pass
                pass

            pass

        return

    def append( self, alg, inputPropName, outputPropName = None,
                affectingSystematics = None ):
        """Add one analysis algorithm to the sequence

        This function is specifically meant for adding one of the centrally
        provided analysis algorithms to the sequence. When users want to add
        a "simple" algorithm to a sequence of their own, they should be using
        the functions of 'AnaAlgorithm.AlgSequence' for doing so.

        Keyword arguments:
          alg -- The algorithm to add (an Athena configurable, or an
                 EL::AnaAlgorithmConfig instance)
          inputPropName -- The name of the property setting the input
                           object/container name for the algorithm
          outputPropName -- The name of the property setting the output
                            object/container name for the algorithm [optional]
          affectingSystematics -- Regular expression describing which systematic
                                  variations would affect this algorithm's
                                  behaviour [optional]
        """

        self += alg
        if isinstance( inputPropName, dict ):
            self._inputPropNames.append( inputPropName )
        else:
            if inputPropName:
                self._inputPropNames.append( { "default" : inputPropName } )
            else:
                self._inputPropNames.append( None )
                pass
            pass
        if isinstance( outputPropName, dict ):
            self._outputPropNames.append( outputPropName )
        else:
            if outputPropName:
                self._outputPropNames.append( { "default" : outputPropName } )
            else:
                self._outputPropNames.append( None )
                pass
            pass
        if isinstance( affectingSystematics, dict ):
            self._affectingSystematics.append( affectingSystematics )
        else:
            if affectingSystematics:
                self._affectingSystematics.append( { "default" : affectingSystematics } )
            else:
                self._affectingSystematics.append( None )
                pass
            pass

        return

    def addPublicTool( self, tool ):
        """Add a public tool to the job

        This function is here to provide a uniform interface with which
        analysis algorithm sequences can declare the public tools that they
        need. In Athena mode the function doesn't do anything. But in EventLoop
        mode it remembers the EL::AnaAlgorithmConfig object that it receives,
        which describes the public tool.

        Keyword arguments:
           tool -- The tool object to add to the sequence/job
        """

        try:
            # Try to access the ToolSvc, to see whethet we're in Athena mode:
            from AthenaCommon.AppMgr import ToolSvc
        except ImportError:

            # We're not, so let's remember this as a "normal" algorithm:
            self.append( tool, inputPropName = None )
            pass
        return

    pass

#
# Declare some unit tests for the code
#

## Test case for a sequence handling a single container
class TestAnaAlgSeqSingleContainer( unittest.TestCase ):

    ## Set up the sequence that we'll test.
    def setUp( self ):
        self.seq = AnaAlgSequence( 'SingleContainerSeq' )
        alg = createAlgorithm( 'CalibrationAlg', 'Calibration' )
        self.seq.append( alg, inputPropName = 'electrons',
                         outputPropName = 'electronsOut',
                         affectingSystematics = '(^EL_.*)' )
        alg = createAlgorithm( 'SelectionAlg', 'Selection' )
        self.seq.append( alg, inputPropName = 'particles',
                         outputPropName = 'particlesOut' )
        alg = createAlgorithm( 'EfficiencyAlg', 'Efficiency' )
        self.seq.append( alg, inputPropName = 'egammas',
                         outputPropName = 'egammasOut',
                         affectingSystematics = '(^EG_.*)' )
        self.seq.configure( inputName = 'Electrons',
                            outputName = 'AnalysisElectrons_%SYS%' )
        return

    ## Test the input/output containers set up for the sequence.
    def test_inputAndOutput( self ):
        self.assertEqual( self.seq.Calibration.electrons, 'Electrons' )
        self.assertEqual( self.seq.Efficiency.egammasOut,
                          'AnalysisElectrons_%SYS%' )
        return

    ## Test the various regular expressions set up for the algorithms of the
    ## sequence.
    def test_regularExpressions( self ):
        self.assertEqual( self.seq.Calibration.electronsRegex, '(^$)' )
        self.assertEqual( self.seq.Calibration.systematicsRegex, '(^EL_.*)' )
        self.assertEqual( self.seq.Selection.particlesRegex, '(^$)|(^EL_.*)' )
        self.assertEqual( self.seq.Efficiency.systematicsRegex, '(^EG_.*)' )
        return

    pass

## Test case for a sequence receiving multiple containers, and producing just
## one.
class TestAnaAlgSeqMultiInputContainer( unittest.TestCase ):

    ## Set up the sequence that we'll test.
    def setUp( self ):
        self.seq = AnaAlgSequence( 'MultiInputContainerSeq' )
        alg = createAlgorithm( 'SelectionAlg', 'ElectronSelection' )
        self.seq.append( alg, inputPropName = { 'electrons' : 'particles' },
                         outputPropName = { 'electrons' : 'particlesOut' },
                         affectingSystematics = { 'electrons' :
                                                  '(^EL_.*)|(^EG_.*)' } )
        alg = createAlgorithm( 'SelectionAlg', 'MuonSelection' )
        self.seq.append( alg, inputPropName = { 'muons' : 'particles' },
                         outputPropName = { 'muons' : 'particlesOut' },
                         affectingSystematics = { 'muons' : '(^MU_.*)' } )
        alg = createAlgorithm( 'ZCreatorAlg', 'ElectronZCreator' )
        self.seq.append( alg, inputPropName = { 'electrons' : 'particles' },
                         outputPropName = { 'electronZs' : 'zCandidates' } )
        alg = createAlgorithm( 'ZCreatorAlg', 'MuonZCreator' )
        self.seq.append( alg, inputPropName = { 'muons' : 'particles' },
                         outputPropName = { 'muonZs' : 'zCandidates' } )
        alg = createAlgorithm( 'ZCombinerAlg', 'ZCombiner' )
        self.seq.append( alg, inputPropName = { 'electronZs' : 'container1',
                                                'muonZs'     : 'container2' },
                         outputPropName = { 'Zs' : 'output' } )
        alg = createAlgorithm( 'ZCalibratorAlg', 'ZCalibrator' )
        self.seq.append( alg, inputPropName = { 'Zs' : 'input' },
                         outputPropName = 'output' )
        self.seq.configure( inputName = { 'electrons' : 'AnalysisElectrons_%SYS%',
                                          'muons'     : 'AnalysisMuons_%SYS%' },
                            outputName = 'ZCandidates_%SYS%' )
        return

    ## Test the input/output containers set up for the sequence.
    def test_inputAndOutput( self ):
        self.assertEqual( self.seq.ElectronSelection.particles,
                          'AnalysisElectrons_%SYS%' )
        self.assertEqual( self.seq.MuonSelection.particles,
                          'AnalysisMuons_%SYS%' )
        self.assertEqual( self.seq.ZCalibrator.output,
                          'ZCandidates_%SYS%' )
        return

    ## Test the various regular expressions set up for the algorithms of the
    ## sequence.
    def test_regularExpressions( self ):
        self.assertEqual( self.seq.ElectronSelection.particlesRegex, '(^$)' )
        self.assertEqual( self.seq.MuonSelection.particlesRegex, '(^$)' )
        self.assertEqual( self.seq.ElectronZCreator.particlesRegex,
                          '(^$)|(^EL_.*)|(^EG_.*)' )
        self.assertEqual( self.seq.MuonZCreator.particlesRegex,
                          '(^$)|(^MU_.*)' )
        self.assertEqual( self.seq.ZCombiner.container1Regex,
                          '(^$)|(^EL_.*)|(^EG_.*)' )
        self.assertEqual( self.seq.ZCombiner.container2Regex,
                          '(^$)|(^MU_.*)' )
        self.assertEqual( self.seq.ZCalibrator.inputRegex,
                          '(^$)|(^MU_.*)|(^EL_.*)|(^EG_.*)' )
        return

## Test case for a sequence starting from a single container, producing
## multiple ones.
class TestAnaAlgSeqMultiOutputContainer( unittest.TestCase ):

    ## Set up the sequence that we'll test.
    def setUp( self ):
        self.seq = AnaAlgSequence( 'MultiOutputContainerSeq' )
        alg = createAlgorithm( 'CalibrationAlg', 'Calibration' )
        self.seq.append( alg, inputPropName = 'particles',
                         outputPropName = 'particlesOut',
                         affectingSystematics = '(^EL_.*)' )
        alg = createAlgorithm( 'ParticleSplitterAlg', 'ParticleSplitter' )
        self.seq.append( alg, inputPropName = 'particles',
                         outputPropName = { 'goodObjects' : 'goodParticles',
                                            'badObjects' : 'badParticles' },
                         affectingSystematics = { 'goodObjects' : '(^FOO_.*)',
                                                  'badObjects' : '(^BAR_.*)' } )
        alg = createAlgorithm( 'ParticleTrimmerAlg', 'GoodParticleTrimmer' )
        self.seq.append( alg, inputPropName = { 'goodObjects' : 'particles' },
                         outputPropName = { 'goodObjects' : 'particlesOut' } )
        alg = createAlgorithm( 'ParticleTriggerAlg', 'BadParticleTrimmer' )
        self.seq.append( alg, inputPropName = { 'badObjects' : 'particles' },
                         outputPropName = { 'badObjects' : 'particlesOut' } )
        self.seq.configure( inputName = 'Electrons',
                            outputName = { 'goodObjects' : 'GoodElectrons_%SYS%',
                                           'badObjects' : 'BadElectrons_%SYS%' } )
        return

    ## Test the input/output containers set up for the sequence.
    def test_inputAndOutput( self ):
        self.assertEqual( self.seq.Calibration.particles, 'Electrons' )
        self.assertEqual( self.seq.GoodParticleTrimmer.particlesOut,
                          'GoodElectrons_%SYS%' )
        self.assertEqual( self.seq.BadParticleTrimmer.particlesOut,
                          'BadElectrons_%SYS%' )
        return

    ## Test the various regular expressions set up for the algorithms of the
    ## sequence.
    def test_regularExpressions( self ):
        self.assertEqual( self.seq.Calibration.particlesRegex, '(^$)' )
        self.assertEqual( self.seq.ParticleSplitter.particlesRegex,
                          '(^$)|(^EL_.*)' )
        self.assertEqual( self.seq.GoodParticleTrimmer.particlesRegex,
                          '(^$)|(^EL_.*)|(^FOO_.*)' )
        self.assertEqual( self.seq.BadParticleTrimmer.particlesRegex,
                          '(^$)|(^EL_.*)|(^BAR_.*)' )
        return

## Test case for a sequence starting from multiple containers, and producing
## multiple new ones.
class TestAnaAlgSeqMultiInputOutputContainer( unittest.TestCase ):

    ## Set up the sequence that we'll test.
    def setUp( self ):
        self.seq = AnaAlgSequence( 'MultiInputOutputContainerSeq' )
        alg = createAlgorithm( 'ElectronSelectionAlg', 'ElectronSelection' )
        self.seq.append( alg, inputPropName = { 'electrons' : 'particles' },
                         outputPropName = { 'electrons' : 'particlesOut' },
                         affectingSystematics = { 'electrons' : '(^EL_.*)' } )
        alg = createAlgorithm( 'MuonSelectionAlg', 'MuonSelection' )
        self.seq.append( alg, inputPropName = { 'muons' : 'particles' },
                         outputPropName = { 'muons' : 'particlesOut' },
                         affectingSystematics = { 'muons' : '(^MU_.*)' } )
        alg = createAlgorithm( 'OverlapRemovalAlg', 'OverlapRemoval' )
        self.seq.append( alg, inputPropName = { 'electrons' : 'electrons',
                                                'muons' : 'muons' },
                         outputPropName = { 'electrons' : 'electronsOut',
                                            'muons' : 'muonsOut' } )
        self.seq.configure( inputName = { 'electrons' : 'AnalysisElectrons_%SYS%',
                                          'muons' : 'AnalysisMuons_%SYS%' },
                            outputName = { 'electrons' : 'FinalElectrons_%SYS%',
                                           'muons' : 'FinalMuons_%SYS%' } )
        return

    ## Test the input/output containers set up for the sequence.
    def test_inputAndOutput( self ):
        self.assertEqual( self.seq.ElectronSelection.particles,
                          'AnalysisElectrons_%SYS%' )
        self.assertEqual( self.seq.MuonSelection.particles,
                          'AnalysisMuons_%SYS%' )
        self.assertEqual( self.seq.OverlapRemoval.electronsOut,
                          'FinalElectrons_%SYS%' )
        self.assertEqual( self.seq.OverlapRemoval.muonsOut,
                          'FinalMuons_%SYS%' )
        return

    ## Test the various regular expressions set up for the algorithms of the
    ## sequence.
    def test_regularExpressions( self ):
        self.assertEqual( self.seq.ElectronSelection.particlesRegex, '(^$)' )
        self.assertEqual( self.seq.MuonSelection.particlesRegex, '(^$)' )
        self.assertEqual( self.seq.OverlapRemoval.electronsRegex,
                          '(^$)|(^EL_.*)' )
        self.assertEqual( self.seq.OverlapRemoval.muonsRegex, '(^$)|(^MU_.*)' )
        return
