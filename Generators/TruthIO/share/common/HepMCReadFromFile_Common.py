from TruthIO.TruthIOConf import HepMCReadFromFile
genSeq += HepMCReadFromFile()
genSeq.HepMCReadFromFile.InputFile="events.hepmc"
evgenConfig.generators += ["HepMCAscii"]


