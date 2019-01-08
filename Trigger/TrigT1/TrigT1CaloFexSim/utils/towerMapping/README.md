
Scripts to produce and plot maps of jFex jets and seeds from towers
- readTowerInfo.py contains functions to read in tower information from out.txt,
  which was dumped by not-committed-yet changes to JGTowerReader.
  No need to run it directly, the functions are called by makeMap.py.
- makeMap.py makes maps
- plotMap.py plots them

Maps should be committed to ../../JetTowerLists for the time being (other methods are being investigated for the noise files, which this could piggyback on)
