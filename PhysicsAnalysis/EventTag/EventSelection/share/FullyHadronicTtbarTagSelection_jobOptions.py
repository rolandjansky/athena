include.block ( "EventSelection/FullyHadronicTtbarTagSelection_jobOptions.py" )

include ( "EventSelection/FourJetTagSelection_jobOptions.py" )
include ( "EventSelection/MissingEtTagSelection_jobOptions.py" )

#define the 4-jet channel
FourJetChannel = FourJet + "&&" + MissingET + "&&" + "NBJet>0"

#fully-hadronic ttbar channel
FullyHadronicTtbar = "("+FourJetChannel+")"

