def lsroot(dir):
   """Return list of all keys in 'dir' (recursively)"""

   import ROOT
   
   def dols(dir, keys):
      """Do the recursive traversal"""
      dirList = dir.GetListOfKeys()
      for k in dirList:
         kname = k.GetName()
         if k.GetClassName()=="TDirectoryFile" or k.GetClassName()=="TDirectory":
            dir.cd(kname)
            dols(ROOT.gDirectory, keys)
         else:
            keys += [dir.GetPath()+"/"+kname]
      
      dir.cd("..")
      return

   keys = []
   basedir = dir.GetPath().rstrip("/") + "/"
   dols(dir,keys)

   # Return sorted list with base directory removed
   return sorted([k.replace(basedir,"") for k in keys])

