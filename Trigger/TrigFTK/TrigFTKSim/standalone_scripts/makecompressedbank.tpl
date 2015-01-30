#
# sample steering for executable
#    makecompressedbank
#

LOGGING_PRINT_LEVEL 5

# give the name of a "cached bank" file to read
#NINPUTS 1
#../TDAQTDRConf/patterns_raw_8L_15x16x36Ibl_4M_reg24.pcache.root


# give the name of an itermediate input/output file
# the "cached bank file" is converted to the intermediate file
#PATTERNSBYSECTOR ../TDAQTDRConfTest/pcache_by_sector.reg24.root

# give the name of a target root file, containing the compressed bank
#ROOTFILEOUT  ../TDAQTDRConfTest/lookup_by_ssid.reg24.compressed.root

# give the name of a target pcache.root file, containing patterns 
#  ordered exactly as for the compressed bank
#  this is useful for comparing roads obtained from teh old/new algorithm
#  by their pattern ID
#CACHEDBANKFILEOUT ../TDAQTDRConfTest/patterns_ordered.reg24.pcache.root

# for debugging: write out binary file
#BINARYFILEOUT ../TDAQTDRConfTest/lookup_by_ssid.reg24.compressed.bin
# for debugging: read in root file
ROOTFILEIN  ../TDAQTDRConfTest/compressed.root/patterns_raw_8L_15x16x36Ibl_4M_reg24.compressed.root
# for debugging: read in binary file
#BINARYFILEIN  ../TDAQTDRConfTest/compressed.binary/patterns_raw_8L_15x16x36Ibl_4M_reg24.compressed.binary

# specify FTK geometry -- has to match geomenry in pcache.root file
PMAP ../config/map_file/raw_8LcIbl123.pmap
RMAP ../config/map_file/raw_12Libl.tmap
SSMAP ../config/ss_file/raw_30x32x72Ibl.ss
TSP_SSMAP ../config/ss_file/raw_15x16x36Ibl.ss

#debugging: give a list of SSIDs and perform the pattern finding
# the SSID are given as:  ssid.plane    (0<=plane<8)
# DC_SSID 8 19215.0 32415.1 42815.2 30505.3 38105.4 45705.5 45705.6 53305.7
# DC_SSID 7 19215.0 32415.1 42815.2 30505.3 38105.4 45705.5 45705.6
#
#note: alternatively, use TSP_SSID to specify a list of SSIDs in TSP space
#
