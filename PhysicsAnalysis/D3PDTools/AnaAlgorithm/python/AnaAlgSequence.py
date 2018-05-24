# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Import(s):
from AnaAlgorithm.AlgSequence import AlgSequence

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

        # Iterate over the algorithms:
        currentInput = inputName
        inputRegex = '(^$)'
        tmpIndex = 1
        for alg, inName, outName, syst in zip( self, self._inputPropNames,
                                               self._outputPropNames,
                                               self._affectingSystematics ):

            # Set the input name(s)
            setattr( alg, inName, currentInput )
            setattr( alg, inName + 'Regex', inputRegex )

            if outName:
                currentInput = '%s_tmp%i_%%SYS%%' % ( inputName, tmpIndex )
                tmpIndex += 1
                setattr( alg, outName, currentInput )
                if syst:
                    inputRegex += '|%s' % syst
                    pass
                pass

            if syst:
                alg.systematicsRegex = syst
                pass

            pass

        # Add a '_%SYS%' postfix to the output name if the user didn't put
        # this string into the name somewhere:
        if not '%SYS%' in outputName:
            outputName += '_%SYS%'
            pass

        # Set the output name of the last algorithm (that provides output)
        # to the requested value:
        for alg, outName in reversed( zip( self, self._outputPropNames ) ):
            if outName:
                setattr( alg, outName, outputName )
                break
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
        self._inputPropNames.append( inputPropName )
        self._outputPropNames.append( outputPropName )
        self._affectingSystematics.append( affectingSystematics )

        return

    pass
