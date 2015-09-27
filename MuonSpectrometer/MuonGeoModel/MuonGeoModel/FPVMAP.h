/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPVMAP_H
#define FPVMAP_H

#include <map>
#include <string>
class GeoVPhysVol;

namespace MuonGM {

typedef std::map<std::string,GeoVPhysVol* >::const_iterator DetectorIterator;

class FPVMAP {
// singleton
private:
    static FPVMAP* thePointer;
    FPVMAP();
    std::map<std::string,GeoVPhysVol* > Detectors;
    int m_nreused;
    

public:
    inline DetectorIterator Begin();
    inline DetectorIterator End();
    inline int              NDetectors();
    inline int              NDetectorsReused();
    
    static FPVMAP* GetPointer();
    GeoVPhysVol* GetDetector(std::string name);
    void StoreDetector(GeoVPhysVol *s, std::string name);
    void PrintAllDetectors();
};
DetectorIterator FPVMAP::Begin()
{
    return Detectors.begin();
}
DetectorIterator FPVMAP::End()
{
    return Detectors.end();
}
int FPVMAP::NDetectors()
{
    return Detectors.size();
}
int FPVMAP::NDetectorsReused()
{
    return m_nreused;
}
} // namespace MuonGM

#endif
