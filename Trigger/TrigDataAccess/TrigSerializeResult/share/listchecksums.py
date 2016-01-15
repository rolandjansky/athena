import ROOT
import cppyy
try:
    # ROOT 5 needs that, ROOT 6 does not have Cintex
    cppyy.Cintex.Enable()
except :
    pass

for i in ROOT.TFile.Open('bs-streamerinfos.root').GetStreamerInfoList():
    if i.GetName() != 'listOfRules':
        print i.GetName(), "%x" % i.GetCheckSum()
