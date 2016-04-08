include.block ( "EventSelection/SemiLeptonicTtbarTagSelection_jobOptions.py" )

include ( "EventSelection/SingleElectronTagSelection_jobOptions.py" )
include ( "EventSelection/SingleMuonTagSelection_jobOptions.py" )
include ( "EventSelection/ThreeJetTagSelection_jobOptions.py" )
include ( "EventSelection/MissingEtTagSelection_jobOptions.py" )

#define the 1-electron/3-jet channel
ElectronChannel = SingleElectron + "&&" + ThreeJet + "&&" + MissingET + "&&" + "NBJet>0"

#define the 1-muon/3-jet channel
MuonChannel = SingleMuon + "&&" + ThreeJet + "&&" + MissingET + "&&" + "NBJet>0"

#semi-leptonic ttbar channel
SemiLeptonicTtbar = "("+ElectronChannel+")" + "||" + "("+MuonChannel+")"

