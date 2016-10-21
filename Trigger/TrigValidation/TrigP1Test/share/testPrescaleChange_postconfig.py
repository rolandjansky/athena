# Change the partition name in HLTConfigSvc to avoid collissions between
# parallel running ATN tests
from GaudiPython.Bindings import iProperty
from TrigP1Test.PrescaleChange import partitionName

prop = iProperty('HLTConfigSvc')
prop.PartitionName = partitionName
