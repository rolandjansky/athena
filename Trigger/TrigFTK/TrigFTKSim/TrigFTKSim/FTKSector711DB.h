/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKSector711DB_H
#define FTKSector711DB_H

#include <string>
#include <vector>

class FTKSector711DB {
private:
  // path of the file describing
  std::string m_path;

  int m_nsectors; // number of 7L sectors
  int m_nextralayers; // number of extra layers described in the table
  int *m_nrelations; // number of relations for each 7L sector
  int **m_subID; // sub-region ID of each realtion
  int **m_secID; // related sector
  int ***m_stereoIDs; // stereo module IDs

public:
  FTKSector711DB(int, int, const char *);
  ~FTKSector711DB();

  const char* getPath() const { return m_path.c_str(); }
  const int& getNSectors() const { return m_nsectors; }
  const int& getNExtraLayers() const { return m_nextralayers; }

  const int& getNSimilarSectors(int secid) const 
    { return m_nrelations[secid]; }
  const int& getSimilarSubID(int secid, int irel) const 
    { return m_subID[secid][irel]; }
  const int& getSimilarSecID(int secid, int irel) const 
    { return m_secID[secid][irel]; }
  const int* getSimilarStereoIDs(int secid, int irel) const 
    { return m_stereoIDs[secid][irel]; }

};
#endif // FTKSector711DB_H
