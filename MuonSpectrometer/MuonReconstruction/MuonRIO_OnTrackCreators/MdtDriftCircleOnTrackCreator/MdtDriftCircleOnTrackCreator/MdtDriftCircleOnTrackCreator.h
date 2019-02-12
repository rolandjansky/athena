/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for class  MdtOnTrackTool 
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for MuonDriftCircleOnTrack production
// (for MDT technology)
///////////////////////////////////////////////////////////////////
// Version 1.0 18/07/2004 
///////////////////////////////////////////////////////////////////

#ifndef MUON_MDTDRIFTCIRCLEONTRACKCREATOR_H
#define MUON_MDTDRIFTCIRCLEONTRACKCREATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSpaceTimePoint/SpaceTimePoint.h"
#include <bitset>
#include <string>

class Identifier;
class MdtIdHelper;
class MdtCalibrationSvc;
class MdtCalibrationDbSvc;
class MdtCalibrationSvcSettings;
class MdtCalibrationSvcInput;
class MsgStream;

namespace Muon {

    class IMuonTofTool;
    class MdtPrepData;
    class MuonIdHelperTool;

    /**Interface for the reconstruction to MDT calibration and alignment corrections. It should be used by 
              reconstruction and pattern recognition to create Muon::MdtDriftCircleOnTrack (s).

       It offers several interfaces:
       - Create new Muon::MdtDriftCircleOnTrack from a Trk::MdtPrepData and a predicted Trk::TrackParameter.       
         @code const MdtDriftCircleOnTrack* correct ( const Trk::PrepRawData& prd, const Trk::TrackParameters& tp) const @endcode
       - Create new Muon::MdtDriftCircleOnTrack from a Trk::MdtPrepData and a prediction of the global position 
         of the point of closest approach of the track to the wire. Knowing the second coordinate is sufficient as 
         the side on which the track passed the wire is not determined.
         @code createRIO_OnTrack(const MdtPrepData& prd, const Trk::GlobalPosition& globalPos) const @endcode
       - Update the side (sign) of the  Muon::MdtDriftCircleOnTrack.
         @code updateSign(const MdtDriftCircleOnTrack& rot, const Trk::DriftCircleSide si) const @endcode
       
       The tool is capable of handling serveral different timing cases (click links to see definitions):
        - normal time of flight corrections assuming IP + light speed to be used for simulated data and collision data
        - cosmics taken with scintilator trigger or cosmic simulation without TOF 
        - cosmics taken with scintilator trigger which is read out so the offset with respect to the clock is known. 
           This mode is not completely functional yet as the way to access the trigger time is not known at the moment
        - normal cosmic data with rpc trigger or simulation including TOF. 
    It uses the MuonCosmicTofTool to calculate the correct timing with respect to the MuonTriggerPoint
        - testbeam running. Currently not implemented, propose not to change anything for the moment for testbeam
       The modes can be selected setting the TimingMode in the job options.

       JobOptions Flags:
       - doMDT: switch on/off ROT creation (default = true)
       - TimingMode: select timing mode (default = ATLTIME)
       - MuonTofTool: Tool to be used to calculate time of flight (default = "Muon::MuonCosmicTofTool/MuonCosmicTofTool")
       - DoWireSag: Flag to turn on application of geometrical wire sagging correstions (default = false)
       - CreateTubeHit: Flag to turn on the creation of tube hits (default = false)
    */
    class MdtDriftCircleOnTrackCreator : public AthAlgTool, virtual public IMdtDriftCircleOnTrackCreator {
    public:
      enum TimingMode { ATLTIME=0, //!< normal time of flight corrections assuming IP + light speed to be used for simulated data and collision data
            NO_CORRECTIONS=1, //!< special case for cosmics taken with scintilator trigger or cosmic simulation without TOF 
            COSMICS_TRIGGERTIME=2, //!<  special case for cosmics taken with scintilator trigger which is read out so the offset with respect to the clock is known. This mode is not completely functional yet as the way to access the trigger time is not known at the moment
            COSMICS_TOF=3, //!< case for normal cosmic data with rpc trigger or simulation including TOF. It uses the MuonCosmicTofTool to calculate the correct timing with respect to the MuonTriggerPoint
            NumberOfTimingModes
                  };

    public:

      MdtDriftCircleOnTrackCreator(const std::string&,const std::string&,const IInterface*);
      virtual ~MdtDriftCircleOnTrackCreator ();
      virtual StatusCode initialize();
      virtual StatusCode finalize  ();

