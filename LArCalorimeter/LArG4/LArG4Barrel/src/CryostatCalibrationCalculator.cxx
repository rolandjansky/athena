/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4::BarrelCryostat::CalibrationCalculator
// Prepared 06-Jul-2004 Bill Seligman

// This class calculates the values needed for calibration hits in the
// simulation.
//
//
// G. Pospelov (8-Fev-2006) ==>
// In 11.0.X geometry of barrel cryostat was changed:
// "Many of the individual cylinders and cones which were separate logical
// volumes have become one single piece." (c) J. Boudreau.
// Names of volumes were changed also. So original code was changed to adopt
// calculator to the new geometry. It's temporary solution, somedays db
// oriented version will be written.
//
// This calculator is intended to apply to the following volumes
// LArMgr::LAr::Barrel::Cryostat::Cylinder::*
// LArMgr::LAr::Barrel::Cryostat::InnerWall
// LArMgr::LAr::Barrel::Cryostat::Sector::*


#define DEBUG_VOLUMES
#undef DEBUG_HITS
#undef DEBUG_MAPS

#include "CryostatCalibrationCalculator.h"
#include "CryostatCalibrationLArCalculator.h"

#include "LArG4Code/LArG4Identifier.h"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <map>
#include <algorithm>
#include <cmath>

namespace LArG4 {

  namespace BarrelCryostat {

    ///////////////////////////////////////////////////////////
    // Tables
    ///////////////////////////////////////////////////////////

    // There are two ways to implement a routine like this:

    // 1) Lots and lots of "if" statements.

    // 2) Try to encapsulate the decision-making process in tables.

    // I chose the latter approach.  The advantage of (2) is that it
    // formalizes a lot of rules into a relatively small set of
    // tables; the disadvantage is that the code can be complicated.
    // (The class CryostatCalibrationLArCalculator demonstrates the
    // type (1) approach.)

    // One day, the following information may in some sort of
    // database.  For now, it's hard-coded into this routine.

    // The information needed to determine the dead-material
    // identifier is stored as a "map of a map".  The structure looks
    // something like this:

    // Volume name -- the named defined in the barrel-cryostat
    //    detector-construction routine (make sure these names match the
    //    values in LArBarrelCryostatVisualConsultant)

    //    Copy number -- assigned in the barrel-cryostat
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
    // be many regions associated with one sampling (equivalent to a
    // range of copy numbers); there can be many ranges of copy
    // numbers associated with the same volume name.

    // Region definitions

    // ========== Sampling 1 =============
    static const RegionInfo_t region010[] =
      // inner warm wall and solenoid
      // region  etamin etamax deta dphi
      { { 0,   0.0,   1.5,   0.1, M_PI/32. },
        { 4,   1.5,   1.6,   0.1, M_PI/32. },
        { 5,   1.5,   1.8,   0.1, M_PI/32. },
        { 6,   1.3,   3.2,   0.1, M_PI/32. } };

#if 0
    static const RegionInfo_t region011[] =
      // inner cold wall
      // region  etamin etamax deta dphi
      { { 1,   0.0,   1.5,   0.1, M_PI/32. },
        { 4,   1.5,   1.6,   0.1, M_PI/32. },
        { 5,   1.5,   1.8,   0.1, M_PI/32. } };
#endif

    static const RegionInfo_t region012[] =
      // inner warm wall, cones, cables and services in front of EMEC
      // region  etamin etamax deta dphi
      { { 4,   1.5,   1.6,   0.1, M_PI/32. },
        { 5,   1.5,   1.8,   0.1, M_PI/32. },
        { 6,   1.3,   3.2,   0.1, M_PI/32. } };

    // ========== Sampling 2 =============
    static const RegionInfo_t region020[] =
      // outer warm and cold walls in front of Tile-barrel
      // region  etamin etamax deta dphi
      { { 0,   0.0,   1.0,   0.1, M_PI/32. },
        { 2,   1.0,   1.5,   0.1, M_PI/32. } };

    static const RegionInfo_t region021[] =
      // outer warm and cold walls behind of Tile-barrel but
      // in front of Tile-extended-barrel, cables and services
      // region  etamin etamax deta dphi
      { { 1,   0.0,   1.0,   0.1, M_PI/32. },
        { 2,   1.0,   1.5,   0.1, M_PI/32. } };

    // Region->Volume, based on a range of copy numbers.  Note that
    // some "ranges" of copy numbers contain only one volume.

