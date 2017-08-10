// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTILECORRECTION_IJETTILECORRECTIONTOOL_H
#define JETTILECORRECTION_IJETTILECORRECTIONTOOL_H

// Framework include(s):
#include "PATInterfaces/ISystematicsTool.h"

// EDM include(s):
#include "xAODJet/Jet.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"


//eta-phi (parametrization) coordinates 
typedef std::pair<int,int> IPair;

namespace JTC{

  enum class PART : unsigned int {LB, EB};

  enum class TS : unsigned int {GOOD,EDGE,CORE,UNKNOWN};

  enum class TYPE : unsigned int {User, DB};

  struct Hole{
    double eta1;
    double eta2;
    double phi1;
    double phi2;
    std::pair<int,int> iov;
  };

  struct Region{

    Hole  hole;
    IPair ep;
    PART  part;
    float cfactor;
    TS    status;
    TYPE  type;

    Region(Hole h, IPair pair, PART p, float cf, TS st, TYPE t)
      : hole(h), 
    	ep(pair),
    	part(p),
    	cfactor(cf),
    	status(st),
    	type(t)
    {}

    bool operator < (const Region& r) const {
      return (this->cfactor < r.cfactor);
    }
    bool operator > (const Region& r) const {
      return (this->cfactor > r.cfactor);
    }
  }; 

}

namespace CP {

  /// Interface for the tool correcting for dead Tile modules
  ///
  /// Documentation to be written...
  ///
  /// @author Martin Tripiana <tripiana@cern.ch>
  ///
  /// $Revision$
  /// $Date$
  ///
  class IJetTileCorrectionTool : public virtual ISystematicsTool {
    
    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( CP::IJetTileCorrectionTool )
    
    public:
    /// Apply the correction on a modifyable object
    virtual CorrectionCode applyCorrection( xAOD::Jet& jet ) = 0;
    
    /// Create a corrected copy from a constant jet
    virtual CorrectionCode correctedCopy( const xAOD::Jet& input,
					  xAOD::Jet*& output ) = 0;
    
    
    //status helpers
    virtual JTC::TS getTileStatus(const xAOD::Jet& jet) = 0;
    virtual StatusCode addTileStatus(const xAOD::Jet& jet) = 0;
    
    //DR helper
    virtual void setRJET(float r) = 0; //Set Jet Radius. 
    //NOTE: Only for checking overlap! The correction is only derived for EMTopo R=0.4 jets at the moment.
    
  }; // class IJetTileCorrectionTool
  
} // namespace CP

#endif // JETTILECORRECTION_IJETTILECORRECTIONTOOL_H
