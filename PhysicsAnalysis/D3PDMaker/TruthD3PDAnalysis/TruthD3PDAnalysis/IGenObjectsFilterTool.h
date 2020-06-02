// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IGenObjectsFilterTool.h 348268 2011-02-28 16:12:50Z krasznaa $
/**
 * @file TruthD3PDAnalysis/TruthD3PDAnalysis/IGenObjectsFilterTool.h
 * @author Georges Aad
 * @date Nov, 2010
 * @brief interface for McEventCollection filters
*/
#ifndef TRUTHD3PDANALYSIS_IGENOBJECTSFILTERTOOL_H
#define TRUTHD3PDANALYSIS_IGENOBJECTSFILTERTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/IAlgTool.h"

// Forward declaration(s):
#include "AtlasHepMC/GenParticle_fwd.h"
#include "AtlasHepMC/GenEvent_fwd.h"
#include "AtlasHepMC/GenVertex_fwd.h"

class McEventCollection;

/// Interface ID for this tool type
static const InterfaceID IID_IGenObjectsFilterTool( "IGenObjectsFilterTool", 1, 0 );

/**
 *  @short Interface for truth object selector tools
 *
 *         This class provides a uniform interface for all the truth object
 *         selection tools which can be used in Athena jobs to select the truth
 *         objects of interest to the analysis.
 *
 * @author Georges Aad <aad@cern.ch>
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 348268 $
 * $Date: 2011-02-28 17:12:50 +0100 (Mon, 28 Feb 2011) $
 */
class IGenObjectsFilterTool : virtual public IAlgTool  {

public:
   /// Virtual destructor, to make vtable happy
   virtual ~IGenObjectsFilterTool() {}

   /// The interface declaration
   static const InterfaceID& interfaceID() { return IID_IGenObjectsFilterTool; }

   /// Function selecting GenEvent objects
   /**
    * This function is used to select GenEvent objects of interest for a given
    * analysis.
    *
    * @param evt Pointer to the GenEvent object in question
    * @param coll Optional pointer to the full truth record
    * @returns <code>true</code> if the object should be selected,
    *          <code>false</code> otherwise
    */
   virtual bool pass( const HepMC::GenEvent* evt,
                      const McEventCollection* coll = 0 ) const = 0;

   /// Function selecting GenParticle objects
   /**
    * This function is used to select GenParticle objects of interest for a given
    * analysis.
    *
    * @param part Pointer to the GenParticle object in question
    * @param coll Optional pointer to the full truth record
    * @returns <code>true</code> if the object should be selected,
    *          <code>false</code> otherwise
    */
   virtual bool pass( const HepMC::GenParticle* part,
                      const McEventCollection* coll = 0 ) const = 0;

   /// Function selecting GenVertex objects
   /**
    * This function is used to select GenVertex objects of interest for a given
    * analysis.
    *
    * @param vtx Pointer to the GenVertex object in question
    * @param coll Optional pointer to the full truth record
    * @returns <code>true</code> if the object should be selected,
    *          <code>false</code> otherwise
    */
   virtual bool pass( const HepMC::GenVertex* vtx,
                      const McEventCollection* coll = 0 ) const = 0;

}; // class IGenObjectsFilterTool

#endif // TRUTHD3PDANALYSIS_IGENOBJECTSFILTERTOOL_H
