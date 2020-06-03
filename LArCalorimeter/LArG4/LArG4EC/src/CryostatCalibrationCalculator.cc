/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::EndcapCryostat::CalibrationCalculator
// Prepared 13-Jan-2004 Bill Seligman
// 20-Jul-2004 M.Leltchouk - region and volume info definitions

// This class calculates the values needed for calibration hits in the
// volumes:
// "LAr::Endcap::Cryostat::Cylinder"
// "LAr::Endcap::Cryostat::Cone"
// "LAr::Endcap::Cryostat::EmecHecLAr::Cylinder"
// "LAr::Endcap::Cryostat::EmecHecLAr::Sector"
// "LAr::Endcap::Cryostat::FcalLAr::Cylinder"
// "LAr::Endcap::Cryostat::FcalLAr::Sector"

#undef DEBUG_VOLUMES
#undef DEBUG_HITS
#undef DEBUG_MAPS
#undef DEBUG_DMXYZ

#include "CryostatCalibrationCalculator.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <map>
#include <cmath>

namespace LArG4 {

  namespace EndcapCryostat {

    ///////////////////////////////////////////////////////////
    // Tables
    ///////////////////////////////////////////////////////////

    // One day, the following information may in some sort of
    // database.  For now, it's hard-coded into this routine.

    // The information needed to determine the dead-material
    // identifier is stored as a "map of a map".  The structure looks
    // something like this:

    // Volume name -- the named defined in the endcap-cryostat
    //    detector-construction routine (make sure these names match the
    //    values in LArEndcapCryostatVisualConsultant)

    //    Copy number -- assigned in the endcap-cryostat
    //       detector-construction routine

    //       Identifier information record -- the values needed to
    //          construct the identifier for the volume.

    //          Region information record -- many large dead volumes
    //             span more than one region.  This record gives the
    //             region-specific values.


    typedef struct {
      G4int regionNumber;
      G4double etaMin;
      G4double etaMax;
      G4double deltaEta;
      G4double deltaPhi;
    } RegionInfo_t;

    typedef struct {
      G4int detector;
      G4int subdet;
      G4int type;
      G4int sampling;
      G4int numberOfRegions;
      const RegionInfo_t* regionInfoArray;
    } IdentifierInfo_t;

    typedef struct {
      G4int copyNumberLow;
      G4int copyNumberHigh;
      IdentifierInfo_t identifierInfo;
    } CopyNumberInfo_t;

    typedef struct {
      G4String volumeName;
      G4int numberOfCopies;
      const CopyNumberInfo_t* copyInfo;
    } VolumeInfo_t;

    // Please forgive the dull names, but it saves quite a lot on
    // typing.  Note how the structured hierarchy continues: there can
    // be many regions associated with one sampling (equivalent to
    // copy number); there can be many copy numbers associated with
    // the same volume name.

    // Region definitions

    // ========== Sampling 1 =============
    static const RegionInfo_t region016[] =
      // inner warm and cold walls, cones
      // region  etamin etamax deta dphi
      { { 6,   1.3,   3.2,   0.1, M_PI/32. },
        { 7,   3.2,   5.0,   0.1, M_PI/32. } };

    static const RegionInfo_t region017[] =
      // inner warm and cold walls, cones
      // region  etamin etamax deta dphi
      { { 7,   3.2,   5.0,   0.1, M_PI/32. } };


    // ========== Sampling 2 =============
    static const RegionInfo_t region025[] =
      // pieces at largest radius of inner warm and cold walls (info5)
      // and CREK7 - front part of HEC supports (info7)
      // region  etamin etamax deta dphi
      { { 5,   1.0,   1.7,   0.1, M_PI/32. } };

    // ========== Sampling 3 =============
    static const RegionInfo_t region030[] =
      // warm and cold walls, shields behind HEC, FCAL (info, info8)
      // region  etamin etamax deta dphi
      { { 0,   1.7,   5.0,   0.1, M_PI/32. },
        { 1,   5.0,   8.0,   0.2, M_PI/32. }
      };

