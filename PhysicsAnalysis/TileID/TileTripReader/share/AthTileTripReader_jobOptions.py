include.block( "TileTripReader/AthTileTripReader_jobOptions.py" )


from TileTripReader.TileTripReaderConf import \
     AthTileTripReader as ConfiguredTileTripReader

AthTileTripReader = ConfiguredTileTripReader(TileTripFile="TileTripReader/CompleteTripList_2011-2012.root",dR=0.4)

ToolSvc += AthTileTripReader
print      AthTileTripReader
