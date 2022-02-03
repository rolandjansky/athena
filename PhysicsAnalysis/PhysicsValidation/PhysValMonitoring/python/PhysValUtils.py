# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Defining a python module with helpers for Physics Validation

from AthenaCommon.Logging import logging
logger = logging.getLogger('PhysValUtils')

################################################################################################

def addPhysValAODContent(algseq,doJets,doTopoCluster):
    '''
    Schedule the addition of collections needed for validation of
    primary xAODs: AntiKt4TruthJets and LC/EMOriginTopoClusters
    '''

    logger.info( '****************** Adding content for AOD PhysVal *****************' )
    
    # Check some flags for steering
    from RecExConfig.AutoConfiguration import IsInInputFile
    requiresTruthJets  = IsInInputFile('xAOD::TruthParticleContainer','TruthParticles') and not IsInInputFile('xAOD::JetContainer','AntiKt4TruthJets')
    requiresLCOriginTC = not IsInInputFile('xAOD::CaloClusterContainer','LCOriginTopoClusters')
    requiresEMOriginTC = not IsInInputFile('xAOD::CaloClusterContainer','EMOriginTopoClusters')

    # Truth jets
    if doJets and requiresTruthJets:
        addAntiKt4TruthJets(algseq)

    ## Origin-corrected topoclusters
    if doTopoCluster and (requiresLCOriginTC or requiresEMOriginTC):
        addOriginCorrectedClusters(algseq,requiresLCOriginTC,requiresEMOriginTC)

    logger.info( '******************              Done              *****************' )
    
################################################################################################

def addAntiKt4TruthJets(algseq):

    from JetRecConfig.StandardSmallRJets import AntiKt4Truth
    from JetRecConfig.JetRecConfig import getJetAlgs
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    algs, jetdef_i = getJetAlgs(ConfigFlags, AntiKt4Truth, True)
    sortJetAlgs(algseq, algs)

################################################################################################

def addOriginCorrectedClusters(algseq,doLC,doEM):
    from JetRecConfig.JetRecConfig import getInputAlgs
    from JetRecConfig.StandardJetConstits import stdConstitDic as cst
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    logger.info( 'Configuring origin-corrected cluster collections:' )

    if doLC:
        logger.info( '    * LCOriginTopoClusters' )
        constit_algs = getInputAlgs(cst.LCTopoOrigin, configFlags=ConfigFlags)
        sortJetAlgs(algseq, constit_algs)
    if doEM:
        logger.info( '    * EMOriginTopoClusters' )
        constit_algs = getInputAlgs(cst.EMTopoOrigin, configFlags=ConfigFlags)
        sortJetAlgs(algseq, constit_algs)

################################################################################################

def sortJetAlgs(algseq, algs):

    from JetRecConfig.JetRecConfig import reOrderAlgs
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable

    algs = reOrderAlgs( [a for a in algs if a is not None])
    for a in algs:
        if hasattr(algseq,a.getName()):
            continue
        algseq += conf2toConfigurable(a)

################################################################################################

def getHistogramDefinitions(fileName, usage, use_group):
    import os.path
    import json

    # vars required in the input json
    requiredKeys = ['title', 'xtitle', 'ytitle', 'path', 'xbins', 'xmin', 'xmax', 'type']
    # define a order for the keys in the output list
    position = {'name': 0, 'title': 1, 'path': 2, 'xbins': 3, 'xmin': 4, 'xmax': 5, 'type': 6, 'ymin': 7, 'ymax': 8}

    # check if the path exists
    if not os.path.isfile(fileName):
        raise IOError("This path to the file does not exist:" + fileName)
    
    # read file
    file_dict = {}
    with open(fileName) as json_file:
        file_dict = json.load(json_file)
    
    # check if the file has the right keys in the dict
    for group in file_dict:
        for var in file_dict[group]:
            for required_key in requiredKeys:
                if required_key not in file_dict[group][var]:
                    raise IOError("The key " + required_key + " is missing in the dict for the variable " + var + ".")
    
    # read the right parts of the dict 
    list = []
    # loop over all groups
    for group in file_dict:
        # skip that group if it is not asked for
        if not (use_group == 'ALL' or group == use_group) or group == 'template':
            continue
        # loop over all vars
        for var in file_dict[group]:
            # only fill the usage dict with this var if the usage exists in the 'path' dict
            if usage in file_dict[group][var]['path']:
                # if the dict has the key forEachTruthType -> expand the one dict into one for each type
                if 'forEach' in file_dict[group][var]:
                    for type in file_dict[group][var]['forEach']:
                        var_list = ['', '', '', '', '', '', '', '', '']
                        # adapt the new variable name
                        new_var_name = var + '_' + type
                        # fill the name
                        var_list[position['name']] = str(new_var_name)
                        # merge the tile keys
                        var_list[position['title']] = str(file_dict[group][var]['title'] + ' ' + type + ';' + file_dict[group][var]['xtitle'] + ';' + file_dict[group][var]['ytitle'])
                        # get the right path
                        var_list[position['path']] = str(file_dict[group][var]['path'][usage])
                        # just copy the rest
                        for key in ['xbins', 'xmin', 'xmax', 'type']:
                            var_list[position[key]] = str(file_dict[group][var][key])
                        if(file_dict[group][var]['type'] == 'TProfile'):
                            var_list[position['ymin']] = str(file_dict[group][var]['ymin'])
                            var_list[position['ymax']] = str(file_dict[group][var]['ymax'])
                        # append the current variable to the list
                        list.append(var_list)
                else:
                    var_list = ['', '', '', '', '', '', '', '', '']
                    # fill the name
                    var_list[position['name']] = str(var)
                    # merge the tile keys
                    var_list[position['title']] = str(file_dict[group][var]['title'] + ';' + file_dict[group][var]['xtitle'] + ';' + file_dict[group][var]['ytitle'])
                    # get the right path
                    var_list[position['path']] = str(file_dict[group][var]['path'][usage])
                    # just copy the rest
                    for key in ['xbins', 'xmin', 'xmax', 'type']:
                        var_list[position[key]] = str(file_dict[group][var][key])
                    if(file_dict[group][var]['type'] == 'TProfile'):
                        var_list[position['ymin']] = str(file_dict[group][var]['ymin'])
                        var_list[position['ymax']] = str(file_dict[group][var]['ymax'])
                    # append the current variable to the list
                    list.append(var_list)
    
    return list



