include.block("LArClusterRec/LArClusterCollisionTime_jobOptions.py")

from LArClusterRec.LArClusterRecConf import LArClusterCollisionTimeAlg

topSequence+=LArClusterCollisionTimeAlg(OutputName="ClusterCollTime",
                                        InputName="egammaClusters",
                                        )

