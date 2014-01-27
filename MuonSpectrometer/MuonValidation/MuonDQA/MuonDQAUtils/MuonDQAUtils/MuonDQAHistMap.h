/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
// Package : MuonDQAUtils - MuonDQAHistMap
// Author:   N. Benekos(MPI) - M. Woudstra(University of Massachusetts), J. Griffiths(University of Washington)
// August. 2007
//
// DESCRIPTION:
// Subject: Offline Muon Data Quality --> returns maps of HistLists
///////////////////////////////////////////////////////////////////////////

#ifndef MuonDQAHistMap_H
#define MuonDQAHistMap_H

#include <sstream>
#include <string.h>
#include <vector>
#include <map>
 
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

  
using namespace std;
 
/////////////////////////////////////////////////////////////////////////////

class MuonDQAHistList {
 public:
  void clear() {
    m_H1s.clear();
    m_H2s.clear();
  }
  void addHist( TH1* p ) {
    m_H1s.push_back(p);
  }
  void addHist( TH2* p ) {
    m_H2s.push_back(p);
  }
  TH1* getH1( const std::string& hist_name ) const {
    std::vector<TH1*>::const_iterator it = m_H1s.begin(),
      itEnd = m_H1s.end();
    for ( ; it != itEnd; ++it ) {
      if ( (*it)->GetName() == hist_name ) {
	return *it;
      }
    }
    return 0;    
  }
  TH2* getH2( const std::string& hist_name ) const {
    std::vector<TH2*>::const_iterator it = m_H2s.begin(),
      itEnd = m_H2s.end();
    for ( ; it != itEnd; ++it ) {
      if ( (*it)->GetName() == hist_name ) {
	return *it;
      }
    }
    return 0;
  }
   
 private:
  std::vector<TH1*> m_H1s;
  std::vector<TH2*> m_H2s;
};
 
class MuonDQAHistList_LW {
 public:
  void clear() {
    m_H1s.clear();
    m_H2s.clear();
  }
  void addHist( TH1F_LW* p ) {
    m_H1s.push_back(p);
  }
  void addHist( TH2F_LW* p ) {
    m_H2s.push_back(p);
  }

  TH1F_LW* getH1( const std::string& hist_name ) const {
    std::vector<TH1F_LW*>::const_iterator it = m_H1s.begin(),
      itEnd = m_H1s.end();
    for ( ; it != itEnd; ++it ) {
      if ( (*it)->GetName() == hist_name ) {
	return *it;
      }
    }
    return 0;    
  }
  TH2F_LW* getH2( const std::string& hist_name ) const {
    std::vector<TH2F_LW*>::const_iterator it = m_H2s.begin(),
      itEnd = m_H2s.end();
    for ( ; it != itEnd; ++it ) {
      if ( (*it)->GetName() == hist_name ) {
	return *it;
      }
    }
    return 0;
  }
   
 private:
  std::vector<TH1F_LW*> m_H1s;
  std::vector<TH2F_LW*> m_H2s;
};  
   
class MuonDQAHistMap {
 public:
  void clear() {
    m_data.clear();
  }
  
  void addList( const std::string& station_name, const MuonDQAHistList& lst ) {
    m_data[station_name] = lst;
  }
	
  MuonDQAHistList& getList( std::string station_name ) {
    std::map<std::string,MuonDQAHistList>::iterator it;
    it = m_data.find( station_name );
    if ( it != m_data.end() ) {
      return it->second;
    } else {
      m_data[station_name] = MuonDQAHistList();
      return m_data[station_name];
    }
  }
 
  void addList( const int& station_idInt, const MuonDQAHistList& lst ) {
    m_data_int[station_idInt] = lst;
  }
	
  MuonDQAHistList& getList( int station_idInt) {
    std::map<int, MuonDQAHistList>::iterator it;
    it = m_data_int.find( station_idInt );
    if ( it != m_data_int.end() ) {
      return it->second;
    } else {
      m_data_int[station_idInt] = MuonDQAHistList();
      return m_data_int[station_idInt];
    }
  }
  
 private:
  std::map< std::string, MuonDQAHistList > m_data;
  std::map< int, MuonDQAHistList > m_data_int;
};

class MuonDQAHistMap_LW {
 public:
  void clear() {
    m_data.clear();
  }
  
  void addList( const std::string& station_name, const MuonDQAHistList_LW& lst ) {
    m_data[station_name] = lst;
  }
	
  MuonDQAHistList_LW& getList( std::string station_name ) {
    std::map<std::string,MuonDQAHistList_LW>::iterator it;
    it = m_data.find( station_name );
    if ( it != m_data.end() ) {
      return it->second;
    } else {
      m_data[station_name] = MuonDQAHistList_LW();
      return m_data[station_name];
    }
  }
 
  void addList( const int& station_idInt, const MuonDQAHistList_LW& lst ) {
    m_data_int[station_idInt] = lst;
  }
	
  MuonDQAHistList_LW& getList( int station_idInt) {
    std::map<int, MuonDQAHistList_LW>::iterator it;
    it = m_data_int.find( station_idInt );
    if ( it != m_data_int.end() ) {
      return it->second;
    } else {
      m_data_int[station_idInt] = MuonDQAHistList_LW();
      return m_data_int[station_idInt];
    }
  }
  
 private:
  std::map< std::string, MuonDQAHistList_LW > m_data;
  std::map< int, MuonDQAHistList_LW > m_data_int;
};
	
#endif
