///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IItemListSvc.h 
// Header file for class IItemListSvc
// Author: S.Binet<binet@cern.ch>
//         B.Radics<radbal@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENAKERNEL_IITEMLISTSVC_H 
#define ATHENAKERNEL_IITEMLISTSVC_H 1

/**
 * @class IItemListSvc
 * @brief This class defines a protocol to register boolean decisions and 
 * @brief and retrieve them and their combined result 
 */

// STL includes
#include <vector>
#include <string>
#include <map>
#include <mutex>

// fwd declares
class INamedInterface;


class IItemListSvc  
  : virtual public ::INamedInterface
{ 

public:

  virtual ~IItemListSvc();

  /////////////////////////////////////////////////////////////////// 
  // Public interface methods
  // non-const
  //
  // add a stream-item pair to the service listing
  virtual StatusCode addStreamItem(std::string stream, std::string itemname) = 0;
  // remove Item 
  virtual StatusCode removeStreamItem(std::string stream, std::string itemname) = 0;
  //
  // const
  //
  // check if a stream-item is registered
  //virtual bool containsItem(const std::string itemname) const = 0;
  virtual bool containsItem(const std::string itemname, const std::string stream="ANY") const = 0;
  // check how many streams contain an item
  //virtual long countItemInstances(const std::string itemname) const = 0;
  // get the streams for a given item
  virtual std::vector<std::string> getStreamsForItem(const std::string itemname) const = 0;
  // get the items for a given stream
  virtual std::vector<std::string> getItemsForStream(const std::string stream) const = 0;

  // get mutex for streaming itemlist to output
  virtual std::mutex& streamMutex();


public:

  static const InterfaceID& interfaceID();
  
private:

  std::mutex m_stream_mut;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
const InterfaceID& 
IItemListSvc::interfaceID() 
{ 
  static const InterfaceID IID_IItemListSvc("IItemListSvc", 1, 0);
  return IID_IItemListSvc; 
}

inline
std::mutex&
IItemListSvc::streamMutex()
{
  return m_stream_mut;
}

#endif //> !ATHENAKERNEL_IITEMLISTSVC_H

