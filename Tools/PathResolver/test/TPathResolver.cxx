/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/




#include <PathResolver/PathResolver.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>

void batch( );
void interactive( int argc, char* argv[] );

int main ( int argc, char* argv[] )
{
    if ( argc <= 1 ) {
        batch( );
    } else {
        interactive( argc, argv );
    }
}

void batch( )
{
  char b1[256];
  //char b2[256];
  //char b3[256];

  sprintf (b1, "APATH=../test:../test/A:../test/B");

  ::putenv (b1);

  const char* env_ptr = ::getenv ("APATH");
  if (env_ptr != 0)
    {
      std::string env = env_ptr;

      std::cout << "APATH=" << env << std::endl;
    }

  std::string name;
#ifndef ROOTCORE
  name = PathResolverFindFile ("a.txt", "APATH");
  std::cout << "1) [a.txt] Name = " << name << " [expected : ../test/A/a.txt]" << std::endl;

  name = PathResolverFindFile ("x", "APATH");
  std::cout << "2) [x] Name = " << name << " [expected : ]" << std::endl;

  name = PathResolverFindDirectory ("x", "APATH");
  std::cout << "3) [x] Name = " << name << " [expected : ../test/A/x]" << std::endl;
#endif
  name = PathResolver::find_file ("b.txt", "APATH");
  std::cout << "4) [b.txt] Name = " << name << " [expected : ../test/A/b.txt]" << std::endl;

  name = PathResolver::find_file ("c.txt", "APATH");
  std::cout << "5) [c.txt] Name = " << name << " [expected : ../src/dd.txt]" << std::endl;

  name = PathResolver::find_file ("A/a.txt", "APATH");
  std::cout << "6) [A/a.txt] Name = " << name << " [expected : ../test/A/a.txt]" << std::endl;

  name = PathResolver::find_file ("A/d.txt", "APATH");
  std::cout << "7) [A/d.txt] Name = " << name << " [expected : ]" << std::endl;

  name = PathResolver::find_file ("A/e.txt", "APATH");
  std::cout << "8) [A/e.txt] Name = " << name << " [expected : ]" << std::endl;

  name = PathResolver::find_file ("e.txt", "APATH", PathResolver::RecursiveSearch);
  std::cout << "9) [e.txt] Name = " << name << " [expected : ./s/t/u/e.txt]" << std::endl;

}

void interactive( int argc, char* argv[] )
{
    std::string name;
    std::string path;
    
    path = "DATAPATH";
    if ( argc > 2) {
        path = argv[2];
    }
    std::string file = argv[1];
    name = PathResolver::find_file( file, path );
    std::cout << "File   : " << file << std::endl;
    std::cout << "  found: " << name << std::endl;
}


