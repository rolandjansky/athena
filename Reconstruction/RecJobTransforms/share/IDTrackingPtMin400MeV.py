#syntax :
# preInclude_r2e=RecJobTransforms/IDTrackingPtMin400MeV.py
# in addition one can change the effective cut
# preExec_r2e='rec.UserFlags=["InDetNewTrackingCuts._ConfiguredNewTrackingCuts__minPT=300."]'
# one should use append_preInclude/Exec if used in conjunction with AMI=..

b=['from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts','InDetNewTrackingCuts=ConfiguredNewTrackingCuts("Offline")','InDetNewTrackingCuts._ConfiguredNewTrackingCuts__minPT=400.0']
#if rec.UserFlags is a non empty string, make it a vector
a=rec.UserFlags()
if a!="" and type(a)==type(""):
    a=[a]

if type(a)==type([]) and len(a)>0:    
    rec.UserFlags=b+a
else:
    rec.UserFlags=b

del a,b
print "IDTrackingPtMin400MeV.py setting rec.UserFlags to ", rec.UserFlags()

