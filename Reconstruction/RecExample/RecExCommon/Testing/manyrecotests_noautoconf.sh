# set default value if not set
# using parameters substitution operator
export SUFF=${SUFF-_test1}
export NEVT=${NEVT-5}


echo "suffix used : " $SUFF
echo "number of events reconstructed : " $NEVT


echo " default file  write esd"
date
# add to remove doShowSizeStatistics=True ;
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ; doNameAuditor=True ; doWriteESD=True ; doAOD=False ; doWriteAOD=False ; doWriteTAG=False ; PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root' ; doDumpPoolInputContent=True ;  doDumpTES=True " >& def_rdotoesd$SUFF.log  


echo " default read esd write AOD "
date
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_esdtoaod$SUFF.root' ; EvtMax=$NEVT  ; doNameAuditor=True ; readESD=True ; doAOD=True ; doWriteAOD=True ; doWriteESD=False ; doWriteTAG=False ; PoolESDInput=['def_rdotoesd$SUFF.ESD.pool.root'] ; PoolAODOutput='def_esdtoaod$SUFF.AOD.pool.root' ; doDumpPoolInputContent=True ;  doDumpTES=True " >& def_esdtoaod$SUFF.log

echo " default read esd write CBNT "
date
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_esdtocbnt$SUFF.root' ; EvtMax=$NEVT  ; doNameAuditor=True ; readESD=True ; doAOD=False ; doWriteAOD=False ; doWriteESD=False ; doWriteTAG=False ; PoolESDInput=['def_rdotoesd$SUFF.ESD.pool.root']  ; doDumpPoolInputContent=True ;  doDumpTES=True " >& def_esdtocbnt$SUFF.log





echo " default read AOD write CBNT "
date
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_aodtocbnt$SUFF.root' ; EvtMax=$NEVT  ; doNameAuditor=True ; readESD=False ; readAOD=True ; doAOD=False ; doWriteESD=False ; doWriteAOD=False ; doWriteTAG=False ; PoolAODInput=['def_esdtoaod$SUFF.AOD.pool.root']  ; doDumpPoolInputContent=True ; doDumpTES=True " >& def_aodtocbnt$SUFF.log



echo " default read AOD write tag "
date
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_aodtotag$SUFF.root' ; EvtMax=$NEVT  ; doNameAuditor=True ; readESD=False ; readAOD=True ; doAOD=False ; doWriteESD=False ; doWriteAOD=False ; doWriteTAG=True ; PoolAODInput=['def_esdtoaod$SUFF.AOD.pool.root'] ; PoolTAGOutput='def_aodtotag$SUFF.TAG.pool.root' ; doDumpPoolInputContent=True ; doDumpTES=True " >& def_aodtotag$SUFF.log


echo " default read TAG (from AOD) write CBNT "
date
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_tagtocbnt$SUFF.root' ; EvtMax=$NEVT  ; doNameAuditor=True ; readAOD=True ; readTAG=True ; doAOD=False ; doWriteESD=False ; doWriteAOD=False ; doWriteTAG=False ; PoolTAGInput=['def_aodtotag$SUFF.TAG.pool.root']  ; doDumpPoolInputContent=True ; doDumpTES=True " >& def_tagtocbnt$SUFF.log



echo " def do all "
date
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_all$SUFF.root' ; EvtMax=$NEVT ; doNameAuditor=True ; PoolESDOutput='def_all$SUFF.ESD.pool.root' ; PoolAODOutput='def_all$SUFF.AOD.pool.root' ; PoolTAGOutput='def_all$SUFF.TAG.pool.root' ; doDumpPoolInputContent=True ; doDumpTES=True " >& def_all$SUFF.log  


echo " default file  RDO to BS"
date
# add to remove doShowSizeStatistics=True ;
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; doESD=False ; doCBNT=False ; doWriteRDO=False ; doESD=False ; doWriteESD=False ; doWriteAOD=False ; doAOD=False ; doWriteTAG=False ; doWriteBS=True ; doTrigger=True ; EvtMax=$NEVT ; doNameAuditor=True ; doDumpTES=True ; BSRDOOutput='AppName=Athena$SUFF,'  "  >& def_rdotobs$SUFF.log  


echo " default file  read BS write ESD"
date
# add to remove doShowSizeStatistics=True ;
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_bstoesd$SUFF.root' ; EvtMax=$NEVT ; doNameAuditor=True ; doWriteESD=True ; doAOD=False ; doWriteRDO=False ; doWriteTAG=False ; from AthenaCommon.GlobalFlags import globalflags ; globalflags.InputFormat='bytestream' ; globalflags.ConditionsTag='OFLCOND-CSC-00-01-00' ; BSRDOInput=['data_test.00005200.Single_Stream.daq.RAW._lb0000._Athena$SUFF._0001.data'] ;  PoolESDOutput='def_bstoesd$SUFF.ESD.pool.root' ; doDumpPoolInputContent=True ;  doDumpTES=True " >& def_bstoesd$SUFF.log  

echo " default read esd from BS write AOD "
date
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_bstoesdtoaod$SUFF.root' ; EvtMax=$NEVT  ; doNameAuditor=True ; readESD=True ; doAOD=True ; doWriteAOD=True ; doWriteESD=False ; doWriteTAG=False ; PoolESDInput=['def_bstoesd$SUFF.ESD.pool.root'] ; PoolAODOutput='def_bstoesdtoaod$SUFF.AOD.pool.root' ; doDumpPoolInputContent=True ;  doDumpTES=True " >& def_bstoesdtoaod$SUFF.log




#time athena.py -s -c "RootNtupleOutput='ntuple_dc2_lumi10$SUFF.root' ; DetDescrVersion='DC2' ; EvtMax=2 ; from AthenaCommon.GlobalFlags import GlobalFlags ; GlobalFlags.Luminosity.set_high() ; PoolRDOInput = ['LFN:zmumu_highlumi_RDO.pool'] ; from AthenaCommon.DetFlags import DetFlags ; DetFlags.Muon_setOff() ; doWriteESD=True ; PoolESDOutput='dc2_lumi10$SUFF.ESD.pool.root'; PoolAODOutput='dc2_lumi10$SUFF.AOD.pool.root' ; PoolTAGOutput='dc2_lumi10$SUFF.TAG' ; doDumpPoolInputContent=True " >& dc2_lumi10$SUFF.log
#date


sumfile=summary$SUFF.txt
rm -f $sumfile
touch $sumfile

grep -e "processing event" -e "leaving with" *$SUFF.log | tee -a $sumfile
echo "****************************************************" >> $sumfile
grep -B 5 -A 20 Traceback *$SUFF.log | tee -a $sumfile
echo "****************************************************" >> $sumfile
grep -B 1 " ERROR " *$SUFF.log   >> $sumfile
echo "****************************************************" >> $sumfile
grep -A 15 "unchecked return" *$SUFF.log   >> $sumfile
echo "****************************************************" >> $sumfile
grep -B 1 WARNING *$SUFF.log   >> $sumfile
echo "****************************************************" >> $sumfile

grep -i virtual *$SUFF.log >> $sumfile
echo "****************************************************" >> $sumfile




checklog.py *$SUFF.log >> $sumfile

echo "Tests completed ! \a"
echo "\a"
