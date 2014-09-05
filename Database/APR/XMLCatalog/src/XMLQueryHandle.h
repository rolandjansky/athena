/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <vector>
namespace pool {

  class XMLQueryHandle{

  public:

    enum WildStatus {NONE, START, END, MIDDLE};
    XMLQueryHandle();
    
    void init(const std::string& query);
    const std::string& subquery(int i);
    const std::string& get_attrib(int i);
    const std::string& get_comman(int i);
    const std::string& get_value(int i);
    WildStatus is_wild(int i);
    unsigned int nqueries();

  private:
    std::vector<std::string> m_subquery;
    std::vector<std::string> m_attrib;
    std::vector<std::string> m_comman;
    std::vector<std::string> m_value;
    std::vector<WildStatus>  m_wild;
    unsigned int  m_nq;

  };

} //ns pool 
