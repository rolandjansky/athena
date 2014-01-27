/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonDQAHist_HistDQADataHelper_H
#define MuonDQAHist_HistDQADataHelper_H

#include "HistDQAData.h"

#include <string>
#include <map>
#include "TProfile.h"

class TH1;
class TH2;
class TProfile;

namespace MuonDQAHist {
  class HistDQADataHelper {
  public:
    /** @brief get pointer to helper (singleton) */
    static HistDQADataHelper* getHelper();

    //     /** @brief destructor */
    ~HistDQADataHelper();

    /** @brief get a predefined HistDQAData object, returns zero if not found. 
	Memory managed by helper, do not delete pointer */
    HistDQAData* histData( std::string name ) ;
    
    /** @brief add new HistDQAData object, returns false if name already exists in map. 
	The helper takes ownership of the HistDQAData object 
	The new HistDQAData will not be added in that case but deleted */
    bool addHistData( std::string name, HistDQAData* data );

    /** @brief create a TH1 for a registered HistDQAData object, will return zero if HistDQAData not found.
	The ownership of the TH1 is passed to the caller. 
	If the HistDQAData corresponds to a 2D histogram a TH2 is created */
    TH1* createHist( std::string name, std::string prefix );

    /** @brief create a TH2 for a registered HistDQAData object, will return zero if no HistDQAData2D is found.
	The ownership is passed to the caller */
    TH2* createHist2D( std::string name, std::string prefix );
    
    /** @brief create a TProfile for a registered HistData object, will return zero if HistData not found.
	The ownership of the TProfile is passed to the caller. */
    TProfile* createHistProf( std::string name, std::string prefix );
    
    /** create TH1 from a HistDQAData, will create a 1D hist or a 2D hist depending on the HistDQAData type
	The ownership is passed to the caller */
    TH1* createHist(  HistDQAData& data, std::string prefix );

    /** create TH1 from a one dimensional HistDQAData. The ownership is passed to the caller */
    TH1* createHist1D( HistDQAData& data, std::string prefix ) ;

    /** create TH2 from a HistDQAData2D. The ownership is passed to the caller*/
    TH2* createHist2D(  HistDQAData2D& data, std::string prefix ) ;

    /** create TProfile from a one dimensional HistData. The ownership is passed to the caller */
    TProfile* createHistProf( HistDQAData& data, std::string prefix ) ;

    //    int size() const { return m_histDQADataMap.size(); }
    
  private:
    void loadDefaults();
    typedef std::map<std::string,HistDQAData*> HistDQADataMap;
    typedef HistDQADataMap::iterator                 HistDQADataIt;
    static HistDQADataMap     m_histDQADataMap;     //<! map to store default HistDQAData objects
    //    static HistDQADataHelper* m_helperInstance;
    //    HistDataHelper* HistDataHelper::instance()
    //      {
    //	static HistDataHelper s_instance;
    //	return &s_instance;
    //      }
  };
}

#endif
