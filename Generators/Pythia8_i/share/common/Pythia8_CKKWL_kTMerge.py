
try:
    PYTHIA8_nJetMax
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_nJetMax\" is not defined, this is needed to configure Pythia8 CKKW-L kT merging settings. Please define it in your jobOptions")
else:
    print "PYTHIA8_nJetMax = %i"%PYTHIA8_nJetMax
    
try:
    PYTHIA8_Process
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_Process\" is not defined, this is needed to configure Pythia8 CKKW-L kT merging settings. Please define it in your jobOptions")
else:
    print "PYTHIA8_Process = %s"%PYTHIA8_Process  
    
try:
    PYTHIA8_TMS
except RuntimeError:
    raise RuntimeError("Variable \"TMS\" is not defined, this is needed to configure Pythia8 CKKW-L kT merging settings. Please define it in your jobOptions")
else:
    print "PYTHIA8_TMS = %f"%PYTHIA8_TMS

try:
    PYTHIA8_Dparameter
except RuntimeError:
    raise RuntimeError("Variable \"Dparameter\" is not defined, this is needed to configure Pythia8 CKKW-L kT merging settings. Please define it in your jobOptions")
else:
    print "PYTHIA8_Dparameter = %f"%PYTHIA8_Dparameter

try:
    PYTHIA8_nQuarksMerge
except RuntimeError:
    raise RuntimeError("Variable \"PYTHIA8_nQuarksMerge\" is not defined, this is needed to configure Pythia8 CKKW-L kT merging settings. Please define it in your jobOptions")
else:
    print "PYTHIA8_nQuarksMerge = %i"%PYTHIA8_nQuarksMerge
    
genSeq.Pythia8.Commands += ["Merging:doKTMerging = on",
                            "Merging:ktType = 1",
                            "Merging:nJetMax = %i"%PYTHIA8_nJetMax,
                            "Merging:Process = %s"%PYTHIA8_Process,
                            "Merging:TMS = %f"%PYTHIA8_TMS,
                            "Merging:Dparameter = %f"%PYTHIA8_Dparameter,
                            "Merging:nQuarksMerge = %i"%PYTHIA8_nQuarksMerge]

### Cross section fix only for versions prior to 8.219
cmak_sys = os.getenv('CMAKE_PREFIX_PATH')
if not cmak_sys:
  print "we are running in cmt system" 
  verstr = os.popen("cmt show versions External/Pythia8").read()
  start=1
  versions=[]
  while start > 0:
    start = verstr.find("Pythia8-", start + 1)
    if start < 0:
      break
    versions.append(verstr[start+8: start+16])

  versions.sort()
  majorStr = versions[-1][3:5]
else:
  print "we are running in cmake system"
  verstr = os.popen("acmake.py show_versions Pythia8_i").read()
  majorStr=0
  ver1=verstr[-6:-4]
  ver2=verstr[-9:-7]
  if ver1 == 12 and ver2==8:
     majorStr=19

if int(majorStr) < 19:
  genSeq.Pythia8.CKKWLAcceptance = True


