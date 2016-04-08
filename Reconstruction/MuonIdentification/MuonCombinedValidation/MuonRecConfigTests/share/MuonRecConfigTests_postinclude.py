from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt")

# import AthenaServices.AthDsoLogger as ADL
# dso = ADL.DsoLogger() # monitor library sizes

# Aesthetic track summary for all collections
include("MuonTrackPerformance/MuonTrackStatistics_bsJobOptions.py")
MuonTrackStatistics.writeToFile = True
