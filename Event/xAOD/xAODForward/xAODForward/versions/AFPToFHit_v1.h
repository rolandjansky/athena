/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODFORWARD_VERSIONS_AFPToFHIT_V1_H
#define XAODFORWARD_VERSIONS_AFPToFHIT_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"
#include "xAODForward/AFPStationID.h"

// needed for forward declaration
#include "AthContainers/DataVector.h"


namespace xAOD {

  /// @brief Class representing a hit in ToF detector.
  ///
  /// This class provides information about a single hit in AFP ToF
  /// detector.
  class AFPToFHit_v1 : public SG::AuxElement
  {
  public:
    // needed by athenaRoot 
    AFPToFHit_v1();

    /// @brief Time of AFP ToF detector hit relative to the reference clock
    ///
    /// Time between the signal in AFP ToF detector and reference
    /// clock tick. The reference clock runs with the 40 MHz rate
    /// which corresponds to LHC bunch spacing so there should be a
    /// clock tick that zeros the time measurement for every bunch
    /// crossing. Time is expressed in nanoseconds.
    float time() const;

    void setTime(float time);

    /// @brief HPTDC channel number
    ///
    /// Information about mapping and more details about High
    /// Performance/Precision Time to Digital Converter (HPTDC) are
    /// available at
    /// https://twiki.cern.ch/twiki/bin/view/Atlas/AFPHPTDC
    int  hptdcChannel() const;

    void setHptdcChannel(int hptdcChannel);

    /// @brief ID number of the LQBar in a train where hit was detected
    ///
    /// The value should be between 0 and 3
    int barInTrainID() const;

    void setBarInTrainID(const int barInTrainID);

    /// @brief ID number of the train of LQBars (set of 4 bars), where hit was detected
    ///
    /// The value should be between 0 and 3
    int trainID() const;

    void setTrainID(const int barInTrainID);

    /// @brief Lenght of the signal pulse which is proportional to the signal
    /// amplitude
    ///
    /// The returned value is roughly lenght of the signal above
    /// threshold expressed in nanoseconds.
    float pulseLength() const;

    void setPulseLength(float pulseLength);

    /// Identification number of the HPTDC module.
    ///
    /// Detailed documentation of High Performance/Precision Time to
    /// Digital Converter (HPTDC) is available in
    /// https://twiki.cern.ch/twiki/bin/view/Atlas/AFPHPTDC
    int hptdcID() const;
    
    void setHptdcID(int hptdcID);

    /// @brief Index of the station with time-of-flight hit
    ///
    /// The numbering scheme is the same as in xAOD::AFPStationID. It
    /// is recommended to use this class instead of bare numbers.
    int stationID() const;

    void setStationID(int side);

    /// Returns true if ToF hit is in far station on A side
    bool isSideA() const
    { return stationID() == xAOD::AFPStationID::farA;}

    /// Returns true if ToF hit is in far station on A side
    bool isSideC() const
    { return stationID() == xAOD::AFPStationID::farC;}

    /// Function making sure that the object is ready for persistification i.e. saving - does nothing
    void toPersistent() {}
  };


} // namespace xAOD

// Declare the inheritance of the type to StoreGate:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPToFHit_v1, SG::AuxElement );

#endif // XAODFORWARD_VERSIONS_AFPToFHIT_V1_H
