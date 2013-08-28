/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IRegionSelectorTool.h
//   Header file for class IRegionSelectorTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Niels.Van.Eldik@cern.ch, 
///////////////////////////////////////////////////////////////////


#ifndef MUONCALIB_IREGIONSELECTORTOOL_H
#define MUONCALIB_IREGIONSELECTORTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "MuonCalibIdentifier/MuonFixedId.h"

#include <vector>

namespace MuonCalib {

  /** Interface ID for IRegionSelectorTool */  
  static const InterfaceID IID_IRegionSelectorTool("MuonCalib::IRegionSelectorTool", 1, 0);
  
  /**@class IRegionSelectorTool
     Interface class IRegionSelectorTool.
     Implementations are used to define the calibration regions within 
     atlas. They should generate a unique key for every region.
  
     @author Niels.Van.Eldik@cern.ch, 
  */

  class MuonCalibSegment;

  class IRegionSelectorTool : virtual public IAlgTool {
  public:
    
    typedef MuonFixedId  id_type;                                //!< typedef for a MuonFixedId
    typedef std::string RegionKey;                               //!< typedef for a std::string used as region key
    typedef std::pair<const MuonCalibSegment*,RegionKey> Region; //!< typedef of a pair of a MuonCalibSegment and a RegionKey
    typedef std::vector<Region> RegionVec;                       //!< typedef of a set of Region (pairs)
  public:
    IRegionSelectorTool() : m_notUsed("notUsed") {};             //!< IAlgTool default constructor
    virtual ~IRegionSelectorTool(){};                            //!< pure virtual IAlgTool destructor
  
    static const InterfaceID& interfaceID() { return IID_IRegionSelectorTool; };  //!< retrieve Interface ID
  
    virtual RegionKey getRegionKey(const id_type& id) const = 0;                    //!< pure virtual IAlgTool interface: returns region key for the given id_type 
        virtual RegionVec splitIntoRegions( const MuonCalibSegment* seg) const = 0; //!< pure virtual IAlgTool interface: divide MdtSegment into subsegments if they belong to differnt region and generates a key for all of them 
	

	virtual bool useRegion( const RegionKey& key ) const { return key != m_notUsed; } //!< virtual IAlgTool interface: returns true if the generated key corresponds to a region that should be used in calibration.  
	virtual RegionKey notUsed() const { return m_notUsed; }                           //!< virtual IAlgTool interface: returns the notUsed key 
    
  private:
	RegionKey m_notUsed;  //!< flag for regions that are not to be calibrated 
  };

}

#endif //
