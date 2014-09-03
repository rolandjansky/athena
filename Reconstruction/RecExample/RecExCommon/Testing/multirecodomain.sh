export NEVT=5
export COMMONSUFF="_noesd"

cat commonconfig.py

export SUFF=_nothing$COMMONSUFF
echo $SUFF
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.doCalo=False; rec.doInDet=False ; rec.doMuon=False ; rec.doJetMissingETTag=False ; rec.doEgamma=False ; rec.doMuonCombined=False ; rec.doTau=False ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ; PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root'  " commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  


export SUFF=_muon$COMMONSUFF
echo $SUFF
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.doCalo=False; rec.doInDet=False ; rec.doMuon=True ; rec.doJetMissingETTag=False ; rec.doEgamma=False ; rec.doMuonCombined=False ; rec.doTau=False ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ; PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root'  " commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  


 
export SUFF=_calo$COMMONSUFF
echo $SUFF
time athena.py -s -c " from RecExConfig.RecFlags import rec; rec.doCalo=True; rec.doInDet=False ; rec.doMuon=False ; rec.doJetMissingETTag=False ; rec.doEgamma=False ; rec.doMuonCombined=False ; rec.doTau=False ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF';  RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ; PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root'  " commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  

export SUFF=_indet$COMMONSUFF
echo $SUFF
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.doCalo=False; rec.doInDet=True ; rec.doMuon=False ; rec.doJetMissingETTag=False ; rec.doEgamma=False ; rec.doMuonCombined=False ; rec.doTau=False ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF'; RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ; PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root' "  commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  

export SUFF=_idmuon$COMMONSUFF
echo $SUFF
time athena.py -s -c " from RecExConfig.RecFlags import rec; rec.doCalo=False; rec.doInDet=True ; rec.doMuon=True ; rec.doJetMissingETTag=False ; rec.doEgamma=False ; rec.doMuonCombined=False ; rec.doTau=False ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF';  RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ;  PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root'  "  commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  

export SUFF=_idmuoncalo$COMMONSUFF
echo $SUFF
time athena.py -s -c "doiPatRec=False ; from RecExConfig.RecFlags import rec; rec.doCalo=True; rec.doInDet=True ; rec.doMuon=True ; rec.doJetMissingETTag=False ; rec.doEgamma=False ; rec.doMuonCombined=False ; rec.doTau=False ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF'; doDumpProperties=True ; RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ; doNameAuditor=True ; doWriteESD=True ; doAOD=False ; doWriteAOD=False ; doWriteTAG=False ; PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root' ; doDumpPoolInputContent=True ;  doDumpTES=True ;  doDetailedAuditor=True ; doCBNT=False   " commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  



export SUFF=_idmuoncalomuoncombined$COMMONSUFF
echo $SUFF
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.doCalo=True; rec.doInDet=True ; rec.doMuon=True ; rec.doJetMissingETTag=False ; rec.doEgamma=False ; rec.doMuonCombined=True ; rec.doTau=False ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF'; RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ;  PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root' "  commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  


export SUFF=_idmuoncaloegamma$COMMONSUFF
echo $SUFF
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.doCalo=True; rec.doInDet=True ; rec.doMuon=True ; rec.doJetMissingETTag=False ; rec.doEgamma=True ; rec.doMuonCombined=False ; rec.doTau=False ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF';  RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ; PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root'  "  commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  



export SUFF=_idmuoncaloegammamucomb$COMMONSUFF
echo $SUFF
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.doCalo=True; rec.doInDet=True ; rec.doMuon=True ; rec.doJetMissingETTag=False ; rec.doEgamma=True ; rec.doMuonCombined=True ; rec.doTau=False ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF';  RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ; PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root'  "  commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  


export SUFF=_idmuoncaloegammamucombjet$COMMONSUFF
echo $SUFF
time athena.py -s -c " from RecExConfig.RecFlags import rec; rec.doCalo=True; rec.doInDet=True ; rec.doMuon=True ; rec.doJetMissingETTag=True ; rec.doEgamma=True ; rec.doMuonCombined=True ; rec.doTau=False ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF'; RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ;  PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root' ;  " commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  


export SUFF=_idmuoncaloegammamucombjettau$COMMONSUFF
echo $SUFF
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.doCalo=True; rec.doInDet=True ; rec.doMuon=True ; rec.doJetMissingETTag=True ; rec.doEgamma=True ; rec.doMuonCombined=True ; rec.doTau=True ; rec.doTrigger=False ; rec.OutputSuffix='$SUFF';  RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ; PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root' ;  "  commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  


export SUFF=_idmuoncaloegammamucombjettautrig$COMMONSUFF
echo $SUFF
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.doCalo=True; rec.doInDet=True ; rec.doMuon=True ; rec.doJetMissingETTag=True ; rec.doEgamma=True ; rec.doMuonCombined=True ; rec.doTau=True ; rec.doTrigger=True ; rec.OutputSuffix='$SUFF';  RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ;  PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root' "  commonconfig.py myTopOptions.py >& def_rdotoesd$SUFF.log  



export SUFF=_all_plusesd$COMMONSUFF
echo $SUFF
time athena.py -s -c "from RecExConfig.RecFlags import rec; rec.doCalo=True; rec.doInDet=True ; rec.doMuon=True ; rec.doJetMissingETTag=True ; rec.doEgamma=True ; rec.doMuonCombined=True ; rec.doTau=True ; rec.doTrigger=True ; rec.OutputSuffix='$SUFF';  RootNtupleOutput='ntuple_def_rdotoesd$SUFF.root' ; EvtMax=$NEVT ;  PoolESDOutput='def_rdotoesd$SUFF.ESD.pool.root' "  commonconfig.py reenable.py myTopOptions.py >& def_rdotoesd$SUFF.log  

 
