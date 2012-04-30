################################
#FCAL stuff
################################
get_files -symlink -data " FCal*.dat "
#ln -fs $LARIDENTIFIERROOT/share/FEB* . 
cp -f ../share/RecExTB*.py .

# P15 245 GeV medium gain eta=2 phi=10
ln -fs /castor/cern.ch/atlas/testbeam/emebh6/2002/rd13_run215204.dat .
# photon run
ln -fs /castor/cern.ch/atlas/testbeam/emebh6/2000/rd13_run209763.dat .
ln -fs rd13_run215204.dat ZEBRA.P
# byte stream
ln -fs  /afs/cern.ch/atlas/offline/data/testfile/RUN218266digitstdc_H8.re .
ln -fs  RUN218266digitstdc_H8.re RawEvent.re
ln -fs  /afs/cern.ch/atlas/offline/data/testfile/Raw_EC02_13149.re RawEvent_EC02.re
ln -fs  /afs/cern.ch/atlas/offline/data/testfile/run431.bstr RawEvent_H6.re
ln -fs  /afs/cern.ch/atlas/offline/data/testfile/Raw_H8_1769.data RawEvent_H8.re
##################################
# Inner Detector stuff
#################################
# Get PoolFileCatalog for simulated Pixel+SCT+TRT RDOs
#ln -s /afs/cern.ch/user/t/tcorneli/public/PoolFileCatalog.xml .
ln -fs ../share/PoolFileCatalog.xml .
# Get mapping files for BS converter
get_files -symlink -data " PixelBarrelMUR2RESrcIdMapping.dat "
get_files -symlink -data " PixelBarrelMUR2TEMapping.dat "
get_files -symlink -data " PixelEndcapMUR2RESrcIdMapping.dat "
get_files -symlink -data " PixelEndcapMUR2TEMapping.dat "
get_files -symlink -data " SCTBarrelMUR2RESrcIdMapping.dat "
get_files -symlink -data " SCTBarrelMUR2TEMapping.dat "
get_files -symlink -data " SCTEndcapMUR2RESrcIdMapping.dat "
get_files -symlink -data " SCTEndcapMUR2TEMapping.dat "
get_files -symlink -data " SCT_TB03_EndcapMUR2RESrcIdMapping.dat"
# Get calibartion constants for beam chambers
get_files -symlink -data " H8BPCCalib.txt"
get_files -symlink -data " H6BPCCalib.txt"
get_files -symlink -data " BPCAlignment_2004TB_RunI.txt"
get_files -symlink -data " BPCAlignment_2004TB_RunII.txt"

#still needed by PartPropSvc
get_files -symlink -data PDGTABLE.MeV

# in order to use the afs resident copy of the POOL file catalogue
# and avoid a connection to the atlobk02 server
ln -s  /afs/cern.ch/user/a/atlcond/ctbrep/poolcond  poolcond
