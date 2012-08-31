/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HTMLUtil_cxx_
#define __HTMLUtil_cxx_

#include "HTMLUtil.h"
#include <fstream>


void CreateMain(const std::string www_dir){
  //--------------- Main Page ---------------//
  // setting //
  string pagefile = www_dir + "TrigMenuNtPlot.html";
  std::ofstream ofs;
  ofs.open( pagefile.c_str() );

  ofs << "<html>" << std::endl;
  // header //
  ofs << "<head>" << std::endl;
  ofs << "<title>TrigMenuNtPlot</title>" << std::endl; 
  ofs << "</head>" << std::endl;
  // main //
  ofs << "<frameset cols=\"15%,*\">" << std::endl;
  ofs << "  <frame src=\"ChainMenu.html\" name=\"frame1\">" << std::endl;
  ofs << "  <frame src=\"Top.html\" name=\"frame2\">" << std::endl;
  ofs << "</html>" << std::endl;
  ofs.close();
}

void CreateTop(const std::string textFile,
	       const std::string www_dir){
  //--------------- Top Page ---------------//
  string pagefile = www_dir + "Top.html";
  std::ofstream ofs;
  ofs.open( pagefile.c_str() );

  ofs << "<html>" << std::endl;
  // header //
  ofs << "<head>" << std::endl;
  ofs << "<title>Top</title>" << std::endl; 
  ofs << "</head>" << std::endl;
  // main //
  ifstream ifs( textFile .c_str() );
  if( !ifs ){
    std::cout << "file open error : " << textFile << " is not exist"<<std::endl;
  }
  string buf;
  while( getline(ifs, buf) ){
    ofs << buf << "<br>" << std::endl;
  }
  ofs << "</html>" << std::endl;
  ofs.close();
}

void CreateMenu(std::vector<std::string> e_chain,
		std::vector<std::string> mu_chain,
		std::vector<std::string> j_chain,
		const std::string www_dir){
  // setting //
  string pagefile = www_dir + "ChainMenu.html";
  std::ofstream ofs;
  ofs.open( pagefile.c_str() );

  ofs << "<html>" << std::endl;
  // header //
  ofs << "<head>" << std::endl;
  ofs << "<title>ChainMenu</title>" << std::endl; 
  ofs << "</head>" << std::endl;
  // main //
  ofs << "-- Electron --<br>" << std::endl;
  for( int i=0; i<(int)e_chain.size(); i++ ) {
    ofs <<"<a href=\"" << e_chain[i] << ".html\" target=\"frame2\">";
    ofs << e_chain[i] << "</a><br>" << std::endl;
  }
  ofs << "-- Muon --<br>" << std::endl;
  for( int i=0; i<(int)mu_chain.size(); i++ ) {
    ofs <<"<a href=\"" << mu_chain[i] << ".html\" target=\"frame2\">";
    ofs << mu_chain[i] << "</a><br>" << std::endl;
  }
  ofs << "-- Jet --<br>" << std::endl;
  for( int i=0; i<(int)j_chain.size(); i++ ) {
    ofs <<"<a href=\"" << j_chain[i] << ".html\" target=\"frame2\">";
    ofs << j_chain[i] << "</a><br>" << std::endl;
  }
  ofs << "</html>" << std::endl;
  ofs.close();
}



void img2html (std::vector<std::string> chain,
	       const std::string width,
	       const std::string height,
	       const std::string file_type,
	       const std::string www_dir) {
  
  for( int i=0; i<(int)chain.size(); i++ ) {
    // setting //
    string pagefile = www_dir + chain[i] + ".html";
    std::ofstream ofs;
    ofs.open( pagefile.c_str() );
    
    ofs << "<html>" << std::endl;
    // header //
    ofs << "<head>" << std::endl;
    ofs << "<title>" << chain[i] << "</title>" << std::endl; 
    ofs << "</head>" << std::endl;
    // main //
    ofs << chain[i] << "<br>" << std::endl;
    ofs <<"<img src=\"" << chain[i] << file_type;
    ofs<<   "\" width="<< width <<" height=" << height << ">";
    ofs << "</html>" << std::endl;
    ofs.close();
  }
}

#endif//__HTMLUtil_cxx_
