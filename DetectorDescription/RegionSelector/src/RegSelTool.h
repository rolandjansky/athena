/// emacs: this is -*- c++ -*-
///
///   @class RegSelTool RegSelTool.h
/// 
///     This is a Region Selector tool
///     
///   @author Mark Sutton
///
///   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
///

#ifndef REGIONSELECTOR_REGSELTOOL_H
#define REGIONSELECTOR_REGSELTOOL_H

// interface includes
#include "IRegionSelector/IRegSelTool.h"
#include "IRegionSelector/IRoiDescriptor.h"

// spam
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>
#include <iostream>
#include <vector>
#include <stdint.h>


class RegSelModule;
class RegSelSiLUT;
class IInterface;



class RegSelTool : public extends<AthAlgTool, IRegSelTool> {

 public:

  /** @c Standard constructor for tool ???.
   */
  RegSelTool( const std::string& type, const std::string& name, const IInterface* parent );

  //! Destructor.
  virtual ~RegSelTool() override;

  /** @c queryInterface only needed for Gaudi services ????
   */
  //  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF); // ???


  //! @method initialize, loads lookup tables for retrieve %Identifier %Hash and ROBID 
  StatusCode initialize() override;

  //! @method finalize, deletes lookup table from memory
  StatusCode finalize() override;
  

  //! @method handle, handles the actual lookup table
  bool handle(); 

  //! @method reinitialise, reinitialise everything if needed
  bool reinitialise();


  /// IRegSlTool interface ...

  // Interface inherited from IRegSelTool service

  void HashIDList( const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist );

  void HashIDList( long layer, const IRoiDescriptor& roi, std::vector<IdentifierHash>& idlist);
   
  void ROBIDList( const IRoiDescriptor& roi, std::vector<uint32_t>& roblist );

  void ROBIDList( long layer, const IRoiDescriptor& roi, std::vector<uint32_t>& roblist );

   
protected:

  // full scan
  void HashIDList( std::vector<IdentifierHash>& idlist );  

  // full scan for a specific layer
  void HashIDList( long layer, std::vector<IdentifierHash>& idlist );
     

  // Methods to obtain the rob id list

  // full scan
  void ROBIDList( std::vector<uint32_t>& roblist );

  // full scan by layer
  void ROBIDList( long layer, std::vector<uint32_t>& roblist );


  // get list of modules
  
  void getRoIData( const IRoiDescriptor& roi, std::vector<const RegSelModule*>& modulelist );

private:

  //! Flag to determine whether it has yet been initialised
  bool              m_initialised; 

  //! Flag to dump loaded table in data file.
  BooleanProperty  m_dumpTable;

  //! Actual lookup table
  RegSelSiLUT*      m_lookuptable;

};

#endif // REGIONSELECTOR_REGSELTOOL_H
