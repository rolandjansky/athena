/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_ElectronPidTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRT_ELECTRONPIDTOOL_H
#define INDETTRT_ELECTRONPIDTOOL_H

// #include "GaudiKernel/AlgTool.h"           // Exchange OUT
// #include "GaudiKernel/MsgStream.h"         // Exchange OUT
#include "AthenaBaseComps/AthAlgTool.h"       // Exchange IN

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkToolInterfaces/ITRT_ElectronPidTool.h"
#include "TRT_ElectronPidTools/ITRT_ElectronToTTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"



//#include "TRT_ToT_Tools/ITRT_ToT_dEdx.h"

#include <vector>
#include <string>

class AtlasDetectorID;
class Identifier;
class TRT_ID;
namespace InDetDD{ class TRT_DetectorManager; }

//class IChronoStatSvc;
class ITRT_ToT_dEdx;
namespace Trk {
  class Track;
}

namespace InDet 
{

  /** @class TRT_ElectronPidTool 

      TRT_ElectronPidTool is a tool for identification of electrons based on information
      mainly from the TRT and partially from the whole ID. Given a track, three methods are used:

      1: High Threshold (HT) information - sensitive to Transition Radiation (TR) photons
      emitted by electrons, when traversing the TRT radiator material between the straws.
      Electrons start to emit TR at 500 MeV, but only fully at 5 GeV.

      2: Time-over-Threshold (ToT) information - sensitive to (increased) ionization by
      electrons compared to heavier particles (eg. pions, protons, etc.). ToT is computed
      only for straws without a HT hit (to avoid correlations).
      The separation is largest at low momentum, and deminishes with increased energy.

      3: Bremfitting (Brem) information - sensitive to (increased) energy losses of electron
      tracks due to bremsstrahlung. Ideally, a continuous variable should be used, but
      for now only the discreet variable DNA-kicked-in-or-not is used.
      ---This part is currently not used

      The three estimators are independent, and can thus be combined directly. The tool returns
      a vector with four floats, giving the probability of track being an electron based on:
      0: All information combined, 1: HT information, 2: ToT/D, 3: Brem information
      
      the third value in the vector is the discriminator that is used in the determination of 
      the particle likelyhood based on ToT. 

      @author  Troels Petersen <Troels.Petersen@cern.ch>
      and Simon Heisterkamp <Simon.Heisterkamp@cern.ch>
  */

  class TRT_ElectronPidTool;

  class TRT_ElectronPidTool : virtual public Trk::ITRT_ElectronPidTool, virtual public ITRT_ElectronToTTool, public AthAlgTool
  {
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //Athena specific, called externally:
  public:
    TRT_ElectronPidTool(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~TRT_ElectronPidTool ();
      
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();

    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  (); 

    /** Electron probabilities to be returned */
    std::vector<float> electronProbability(const Trk::Track& track);

    /** Electron probabilities to be returned */
    std::vector<float> electronProbability_old(const Trk::Track& track);

    double probHT( const double pTrk, const Trk::ParticleHypothesis hypothesis, const int HitPart, const int Layer, const int Strawlayer);
    double probHTRun2( float, Trk::ParticleHypothesis, int, int, int, float, float, float ) { return 0.5; }



  // get the ToT from the bitpattern and correct for local variations
    double GetToT(unsigned int bitpattern, double HitZ, double HitR, int BEC, int Layer, int Strawlayer) const;
  
    // get the distance used to normalize the ToT
    double GetD(double R_track) const;

  private:
    // Update of database entries.
    StatusCode update( IOVSVC_CALLBACK_ARGS );        
    //
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    
    /** Probability functions used in calculation */
    // Also used extenally by the Atlfast simulation to get pHT:
    //virtual double probHT(double momentum, Trk::ParticleHypothesis, int HitPart, double HitDepth, double TrkAnodeDist, double eta, double phi);
    
    // //possibly used by Atlfast in the future
    //virtual double probToT(double momentum, Trk::ParticleHypothesis, int ToT, double TrkAnodeDist, double eta);
    
    //probability of being a particular particle based on the Brem finders:
    virtual float probBrem(const Trk::Track& track);

    bool m_DATA;

    //Check valid TRT straw:
    bool CheckGeometry(int BEC, int Layer, int Strawlayer);

    //Turn the Bitpattern into a human readable string
    std::string PrintBitPattern(unsigned int bitpattern);
      
    //Count how many HT and LT bits there are.
    int CountLTBitPattern(unsigned int bitpattern);
    int CountHTBitPattern(unsigned int bitpattern);

    //    IChronoStatSvc  *m_timingProfile;

 

    const TRT_ID*              m_trtId;               // TRT ID helper (identifying barrel/wheels and global position)
    const InDetDD::TRT_DetectorManager* m_TRTdetMgr;  // TRT detector manager (to get ID helper)
    // StoreGateSvc*              p_detstore;            // Detector store.
    Trk::ParticleMasses        m_particlemasses;      // Particle masses. (initalized in default constructor)
    unsigned int               m_minTRThits;          // Minimum number of TRT hits to give PID.
    bool                       m_bremFitterEnabled;   // jobOption whether or not brem Chi2 fitter is enabled.

    //Some constants about the Bitpattern:
    static const unsigned int LTbits=0x3FDFEFF;
    static const unsigned int HTbits=0x4020100;

    //The following class is designed to conatin all code that is used to calculate the likelyhood from ToT.


   public:
    class ToTcalculator;
   private:
    ToTcalculator & ToTcalc;
  
   public:
    class HTcalculator;
   private:
    HTcalculator & HTcalc;

    ToolHandle<ITRT_ToT_dEdx> m_TRTdEdxTool;     //!< the track selector tool
/*
    class BaseCalculator {
    public:
      //Class to encode those aspects of the ToT and HT calculator that they have in common
      AthAlgTool & parent;
      const char * my_name;
      int BLOB_SIZE;
      unsigned char * Blob;

      float & UpperLimit;
      float & LowerLimit;

      bool HasBeenInitialized;

      static const int SIZE_OF_HEADER = 12;
      
      // | 
      // | The following are all the major offsets from the blob start:
      
      //versioning check constants
      int CurrentVersion;
      static const int _Version    = 0;
      static const int _Day   = 1;
      static const int _Month = 2;
      static const int _Year  = 3;
      
      static const int OFF_UpperLim = 4;
      static const int OFF_LowerLim = 8;


    BaseCalculator(AthAlgTool & p, int size, const char * name):parent(p),
        my_name(name),
        BLOB_SIZE(size),
        Blob(new unsigned char[size]),
        UpperLimit( * ((float*)( Blob + OFF_UpperLim) ) ),
        LowerLimit( * ((float*)( Blob + OFF_LowerLim) ) ),
        HasBeenInitialized(0)
          {}
      
      ~BaseCalculator(){
        delete [] Blob;
      }

      // set constants to hard coded defaults
      virtual void setDefaultCalibrationConstants()=0;

    public:
      void checkInitialization();

      // Fill the data blob from a given pointer
      bool FillBlob(const unsigned char*);      
      
      //print the blob of data to the screen very verbosely.
      void PrintBlob();

      // Limit the allowed PID value to lie between a lower and an upper limt
      float Limit(float prob);      
    };
*/
   }; 
} // end of namespace

#endif 
