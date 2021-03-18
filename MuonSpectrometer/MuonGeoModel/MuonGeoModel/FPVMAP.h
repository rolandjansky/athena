/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPVMAP_H
#define FPVMAP_H

#include <map>
#include <string>

class GeoVPhysVol;

namespace MuonGM {

typedef std::map<std::string,GeoVPhysVol* >::const_iterator DetectorIterator;

class FPVMAP {
public:
  FPVMAP();
  ~FPVMAP()=default;
     inline DetectorIterator Begin();
    inline DetectorIterator End();
    inline int              NDetectors();
    inline int              NDetectorsReused();
    
    GeoVPhysVol* GetDetector(std::string name);
    void StoreDetector(GeoVPhysVol *s, std::string name);
    void PrintAllDetectors();
private:

    std::map<std::string,GeoVPhysVol* > m_Detectors;
    int m_nreused;
    
};
DetectorIterator FPVMAP::Begin()
{
    return m_Detectors.begin();
}
DetectorIterator FPVMAP::End()
{
    return m_Detectors.end();
}
int FPVMAP::NDetectors()
{
    return m_Detectors.size();
}
int FPVMAP::NDetectorsReused()
{
    return m_nreused;
}
} // namespace MuonGM

#endif
