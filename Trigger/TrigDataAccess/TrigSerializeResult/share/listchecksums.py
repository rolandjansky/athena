import ROOT
import cppyy
import sys

try:
    # ROOT 5 needs that, ROOT 6 does not have Cintex
    cppyy.Cintex.Enable()
except :
    pass

if len(sys.argv) == 1:
  fname = 'bs-streamerinfos.root'
else:
  fname = sys.argv[1]
print "Opening " + fname

for i in ROOT.TFile.Open(fname).GetStreamerInfoList():
    if i.GetName() != 'listOfRules':
        print i.GetName(), "%x" % i.GetCheckSum()
