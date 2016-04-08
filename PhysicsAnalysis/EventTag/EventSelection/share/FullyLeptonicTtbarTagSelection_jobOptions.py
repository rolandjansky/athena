include.block ( "EventSelection/FullyLeptonicTtbarTagSelection_jobOptions.py" )

include ( "EventSelection/DiElectronTagSelection_jobOptions.py" )
include ( "EventSelection/DiMuonTagSelection_jobOptions.py" )
include ( "EventSelection/DiJetTagSelection_jobOptions.py" )
include ( "EventSelection/MissingEtTagSelection_jobOptions.py" )

#define the 2-electron/2-jet channel
DiElectronChannel = DiElectron + "&&" + DiJet + "&&" + MissingET + "&&" + "NBJet>0"

#define the 2-muon/2-jet channel
DiMuonChannel = DiMuon + "&&" + DiJet + "&&" + MissingET + "&&" + "NBJet>0"

#fully-leptonic ttbar channel
FullyLeptonicTtbar = "("+DiElectronChannel+")" + "||" + "("+DiMuonChannel+")"

