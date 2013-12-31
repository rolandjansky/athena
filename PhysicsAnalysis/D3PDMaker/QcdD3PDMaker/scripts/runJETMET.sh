#for example for Reco_trf.py

inputFile=./AOD.pool.root
outputNTUPFileName=qcd.root

Reco_trf.py inputAODFile=$inputFile maxEvents=10 outputNTUP_JETMETFile=${outputNTUPFileName} --ignoreerrors=True autoConfiguration=everything 
