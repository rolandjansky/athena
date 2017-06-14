# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id$
#
# @file RecExConfig/python/hideInput.py
# @author sss
# @date Apr 2017
# @brief Block the reading of a specific data object.
#


from RecExConfig.ObjKeyStore                  import cfgKeyStore
from SGComps.AddressRemappingSvc              import addInputRename


##
# @brief Block the reading of a specific data object.
#
# Call hideInput('TYPE', 'KEY') to avoid reading an object KEY of type TYPE
# from the input file.  This can be useful during reprocessing, when the
# object is being remade.
#
# If TYPE is an xAOD type, then the corresponding auxiliary store object
# will be hidden as well.  KEY may also be of the form KEY.ATTR to hide
# a specific dynamic auxiliary variable.
#
# The hiding is actually implemented using input renaming.
# The hidden objects are renamed to end with `_old'.
#
def hideInput (typ, key):
    sgkey = key
    dpos = sgkey.find ('.')
    if dpos >= 0 and dpos < len(sgkey)-1:
        sgkey = sgkey[:dpos]
    if cfgKeyStore.isInInput (typ, sgkey):
        if key.endswith('.'):
            newkey = key[:-1] + '_old.'
        else:
            newkey = key + '_old'
        addInputRename (typ, key, newkey)

        # Check for an xAOD type.
        if typ.startswith('xAOD::') and key.find('.') < 0:
            if typ.endswith ('Container'):
                auxtyp = typ[:-9] + 'AuxContainer'
            else:
                auxtyp = typ + 'AuxInfo'

            auxkey = key + 'Aux.'
            auxnewkey = key + 'Aux_old.'
            addInputRename (auxtyp, auxkey, auxnewkey)
    return
