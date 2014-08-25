include.block( "MissingETPerformance/TopoClusterVariablesTool_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     TopoClusterVariablesTool as ConfiguredTopoClusterVariablesTool
TopoClusterVariablesTool = ConfiguredTopoClusterVariablesTool(

    CaloClusterContainerKey     =    'CaloCalTopoCluster',

    )

ToolSvc += TopoClusterVariablesTool
print      TopoClusterVariablesTool
