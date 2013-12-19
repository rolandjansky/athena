## @file RegistrationStream_jobOptions.py
## @brief Description of properties of RegistrationStream

 # Loads DEBUG INFO etc..
from AthenaCommon.Constants import *

## get a handle on the top-level Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

##
##  Documentation of RegistrationStream properties
##
include.block ( "RegistrationServices/RegistrationStream_jobOptions.py" )

# Load general jobopts
include ( "RegistrationServices/RegistrationServices_jobOptions.py" )

#
#  Definitions and default values 
#

# ItemList specifies what is added to the collection
#RegStream.ItemList             = []

# Collection type may be ExplicitROOT, ExplicitMySQL or ExplicitMySQLlt
#RegStream.CollectionType       = "ExplicitROOT"

# Connection string to db - needed for non-ROOT collections
#RegStream.Connection           = ""

# Name of the collection - db name for MySQL, file name for ROOT
#RegStream.OutputCollection     = ""

# Possible Open modes: CREATE, CREATE_AND_OVERWRITE (default), UPDATE,
# or READ (not appropriate for RegStream)
#RegStream.CollectionOpenMode   = "CREATE_AND_OVERWRITE"

# Switch to add ref to the input data header - if true, must provide
# one value for ExtraRefNames
#RegStream.WriteInputDataHeader = False

# Boolean, def=True, if true then writes all provenance ref's found in DataHeader
#RegStream.WriteAllProv

# Redundant if WriteAllProv is set True, otherwise is list of XXX which will 
#   show up in collection as XXX_ref if found in provenance record
#RegStream.IncludeProvStages

# Allows you to exclude Provenance stages from the list
#RegStream.ExcludeProvStages

# The name used in the attribute list for the input data header,
# e.g. ESD when making collection from the AOD
#RegStream.ExtraRefNames        = []

# Store from which to look for the data header and attribute list
#from StoreGate.StoreGateConf import StoreGateSvc
#RegStream.Store            = StoreGateSvc()

# Persistency service corresponding to the StoreName
#RegStream.PersistencySvc       = "EventPersistencySvc"

# List of Algorithms that this stream accepts
#RegStream.AcceptAlgs           = []

# List of Algorithms that this stream requires
#RegStream.RequireAlgs          = []

# List of Algorithms that this stream is vetoed by
#RegStream.VetoAlgs             = []



#--------------------------------------------------------------
# Example for Event Collection Registration
#--------------------------------------------------------------
#
# For each output collection, use a separate registration stream
#
# Run RegistrationStream as an algorithm
from RegistrationServices.RegistrationServicesConf import RegistrationStream
topSequence += RegistrationStream( "RegStream1",
                                   CollectionType   = "ExplicitROOT",
                                   OutputCollection = "NewEventCollection",
                                   OutputLevel      = INFO )

## backward compat
RegStream1 = topSequence.RegStream1

# List of DO's to register:

# First element is used to obtain a token to the DataHeader 
RegStream1.ItemList += [ "DataHeader#*" ]
# The second element is the AthenaAttributeList to be written into the
# collection as the tag:
RegStream1.ItemList += [ "AthenaAttributeList#SimpleTag" ]

## the following is to examplify how one would filter the output based on
## the decision of some selection algorithm
## (SelectionAlgs does not really exists, hence the try/except to catch the
## expected failure on importing it)
try:
    from SomeModule.SomeLibConf import SelectionAlgs
    # Assign a selection alg to the RegStream
    topSequence += SelectionAlgs("Select1")
    RegStream1.RequireAlgs =  [ "Select1" ]
    # Save ref in collection to ESD
    RegStream1.ExtraRefNames = [ "ESD" ]
except ImportError, err:
    ## yeah I know : it does not exist...
    pass


