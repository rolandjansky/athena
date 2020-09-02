/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONTOOL_H
#define MDTCALIBSVC_MDTCALIBRATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/IInterface.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/MdtDriftCircleStatus.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MdtCalibSvc/MdtCalibrationDbTool.h"

#include <string>

class MdtCalibHit;
class MdtCalibrationSvcInput;
class MdtCalibrationSvcSettings;

namespace MuonCalib {
  class MdtRtRelation;
}

/**
   @class MdtCalibrationTool
   the Mdt Calib Service provides, on request, the drift radius and its 
   error, computed applying the proper calibration, for any hit in Mdt chambers  
   @author Martin Woudstra, Niels van Eldik
*/

const static InterfaceID s_iID("MdtCalibrationTool", 1, 0);

class MdtCalibrationTool : public extends<AthAlgTool, IInterface> {
public:
  /** constructor */
  MdtCalibrationTool(const std::string& type, const std::string &name, const IInterface* parent);

  /** destructor */
  virtual ~MdtCalibrationTool();

  /** implements IInterface */
  static const InterfaceID &interfaceID() {
    return s_iID;
  }

  /** initialization */
  virtual StatusCode initialize();

  /** Convert the raw MDT time (+charge) into a drift radius + error.
      It returns whether the conversion was successful.
       
      @param[in,out] hit Hit must have pointer set to the MdtDigit,
      as well as the global hit position (including the position along the tube!)
      @param[in] signedTracklength the track length from the 'triggerpoint' to the hit.
      It is used for the time-of-flight correction.
      This triggerpoint is the I.P. for ATLAS p-p collisions,
      typically scintillators in test-beam and cosmic teststands,
      and not clear yet what is it is for cosmics in ATLAS.
      The sign is for determining the sign of the time-of-flight correction.
      If a muon first passes the triggerpoint, and then the MDT tube, the sign
      should be positive (the case for ATLAS p-p and H8 test-beam).
      If a muon first passes the MDT tube, and then de triggerpoint, the sign
      should be negative (typically the case for cosmic-ray teststands).
      @param[in] triggerTime the time of the 'triggerpoint' in ns. This is the time (measured with
      the same clock as the MDT TDC's) when the muon passed a known point in
      space: the 'triggerpoint'. 
      For ATLAS this is 0.0 since the TDC's are synchonised w.r.t. the bunch-crossings.
      For H8 test-beam it is the trigger time, which is time when the muon passed
      the trigger scintillators. For cosmic-ray teststands it is usually also the
      time when the muon passed the trigger scintillators.
      For cosmics in ATLAS it is not clear yet.
      @param[in] resolFromRtrack indicates the method to provide the resolution as a function 
      of the distance of the reconstructed track from the anode wire instead of the drift
      radius
  */
  bool driftRadiusFromTime( MdtCalibHit &hit, double signedTrackLength, double triggerTime = 0.0, bool resolFromRtrack=false ) const;

  /** Convert the raw MDT time (+charge) into a drift radius + error.
      It returns whether the conversion was successful.
       
  */
  bool driftRadiusFromTime( MdtCalibHit &hit, const MdtCalibrationSvcInput &inputData, const MdtCalibrationSvcSettings &settings, bool resolFromRtrack=false  ) const;

  /** Convert the raw MDT time (+charge) into a drift radius + error.
      It returns whether the conversion was successful.
       
  */
  bool driftRadiusFromTime( MdtCalibHit &hit, const MdtCalibrationSvcInput &inputData, bool resolFromRtrack=false  ) const;

  /** TDC bin size. 25/32 ns for all MDT/sMDT, except BMG=0.2ns */
  double tdcBinSize(const Identifier &id) const;

  /** Convert the raw MDT times of two twin hits into a Twin position (coordinate along tube)
      It returns whether the conversion was successful. */
  bool twinPositionFromTwinHits( MdtCalibHit &hit, MdtCalibHit &twinhit, double signedTrackLength, double twinSignedTrackLength, bool &twinDigitIsPrompt, double triggerTime = 0.0 ) const;
   
  /** Convert the raw MDT times of two twin hits into a Twin position (coordinate along tube)
      It returns whether the conversion was successful. */
  bool twinPositionFromTwinHits( MdtCalibHit &hit, MdtCalibHit &twinhit, const MdtCalibrationSvcInput &inputData, const MdtCalibrationSvcInput &twinInputData, const MdtCalibrationSvcSettings &settings, bool &twinDigitIsPrompt ) const;
   
  /** Return status of drift time: in window, above/below window (with upper/lower bounds described by the settings) or invalid.
      @return @c Muon::MdtDriftCircleStatus saying whether the drift time comes before, during or after the bounds of the drift time spectrum.
      @param[in] driftTime is the drift time of the hit.
      @param[in] rtRelation is used to obtain the upper and lower limits of the r(t) relation.
      @param[in] settings define the extra window around the r(t) relationship bounds that is acceptable.
  */
  Muon::MdtDriftCircleStatus driftTimeStatus( double driftTime, const MuonCalib::MdtRtRelation *rtRelation, const MdtCalibrationSvcSettings &settings ) const;
   
private:

  /// please don't add any data members here!!
  /// they should be added to Imp to keep the class free from data exposed to clients
  class Imp;
  std::unique_ptr<Imp> m_imp;

  ToolHandle<MdtCalibrationDbTool> m_dbTool{this,"CalibrationDbTool","MdtCalibrationDbTool"};

  // Read handle for conditions object to get the field cache
  // If one wants to avoid that adding of this read handle here, then client tools/algs calling driftRadiusFromTime
  // must implement this to get the AtlasFieldCache which can then be passed through the call to driftRadiusFromTime
  // Note: a readhandle must be in a tool or an alg, and so it cannot be in the class Imp.)
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

};

#endif
