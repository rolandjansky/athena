/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeometryMap.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
#include <map>
#include <sys/types.h>
#include <regex.h>
// '^to\w*re$'   
class GeometryMap::Clockwork {

public:

  Clockwork():lV(0){}
  
  std::vector<std::string>                geometryRegex;
  std::vector<regex_t *>                  geometryPreg;
  std::vector<std::vector<std::string> >  pathList;
  std::vector<PVConstLink>                physicalVolumes;
  unsigned int                            lV;

  void finalize(unsigned int i, PVConstLink v);
};


GeometryMap::GeometryMap():
  m_c(new Clockwork()) {
}


GeometryMap::~GeometryMap() {
  delete m_c;
}

void GeometryMap::add(const std::string & Path) {
  std::vector <std::string> pathList;

  std::string path=Path;
  // First look for the character "/":
  size_t pos = 0;
  while (1) {
    size_t end = path.find('/',pos);
    if (end==path.npos) break;

    std::string sub0(path,pos,end-pos);
    pathList.push_back(sub0);
    std::string sub1(path,end+1,path.npos-end-1);
    path=sub1;
    pos=0;
  }
  // Then look for wild cards:
  m_c->pathList.push_back(pathList);

  std::string regex="^";
  //size_t pos=0;
  while (1) {
    size_t end = path.find('*',pos);
    std::string sub(path,pos,end-pos);
    regex+=sub;
    if (end==path.npos) break;
    regex+=".*";
    pos=end+1;
    if (pos==path.npos) break;
  }
  regex+="$";
  m_c->geometryRegex.push_back(regex);
  regex_t *preg = new regex_t;
  regcomp(preg,regex.c_str(),REG_EXTENDED);
  m_c->geometryPreg.push_back(preg);

}

void GeometryMap::finalize( PVConstLink v) {

  for (size_t i=0;i<m_c->pathList.size();i++) {
    m_c->lV=0;
    m_c->finalize(i,v);
  }
}


void GeometryMap::Clockwork::finalize(unsigned int i, PVConstLink v) {

  static const size_t NMATCH=11;     
  regmatch_t pmatch[NMATCH];          

  // Something has gone wrong...
  if (lV>pathList[i].size()) {
    return;
  }
  if (lV==pathList[i].size()) {
    std::string name = v->getLogVol()->getName();
    if (regexec((geometryPreg[i]),name.c_str(), NMATCH, pmatch, 0)==0) {
      physicalVolumes.push_back(v);
    }
  }
  else {
    GeoVolumeCursor x(v,nullptr);
    while (!x.atEnd()) {
      lV++;
      finalize(i, x.getVolume());
      lV--;
      x.next();
    }
  }
}


   
    


GeometryMap::Iterator GeometryMap::begin() const {
  return m_c->physicalVolumes.begin();
}

GeometryMap::Iterator GeometryMap::end() const {
  return m_c->physicalVolumes.end();
}