      /** @brief Calibrate a MdtPrepData object. The result is stored in a new MdtDriftCircleOnTrack object.
      Included calibrations:
       - Conversion t->r using MdtCalibrationSvc
       - Wire sag + chamber deformations (if available)
       - Special treatment for cosmics if switched on
      @param prd  MdtPrepData object
      @param globalPos GlobalPosition (including second coordinate along the tube)
      @param gdir GlobalDirection of track
      @param strategy optional drift circle error strategy to override the default
      @return Fully calibrated MdtDriftCircleOnTrack (the user must delete this object when it is no longer needed)
      */
      virtual const MdtDriftCircleOnTrack* createRIO_OnTrack( const MdtPrepData& prd,
                                                              const Amg::Vector3D& globalPos,
                                                              const Amg::Vector3D* gdir = 0,
                                                              const MuonDriftCircleErrorStrategy* strategy = 0 ) const;

      /** @brief Update of the sign of the drift radius. The method creates a new MdtDriftCircleOnTrack, the old input MdtDriftCircleOnTrack is 
             not deleted. The user should take care of the memory managment of both MdtDriftCircleOnTracks.
      @param rot reference to the Muon::MdtDriftCircleOnTrack of which the sign should be updated.
      @param si  Trk::DriftCircleSide indicating whether the muon passed on the left or right side of the wire.
      */
      virtual void    updateSign( MdtDriftCircleOnTrack& rot, 
                                  const Trk::DriftCircleSide si) const;

      /** @brief Update error of a ROT without changing the drift radius
	  @param DCT reference to the Muon::MdtDriftCircleOnTrack of which the sign should be updated.
	  @param pars track prediction at DCT used when using the track prediction to update the error
      @param strategy optional drift circle error strategy to override the default
	  @return New ROT with updated error. (the user must delete this object when it is no longer needed).
      */ 
      virtual const MdtDriftCircleOnTrack* updateError( const MdtDriftCircleOnTrack& DCT, 
                                                        const Trk::TrackParameters* pars = 0,
                                                        const MuonDriftCircleErrorStrategy* strategy = 0 ) const;

      /** @brief Update error of a ROT without changing the drift radius
	  @param DCT reference to the Muon::MdtDriftCircleOnTrack of which the sign should be updated.
	  @param pars track prediction at DCT used when using the track prediction to update the error
      @param errorlist holds the identifier of the chamber/det element and the error to be applied on the DCTs inside
	  @return New ROT with updated error. (the user must delete this object when it is no longer needed).
      */ 
      virtual const MdtDriftCircleOnTrack* updateErrorExternal( const MdtDriftCircleOnTrack& DCT,
                                                                const Trk::TrackParameters* pars = 0,
                                                                const std::map<Identifier,double>* errorlist = 0 ) const;

      /** @brief Retrieves just the MdtFullCalibData and uses this to get updated MdtRtRelation.*/
      double getErrorFromRt(const Muon::MdtDriftCircleOnTrack& DCT) const ;
      
      /** @brief Returns calibrated MdtDriftCircleOnTrack.
      Implementation of IRIO_OnTrackCreator method
      @param prd Reference to a Trk::PrepRawData object (which should always be a Muon::MdtPrepData in this case)
      @param tp Reference to the extrapolated/predicted TrackParameters at this MdtPrepData
      @return calibrated MdtDriftCircleOnTrack. Memory management is passed to user.
      */
      virtual const MdtDriftCircleOnTrack* correct( const Trk::PrepRawData& prd, 
                                                    const Trk::TrackParameters& tp,
                                                    const MuonDriftCircleErrorStrategy* strategy ) const; 

      /** @brief Base class method for correct. */
      virtual const MdtDriftCircleOnTrack* correct( const Trk::PrepRawData& prd,
                                                    const Trk::TrackParameters& tp ) const;

      /** @brief Returns the MdtDriftCircleOnTrack for a Muon::MdtDriftCircleOnTrack by comparing its time with the range of the RT relation. */
      MdtDriftCircleStatus driftCircleStatus( const MdtDriftCircleOnTrack& DCT ) const ;

      /** @brief Returns the default error strategy object */     
      const MuonDriftCircleErrorStrategy& errorStrategy() const { return m_errorStrategy; };
      
    private:

