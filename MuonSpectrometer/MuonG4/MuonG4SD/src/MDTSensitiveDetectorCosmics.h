/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @class MDTSensitiveDetectorCosmics
	@author daniela.rebuzzi@pv.infn.it
	@author nveldik@nikhef.nl

    @section MDTSensitiveDetectorCosmics Class methods and properties
    
The method MDTSensitiveDetectorCosmics::ProcessHits is executed by the G4 kernel each
time a charged particle (or a geantino) crosses one of the MDT Sensitive Gas
volumes.

Once a G4Step is perfomed by the particle in the sensitive volume (both when
the particle leaves the tube or stops in it), Pre and
PostStepPositions are tranformed into local coordinates (chamber reference
system, with Z along the tube direction and XY tranversal plane) and used to
calculate the local direction of the track. 

Two cases are given:
1) the particle over-passed the wire: here the drift radius (the impact 
   parameter) is computed as the local direction distance from the wire;  
2) the step doesn't allow the particle to over-pass the wire: here the shortest 
   distance to the wire, which is one of the two
   end-points, is calculated for each step which occurs inside the sensitive 
   volumes and only the closer one is kept; this case includes also the hard
   scattering case inside the sensitive volume.

Navigating with the touchableHistory method GetHistoryDepth()
through the hierarchy of the volumes crossed by the particles, the 
MDTSensitiveDetectorCosmics determinates the
correct set of geometry parameters to be folded in the Simulation Identifier
associated to each hit. The MDT SimIDs are 32-bit unsigned integers, built 
using the MuonSimEvent/MdtHitIdHelper class
which inherits from the MuonHitIdHelper base class. 

The modified version (Daniela Rebuzzi, 16/12/2005) calculates the tof for the
cosmics, using as t0 the time to the closest approach of the track to (0,0,0).
This implementation works also for slow particles, since the discrimination
physics event/cosmics is done relying on the G4Track vertex (not on the tof). 

We describe in the following, how each field of the identifier is retrieved.

1) stationName, stationEta, stationPhi: when a volume is found in the hierarchy,
   whose name contains the substring "station", the stationName is extracted from
   the volume's name; stationPhi and stationEta are calculated starting from the
   volume copy number, assigned by MuonGeoModel.

2) multilayer: when a volume is found in the hierarchy,
   whose name contains the substring "component", the multilayer is set to 1 or to
   2, according to the component number (multilayer=1 if the component is 1, 5 or
   8; multilayer=2 if the component is 3, 7, 8, 10, 11 or 14).

3) tubeLayer and tube: when a volume is found in the hierarchy,
   whose name contains the substring "Drift", tubeLayer and tube are calculated 
   starting from the Drift volume copy number.

    @section Some notes:

1) this implementation of the MDT Sensitive Detectors can handle different
   situations: hard scattering of the muon on the sensitive volume (its direction
   changes), soft secondary particles completely included in the sensitive volume,
   muon hits masked by secondaries, like delta rays.

2) for each hit, the time of flight (the G4 globalTime), is recorded and
   associated to the hit.

3) more than none MDT hit can occur in the same tube. The hit selection is done
   at the level of the digitization procedure.

4) the MDTHit object contains: the SimID, the drift radius and the globalTime.


*/

#ifndef MDTSensitiveDetectorCosmics_H
#define MDTSensitiveDetectorCosmics_H

#include "G4VSensitiveDetector.hh"
#include "StoreGate/WriteHandle.h"
#include "MuonSimEvent/MDTSimHitCollection.h"

class G4TouchableHistory;
class MdtHitIdHelper;
class MDTSimHit;

class MDTSensitiveDetectorCosmics : public G4VSensitiveDetector {
    
public:
    /** construction/destruction */
    MDTSensitiveDetectorCosmics(const std::string& name, const std::string& hitCollectionName, const unsigned int nTubesMax);
    ~MDTSensitiveDetectorCosmics()=default;
    
    /** member functions */
    void   Initialize(G4HCofThisEvent* HCE) override final;
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist) override final;
    
private:
    int  GetIdentifier(G4TouchableHistory* touchHist);
    Amg::Vector3D m_mom;           
    double m_momMag;           
    Amg::Vector3D m_vertex; 
    Amg::Vector3D m_currVertex;          
    Amg::Vector3D m_globH;

    /** member data */
    SG::WriteHandle<MDTSimHitCollection> m_MDTHitColl;
    MdtHitIdHelper*            m_muonHelper;

    double                     m_driftRadius;
    double                     m_globalTime;
    Amg::Vector3D              m_localPosition;
    
    /** radius assigned to radius if radius is invalid */
    double m_DEFAULT_TUBE_RADIUS;

};

#endif
