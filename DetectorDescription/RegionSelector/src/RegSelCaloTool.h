/// emacs: this is -*- c++ -*-
///
///   @class RegSelCaloTool RegSelCaloToolCalo.h
/// 
///          This is the Region Selector tool for the ID And muon spectrometer 
///          tables
///     
///   @author Mark Sutton
///
///   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
///

#ifndef REGIONSELECTOR_REGSELCALOTOOL_H
#define REGIONSELECTOR_REGSELCALOTOOL_H

// interface includes
#include "IRegionSelector/IRegSelTool.h"
#include "IRegionSelector/IRoiDescriptor.h"

// spam
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>
#include <iostream>
#include <vector>
#include <cstdint>

#include "IRegionSelector/RegSelCaloLUTCondData.h"



class RegSelCaloTool : public extends<AthAlgTool, IRegSelTool> {

public:

  /** @c Standard constructor for tool (obviously).
   */
  RegSelCaloTool( const std::string& type, const std::string& name, const IInterface* parent );

  //! Destructor.
  virtual ~RegSelCaloTool() override;


  //! @method initialize, loads lookup tables for retrieve %Identifier %Hash and ROBID 
  virtual StatusCode initialize() override;

  //! @method finalize, deletes lookup table from memory
  virtual StatusCode finalize() override;
  
  /// IRegSlTool interface ...

  // Interface inherited from IRegSelCaloTool service

  void HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist ) const override;

  void HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist) const override;

  void ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const override;

  void ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist ) const override;

   
protected:

  // full scan
  void HashIDList( std::vector<IdentifierHash>& idlist ) const;  

  // full scan for a specific layer
  void HashIDList( long layer, std::vector<IdentifierHash>& idlist ) const;
     
  // Methods to obtain the rob id list

  // full scan
  void ROBIDList( std::vector<uint32_t>& roblist ) const;

  // full scan by layer
  void ROBIDList( long layer, std::vector<uint32_t>& roblist ) const;

  //! @method lookup, actually retrieve the lookup table as conditions data - might combine with handle()
  const RegSelectorHashMap* lookup() const;

private:

  //! Flag to determine whether it has yet been initialised
  bool              m_initialised; 

  //! Flag to dump loaded table in data file.
  BooleanProperty  m_dumpTable;

  SG::ReadCondHandleKey<RegSelCaloLUTCondData> m_tableKey{ this, "RegSelLUT", "Tool_Not_Initalised", "Region Selector lookup table" };

};

#endif // REGIONSELECTOR_REGSELTOOL_H
