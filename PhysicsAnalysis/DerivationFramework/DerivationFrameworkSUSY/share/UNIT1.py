#********************************************************************
# UNIT.py 
# Copy input xAOD to output xAOD.
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *

### Set up stream
streamName = derivationFlags.WriteDAOD_UNIT1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_UNIT1Stream )
UNIT1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
UNIT1Stream.AcceptAlgs(["UNIT1KernelSkim"])

### Init

# stream-specific sequence for on-the-fly jet building
SeqUNIT1 = CfgMgr.AthSequencer("SeqUNIT1")
DerivationFrameworkJob += SeqUNIT1



#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqUNIT1 += CfgMgr.DerivationFramework__DerivationKernel(
                            "UNIT1KernelSkim"
                          )


from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )


