/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXFITTERVALIDATIONUTILS_TRKPRIVXPURITYTOOL_H
#define TRKVERTEXFITTERVALIDATIONUTILS_TRKPRIVXPURITYTOOL_H

/**
 * @class  Trk::TrkPriVxPurityTool
 * 
 * A tool estimating the purity of the reconstructed 
 * primary vertex. The Generator level tracks,
 * corresponding to those fitted to the primary 
 * vertex candidate are examined. The fraction
 * of outliers fitted to the vertex and tracks 
 * originating from the pileup is determined.
 * 
 * @authpr Kirill Prokofiev, September 2006  
 */

#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk
{
 static const InterfaceID IID_TrkPriVxPurityTool("TrkPriVxPurityTool", 1, 0);
 
 class VxCandidate;
 class TrkPriVxPurity;
 
 class TrkPriVxPurityTool : public AthAlgTool
 {
  public:

/**
 * AlgTool method 
 */
   StatusCode initialize();

/**
 * AlgTool method 
 */
   StatusCode finalize();

/**
 * AlgTool constructor 
 */
   TrkPriVxPurityTool(const std::string& t, const std::string& n, const IInterface* p);

/**
 * AlgTool destructor 
 */
   virtual ~TrkPriVxPurityTool();
  
/**
 * AlgTool interface method 
 */
   static const InterfaceID& interfaceID(){ return IID_TrkPriVxPurityTool; } 

/**
 * Actual analysis method, returning a Trk::TrkPriVxPurity object for a 
 * given TrkVxCandidate
 */
   const TrkPriVxPurity * purity(const Trk::VxCandidate * vertex) const;
      
  private:
  
/**
 * Primary vertex definition for MC: tolerances taken in r (mm)
 * around the simulated point of proton-proton interaction.
 * All the vertices in this region will be merged to form a single 
 * simulated PV with associated charged tracks.
 */
   double m_r_tol; 

/**
 * Primary vertex definition for MC: tolerances taken in z (mm)
 * around the simulated point of proton-proton interaction.
 * All the vertices in this region will be merged to form a single 
 * simulated PV with associated charged tracks.
 */   
   double m_z_tol;

/**
 * Name of  the track true <-> rec map to be read from the storegate
 */
   std::string m_trackParticleTruthCollName;   
    
/**
 * Name of the MC event collection to read
 */
   std::string m_mc_collection_name;
   
 };//end of class definitions

}//end of namespace definitions

#endif
