# File: AthExJobOptions/AthExJobOptions_CustomTopAlgorithmJopOptions.py
# Author: Wim Lavrijsen <WLavrijsen@lbl.gov>

# This Athena job consists of algorithms that loop over events;
# here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Get hold of the algorithms that we want to run
from AthExJobOptions.CustomTopAlgorithm import CustomTopAlgorithm
#    ^^package name  ^^user-authored file      ^^custom class

# Add the algorithm
job += CustomTopAlgorithm( 'MyCustomAlg' )

# Verify the set value, not this is from the ctor, not setDefaults()
assert( job.MyCustomAlg.RndmFactor == 101. )

# But it can be overwritten without problem:
job.MyCustomAlg.RndmFactor = 33
assert( job.MyCustomAlg.RndmFactor == 33. )


# Some global settings (typically, you *only* want to
# do this in the final options file, never in a fragment):
theApp.EvtMax = 2

# Print the final job for reference purposes
print job

# End of AthExJobOptions_CustomTopAlgorithmJobOptions.py
