
2005-10-27:	In TrigCaloEvent directory :
		TrigCaloCluster.h -> base classe of
		the package.
		TrigEMCluster.h -> egamma object
		TrigTauCluster.h -> tau object
		TrigEMCluster.h -> container for 
		egamma object
		TrigTauCluster.h -> container for
		tau object
		TestEDM.h -> Writes the classes
		to StoreGate and tried to read it
		back.
		In the src directory -> the implementation
		of the classes described above.
		In the docs directory -> this file.
		In the share directory -> very simple
		athena python script to test clusters
		and containers.
		If the package is configured you can
		just get the TestEDM.py jobOption file
		and do:
		athena.py TestEDM.py
