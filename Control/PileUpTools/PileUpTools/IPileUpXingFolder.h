/* -*- C++ -*- */

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_IPILEUPXINGFOLDER_H
#define PILEUPTOOLS_IPILEUPXINGFOLDER_H 1
/** @file IPileUpXingFolder.h
 * @brief an interface to SG::Folder with an attached bunch crossing interval
 * @author pcalafiura@lbl.gov - ATLAS Collaboration
 * $Id: IPileUpXingFolder.h,v 1.1 2007-06-22 23:01:46 calaf Exp $
 **/

#include "GaudiKernel/IAlgTool.h"
#include "SGTools/SGIFolder.h" //iterator tdef

/** @class IPileUpXingFolder
 * @brief an interface to SG::Folder with an attached bunch crossing interval
 **/
class IPileUpXingFolder : virtual public IAlgTool {
public:
  /// @name No data should be returned for xings outside this range of xings
  //@{
  virtual int firstXing() const=0;
  virtual int lastXing() const=0;
  //@}
  /// controls the rate at which we clear data objs in this folder at the end of  event
  virtual float cacheRefreshFrequency() const=0;

  typedef SG::IFolder::const_iterator const_iterator;
  /// @name access folder items
  //@{
  virtual const_iterator begin() const=0;
  virtual const_iterator end() const=0;
  //@}

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IPileUpXingFolder, 1, 0 );
};

#endif // PILEUPTOOLS_IPILEUPXINGFOLDER_H
