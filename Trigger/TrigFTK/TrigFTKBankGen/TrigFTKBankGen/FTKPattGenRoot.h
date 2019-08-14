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
   void SetBadModules(std::istream &badlist);
   void SetRootOutput(const std::string& outfilename);		                                //! Set filename to write patterns to root file  
   void SetFlatInCot(bool flatInCot) { m_flat_in_cot = flatInCot;}                                  //! Set algorithm for polar angle: flat in eta=0 or cot(theta)=1
   void SetOverlapRemoval(int overlap) { m_overlap = overlap;}                                  //! Set algorithm for overlap removal: 0=disabled; 1=barrel-only, 2=everywhere
   bool ReadSectorFile(const std::string& sectorfile);						//! Read in sector file
   void DeleteMaps();										//! Delete/destroy all the given maps/slices/etc...
   void WriteMapsToFile();                                                                      //! Write ssmap (and hence pmap and rmap) to output root-file.
   void SetTrackParameterRange
      (std::pair<double,double> &track_phiRange,
       std::pair<double,double> &track_cRange,
       std::pair<double,double> &track_d0Range,
       std::pair<double,double> &track_z0Range,
       std::pair<double,double> &track_etaRange);
   //! initialize track parameters
protected:
   
   // input
   const FTKPlaneMap* pmap() const;                                                             //! pmap is an ASCII file containing the match between the ATLAS planes and the FTK planes
   int m_curreg;										//! Region
   FTKSSMap *m_ssmap;										//! Super-strip map, this object transform an hit in a SS index
   FTKSectorSlice* m_sectorslice;								//! FTKSectorSlice reads the slices file
   FTKConstantBank* m_constbank;								//! FTKConstantsBank reads the fit constants file
   SectorSelection m_select;									//! Selector algorithm

   int m_overlap;                                                                           //! Selector for overlap removal: 0=disabled; 1=barrel-only, 2=everywhere

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

   bool m_flat_in_cot;
   // track parameter intervals from which the patterns are drawn
   // by default, these ranges are taken from the slide parameters
   std::pair<double,double> m_phi_range,m_c_range,m_d0_range,m_z0_range,
      m_eta_range,m_cot_range;
   // beamspot
   double m_beamspot[2];
#ifdef DEBUG_BEAMSPOT
   double m_d0BS,m_z0BS;
#endif
   typedef struct {
      float rMin,rMax,zMin,zMax;
   } RZminmax_t;

   std::map<int,RZminmax_t > m_moduleBoundsPixel,m_moduleBoundsSCT;
   std::set<int> m_moduleBadPixel,m_moduleBadSCT;

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
