# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
                                                                     #
JetModConfig: A helper module for configuring jet modifier tools     #
Author: TJ Khoo                                                      #
                                                                     #
"""
from AthenaCommon import Logging
from .JetDefinition import JetModifier
import JetRecConfig.ConstModHelpers as constHelper
modlog = Logging.logging.getLogger('JetModConfig')

def getModifier(jetdef, moddef, modspec):
    """Translate JetModifier into a concrete tool"""
    modtool = None
    modlog.verbose("Retrieving modifier {0}".format(str(moddef)))

    # Define some optional keyword arguments
    kwargs = {}
    if moddef.passJetDef:
        kwargs["jetdef"] = jetdef
    if modspec!="":
        kwargs["modspec"] = modspec

    # Get the modifier tool
    try:
        modtool = moddef.createfn(**kwargs)
    except Exception as e:
        modlog.error( "Unhandled modifier specification {0} for mod {1} acting on jet def {2}!".format(modspec,moddef,jetdef.basename) )
        modlog.error( "Received exception \"{0}\"".format(e) )
        modlog.error( "Helper function is \"{0}\"".format(moddef.createfn) )
        raise ValueError( "JetModConfig unable to handle mod {0} with spec \"{1}\"".format(moddef,modspec) )

    return modtool



def prereqToDef(prereq, parentjetdef):
    """translate a prereq string in the form 'type:alias' into a known config object.
    """
    reqtype, reqkey = prereq.split(':',1)
    if reqtype=='mod':
        reqInstance = aliasToModDef(reqkey, parentjetdef)
    else:
        reqInstance = constHelper.aliasToInputDef(reqkey, parentjetdef)
    return reqInstance

def aliasToModDef(alias, parentjetdef ):
    """return an alias into a JetModifier config object, recursively instatiating all aliases in the dependencies of this JetModifier."""
    if isinstance(alias, JetModifier):
        return alias
    # else assume it's a string

    # split it, to extract the optional specifiers 
    modL = alias.split(":")
    modkey = modL[0]
    modspec = ':'.join(modL[1:])

    # retrieve an instance from the known modifiers in StandardJetMods :
    from .StandardJetMods import jetmoddict
    moddef = jetmoddict[modkey].clone( modspec = modspec)

    
    if callable(moddef.prereqs):
        moddef.prereqs = moddef.prereqs( modspec, parentjetdef )        
    
    for prereq in moddef.prereqs:
        reqInstance = parentjetdef._prereqDic.get( prereq, None)
        if reqInstance is None:
            reqInstance = prereqToDef(prereq, parentjetdef)

        if prereq.startswith('ghost:'):
            # then it is also an input : register this requirement also as an input
            prereqN = prereq.split(':')[1]
            parentjetdef._prereqOrder.append('input:'+prereqN)
            parentjetdef._prereqDic['input:'+prereqN] = reqInstance # the input config instance is identical for input and ghost (only the PseudoJet will differ)
            
        parentjetdef._prereqOrder.append(prereq)
        parentjetdef._prereqDic[prereq] = reqInstance
        
    return moddef



def jetModWithAlternateTrk(jetdef, trkopt):
    """ Update all the track-related JetModifier in jetdef._prereqOrder so that they have their optional specification
    ('modspec') set to trkopt.
    This is needed when some prereq JetModifier are generated without the specification. The main use case is trigger where the Track and vertex container name are different than the standard.
    IMPORTANT : this must be called after instantiateAliases() .
    """
    
    from JetRecTools.JetRecToolsConfig import trackcollectionmap

    def changeJetModSpec( mod ):
        if mod in ['mod:JVT','mod:JVF', 'mod:TrackMoments', 'mod:TrackSumMoments']:
            return mod+':'+trkopt
        return mod

    for i in range(len(jetdef._prereqOrder)):
        jetdef._prereqOrder[i] = changeJetModSpec( jetdef._prereqOrder[i] )
    
    # Deduplicate the prereq (with python > 3.6 dict is ordered so the trick is guaranteed to work) :
    jetdef._prereqOrder[:] = list(dict.fromkeys(jetdef._prereqOrder) )
