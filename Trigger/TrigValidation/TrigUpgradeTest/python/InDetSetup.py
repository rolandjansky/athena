def inDetSetup():
  from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
  InDetTrigFlags.doPixelClusterSplitting = False
  
  # PixelLorentzAngleSvc and SCTLorentzAngleSvc
  from AthenaCommon.Include import include
  include("InDetRecExample/InDetRecConditionsAccess.py")