    static const CopyNumberInfo_t info1[] =
      {
        // ------------- region inner warm wall and solenoid ------------
        // copy number range (low, high)
        //	{  1, 1,
        //         // Det  Sub  Typ  Sam
        //         { 10,   4,   1,   1,
        //           sizeof(region010)/sizeof(RegionInfo_t),
        //           region010 }
        //	},
        // copy number range (low, high)
        {  56, 60,
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   1,
             sizeof(region010)/sizeof(RegionInfo_t),
             region010 }
        },
        // ------------------ region inner cold wall, Cyl -----------------
        // copy number range (low, high)
        //	{  4, 5,
        //         // Det  Sub  Typ  Sam
        //         { 10,   4,   1,   1,
        //           sizeof(region011)/sizeof(RegionInfo_t),
        //           region011 }
        //	},
        // copy number range (low, high)
        //	{  55, 55,
        //         // Det  Sub  Typ  Sam
        //         { 10,   4,   1,   1,
        //           sizeof(region011)/sizeof(RegionInfo_t),
        //           region011 }
        //	},
        // copy number range (low, high)
        //	{  61, 61,
        //         // Det  Sub  Typ  Sam
        //         { 10,   4,   1,   1,
        //           sizeof(region011)/sizeof(RegionInfo_t),
        //           region011 }
        //         // 84 bolts inside this cylinder have the same name
        //         // and the same copy#=61 => Edeposits in bolts are collected
        //
        //	},
        // --- region: inner warm walls, cables and services in front of EMEC
        // copy number range (low, high)
        //	{  17, 19,
        //         // Det  Sub  Typ  Sam
        //         { 10,   4,   1,   1,
        //           sizeof(region012)/sizeof(RegionInfo_t),
        //           region012 }
        //	},
        //	{  50, 52,
        //         // Det  Sub  Typ  Sam
        //         { 10,   4,   1,   1,
        //           sizeof(region012)/sizeof(RegionInfo_t),
        //           region012 }
        //	},

