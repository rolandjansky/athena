/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define GRLReader_cxx
#include "../MuonCalibrationFit/GRLReader.h"

GRLReader::GRLReader( string grl_name ) { 

  string line;
  ifstream grl_file( grl_name.c_str() );
  vector< string > all_runs;
  if( grl_file.is_open() ) { 
    //regex the_reg( "<Run>(\\d+)</Run>" );
    //cmatch the_match;
    boost::regex the_reg( "<Run>(\\d+)</Run>" );
    boost::smatch the_match;
    while( getline( grl_file, line ) ) {
      //if( regex_match( line.c_str(), the_match, the_reg ) ) {
      if( boost::regex_match( line, the_match, the_reg, boost::match_extra ) ) {
        all_runs.push_back( the_match[ 1 ] );
      }
    }
    grl_file.close();
  }
  for( auto run_s : all_runs ) { 
    int run = atoi( run_s.c_str() );
    bool found_run_line = false;
    grl_file.open( grl_name.c_str() );
    grl_vec vec;  
    if( grl_file.is_open() ) { 
      //regex run_reg( "<Run>" + run_s + "</Run>" );
      //regex lb_reg( "<LBRange Start=\"(\\d+)\" End=\"(\\d+)\"/>" );
      //regex end_reg( "</LumiBlockCollection>" );
      //cmatch lb_match;
      boost::regex run_reg( "<Run>" + run_s + "</Run>" );
      boost::regex lb_reg( "<LBRange Start=\"(\\d+)\" End=\"(\\d+)\"/>" );
      boost::regex end_reg( "</LumiBlockCollection>" );
      boost::smatch lb_match;
      while( getline( grl_file, line ) ) {
        //if( regex_match( line.c_str(), end_reg ) ) found_run_line = false;
        //if( regex_match( line.c_str(), run_reg ) ) found_run_line = true;
        if( boost::regex_match( line.c_str(), end_reg ) ) found_run_line = false;
        if( boost::regex_match( line.c_str(), run_reg ) ) found_run_line = true;
        if( found_run_line ) {
          //if( regex_match( line.c_str(), lb_match, lb_reg ) ) {
          if( boost::regex_match( line, lb_match, lb_reg, boost::match_extra ) ) {
            vec.push_back( make_pair( atoi( ( lb_match[ 1 ].str() ).c_str() ), atoi( ( lb_match[ 2 ].str() ).c_str() ) ) );
          }
        }
      }
      grl_file.close();
    }
    m_map[ run ] = vec;
  }
  //:::
  //Print();
}

GRLReader::~GRLReader() { }

void GRLReader::Print() {
  for( auto& key_value : m_map ) {
    cout << "   Loaded Run: " << key_value.first << endl;
    int i = 1;
    for( auto& element : key_value.second ) { 
      cout << "  Interval #" << i << ": " << element.first << " -> " << element.second << endl;
      i++;
    }
  }
}

bool GRLReader::PassGRL( int run, int lumiblock ) {
  if( m_map.find( run ) != m_map.end() ) {
    bool found_good_range = false;
    for( grl_vec::iterator itr = m_map[ run ].begin(); itr != m_map[ run ].end(); ++itr ) {
      if( ( *itr ).first <= lumiblock && lumiblock <= ( *itr ).second ) found_good_range = true;
    }
    return found_good_range;
  }
  else return false;
}
