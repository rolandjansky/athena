/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::DigitMonitor
   @brief storage of the time histories of all the cells
 */

#ifndef LArSamples_MonitorBase_H
#define LArSamples_MonitorBase_H

#include "TH1D.h"
#include "TH2D.h"
#include "LArSamplesMon/AbsLArCells.h"
#include "LArSamplesMon/Data.h"
#include "LArCafJobs/CaloId.h"
#include "LArSamplesMon/FilterParams.h"


namespace LArSamples {
  
  class Interface;
  class History;
  class ShapeFitter;
  
  enum CombinationType { AverageValue = 0, MaxValue = 1, MinValue = 2, TotalValue = 3, RMSValue = 4 };

  class MonitorBase : public AbsLArCells
  {
  
    public:

      /** @brief Constructor  */
      MonitorBase(const Interface& interface) : m_interface(&interface) { } 

#ifndef __GCCXML__
      TH1D* dist(const DataFuncSet& func, const DataFuncArgs& args, TString name, int nBins, double xMin, double xMax, 
                 TString title = "", TString xTitle = "", TString yTitle = "", 
                 const FilterParams& f = FilterParams()) const;
                 
      TH2D* dist(const DataFuncSet& funcX, const DataFuncArgs& argsX, const DataFuncSet& funcY, const DataFuncArgs& argsY, TString name, 
		 int nBinsX, double xMin, double xMax, 
		 int nBinsY, double yMin, double yMax, 
                 TString title = "", TString xTitle = "", TString yTitle = "", 
                 const FilterParams& f = FilterParams()) const;
                 
      TH2D* partitionMap(const DataFuncSet& func, const DataFuncArgs& args, TString name, PartitionId partition, 
                TString title = "", CombinationType comb = AverageValue, 
                         const FilterParams& f = FilterParams()) const;
      TH2D* etaPhiMap(const DataFuncSet& func, const DataFuncArgs& args, TString name, CaloId calo, 
                      short layer, TString title = "", CombinationType comb = AverageValue, 
                      const FilterParams& f = FilterParams()) const;

      bool prepareDumpParams(const TString& vars, int verbosity,
			     std::vector<TString>& variables, std::vector<DataFuncSet>& funcs, std::vector<DataFuncArgs>& args,
			     std::vector<TString>& formats, TString& locFormat, TString& locHeader, TString& varHeader) const;
			     
      bool dump(const TString& vars, const FilterParams& f = FilterParams(), unsigned int verbosity = 1) const;
      bool dump(const TString& vars, CombinationType comb = AverageValue, const FilterParams& f = FilterParams(), const TString& ranges = "", unsigned int verbosity = 1) const;

      static double history_value(const History& history, const DataFuncSet& func, const DataFuncArgs& args,
                                  CombinationType comb,  const FilterParams& f, unsigned int& nValues);
		      
      static DataFuncSet func(TString var);
      static bool parseVariables(TString varStr, std::vector<TString>& vars, std::vector<DataFuncSet>& funcs, 
                                 std::vector<DataFuncArgs>& args);
      
      bool statParams(const std::vector<DataFuncSet>& funcs, const std::vector<DataFuncArgs>& args,
                      TVectorD& mean, TVectorD& meanErr, TMatrixD& covMatrix, TMatrixD& covMatrixErr, 
                      const FilterParams& f = FilterParams()) const;
#endif

      const Interface& interface() const { return *m_interface; }
      unsigned int nChannels() const;

      static TString str(CombinationType comb);

      static const int printPeriodicity = 10000;

      const History* getCellHistory(unsigned int i) const;      
      const CellInfo* getCellInfo(unsigned int i) const;

    private:
      
      const Interface* m_interface;
  };
}
  
#endif
