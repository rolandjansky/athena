// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for struct SiSpacePointsSeedMakerEventData
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMakerEventData_h
#define SiSpacePointsSeedMakerEventData_h

#include "SiSpacePointsSeed/SiSpacePointsSeed.h"
#include "SiSPSeededTrackFinderData/SiSpacePointForSeed.h"
#include "SiSPSeededTrackFinderData/SiSpacePointForSeedITK.h"
#include "SiSPSeededTrackFinderData/SiSpacePointsProSeed.h"
#include "SiSPSeededTrackFinderData/SiSpacePointsProSeedITK.h"

#include <list>
#include <map>
#include <vector>

namespace InDet {

 /**
  @class InDet::SiSpacePointsSeedMakerEventData
  
  InDet::SiSpacePointsSeedMakerEventData holds event dependent data
  used by ISiSpacePointsSeedMaker.
  The object is owened by SiSPSeededTrackFinder.
  Some data members are not used depending on ToolType.
  @author Susumu.Oda@cern.ch
  */

  class SiSpacePointsSeedMakerEventData {
  public:
    /// enums to specify which SiSpacePointsSeedMaker owns the object.
    enum ToolType {
      ATLxk, ///< SiSpacePointsSeedMaker_ATLxk
      BeamGas, ///< SiSpacePointsSeedMaker_BeamGas
      Cosmic, ///< SiSpacePointsSeedMaker_Cosmic
      HeavyIon, ///< SiSpacePointsSeedMaker_HeavyIon
      ITK, ///< SiSpacePointsSeedMaker_ITK
      LowMomentum, ///< SiSpacePointsSeedMaker_LowMomentum
      Trigger ////< SiSpacePointsSeedMaker_Trigger
    };

    bool initialized{false};    ///< has the data object been initialized?  
    bool trigger{false};        ///< are we running in trigger mode? 
    bool izvertex{false};
    /**  indicate if we are done with the seed search for an event. 
    * Is set to false if we have to abort the current seed finding pass 
    * (for example due to reaching max seed capacity) and need to 
    * continue after returning a few seeds to the user first. 
    **/ 
    bool endlist{true};    
    bool isvertex{false};   ///< whether or not we contain a non-empty vertex list
    bool checketa{false};   ///< whether to apply eta cuts

    int maxSeedsPerSP{0};   ///<Number of Seeds allowed to be built per central Seed
    bool keepAllConfirmedSeeds{false};   ///<Do we keep an unlimited number of confirmed seeds per central SP?
    int iteration{0};   ///< iteration currently being run 
    int iteration0{0};
    int r_first{0}; ///< points to the index of the highest occupied radius bin 
    int ns{0};      ///< total number of SP that we sorted into our r-binned vector
    int nsaz{0};      ///< number of SP in the Phi-Z 2D binning
    int nsazv{0};
    int nr{0};        ///< this keeps track of the number of occupied radial bins (so it is typically less than the total number of r bins
    int nrf{0};
    int nrfz{0};     ///< also number of SP in the phi-Z 2D binning??
    int nrfzv{0};
    int state{0};     ///< state info - 0 seems to mean cuts aren ot configured
    int nspoint{2};   ///< number of required SP
    int mode{0};      ///< operation mode (SP per seed etc) 
    int nlist{0};
    int fNmin{0};     ///< starting phi bin for looping
    int fvNmin{0};
    int zMin{0};
    int nOneSeeds{0};
    int fillOneSeeds{0};
    int nprint{0};
    int nseeds{0};
    int seedPerSpCapacity{0}; ///< capacity for seeds from a single SP in the respective storage vector

    float K{0.};      ///< conversion from pT in MeV to twice the circle radius in the transverse plane in mm 
    float dzdrmin{0.};    //<! store eta cuts interpreted as dz/dr 
    float dzdrmax{0.};    //<! store eta cuts interpreted as dz/dr 
    float ipt2C{0.};  ///< inverse of 90% of the pt cut, squared, multiplied by a magic conversion factor 
    float ipt2K{0.};  ///< 1 / (K * 0.9 * pt cut)^2, allows us to directly apply our pt cut on the (2R)^2 estimate we obtain from the seed
    float COFK{0.};   ///< a magic number 
    float zminU{0.};
    float zmaxU{0.};
    float zminB{0.};
    float zmaxB{0.};
    float ftrig{0.};
    float ftrigW{0.};
    float maxScore{0.};    

    /**
     * @name Beam geometry
     * Updated only in buildBeamFrameWork,
     * which is called by newEvent and newRegion
     */
    //@{
    float xbeam[4]{0., 1., 0., 0.}; ///< x,ax,ay,az - center and x-axis direction
    float ybeam[4]{0., 0., 1., 0.}; ///< y,ax,ay,az - center and y-axis direction
    float zbeam[4]{0., 0., 0., 1.}; ///< z,ax,ay,az - center and z-axis direction
    //@}

