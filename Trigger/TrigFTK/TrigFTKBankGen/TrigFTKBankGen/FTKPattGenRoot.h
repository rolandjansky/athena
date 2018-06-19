/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef FTKPattGenRoot_h
#define FTKPattGenRoot_h

/* package FTKPattGenRoot
 *
 * Class for merging of patterns stored in root format
 * For instruction, see FTKPattGenRoot.cxx
 *
 */

//#define DEBUG_BEAMSPOT

#ifdef DEBUG_BEAMSPOT
//#include "EventPrimitives/EventPrimitives.h"
//#include "GeoPrimitives/GeoPrimitives.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/IPropagator.h"
#endif




#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <inttypes.h>
#include <TDirectory.h>
#include "TrigFTKSim/FTKLogging.h"
#include "TrigFTKSim/FTKPMap.h"
#include "TrigFTKSim/FTKRegionMap.h"
#include "TrigFTKSim/FTKSSMap.h"
#include "TrigFTKSim/FTKConstantBank.h"
#include "TrigFTKBankGen/FTKSectorSlice.h"
#include "TrigFTKSim/FTKPatternWithCoverage.h"
#include "TrigFTKSim/FTKPatternBySector.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"

class FTKPattGenRoot : FTKLogging {
public:
   
   enum SectorSelection {
      // random sector
      RndSector=0,
      // check module geometry
      //   if there are exact matches, use one of them, drop others. 
      //   if there are no exact matches, use one of them
      ModuleGeometry=1,
      // check module geometry
      //   if there are exact matches, use the one with lowest sectorID
      //   if there are no exact matches, use one of them
      ModuleGeometrySmallestSector=2,
      ModuleGeometryBestMatch=3
   };

   FTKPattGenRoot(int curreg, FTKSSMap* ssmap , FTKSectorSlice* slices, FTKConstantBank* constbank,int keep7of8,double tolerance7of8
#ifdef DEBUG_BEAMSPOT
                  ,ToolHandle<Trk::IPropagator> *propagator
#endif
                  );
   ~FTKPattGenRoot();
   void setBeamspot(double beamspotX,double beamspotY);
   bool isBeamAtZero(void) const;
   void transformToOrigin(FTKTrack &track);
   uint64_t trackInvertion(u_int64_t ntrials, bool smear=true);
   void SetRandomNumberGenerator(CLHEP::HepRandomEngine* RndEng );				//! Track inversion
   void ConvertPhi(FTKTrack& track);								//! Convert phi to the format used in the constants calculations
   void GetRandomTrack(FTKTrack& track);							//! Generate random track
   FTKPattGenRoot::SectorSelection GetSectorSelection() const { return m_select;}		//! Get selector algorithm
   bool GoodTrack(const FTKTrack& track, int sector, FTKPatternWithSector* tmp_curpatt,int *missingPlanesPtr,double *distancePtr);
   void SetModuleGeometryCheck(const std::string &fileName,
                               FTKPattGenRoot::SectorSelection select); //! set sector selection algorithm and module geometry
   void SetRootOutput(const std::string& outfilename);		                                //! Set filename to write patterns to root file  
   void SetOverlapRemoval(int overlap) { m_overlap = overlap;}                                  //! Set algorithm for overlap removal: 0=disabled; 1=barrel-only, 2=everywhere
   bool ReadSectorFile(const std::string& sectorfile);						//! Read in sector file
   void DeleteMaps();										//! Delete/destroy all the given maps/slices/etc...
   void WriteMapsToFile();                                                                      //! Write ssmap (and hence pmap and rmap) to output root-file.
   void SetSliceParameters(double phi_min=-999,double phi_max=999, double c_min=-999,double c_max=999, 
			   double d0_min=-999,double d0_max=999, double z0_min=-999,double z0_max=999, 
			   double eta_min=-999, double eta_max=999);				//! Set slice parameters. If slice parametersa re not set, take them from the autotune procedure. Use +/- 999 for autotune parameters

   void SetD0Exponent(double d0_alpha) { m_d0_alpha=d0_alpha; }
protected:
   
   // input
   const FTKPlaneMap* pmap() const;                                                             //! pmap is an ASCII file containing the match between the ATLAS planes and the FTK planes
   int m_curreg;										//! Region
   FTKSSMap *m_ssmap;										//! Super-strip map, this object transform an hit in a SS index
   FTKSectorSlice* m_sectorslice;								//! FTKSectorSlice reads the slices file
   FTKConstantBank* m_constbank;								//! FTKConstantsBank reads the fit constants file
   SectorSelection m_select;									//! Selector algorithm

   int m_overlap = 2;                                                                           //! Selector for overlap removal: 0=disabled; 1=barrel-only, 2=everywhere

   // internal variables
   // // HepRandomEngine is an abstract class defining the interface for each random engine.
   //CLHEP::HepRandomEngine *m_pEng;
   CLHEP::RandGauss *m_RndGauss;
   CLHEP::RandFlat *m_RndFlat;
 
   // sectors
   std::vector<FTKHitPattern> m_sectorbank;  


   // output
   TDirectory *m_output;
   FTKPatternBySectorWriter* m_patternWriter;                                                   //! output root file
   // keep 7/8 layer patterns
   int m_keep7of8;
   double m_tolerance7of8;

   // Slice parameters
   double m_phi_min;
   double m_phi_max;
   double m_c_min;  
   double m_c_max;  
   double m_d0_min; 
   double m_d0_max;
   double m_z0_min; 
   double m_z0_max; 
   double m_eta_min;
   double m_eta_max;
   double m_d0_alpha;
   // beamspot
   double m_beamspot[2];
#ifdef DEBUG_BEAMSPOT
   double m_d0BS,m_z0BS;
#endif
   typedef struct {
      float rMin,rMax,zMin,zMax;
   } RZminmax_t;

   std::map<int,RZminmax_t > m_moduleBoundsPixel,m_moduleBoundsSCT;

private:
   double getRandom();
   double getRandomGaus();
   void OverlapRemoval(std::vector<FTKPatternWithSector>& patterns);
   void RemovePattern(std::vector<FTKPatternWithSector>& patterns, unsigned int id);
   int OverlapSectorBarrel(int s1, int s2 );
   int OverlapSector(int s1, int s2 );

protected:

private:
#ifdef DEBUG_BEAMSPOT
   ToolHandle<Trk::IPropagator> *m_propagator;
#endif
};

#endif
