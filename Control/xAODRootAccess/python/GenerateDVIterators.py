# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Module used to generate proper Python iterators for all the known
# DataVector container types.
#

# Pull in ROOT:
import ROOT

# Make sure that the transient trees are explicitly deleted when exiting
# python:
import atexit
atexit.register( ROOT.xAOD.ClearTransientTrees )

## Generic iterator that can be used with any DataVector type
#
# Taken from Noel Dawe, from: https://its.cern.ch/jira/browse/ATLASG-31
#
def DataVectorIterator( self ):
    for i in range( self.size() ):
        yield self.at( i )

## Function installing proper iterators for all the known DataVector types
#
# This function needs to be called once all the dictionaries for our types
# got loaded. It fixes up the DataVector types (in ROOT 6) to behave as proper
# Python lists.
#
def generateDVIterators():

    # Let the user know what's happening:
    print( "generateDVIterators INFO Attaching iterators to all known DV "
           "types" )
    print( "generateDVIterators INFO This may take a moment..." )

    # Load/access the dictionaries of all of the same types that xAOD::Init()
    # accesses as well. Since apparently "the C++ side" and "the Python side"
    # need to do this separately... :-/
    for name in [ 'TruthParticleContainer',
                  'MuonRoIContainer',
                  'CaloClusterContainer',
                  'TrackParticleContainer',
                  'ElectronContainer',
                  'MuonContainer',
                  'JetContainer',
                  'TauJetContainer',
                  'PFOContainer',
                  'TrigElectronContainer',
                  'L2CombinedMuonContainer',
                  'ParticleContainer' ]:
        getattr( ROOT.xAOD, name, None )
        pass

    # Iterate over all classes known to ROOT:
    for typ in ROOT.gROOT.GetListOfClasses():
        # Skip emulated classes:
        if not typ.IsLoaded():
            continue
        # We're only interested in DataVector types:
        if typ.GetName().find( "DataVector<" ) != 0:
            continue
        # Access the python type associated with this type:
        ptyp = getattr( ROOT, typ.GetName(), None )
        if not ptyp:
            print( "generateDVIterators ERROR Couldn't get python type for " \
                   "\"%s\"" % typ.GetName() )
            continue
        # Massage the iterator of this type:
        ptyp.__iter__ = DataVectorIterator
        pass

    # Return gracefully:
    return

# Call the function:
generateDVIterators()
