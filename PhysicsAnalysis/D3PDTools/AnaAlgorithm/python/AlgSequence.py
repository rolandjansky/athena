# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Check if we have the Athena package available. If yes, just use that code.
try:
    from AthenaCommon.AlgSequence import AthSequencer as AlgSequence
except ImportError:

    # If not, then we implement a standalone version of it...

    # Import(s):
    from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig

    class AlgSequence( object ):
        """Standalone algorithm sequence

        This is a light-weight emulation of Athena's AthSequencer class,
        implementing a simple algorithm sequence for EventLoop jobs.
        """

        def __init__( self, name = "AlgSequence" ):
            """Algorithm sequence constructor

            Note that the name parameter is completely ignored, it is only here
            to emulate the user interface of Athena's AthSequencer class.
            """

            # Set up the variable that will hold the algorithms of this
            # sequence:
            self._algs = []

            return

        def __getitem__( self, index ):
            """Return one algorithm from the sequence by index

            This is to allow getting the n'th element of an algorithm sequence,
            including the n'th element from the back of it if needed.

            Keyword arguments:
              index -- The index of the algorithm to get from the sequence
            """

            return self._algs[ index ]

        def __getattr__( self, name ):
            """Access one algorithm in the sequence, by name

            This is to allow modifying the properties of algorithms in a
            sequence that was set up centrally.

            Keyword arguments:
              name -- The name of the algorithm to look up in the sequence
            """

            # Look up the algorithm by name:
            for alg in self._algs:
                if alg.name() == name:
                    return alg
                pass

            # If no algorithm with this name was found, that's a problem:
            raise AttributeError( 'Algorithm with name "%s" was not found in ' \
                                      'sequence' % name )

        def __iter__( self ):
            """Create an iterator over the algorithms of this sequence

            This is to allow for a Python-like iteration over all algorithms
            that are part of the sequence.
            """

            # Create the iterator to process the internal list of algorithms:
            return iter( self._algs )

        def __iadd__( self, alg ):
            """Add one algorithm to the sequence

            This function is used to add one algorithm to the sequence object,
            using the '+=' operator.

            Keyword arguments:
              alg -- The algorithm to add to the sequence
            """

            # Check that the received object is of the right type:
            if not isinstance( alg, AnaAlgorithmConfig ):
                raise TypeError( 'The received object is not of type ' \
                                 'AnaAlgorithmConfig' )
                pass

            # Now check if an equivalent algorithm is already in the list.
            # As that's also an error.
            if alg in self:
                raise RuntimeError( 'Algorithm %s/%s is already in this ' \
                                    'sequence' % ( alg.type(), alg.name() ) )
                pass

            # Add the algorithm to the internal list:
            self._algs.append( alg )

            # Return the modified object:
            return self

        def __str__( self ):
            """Print the algorithm sequence in a user-friendly way

            This function takes care of printing the full configuration of every
            algorithm in the sequence.
            """

            result = ""
            for alg in self._algs:
                result += "%s\n" % str( alg )
                pass
            return result

        def __len__( self ):
            """Return the size/length of the algorithm sequence

            Just returning the number of algorithms that are in the sequence.
            """

            return len( self._algs )

        pass
