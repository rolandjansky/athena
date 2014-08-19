#configuration files
echo "RecExConfig topOptions.py : all in one"
get_files -jo RecExConfig/topOptions.py


#make a soft link to default file name
ln -fs topOptions.py jobOptions.py

echo "Now build PoolFileCatalog.xml file"

# ATLAS-CSC-01-02-00 top file 
pool_insertFileToCatalog $ATLASTESTDATA/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root
FCregisterLFN -p $ATLASTESTDATA/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root -l top_CSC-01-02-00_RDO_extract.pool