    std::vector<int> r_index;
    std::vector<int> r_map;
    std::vector<int> rf_index;
    std::vector<int> rf_map;
    std::vector<int> rfz_index;  ///< 2D index in the 2D phi-z map of each SP in the r-sored vector
    std::vector<int> rfz_map;    ///< number of SP in each bin of the 2D phi-z map 
    std::vector<int> rfzv_index;
    std::vector<int> rfzv_map;

    std::set<float> l_vertex;

    /**
     * @name Tables for 3 space points seeds search
     * Updated in many mthods
     */
    //@{
    std::vector<InDet::SiSpacePointForSeed*> SP;    ///< space points to consider for the current seed candidate
    std::vector<InDet::SiSpacePointForSeedITK*> SP_ITK;
    
    /// The following are parameters characterising individual space points within a seed (relative to the central point)
    std::vector<float> Zo;    ///< z0 estimate from 2 points
    std::vector<float> Tz;    ///< 1/tan(theta) == dz/dr estimate from 2 points
    std::vector<float> R;     ///< inverse distance to the central space point 
    std::vector<float> U;     ///< transformed U coordinate (x/(x^2+y^2)) in frame around central SP 
    std::vector<float> V;     ///< transformed V coordinate (y/(x^2+y^2)) in frame around central SP 
    std::vector<float> X;
    std::vector<float> Y;
    std::vector<float> Er;    ///< error component on 1/tan(theta)==dz/dr from the position errors on the space-points
    //@}

    InDet::SiSpacePointsSeed seedOutput;

    std::vector<InDet::SiSpacePointsSeed> OneSeeds;
    std::vector<InDet::SiSpacePointsProSeed> OneSeeds_Pro;
    std::vector<InDet::SiSpacePointsProSeedITK> OneSeeds_ITK;

    std::vector<std::pair<float,InDet::SiSpacePointForSeed*>> CmSp;
    std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>> CmSp_ITK;

    std::vector<std::vector<InDet::SiSpacePointForSeed*>> r_Sorted;     ///< vector of space points in each bin of the 1D radial binning 
    std::vector<std::vector<InDet::SiSpacePointForSeed*>> rf_Sorted;
    std::vector<std::vector<InDet::SiSpacePointForSeed*>> rfz_Sorted;   ///< vector of space points in each bin of the 2D phi-z binning
    std::vector<std::vector<InDet::SiSpacePointForSeed*>> rfzv_Sorted;
    std::vector<std::list<InDet::SiSpacePointForSeedITK*>> r_Sorted_ITK;
    std::vector<std::list<InDet::SiSpacePointForSeedITK*>> rfz_Sorted_ITK;
    std::vector<std::list<InDet::SiSpacePointForSeedITK*>> rfzv_Sorted_ITK;

    std::vector<InDet::SiSpacePointsSeed> seeds;

    std::list<InDet::SiSpacePointForSeed> l_spforseed;      //<! list of all space points considered for seed building. This has ownership over the pointers stored in the binned vectors ("histograms") above
    std::list<InDet::SiSpacePointForSeed>::iterator i_spforseed;    //<! keep track of an iterator over the seed list. Frequently used to keep track of where to add the next SP
    std::list<InDet::SiSpacePointForSeedITK> l_spforseed_ITK;
    std::list<InDet::SiSpacePointForSeedITK>::iterator i_spforseed_ITK;

    std::list<InDet::SiSpacePointsSeed> l_seeds;
    std::list<InDet::SiSpacePointsSeed>::iterator i_seed;
    std::list<InDet::SiSpacePointsSeed>::iterator i_seede;
    std::list<InDet::SiSpacePointsProSeed> l_seeds_Pro;       //<! lists of output seeds 
    std::list<InDet::SiSpacePointsProSeed>::iterator i_seed_Pro;       //<! iterators over the said list 
    std::list<InDet::SiSpacePointsProSeed>::iterator i_seede_Pro;
    std::list<InDet::SiSpacePointsProSeedITK> l_seeds_ITK;
    std::list<InDet::SiSpacePointsProSeedITK>::iterator i_seed_ITK;
    std::list<InDet::SiSpacePointsProSeedITK>::iterator i_seede_ITK;

    std::vector<InDet::SiSpacePointForSeed*>::iterator rMin;
    std::list<InDet::SiSpacePointForSeedITK*>::iterator rMin_ITK;

