/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_THREADOCCUPANCYTABLECONSTRUCTOR_H
#define TRIGCOSTANALYSIS_THREADOCCUPANCYTABLECONSTRUCTOR_H 1

#include "../TableConstructorBase.h"

/**
 * @class ThreadOccupancyTableConstructor
 * @brief Construct a CSV table to process the output of a MonitorThreadOccupancy
 */
class ThreadOccupancyTableConstructor : public TableConstructorBase {
  public:
    /**
     * @brief Construct ThreadOccupancyTableConstructor.
     * @param[in] name ThreadOccupancyTableConstructor's name
     */
    ThreadOccupancyTableConstructor(const std::string& name);

    /**
     * @brief Default destructor
     */
    virtual ~ThreadOccupancyTableConstructor() = default;

  protected:

    /**
     * @brief Get TableEntry for a Global counter equating to a LB or a range of LB
     * @param[in] name Name of LB TDirectory to generate a TableEntry.
     * @return TableEntry populated with a row of data for the given TDirectory.
     */  
    virtual TableEntry getTableEntry(const std::string name) override;

    /**
     * @brief Post-processing. Note - not used for this table.
     * @param[in,out] tableEntry Mutable vector of TableEntries, each corresponding to a row in the table.
     * @param[in] walltime Normalisation parameter.
     */  
    virtual void tablePostProcessing(std::vector<TableEntry>& tableEntries, const float walltime) override;
  
};

#endif // TRIGCOSTANALYSIS_THREADOCCUPANCYTABLECONSTRUCTOR_H
