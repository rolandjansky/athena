# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TPyStore.py 790263 2016-12-16 21:24:02Z ssnyder $
#
# Module holding the TPyStore Python class
#

# Pull in ROOT:
import ROOT

def _typename(t):
    return getattr (t, '__cppname__', t.__name__)

## A Python wrapper around xAOD::TPyStore
#
# In order to make the semi-templated functions of xAOD::TPyStore more
# convenient to use from PyROOT, the user is supposed to use an instance
# of this class, and not ROOT.xAOD.TPyStore directly.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 790263 $
# $Date: 2016-12-16 22:24:02 +0100 (Fri, 16 Dec 2016) $
#
class TPyStore( ROOT.xAOD.TPyStore ):

    ## Constructor for the class
    def __init__( self ):

        # Forward the call to the base class's constructor:
        ROOT.xAOD.TPyStore.__init__( self )
        return

    ## Convenient version of the base class's contains function
    #
    # This function allows the user to, instead of figuring out the exact
    # type name of some C++ type, to rather write code like:
    #
    # <code>
    #   if store.contains( "Electrons", ROOT.xAOD.ElectronContainer_v1 ):
    # </code>
    #
    # @param key  The string key of the object to check for
    # @param type The type of the object we are looking for
    # @returns <code>True</code> if the object is available in the store,
    #          <code>False</code> if it's not
    #
    def contains( self, key, type ):
        return super( TPyStore, self ).contains( key, _typename (type) )

    ## Convenient version of the base class's isConst function
    #
    # This function allows the user to, instead of figuring out the exact
    # type name of some C++ type, to rather write code like:
    #
    # <code>
    #   if store.isConst( "Electrons", ROOT.xAOD.ElectronContainer_v1 ):
    # </code>
    #
    # @param key  The string key of the object to check for
    # @param type The type of the object we are looking for
    # @returns <code>True</code> if the object is only available as a constant,
    #          <code>False</code> if it's not
    #
    def isConst( self, key, type ):
        return super( TPyStore, self ).isConst( key, _typename (type) )

    ## Convenient version of the base class's record function
    #
    # This is the main function of this wrapper class. It allows the user to
    # record objects into the transient store like:
    #
    # <code>
    #   el = ROOT.xAOD.ElectronContainer_v1()
    #   store.record( el, "MyElectrons" )
    # </code>
    #
    # @param obj The object to be recorded into the output file
    # @param key The key (branch name) for the object
    # @returns <code>xAOD::TReturnCode::kSuccess</code> if all was successful,
    #          or <code>xAOD::TReturnCode::kFailure</code> if not
    #
    def record( self, obj, key ):
        return super( TPyStore, self ).record( obj, key,
                                               _typename (obj.__class__) )
