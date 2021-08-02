/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::Container
   @brief storage of the time histories of all the cells
*/

#ifndef LArSamples_Interface_H
#define LArSamples_Interface_H

#include "LArSamplesMon/Accessor.h"
#include "LArCafJobs/CaloId.h"
#include "LArCafJobs/CellInfo.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/HistoryIterator.h"
#include "LArSamplesMon/OccupancyMonitor.h"
#include "LArSamplesMon/DigitMonitor.h"
#include "LArSamplesMon/FitMonitor.h"
#include "LArSamplesMon/Chi2Calc.h"
#include "TString.h"
#include <vector>

class IdentifierHash;
class TH1I;
class TFile;

namespace LArSamples {
  
  class Data;
  class FilterParams;
  class DataTweaker;
  class AbsShapeErrorGetter;

  class Interface : public Accessor {
  
    public:
   
      /** @brief Constructor  */
      Interface(const Accessor& accessor)
        : m_accessor(&accessor), m_shapeErrorGetter(0), m_ownShapeErrorGetter(0), m_neighborCache(nChannels()) { }
      
      static Interface* open(const TString& fileName);
      static Interface* open(const std::vector<TString>& fileNames);
      static Interface* openList(const TString& fileList);
      static Interface* openWild(const TString& wcName);

      virtual ~Interface();

      unsigned int nEvents() const { return accessor().nEvents(); }
      unsigned int nRuns() const { return accessor().nRuns(); }

      const EventData* eventData(unsigned int i) const { return accessor().eventData(i); }
      const RunData* runData(unsigned int i) const { return accessor().runData(i); }
      
      unsigned int historySize(unsigned int i) const { return accessor().historySize(i); }

      HistoryIterator begin(unsigned int pos = 0, double eMin = -1, double adcMaxMin = -1) const;
      unsigned int end() const { return nChannels(); }
      
      unsigned int size() const;
      unsigned int nFilledChannels() const;
      void printFilledRanges(unsigned int skip = 0) const;
    
      bool highEData(double eCut, TArrayI& hashes, TArrayI& indices) const;
    
      HistoryIterator findEtaPhi(CaloId calo, short layer, short iEta, short iPhi, short region = 0) const;
      HistoryIterator findFebChannel(CaloId calo, short feb, short channel) const;
      HistoryIterator findFTSlotChannel(CaloId calo, short ft, short slot, short channel) const;

      bool neighbors(const CellInfo& cell, double dRCut, std::vector<unsigned int>& hashes) const;
      bool firstNeighbors(unsigned int hash, std::vector<unsigned int>& hashes, short layer = -2) const;
      bool data(const std::vector<unsigned int>& hashes,const EventData& event, std::vector<const Data*>& data) const;
    
      Interface* merge(const Interface& other, const TString& fileName) const;
      Interface* merge(const Interface& other, const TString& fileName, const TString& LBFile) const;
#ifndef __GCCXML__
      static Interface* merge(const std::vector<const Interface*>& interfaces, const TString& fileName);
      static Interface* merge(const std::vector<const Interface*>& interfaces, const TString& fileName, const TString& LBFile);
#endif
      static Interface* merge(const TString& listFileName, const TString& fileName);
      static Interface* merge(const TString& listFileName, const TString& fileName, const TString& LBFile);
      static bool filterAndMerge(const TString& listFileName, const TString& outFile, const TString& filters, const TString& tweaks = "");

      Interface* filter(const TString& sel, const TString& fileName, const TString& tweaks = "") const;
      Interface* filter(const FilterParams& filterParams, const DataTweaker& tweaker, const TString& fileName) const;

      Interface* makeTemplate(const TString& fileName) const;

      Interface* refit(TString newFileName, Chi2Params pars = DefaultChi2) const;
      
      const Accessor& accessor() const { return *m_accessor; }
      bool isValid() const;

      TH1D* Draw(const TString& var, int nBins, double xMin, double xMax, const TString& sel = "", const TString& opt = "") const;
      TH2D* Draw(const TString& varList, int nBinsX, double xMin, double xMax, int nBinsY, double yMin, double yMax, 
		 const TString& sel = "", const TString& opt = "") const;
      TH2D* DrawPartition(PartitionId partition, const TString& var, const TString& sel = "", const TString& opt = "", 
		 CombinationType comb = TotalValue) const;      
      TH2D* DrawEtaPhi(CaloId calo, short layer, const TString& var, const TString& sel = "", const TString& opt = "", 
		 CombinationType comb = TotalValue) const;
      bool Scan(const TString& vars, const TString& sel = "", unsigned int verbosity = 1) const;
      bool Scan(const TString& vars, CombinationType comb, const TString& sel = "", const TString& ranges = "", unsigned int verbosity = 1) const;

      bool Show(unsigned int hash, unsigned int verbosity = 1) const;
      bool Show(const TString& sel = "", unsigned int verbosity = 1) const;
      bool ShowEvents(const TString& sel = "", unsigned int verbosity = 1) const;
      bool ShowRuns(unsigned int verbosity = 1) const;

      bool ShowStats(const TString& varList, const TString& sel = "", bool withErrors = false) const;

      OccupancyMonitor occupancyMonitor() const { return OccupancyMonitor(*this); }
      DigitMonitor digitMonitor() const { return DigitMonitor(*this); }
      FitMonitor fitMonitor() const { return FitMonitor(*this); }

      bool dumpEventTuple(const TString& variables, const TString& fileName) const;

      void setShapeErrorGetter(const AbsShapeErrorGetter* err);
      void setShapeError(double k);
      void setShapeError(const TString& fileName);
      const AbsShapeErrorGetter* shapeErrorGetter() const { return m_shapeErrorGetter; }

      bool writeToFile(const TString& fileName) const { return accessor().writeToFile(fileName); }
      
      static TString addSuffix(const TString& fileName, const TString& suffix);
      
      const History*  cellHistory(unsigned int i) const;      
      const History*  getCellHistory(unsigned int i) const;      
      const CellInfo* getCellInfo(unsigned int i) const;
      
    private:
      
      const Accessor* m_accessor;
      const AbsShapeErrorGetter* m_shapeErrorGetter;
      bool m_ownShapeErrorGetter;
    
      mutable std::vector<std::vector<unsigned int>*> m_neighborCache;
      mutable std::vector<unsigned int> m_neighborHistoryPos;
      mutable std::vector<const History*> m_neighborHistories;
  };
}
  
#endif
