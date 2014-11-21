///////////////////////////////////////////////////////////////////
// L1CPMTools.h, (c) Alan Watson
///////////////////////////////////////////////////////////////////

 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/

#ifndef ILVL1L1CPMTOOLS_H
#define ILVL1L1CPMTOOLS_H

#include "GaudiKernel/IAlgTool.h"
#include "DataModel/DataVector.h"

namespace LVL1 
{

class CPMTobAlgorithm;
class CPMTobRoI;
class CPMTower;

  /** Interface definition for L1CPMTools

      @author  Alan Watson <Alan.Watson@cern.ch>
  */  

  static const InterfaceID IID_IL1CPMTools("LVL1::IL1CPMTools", 1, 0);

  class IL1CPMTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
      virtual void mapTowers(const DataVector<CPMTower>* cpmts, std::map<int, CPMTower*>* towers) = 0;
      
      virtual void findRoIs(const std::map<int, CPMTower*>* towers, DataVector<CPMTobAlgorithm>* tobs, int slice = -1) = 0;
      
      virtual void findRoIs(const DataVector<CPMTower>* cpmts, DataVector<CPMTobAlgorithm>* tobs, int slice = -1) = 0;
     
      virtual void findCPMTobRoIs(const std::map<int, CPMTower*>* towers, DataVector<CPMTobRoI>* rois, int slice = -1) = 0;
     
      virtual void findCPMTobRoIs(const DataVector<CPMTower>* cpmts, DataVector<CPMTobRoI>* rois, int slice = -1) = 0;
      
      virtual void findCPMResults(const std::map<int, CPMTower*>* towers, int crate, int module,
                                  DataVector<CPMTobRoI>* rois, std::vector<unsigned int>& emCMXData,
                                  std::vector<unsigned int>& tauCMXData, int slice = -1) = 0;
      
      virtual CPMTobAlgorithm findRoI(double RoIeta, double RoIphi, const std::map<int, CPMTower*>* towers, int slice = -1)  = 0;
      
      virtual void formSums(double RoIeta, double RoIphi, const std::map<int, CPMTower*>* towers, int slice = -1)  = 0;
      
      virtual void formSums(uint32_t roiWord, const std::map<int, CPMTower*>* towers, int slice = -1)  = 0;
      
      virtual int Core() const = 0;
      
      virtual int EMCore() const = 0;
      
      virtual int HadCore() const = 0;
      
      virtual int EMClus() const = 0;
      
      virtual int TauClus() const = 0;
      
      virtual int EMIsol() const = 0;
      
      virtual int HadIsol() const = 0;
      
      virtual bool isEtMax() const = 0;
      
      virtual bool isEMRoI() const = 0;
      
      virtual bool isTauRoI() const = 0;
      
      virtual unsigned int EMIsolWord() const = 0;
      
      virtual unsigned int TauIsolWord() const = 0;
            
    };

    inline const InterfaceID& LVL1::IL1CPMTools::interfaceID()
    { 
      return IID_IL1CPMTools;
    }
       
} 

#endif 
