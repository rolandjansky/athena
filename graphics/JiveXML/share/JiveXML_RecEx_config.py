## To all to do 'doJiveXML=True' on _any_ input: 
## Switch for AOD/non-AOD   jpt 15Sep09
include ( "JiveXML/JiveXML_jobOptionBase.py" )
include ("RecJiveXML/RecJiveXML_DataTypes.py")
## GEN_AOD may not be present in ESDs. Switch back to default TruthEvent
#### for ESD or RDO:
if not jp.Rec.readAOD():
   include ("TruthJiveXML/TruthJiveXML_DataTypes.py")
   theTruthTrackRetriever.StoreGateKey = "TruthEvent"
#### for RDO only (DetFlags not properly defined in ESDs):
if jp.Rec.readRDO(): 
   InDetFlags.doSlimming=False
   if DetFlags.detdescr.ID_on() :
      include ("InDetJiveXML/InDetJiveXML_DataTypes.py")
      include ("TrkJiveXML/TrkJiveXML_DataTypes.py")
   if DetFlags.detdescr.Calo_on():
      include ("CaloJiveXML/CaloJiveXML_DataTypes.py")
   if DetFlags.detdescr.Muon_on():
      include ("MuonJiveXML/MuonJiveXML_DataTypes.py")
if jp.Rec.readESD():
   InDetFlags.doSlimming=False
   include ("InDetJiveXML/InDetJiveXML_DataTypes.py")
   include ("TrkJiveXML/TrkJiveXML_DataTypes.py")
   include ("CaloJiveXML/CaloJiveXML_DataTypes.py")
   include ("MuonJiveXML/MuonJiveXML_DataTypes.py") 
#### Add this at the end, otherwise will double-insert
#### STr, RVx, ETMis: 
## old-style (rel17) AOD retrievers, to be removed soon:
#include ("AnalysisJiveXML/AnalysisJiveXML_DataTypes.py")
## new-style (rel19) AOD retrievers, for Run2:
include ("xAODJiveXML/xAODJiveXML_DataTypes.py")
include ("TrigJiveXML/TrigJiveXML_DataTypes.py")
