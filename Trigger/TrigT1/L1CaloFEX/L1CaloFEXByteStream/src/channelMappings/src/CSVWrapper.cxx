#include "channelMappings/CSVWrapper.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
/*!
 * \class CSVWrapper
 * Class used to read in/out/print csv tables of mappings written 
 * by generateMapping.py in efexControl to use in place of currently missing 
 * database solutions
 */
CSVWrapper::CSVWrapper(std::ifstream& inputfile):
    m_valid(false) , m_firstsearch(true) , m_table(), m_columnwidth(0)
{
    std::istream_iterator<std::string> file_start{inputfile};
    std::istream_iterator<std::string> file_end;

    std::vector<std::string> fileContent;
    std::copy(file_start,file_end,back_inserter(fileContent));  // copy file content into vector

    fileContent.erase(begin(fileContent));  // first line is comment
    // WARNING if the line contains spaces, it is 
    // treted as several lines!!!

    for (auto& line: fileContent){      // iterate over lines, unpack
        std::vector<std::string>   table_entry;

        std::string                cell;
        std::stringstream          lineStream(line);

        while(std::getline(lineStream,cell, ','))
            table_entry.push_back(cell);                          
        m_table.push_back(table_entry);
    }
    m_columnwidth = m_table.at(10).size();
    //Intialise prev results vector
    m_previousresults.push_back(m_table);
    m_valid = true;
}
/*!
 * \param argumentpairs a vector of search inputs where the 1st value is the
 * column index and the second 
 * \return a shared pointer to a row that uniquely matches critereon
 */
std::shared_ptr<CSVWrapper::tableline> CSVWrapper::FindLine(
    const std::vector<std::pair<int,std::string>>& argumentpairs){
    std::vector<CSVWrapper::tableline> result = QueryTable(argumentpairs);
    //Check for unique result, if not throw exception
    if ( result.size() == 0 )
    {
      CSVWrapper::tableline returnval;
      std::shared_ptr<CSVWrapper::tableline> return_ptr (
            new CSVWrapper::tableline (returnval));
      return return_ptr;
    }
    if(result.size() != 1) {
        std::cout << "Duplicate found!" << std::endl;
        for (auto res: result) {
            for (auto r: res) {
                std::cout << r << ", ";
            }
            std::cout << std::endl;
        }
        throw  "Non unique result to FindLine";
    }
    
    CSVWrapper::tableline returnval = result.at(0);
    std::shared_ptr<CSVWrapper::tableline> return_ptr (
            new CSVWrapper::tableline (returnval));
    return return_ptr;
}

/*!
 * \param argumentpairs a vector of search inputs where the 1st value is the
 * column index and the second 
 * \return a list of shared pointers to all rows that match the critereon
 */
std::vector<std::shared_ptr<CSVWrapper::tableline>> CSVWrapper::FindLines(
    const std::vector<std::pair<int,std::string>>& argumentpairs){
    std::vector<CSVWrapper::tableline> result = QueryTable(argumentpairs);

    std::vector<std::shared_ptr<CSVWrapper::tableline>> return_vec;
    if ( result.size() == 0 )
    {
      CSVWrapper::tableline returnval;
      std::shared_ptr<CSVWrapper::tableline> return_ptr (
            new CSVWrapper::tableline (returnval));
      return_vec.push_back(return_ptr);
    }
    else {
        for (CSVWrapper::tableline returnval: result) {        
            std::shared_ptr<CSVWrapper::tableline> return_ptr (
                new CSVWrapper::tableline (returnval));
            return_vec.push_back(return_ptr);
        }
    }
    return return_vec;
}

CSVWrapper::tableline  CSVWrapper::GetLine(const int element) const{
    return m_table.at(element);
}

/*!
 * \param sample bool defaulted to false that when true prints 
 * only the last 100 lines
 */
void CSVWrapper::PrintTable(bool sample) const{
    if(m_valid){
        size_t rowidx(0) ;
        for(const auto& row : m_table){
            if(!sample || (m_table.size()-rowidx)<100){
                for(size_t idx(0);idx<row.size();idx++ ){
                    std::cout << row.at(idx) << " , "; 
                }
                std::cout << std::endl;
            }
            rowidx++;
        }
    }
}
//Check what values compared to the last FindLine search remain the same and are in the same order, halt if no match
//i.e match,match,not_match,match will return match,match,not_match,notmatch
std::vector<bool> CSVWrapper::ComparePreviousSearch(const std::vector<std::pair<int,std::string>>& argumentpairs){ 
    std::vector<bool> matchvector(argumentpairs.size(), false);
    if(m_firstsearch){
        m_firstsearch = false;
    }
    else{
        int idx = 0;
        for(const auto& newsearch : argumentpairs){
            matchvector.at(idx) = ((newsearch.first == m_prevsearch.at(idx).first) && 
                                   (newsearch.second == m_prevsearch.at(idx).second));
            if(!matchvector.at(idx)) break; //Escape if match fails
            idx++;
        }
    }
    m_prevsearch = argumentpairs;
    return matchvector;
}

std::vector<CSVWrapper::tableline> CSVWrapper::QueryTable(const std::vector<std::pair<int,std::string>>& 
            argumentpairs) 
{
   //Loop over every column argument in the find statement
    size_t searchcolumnidx = 0;
    auto matches = this->ComparePreviousSearch(argumentpairs);
    //std::vector<CSVWrapper::tableline> result = m_table;
    std::vector<CSVWrapper::tableline> result;
    for(bool matched : matches ){
        if(!matched){ //If not the same search as before (in this column)
            //Complete one column  search routine
            result = m_previousresults.at(searchcolumnidx); 
            auto search = argumentpairs.at(searchcolumnidx);
            auto findmismatch = [&] (CSVWrapper::tableline row){
                return (row.at(search.first) != search.second);
            };
            result.erase(std::remove_if(result.begin(), result.end(), findmismatch)
                    , result.end() );
            if(m_previousresults.size() <= searchcolumnidx + 1){//If unintialised
                m_previousresults.push_back(result);
            }
            else{
                m_previousresults.at(searchcolumnidx+1) = result;
            }
        }
        searchcolumnidx++;
    }
    //Cover Case that exactly the same search as before
    if(matches.back() == true){result = m_previousresults.at(searchcolumnidx);}
    return result;
}
