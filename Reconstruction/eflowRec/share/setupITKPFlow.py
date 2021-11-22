from AthenaCommon.GlobalFlags import globalflags

from RecExConfig.InputFilePeeker import inputFileSummary
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
globalflags.DetDescrVersion = inputFileSummary['geometry']

# New geotags do not need any special pre- and post-includes
# This xmlTags list is kept for backward compatibility and should not be updated
xmlTags = [
            # step 1.7
            ["ATLAS-P2-ITK-09","ExtBrl_4",""],
            ["ATLAS-P2-ITK-10","InclBrl_4",""],
            # step 1.8 and step 1.9, nominal (run4) beam-pipe
            # step 1.9, run2 beam-pipe
            ["ATLAS-P2-ITK-13","InclBrl_4_OptRing",""],
            # step 3
            ["ATLAS-P2-ITK-17","InclBrl_4","InclinedAlternative"],
            # step 2
            ["ATLAS-P2-ITK-19","InclBrl_4","InclinedQuads"],
            ["ATLAS-P2-ITK-20","InclBrl_4","InclinedDuals"],
            # step 3.1
            ["ATLAS-P2-ITK-22-00","InclBrl_4","InclinedAlternative"],
            ["ATLAS-P2-ITK-22-01","InclBrl_4","InclinedAlternative"],
            ["ATLAS-P2-ITK-22-02","",""],
            ]

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags

foundGeoTag = False
for geoTag, layoutDescr, layoutOption in xmlTags:
   if (globalflags.DetDescrVersion().startswith(geoTag)):
      foundGeoTag = True
      print "preIncludes for ",layoutDescr, " layout"
      if (layoutOption!=""): 
         SLHC_Flags.LayoutOption=layoutOption
      from InDetRecExample.InDetJobProperties import InDetFlags
      if (layoutDescr!=""):
         include('InDetSLHC_Example/preInclude.SLHC_Setup_'+layoutDescr+'.py')
      else:
         include("InDetSLHC_Example/preInclude.SLHC_Setup.py")
      include('InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py')
      include('InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Reco.py')
      break

if(not foundGeoTag):
   from InDetRecExample.InDetJobProperties import InDetFlags
   include("InDetSLHC_Example/preInclude.SLHC_Setup.py")
   include('InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py')
   include('InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Reco.py')
