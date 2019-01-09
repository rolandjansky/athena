
Scripts to produce and plot maps of jFex jets and seeds from towers
- towerDump.txt comes from enabling the dumpTowerInfo option of JGTowerReader
- readTowerInfo.py contains functions to read in tower information from towerDump.txt. 
  No need to run it directly, the functions are called by makeMap.py.
- makeMap.py makes maps
- plotMap.py plots them

Maps should be committed to ../../JetTowerLists for the time being 
(other methods are being investigated for the noise files, which this could piggyback on)
