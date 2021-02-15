/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_ROSTABLECONSTRUCTOR_H
#define TRIGCOSTANALYSIS_ROSTABLECONSTRUCTOR_H 1

#include "../TableConstructorBase.h"

/**
 * @class ROSTableConstructor
 * @brief Construct a CSV table to process the output of a MonitorROS
 */
class ROSTableConstructor : public TableConstructorBase {
  public:
    /**
     * @brief Construct ROSTableConstructor
     * @param[in] name ROSTableConstructor's name
     */
    ROSTableConstructor(const std::string& name);

    /**
     * @brief Default destructor
     */
    virtual ~ROSTableConstructor() = default;

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

#endif // TRIGCOSTANALYSIS_ROSTABLECONSTRUCTOR_H