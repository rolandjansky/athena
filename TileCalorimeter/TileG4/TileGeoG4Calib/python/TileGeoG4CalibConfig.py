# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getTileGeoG4CalibSD(name="TileGeoG4CalibSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", [ 'Tile::Scintillator', 'Tile::Tile', 'Tile::TileCentralBarrel', 'Tile::TileEndcapPos', 'Tile::TileEndcapNeg', 'Tile::Barrel', 'Tile::BarrelModule', 'Tile::FrontPlate', 'Tile::EndPlate1', 'Tile::EPHole1', 'Tile::EndPlate2', 'Tile::EPHole2', 'Tile::GirderMother', 'Tile::GirderIron', 'Tile::GirderAluminium', 'Tile::GirderElectronics', 'Tile::Absorber', 'Tile::AbsorberChild', 'Tile::Period', 'Tile::Glue', 'Tile::Wrapper', 'Tile::EBarrel', 'Tile::EBarrelModule', 'Tile::EndPlateSh', 'Tile::ITC', 'Tile::ITCModule', 'Tile::Plug1Module', 'Tile::FrontPlateSh', 'Tile::Plug2Module', 'Tile::Gap', 'Tile::GapModule', 'Tile::IrUp', 'Tile::IrDw', 'Tile::Iron4', 'Tile::Iron3', 'Tile::Iron2', 'Tile::Iron1', 'Tile::IrBox', 'Tile::SaddleModule', 'Tile::LArService', 'Tile::LArCables', 'Tile::ExtBarrelSaddleSupport', 'Tile::Crack', 'Tile::CrackModule', 'Tile::FingerModule', 'Tile::FingerIron', 'Tile::FingerAluminum', 'Tile::FingerElectronics', 'Tile::EFinger', 'Tile::EFingerModule', 'Tile::FingerPos', 'Tile::FingerNeg', 'Tile::SaddlePos', 'Tile::SaddleNeg', 'Tile::ESaddlePos', 'Tile::ESaddleNeg' ] )
    kwargs.setdefault("OutputCollectionNames", ["TileHitVec", "TileCalibHitActiveCell", "TileCalibHitInactiveCell", "TileCalibHitDeadMaterial"] )

    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == 'cosmics' or (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
        kwargs.setdefault("DeltaTHit", [1])
        kwargs.setdefault("DoTOFCorrection", False)
    if hasattr(simFlags, "ParticleID"):
        kwargs.setdefault("DoCalibHitParticleID", simFlags.ParticleID())
    return CfgMgr.TileGeoG4CalibSDTool(name, **kwargs)
