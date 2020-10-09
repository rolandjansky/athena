/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_TABLECONSTRUCTORBASE_H
#define TRIGCOSTANALYSIS_TABLECONSTRUCTORBASE_H 1

#include <string>
#include <unordered_map>
#include <vector>

#include "TableEntry.h"

#include "AsgMessaging/MsgStream.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TString.h"

class TFile; //!< Forward declaration
class TH1; //!< Forward declaration


/**
 * @class TableConstructorBase
 * @brief Base class to construct a CSV table for a TDirectory which contains TDirectories.
 * Each sub-TDirectory corresponds to a row in the table, histograms in these sub directories
 * are used to populate each row with data.
 */
class TableConstructorBase {
  public:
    /**
     * @brief Construct Table Constructor with given name.
     * @param[in] name Table Constructor's name
     */
    TableConstructorBase(const std::string& name);

    /**
     * @brief Default destructor
     */
    virtual ~TableConstructorBase() = default;

    /**
     * @brief Return full table for the current global TDirectory
     * @param[in] file TFile being exported to CSV
     * @param[in] rootDir Current directory. Used as root for this constructor.
     * @param[in] walltime Normalisation parameter
     */
    std::string getTable(TFile* file, const TString& rootDir, const float walltime);

  protected:

    /**
     * @brief Register a histogram which is expected to be found and is needed to compute one or more entries in the CSV
     * @param[in] name Expected histogram name.
     */
    void addExpectedHistogram(const TString& name);
    
    /**
     * @brief Check that all expected histograms are present. Fetch pointers to all of them.
     * @param[in] name Name of TDirectory in which to search for histograms.
     */    
    void getHistograms(const std::string& name);

    /**
     * @brief Get base TableEntry which is configured with the correct set of Columns for this Table.
     * @return Reference to this Table's base entry. Only has headers, no row payload data.
     */   
    TableEntry& getBaseEntry();

    /**
     * @brief Get "total" value by integrating over a histogram, weighting every entry by its x-axis mean.
     * @param[in] name Name of histogram
     * @param[in] isLong If histogram is log x-axis, modifies how x-axis mean is computed for each bin.
     * @return Total value of the histogram.
     */   
    float histGetXWeightedIntegral(const std::string& name, bool isLog = true);

    /**
     * @brief Pure virtual method to return a TableEntry for a given TDirectory. Must be overridden.
     * @param[in] name Name of TDirectory to generate a TableEntry.
     * @return TableEntry populated with a row of data for the given TDirectory.
     */  
    virtual TableEntry getTableEntry(const std::string name) = 0;

    /**
     * @brief Pure virtual method to be overridden by derived class. Perform post-construction modifications to whole table.
     * @param[in,out] tableEntry Mutable vector of TableEntries, each corresponding to a row in the table.
     * @param[in] walltime Normalisation parameter
     */  
    virtual void tablePostProcessing(std::vector<TableEntry>& tableEntries, const float walltime) = 0;

    /**
     * @brief Get histogram by name. Histograms are fetched by getHistograms()
     * @return Cached non-owning pointer to ROOT histogram.
     */  
    TH1* hist(const std::string& name) const;

    /**
     * @brief Logging
     * @return Message stream reference.
     */  
    MsgStream& msg() const;

  private:
    mutable MsgStream m_msgStream; //!< Logging member
    std::set<TString> m_expectedHistograms; //!< Storage of names of expected histograms for implementation.
    std::unordered_map<std::string, TH1*> m_histogramMap; //!< Non-owning cache of histogram pointers, keyed to name.
    TableEntry m_baseEntry; //!< Base TableEntry, not used to store any row data. Only used to store column information.

};

#endif // TRIGCOSTANALYSIS_TABLECONSTRUCTORBASE_H
