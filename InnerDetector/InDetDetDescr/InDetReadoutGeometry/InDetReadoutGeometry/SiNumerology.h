/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   SiNumerology.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef INDETREADOUTGEOMETRY_SINUMEROLOGY_H
#define INDETREADOUTGEOMETRY_SINUMEROLOGY_H

#include <vector>

namespace InDetDD {

  /** @class SiNumerology
  
     Class to extract numerology for Pixel and SCT. For example number of layers, disks, sectors, rings, etc.
     See InnerDetector/InDetExample/InDetDetDescrExample/src/SiReadSiDetectorElements.cxx for example of usage.

     @author Grant Gorfine
   */

  class SiNumerology {


    public:
    
      /** Constructor: */
      SiNumerology();
      
      // Accessors:
      
      /** Number of barrels. Normally 1. */
      int numBarrels() const; 
      
      /** Number of endcaps. Normally 2. */
      int numEndcaps() const; 
      
      /** Barrel/endcap identifier for each barrel. Normally barrelId(0) = 0 */
      int barrelId(int index) const;
      
      /** Barrel/endcap identifier for each endcap. Normally endcapId(0) returns 2, endcapId(1) returns -2 */
      int endcapId(int index) const;
      
      /** Number of layers */
      int numLayers() const; 
      
      /** Number of disks */
      int numDisks() const; 
      
      /** Number of rings (ie eta_module) in a disk */
      int numRingsForDisk(int disk) const;
      
      /** Number of sectors in phi for a layer */
      int numPhiModulesForLayer(int layer) const;
      
      /** Number of sectors in phi for a ring in a disk */
      int numPhiModulesForDiskRing(int disk, int ring) const;
      
      /** Number of sectors in phi for a ring in a disk */
      int numEtaModulesForLayer(int layer) const;
      
      /** First eta_module number for a layer */
      int beginEtaModuleForLayer(int layer) const;
      
      /** Last eta_module number + 1 */
      int endEtaModuleForLayer(int layer) const;
      
      /** Check if eta_module=0 exists */
      bool skipEtaZeroForLayer(int layer) const;
      
      // Check presence of layer/disk
      /** Check if layer exists */
      bool useLayer(int layer) const;
      
      /** Check if disk exists */
      bool useDisk(int disk) const;
      
      // Maximums 
      /** Maximum number of modules in a barrel stave */
      int maxNumBarrelEta() const;
      
      /** Maximum number of rings in a disk */
      int maxNumEndcapRings() const;
      
      /** Maximum number of strips. Same as maxNumPhiCells() */
      int maxNumStrips() const; 
      
      /** Maximum number of cells in phi direction. Same as maxNumStrips()  */
      int maxNumPhiCells() const;
      
      /** Maximum number of cells in eta direction. 0 for SCT. */
      int maxNumEtaCells() const;
      
      // Modifiers:
      void addBarrel(int id);
      void addEndcap(int id);
      void setNumLayers(int nLayers); 
      void setNumDisks(int nDisks); 
      void setNumRingsForDisk(int disk, int nRings);
      void setNumPhiModulesForLayer(int layer, int nPhiModules);
      void setNumPhiModulesForDiskRing(int disk, int ring, int nPhiModules);
      void setNumEtaModulesForLayer(int layer, int nEtaModules);
      void setMaxNumEtaCells(int cells);
      void setMaxNumPhiCells(int cells);
      
      // DBM 
      int numDisksDBM() const; 
      int numBarrelDBM() const; 
      bool useDiskDBM(int disk) const; 
      int numRingsForDiskDBM(int currentdisk) const; 
      int numPhiModulesForDiskRingDBM(int disk, int ring) const; 
      int numEndcapsDBM() const; 
      int endcapIdDBM(int index) const; 
      
      void setNumDisksDBM(int nDisks); 
      void setNumBarrelDBM(int nBarrel); 
      void setNumPhiModulesForDiskRingDBM(int disk, int ring, int nPhiModules); 
      void addEndcapDBM(int id); 

    private:
      
      int m_numLayers;
      int m_numDisks;
      int m_maxPhiCells;
      int m_maxEtaCells;
      int m_maxNumBarrelEta;
      int m_maxNumEndcapRings;
      int m_maxNumBarrelPhiModules;
      int m_maxNumEndcapPhiModules;
    
      std::vector<int> m_barrelIds;
      std::vector<int> m_endcapIds;
      std::vector<int> m_phiModulesForLayer;
      std::vector<int> m_ringsForDisk;
      std::vector<std::vector<int> > m_phiModulesForDiskRing;
      std::vector<int> m_etaModulesForLayer;

      int m_numDisksDBM; 
      int m_numBarrelDBM; 
      std::vector<int> m_endcapIdsDBM; 
      std::vector<std::vector<int> > m_phiModulesForDiskRingDBM; 
      int m_maxNumEndcapPhiModulesDBM; 
  };
  
}// End namespace

#include "SiNumerology.icc"

#endif // INDETREADOUTGEOMETRY_SINUMEROLOGY_H
