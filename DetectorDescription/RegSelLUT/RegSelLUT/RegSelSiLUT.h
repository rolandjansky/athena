// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelSiLUT.h  
 **
 **   Description:  RegionSelector LUT class for the Si Inner Detector  
 **                   
 **                   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Wed Apr  4 16:03:52 BST 2007
 **   Modified:     Sun Oct 14 09:49:34 BST 2007 add copy constructors, 
 **                                              code to remove robs and 
 **                                              modules
 **
 **************************************************************************/ 


#ifndef __REGSELSILUT_H
#define __REGSELSILUT_H


#include <stdint.h> 

#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <algorithm>

#include "RegSelLUT/RegSelRoI.h"
#include "RegSelLUT/RegSelName.h"
#include "RegSelLUT/RegSelModule.h"
#include "RegSelLUT/RegSelSubDetector.h"

#include "Identifier/IdentifierHash.h"




class RegSelSiLUT : public RegSelName {

public:
  typedef enum { UNDEF, PIXEL, SCT, TRT, FTK } DET;
  
  RegSelSiLUT() : m_ID(UNDEF) { } 
  RegSelSiLUT(DET id);
  RegSelSiLUT(const RegSelSiLUT& r);
  RegSelSiLUT(const std::string& s);


  virtual ~RegSelSiLUT() { } 

  void addModule(RegSelModule& module)           {  m_modules.push_back(module); }
  void addModules(const std::vector<RegSelModule>& m) {  m_modules = m; }  

  // Access the lookup table for data taking 
  const std::vector<RegSelModule>& getModules() const { return m_modules; } 
  bool getRoIData(const RegSelRoI& r, std::vector<const RegSelModule*>& modules) const;
  bool getRoIData(const RegSelRoI& r, unsigned layer, std::vector<const RegSelModule*>& modules) const;
  bool getRoIData(const RegSelRoI& r, unsigned subdetector, unsigned layer, std::vector<const RegSelModule*>& modules) const;

  bool getRoIData(const RegSelRoI& r, std::vector<const RegSelModule*>& modules, double x, double y) const;
  bool getRoIData(const RegSelRoI& r, unsigned subdetector, unsigned layer, std::vector<const RegSelModule*>& modules, double x, double y) const;


  DET ID() const { return m_ID; } 

  // reset (empty) the lookup table, back to 
  // main module vector
  void reset();

  // initialise (fill) the lookup tables from 
  // the module vector
  void initialise();

  // and for the disabled modules
  void initialiseDisabledModules();

  // and for the full scan lists
  void initialiseFullScan();

  // IO
  void write(std::ostream& s=std::cout) const;
  void write(const std::string& filename) const;

  bool read(std::istream& s);
  bool read(const std::string& s);

  // get the sub detectors
  const std::vector<RegSelSubDetector>& getSubDet() const { return  m_SubDet; }

  // get a module from it's hash id
  const RegSelModule* Module(const IdentifierHash& h) const;


  /// here if layer is of the format 1XYY where X=0 (barrel), X=2 (endcap) and YY is the layer within the sub-system
  /// then the specific layer in the sub-system is used.
  /// If it is of the form YY, then simple layer number is used
  /// NB: ALL the layer IdentifierHash and uint32_t retreival methods use this
  ///     format
  void getModules( unsigned layer, std::vector<const RegSelModule*>& modules) const;



  // roi access methods

  // access functions to get the list of hash id's 
  virtual void  getHashList(const RegSelRoI& roi, std::vector<IdentifierHash>& hashlist ) const;

  // access functions to get the list of hash id's 
  virtual void  getHashList(const RegSelRoI& roi, std::vector<IdentifierHash>& hashlist, double x, double y) const;

  // roi layer methods
  // get the hash id's for a specified layer - see comment above about the layer format
  virtual void  getHashList(const RegSelRoI& roi, unsigned layer, std::vector<IdentifierHash>& hashlist) const;

  // get the hash id's for the entire detector
  virtual void  getHashList(std::vector<IdentifierHash>& hashlist) const;

