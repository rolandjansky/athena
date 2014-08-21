/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/************************************************************************
                    ICaloParticleIDTool.h - Copyright ATLAS Collaboration

author: Doug Schouten <dschoute at sfu dot ca>

**************************************************************************/

#ifndef ICALOPARTICLEIDTOOL_H
#define ICALOPARTICLEIDTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"

#include <map>
#include <vector>
#include <string>

namespace HepMC 
{ 
  class GenParticle; 
}
class CaloCalibrationHitContainer;
class INavigable;

///////////////////////////////////////////////////////////////////////////////
//
// Interface to access calibration hit information, including 
// particle provenance, for (composite) calorimeter objects.
//
///////////////////////////////////////////////////////////////////////////////


static const InterfaceID IID_ICaloParticleIDTool("ICaloParticleIDTool", 1, 0);

class ICaloParticleIDTool : virtual public IAlgTool
{
public:
  static const InterfaceID& interfaceID( ) { return IID_ICaloParticleIDTool; }
  
  enum EnergyType
    {
      VISIBLE,
      VISIBLE_EM,
      VISIBLE_HAD,
      INVISIBLE,
      TOTAL
    };
  
  inline static bool idAccept( void* caller = 0x0, const Identifier& id = Identifier() ) { 
     if (caller == NULL || !id.is_valid()) return true;
     return true; 
  }
  
  // * -------------------------------------------------------------------------------- * //
  
  //
  // accessors for particle ID information
  //
  
  
  /*
    Return the sum of the energy of all hits left by a particle inside a calorimeter object.
    
    @param obj the calorimeter object for which to tabulate calibration hits
    @param part the particle whose hits are to be included in the sum
    @param etype the type of energy deposition to count 
  */
  virtual double energyFromParticle( const INavigable* obj,
				     const HepMC::GenParticle* part,
				     EnergyType etype = VISIBLE ) const = 0;

  
  /*
    Return the energy deposited in the calorimeter by a particle, applying a callback
    filter to identify cells of interest.
    
    @param part the particle for which to find hits in the (filtered) collection of cells
    @param etype the type of energy deposition to count
    @param caller a pointer to the client, passed to the filter function 
    @param accept a boolean function that filters out cells to consider (return true iff accept cell)
           example: return true if cell |E| / noise > 4 to only count hits in seeds for 4/2/0 topoclusters
  */
  virtual double energyInCalorimeter( const HepMC::GenParticle* part,
				      EnergyType etype,
				      void* caller = 0x0,
				      bool (*accept)( void*, const Identifier& ) = &(ICaloParticleIDTool::idAccept) ) const = 0;
  
  
  /*
    Return the energy deposited in dead material by a particle.
    
    @param part the particle for which to find hits in the dead material
    @param etype the type of energy deposition to count
    @param caller a pointer to the calling object, passed to filter function
    @param accept a boolean function that filters out DM to consider (return true iff accept DM ID)
  */
  virtual double energyInDeadMaterial( const HepMC::GenParticle*,
				       EnergyType etype,
				       void* caller = 0x0, 
				       bool (*accept)( void*, const Identifier& ) = &(ICaloParticleIDTool::idAccept) ) const = 0;
  
  
  /*
    Load energy map for all particles contributing hits to a calorimeter object.
    
    @param obj the calorimeter object to inspect
    @param emap the map of {particle : E} to build for the provided calorimeter object
    @param etype the type of energy deposition to count
  */
  virtual void loadParticleMap( const INavigable* obj, 
				std::map<const HepMC::GenParticle*,double>& emap,
				EnergyType etype = VISIBLE,
				void* caller = 0x0,
				bool (*accept)(void*, const Identifier&) = &(ICaloParticleIDTool::idAccept) ) const = 0;
  
  // 
  // accessors for calibration hit information
  //
  
  
  /* 
     Return the energy of a given type from all hits in a calorimeter object.
     
     @param obj the calorimeter object to analyze
     @param etype the type of energy deposition to count
  */
  virtual double energyInObject( const INavigable* obj, 
				 EnergyType etype = INVISIBLE ) const = 0;
  
  
  //
  // accessors to get hits directly for particles / calorimeter objects
  //
  
  
  /*
    Retrieve all hits associated with a particle.
    
    @param part the particle for which to find all associated calibration hits
    @param hits the collection of calibration hits to build (allocated if NULL)
  */
  virtual void retrieveHits( const HepMC::GenParticle* part,
			     CaloCalibrationHitContainer* hits /* = NULL */ ) const = 0;
  
  
  /*
    Retrieve all hits associated with a calorimeter object.
    
    @param obj the calorimeter object for which to find all associated calibration hits
    @param hits the collection of calibration hits to build (allocated if NULL)   
  */
  virtual void retrieveHits( const INavigable* obj,
			     CaloCalibrationHitContainer* hits /* = NULL */ ) const = 0;
    
  
  //
  // helpers that clients may find useful
  //
  
  /*
    Return pointer to the HepMC::GenParticle object with the specified barcode
  */
  virtual const HepMC::GenParticle* getParticle( unsigned int ) const = 0;
  
};

#endif // ICALOPARTICLEIDTOOL_H
