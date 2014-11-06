/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __APPLE__
#include "ers/ers.h"
#include <dcap.h>

#include "fReaddCache.h"
#include "EventStorage/EventStorageIssues.h"

fReaddCache::fReaddCache() 
{
  m_pfd = NULL;
} 

fReaddCache::~fReaddCache() 
{
  this->closeFile();
}

bool fReaddCache::isOpen() {return m_pfd != NULL;}

bool fReaddCache::isEoF() 
{
  if(this->isOpen()) {
    return dc_feof(m_pfd);
  } else {
    return false;
  }
}

bool fReaddCache::fileExists(std::string fName) const
{
  char * nm = new char[fName.size()+1];
  memcpy(nm,fName.c_str(),fName.size());
  nm[fName.size()] = 0;

  FILE *pf = dc_fopen(nm,(char *)"r");

  bool isThere = (pf != NULL);

  if(isThere) dc_fclose(pf);
  delete[] nm;
 
  return isThere;
}

void fReaddCache::openFile(std::string fName) 
{
  if(this->isOpen()) this->closeFile();

  char * nm = new char[fName.size()+1];
  memcpy(nm,fName.c_str(),fName.size());
  nm[fName.size()] = 0;

  m_pfd = dc_fopen(nm,(char *)"r");

  // A missing initialization was discovered in dCache.
  // Diagnosed by Tadashi. It will be fixed in the next release
  // of dCache. For the moment we do the reset ourselves.
  if(m_pfd!=NULL) m_pfd->_flags = 0;

  delete[] nm;
}

void fReaddCache::closeFile() 
{
  if(m_pfd != NULL) dc_fclose(m_pfd);
  m_pfd = NULL;
}

void fReaddCache::readData(char *buffer, unsigned int sizeBytes) 
{
  if (sizeBytes==0) return;
  if(this->isOpen())
    {
      unsigned int totalRead=0,ntry=0;
      while (sizeBytes > totalRead)
	{
	  int ret = dc_fread(buffer+totalRead,sizeof(char),
			     (sizeBytes-totalRead),m_pfd);
	  totalRead += ret; ++ntry;
	  if(ntry>5) {
	    /*  ERS_WARNING("Problem reading from the data file. "
			<<"fReadCastor::readData asked to read "<<sizeBytes
			<<" bytes and managed to read only "<<totalRead
			<<" bytes.");
	    */
	    std::stringstream mystream;
	    mystream << "Problem reading from the data file. "
			<<"fReaddCache::readData asked to read "<<sizeBytes
			<<" bytes and managed to read only "<<totalRead
			<<" bytes.";  
	    EventStorage::ReadingIssue ci(ERS_HERE, mystream.str().c_str());
	    ers::warning(ci);


	    return;
	  }
	}
    }
}

int64_t fReaddCache::getPosition()
{
  if(this->isOpen()) return dc_ftell(m_pfd);
  return -1;
}
 
void fReaddCache::setPosition(int64_t p) 
{  
  if(this->isOpen()) dc_fseek(m_pfd,p,SEEK_SET);
}

void fReaddCache::setPositionFromEnd(int64_t p)
{  
  if(this->isOpen()) dc_fseek(m_pfd,p,SEEK_END);
}
 
fRead * fReaddCache::newReader() const
{
  fReaddCache * nfr = new fReaddCache();
  return (fRead *)nfr;
}

extern "C" {
  fRead * fReadFactory() 
  {
    fReaddCache * nfr = new fReaddCache();
    return (fRead *)nfr;
  }
}
#endif
