/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoring_HistogramDef_h
#define AthenaMonitoring_HistogramDef_h

#include <stdexcept>
#include <string>
#include <vector>

namespace Monitored {
  struct HistogramDefParseException : public std::runtime_error {
    HistogramDefParseException(const std::string &message)
        : std::runtime_error(message) {}
  };
  /**
   * @brief the internal class used to keep parsed Filler properties
   *
   */
  struct HistogramDef {
    std::vector<std::string> name; //!< names of variables
    std::string alias;             //!< histogram name alias
    std::string type;              //!< histogram type
    std::string path;              //!< booking path
    std::string title;             //!< title of the histogram
    std::string opt;               //!< options
    std::string tld{""};           //!< top level directory (below THistSvc stream)
    std::string convention;        //!< path naming convention (e.g. OFFLINE)
    std::string weight;            //!< names of weights

    int xbins{0};  //!< number of bins in X
    float xmin{0}; //!< left
    float xmax{0}; //!< right
    std::vector<double> xArray; //!< array of x bins

    int ybins{0};  //!< number of bins in Y
    float ymin{0}; //!< bottom
    float ymax{0}; //!< top
    std::vector<double> yArray; //!< array of y bins

    float zmin{0}; //!< in
    float zmax{0}; //!< out

    bool ok{false};   //!<  good declaration
    bool ycut{false}; //!<  TProfile with cut on y
    bool zcut{false}; //!<  TProfile2D with cut on z

    std::vector<std::string> labels; //!< bins labels
      
    /**
     * @brief Parses histogram defintion.
     * 
     * Example:
     * 1D: "EXPERT, TH1I, Name;Alias, Title, nBins, xmin, xmax, BinLabel1:BinLabel2:BinLabel3, kCumulative"
     * 
     * @param histogramDefinition A string contains histogram definition to parse
     * @return HistogramDef object that represent parsed histogram definition
     * @throw HistogramDefParseException If the histogram definition parse process failed
     */
    static const HistogramDef parse(const std::string &histogramDefinition);

  private:
    /**
     * @brief TokenException represents an error that occurs during parsing histogram definition.
     */
    struct TokenException : public std::runtime_error {
      /**
       * @brief Constructor of TokenException.
       * 
       * @param message Error description
       */
      TokenException(const std::string &message)
          : std::runtime_error(message) {}
    };

    /**
     * @brief Splits string into an array of substrings using separator. 
     * 
     * Leading and trailing whitespace characters of the substrings will be removed.
     * 
     * @param input String to split
     * @param separator Separator
     * @return An array of substrings
     */
    static std::vector<std::string> splitWithSeparator(const std::string &input, const char *separator);
     /**
     * @brief Returns current value pointed by iterator and advances it.
     * 
     * @param propertiesIterator Iterator to advance
     * @return Value pointed by iterator before it advances
     */
    static std::string nextProperty(std::vector<std::string>::iterator &propertiesIterator);
    /**
     * @brief Parses string into a desired type.
     * 
     * @tparam T Desired type of the token
     * @param token String to parse
     * @param errorDescription Error message
     * @return Parsed value
     * @throws TokenException If error occures during parsing process. It will contain errorDescription
     */
    template <typename T>
    static T parseToken(const std::string &token, const std::string &errorDescription);
    /**
     * @brief Based on variables' names, method will fill up alias field of the HistogramDef object.
     * 
     * @param histogramDefinition HistogramDef object to modify
     */
    static void resolveAlias(HistogramDef &histogramDefinition);
    /**
     * @brief Based on histogram type, method resolves and fill up axes' informations.
     * 
     * @param histogramDefinition HistogramDef object to modify
     * @param properties Set of all string preperties that defines the histogram
     * @param propertiesIterator Iterator that points at the beginning of axes definition
     */
    static void resolveAxies(HistogramDef &histogramDefinition, std::vector<std::string> &properties, std::vector<std::string>::iterator &propertiesIterator);
  };
} // namespace Monitored

#endif /* AthenaMonitoring_HistogramDef_h */