    static const CopyNumberInfo_t info5[] =
      // "LAr::Endcap::Cryostat::Cylinder"
      {
        // ------------- region: inner warm and cold walls  ------------
        // copy number range (low, high)
        {  4, 6,
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   1,
             sizeof(region016)/sizeof(RegionInfo_t),
             region016 }
        },
        { 15, 15,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   1,
            sizeof(region016)/sizeof(RegionInfo_t),
            region016 }
        },
        { 29, 29,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   1,
            sizeof(region016)/sizeof(RegionInfo_t),
            region016 }
        },

        // ------------- region: outer warm - cold piece  ------------
        // copy number range (low, high)
        { 11, 11,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   2,
            sizeof(region025)/sizeof(RegionInfo_t),
            region025 }
        },
        // ----- region: warm and cold walls, shields behind HEC, FCAL
        {  8, 8,
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   3,
             sizeof(region030)/sizeof(RegionInfo_t),
             region030 }
        },
        { 20, 21,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   3,
            sizeof(region030)/sizeof(RegionInfo_t),
            region030 }
        },
        // ------------- region: warm and cold walls behind HEC, FCAL ----
        // copy number range (low, high)
        { 25, 26,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   3,
            sizeof(region030)/sizeof(RegionInfo_t),
            region030 }
        },
	// ------------- region: warm and cold walls behind HEC, FCAL ----
	// copy number range (low, high)
 	{ 200, 201, // cold back wall of endcap cryostat
	   // Det  Sub  Typ  Sam
	   { 10,   4,   1,   3, 
	     sizeof(region030)/sizeof(RegionInfo_t),
	     region030 } 
	},
      };

    static const CopyNumberInfo_t info6[] =
      // "LAr::Endcap::Cryostat::Cone"
      {
        // ------------- region: inner warm and cold walls  ------------
        // copy number range (low, high)
        {  1, 3,
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   1,
             sizeof(region017)/sizeof(RegionInfo_t),
             region017 }
        },
      };

    static const CopyNumberInfo_t info7[] =
      // "LAr::Endcap::Cryostat::EmecHecLAr::Cylinder"
      {
        // ------------- region: warm and cold walls behind HEC, FCAL ----
        // copy number range (low, high)
        { 17, 18,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   3,
            sizeof(region030)/sizeof(RegionInfo_t),
            region030 }
        },
        { 22, 22,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   3,
            sizeof(region030)/sizeof(RegionInfo_t),
            region030 }
        },
        { 23, 23,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   2,
            sizeof(region025)/sizeof(RegionInfo_t),
            region025 }
        },
        { 24, 24,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   3,
            sizeof(region030)/sizeof(RegionInfo_t),
            region030 }
        },
        { 30, 30,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   3,
            sizeof(region030)/sizeof(RegionInfo_t),
            region030 }
        },
      };

    static const CopyNumberInfo_t info8[] =
      // "LAr::Endcap::Cryostat::EmecHecLAr::Sector"
      {
        // ------------- region: inner warm and cold walls  ------------
        // copy number range (low, high)
        {  7, 7,
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   2,
             sizeof(region025)/sizeof(RegionInfo_t),
             region025 }
        },
      };

    //    static const CopyNumberInfo_t info9[] =
    //    // "LAr::Endcap::Cryostat::FcalLAr::Cylinder"
    //      {
    //	// ------------- region: warm and cold walls behind HEC, FCAL ----
    //	// copy number range (low, high)
    //      { 25, 26,
    //         // Det  Sub  Typ  Sam
    //         { 10,   4,   1,   3,
    //           sizeof(region030)/sizeof(RegionInfo_t),
    //           region030 }
    //	},
    //      };
    //
    //    static const CopyNumberInfo_t info10[] =
    //    // "LAr::Endcap::Cryostat::FcalLAr::Sector"
    //      {
    //	// ------------- region: warm and cold walls behind HEC, FCAL ----
    //	// copy number range (low, high)
    //      {  5,  8,
    //         // Det  Sub  Typ  Sam
    //         { 10,   4,   1,   3,
    //           sizeof(region030)/sizeof(RegionInfo_t),
    //           region030 }
    //	},
    //      };

    static const VolumeInfo_t volume5 =
      { "LAr::Endcap::Cryostat::Cylinder",
        sizeof(info5) / sizeof(CopyNumberInfo_t),
        info5
      };

    static const VolumeInfo_t volume6 =
      { "LAr::Endcap::Cryostat::Cone",
        sizeof(info6) / sizeof(CopyNumberInfo_t),
        info6
      };

    static const VolumeInfo_t volume7 =
      { "LAr::Endcap::Cryostat::EmecHecLAr::Cylinder",
        sizeof(info7) / sizeof(CopyNumberInfo_t),
        info7
      };

    static const VolumeInfo_t volume8 =
      { "LAr::Endcap::Cryostat::EmecHecLAr::Sector",
        sizeof(info8) / sizeof(CopyNumberInfo_t),
        info8
      };

    //    static const VolumeInfo_t volume9 =
    //      { "LAr::Endcap::Cryostat::FcalLAr::Cylinder",
    //	sizeof(info9) / sizeof(CopyNumberInfo_t),
    //	info9
    //      };

    //    static const VolumeInfo_t volume10 =
    //      { "LAr::Endcap::Cryostat::FcalLAr::Sector",
    //	sizeof(info10) / sizeof(CopyNumberInfo_t),
    //	info10
    //      };

    static const VolumeInfo_t volumes[] = { volume5, volume6, volume7, volume8};
    static const G4int numberOfVolumes = sizeof(volumes) / sizeof(VolumeInfo_t);

    typedef std::map < G4int, const IdentifierInfo_t* > identifierMap_t;
    typedef identifierMap_t::iterator                   identifierMap_ptr_t;
    typedef std::map < G4String, identifierMap_t > volumeMap_t;
    typedef volumeMap_t::iterator                  volumeMap_ptr_t;
    static volumeMap_t volumeMap; // used as const after initialization

    static G4bool notInitialized = true;  // used as const after initialization

    ///////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////

    CalibrationCalculator::CalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_backupCalculator("EndcapCryostatCalibrationLArCalculator",name)
    {
      declareProperty("BackupCalculator",m_backupCalculator);
    }

    StatusCode CalibrationCalculator::initialize() {
      // Get a "backup" calculator.

      ATH_CHECK(m_backupCalculator.retrieve());

#if defined (DEBUG_HITS) || defined (DEBUG_MAPS)
      G4cout << "LArG4::EndcapCryostat::CalibrationCalculator::CalibrationCalculator - "
             << "   notInitialized="
             << notInitialized
             << ", numberOfVolumes=" << numberOfVolumes
             << ", sizeof(volumes)=" << sizeof(volumes)
             << ", sizeof(VolumeInfo_t)=" << sizeof(VolumeInfo_t)
             << G4endl;
#endif

      // Intialize the maps.
      if ( notInitialized )
        {
          notInitialized = false;

          // For each volume managed by this calculator...
          for (G4int v = 0; v != numberOfVolumes; v++)
            {
              const VolumeInfo_t& volume = volumes[v];

#if defined (DEBUG_HITS) || defined (DEBUG_MAPS)
              G4cout << "LArG4::EndcapCryostat::CalibrationCalculator::CalibrationCalculator - "
                     << "   volume '"
                     << volume.volumeName
                     << "' has number of entries=" << volume.numberOfCopies
                     << G4endl;
#endif

              identifierMap_t identifierMap;
              const CopyNumberInfo_t* copyInfo = volume.copyInfo;

              // For each range copy numbers that can be found in a volume with this name...
              for (G4int c = 0; c != volume.numberOfCopies; c++, copyInfo++)
                {
                  // For each copy in the range of copy numbers
                  for (G4int copy = copyInfo->copyNumberLow;
                       copy <= copyInfo->copyNumberHigh;
                       copy++)
                    {
#if defined (DEBUG_HITS) || defined (DEBUG_MAPS)
                      G4cout << "LArG4::EndcapCryostat::CalibrationCalculator::CalibrationCalculator - "
                             << "   adding copy=" << copy
                             << " to identifierMap"
                             << G4endl;
#endif
                      // Add to the map that's based on copy number.
                      identifierMap[copy] = &(copyInfo->identifierInfo);
                    }
                }

              // Add to the map that's based on volume name; it
              // contains maps based on copy number.
              volumeMap[volume.volumeName] = identifierMap;

#if defined (DEBUG_HITS) || defined (DEBUG_MAPS)
              G4cout << "LArG4::EndcapCryostat::CalibrationCalculator::CalibrationCalculator - "
                     << "   volume '"
                     << volume.volumeName
                     << "' added to map."
                     << G4endl;
#endif

            }
        } // if not initialized

#if defined (DEBUG_HITS) || defined (DEBUG_MAPS)
      G4cout << "LArG4::EndcapCryostat::CalibrationCalculator::CalibrationCalculator - "
             << G4endl
             << "   initialization complete; map size="
             << volumeMap.size()
             << G4endl;
#endif

      return StatusCode::SUCCESS;
    }// initialize


    CalibrationCalculator::~CalibrationCalculator()
    {
      // Clean up pointers.
      //delete m_backupCalculator;
      //m_backupCalculator = 0;
    }

    G4bool CalibrationCalculator::Process (const G4Step* a_step,
                                           LArG4Identifier & _identifier,
                                           std::vector<G4double> & _energies,
                                           const eCalculatorProcessing a_process) const
    {
      // Use the calculators to determine the energies and the
      // identifier associated with this G4Step.  Note that the
      // default is to process both the energy and the ID.

      _energies.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyEnergy )
        {
          m_energyCalculator.Energies( a_step, _energies );
        } else {
        for (unsigned int i=0; i != 4; i++) _energies.push_back( 0. );
      }

      _identifier.clear();
      if ( a_process == kEnergyAndID  ||  a_process == kOnlyID )
        {
          // Calculate the identifier.

          // First, find the physical volume copy number, and the
          // logical volume name.

          const G4VPhysicalVolume* physical = a_step->GetPreStepPoint()->GetPhysicalVolume();
          const G4int copyNumber = physical->GetCopyNo();
          G4String volumeName = physical->GetLogicalVolume()->GetName();

          if(volumeName.index("LArMgr::") == 0) volumeName.erase(0,8);


#ifdef DEBUG_HITS
          G4cout << "LArG4::EndcapCryostat::CalibrationCalculator::Process - "
                 << G4endl
                 << "   searching for volume '"
                 << volumeName
                 << "' copyNumber="
                 << copyNumber
                 << G4endl;
#endif

          // Search the maps for this volume/copy number combination.
          volumeMap_ptr_t v = volumeMap.find( volumeName );
          if ( v != volumeMap.end() )
            {

              // Recall that maps are made from pair <key,value>, and
              // you access a pair with the "first" and "second"
              // members.  In this case, "second" is a map.  We don't
              // need to copy the entire map; it's enough to get its
              // reference.

              identifierMap_t& identifierMap = (*v).second;
              identifierMap_ptr_t i = identifierMap.find( copyNumber );

              if ( i != identifierMap.end() )
                {
                  const IdentifierInfo_t* info = (*i).second;

                  // Find our (x,y,z) location from the G4Step.

                  const G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
                  const G4StepPoint* post_step_point = a_step->GetPostStepPoint();
                  const G4ThreeVector startPoint = pre_step_point->GetPosition();
                  const G4ThreeVector endPoint   = post_step_point->GetPosition();
                  const G4ThreeVector p = (startPoint + endPoint) * 0.5;

                  // Determine the geometric eta and phi.  Note that we do not
                  // adjust for any endcap shifts; the values are assigned to
                  // the volumes based on their actual positions

                  G4double eta = fabs( p.pseudoRapidity() );
                  G4double phi = p.phi();
                  // For this calculation, we need 0<phi<2*PI.  (The
                  // official ATLAS standard of -PI<phi<PI is
                  // meaningless here.)
                  if ( phi < 0 ) phi += 2*M_PI;

                  // The region can depend on eta.  Search through the
                  // list of regions within this IdentifierInfo record
                  // to find which one has etaMin<eta<etaMax.

                  const G4int regions = info->numberOfRegions;
                  const RegionInfo_t* region = info->regionInfoArray;

#ifdef DEBUG_HITS
                  G4cout << "LArG4::EndcapCryostat::CalibrationCalculator::Process - "
                         << G4endl
                         << "   found volume, number of regions="
                         << regions
                         << " eta=" << eta << " phi=" << phi
                         << G4endl;
#endif

                  G4int r;
                  for ( r = 0; r < regions; r++, region++ )
                    {
                      if ( copyNumber!=8 && region->etaMax == 5.0 && eta > region->etaMax) {
                        // g.p. 3-Apr-2006
                        // region is defined only until 5.0, but we may have hits
                        // from higher eta. Lets put them into last bin
                        eta = region->etaMax - 0.00001;
                        break;
                      }
                      // hack for strongly missaligned geometry, 2-Oct-2007, g.p.
                      if(region == &region025[0] && copyNumber == 11 && volumeName == "LAr::Endcap::Cryostat::Cylinder" && eta>=region->etaMax) {
                        eta = region->etaMax - 0.00001;
                      }
                      if ( eta > region->etaMin  &&  eta < region->etaMax ) break;
                    }

                  // If the following statement is not true, we're in
                  // trouble.  It means that the eta of our energy
                  // deposit is outside the assumed eta limits of the
                  // volume.

                  if ( r < regions )
                    {
                      // Convert eta and phi to their integer equivalents for the
                      // identifier.

                      const G4int etaInteger = G4int( (eta - region->etaMin) / region->deltaEta );
                      G4int phiInteger = G4int( phi / region->deltaPhi );
                      // to handle rounding error problem
                      if(phiInteger > 63) phiInteger = 0;

#ifdef DEBUG_HITS
                      G4cout << "LArG4::EndcapCryostat::CalibrationCalculator::Process - "
                             << G4endl
                             << "   found region="
                             << region->regionNumber
                             << " eta bin=" << etaInteger << " phi bin=" << phiInteger
                             << G4endl;
#endif

                      // The sign of subdet will depend on whether z is positive
                      // or negative.
                      G4int subDetSign = 1;
                      if ( p.z() < 0 ) subDetSign = -1;

                      // A quick check against a bad value of etaMin.
                      if ( etaInteger >= 0 ) {
                        // Build the full identifier.
                        _identifier << info->detector
                                    << info->subdet * subDetSign
                                    << info->type
                                    << info->sampling
                                    << region->regionNumber
                                    << etaInteger
                                    << phiInteger;

                      } // etaInteger valid
                    } // eta valid
                } // copy number valid
            } // volume name valid
          if ( _identifier == LArG4Identifier() )
            {
              // g.p. 09/05/2006
#ifdef DEBUG_DMXYZ
              LArG4::CalibrationDefaultCalculator::Print("UNEXP LArG4EC/CryostatCalibrationCalculator",_identifier,a_step);
#endif
#if defined (DEBUG_HITS) || defined (DEBUG_VOLUMES)
              static const G4int messageMax = 1000;
              static G4int messageCount = 0; // exists only if debug activated
              if ( messageCount++ < messageMax ) {
                G4ThreeVector p = (a_step->GetPreStepPoint()->GetPosition() + a_step->GetPostStepPoint()->GetPosition()) * 0.5;
                std::cout << "LArG4::EndcapCryostat::CalibrationCalculator::Process" << std::endl
                          << "   G4Step in volume '" << volumeName
                          << "' copy# " << copyNumber
                          << " not found on tables, using backup calculator;"
                          << " xyz:"<<p.x()<<", "<<p.y()<<", "<<p.z()
                          << " eta:"<< p.pseudoRapidity()
                          << " (error " << messageCount << " of " << messageMax << " max displayed)."
                          << std::endl;
              }
#endif
            }
        } // calculate identifier

      // What if we haven't implemented the code for a given volume?
      // Use a "backup" calculator to try to process the step.

      if ( _identifier == LArG4Identifier() )
        {
          //m_backupCalculator->Process(a_step, kOnlyID);
          //_identifier = m_backupCalculator->identifier();
          std::vector<G4double> _tmpv;
          m_backupCalculator->Process(a_step, _identifier, _tmpv, kOnlyID);
        }

#ifdef DEBUG_HITS
      //      G4double energy = accumulate(_energies.begin(),_energies.end(),0.);
      std::cout << "LArG4::EndcapCryostat::CalibrationCalculator::Process"
                << " ID=" << std::string(_identifier)
        //        << " energy=" << energy
                << " energies=(" << _energies[0]
                << "," << _energies[1]
                << "," << _energies[2]
                << "," << _energies[3] << ")"
                << std::endl;
#endif

      // g.p. 09/05/2006
#ifdef DEBUG_DMXYZ
      LArG4::CalibrationDefaultCalculator::Print("DMXYZ LArG4EC/CryostatCalibrationCalculator",_identifier,a_step);
#endif
      // Check for bad result.
      if ( _identifier == LArG4Identifier() ) return false;
      return true;
    }

  } // namespace EndcapCryostat

} // namespace LArG4
