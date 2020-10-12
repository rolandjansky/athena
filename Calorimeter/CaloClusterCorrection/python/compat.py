# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/compat.py
# Created: Aug 2020, sss
# Purpose: Backwards-compatbility helpers.
#
# Helpers for configuring cluster corrections using old configuration.
#


from AthenaConfiguration.ComponentAccumulator import \
     conf2toConfigurable, appendCAtoAthena
import string


#
# Return a new-style configuration flags object containing information
# from old flags.
#
def makeFlags():
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.loadAllDynamicFlags()
    flags = ConfigFlags.clone()

    from CaloClusterCorrection.CaloClusterCorrectionFlags \
         import caloClusterCorrectionFlags

    for name in dir(caloClusterCorrectionFlags):
        if name[0] not in string.ascii_uppercase:
            continue
        flag = getattr (caloClusterCorrectionFlags, name)
        if not flag.isDefault():
            if name == 'DBSubdetName':
                name2 = 'dbSubdetName'
            else:
                name2 = name[0].lower() + name[1:]
            val = flag.get_Value()
            setattr (flags.Calo.ClusterCorrection, name2, val)

    from AthenaCommon.JobProperties import jobproperties
    from AthenaCommon.GlobalFlags   import globalflags

    flags.GeoModel.AtlasVersion = jobproperties.Global.DetDescrVersion()
    flags.Input.isMC = globalflags.DataSource() != 'data'
    flags.IOVDb.GlobalTag = globalflags.ConditionsTag()

    return flags


#
# Apply the results of a ComponentAccumulator to Athena.
# Returns the list of configured correction tools.
#
def unpackCA (ca):
    tools = [conf2toConfigurable(c) for c in  ca.popPrivateTools()]
    appendCAtoAthena (ca)
    return tools