    std::multimap<float,InDet::SiSpacePointsSeed*> mapOneSeeds;
    std::multimap<float,InDet::SiSpacePointsSeed*> mapSeeds;
    std::multimap<float,InDet::SiSpacePointsSeed*> l_seeds_map;
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator seed;
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator seede;
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator i_seed_map;
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator i_seede_map;
    std::multimap<float,InDet::SiSpacePointsProSeed*> mapOneSeeds_Pro;
    std::multimap<float,InDet::SiSpacePointsProSeed*> seeds_Pro;
    std::multimap<float,InDet::SiSpacePointsProSeed*>::iterator seed_Pro;
    std::multimap<float,InDet::SiSpacePointsProSeedITK*> mapOneSeeds_ITK;
    std::multimap<float,InDet::SiSpacePointsProSeedITK*> seeds_ITK;
    std::multimap<float,InDet::SiSpacePointsProSeedITK*>::iterator seed_ITK;


    /// allow to resize the space-point container on-the-fly in case
    /// more space is needed. 
    /// This is a compromise to avoid a fixed array size while 
    /// still minimising the number of re-allocations
    void resizeSPCont(size_t increment=50, ToolType type = ToolType::ATLxk){
      size_t currSize = SP.size();
      size_t newSize = currSize + increment; 
      if (type == ITK) {
        SP_ITK.resize(newSize, nullptr);
        X.resize(newSize, 0.);
        Y.resize(newSize, 0.);
      } else {
        SP.resize(newSize, nullptr);
      }
      R.resize(newSize, 0.);
      Tz.resize(newSize, 0.);
      Er.resize(newSize, 0.);
      U.resize(newSize, 0.);
      V.resize(newSize, 0.);
      if (type != Cosmic) {
        Zo.resize(newSize, 0.);
      }
    }

    /// Initialize data members based on ToolType enum.
    /// This method has to be called just after creation in SiSPSeededTrackFinder.
    void initialize(ToolType type,
                    int maxsizeSP,
                    int maxOneSize,
                    int maxsize,
                    int sizeR,
                    int sizeRF,
                    int sizeRFZ,
                    int sizeRFZV,
                    bool checkEta) {
      if (type==ATLxk) {
        CmSp.reserve(500);
      } else if (type==ITK) {
        CmSp_ITK.reserve(500);
      }
      resizeSPCont(maxsizeSP,type); 
      seedPerSpCapacity = maxOneSize; 
      if (type==ATLxk) {
        OneSeeds_Pro.resize(maxOneSize);
      } else if (type==BeamGas or type==HeavyIon or type==LowMomentum or type==Trigger) {
        OneSeeds.resize(maxOneSize);
      } else if (type==ITK) {
        OneSeeds_ITK.resize(maxOneSize);
      }

      // Build radius sorted containers
      r_index.resize(sizeR, 0);
      r_map.resize(sizeR, 0);
      if (type==ITK) {
        r_Sorted_ITK.resize(sizeR);
      } else {
        r_Sorted.resize(sizeR);
      }

      if (type==BeamGas or type==Cosmic) {
        // Build radius-azimuthal sorted containers
        rf_index.resize(sizeRF, 0);
        rf_map.resize(sizeRF, 0);
        rf_Sorted.resize(sizeRF, {});
      }

      // Build radius-azimuthal-Z sorted containers
      rfz_index.resize(sizeRFZ, 0);
      rfz_map.resize(sizeRFZ, 0);
      if (type==ITK) {
        rfz_Sorted_ITK.resize(sizeRFZ, {});
      } else {
        rfz_Sorted.resize(sizeRFZ, {});
      }

      if (type==ATLxk or type==HeavyIon or type==ITK or type==Trigger) {
        // Build radius-azimuthal-Z sorted containers for Z-vertices
        rfzv_index.resize(sizeRFZV, 0);
        rfzv_map.resize(sizeRFZV, 0);
        if (type==ITK) {
          rfzv_Sorted_ITK.resize(sizeRFZV, {});
        } else {
          rfzv_Sorted.resize(sizeRFZV, {});
        }
      }

      if (type==Cosmic) {
        seeds.resize(maxsize+5);
      }

      if (type==ATLxk) {
        i_seed_Pro  = l_seeds_Pro.begin();
        i_seede_Pro = l_seeds_Pro.end();
      } else if (type==BeamGas or type==HeavyIon or type==LowMomentum or type==Trigger) {
        i_seed  = l_seeds.begin();
        i_seede = l_seeds.end();
      } else if (type==ITK) {
        i_seed_ITK  = l_seeds_ITK.begin();
        i_seede_ITK = l_seeds_ITK.end();
      }

      if (type==Trigger) {
        seed  = mapSeeds.begin();
        seede = mapSeeds.end();
      }

      if (type==ATLxk or type==ITK) {
        checketa = checkEta;
      }

      initialized = true;
    }
  };
  
} // end of name space

#endif // SiSpacePointsSeedMakerEventData_h
