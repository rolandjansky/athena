#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TruthIO/DumpMC.h"
#include "TruthIO/HepMCReadFromFile.h"
#include "TruthIO/WriteHepMC.h"
#include "TruthIO/PrintMC.h"
#include "TruthIO/PrintHijingPars.h"
#include "TruthIO/ReadHepEvtFromAscii.h"

DECLARE_ALGORITHM_FACTORY(DumpMC)
DECLARE_ALGORITHM_FACTORY(HepMCReadFromFile)
DECLARE_ALGORITHM_FACTORY(PrintMC)
DECLARE_ALGORITHM_FACTORY(WriteHepMC)
DECLARE_ALGORITHM_FACTORY(PrintHijingPars)
DECLARE_ALGORITHM_FACTORY(ReadHepEvtFromAscii)

DECLARE_FACTORY_ENTRIES(GeneratorModules) {
  DECLARE_ALGORITHM(DumpMC)
  DECLARE_ALGORITHM(HepMCReadFromFile)
  DECLARE_ALGORITHM(PrintMC)
  DECLARE_ALGORITHM(WriteHepMC)
  DECLARE_ALGORITHM(PrintHijingPars)
  DECLARE_ALGORITHM(ReadHepEvtFromAscii)
}
