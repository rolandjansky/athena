# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetModConfig: A helper module for configuring jet modifier tools     #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

from AthenaCommon import Logging
modlog = Logging.logging.getLogger('JetModConfig')

########################################################################
# Function for generating a list of JetModifier tools
# Keep a default empty modstrings as we might want to actually
# specify some defaults (e.g. calib/filter/sort) for specific configs
#
# First incarnation is "dumb" but we want this to be "smart" and set up
# the right tools in order, propagating dependencies.
#
def getFinalModifierListAndPrereqs(mods_initial, jetdef):
    if len(mods_initial)==0: return [], set()
    mods_final = []
    # By the end of the recursion, all mods should have been applied already.
    prereqs = set()

    modlog.verbose("Initial mod list: " + str(mods_initial))

    # Loop over the initial list
    # Extract the concrete definition and specifier string
    # Then grab any prereqs, and expand if the list contains mods
    for mod in mods_initial:
        moddef, modspec = getModDefAndSpec( mod )
        modreqs = moddef.getPrereqs(modspec,jetdef)
        prereqmods = []
        for req in modreqs:
            reqtype, reqspec = req.split(':',1)
            if reqtype == "mod":
                prereqmods.append(reqspec)
            else:
                prereqs.add( req )
        # Recursion happens here
        prereqmods_final, moreprereqs = getFinalModifierListAndPrereqs( prereqmods, jetdef )
        prereqs.update( moreprereqs )
        mods_final += prereqmods_final + [(moddef,modspec)]

    modlog.verbose("Current input prereqs: {0}".format(prereqs))
    modlog.verbose("Final modlist: {0}".format(mods_final))
    return mods_final, prereqs

# [Optional] Args are:
#    1. Tool Type (may be ignored if the helper is a custom one)
#    2. Tool Name (may be ignored if the helper is a custom one)
#   [3.] Config helper
#   [4.] Prereqs (default to []). If "None", will try to get from the function.
#   [5.] Flag passJetDef specifying if the helper needs the jet definition
# We use a helper class to encapsulate the modifier configuration

# Translate modifier string into JetModifier if necessary
# Extracts the modspec from the string or the config object
def getModDefAndSpec(mod):
    from StandardJetMods import jetmoddict
    moddef = mod
    modspec = ""
    if mod.__class__ == type("str"):
        modkey = mod
        if ":" in mod:
            modkey, modspec = mod.split(':',1)
        moddef = jetmoddict[modkey]
    else:
        modspec = moddef.modspec
    modlog.verbose("Interpreted modifier string {0} as {1} with specification \"{2}\"".format(mod,moddef,modspec))
    return moddef, modspec

# Translate JetModifier into a concrete tool
def getModifier(jetdef, moddef, modspec):
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
        modtool = moddef.helperfn(**kwargs)
    except Exception as e:
        modlog.error( "Unhandled modifier specification {0} for {1}!".format(moddef,jetdef.basename) )
        modlog.error( "Received exception \"{0}\"".format(e) )
        modlog.error( "Helper function is \"{0}\"".format(moddef.helperfn) )
        raise ValueError( "JetModConfig unable to handle mod {0}".format(moddef) )

    return modtool
