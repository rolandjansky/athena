
/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "RoiUpdaterTool.h"



RoiUpdaterTool::RoiUpdaterTool(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent), m_update(false) 
{}



StatusCode RoiUpdaterTool::initialize()  {

  ATH_MSG_DEBUG("initialize()");

  if ( m_etaWidth!=-999 || m_phiWidth!=-999 || m_zedWidth!=-999 ) m_update = true;

  if ( m_useBeamspot ) { 
    m_update = true;
    ATH_CHECK( m_beamspotKey.initialize( m_useBeamspot ) );
  }

  return StatusCode::SUCCESS;
}



void zrange( double nsigma, double fence, double& zed, double& zedMinus, double& zedPlus, const SG::ReadCondHandleKey<InDet::BeamSpotData>& bs, const EventContext& ctx ) { 

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle( bs, ctx );

  uint32_t bitmap = beamSpotHandle->beamStatus();
  
  /// 0x4 is the online flag - 0x7 is presumably converged online
  if ( ( (bitmap & 0x7) == 0x7 ) || !(bitmap & 0x4) ) { 
    double zsigma = beamSpotHandle->beamSigma(2);
    double zpos   = beamSpotHandle->beamPos()[2];
    
    zed      = zpos; /// technically, updating the z position itself should not be needed
    zedMinus = zpos - nsigma*zsigma - fence;
    zedPlus  = zpos + nsigma*zsigma + fence;
  }
  else { 
    zedMinus = -RoiDescriptor::zedWidthDefault();     
    zedPlus  =  RoiDescriptor::zedWidthDefault();  	
  }
  
}




std::unique_ptr<TrigRoiDescriptor> RoiUpdaterTool::execute( const EventContext& ctx ) const {  
  /// create initial dummy FS Roi here - used to define the new Roi and is then discarded  
  RoiDescriptor troi( RoiDescriptor::FULLSCAN );
  return execute( &troi, ctx );
}


std::unique_ptr<TrigRoiDescriptor> RoiUpdaterTool::execute( const IRoiDescriptor* iroi, const EventContext& ctx ) const {  

  ATH_MSG_DEBUG( "execute()" );
  
  /// if we don't want to update, why are we even including trhe RoiUIpdater ???
  /// will return a unique_ptr - if that causes issues down the line, so be it
  if ( !m_update ) return std::make_unique<TrigRoiDescriptor>( *iroi );

  /// should not update a composite Roi - or do we want the constituents updated ???
  /// or what ??? Return a copy ? Who will own this stuff ?? 
  /// why are you calling the updater on a composite Roi in any case ? 
  /// will return a unique_ptr - if that causes issues down the line, so be it
  if ( iroi->composite() )  { 
    ATH_MSG_WARNING( "Will not update composite Roi parameters: " << *iroi );
    return std::make_unique<TrigRoiDescriptor>( *iroi );
  }  

  double eta      = iroi->eta();
  double etaMinus = iroi->etaMinus();
  double etaPlus  = iroi->etaPlus();
  
  double phi      = iroi->phi();
  double phiMinus = iroi->phiMinus();
  double phiPlus  = iroi->phiPlus();
  
  double zed      = iroi->zed();
  double zedMinus = iroi->zedMinus();
  double zedPlus  = iroi->zedPlus();
  
  if ( m_etaWidth!=-999 ) { 
    etaMinus = eta - m_etaWidth;  
    etaPlus  = eta + m_etaWidth;  
  }
  
  if ( m_phiWidth!=-999 ) { 
    /// NB: the correct mapping of phi to the range  -M_PIF < phi < M_PIF is
    ///     done in the RoiDescriptor constuctor so we don't bother here 
    phiMinus = phi - m_phiWidth;  
    phiPlus  = phi + m_phiWidth;  
  }
  
  if ( m_zedWidth!=-999 ) { 
    zedMinus = zed - m_zedWidth;
    zedPlus  = zed + m_zedWidth;
  }
  
  /// do *not* prevent the beamspot from *increasing* the size of the Roi
  if ( m_useBeamspot ) zrange( m_nsigma, m_fence, zed, zedMinus, zedPlus, m_beamspotKey, ctx );
  
  /// limit to old range - no point going well outside the interaction region
  if ( zedMinus<-RoiDescriptor::zedWidthDefault() ) zedMinus = -RoiDescriptor::zedWidthDefault();     
  if (  zedPlus> RoiDescriptor::zedWidthDefault() ) zedPlus  =  RoiDescriptor::zedWidthDefault();  
  
  /// don't use an actual fullscan Roi, since the parameters have been changed so may not actually 
  /// include all detector elements - the FS flag, prevents the RegionSelector from actually testing
  /// the detector element containment, so could create a "fullscan" Roi with only a small number of
  /// detector elements. If you want genuine FS behaviour for the RS, use an FS Roi. If you want 
  /// a restricted Roi for non RS use, even if it is FS for a specific subdetector, use a restricted 
  /// Roi    
  return std::make_unique<TrigRoiDescriptor>( eta, etaMinus, etaPlus, phi, phiMinus, phiPlus, zed, zedMinus, zedPlus );

}

