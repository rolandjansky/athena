/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/D3PDConverter.h"

#include "LArCafJobs/DataStore.h"
#include "LArCafJobs/RunData.h"
#include "LArCafJobs/EventData.h"
#include "LArCafJobs/DataContainer.h"
#include "LArCafJobs/HistoryContainer.h"
#include "LArCafJobs/CellInfo.h"

#include "LArSamplesMon/LArIdTranslatorHelper.h"

#include "Identifier/IdentifierHash.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;

bool D3PDConverter::makeSamplesTuple(const TString& outputFileName)
{
  DataStore* samples = new DataStore();
  RunData* runData = nullptr;
  
  cout << "Processing " << fChain->GetEntries() << " entries" << endl;
  for (long long i = 0; i < fChain->GetEntries(); i++) {
    if (i % 500 == 0) cout << "Processing Entry " << i << endl;
    fChain->GetEntry(i);
    if (!runData || runData->run() != (int)RunNumber) {
      // new run
      runData = new RunData(RunNumber);
      samples->addRun(runData);
    }
    
    EventData* eventData = new EventData(EventNumber, 0, lbn, bcid);
    eventData->setRunData(runData);
    unsigned int eventIndex = samples->addEvent(eventData);

    //cout << "Entry " << i << " nCells = " << cc_sel_n << endl;
    
    for (int j = 0; j < cc_sel_n; j++) {
      // skip Tile cells
      if ((*cc_sel_DetCells)[j] & 8) continue;
      unsigned int hash = m_id2hash[(*cc_sel_IdCells)[j]];
      if (hash == 0) {
        cout << "ERROR: skipping cell with offline ID = " << (*cc_sel_IdCells)[j] << endl;
        continue;
      }

      // find digit
      int digitIndex = -1;
      for (int k = 0; k < lardigit_n; k++) {
        if ((*lardigit_offlineId)[k] == (*cc_sel_IdCells)[j]) {
          digitIndex = k;
          break;
        }
      }
      //if (digitIndex >= 0) cout << "digitIndex = " << digitIndex << endl;
      hash -= 1;
      HistoryContainer* histCont = samples->hist_cont(hash);
      CellInfo* info = nullptr;
      if (!histCont) {
        const CellInfo* templateInfo = m_template->cellInfo(hash);
        info = new CellInfo(templateInfo->calo(), templateInfo->layer(),
                            templateInfo->iEta(), templateInfo->iPhi(),
                            templateInfo->feedThrough(), templateInfo->slot(), templateInfo->channel(),
                            nullptr,nullptr,nullptr, templateInfo->position(), templateInfo->onlid());
        histCont = samples->makeNewHistory(IdentifierHash(hash), info);
      }
      else
        info = histCont->cell_info();

      CaloGain::CaloGain gain = CaloGain::UNKNOWNGAIN;
      std::vector<short> samples;
      if (digitIndex >= 0) {
        gain = (CaloGain::CaloGain)(*lardigit_gain)[digitIndex];
        std::vector<int> intSamples = (*lardigit_Samples)[digitIndex];
        for (int s : intSamples) {
          samples.push_back((short)s);
        }
      }
      DataContainer* data =
        new DataContainer(gain, samples, (*cc_sel_E)[j], (*cc_sel_TimeCells)[j],
                          (*cc_sel_QCells)[j], eventIndex,
                            std::vector<float>(), (*cc_sel_Sigma)[j],
                            -1, -1, (*cc_sel_BadCells)[j]);
      //cout << "Adding data to " << hash << " " << histCont->nDataContainers() << endl;
      histCont->add(data);
    }
  }
  samples->writeTrees(outputFileName);
  return true;
}


bool D3PDConverter::initMapping(const TString& templateFile, const TString& translatorFile)
{
  LArIdTranslatorHelper* translator = new LArIdTranslatorHelper(translatorFile);
  m_template = Interface::open(templateFile);

  cout << "Making online->hash map" << endl;
  std::map<unsigned long long, unsigned int> on2hash;
  for (unsigned int i = 0; i < m_template->nChannels(); i++) {
    const CellInfo* info = m_template->cellInfo(i);
    if (!info) continue;
    on2hash[info->onlid()] = i;
  }

  cout << "Making offlineID->hash map" << endl;
  for (unsigned int i = 0; i < translator->Tree()->GetEntries(); i++) {
    translator->Tree()->GetEntry(i);
    m_id2hash[translator->offlid] = on2hash[translator->onlid] + 1;
  }
  // delete translator; // crashes
  return true;
}


 
