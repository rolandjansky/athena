# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloClusterCorrectionFlags.py,v 1.5 2009-04-21 21:16:21 ssnyder Exp $
#
# File: CaloClusterCorrection/python/CaloClusterCorrectionFlags.py
# Created: Oct 2007, sss
#

"""   Define global flags for cluster corrections.
"""


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
from CaloClusterCorrection.constants \
     import CALOCORR_JO, CALOCORR_POOL, CALOCORR_COOL


class DoSlidingWindowCellWeights (JobProperty):
    """ If true, then reweight cells to prevent double-counting between clusters.
"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False


class DoSlidingWindowRemoveDuplicates (JobProperty):
    """ If true, then among groups of clusters very close to each other,
remove all clusters but one.
"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class DoSlidingWindowRemoveBad (JobProperty):
    """ If true, remove clusters with energy below a threshold.
"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    

class DefaultSource (JobProperty):
    """ Defines from where we should try to construct the tools.
May be one of CALOCORR_JO, CALOCORR_POOL, CALOCORR_COOL.
May also be a list of them, to try in order.
It may also be set to a string defined in poolfiles to try to
read all tools from that file (except those marked as nopool).
"""
    statusOn     = True
    allowedTypes = ['str', 'list']
    StoredValue  = [CALOCORR_COOL, CALOCORR_POOL, CALOCORR_JO]


class CaloSwWhichCorrection (JobProperty):
    """Override the default correction version for sliding window clusters.
"""
    statusOn     = False
    allowedTypes = 'str'
    StoredValue  = None


class CaloTopoEMWhichCorrection (JobProperty):
    """Override the default correction version for EM topo clusters.
"""
    statusOn     = False
    allowedTypes = 'str'
    StoredValue  = None

class CaloSwCorrectionArgs (JobProperty):
    """This flag allows setting additional correction arguments for
sliding window clusters.  It should normally be off, but it may be
set for testing new correction versions.
"""
    statusOn = False
    allowedTypes = 'dict'
    StoredValue = {}

class CaloTopoEMCorrectionArgs (JobProperty):
    """This flag allows setting additional correction arguments for
EM topo clusters.  It should normally be off, but it may be
set for testing new correction versions.
"""
    statusOn = False
    allowedTypes = 'dict'
    StoredValue = {}

class CaloSwGeneration (JobProperty):
    """Correction generation string that's embedded in database tags.
This string gets changed every time the database is updated."""
    statusOn     = False
    allowedTypes = 'str'
    StoredValue  = None


class CaloTopoEMGeneration (JobProperty):
    """Correction generation string that's embedded in database tags.
This string gets changed every time the database is updated."""
    statusOn     = False
    allowedTypes = 'str'
    StoredValue  = None


class DBSubdetName (JobProperty):
    """This properly allows overriding the subdetector name used for
database connections.  It's a dictionary, the keys of which are the
correction classes (normally either `CaloSwClusterCorrections'
or `EMTopoClusterCorrections').  If the correction class isn't
found, we use a key of None instead as a default.

The subdetector name is normally `CALO'.  However, it can be set to the
name of a local sqlite file to force reading COOL information from there."""
    statusOn = True
    allowedTypes = 'dict'
    StoredValue = {None : "CALO"}


class CaloClusterCorrectionFlags (JobPropertyContainer):
    """ Flags for cluster corrections.
    """
    pass

# add the 'Configured' flags container to the top container 
jobproperties.add_Container(CaloClusterCorrectionFlags)


for i in [DoSlidingWindowCellWeights,
          DoSlidingWindowRemoveDuplicates,
          DoSlidingWindowRemoveBad,
          DefaultSource,
          CaloSwWhichCorrection,
          CaloTopoEMWhichCorrection,
          CaloSwCorrectionArgs,
          CaloTopoEMCorrectionArgs,
          CaloSwGeneration,
          CaloTopoEMGeneration,
          DBSubdetName,
          ]:
    jobproperties.CaloClusterCorrectionFlags.add_JobProperty(i)

caloClusterCorrectionFlags = jobproperties.CaloClusterCorrectionFlags
