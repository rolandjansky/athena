# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""postInclude to fix AMITag for AOD outputs

The input AMITag is contained in the name of the dataset holding the input
files and should also be in the AMITag attribute of the /TagInfo. At the time
of writing a bug was discovered in the in-file metadata propogation of the
AMITag. This script checks the AMITag from metadata against the one in the name
of in input dataset name. If there is a mismatch the AMITag from the input
dataset name is used to overwrite the value in the in-file metadata for the
output.

    Usage:
        Reco_tf.py --postInclude a2da:EventInfoMgt/AMITagFix.py
"""
import os
import re
from PyUtils.MetaReaderPeeker import metadata
from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ServiceMgr
from RecExConfig.RecFlags import rec

log = logging.getLogger("AMITagFix")
amitagRegex = re.compile('^[a-z][0-9]+')


def TagsFromInputDataset():
    """Returns list of AMITags, e.g. ["e6337", "s3681", "r13145"]

    Looks for the INDS environmental variable to parse the AMITag from the
    input dataset name. If this is not found just use what can be parsed from
    the in-file metadata.
    """
    # from INDS environmental variable
    inputDataSetName = os.environ.get('INDS')
    if inputDataSetName:
        # try extracting the AMITag of the input dataset from its name
        tags = inputDataSetName.split('.')[-1].split('_')
        tags = [tag for tag in tags if amitagRegex.match(tag)]
        log.debug('Built input AMITag from input dataset name: ', tags)
        return tags
    else:
        log.debug("Not INDS environmental variable for reference")
    return list()


def TagsFromMetadata():
    """Returns list of AMITags, e.g. ["e6337", "s3681", "r13145"]

    Looks up the AMITag from the /TagInfo in-file metadata. If this is the well
    known string format, it is converted into a list for convenience.
    """
    try:
        tags = metadata['AMITag']
        if isinstance(tags, str):
            tags = tags.split('_')
        log.debug('Built input AMITag from in-file metadata: ', tags)
        return tags
    except KeyError:
        log.warning("Cannot access /TagInfo/AMITag from in-file metadata")
    except NameError:
        log.warning("Cannot access in-file metadata")
    return list()


def main():
    """Checks the input AMITags for consitency and sets output AMITag

    Retrieves the AMITag according to the input dataset name and the in-file
    metadata. Checks for consistency and sets the AMITag for in-file metadata
    of the output file.
    """
    tags = TagsFromInputDataset()
    tagsFromMeta = TagsFromMetadata()

    # tagsFromName will be empty if INDS is not set
    if not tags or tags == tagsFromMeta:
        # Either we do not have a reference or everything is OK
        return

    log.info("Input AMITag mismatch, overwriting with dataset name")

    # add this workflow's AMITag
    try:
        if rec.AMITag() not in tags:
            tags += [rec.AMITag()]
    except NameError:
        # No recflags to work with, try runArgs
        try:
            if runArgs.AMITag not in tags:
                tags += runArgs.AMITag
        except NameError:
            log.info(("Failed to get workflow AMITag from arguments or "
                      "configuraiton flags"))
        except KeyError:
            log.info("No AMITag provided for this workflow step")
        # If the flags or runArgs cannot be converted to a string

    try:
        # strip things like "" or "NONE"
        tags = [tag for tag in tags if amitagRegex.match(tag)]
        amitag = '_'.join(tags)
        ServiceMgr.TagInfoMgr.ExtraTagValuePairs.update(
                {'AMITag': amitag})
        log.info("Output AMITag in in-file metadata set to {}".format(amitag))
    except NameError:
        log.error("TagInfoMgr not available to set output AMITag")
    except TypeError:
        log.info("AMITags could not be converted to standard string")


# Do the work
main()
