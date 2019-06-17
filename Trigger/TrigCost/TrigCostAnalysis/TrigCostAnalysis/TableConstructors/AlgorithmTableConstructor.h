/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_ALGORITHMTABLECONSTRUCTOR_H
#define TRIGCOSTANALYSIS_ALGORITHMTABLECONSTRUCTOR_H 1

#include "../TableConstructorBase.h"

/**
 * @class TableConstructorBase
 * @brief Construct a CSV table to process the output of a MonitorAlgorithm
 */
class AlgorithmTableConstructor : public TableConstructorBase {
  public:
    /**
     * @brief Construct AlgorithmTableConstructor.
     * @param[in] name AlgorithmTableConstructor's name
     */
    AlgorithmTableConstructor(const std::string& name);

    /**
     * @brief Default destructor
     */
    virtual ~AlgorithmTableConstructor() = default;

  protected:

    /**
     * @brief Get TableEntry for a single Algorithm
     * @param[in] name Name of algorithm TDirectory to generate a TableEntry.
     * @return TableEntry populated with a row of data for the given TDirectory.
     */  
    virtual TableEntry getTableEntry(const std::string name) override;

    /**
     * @brief Post-processing for the algorithm table.
     * @param[in,out] tableEntry Mutable vector of TableEntries, each corresponding to a row in the table.
     * @param[in] walltime Normalisation parameter.
     */  
    virtual void tablePostProcessing(std::vector<TableEntry>& tableEntries, const float walltime) override;
  
};

#endif // TRIGCOSTANALYSIS_ALGORITHMTABLECONSTRUCTOR_H
