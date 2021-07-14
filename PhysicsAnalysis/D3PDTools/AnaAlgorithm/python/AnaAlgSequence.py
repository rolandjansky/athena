# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# System import(s):
import copy
import unittest
import re

# ATLAS import(s):
from AnaAlgorithm.AlgSequence import AlgSequence
from AnaAlgorithm.AnaAlgorithmMeta import AnaAlgorithmMeta
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
        self._algorithmMeta = []
        self._metaConfigDefault = {}

        return

    def configure( self, inputName, outputName,
                   hiddenLayerPrefix = "" ):
        """Perform a post-configuration on the analysis algorithm sequence

        This function needs to be called once the sequence is configured to
        hold the right algorithms, in the right order, with the right settings.
        It sets the I/O properties of the algorithms to make sure that they
        receive the input object(s) specified, and produce the output object(s)
        requested.

        The arguments can either be simple string names, or dictionaries in the
        form of { "type" : "key", ... }. The latter is used to describe multiple
        inputs/outputs to/from a sequence. See the descriptions of the various
        analysis algorithm sequence setup functions on how their created
        sequences should be configured by this function.

        Keyword arguments:
          inputName  -- The name(s) of the input object(s)/container(s) to
                        process
          outputName -- The name(s) of the output object(s)/container(s) to
                        produce
          hiddenLayerPrefix -- Possible unique string prefix for
                               object(s)/container(s) in "hidden layers" of the
                               algorithm sequence. To avoid name clashes when
                               scheduling multiple instances of the sequence.
        """

        # Make sure that all internal variables are of the same size:
        nAlgs = len( self )
        if len( self._algorithmMeta ) != nAlgs:
            raise RuntimeError( 'Analysis algorithm sequence is in an ' \
                                'inconsistent state' )

        # do the dynamic configuration based on meta-information
        metaConfig = {}
        for name, value in self._metaConfigDefault.items() :
            metaConfig[name] = value[:]
            pass
        for alg, meta in zip( self, self._algorithmMeta ):
            for var, func in meta.dynConfig.items() :
                # if this is a subtool, find the subtool
                obj = alg
                while '.' in var :
                    obj = getattr (alg, var[:var.find('.')])
                    var = var[var.find('.')+1:]
                    pass
                # set the property on the algorithm/tool
                setattr (obj, var, func (metaConfig))
                pass
            for name, value in meta.metaConfig.items() :
                if not name in metaConfig :
                    raise RuntimeError ("metaConfig value " + name + " for algorithm " + alg.name() + " not registered, did you forget to call addMetaConfigDefault?")
                metaConfig[name] += value[:]
                pass
            pass

        # Make the inputs and outputs dictionaries. Allowing simple sequences to
        # be configured using simple string names.
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
        tmpIndex = {}
        systematicsUsed = False
        for alg, meta in zip( self, self._algorithmMeta ):

            # If there is no input defined for the algorithm (because it may
            # be a public tool), then skip doing anything with it:
            if not meta.inputPropName:
                continue

            # Set the input name(s):
            for inputLabel, inputPropName in meta.inputPropName.iteritems():
                if not inputLabel in currentInputs.keys():
                    continue
                setattr( alg, inputPropName, currentInputs[ inputLabel ] )
                pass

            # Set up the output name(s):
            if meta.outputPropName:

                # Loop over the outputs of the algorithm.
                for outputLabel, outputPropName in meta.outputPropName.iteritems():
                    if outputLabel not in tmpIndex.keys():
                        tmpIndex[ outputLabel ] = 1
                        pass
                    if outputLabel in outputNameDict.keys():
                        currentInputs[ outputLabel ] = \
                          '%s_tmp%i' % ( outputNameDict[ outputLabel ],
                                         tmpIndex[ outputLabel ] )
                    else:
                        currentInputs[ outputLabel ] = \
                          '%s%s_tmp%i' % ( hiddenLayerPrefix, outputLabel,
                                           tmpIndex[ outputLabel ] )
                        pass

                    tmpIndex[ outputLabel ] += 1
                    setattr( alg, outputPropName, currentInputs[ outputLabel ] )

                    pass
                pass

            pass

        # Set the output name(s) of the last algorithm (that provides output)
        # to the requested value:
        currentOutputs = copy.deepcopy( outputNameDict )
        for alg, meta in reversed( zip( self, self._algorithmMeta ) ):

            # Stop the loop if we're already done.
            if len( currentOutputs ) == 0:
                break

            # If the algorithm has (an) output(s), set them up appropriately.
            # Remembering which "final" output still needs to be set.
            if meta.outputPropName:
                for outputLabel, outputKey in meta.outputPropName.iteritems():
                    if outputLabel in currentOutputs.keys():
                        setattr( alg, outputKey, currentOutputs[ outputLabel ] )
                        del currentOutputs[ outputLabel ]
                        pass
                    pass
                pass

            # Set up the input name(s) of the algorithm correctly, in case this
            # is needed...
            for inputLabel, inputKey in meta.inputPropName.iteritems():
                if inputLabel in currentOutputs.keys():
                    setattr( alg, inputKey, currentOutputs[ inputLabel ] )
                    pass
                pass

            pass

        return

    def append( self, alg, inputPropName, outputPropName = None,
                stageName = 'undefined',
                metaConfig = {},
                dynConfig = {}):
        """Add one analysis algorithm to the sequence

        This function is specifically meant for adding one of the centrally
        provided analysis algorithms to the sequence.

        Keyword arguments:
          alg -- The algorithm to add (an Athena configurable, or an
                 EL::AnaAlgorithmConfig instance)
          inputPropName -- The name of the property setting the input
                           object/container name for the algorithm
          outputPropName -- The name of the property setting the output
                            object/container name for the algorithm [optional]
          stageName -- name of the current processing stage [optional]
        """

        meta = AnaAlgorithmMeta( stageName=stageName, inputPropName=inputPropName, outputPropName=outputPropName, metaConfig=metaConfig, dynConfig=dynConfig )
        self += alg
        self._algorithmMeta.append( meta )
        return self

    def insert( self, index, alg, inputPropName, outputPropName = None,
                stageName = 'undefined',
                metaConfig = {},
                dynConfig = {} ):
        """Insert one analysis algorithm into the sequence

        This function is specifically meant for adding one of the centrally
        provided analysis algorithms to the sequence, in a user defined
        location.

        Keyword arguments:
          index -- The index to insert the algorithm at
          alg -- The algorithm to add (an Athena configurable, or an
                 EL::AnaAlgorithmConfig instance)
          inputPropName -- The name of the property setting the input
                           object/container name for the algorithm
          outputPropName -- The name of the property setting the output
                            object/container name for the algorithm [optional]
          stageName -- name of the current processing stage [optional]
        """

        meta = AnaAlgorithmMeta( stageName=stageName, inputPropName=inputPropName, outputPropName=outputPropName, metaConfig=metaConfig, dynConfig=dynConfig )
        super( AnaAlgSequence, self ).insert( index, alg )
        self._algorithmMeta.insert( index, meta )
        return self

    def addPublicTool( self, tool, stageName = 'undefined' ):
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
            self.append( tool, inputPropName = None, stageName = stageName )
            pass
        return

    def __delattr__( self, name ):
        """Remove one algorithm/sequence from this sequence, by name

        This is to allow removing algorithms (or even sequences) from this
        sequence in case that would be needed.

        Keyword arguments:
        name -- The name of the algorithm/sequence to delete from the
        sequence
        """

        # Figure out the algorithm's index:
        algIndex = -1
        index = 0
        for alg in self:
            if alg.name() == name:
                algIndex = index
                break
            index += 1
            pass

        # Check if we were successful:
        if algIndex == -1:
            raise AttributeError( 'Algorithm/sequence with name "%s" was not ' \
                                  'found' % name )
        
        # Remove the element from the base class:
        super( AnaAlgSequence, self ).__delattr__( name )

        # Now remove the elements from the member lists of this class:
        del self._algorithmMeta[ algIndex ]
        pass

    def removeStage( self, stageName ):
        """Remove all algorithms for the given stage

        Keyword arguments:
          stageName -- name of the processing stage to remove
        """

        if not stageName in self.allowedStageNames() :
            raise ValueError ('unknown stage name ' + stageName + ' allowed stage names are ' + ', '.join(self.allowedStageNames()))

        # safety check that we actually know the stages of all
        # algorithms
        if stageName != "undefined" :
            for meta in self._algorithmMeta :
                if meta.stageName == "undefined" :
                    raise ValueError ("can not remove stages from an algorithm sequence if some algorithms belong to an undefined stage")
                pass
            pass

        names = []
        for alg in self:
            names.append (alg.name())
            pass
        iter = 0
        while iter < len( self ):
            if self._algorithmMeta[iter].stageName == stageName :
                super( AnaAlgSequence, self ).__delattr__( names[iter] )
                del names[iter]
                del self._algorithmMeta[ iter ]
                pass
            else :
                iter = iter + 1
                pass
            pass
        pass



    def addMetaConfigDefault (self, name, value) :
        """add a default value for the given meta-configuration entry

        This will both register name as a valid meta-configuration
        value and set its default value, or add to its default value,
        if that name is already known."""

        if name in self._metaConfigDefault :
            self._metaConfigDefault[name] += value
            pass
        else :
            self._metaConfigDefault[name] = value
            pass
        pass



    def getMetaConfig (self, name) :
        """get the value for the given meta-configuration entry"""

        if not name in self._metaConfigDefault :
            raise RuntimeError ("metaConfig value " + name + " not registered, did you forget to call addMetaConfigDefault?")
        result = self._metaConfigDefault[name][:]
        for meta in self._algorithmMeta :
            if name in meta.metaConfig :
                result += meta.metaConfig[name]
                pass
            pass
        return result



    @staticmethod
    def allowedStageNames():
        return AnaAlgorithmMeta.allowedStageNames ()

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
                         stageName = 'calibration' )
        alg = createAlgorithm( 'EfficiencyAlg', 'Efficiency' )
        self.seq.append( alg, inputPropName = 'egammas',
                         outputPropName = 'egammasOut',
                         stageName = 'efficiency' )
        alg = createAlgorithm( 'SelectionAlg', 'Selection' )
        self.seq.insert( 1, alg, inputPropName = 'particles',
                         outputPropName = 'particlesOut',
                         stageName = 'selection' )
        alg = createAlgorithm( 'DummyAlgorithm', 'Dummy' )
        self.seq.append( alg, inputPropName = None )
        del self.seq.Dummy
        self.seq.configure( inputName = 'Electrons',
                            outputName = 'AnalysisElectrons_%SYS%' )
        return

    ## Test some very basic properties of the sequence.
    def test_basics( self ):
        self.assertEqual( len( self.seq ), 3 )
        return

    ## Test the input/output containers set up for the sequence.
    def test_inputAndOutput( self ):
        self.assertEqual( self.seq.Calibration.electrons, 'Electrons' )
        self.assertEqual( self.seq.Efficiency.egammasOut,
                          'AnalysisElectrons_%SYS%' )
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
                         outputPropName = { 'electrons' : 'particlesOut' } )
        alg = createAlgorithm( 'SelectionAlg', 'MuonSelection' )
        self.seq.append( alg, inputPropName = { 'muons' : 'particles' },
                         outputPropName = { 'muons' : 'particlesOut' } )
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

