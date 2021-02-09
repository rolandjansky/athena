
# in the CTP triggers (L1 items) get accepted if they satisfy the physics signal, e.g. 2xMU10 or XE60&EM20 
# and they satisfy the bunchgroup requirement, i.e. the BunchCrossing number (BCID) must be in the list that defines the bunchgroup

# the CTPSimulation simulates that behavior
# a) when being executed on data it takes the bunchgroup definitions that correspond to the data and the BCID of the event
# b) when being executed on MC it takes the bunchgroup definition from the HLT menu configuration file, which only defines bunchgroups 0,1,and 7 and only for BCID=1, and it assumes the event has BCID=1

# For Heavy Ion MC simulation with data overlay the setup is a bit complicated:
# The bunchgroup definition in the Reco step (RDOtoRDOtrigger) is taken from the overlayed data, however, the CTPSimulation runs on MC and assumes BCID=1. 
# To fix this one needs to force it to use bcid from the event

topSequence.CTPSimulation.ForceUsingBCIDforBunchGroupEval = True

