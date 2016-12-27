# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TPyEvent.py 790263 2016-12-16 21:24:02Z ssnyder $
#
# Module holding the TPyEvent Python class
#

# Pull in ROOT:
import ROOT

def _typename(t):
    return getattr (t, '__cppname__', t.__name__)

## A Python wrapper around xAOD::TPyEvent
#
# In order to make the semi-templated functions of xAOD::TPyEvent more
# convenient to use from PyROOT, the user is supposed to use an instance
# of this class, and not ROOT.xAOD.TPyEvent directly.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 790263 $
# $Date: 2016-12-16 22:24:02 +0100 (Fri, 16 Dec 2016) $
#
class TPyEvent( ROOT.xAOD.TPyEvent ):

    ## Constructor for the class
    def __init__( self ):

        # Forward the call to the base class's constructor:
        ROOT.xAOD.TPyEvent.__init__( self )
        return

    ## Convenient version of the base class's contains function
    #
    # This function allows the user to, instead of figuring out the exact
    # type name of some C++ type, to rather write code like:
    #
    # <code>
    #   if event.contains( "Electrons", ROOT.xAOD.ElectronContainer_v1 ):
    # </code>
    #
    # @param key  The string key of the object to check for
    # @param type The type of the object we are looking for
    # @returns <code>True</code> if the object is available in the event,
    #          <code>False</code> if it's not
    #
    def contains( self, key, type ):
        return super( TPyEvent, self ).contains( key, _typename(type) )

    ## Convenient version of the base class's transientContains function
    #
    # This function allows the used to, instead of figuring out the exact
    # type name of some C++ type, to rather write code like:
    #
    # <code>
    #   if event.transientContains( "MyElectrons",<br/>
    #                               ROOT.xAOD.ElectronContainer_v1 ):
    # </code>
    #
    # @param key  The string key of the object to check for
    # @param type The type of the object we are looking for
    # @returns <code>True</code> if the object is available in the event in a
    #          modifyable form, <code>False</code> if it's not
    #
    def transientContains( self, key, type ):
        return super( TPyEvent,
                      self ).transientContains( key, _typename (type) )

    ## Convenient version of the base class's record function
    #
    # This is the main function of this wrapper class. It allows the user to
    # record objects into an output file like:
    #
    # <code>
    #   el = ROOT.xAOD.ElectronContainer_v1()
    #   event.record( el, "MyElectrons" )
    # </code>
    #
    # @param obj The object to be recorded into the output file
    # @param key The key (branch name) for the object
    # @param basketSize The (optional) size of the basket for the output branch
    # @param splitLevel The (optional) split level for the output branch
    # @returns <code>xAOD::TReturnCode::kSuccess</code> if all was successful,
    #          or <code>xAOD::TReturnCode::kFailure</code> if not
    #
    def record( self, obj, key, basketSize = 32000, splitLevel = 0 ):
        return super( TPyEvent, self ).record( obj, key,
                                               _typename (obj.__class__),
                                               basketSize,
                                               splitLevel )
