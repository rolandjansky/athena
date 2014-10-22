/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IEnvelopeDefSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IENVELOPEDEFSVC_H
#define IENVELOPEDEFSVC_H

// Gaudi
#include "GaudiKernel/IInterface.h"

// Atlas Regions
#include "AtlasDetDescr/AtlasRegion.h"

// SubDetectorEnvelopes includes
#include "SubDetectorEnvelopes/RZPair.h"

// interface definition
static const InterfaceID IID_IEnvelopeDefSvc("IEnvelopeDefSvc", 1, 0);

class IEnvelopeDefSvc : virtual public IInterface {
  /**
   @class IEnvelopeDefSvc

   Interface class which is used to retrieve sub-detector envelope definitions
   in the form of (r,z) pairs. This even supports multiple volume definitions in
   each sub-detector.

   @author Elmar.Ritsch -at- cern.ch
   */

  public:
    /** virtual distructor */
    virtual ~IEnvelopeDefSvc() {};

    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IEnvelopeDefSvc; }

    /** return a vector of (r,z) pairs, defining the respective envelope */
    virtual const RZPairVector &getRZBoundary( AtlasDetDescr::AtlasRegion region ) const = 0;

    /** return a vector of (r,z) pairs, defining the envelope in the z>0 region */
    virtual const RZPairVector &getRPositiveZBoundary( AtlasDetDescr::AtlasRegion region ) const = 0;

    /** wrapper methods */
    const RZPairVector &getBeamPipeRZBoundary() const { return getRZBoundary(AtlasDetDescr::fAtlasForward); }
    const RZPairVector &getInDetRZBoundary()    const { return getRZBoundary(AtlasDetDescr::fAtlasID);      }
    const RZPairVector &getCaloRZBoundary()     const { return getRZBoundary(AtlasDetDescr::fAtlasCalo);    }
    const RZPairVector &getMuonRZBoundary()     const { return getRZBoundary(AtlasDetDescr::fAtlasMS);      }
    const RZPairVector &getCavernRZBoundary()   const { return getRZBoundary(AtlasDetDescr::fAtlasCavern);  }

    /** legacy methods
          Any client should update to use the methods defined above!
          The following lecagy methods will be phased out once all clients have migrated to the new methods above */
    RZPairVector &getBeamPipeRZValues( unsigned short = 0) const { return const_cast<RZPairVector&>( getRZBoundary(AtlasDetDescr::fAtlasForward) ); }
    RZPairVector &getInDetRZValues( unsigned short = 0)    const { return const_cast<RZPairVector&>( getRZBoundary(AtlasDetDescr::fAtlasID) );      }
    RZPairVector &getCaloRZValues( unsigned short = 0)     const { return const_cast<RZPairVector&>( getRZBoundary(AtlasDetDescr::fAtlasCalo) );    }
    RZPairVector &getMuonRZValues( unsigned short = 0)     const { return const_cast<RZPairVector&>( getRZBoundary(AtlasDetDescr::fAtlasMS) );      }
    RZPairVector &getCavernRZValues( unsigned short = 0)   const { return const_cast<RZPairVector&>( getRZBoundary(AtlasDetDescr::fAtlasCavern) );  }

  protected:
    /** mirror the given srcRZ RZPairVector in the XY-plane to describe all corner points
     in (r,z) space in the dstRZ RZPairVector */
    inline void mirrorRZ( const RZPairVector &srcRZ, RZPairVector &dstRZ ) const ;

};

/** mirror the given RZPairs in the XY-plane to describe all corner points in (r,z) space */
inline void IEnvelopeDefSvc::mirrorRZ( const RZPairVector &srcRZ, RZPairVector &dstRZ ) const
{
  int numPosPairs = srcRZ.size();
  // the mirrored envelope will have exactly twice as many entries as the given srcRZ
  dstRZ.resize(2*numPosPairs);

  // loop over all positive (r,z) pairs
  for ( int curNum = 0; curNum<numPosPairs; curNum++) {
    double curR = srcRZ[curNum].first;
    double curZ = srcRZ[curNum].second;

    // debugging output:
    //std::cout << "Envelope: pos=" << curNum << " r=" << curR << " z="<< curZ << std::endl;

    // fill in the z<0 side
    dstRZ[numPosPairs-curNum-1] = RZPair(curR, -curZ);
    // fill in the z>0 side
    dstRZ[numPosPairs+curNum]   = RZPair(curR, curZ);
  }

  return;
}

#endif // IENVELOPEDEFSVC_H

