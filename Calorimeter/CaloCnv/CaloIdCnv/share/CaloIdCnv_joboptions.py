# Avoid double inclusion:
include.block("CaloIdCnv/CaloIdCnv_joboptions.py")
#
#  Joboptions for the loading of the of CaloIdManager are not needed anymore
#  after the era of migration to Configurables (theApp.Dll statement not needed anymore)
#  Following include inserted only to ease migration of users from CaloIdConv to CaloConditions
include( "CaloConditions/CaloConditions_jobOptions.py" )


