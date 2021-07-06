#====================================================================
# BPHY22.py
# This an example job options script showing how to set up a
# derivation of the data using the derivation framework.
# It requires the reductionConf flag BPHY12 in Reco_tf.py
#====================================================================

# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# This is a dummy file thus just printing something

print("")
print("BPHY22 dummy file ... doing nothing.")
print("")
print("Please make sure that all local variables in this python")
print("script are prefixed by BPHY22_ in order to avoid collisions")
print("in case this derivation format is run in a train with others.")
print("Please ensure that it is python3 compatible e.g. by using")
print("print() instead of just print withouth parentheses.")
print("")
