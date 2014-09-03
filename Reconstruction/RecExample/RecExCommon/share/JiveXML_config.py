include ( "JiveXML/JiveXML_jobOptionBase.py" )

if rec.doTruth():
    include ( "JiveXML/DataTypes_Truth.py" )

if DetFlags.detdescr.ID_on() :
    include ( "JiveXML/DataTypes_InDet.py" )

if DetFlags.detdescr.Calo_on():
    include ( "JiveXML/DataTypes_Calos.py" )

if DetFlags.detdescr.Muon_on() :
    include ( "JiveXML/DataTypes_Muons.py" )

include ( "JiveXML/DataTypes_Reco.py" )

if rec.doTrigger():
    include ( "JiveXML/DataTypes_Trig.py" )