        // ---------------- outer warm and cold walls, cables and services
        // copy number range (low, high)
        //	{  3, 3,
        //         // Det  Sub  Typ  Sam
        //         { 10,   4,   1,   2,
        //           sizeof(region021)/sizeof(RegionInfo_t),
        //           region021 }
        //	},
        {  6, 6,
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   2,
             sizeof(region020)/sizeof(RegionInfo_t),
             region020 }
        },
        {  7, 10,
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   2,
             sizeof(region021)/sizeof(RegionInfo_t),
             region021 }
        },
        {  11, 11,
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   2,
             sizeof(region020)/sizeof(RegionInfo_t),
             region020 }
        },
        {  12, 12,
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   2,
             sizeof(region021)/sizeof(RegionInfo_t),
             region021 }
        },
        {  14, 16,
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   2,
             sizeof(region021)/sizeof(RegionInfo_t),
             region021 }
        },
        //	{  48, 48,
        //         // Det  Sub  Typ  Sam
        //         { 10,   4,   1,   2,
        //           sizeof(region021)/sizeof(RegionInfo_t),
        //           region021 }
        //	},
        // --------------- Half::Cylinder (Old name, info4 was moved to here)
        // --- placed inside LAr of each Half Barrel ------
        // copy number range (low, high)
        {  20, 20,                   // outer of acco in the center
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   2,
             sizeof(region020)/sizeof(RegionInfo_t),
             region020 }
        },
        {  24, 44,                   // 24 - 44 outer support-rings
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   2,
             sizeof(region020)/sizeof(RegionInfo_t),
             region020 }
        },
      };

    static const CopyNumberInfo_t info2[] =
      {
        //----region: inner wall ---------------------
        { 1, 1,
          // Det  Sub  Typ  Sam
          { 10,   4,   1,   1,
            sizeof(region010)/sizeof(RegionInfo_t),
            region010 }
        },
      };

    static const CopyNumberInfo_t info3[] =
      {
        // ---------------Sectors -----------------
        // copy number range (low, high)
        {  12, 12,                     // legs
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   2,
             sizeof(region021)/sizeof(RegionInfo_t),
             region021 }
        },
        {  7, 7,                     // ears
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   2,
             sizeof(region021)/sizeof(RegionInfo_t),
             region021 }
        },
        {  1, 1,                   // Ti blocks 11 - 34  (copy number arbitrary)
           // Det  Sub  Typ  Sam
           { 10,   4,   1,   1,
             sizeof(region012)/sizeof(RegionInfo_t),
             region012 }
        },
      };

    //    static const CopyNumberInfo_t info4[] =
    //      {
    //	// --------------- Half::Cylinder -----------------
    //	// --- placed inside LAr of each Half Barrel ------
    //	// copy number range (low, high)
    //	{  20, 20,                   // outer of acco in the center
    //         // Det  Sub  Typ  Sam
    //         { 10,   4,   1,   2,
    //           sizeof(region020)/sizeof(RegionInfo_t),
    //           region020 }
    //	},
    //	{  24, 44,                   // 24 - 44 outer support-rings
    //         // Det  Sub  Typ  Sam
    //         { 10,   4,   1,   2,
    //           sizeof(region020)/sizeof(RegionInfo_t),
    //           region020 }
    //	},
    //      };

    // Copy number range->Volume, based on name

    static const VolumeInfo_t volume1 =
      { "LAr::Barrel::Cryostat::Cylinder",
        sizeof(info1) / sizeof(CopyNumberInfo_t),
        info1
      };

    //    static const VolumeInfo_t volume2 =
    //      { "LAr::Barrel::Cryostat::Cone",
    //	sizeof(info2) / sizeof(CopyNumberInfo_t),
    //	info2
    //      };
    static const VolumeInfo_t volume2 =
      { "LAr::Barrel::Cryostat::InnerWall",
        sizeof(info2) / sizeof(CopyNumberInfo_t),
        info2
      };

    static const VolumeInfo_t volume3 =
      { "LAr::Barrel::Cryostat::Sector",
        sizeof(info3) / sizeof(CopyNumberInfo_t),
        info3
      };

    //    static const VolumeInfo_t volume4 =
    //      { "LAr::Barrel::Cryostat::Half::Cylinder",
    //	sizeof(info4) / sizeof(CopyNumberInfo_t),
    //	info4
    //      };

    static const VolumeInfo_t volumes[] = { volume1, volume2, volume3};
    static const G4int numberOfVolumes = sizeof(volumes) / sizeof(VolumeInfo_t);

    typedef std::map < G4int, const IdentifierInfo_t* > identifierMap_t;
    typedef identifierMap_t::iterator                   identifierMap_ptr_t;
    typedef std::map < G4String, identifierMap_t > volumeMap_t;
    typedef volumeMap_t::iterator                  volumeMap_ptr_t;
    static volumeMap_t volumeMap;

    static G4bool notInitialized = true;

    ///////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////

    CalibrationCalculator::CalibrationCalculator(const std::string& name, ISvcLocator *pSvcLocator)
      : LArCalibCalculatorSvcImp(name, pSvcLocator)
      , m_backupCalculator("BarrelCryostatCalibrationLArCalculator",name)
    {
      declareProperty("BackupCalculator",m_backupCalculator);
    }

    StatusCode CalibrationCalculator::initialize(){
      // Get the "backup" calculator.
      ATH_CHECK(m_backupCalculator.retrieve());

#if defined (DEBUG_HITS) || defined (DEBUG_MAPS)
      G4cout << "LArG4::BarrelCryostat::CalibrationCalculator::CalibrationCalculator - "
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
              G4cout << "LArG4::BarrelCryostat::CalibrationCalculator::CalibrationCalculator - "
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
                      G4cout << "LArG4::BarrelCryostat::CalibrationCalculator::CalibrationCalculator - "
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
              G4cout << "LArG4::BarrelCryostat::CalibrationCalculator::CalibrationCalculator - "
                     << "   volume '"
                     << volume.volumeName
                     << "' added to map."
                     << G4endl;
#endif

            }
        } // if not initialized

#if defined (DEBUG_HITS) || defined (DEBUG_MAPS)
      G4cout << "LArG4::BarrelCryostat::CalibrationCalculator::CalibrationCalculator - "
             << G4endl
             << "   initialization complete; map size="
             << volumeMap.size()
             << G4endl;
