import os, re, sys

#=#=#
DataSets = [
  'mc14_8TeV.208010.Pythia8B_AU2_CTEQ6L1_pp_Jpsimu6p5mu6p5.recon.AOD.e1966_s1933_s1911_r6242/',
  'mc14_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.merge.AOD.e1852_s1896_s1912_r6259_r6260/',
  'data12_8TeV.periodB.physics_Muons.PhysCont.AOD.repro17_v02/',
]

#=#=#
PrintOnly = False
IsOfficial = True
Version = 'Version1'
FilesPerJob = 20
TotalFiles = -1

#=#=#
if IsOfficial:
  UserName = 'group.perf-muons'
else:
  UserName = 'user.gartoni'

#=#=#
RegEx_MC = re.compile( '(\w*)\.(\d*)\.(\w*)\.(\w*)\.(\w*)\.(\w*)/' ) 
RegEx_Data = re.compile( '(\w*)\.(\w*)\.(\w*)\.(\w*)\.(\w*)\.(\w*)/' ) 
JobOptions = os.path.expandvars( '$TestArea/InstallArea/jobOptions/MuonPtCalibNtupleMaker/MuonPtCalibNtupleMaker_JobOptions.py' )

for DataSet in DataSets:
  Match_MC = RegEx_MC.match( DataSet ) 
  Match_Data = RegEx_Data.match( DataSet ) 
  OutputDataSet = ''
  if Match_MC:
    OutputDataSet = UserName + '.' + Match_MC.group( 1 ) + '.' + Match_MC.group( 2 ) + '.' + Match_MC.group( 6 ) + '.PtCalibNtuple.' + Version
  elif Match_Data:
    OutputDataSet = UserName + '.' + Match_Data.group( 1 ) + '.' + Match_Data.group( 2 ) + '.' + Match_Data.group( 3 ) + '.' + Match_Data.group( 6 ) + '.PtCalibNtuple.' + Version
  Command  = 'pathena'
  Command += ' %s' % JobOptions
  Command += ' --inDS=%s' % DataSet
  Command += ' --outDS=%s' % OutputDataSet
  Command += ' --nFilesPerJob=%d' % FilesPerJob 
  if TotalFiles > 0:
    Command += ' --nFiles=%d' % TotalFiles
  if IsOfficial:
    Command += ' --official --voms=atlas:/atlas/perf-muons/Role=production' 
  
  if PrintOnly:
    print Command
  else:
    os.system( Command )
