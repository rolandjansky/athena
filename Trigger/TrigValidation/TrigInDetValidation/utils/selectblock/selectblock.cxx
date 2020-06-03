/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    selectblock.cxx         
//   
//
//   @author M.Sutton
// 
//
//   $Id: selectblock.cxx, v0.0   Mon 23 Jan 2017 12:30:25 CET sutt $

#include <iostream> 
#include <fstream> 
#include <string> 
#include <vector> 
#include <cstdlib> 
#include <cstdio> 

template<typename T>
std::ostream& operator<<( std::ostream& s, std::vector<T>& v ) { 
  for ( size_t i=0 ; i<v.size() ; i++ ) s << v[i] << std::endl;
  return s; 
}


void replace( std::vector<std::string> b, int position, const std::string& regex, const std::string& pattern ) { 
  for ( int i=0 ; i<b.size() ; i++ ) { 

    size_t p0 = b[i].find( "position=" );
    
    std::string newline;

    if ( p0!=std::string::npos ) { 
      char nl[128];

      sprintf( nl, b[i].substr( 0, p0 ).c_str() );
      
      std::string ns=nl;
      ns+="position=\"%02d\">";
      
      sprintf( nl, ns.c_str(), position );
      
      
      newline = nl;
    }
    else { 

      size_t p = b[i].find( regex );

      if ( p!=std::string::npos ) { 
	newline = b[i].replace( p, regex.size(), pattern );
      }
      else newline = b[i];
    }

    if ( newline.find("-plots")!=std::string::npos ) { 
      newline.replace( newline.find("-plots"), 6, "-test-plots" );
    }

    std::cout << newline << std::endl;
  }
}


bool comparitor( std::vector<std::string>& b ) { 
  //  for ( size_t i=0 ; i<b.size() ; i++ ) if ( b[i].find("TIDAcomparitor")!=std::string::npos ) return true;
  for ( size_t i=0 ; i<b.size() ; i++ ) if ( b[i].find("TIDArun")!=std::string::npos ) return true;
  return false;
}


int main( int argc, char** argv ) {
  
  if ( argc<2 ) return -1;

  std::ifstream file( argv[1] );

  std::vector<std::vector<std::string> > block(1,std::vector<std::string>() );  

  bool start = false;

  std::string line;

  std::vector<int> position(1, 0);

  int iblock = 0;

  while( getline( file, line ) && !file.fail() ) {

    if ( line.find("action")!=std::string::npos ) { 
      if ( line.find("/action")==std::string::npos ) { 
	start = true;
	std::string tmp = line;
	size_t pos = tmp.find("\"");
	std::string p = tmp.substr( pos+1, tmp.size()-pos-1 );
	pos = p.find("\"");
	p = p.substr( 0, pos );
	//	position[iblock] = std::atoi( p.c_str() );

	//	std::cout << "pos: " << position << std::endl;

	if ( iblock==0 ) { 
	  iblock++;
	  block.push_back(std::vector<std::string>());
	  position.push_back( 0 );
	}
	position[iblock] = std::atoi( p.c_str() );
      }
    }
    
    block[iblock].push_back(line);

    
    if ( line.find("/action")!=std::string::npos ) { 
      start = false;
      iblock++;
      block.push_back(std::vector<std::string>());
      position.push_back( 0 );
    }
    
  }

  //  std::cout << "\nBLOCKS " << block.size() << "\t" << position.size() << std::endl;


  int maxblock = 0;

  for ( size_t i=0 ; i<block.size() ; i++ ) { 
    if ( comparitor(block[i]) && position[i]>position[maxblock] ) maxblock = i;
  }

  int lastblock = 0;
  
  for ( size_t i=0 ; i<block.size() ; i++ ) if ( position[i]>0 ) lastblock = i;

  //  std::cout << "maxblock " << maxblock << std::endl;

  for ( size_t i=0 ; i<=lastblock ; i++ ) { 
    if ( !comparitor( block[i] ) ) std::cout << block[i] << std::endl;
  }
  
  int nblock = position[maxblock]+1;

  //  for ( size_t i=0 ; i<block.size() ; i++ ) { 
  //    if ( comparitor(block[i]) && position[i]>0 ) { 
  //      replace( block[i], nblock++, "TIDAcomparitor.exe", "TIDArun.sh" );
  //    }
  //  }

  for ( size_t i=lastblock+1 ; i<block.size() ; i++ ) std::cout << block[i] << std::endl;
 

  return 0;
} 

