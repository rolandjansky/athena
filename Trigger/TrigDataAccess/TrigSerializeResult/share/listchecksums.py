import ROOT
import cppyy
cppyy.Cintex.Enable()
for i in ROOT.TFile.Open('bs-streamerinfos.root').GetStreamerInfoList():
    if i.GetName() != 'listOfRules':
        print i.GetName(), "%x" % i.GetCheckSum()
