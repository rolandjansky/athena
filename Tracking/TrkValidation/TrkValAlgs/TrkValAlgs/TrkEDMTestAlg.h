/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKINGTESTEDM_H
#define TRACKINGTESTEDM_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <iostream>
#include <iomanip>

namespace Trk {
  class TrkDetElementBase;
}

class IdentifierHash;


class TrkEDMTestAlg : public AthAlgorithm
{
public:
  TrkEDMTestAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrkEDMTestAlg();
  StatusCode	initialize();
  StatusCode	execute();
  StatusCode	finalize();

protected:
  virtual StatusCode runTest() = 0;

  /** load container of objects, and dump each contained object to MsgStream, and or external file (depending on jobOpts)*/
  template <class T>
  void loadAndDumpContainer(const std::string& collectionName, std::ofstream& stream);

  template <class T>
  void loadAndDumpIdentifiableContainer(const std::string& collectionName, std::ofstream& stream);

  /** load container of objects, and dump each contained object to MsgStream, and or external file (depending on jobOpts)*/
  template <class T>
  void loadAndDumpMultipleContainers(std::vector<std::string>& collectionNames, std::ofstream& stream);

  template <class T>
  unsigned int dumpAndTestElements (const T* container, std::ofstream& stream);

  template <class COLLECTION> 
  void checkPRDCollectionValidity(const COLLECTION& coll);

  template < class PRD, class IDHELPER, class COLLECTION >
  void checkPRD(const PRD* prd, const IDHELPER* helper, const COLLECTION& coll);

  template <class PRD>
  std::pair<const Trk::TrkDetElementBase*, const Trk::TrkDetElementBase*> getReadoutElements(const PRD* prd) const;

  template <class IDHELPER, class PRD>
  bool testDetectorElement(IdentifierHash idhash, const IDHELPER* helper, const PRD* prd) const;

  /** numbers of event*/
  unsigned int  m_eventNum;
 
  bool m_dumpToMsg;
  bool m_dumpToFile;
  bool m_dumpSummaryToFile;

  std::string m_summaryDumpFileName;
  std::ofstream  m_summaryFileOutput; 

  bool m_doDetailedChecks; //!< If true, try to examine the PRDs etc in detail and catch any errors. Will take more time.
  unsigned int m_numDetailedWarnings; //!< Count of detailed warning found.
    
  /*Count of number of times successfully retrieved. 1st is SG key, 2nd is count.*/
  std::map<std::string, unsigned int> m_numContsFound;

  /*Count of number of component objects. 1st is SG key, 2nd is count.*/
  std::map<std::string, unsigned int> m_numConstObjsFound;

};


template <class T> 
void
TrkEDMTestAlg::loadAndDumpIdentifiableContainer( const std::string& containerName, std::ofstream& stream)
{
  using namespace std;    

  const T* container;
  if (!evtStore()->contains<T>(containerName)) return;
  StatusCode sc = evtStore()->retrieve(container, containerName);
  if (sc.isSuccess())
    {
      typename T::const_iterator::const_iterator element     = container->begin(); 
      typename T::const_iterator::const_iterator lastElement = container->end();
      if (m_dumpToFile)
	stream  <<"Found container "<<containerName<<", which has "<<container->numberOfCollections()<<" collections (see below)."<<std::endl;
      if (m_dumpToMsg) 
	ATH_MSG_VERBOSE("Found container "<<containerName<<", which has "<<container->numberOfCollections()<<" collections (see below).");   

      m_numContsFound[containerName]++; // increment counter.

      unsigned int totalConsituents = dumpAndTestElements(container, stream);

      if (m_dumpToFile)
	stream<<"Found "<<totalConsituents<<" constituent objects."<<std::endl;
      if (m_dumpToMsg) 
	ATH_MSG_VERBOSE("Found "<<totalConsituents<<" constituent objects.");
      m_numConstObjsFound[containerName]+=totalConsituents; // increment counter.
    }else{
    if (m_dumpToFile)
      stream<<"WARNING! Could not find container: "<<containerName<<"!"<<std::endl;
    if (m_dumpToMsg) 
      ATH_MSG_WARNING("Could not find container: "<<containerName<<"!");
  }

  return;        
}    

template <class T> 
void
TrkEDMTestAlg::loadAndDumpContainer( const std::string& containerName, std::ofstream& stream)
{
  using namespace std;    
  const T* container;
  if (!evtStore()->contains<T>(containerName)) return;
  if (evtStore()->retrieve(container, containerName).isSuccess())    {
    if (m_dumpToFile)
      stream  <<"Found container "<<containerName<<", which has "<<container->size()
	      <<" elements (see below)."<<std::endl;
    if (m_dumpToMsg) 
      ATH_MSG_VERBOSE("Found container "<<containerName<<", which has "<<container->size()
		      <<" elements (see below).");
  
    m_numContsFound[containerName]++; // increment counter.
    m_numConstObjsFound[containerName]+=container->size(); // increment counter.
    unsigned int totalConsituents = dumpAndTestElements(container, stream);
    if (m_dumpToFile)
      stream<<"Found "<<totalConsituents<<" constituent objects. (size was reported as "<<container->size()<<")"<<std::endl;
    if (m_dumpToMsg) 
      ATH_MSG_VERBOSE("Found "<<totalConsituents<<" constituent objects.(size was reported as "<<container->size()<<")");
  }else{
    if (m_dumpToFile)
      stream<<"WARNING! Could not find container: "<<containerName<<"!"<<std::endl;
    if (m_dumpToMsg) 
      ATH_MSG_WARNING("Could not find container: "<<containerName<<"!");
  }
  return;        
}    


template <class T> 
void
TrkEDMTestAlg::loadAndDumpMultipleContainers( std::vector<std::string>& containerNames, std::ofstream& stream)
{
  using namespace std;    
  //loads tracks
  for (vector<string>::const_iterator it=containerNames.begin(); it!=containerNames.end() ; ++it)
    {
      const std::string& containerLoc = *it;
      loadAndDumpContainer<T>( containerLoc, stream );
    }
  return;
}    

template <class T>
unsigned int
TrkEDMTestAlg::dumpAndTestElements (const T* container, std::ofstream& stream) 
{ 
  unsigned int numConsituents=0;

  typename T::const_iterator element     = container->begin(); 
  typename T::const_iterator lastElement = container->end();

  for ( ; element!=lastElement ; ++element) {
    if (*element!=0){
      ++numConsituents;
      // dumping collections
      if (m_dumpToFile)
	stream << std::setprecision(6) << **element<<std::endl;
      if (m_dumpToMsg) 
	ATH_MSG_VERBOSE(**element);
    }else{
      if (m_dumpToFile)
	stream<<"WARNING! Zero pointer!"<<std::endl;
      if (m_dumpToMsg) 
	ATH_MSG_WARNING("Zero pointer!");
    }    
  }
  return numConsituents;
}



#endif 
