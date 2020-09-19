# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Check if we have the Athena package available. If yes, just use that code.
try:
    from AthenaCommon.AlgSequence import AthSequencer as AlgSequence
except ImportError:

    # If not, then we implement a standalone version of it...

    # Import(s):
    import unittest
    from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig, indentBy

    class AlgSequence( object ):
        """Standalone algorithm sequence

        This is a light-weight emulation of Athena's AthSequencer class,
        implementing a simple algorithm sequence for EventLoop jobs.
        """

        def __init__( self, name = "AlgSequence" ):
            """Algorithm sequence constructor

            Keyword arguments:
              name -- The name of the algorithm sequence (for debugging)
            """

            # Set up the member variables:
            self._name = name
            self._algsAndSequences = []

            return

        def name( self ):
            """Return the name of this sequence

            Mainly for debugging purposes, and for when we are embedding
            one sequence into another one.
            """

            return self._name

        def insert( self, index, algOrSeq ):
            """Insert one algorithm/sequence into this sequence

            This allows us to extend existing sequences with a greater
            flexibility.

            Keyword arguments:
              index    -- The index to insert the algorithm/sequence under
              algOrSeq -- The object to insert
            """

            return self.__iadd__( algOrSeq, index = index )

        def __getitem__( self, index ):
            """Return one algorithm/sequence from the sequence by index

            This is to allow getting the n'th element of an algorithm sequence
            (which itself may either be an algorithm or a sequence),
            including the n'th element from the back of it if needed.

            Keyword arguments:
              index -- The index of the algorithm/sequence to get from the
                       sequence
            """

            return self._algsAndSequences[ index ]

        def __getattr__( self, name ):
            """Access one algorithm/sequence in this sequence, by name

            This is to allow modifying the properties of algorithms in a
            sequence that was set up centrally.

            Keyword arguments:
              name -- The name of the algorithm/sequence to look up in the
                      sequence
            """

            # Look up the algorithm by name:
            for algOrSeq in self._algsAndSequences:
                if algOrSeq.name() == name:
                    return algOrSeq
                pass

            # If no algorithm with this name was found, that's a problem:
            raise AttributeError( 'Algorithm/sequence with name "%s" was not ' \
                                      'found' % name )

        def __delattr__( self, name ):
            """Remove one algorithm/sequence from this sequence, by name

            This is to allow removing algorithms (or even sequences) from this
            sequence in case that would be needed.

            Keyword arguments:
              name -- The name of the algorithm/sequence to delete from the
                      sequence
            """

            # Look up the algorithm by name:
            for algOrSeq in self._algsAndSequences:
                if algOrSeq.name() == name:
                    # If we found it, remove it:
                    self._algsAndSequences.remove( algOrSeq )
                    return
                pass

            # If no algorithm/sequence with this name was found, that's a
            # problem:
            raise AttributeError( 'Algorithm/sequence with name "%s" was not ' \
                                      'found' % name )

        def __iter__( self ):
            """Create an iterator over all the algorithms of this sequence

            This is to allow for a Python-like iteration over all algorithms
            that are part of the sequence. This includes iterating over the
            algorithms that may be in sub-sequences of this sequence.
            """

            # Create the iterator to process the internal list of algorithms:
            return AlgSequenceIterator( self._algsAndSequences )

        def __iadd__( self, algOrSeq, index = None ):
            """Add one algorithm/sequence to the sequence

            This function is used to add one algorithm (or algorithm sequence)
            to the sequence object, using the '+=' operator.

            Keyword arguments:
              algOrSeq -- The algorithm/sequence to add to the sequence
            """

            # Check that the received object is of the right type:
            if not isinstance( algOrSeq, AnaAlgorithmConfig ) and \
               not isinstance( algOrSeq, AlgSequence ):
                raise TypeError( 'The received object is not of type ' \
                                 'AnaAlgorithmConfig or AlgSequence' )
                pass

            # Now check if an equivalent algorithm/sequence is already in the
            # list. As that's also an error.
            if algOrSeq in self:
                raise RuntimeError( 'Algorithm/sequence %s is already in ' \
                                    'this sequence' % algOrSeq.name() )
                pass

            # Add the algorithm/sequence to the internal list:
            if not index:
                self._algsAndSequences.append( algOrSeq )
            else:
                self._algsAndSequences.insert( index, algOrSeq )
                pass

            # Return the modified object:
            return self

        def __eq__( self, other ):
            """Check for equality with another object

            The implementation of this is very simple. We only check that
            the name of the sequences would match.

            Keyword arguments:
              other -- The object to compare this one against
            """

            # First check that the other object is also an AlgSequence one:
            if not isinstance( other, AlgSequence ):
                return False

            # Base the equality purely on the name of the sequence. This may
            # need to be made smarter at one point.
            return ( self.name() == other.name() )

        def __ne__( self, other ):
            """Check for an inequality with another object

            This is just defined to make the '!=' operator of Python behave
            consistently with the '==' operator for such objects.

            Keyword arguments:
              other -- The object to compare this one against
            """
            return not self.__eq__( other )

        def __str__( self ):
            """Print the algorithm sequence in a user-friendly way

            This function takes care of printing the full configuration of every
            algorithm in the sequence.
            """

            result = AnaAlgorithmConfig._printHeader( 'AlgSequence/%s' %
                                                      self.name() )
            result += '\n'
            for algOrSeq in self._algsAndSequences:
                result += '| %s\n' % indentBy( str( algOrSeq ), '| ' )
                pass
            result += AnaAlgorithmConfig._printFooter( 'AlgSequence/%s' %
                                                       self.name() )
            return result

        def __len__( self ):
            """Return the size/length of the algorithm sequence

            Just returning the number of algorithms and sequences that are in
            this sequence. So this is not a recursive count.
            """

            return len( self._algsAndSequences )

        pass

    class AlgSequenceIterator( object ):
        """Iterator over a standalone algorithm sequence

        This custom class is needed to implement a "recursive iteration", which
        would loop over all algorithms in a sequence that itself may have
        sub-sequences inside of it.
        """

        def __init__( self, sequence ):
            """Constructor for the algorithm sequence iterator

            Keyword arguments:
              sequence -- The sequence to iterate over (recursively)
            """

            # Set up the internal variables:
            self._sequence = sequence
            self._index = 0
            self._iterator = None

            return

        def __iter__( self ):
            """Function making this iterator iterable itself
            """
            return self

        def __next__( self ):
            """Function implementing the recursive iteration over an AlgSequence

            This is where most of the logic is. The iterator loops over the
            elements of the AlgSequence that was given to it, one by one. When
            it finds an element in the AlgSequence that itself is also an
            AlgSequence, then it creates a helper iterator object that would
            process that sub-sequence, and continue the iteration using that
            helper.

            The end result is that the iteration should loop over every
            algorithm in the sequence and its sub-sequences.
            """

            # First off, check whether we reached the end of the sequence.
            if self._index >= len( self._sequence ):
                raise StopIteration()

            # If not, check whether we are currently iterating over a
            # sub-sequence.
            if self._iterator:
                try:
                    return self._iterator.__next__()
                except StopIteration:
                    # If the sub-sequence is exhaused, then switch to the
                    # next element in our sequence, and call this function
                    # recursively.
                    self._index += 1
                    self._iterator = None
                    return self.__next__()
                pass

            # If we are not iterating over a sub-sequence at the moment, let's
            # just take the next element of our sequence.
            element = self._sequence[ self._index ]

            # If this element is a sequence itself, then switch to "sub-sequence
            # iterating mode", and call this function recursively in that mode.
            if isinstance( element, AlgSequence ):
                self._iterator = AlgSequenceIterator( element )
                return self.__next__()

            # Apparently it's an algorithm we found. So update the internal
            # index, and simply return the algorithm.
            self._index += 1
            return element

        pass

    #
    # Unit tests for the code
    #

    ## Test case for a sequence with only algorithms in it
    class TestAlgSequenceWithAlgs( unittest.TestCase ):

        ## Function setting up the sequence to be tested
        def setUp( self ):
            self.seq = AlgSequence( 'TestSequence' )
            self.seq += AnaAlgorithmConfig( 'AlgType1/Algorithm1' )
            self.seq += AnaAlgorithmConfig( 'AlgType2/Algorithm2' )
            return

        ## Test basic features of the algorithm sequence
        def test_sequenceBasics( self ):
            self.assertEqual( self.seq.name(), 'TestSequence' )
            self.assertEqual( len( self.seq ), 2 )
            return

        ## Test index lookup operations on the algorithm sequence
        def test_indexLookup( self ):
            self.assertEqual( self.seq[ 0 ].name(), 'Algorithm1' )
            self.assertEqual( self.seq[ 1 ].name(), 'Algorithm2' )
            with self.assertRaises( IndexError ):
                self.seq[ 2 ]
                pass
            return

        ## Test 'by name' lookup operations on the algorithm sequence
        def test_nameLookup( self ):
            self.assertEqual( self.seq.Algorithm1.type(), 'AlgType1' )
            self.assertEqual( self.seq.Algorithm2.type(), 'AlgType2' )
            with self.assertRaises( AttributeError ):
                self.seq.Algorithm3.type()
                pass
            return

        ## Test the ability to iterate over the algorithms in the sequence
        def test_iteration( self ):
            algNames = [ alg.name() for alg in self.seq ]
            self.assertEqual( algNames, [ 'Algorithm1', 'Algorithm2' ] )
            return

        ## Test the insertion of one algorithm
        def test_insertAlg( self ):
            self.seq.insert( 1, AnaAlgorithmConfig( 'AlgType3/Algorithm3' ) )
            self.assertEqual( len( self.seq ), 3 )
            self.assertEqual( self.seq[ 0 ].name(), 'Algorithm1' )
            self.assertEqual( self.seq[ 1 ].name(), 'Algorithm3' )
            self.assertEqual( self.seq[ 2 ].name(), 'Algorithm2' )
            return

        ## Test the deletion of an algorithm
        def test_deleteAlg( self ):
            del self.seq.Algorithm1
            self.assertEqual( len( self.seq ), 1 )
            self.assertEqual( self.seq[ 0 ].name(), 'Algorithm2' )
            return

        pass

    ## Test case for a sequence with algorithms and sub-sequences
    class TestAlgSequenceWithAlgsAndSeqs( unittest.TestCase ):

        ## Function setting up the (elaborate) sequence to be tested
        def setUp( self ):
            self.seq = AlgSequence( 'MainSequence' )
            self.seq += AnaAlgorithmConfig( 'PreSelectionAlg/PreSelection' )
            self.seq += AlgSequence( 'MuonSequence' )
            self.seq.MuonSequence += \
              AnaAlgorithmConfig( 'MuonAnalysisAlg/MuonAnalysis' )
            self.seq.MuonSequence += \
              AnaAlgorithmConfig( 'MuonHistoAlg/MuonHistogramming' )
            self.seq += AlgSequence( 'JetSequence' )
            self.seq.JetSequence += AlgSequence( 'JetPreSequence' )
            self.seq.JetSequence.JetPreSequence += \
              AnaAlgorithmConfig( 'JetPreSelectorAlg/JetPreSelector' )
            self.seq.JetSequence.JetPreSequence += \
              AnaAlgorithmConfig( 'JetDecoratorAlg/JetDecorator' )
            self.seq.JetSequence += \
              AnaAlgorithmConfig( 'JetAnalysisAlg/JetAnalysis' )
            self.seq.JetSequence += \
              AnaAlgorithmConfig( 'JetHistoAlg/JetHistogramming' )
            self.seq += AnaAlgorithmConfig( 'PostProcessingAlg/PostProcessing' )
            return

        ## Test basic features of the algorithm sequence
        def test_sequenceBasics( self ):
            self.assertEqual( len( self.seq ), 4 )
            return

        ## Test index lookup operations on the algorithm sequence
        def test_indexLookup( self ):
            self.assertEqual( self.seq[ 0 ].name(), 'PreSelection' )
            self.assertEqual( self.seq[ 2 ].name(), 'JetSequence' )
            with self.assertRaises( IndexError ):
                self.seq[ 4 ]
                pass
            return

        ## Test 'by name' lookup operations on the algorithm sequence
        def test_nameLookup( self ):
            self.assertEqual( self.seq.MuonSequence.MuonAnalysis.type(),
                              'MuonAnalysisAlg' )
            self.assertEqual( self.seq.JetSequence.JetHistogramming.type(),
                              'JetHistoAlg' )
            self.assertEqual( self.seq.JetSequence.JetPreSequence.JetDecorator.type(),
                              'JetDecoratorAlg' )
            with self.assertRaises( AttributeError ):
                self.seq.MuonSequence.InvalidAlg.type()
                pass
            return

        ## Test the ability to iterate over the algorithms in the sequence
        def test_iteration( self ):
            algNames = [ alg.name() for alg in self.seq ]
            self.assertEqual( algNames, [ 'PreSelection', 'MuonAnalysis',
                                          'MuonHistogramming', 'JetPreSelector',
                                          'JetDecorator', 'JetAnalysis',
                                          'JetHistogramming',
                                          'PostProcessing' ] )
            return

        pass

    pass
