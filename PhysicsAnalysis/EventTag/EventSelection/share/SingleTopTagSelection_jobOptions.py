include.block ( "EventSelection/SingleTopTagSelection_jobOptions.py" )

include ( "EventSelection/SingleElectronTagSelection_jobOptions.py" )
include ( "EventSelection/SingleMuonTagSelection_jobOptions.py" )
include ( "EventSelection/SingleJetTagSelection_jobOptions.py" )

include ( "EventSelection/MissingEtTagSelection_jobOptions.py" )

#define the 1-electron/3-jet channel
ElectronJet = SingleElectron + "&&" + SingleJet + "&&" + MissingET + "&&" + "NBJet>0"

#define the 1-muon/3-jet channel
MuonJet = SingleMuon + "&&" + SingleJet + "&&" + MissingET + "&&" + "NBJet>0"

SingleTop = "("+ElectronJet+")" + "||" + "("+MuonJet+")"

