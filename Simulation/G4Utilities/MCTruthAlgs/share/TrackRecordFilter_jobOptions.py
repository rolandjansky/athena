theApp.Dlls  += [ "MCTruthAlgs"]
theApp.TopAlg += ["TrackRecordFilter"]

try:
    from AthenaCommon.GlobalFlags import globalflags
    ddv=globalflags.DetDescrVersion()
except Exception:
    # fall back use DetDescrVersion
    print "WAR"+"NING globalflags.DetDescrVersion undefined. using DetDescrVersion instead"
    ddv=DetDescrVersion

if (ddv[0:3]=="DC1" or ddv[0:3]=="DC2" or  ddv[0:4]=="Rome" or ddv=="ATLAS-DC3-01" or ddv=="ATLAS-DC3-02" ):
    TrackRecordFilter = Algorithm("TrackRecordFilter")
    TrackRecordFilter.inputName="MuonEntryRecord"

del ddv

