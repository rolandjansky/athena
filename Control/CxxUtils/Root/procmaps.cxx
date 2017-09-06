/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include "CxxUtils/procmaps.h"
procmaps::Entry::Entry(const char* procMapsLine) :
  begAddress(0),endAddress(0),
  readable(false), writable(false), executable(false), isPrivate(false),
  offset(0), inode(0)
{
  dev[0]=0; dev[1]=0;
  memset(pathname,' ',31);
  char pageProts[5];
  memset(pageProts,' ', 4);
  sscanf(procMapsLine,
	 "%80lx-%80lx %4s %80x %2x:%2x %80x %31s",
	 &this->begAddress,
	 &this->endAddress,
	 pageProts,
	 &this->offset,
	 &this->dev[0],
	 &this->dev[1],
	 &this->inode,
	 this->pathname
	 );
  //printf("pageProts %s pathname <%s> \n", pageProts, pathname);
  this->readable = (pageProts[0] == 'r');
  this->writable = (pageProts[1] == 'w');
  this->executable = (pageProts[2] == 'x');
  this->isPrivate  = (pageProts[3] == 'p');
}

procmaps::procmaps(size_t entries) {
  m_pmaps.reserve(entries);
  loadMaps(false);
}

void 
procmaps::loadMaps(bool dump) {
  m_pmaps.clear();
  std::ifstream f("/proc/self/maps");
  const int LMAX=256;
  char line[LMAX];
  while ( f.getline(line,LMAX) ) {
    if (dump) printf("%s",line);
   m_pmaps.emplace_back(line);
  }
}

const procmaps::Entry*
procmaps::getEntry(const void * address, bool refreshMaps) {
  const procmaps::Entry* ret(0);
  //FIXME slow linear search. We'll make it faster...
  const_iterator i(this->begin()), e(this->end());
  bool found(false), done(false);
  unsigned long toMatch((unsigned long)address);
  while (!done && !found && i!=e) {
    //    printf(" begAddress %x toMatch %x endAddress %x\n", i->begAddress, toMatch, i->endAddress);
    found = i->begAddress <= toMatch && toMatch <= i->endAddress; 
    done = !found && toMatch < i->begAddress; //entries sorted by begAddress
    if (!found && !done) ++i;
  }
  if (found) ret=&*i;
  else  if (refreshMaps) {
    //if ! found recurse once by calling getEntry with refreshMaps false
    loadMaps();
    ret = getEntry(address,false);
  }
  return ret;
}
