

from CaloRec.CaloRecFlags import jobproperties

theEMTopoThreshold=jobproperties.CaloRecFlags.emTopoClusterThreshold()
print theEMTopoThreshold

ETT=theEMTopoThreshold.startswith("633")
if(ETT) : 
    try:
        from CaloRec.CaloClusterTopoEMGetters import CaloClusterTopoEM633Getter
        CaloClusterTopoEM633Getter()
        #from CaloRec.CaloCell2ClusterMapperGetters import EMCell2TopoClusterMapperGetter
        #EMCell2TopoClusterMapperGetter()
    except Exception:
        treatException("Problem with 633EMTopoCluster. Switched off")

ETT=theEMTopoThreshold.startswith("430")
if(ETT) : 
    try:
        from CaloRec.CaloClusterTopoEMGetters import CaloClusterTopoEM430Getter
        CaloClusterTopoEM430Getter()
        #from CaloRec.CaloCell2ClusterMapperGetters import EMCell2TopoCluster430MapperGetter
        #EMCell2TopoCluster430MapperGetter()
    except Exception:
        treatException("Problem with 430EMTopoCluster. ")