  // get the hash id's for a complete layer - see comment above about the layer format
  virtual void  getHashList(unsigned layer, std::vector<IdentifierHash>& hashlist) const;
  //  void  getHashList(long int& layer, std::vector<IdentifierHash>& hashlist ) const { return getHashList( unsigned(layer), hashlist ); }




  // get the rob id's for a specified layer - see comment above about the layer format
  virtual void  getRobList(const RegSelRoI& roi, unsigned layer, std::vector<uint32_t>& roblist, bool removeduplicates=true ) const;

  // access functions to get the roblist as wanted by athena
  virtual void  getRobList(const RegSelRoI& roi, std::vector<uint32_t>& roblist, bool removeduplicates=true ) const;

  // with beam line position
  // access functions to get the roblist as wanted by athena
  virtual void  getRobList(const RegSelRoI& roi, std::vector<uint32_t>& roblist, double x, double y ) const;

  // full scan methods
  // get the rob list for the entire detector
  virtual void  getRobList(std::vector<uint32_t>& roblist) const;

  // get the rob list for a complete layer - see comment above about the layer format
  virtual void  getRobList(unsigned layer, std::vector<uint32_t>& roblist) const;
  //  void  getRobList(long int& layer, std::vector<uint32_t>& roblist) const { return getRobList( unsigned(layer), roblist ); } 

  



  // get the hash id's and robs for the entire detector
  virtual void  getRobHashList(std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const;

  // get the rob list and hash id's for an roi
  virtual void  getRobHashList(const RegSelRoI& roi, std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const;

  // get the rob list and hash id's for an roi - see comment above about the layer format
  virtual void  getRobHashList(const RegSelRoI& roi, unsigned layer, std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const;





  // disabled module access

  // get the hash id's of *disabled* modules in a specified layer 
  void  getDisabledHashList(const RegSelRoI& roi, unsigned layer, std::vector<IdentifierHash>& hashlist) const;

  // get get a list of layers with disabled modules from the specified sub-detector component
  void  getDisabledLayers(const RegSelRoI& roi, int subdetector, std::vector<int>& layers) const;


  // enabling and disabling
  // enable or disable all the modules in the lookup table
  void  enableRobs();
  void disableRobs();

  // enable or disable all the modules in the lookup table
  void  enableModules() {  enableRobs(); } 
  void disableModules() {  disableRobs(); } 

  // enable or disable the modules corresponding to the robs in the roblist
  void  enableRobList(const std::vector<uint32_t>& roblist);
  void disableRobList(const std::vector<uint32_t>& roblist);

  // enable or disable all modules from a list 
  void  enableModuleList(const std::vector<IdentifierHash>& hashlist);
  void disableModuleList(const std::vector<IdentifierHash>& hashlist);

  // enable/disable individual subdetectors 
  void disableSubDetector(int id);
  void enableSubDetector(int id);

  // enable/disable individual layers 
  void disableLayer(int id, unsigned layer);
  void enableLayer(int id, unsigned layer);

  // draw the object (??)
  void drawlayers(std::vector<const ZRObject*>& c) const { 
    for ( unsigned i=0 ; i<m_SubDet.size() ; i++ )  m_SubDet[i].drawlayers(c);
  }

public:

  /// useful for removing duplicates if required ...
  template<typename T>
  static void removeDuplicates(std::vector<T>& vec) {
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
  }
  
protected:

  virtual void construct();

protected:

  DET        m_ID;   // pixel (0), sct (1) or trt (2)
     
  std::vector<RegSelSubDetector>   m_SubDet;  // subdetectors
  std::vector<RegSelModule>        m_modules; // actual module store 

  std::vector<RegSelModule*>       m_disabled; // disabled module list
 
  std::map<const IdentifierHash, const RegSelModule*> m_map; // hashID lookup table

  // quick look ups for entire detector
  std::vector<IdentifierHash> m_allHashes; // hashID lookup table
  std::vector<uint32_t>       m_allROBids;

  std::map<int, int> m_idmap; // hashID lookup table

};



#endif  /* __REGSELSILUT_H */










