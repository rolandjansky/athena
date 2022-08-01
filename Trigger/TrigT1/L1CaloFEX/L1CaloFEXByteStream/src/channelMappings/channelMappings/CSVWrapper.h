#ifndef CSV_WRAPPER_H
#define CSV_WRAPPER_H

#include <vector>
#include <memory>
#include <fstream>

class CSVWrapper {
    public:
        using tableline = std::vector<std::string>;
        //Empty Constructor For initialisation
        CSVWrapper();
        //Constructor
        CSVWrapper(std::ifstream& inputfile);
        std::shared_ptr<tableline> FindLine(const std::vector<std::pair<int,std::string>>& 
            argumentpairs);
        std::vector<std::shared_ptr<tableline>> FindLines(const std::vector<std::pair<int,std::string>>& 
            argumentpairs);

        tableline  GetLine(const int element) const;
        void PrintTable(bool sample = false) const;
    private:
        std::vector<CSVWrapper::tableline> QueryTable(const std::vector<std::pair<int,std::string>>& 
            argumentpairs);
        std::vector<bool> ComparePreviousSearch(const std::vector<std::pair<int,std::string>>&
            argumentpairs);
        bool m_valid;
        bool m_firstsearch;
        std::vector<tableline> m_table;
        size_t m_columnwidth;
        std::vector<std::pair<int,std::string>> m_prevsearch;
        std::vector<std::vector<tableline>> m_previousresults;
};
#endif
