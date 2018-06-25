/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BinMap.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINMAP_H
#define TRKDETDESCRUTILS_BINMAP_H

#include "TrkDetDescrUtils/BinningData.h"
//STL
#include <iostream>
#include <vector>

class MsgStream;

namespace Trk {

/** @class BinMap

   Mapping of path through Binned Array /

   @author sarka.todorova@cern.ch

   */

  template <class T> struct BinMap  {

    std::vector<const T*> objects;
    BinPath               map;

    BinMap<T>(std::vector<const T*>& obj, BinPath intersections):
    objects(obj),map(intersections)
    {}
 
    // constructor with bounded values
    BinMap<T>(float min, float max):
    objects(),map()
    {
      map.add(BinIntersection(min,-1));
      map.add(BinIntersection(max,-1));
      objects.push_back(0);
      objects.push_back(0);
    }

    std::pair< BinIntersection, const T*>  currentBin() {        // entry distance to current Bin
      if (!map.steps.size()) return std::pair<BinIntersection,const T*> (BinIntersection(0.,-1),0);  
      std::vector<Trk::BinIntersection>::iterator it=map.steps.begin();
      while (it+1<map.steps.end() && (*(it+1)).first<0.01) it++; 
      return std::pair<BinIntersection,const T*>(*it,objects[it-map.steps.begin()]);       
    }

    std::pair< BinIntersection, const T*> nextBin() {           // jumping over steps below tolerance
      if (!map.steps.size()) return std::pair<BinIntersection,const T*> (BinIntersection(0.,-1),0);  
      std::vector<Trk::BinIntersection>::iterator it=map.steps.begin();
      while (it<map.steps.end() && (*it).first<0.01) it++; 
      if (it==map.steps.end()) return std::pair<BinIntersection,const T*>(map.steps.back(),0);
      return std::pair<BinIntersection,const T*>(*it,objects[it-map.steps.begin()]);       
    }

    int currentIndex() {
      if (!map.steps.size()) return -1;  
      std::vector<Trk::BinIntersection>::iterator it=map.steps.begin();
      while (it<map.steps.end() && (*(it+1)).first<0.01) it++; 
      return (*it).second;       
    }

    float entryDistance() {
      if (!map.steps.size()) return 0.;  
      std::vector<Trk::BinIntersection>::iterator it=map.steps.begin();
      while (it<map.steps.end() && (*(it+1)).first<0.01) it++; 
      return (*it).first;       
    }

    float exitDistance() {
      if (!map.steps.size()) return 0.;  
      std::vector<Trk::BinIntersection>::iterator it=map.steps.begin();
      while (it<map.steps.end() && (*(it+1)).first<0.01) it++; 
      return (*(it+1)).first;       
    }

    void mergeIn(BinMap<T>& bmap, float shift){

      std::vector<std::pair<float,int> >::iterator cIt = map.steps.begin();
      std::vector<std::pair<float,int> >::iterator nIt = bmap.map.steps.begin();

      //skip bins before lower boundary
      while ( (*nIt).first+shift+0.001 < (*cIt).first ) nIt++;

      // merge
      while (  nIt != bmap.map.steps.end() ) {

        int npos = nIt-bmap.map.steps.begin();
        int cpos = cIt-map.steps.begin();

        if ( fabs((*nIt).first+shift -(*cIt).first)<0.001 && bmap.objects[npos] ) {
	  objects[cpos] = bmap.objects[npos];
          (*cIt).second = (*nIt).second;                    // local bin numbering
          nIt++;                                            // to be used for identification after resolving the hierarchy
          if (nIt==bmap.map.steps.end()) break;
        } 

        cIt++;
        if ( cIt==map.steps.end() ) break;

        float d =  (*cIt).first;
	
	while ( (*nIt).first+shift < d )  {
          int ipos = map.add(std::pair<float,int>((*nIt).first+shift,(*nIt).second));
	  objects.insert(objects.begin()+ipos,bmap.objects[nIt-bmap.map.steps.begin()]);
          cIt = map.steps.begin()+ipos;
          nIt++; cIt++;
	  if ( nIt==bmap.map.steps.end() ) break;          
	}
      }
      
      return;     
    } 

    void check(){
      // check the sequence of intersections
      std::vector<std::pair<float,int> >::iterator cIt = map.steps.begin();
      cIt++;
      while ( cIt!=map.steps.end() ) {
        if ( (*cIt).first< (*(cIt-1)).first ) {
	  std::cout <<"PROBLEM in mapping:"<< std::endl;
	  map.dump(); 
	} else if ( fabs((*cIt).first- (*(cIt-1)).first ) <0.01 ) {
	  // clean up          
          if ((*cIt).second==(*(cIt-1)).second ) {
            objects.erase(objects.begin()+int(cIt-map.steps.begin()));
	    map.steps.erase(cIt);
            cIt = map.steps.begin();
          } else if ( (*(cIt-1)).second==-1 ) {
            objects.erase(objects.begin()+int(cIt-1-map.steps.begin()));
	    map.steps.erase(cIt-1);
            cIt = map.steps.begin();
	  }
	}
        cIt++;
      }
      return;
    }

  };

} // end of namespace


#endif