      /** struct to hold output of calibration */
      struct CalibrationOutput {
        CalibrationOutput( Trk::LocalParameters lp, //!< localParameters
          Amg::MatrixX le,     //!< localErrorMatrix
          double t,                  //!< drift time
          bool ok
          ) :  locPars(lp),locErr(le),driftTime(t),calibOk(ok) {}
        Trk::LocalParameters locPars;
        Amg::MatrixX     locErr;
        double                driftTime;
        bool                  calibOk;
      };
      
      /** preform the mdt calibration */
      CalibrationOutput getLocalMeasurement( const MdtPrepData& DC, 
                                            const Amg::Vector3D& gpos,
                                            const Amg::Vector3D* gdir,
                                            MdtCalibrationSvcInput& inputData,
                                            const MuonDriftCircleErrorStrategy* strategy = 0 ) const;
      
      /** currently returns 0. */
      double getTriggerTime() const { return 0.; }

      /** currently returns 0. */
      double getTriggerTimeTestbeam() const { return 0.; }

      // parametrised error function
      double parametrisedSigma( double r ) const;
      
      double mooreErrorStrategy(const MuonDriftCircleErrorStrategy* myStrategy, double sigmaR, const Identifier& id) const;
      double mooreErrorStrategyMC(const MuonDriftCircleErrorStrategy* myStrategy, double sigmaR, const Identifier& id) const;
      double mooreErrorStrategyLoose(const MuonDriftCircleErrorStrategy* myStrategy, double sigmaR, const Identifier& id) const;
      double mooreErrorStrategyTight(const MuonDriftCircleErrorStrategy* myStrategy, double sigmaR, const Identifier& id) const;
      
      double muonErrorStrategy(const MuonDriftCircleErrorStrategy* myStrategy, double sigmaR, const Identifier& id) const;
      
      ToolHandle<Muon::MuonIdHelperTool>   m_idHelper;
      ServiceHandle<MdtCalibrationSvc>     m_mdtCalibSvc;
      ServiceHandle<MdtCalibrationDbSvc>   m_mdtCalibDbSvc;
      ToolHandle<IMuonTofTool>             m_tofTool; //!<Time of flight tool (handle tof if not coming from IP)

      // Configuration variables
      bool                                 m_doMdt; //!< Process MDT ROTs
      int                                  m_timeCorrectionType; //!< Defined in TimingMode enum.
      //bool                                 m_scaleErrorManually; //!< if set to true errors will be scaled with 'FixedErrorScale' + FixedError
      bool                                 m_discardMaskedHits; //!< if set to true, do not create ROTs for masked hits

      // Constants for use during calculations
      double                               m_invSpeed; //!< Inverse speed of light (used in default tof correction: timing mode=0)
      double                               m_fixedError; //!< Error used when m_doFixed error =true or m_scaleErrorManually = true
      double                               m_globalToLocalTolerance; 
      
      MdtCalibrationSvcSettings*           m_mdtCalibSvcSettings; 

      // Member variables used to fill the default error strategy
      MuonDriftCircleErrorStrategy        m_errorStrategy; //!< Error strategy for created ROTs
      std::string                         m_defaultStrategy; //!< Default error strategy for the error strategy object
      bool                                m_createTubeHits; //!< if set to true, the ROT creator create 'tube' hits with a local position of 0 and an error of tube radius/sqrt(12) 
      bool                                m_scaleMdtCov; //!< Scale ROTs depending on local alignment (i.e. location in detector)
      bool                                m_doFixedError; //!< Fixed error (not tube radius) 
      bool                                m_useErrorParametrisation; //!< Use parameterised errors
      bool                                m_errorAtPredictedPosition; //!< Use the predicted track position to correct the Error. See Muon::MdtDriftCircleOnTrack::ErrorAtPredictedPosition
      bool                                m_doWireSag;  //!< if set to true, then apply wire sag corrections. Controlled via DoWireSag property
      bool                                m_stationError; //!< Add a term to the error to account for very poorly aligned stations
      bool                                m_t0Refit; //!< Add a special error to account for the T0 refit
      bool                                m_doSegments; //!< Use error strategy for segments by default
      bool                                m_doIndividualChamberReweights; //!< Deweight individual chambers
      bool                                m_isMC; //!< toggle between MC and data alignment errors (to be removed in rel. 21!)
      bool                                m_looseErrors; //!< toggle between loose errors (initial before alignment) and tight after alignment
    };

} // End of muon namewpace

#endif
