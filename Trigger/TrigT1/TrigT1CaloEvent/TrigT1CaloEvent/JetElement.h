
//***************************************************************************
//                           JetElement.h  -  description
//                              -------------------
//     begin                : Mon Sep 25 2000
//     copyright            : (C) 2000 by Edward Moyse
//     email                : e.moyse@qmw.ac.uk
//  ***************************************************************************/
  
//***************************************************************************
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  ***************************************************************************/
  
#ifndef JetElement_H
#define JetElement_H

#include "CLIDSvc/CLASS_DEF.h"


#include <vector>
#include <iostream>
#ifndef  TRIGGERSPACE
#include "TrigT1Interfaces/Coordinate.h"
#else
#include "Coordinate.h"
#endif 

namespace LVL1 {
  
   //Doxygen class description below:
   /** 
   Jet elements are the inputs to the Jet, ETmiss and ETsum triggers.

   They are formed in two stages:
     * 2x2 Trigger Towers (EM or Had) are summed in the PreProcessor
     * The corresponding EM and Had towers are summed in the Jet/Energy Module
   In the simulation, both of these stages are performed in the 
   TrigT1CaloSim::JetElementMaker algorithm.
    
   As for most L1Calo outputs, the hardware can read out a variable number
   of slices of data. Hence JetElement stores its internal data in vectors,
   and can return an individal slice of data or the entire vector.
   */
    class JetElement  {
    public:
     
    /** Default constructor */ 
    JetElement();
    /** Single slice constructor (some simulation tasks, legacy really) */
    JetElement(double phi, double eta, int emEnergy, int hadEnergy,
               unsigned int key, int emError, int hadError, int linkError);
    /** Multi-slice constructor, e.g. for bytestream decoding */
    JetElement(double phi, double eta, const std::vector<int>& emEnergy,
               const std::vector<int>& hadEnergy, unsigned int key,
	       const std::vector<int>& emError,
	       const std::vector<int>& hadError,
	       const std::vector<int>& linkError, int peak);
  
    /** Destructor */
    virtual ~JetElement();

    /** Add ET to triggered time slice */
    void addEnergy(int emEnergy, int hadEnergy);
    /* Add ET values to specified slice */
    void addSlice(int slice, int emEnergy, int hadEnergy,
                             int emError, int hadError, int linkError);
    
    /** Eta of centre of JetElement */
    double eta() const;
    /** Phi of centre of JetElement */
    double phi() const;

/** Return ET for peak slice */   
    int emEnergy()  const;
    int hadEnergy() const;
    int energy()    const;

/** return the ET values for a specified slice */
    int emSliceEnergy(int slice) const;
    int hadSliceEnergy(int slice) const;
    int sliceEnergy(int slice) const;

/** Error codes and utility information */
    unsigned int key() const;
    bool isSaturated()    const;
    bool isEmSaturated()  const;
    bool isHadSaturated() const;
    int emError()   const;
    int hadError()  const;
    int linkError() const;
    int peak()      const;

/** Return data for all slices */
    const std::vector<int>& emEnergyVec()  const;
    const std::vector<int>& hadEnergyVec() const;
    const std::vector<int>& emErrorVec()   const;
    const std::vector<int>& hadErrorVec()  const;
    const std::vector<int>& linkErrorVec() const;
 
    /** return coord of JE */
    Coordinate coord() const;
  
    /** Internal data */
    private:
    double m_phi;
    double m_eta;
    unsigned int m_key;
    int    m_peak;
    std::vector<int> m_emEnergy;
    std::vector<int> m_hadEnergy;
    std::vector<int> m_em_error;
    std::vector<int> m_had_error;
    std::vector<int> m_link_error;
  
    static const int m_saturationThreshold = 1023;
    static const int m_layerSaturationThreshold = 511;
    
    };

    typedef std::map<int, JetElement*> JetElementMap_t;
  } // end of namespace

#ifndef JetElement_ClassDEF_H
#include "TrigT1CaloEvent/JetElement_ClassDEF.h"
#endif

#endif
