#
# jobOptions for the instance simplenoisetool of SimpleNoiseToolFromTextFile
# 
ToolSvc = Service( "ToolSvc" )
ToolSvc.simplenoisetool.CellNoiseFileName = "/afs/cern.ch/user/h/hectbmon/public/tb/aug02/noise/H6_2002NoiseMuons.dat"
ToolSvc.simplenoisetool.CellNoiseUnits = MeV
ToolSvc.simplenoisetool.CellNoiseDefault = 100.*MeV
ToolSvc.simplenoisetool.CellNoiseDefaultWarning = TRUE
ToolSvc.simplenoisetool.OutputLevel = INFO
