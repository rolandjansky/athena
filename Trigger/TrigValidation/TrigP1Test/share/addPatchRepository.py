from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import os
import pm.project

def modify(part):

   patchproj = ['AtlasP1HLT','AtlasCAFHLT']  # consider these patch projects
   apps = part.get('PTTemplateApplication') + part.get('L2PUTemplateApplication')
   
   # Add the patch project areas if needed
   for p in os.environ["CMTPATH"].split(":"):
      for prj in patchproj:
         if p.find(prj)==-1: continue
         rep = '%sPatchRepository' % prj
         print 'Adding',rep
         os.system('make_hlt_patch.py --name %s --path %s' % (rep,p))
         patch = pm.project.Project('%s.data.xml' % rep).getObject('SW_Repository',rep)

         for a in apps: a.Uses.append(patch)

   return part
