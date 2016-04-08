/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOMAPPINGTOOLS_PPMMAPPINGTOOL_H
#define TRIGT1CALOMAPPINGTOOLS_PPMMAPPINGTOOL_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"

#include "ChannelCoordinate.h"

#include "TrigT1CaloMappingToolInterfaces/IL1CaloMappingTool.h"

class IInterface;
class StatusCode;

namespace LVL1 {


/** PPM crate/module/channel to eta/phi/layer mappings
 *
 *  The inputs and crate layouts come from "Level-1 Calorimeter Trigger:
 *  Cable Mappings and Crate Layouts from Analogue Inputs to Processors"
 *  version 1.6.
 *  The input-output mapping comes from Steve Hillier's online decoder.
 *
 *  @author Peter Faulkner
 */

class PpmMappingTool : virtual public IL1CaloMappingTool,
                               public AthAlgTool {

 public:

   PpmMappingTool(const std::string& type, const std::string& name,
                                           const IInterface* parent);
   virtual ~PpmMappingTool();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Return eta, phi and layer mapping for given crate/module/channel
   virtual bool mapping(int crate, int module, int channel,
                        double& eta, double& phi, int& layer);
   /// Return crate, module and channel mapping for given eta/phi/layer
   virtual bool mapping(double eta, double phi, int layer,
                        int& crate, int& module, int& channel);

 private:
   //  Crate/module map constituents
   typedef std::pair< double, double >                Offsets;
   typedef std::map< int, ChannelCoordinate >         CoordinateMap;
   typedef std::pair< Offsets, const CoordinateMap* > ModuleInfo;
   typedef std::map< int, ModuleInfo >                ModuleMap;
   typedef std::map< int, ModuleMap >                 CrateMap;
   //  Inverse map constituents
   typedef std::pair< unsigned int, unsigned int >    ChannelIds;
   typedef std::map< unsigned int, ChannelIds >       EtaPhiMap;

   /// Set up crate/module map
   void setupMap();
   /// Set up eta/phi map
   void setupInverseMap();
   /// Add entries to a coordinate map
   void addCoords(int nrows, int ncols, double etaGran, double phiGran,
        double etaOffset, double phiOffset,
	const int* in, const int* out, int incr,
	ChannelCoordinate::CaloLayer layer, CoordinateMap* coordMap);
   /// Add a block of similar modules to a crate
   void addMods(int crate, int modOffset, int nrows, int ncols,
        double etaBase, double phiBase, double etaRange, double phiRange,
	const CoordinateMap* coordMap);
   /// Correction for Had FCAL eta which is adjusted to EM value in TriggerTower
   double etaSim(const ChannelCoordinate& coord) const;
   /// Simple eta/phi key
   unsigned int etaPhiKey(double eta, double phi) const;

   /// Pointer to crate/module map
   CrateMap* m_crateInfo;
   /// Vector of CoordinateMaps
   std::vector<CoordinateMap*> m_coordMaps;
   /// Current Coordinate map
   mutable const CoordinateMap* m_currentMap;
   /// Current module eta offset
   mutable double m_etaOffset;
   /// Current module phi offset
   mutable double m_phiOffset;
   /// Current crate
   mutable int m_currentCrate;
   /// Current module
   mutable int m_currentModule;
   /// Pointer to inverse map
   EtaPhiMap* m_etaPhiMap;

   static const int s_crates   = 8;
   static const int s_modules  = 16;
   static const int s_channels = 64;

};

} // end namespace

#endif
