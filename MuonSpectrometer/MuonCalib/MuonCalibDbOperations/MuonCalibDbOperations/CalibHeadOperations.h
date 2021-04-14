/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CalibHeadOperations_h
#define CalibHeadOperations_h

// c - c++
#include <string>
#include <vector>

namespace coral {
    class IQuery;
    class AttributeList;
}  // namespace coral

namespace MuonCalib {

    class CalibDbConnection;

    class CalibHeadOperations {
    public:
        //=====================constructor - destructor=================================
        CalibHeadOperations(CalibDbConnection &db_conn);
        virtual ~CalibHeadOperations();
        //=====================publlic member functions=================================
        // Get latest HeadId
        int GetLatestHeadId() const;
        // Get header information
        bool GetHeadInfo(int &head_id, int &lowrun, int &uprun, int &lowtime, int &uptime);
        // Open data connection for given head_id. head_id=-1 => Get latest header
        CalibDbConnection *GetDataConnection(int head_id = -1, bool write = false, const std::string &writer_connection = "",
                                             const std::string &writer_user = "", const std::string &writer_password = "");
        //=====================private data members=====================================
    private:
        // connections
        CalibDbConnection *m_meta_connection;
    };  // class

}  // namespace MuonCalib

#endif