## Test case for a sequence starting from a single container, producing
## multiple ones.
class TestAnaAlgSeqMultiOutputContainer( unittest.TestCase ):

    ## Set up the sequence that we'll test.
    def setUp( self ):
        self.seq = AnaAlgSequence( 'MultiOutputContainerSeq' )
        alg = createAlgorithm( 'CalibrationAlg', 'Calibration' )
        self.seq.append( alg, inputPropName = 'particles',
                         outputPropName = 'particlesOut' )
        alg = createAlgorithm( 'ParticleSplitterAlg', 'ParticleSplitter' )
        self.seq.append( alg, inputPropName = 'particles',
                         outputPropName = { 'goodObjects' : 'goodParticles',
                                            'badObjects' : 'badParticles' } )
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

## Test case for a sequence starting from multiple containers, and producing
## multiple new ones.
class TestAnaAlgSeqMultiInputOutputContainer( unittest.TestCase ):

    ## Set up the sequence that we'll test.
    def setUp( self ):
        self.seq = AnaAlgSequence( 'MultiInputOutputContainerSeq' )
        alg = createAlgorithm( 'ElectronSelectionAlg', 'ElectronSelection' )
        self.seq.append( alg, inputPropName = { 'electrons' : 'particles' },
                         outputPropName = { 'electrons' : 'particlesOut' } )
        alg = createAlgorithm( 'MuonSelectionAlg', 'MuonSelection' )
        self.seq.append( alg, inputPropName = { 'muons' : 'particles' },
                         outputPropName = { 'muons' : 'particlesOut' } )
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
