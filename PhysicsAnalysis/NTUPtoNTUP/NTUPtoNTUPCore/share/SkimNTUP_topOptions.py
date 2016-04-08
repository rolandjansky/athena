include.block ("SkimNTUP_topOptions.py")


from AthenaCommon.Logging import logging
logSkimNTUP_topOptions = logging.getLogger( 'SkimNTUP_topOptions' )
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc,theApp,ServiceMgr

theApp.EvtMax = 0

### NTUP->NTUP
ntupFlags=jobproperties.SkimNTUP_ProdFlags
print "Content of rec.SkimNTUPScripts = %s", ntupFlags.SkimNTUPScripts
if ntupFlags.SkimNTUPScripts()!=[]:
  for ntup in ntupFlags.SkimNTUPScripts():
      NTUPName = str(ntup)
      print "Including %s...",NTUPName
      include(ntup)
      pass
else:
  print "no scripts in SkimNTUPScripts..."
