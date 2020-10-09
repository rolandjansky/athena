/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTANALYSIS_TABLEENTRY_H
#define TRIGCOSTANALYSIS_TABLEENTRY_H 1

#include <string>
#include <unordered_map>
#include <vector>

#include "AsgMessaging/MsgStream.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TString.h"

/**
 * @class TableEntry
 * @brief Representation of a row in a table. Owns an ordered list of columns in the table and contains one payload value for each column.
 */
class TableEntry {
  public:
    /**
     * @brief Default constructor.
     */
    TableEntry();

    /**
     * @brief Append a column to this Table
     * @param[in] name Column identifier name
     * @param[in] header Column detailed name to export and use as the column title
     */
    void addColumn(const std::string& name, const std::string& header, const std::string& tooltip);

    /**
     * @brief Set this row's payload for one column. TODO consider templating
     * @param[in] name Column identifier name
     * @param[in] value Floating point payload
     */
    void setEntry(const std::string& name, const double value);

    /**
     * @brief Set this row's payload for one column. TODO consider templating
     * @param[in] name Column identifier name
     * @param[in] value Floating point payload
     */
    void setEntry(const std::string& name, const float value);

    /**
     * @brief Set this row's payload for one column. TODO consider templating
     * @param[in] name Column identifier name
     * @param[in] value Integer payload
     */
    void setEntry(const std::string& name, const int value);

    /**
     * @brief Set this row's payload for one column. TODO consider templating
     * @param[in] name Column identifier name
     * @param[in] value String payload
     */
    void setEntry(const std::string& name, const std::string& value);

    /**
     * @brief Apply a normalisation factor for one of this row's payload.
     * @param[in] name Column identifier name
     * @param[in] denominator Value to normalise by
     */
    void normaliseEntry(const std::string& name, const float denominator);

    /**
     * @brief Getter for one Column which has floating point data
     * @param[in] name Column identifier name
     * @return Stored floating point value in this TableEntry's row for the specified column
     */
    float getEntryFloat(const std::string& name) const;

    /**
     * @brief Get row of column headers for export into CSV file
     * @return CSV formatted string
     */
   TString getHeaderRow() const;

    /**
     * @brief Get this TableEntry's row data for export into CSV file
     * @return CSV formatted string
     */   
   TString getRow() const;

  private:
    /**
     * @brief Logging
     * @return Message stream reference.
     */  
    MsgStream& msg() const;

    mutable MsgStream m_msgStream; //!< Logging member
    std::vector<std::string> m_columnName; //!< Identifier of each column
    std::vector<std::string> m_columnHeader; //!< Printable name of each column
    std::vector<std::string> m_columnTooltip; //!< Printable details on each column to display as tooltip
    std::unordered_map<std::string, float> m_floats; //!< Storage of floating point column data, keyed by name. TODO consider to template this.
    std::unordered_map<std::string, int> m_ints; //!< Storage of integer column data, keyed by name. TODO consider to template this.
    std::unordered_map<std::string, std::string> m_strings; //!< Storage of string-based column data, keyed by name. TODO consider to template this.
};

#endif // TRIGCOSTANALYSIS_TABLEENTRY_H
