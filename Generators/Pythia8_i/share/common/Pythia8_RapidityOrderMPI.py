## this fragment makes sure that the MPI rapidity order is set consistently for Pythia versions 8.219 and later
## Since it depends on the tune settings it must be included *after* the main tune fragment in the JO

addRapidityOrderMPI = True
rapidityOrderMPICommand = []

for cmd in genSeq.Pythia8.Commands:
  
  if "SpaceShower:rapidityOrderMPI = " in cmd:
    addRapidityOrderMPI = False
  
  if "SpaceShower:rapidityOrder" in cmd and "SpaceShower:rapidityOrderMPI" not in cmd and addRapidityOrderMPI:
    
    val = cmd.split("=")[-1]

#    rel = os.popen("echo $AtlasVersion").read()
#    if rel[:2].isdigit() and int(rel[:2])<=20 :
    cmak_sys = os.getenv('CMAKE_PREFIX_PATH')
    if not cmak_sys:
      verstr = os.popen("cmt show versions External/Pythia8").read()

      start=1
      versions=[]
      while start > 0:
        start = verstr.find("Pythia8-", start + 1)
        if start < 0:
          break
        versions.append(verstr[start+8: start+16])

      versions.sort()
      series = versions[-1][0:2]
      majorStr = versions[-1][3:5]

    else:
      print " running in cmake system"
      verstr = os.popen("acmake.py show_versions Pythia8_i").read()
      majorStr=0
      ver1=verstr[-6:-4]
      ver2=verstr[-9:-7]
      if ver1 == 12 and ver2==8:
        majorStr=19
        series="02"

    if int(majorStr) >= 19 and series != "01" :
      rapidityOrderMPICommand = ["SpaceShower:rapidityOrderMPI = " + val]

if addRapidityOrderMPI and len(rapidityOrderMPICommand) != 0:
  genSeq.Pythia8.Commands += rapidityOrderMPICommand
