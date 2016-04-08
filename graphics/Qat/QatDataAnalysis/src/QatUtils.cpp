#include "QatDataAnalysis/QatUtils.h"
#include "QatDataAnalysis/HistogramManager.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatDataAnalysis/Hist2D.h"
#include "QatDataAnalysis/Table.h"
void deepCopy(const HistogramManager *input, HistogramManager *output) {
  for (HistogramManager::DConstIterator d=input->beginDir();d!=input->endDir();d++) {
    HistogramManager *newDir=output->newDir((*d)->name());
    deepCopy(*d,newDir);
  }

  for (HistogramManager::H1ConstIterator d=input->beginH1();d!=input->endH1();d++) {
    output->newHist1D(**d);
  }

  for (HistogramManager::H2ConstIterator d=input->beginH2();d!=input->endH2();d++) {
    output->newHist2D(**d);
  }

  for (HistogramManager::TConstIterator d=input->beginTable();d!=input->endTable();d++) {
    output->newTable(**d);
  }

}
