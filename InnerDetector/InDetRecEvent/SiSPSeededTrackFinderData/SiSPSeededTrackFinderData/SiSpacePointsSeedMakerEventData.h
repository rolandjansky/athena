// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
      ATLxk, //!< SiSpacePointsSeedMaker_ATLxk
      BeamGas, //!< SiSpacePointsSeedMaker_BeamGas
      Cosmic, //!< SiSpacePointsSeedMaker_Cosmic
      HeavyIon, //!< SiSpacePointsSeedMaker_HeavyIon
      ITK, //!< SiSpacePointsSeedMaker_ITK
      LowMomentum, //!< SiSpacePointsSeedMaker_LowMomentum
      Trigger ///!< SiSpacePointsSeedMaker_Trigger
    };

    bool initialized{false};
    bool trigger{false};
    bool izvertex{false};
    bool endlist{true};
    bool isvertex{false};
    bool checketa{false};

    int iteration{0};
    int iteration0{0};
    int r_first{0};
    int ns{0};
    int nsaz{0};
    int nsazv{0};
    int nr{0};
    int nrf{0};
    int nrfz{0};
    int nrfzv{0};
    int state{0};
    int nspoint{2};
    int mode{0};
    int nlist{0};
    int fNmin{0};
    int fvNmin{0};
    int zMin{0};
    int nOneSeeds{0};
    int fillOneSeeds{0};
    int nprint{0};
    int nseeds{0};

    float K{0.};
    float dzdrmin{0.};
    float dzdrmax{0.};
    float ipt2C{0.};
    float ipt2K{0.};
    float COFK{0.};
    float zminU{0.};
    float zmaxU{0.};
    float zminB{0.};
    float zmaxB{0.};
    float ftrig{0.};
    float ftrigW{0.};
    float umax{0.};    

    /**
     * @name Beam geometry
     * Updated only in buildBeamFrameWork,
     * which is called by newEvent and newRegion
     */
    //@{
    float xbeam[4]{0., 1., 0., 0.}; //!< x,ax,ay,az - center and x-axis direction
    float ybeam[4]{0., 0., 1., 0.}; //!< y,ax,ay,az - center and y-axis direction
    float zbeam[4]{0., 0., 0., 1.}; //!< z,ax,ay,az - center and z-axis direction
    //@}

    std::vector<int> r_index;
    std::vector<int> r_map;
    std::vector<int> rf_index;
    std::vector<int> rf_map;
    std::vector<int> rfz_index;
    std::vector<int> rfz_map;
    std::vector<int> rfzv_index;
    std::vector<int> rfzv_map;

    std::set<float> l_vertex;

    /**
     * @name Tables for 3 space points seeds search
     * Updated in many mthods
     */
    //@{
    std::vector<InDet::SiSpacePointForSeed*> SP;
    std::vector<InDet::SiSpacePointForSeedITK*> SP_ITK;
    std::vector<float> Zo;
    std::vector<float> Tz;
    std::vector<float> R;
    std::vector<float> U;
    std::vector<float> V;
    std::vector<float> X;
    std::vector<float> Y;
    std::vector<float> Er;
    //@}

    InDet::SiSpacePointsSeed seedOutput;

    std::vector<InDet::SiSpacePointsSeed> OneSeeds;
    std::vector<InDet::SiSpacePointsProSeed> OneSeeds_Pro;
    std::vector<InDet::SiSpacePointsProSeedITK> OneSeeds_ITK;

    std::vector<std::pair<float,InDet::SiSpacePointForSeed*>> CmSp;
    std::vector<std::pair<float,InDet::SiSpacePointForSeedITK*>> CmSp_ITK;

    std::vector<std::list<InDet::SiSpacePointForSeed*>> r_Sorted;
    std::vector<std::list<InDet::SiSpacePointForSeed*>> rf_Sorted;
    std::vector<std::list<InDet::SiSpacePointForSeed*>> rfz_Sorted;
    std::vector<std::list<InDet::SiSpacePointForSeed*>> rfzv_Sorted;
    std::vector<std::list<InDet::SiSpacePointForSeedITK*>> r_Sorted_ITK;
    std::vector<std::list<InDet::SiSpacePointForSeedITK*>> rfz_Sorted_ITK;
    std::vector<std::list<InDet::SiSpacePointForSeedITK*>> rfzv_Sorted_ITK;

    std::vector<InDet::SiSpacePointsSeed> seeds;

    std::list<InDet::SiSpacePointForSeed> l_spforseed;
    std::list<InDet::SiSpacePointForSeed>::iterator i_spforseed;
    std::list<InDet::SiSpacePointForSeedITK> l_spforseed_ITK;
    std::list<InDet::SiSpacePointForSeedITK>::iterator i_spforseed_ITK;

    std::list<InDet::SiSpacePointsSeed> l_seeds;
    std::list<InDet::SiSpacePointsSeed>::iterator i_seed;
    std::list<InDet::SiSpacePointsSeed>::iterator i_seede;
    std::list<InDet::SiSpacePointsProSeed> l_seeds_Pro;
    std::list<InDet::SiSpacePointsProSeed>::iterator i_seed_Pro;
    std::list<InDet::SiSpacePointsProSeed>::iterator i_seede_Pro;
    std::list<InDet::SiSpacePointsProSeedITK> l_seeds_ITK;
    std::list<InDet::SiSpacePointsProSeedITK>::iterator i_seed_ITK;
    std::list<InDet::SiSpacePointsProSeedITK>::iterator i_seede_ITK;

    std::list<InDet::SiSpacePointForSeed*>::iterator rMin;
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

      if (type==ITK) {
        SP_ITK.resize(maxsizeSP, nullptr);
        X.resize(maxsizeSP, 0.);
        Y.resize(maxsizeSP, 0.);
      } else {
        SP.resize(maxsizeSP, nullptr);
      }
      R.resize(maxsizeSP, 0.);
      Tz.resize(maxsizeSP, 0.);
      Er.resize(maxsizeSP, 0.);
      U.resize(maxsizeSP, 0.);
      V.resize(maxsizeSP, 0.);
      if (type!=Cosmic) {
        Zo.resize(maxsizeSP, 0.);
      }

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
