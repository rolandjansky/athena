# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

'''
@file HTTTagConfig.py
@author Riley Xu - riley.xu@cern.ch
@date August 27, 2020

This file declares functions to interface with the tag system. Tags should be
retrieved and modified using these functions, and not accessing them directly
from their respective files.
'''


import TrigHTTUtils.HTTMapTags as HTTMapTags
import TrigHTTUtils.HTTBankTags as HTTBankTags
import TrigHTTUtils.HTTAlgorithmTags as HTTAlgorithmTags


_tagDependency = {
        'map':  [],
        'bank': ['map'],
        'algo': [],
}


def getTags(stage = 'algo', options = {}):
    '''
    Returns a set of tags used to configure various HTT components, configured
    with additional user options. This function will do consistency checks on
    tag dependencies.

    @param options - This should be a dictionary with keys that match those
            in the packages' tags, and optionally entries to switch to a
            different tag from the default (see below). Extra/missing keys are ok.

            To swith to a different tag, use the following keys:
                HTTMaps - mapTag
                HTTBanks - bankTag
                HTTAlgorithms - algoTag
    @param stage - Which stage this function is being called from: map/bank/algo.
            This is limits the returned tags and the dependency checking to the
            given stage.

    @return [mapTag, bankTag, algoTag]
    '''

    packages = ['map', 'bank', 'algo'] # In order of stage
    tags = [{}] * len(packages) # In order as above

    for i in range(packages.index(stage), -1, -1):
        tags[i] = _getTag(packages[i], options)

        # Consistency check with dependent tags
        for dependentPackage in _tagDependency[packages[i]]:
            keyName = dependentPackage + 'Tag'
            requestedTag = options.get(keyName)
            if requestedTag is None:
                options[keyName] = tags[i][keyName] # Force the use of the dependent tag
            elif requestedTag != tags[i][keyName]:
                raise NameError("Inconsistent tags. Requested " + requestedTag + " but expect " + tags[i][keyName])

    return tags



def _getTag(package, options):
    '''
    Helper function.
    Returns a tag from the specified package, with the given options applied.

    @param package - Can be: map, bank, algo
    @param options - This should be a dictionary with keys that match those
            in the package's tag, and optionally an entry to switch to a
            different tag from the default, using the key (package + 'Tag').
            Extra/missing keys are ok.
    '''

    if package == 'map':
        tagName = HTTMapTags.defaultTag
        tagList = HTTMapTags.HTTMapTags
        keyName = 'mapTag'
    elif package == 'bank':
        tagName = HTTBankTags.defaultTag
        tagList = HTTBankTags.HTTBankTags
        keyName = 'bankTag'
    elif package == 'algo':
        tagName = HTTAlgorithmTags.defaultTag
        tagList = HTTAlgorithmTags.HTTAlgorithmTags
        keyName = 'algoTag'
    else:
        raise NameError("Unknown package: " + package)

    if keyName in options:
        tagName = options[keyName]
        if tagName not in tagList:
            raise NameError("Invalid " + package + " tag: " + tagName)

    tag = tagList[tagName].copy()
    tag.update((k, options[k]) for k in tag.viewkeys() & options.viewkeys())
    return tag


def getDescription(*args, **kwargs):
    '''
    Returns a description string with the tag name(s) and a complete summary of changes
    from the original tag(s), if any.

    @param args - Any number of tags (the actual dictionary, not just the name) in any order.
    @param filename - Omit all whitespace, and use '-' and '_' as seperator
    '''
    description = ''
    for tag in args:
        if tag['package'] == 'map':
            original_tag = HTTMapTags.HTTMapTags[tag['name']].copy()
        elif tag['package'] == 'bank':
            original_tag = HTTBankTags.HTTBankTags[tag['name']].copy()
        elif tag['package'] == 'algo':
            original_tag = HTTAlgorithmTags.HTTAlgorithmTags[tag['name']].copy()

        if 'filename' in kwargs and kwargs['filename']:
            skip_keys = ['bankDir']
                # keys to skip when generating filename / short description

            if description:
                description += '__'
            description += tag['name']

            for key in tag:
                if key in skip_keys:
                    continue
                if tag[key] != original_tag[key]:
                    description += '.' + key + '-' + str(tag[key])
        else:
            description += tag['package'] + ': ' + tag['name'] + '\n'
            for key in tag:
                if tag[key] != original_tag[key]:
                    description += '\t' + key + ': ' + str(tag[key]) + '\n'

    return description

