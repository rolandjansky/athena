/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// DigitizationCell.h, ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDIGEVENT_DIGITIZATIONCELL_H
#define TRKDIGEVENT_DIGITIZATIONCELL_H

// GeoPrimitives
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
    
    /** @typedef of a pair of ints for definition a cell */
    typedef std::pair<size_t,size_t> DigitizationCell;

    /** @struct DigitizationStep for further handling */
    struct DigitizationStep {
      
         double            stepLength;           //!< this is the path length within the cell
         double            driftLength;          //!< this is the path length of the setp center to the readout surface
         DigitizationCell  stepCell;             //!< this is the cell identifier of the segment
         Amg::Vector3D     stepEntry;            //!< this is the Entry point into the segment
         Amg::Vector3D     stepExit;             //!< this is the Exit point from the segment
         Amg::Vector2D     stepReadoutProjected; //!< this is the projected position at the readout surface
         Amg::Vector2D     stepCellCenter;       //!< this is the cell position
        
         /** Standard constructor */
         DigitizationStep() :
           stepLength(0.),         
           driftLength(0.),      
           stepCell(std::pair<size_t,size_t>(0,0)),            
           stepEntry(0.,0.,0.),          
           stepExit(0.,0.,0.),          
           stepReadoutProjected(0.,0.),
           stepCellCenter(0.,0.)
        {}
            
        /** Constructor with arguments */
        DigitizationStep(double sl, double dl, 
                         DigitizationCell dc, 
                         const Amg::Vector3D& entryP, 
                         const Amg::Vector3D& exitP,
                         const Amg::Vector2D& projectedPosition,
                         const Amg::Vector2D& cellPosition) :
          stepLength(sl),         
          driftLength(dl),      
          stepCell(dc),            
          stepEntry(entryP),          
          stepExit(exitP),          
          stepReadoutProjected(projectedPosition),
          stepCellCenter(cellPosition)
        {}
                 
            
    };

}

#endif    
    