#endif

      return StatusCode::SUCCESS;
    }


    CalibrationCalculator::~CalibrationCalculator()
    {
      // Cleanup pointers.
      //delete m_backupCalculator;
      //m_backupCalculator = 0;
    }

    G4bool CalibrationCalculator::Process(const G4Step* step, LArG4Identifier & identifier,
                                          std::vector<G4double> & energies,
                                          const eCalculatorProcessing process) const
    {
      // Use the calculators to determine the energies and the
      // identifier associated with this G4Step.  Note that the
      // default is to process both the energy and the ID.

      if ( process == kEnergyAndID  ||  process == kOnlyEnergy )
        {
          m_energyCalculator.Energies( step, energies );
        }
      else
        for (unsigned int i=0; i != 4; i++) energies.push_back( 0. );


      identifier.clear();
      if ( process == kEnergyAndID  ||  process == kOnlyID )
        {
          // Calculate the identifier.

          // First, find the physical volume copy number, and the
          // logical volume name.

          G4VPhysicalVolume* physical = step->GetPreStepPoint()->GetPhysicalVolume();
          G4int copyNumber = physical->GetCopyNo();
          G4String volumeName = physical->GetLogicalVolume()->GetName();

#ifdef DEBUG_HITS
          G4cout << "LArG4::BarrelCryostat::CalibrationCalculator::Process - "
                 << G4endl
                 << "   searching for volume '"
                 << volumeName
                 << "' copyNumber="
                 << copyNumber
                 << G4endl;
#endif

          if(volumeName.index("LArMgr::") == 0) volumeName.erase(0,8);
          if(volumeName.index("LAr::Barrel::Cryostat::Sector")==0) { // ears, legs, Ti blocks
            volumeName = "LAr::Barrel::Cryostat::Sector";
            if(copyNumber != 7 && copyNumber !=12) copyNumber=1; // assignment arbitrary copyNumber to Ti block
          }else if(volumeName.index("LAr::Barrel::Cryostat::Cylinder")==0){
            volumeName = "LAr::Barrel::Cryostat::Cylinder";
          }else if(volumeName.index("LAr::Barrel::Cryostat::InnerWall")==0){
            volumeName = "LAr::Barrel::Cryostat::InnerWall";
            copyNumber = 1; // assignment arbitrary copyNumber
          }

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

                  G4StepPoint* pre_step_point = step->GetPreStepPoint();
                  G4StepPoint* post_step_point = step->GetPostStepPoint();
                  G4ThreeVector startPoint = pre_step_point->GetPosition();
                  G4ThreeVector endPoint   = post_step_point->GetPosition();
                  G4ThreeVector p = (startPoint + endPoint) * 0.5;

                  // Determine the geometric eta and phi.  Note that we do not
                  // adjust for any endcap shifts; the values are assigned to
                  // the volumes based on their design positions.

                  G4double eta = fabs( p.pseudoRapidity() );
                  G4double phi = p.phi();
                  // For this calculation, we need 0<phi<2*PI.  (The
                  // official ATLAS standard of -PI<phi<PI is
                  // meaningless here.)
                  if ( phi < 0 ) phi += 2*M_PI;

                  // The region can depend on eta.  Search through the
                  // list of regions within this IdentifierInfo record
                  // to find which one has etaMin<eta<etaMax.

                  G4int regions = info->numberOfRegions;
                  const RegionInfo_t* region = info->regionInfoArray;

#ifdef DEBUG_HITS
                  G4cout << "LArG4::BarrelCryostat::CalibrationCalculator::Process - "
                         << G4endl
                         << "   found volume, number of regions="
                         << regions
                         << " eta=" << eta << " phi=" << phi
                         << G4endl;
#endif

                  G4int r;
                  for ( r = 0; r < regions; r++, region++ )
                    {
                      if ( eta > region->etaMin  &&
                           eta < region->etaMax )
                        break;
                    }

                  // If the following statement is not true, we're in
                  // trouble.  It means that the eta of our energy
                  // deposit is outside the assumed eta limits of the
                  // volume.

                  if ( r < regions )
                    {
                      // Convert eta and phi to their integer equivalents for the
                      // identifier.

                      G4int etaInteger = G4int( (eta - region->etaMin) / region->deltaEta );
                      G4int phiInteger = G4int( phi / region->deltaPhi );
                      // phiInteger should be less than 64
                      if (phiInteger > 63) phiInteger = 0;

#ifdef DEBUG_HITS
                      G4cout << "LArG4::BarrelCryostat::CalibrationCalculator::Process - "
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
                      if ( etaInteger >= 0 )
                        {
                          // Build the full identifier.
                          identifier << info->detector
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

          // If a volume is not found on one of the above tables, try
          // the "backup identifier" calculator.

          if ( identifier == LArG4Identifier() )
            {
#if defined (DEBUG_HITS) || defined (DEBUG_VOLUMES)
              std::cout << "LArG4::BarrelCryostat::CalibrationCalculator::Process"
                        << std::endl
                        << "    volume '"
                        << volumeName
                        << "' copy# "
                        << copyNumber
                        << " not found on tables, using backup calculator"
                        << std::endl;
#endif
              m_backupCalculator->Process(step, identifier, energies, process);
            }
        } // calculate identifier

#ifdef DEBUG_HITS
      //G4double energy = accumulate(energies.begin(),energies.end(),0.);
      std::cout << "LArG4::BarrelCryostat::CalibrationCalculator::Process"
                << " vName " <<  step->GetPreStepPoint()->GetPhysicalVolume()->GetName()
                << " ID=" << std::string(identifier)
        //		<< " energy=" << energy
                << " energies=(" << energies[0]
                << "," << energies[1]
                << "," << energies[2]
                << "," << energies[3] << ")"
                << std::endl;
#endif

      // Check for bad result.
      if ( identifier == LArG4Identifier() )
        return false;

      return true;
    }

  } // namespace BarrelCryostat

} // namespace LArG4
