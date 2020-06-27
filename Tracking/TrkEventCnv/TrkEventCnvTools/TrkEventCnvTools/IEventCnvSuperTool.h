/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTCNVTOOLS_IEVENTCNVSUPERTOOL
#define TRKEVENTCNVTOOLS_IEVENTCNVSUPERTOOL

#include "GaudiKernel/IAlgTool.h"

#include "AthLinks/ElementLink.h"
#include "StoreGate/ReadHandleKey.h"

class Identifier;
class AtlasDetectorID;
class TPObjRef;

namespace Trk
{
  class ITrkEventCnvTool;
  class Surface;
  class RIO_OnTrack;

  static const InterfaceID IID_IIEventCnvSuperTool("Trk::IEventCnvSuperTool", 1, 0);

  class IEventCnvSuperTool : virtual public IAlgTool
  {
  public:
    /// Key and index types used for an EL to IdentifiableContainer.
    typedef std::string ELKey_t;
    typedef unsigned int ELIndex_t;

    DeclareInterfaceID( IEventCnvSuperTool, 1, 0 );

    IEventCnvSuperTool() {};

    virtual const Trk::ITrkEventCnvTool* getCnvTool(const Identifier& id) const =0;

    /** From passed Identifier*/
    virtual const Trk::Surface* getSurface(const Identifier& id) const =0;

    /** Take the passed RoT and recreate it (i.e. fill missing pointers etc)*/
    virtual void recreateRIO_OnTrack( RIO_OnTrack *RoT ) const =0;

   /** Take the passed RoT and prepare the PRD ElementLink for writing to disc*/
    virtual void prepareRIO_OnTrack( RIO_OnTrack* Rot) const = 0;

   /** Take the passed RoT and return the EL components to write to disc */
    virtual void prepareRIO_OnTrackLink( const RIO_OnTrack* Rot,
                                         ELKey_t& key,
                                         ELIndex_t& index ) const = 0;

    /**This templated method will return the hashAndIndex of the passed RIO_OnTrack.*/
    template <class CONT, class ROT> bool getHashAndIndex(const ROT* rot,
                                                          const SG::ReadHandleKey<CONT>& contName,
                                                          typename ElementLink<CONT>::index_type& hashAndIndex) const;

    /** returns false if e.g. no ID geometry available*/
    virtual bool canHandleInDet() const =0;

    /** returns false if e.g. no Muon geometry available*/
    virtual bool canHandleMuonSpectrometer() const =0;
    
    /** returns true if e.g. no Muon+ID geometry available*/
    virtual bool noGeometry() const =0;
     
    /** Maximum number of permitted errors. */
    virtual int maxNumberOfErrors() const =0;
    
  };
}

#include "TrkEventCnvTools/IEventCnvSuperTool.icc"

#endif
