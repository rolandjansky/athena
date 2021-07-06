/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_PILEUPXINGFOLDER_H
#define PILEUPTOOLS_PILEUPXINGFOLDER_H 1
/** @file PileUpXingFolder.h
  * @brief specify time interval we want to study pileup for a set of data objs
  * @author pcalafiura@lbl.gov - ATLAS Collaboration
  * $Id: 
  **/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "PileUpTools/IPileUpXingFolder.h"

/** @class PileUpXingFolder
 * @brief specify time interval we want to study pileup for a set of data objs
 * @details Typically this will be the sensitive time of a detector read-out
 system. Implemented as SG::Folder with an attached bunch crossing interval
**/
class PileUpXingFolder : public extends<AthAlgTool, IPileUpXingFolder> {
public:
  /// \name structors and AlgTool implementation
  //@{
  PileUpXingFolder(const std::string& type, 
		   const std::string& name,
		   const IInterface* parent);
  virtual StatusCode initialize();
  //@}
  
  typedef IPileUpXingFolder::const_iterator const_iterator;
  /// \name access folder items
  //@{
  virtual const_iterator begin() const { return m_folder->begin(); }
  virtual const_iterator end() const { return m_folder->end(); }
  //@}
//   ///add a data object identifier to the list
//   virtual StatusCode add(const std::string& typeName, const std::string& skey) {
//     return m_folder->add(typeName, skey);
//   }
//   ///add a data object identifier to the list
//   virtual StatusCode add(const CLID& clid, const std::string& skey) {
//     return m_folder->add(clid, skey);
//   }
  
  /// @name define and access xing range for dobjs in this folder. No data should be returned for xings outside this range
  //@{
  void setXings(int firstXing, int lastXing) {
    m_firstXing.setValue(firstXing); m_lastXing.setValue(lastXing);
  } 
  int firstXing() const { return m_firstXing.value(); }
  int lastXing() const  { return m_lastXing.value(); }
  //@}
  /// controls the rate at which we clear data objs in this folder at the end of  event
  float cacheRefreshFrequency() const {return m_cacheRefreshFrequency.value();}


  /// is iCrossing in the xing range?
  bool contains(int iCrossing) const {
    return firstXing() <= iCrossing && iCrossing <= lastXing();
  }

private:
  /// @name Properties
  //@{
  ToolHandle<SG::IFolder> m_folder;
  Gaudi::Property<int> m_firstXing;
  Gaudi::Property<int> m_lastXing;
  Gaudi::Property<std::vector<std::string>> m_itemList; ///< this must match SG::Folder::m_itemList
  ///Bound property [0.0, 1.0]: frequency with which data objs in this folder
  ///should be cleared from bkg caches at end of event. Default 0 (never clear)
  Gaudi::CheckedProperty<double> m_cacheRefreshFrequency;
  //@}
};

#endif
