/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ers/ers.h"
#include <fcntl.h>
#include <shift.h>

#include "fReadCastor.h"
#include "EventStorage/EventStorageIssues.h"

fReadCastor::fReadCastor() 
{
  m_pfd = NULL;
} 

fReadCastor::~fReadCastor() 
{
  this->closeFile();
}

bool fReadCastor::isOpen() {return m_pfd != NULL;}

bool fReadCastor::isEoF() 
{
  if(this->isOpen()) {
    return rfio_feof(m_pfd);
  } else {
    return false;
  }
}

bool fReadCastor::fileExists(std::string fName) const
{
  char * nm = new char[fName.size()+1];
  memcpy(nm,fName.c_str(),fName.size());
  nm[fName.size()] = 0;

  FILE *pf = rfio_fopen64(nm,(char *)"r");

  bool isThere = (pf != NULL);

  if(isThere) rfio_fclose(pf);
  delete[] nm;

  return isThere;
}

void fReadCastor::openFile(std::string fName) 
{
  if(this->isOpen()) this->closeFile();

  char * nm = new char[fName.size()+1];
  memcpy(nm,fName.c_str(),fName.size());
  nm[fName.size()] = 0;

  m_pfd = rfio_fopen64(nm,(char *)"r");

  delete[] nm;
}

void fReadCastor::closeFile() 
{
  if(m_pfd != NULL) rfio_fclose(m_pfd);
  m_pfd = NULL;
}

void fReadCastor::readData(char *buffer, unsigned int sizeBytes) 
{
  if (sizeBytes==0) return;
  if(this->isOpen())
    {
      unsigned int totalRead=0,ntry=0;
      while (sizeBytes > totalRead)
	{
	  int ret = rfio_fread(buffer+totalRead,sizeof(char),
			       (sizeBytes-totalRead),m_pfd);
	  totalRead += ret; ++ntry;
	  if(ntry>5) {
	    /* ERS_WARNING("Problem reading from the data file. "
			<<"fReadCastor::readData asked to read "<<sizeBytes
			<<" bytes and managed to read only "<<totalRead
			<<" bytes.");  
	    */
	    std::stringstream mystream;
	    mystream << "Problem reading from the data file. "
			<<"fReadCastor::readData asked to read "<<sizeBytes
			<<" bytes and managed to read only "<<totalRead
			<<" bytes.";  
	    EventStorage::ReadingIssue ci(ERS_HERE, mystream.str().c_str());
	    ers::warning(ci);

	    return;
	  }
	}
    }
}

int64_t fReadCastor::getPosition() 
{
  if(this->isOpen()) return rfio_ftello64(m_pfd);
  return -1;
}

void fReadCastor::setPosition(int64_t p) 
{  
  if(this->isOpen()) rfio_fseeko64(m_pfd,p,SEEK_SET);
}
 
void fReadCastor::setPositionFromEnd(int64_t p)
{  
  if(this->isOpen()) rfio_fseek(m_pfd,p,SEEK_END);
}
 
fRead * fReadCastor::newReader() const
{
  fReadCastor * nfr = new fReadCastor();
  return (fRead *)nfr;
}

extern "C" {
  fRead * fReadFactory() 
  {
    fReadCastor * nfr = new fReadCastor();
    return (fRead *)nfr;
  }
}
