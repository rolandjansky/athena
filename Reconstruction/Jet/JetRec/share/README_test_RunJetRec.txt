Last update 2016-10-24 JE

Use
./go test_RunJetRec 
to run a small scale JetRec validation which dumps the content
of an xAOD file at different stages.

JetRec_jobOptions.py is updated to be compatible with 21.0.8
and JetRec-03-00-91 reconstruction options - note especially the
parameters ptminFilter and calibOpt.

test_RunJetRec.py has been updated to use the following RTT input file:
/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/MC/AOD/mc12_8TeV.117050.PowhegPythia_P2011C_ttbar.simul.AOD.e1728_s1581.20_7_.039772.pool.root.1

Also several variables has been removed from the dump w.r.t. to older
versions (esp. moment tools variables).

Current reference files:
* test_RunJetRec.log.clean.ref.2108 using JetRec-03-00-91
* test_RunJetRec.log.clean.ref.20785 using 20.7.8.5 but with 
  JetRec_jobOptions.py and test_RunJetRec.py from JetRec-03-00-91


When use a R21 xAOD file the following code patch has to be applied
at the moment:

Index: JetRecStandardTools.py
===================================================================
--- JetRecStandardTools.py	(revision 779982)
+++ JetRecStandardTools.py	(working copy)
@@ -224,12 +224,12 @@
 # Prepare a sequence of input constituent modifiers
 from JetRecTools.JetRecToolsConfig import ctm
 jtm += ctm.buildConstitModifSequence( "JetConstitSeq_LCOrigin",
-    OutputContainer='LCOriginTopoClusters',
+    OutputContainer='LCOriginTopoClusters2',
     InputContainer= 'CaloCalTopoClusters',
     modList = [  'lc_origin' ] )
 
 jtm += ctm.buildConstitModifSequence( "JetConstitSeq_EMOrigin",
-    OutputContainer='EMOriginTopoClusters',
+    OutputContainer='EMOriginTopoClusters2',
     InputContainer= 'CaloCalTopoClusters',                                      
     modList = [  'em_origin' ] )
