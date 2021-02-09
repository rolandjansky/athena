# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import os, stat

class batchJobBase:
  """A class containing all information necessary to run given bash commands in an arbitrary batch system."""
  
  def __init__(self, name, hours=0, nCores=1, memMB=0, basedir=""):
    self.name = name
    self.cmds = []
    self.hours = hours
    self.nCores = nCores
    self.memMB = memMB
    if self.memMB == 1:
      self.memMB = 1499
    if self.memMB == 2:
      self.memMB = 2499
    self.env = {}
    self.basedir = basedir
    self.id = None
    self.dependsOnOk = []
    self.dependsOnAny = []

  def write(self, useSingularity=True, extraDirs=[]):
    executable =  "#!/bin/sh -\n"

    if useSingularity:
      executable += 'if [ "$1" != "--really" ]; then \n'
      executable += '  exec singularity exec -e --no-home'
      for dir in ["/cvmfs", "/var", self.basedir, "$(pwd | cut -d '/' -f 1-2)"] + extraDirs:
        executable += ' -B '+dir
      executable += ' /cvmfs/atlas.cern.ch/repo/containers/fs/singularity/x86_64-centos7 /bin/bash -- "$0" --really "$@";\n'
      executable += 'fi\n'
      executable += "shift;\n\n"
      executable += "export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase\n"
      executable += "source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh\n"

    executable += "ulimit -f 1000000;\n"
    executable += "cd "+self.basedir+"\n\n"
    executable += "echo 'ncores="+str(self.nCores)+" nhours="+str(self.hours)+" "+self.basedir+"/"+self.name+".sh';\n"
    for cmd in self.cmds:
      executable += cmd+"\n"
    executable += "exit 0\n"


    filename = self.basedir+"/"+self.name+".sh"
    with open(filename, 'w') as f:
        f.write(executable)

    #make shell-files executable
    st = os.stat(filename)
    os.chmod(filename, st.st_mode | stat.S_IEXEC)


