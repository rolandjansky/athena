
# include it after filling Pythia8.Commands to increase its utility
from Pythia8_i.Pythia8Util import *
genSeq += Pythia8Util("newP8util")
genSeq.newP8util.Pythia8Commands=genSeq.Pythia8.Commands
