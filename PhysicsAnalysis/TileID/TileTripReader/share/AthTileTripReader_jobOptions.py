include.block( "TileTripReader/AthTileTripReader_jobOptions.py" )


from TileTripReader.TileTripReaderConf import \
     AthTileTripReader as ConfiguredTileTripReader

AthTileTripReader = ConfiguredTileTripReader(TileTripFile="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/TileTripReader/CompleteTripList_2015-2018.root",dR=0.4)

ToolSvc += AthTileTripReader
print      AthTileTripReader
