# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""Utilities to get and set the AMITag in the metadata

The input AMITag is contained in the AMITag attribute of the /TagInfo in-file
metadata object. In the grid environment this may be checked against the AMITag
in the name of the input dataset, to be looked up from the INDS environmental
variable.

The AMITag of the current processing step is provided by command line argument
to the transform. In the reconstruction environment it may be taken from the
RecFlags.

The tool provides a function to get and check the AMITags from the input and
add the output AMITag. In addition a script can do all this and set the value
for the output /TagInfo object.

    Usage:
        AMITagHelper.SetOutputAMITag(runargs.AMITag)
"""
import os
import re
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.Logging import logging
try:
    from PyJobTransformsCore.runargs import RunArguments
except ModuleNotFoundError:
    from PyJobTransforms.trfJobOptions import RunArguments
from PyUtils.MetaReaderPeeker import metadata
try:
    from RecExConfig.RecFlags import rec
except ModuleNotFoundError:
    pass

log = logging.getLogger('AMITagHelper')
amitagRegex = re.compile('^[a-z][0-9]+')


def InputAMITags():
    """Returns AMITag of input, e.g. e6337_s3681_r13145

    Looks up AMITag of input from INDS environmental variable and in-file
    metadta. If nothing can be retrieved from the in-file metadata return value
    from INDS, otherwise return value from metadata. Inform about differences.
    """
    tags = []
    tagsFromINDS = []
    # from INDS environmental variable
    inputDataSetName = os.environ.get('INDS')
    if inputDataSetName:
        # try extracting the AMITag of the input dataset from its name
        tagsFromINDS = inputDataSetName.split('.')[-1].split('_')
        tagsFromINDS = [tag for tag in tagsFromINDS if amitagRegex.match(tag)]
        log.debug('AMITag from input dataset name: {}'.format(tagsFromINDS))

    # from in-file metadata
    try:
        tags = metadata['AMITag']
        if isinstance(tags, str):
            tags = tags.split('_')
    except KeyError:
        log.info("Cannot access /TagInfo/AMITag from in-file metadata")
        return tagsFromINDS
    except NameError:
        log.info("Cannot access in-file metadata")
        return tagsFromINDS

    if tagsFromINDS and tags != tagsFromINDS:
        log.warning("AMITag mismatch, check metadata of input dataset")

    # Well that is weird, evgen?
    return tags


def OutputAMITag(runArgs=None):
    """Return value of new AMITag for this step in the workflow

    Look up AMITag for this processing step from the command line arugments and
    the reconstruction flags. Return the value from the CLI arguments if it is
    set, otherwise try the value from the confi flags. Inform about differences.
    """
    if not runArgs:
        runArgs = RunArguments()

    flagTag = ''
    try:
        flagTag = rec.AMITag()
    except (NameError, AttributeError,):
        pass

    cliTag = ''
    try:
        cliTag = runArgs.AMITag
    except AttributeError:
        pass

    if flagTag and flagTag != cliTag:
        log.info('AMITag from recFlags does not match value form command line')

    if cliTag:
        return cliTag
    else:
        return flagTag


def SetAMITag(outputTag=None, inputTags=None, runArgs=None):
    """Add input and output AMITag values and set result in in-file metadata

    If no values are provided for the input and output AMITags we look the up.
    The AMITags will be combined with '_' as delimiters. The result is set in
    the in-file metadata.
    """
    if not outputTag:
        outputTag = OutputAMITag(runArgs)
    if not inputTags:
        inputTags = InputAMITags()

    try:
        if outputTag not in inputTags:
            tags = inputTags + [outputTag]
        tags = [tag for tag in tags if amitagRegex.match(tag)]
        amitag = '_'.join(tags)
        if amitag:
            ServiceMgr.TagInfoMgr.ExtraTagValuePairs.update(
                    {'AMITag': amitag})
            log.info("Output AMITag in in-file metadata set to {}".format(amitag))
        else:
            log.debug("Not adding empty AMITag to /TagInfo")
    except NameError:
        log.warning("TagInfoMgr not available to set output AMITag")
    except TypeError:
        log.info("AMITags could not be converted to well-known format")
