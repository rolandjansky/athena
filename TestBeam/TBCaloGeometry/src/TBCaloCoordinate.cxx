/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// Liquid Argon detector description package
// -----------------------------------------
//
//****************************************************************************

#include "TBCaloGeometry/TBCaloCoordinate.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"
#include <vector>

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TBCaloConditions/ITBCaloPosTool.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include "TBDetDescr/TBDetDescrManager.h"
#include "TBDetDescr/TBElement.h"
#include "TBDetDescr/TBElementID.h"

//#include "CLHEP/Geometry/Transform3D.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <iostream>
#include <iomanip>


using Amg::Vector3D;
using Amg::Transform3D;
using Amg::Rotation3D;
using Amg::Translation3D;
using Amg::RotationMatrix3D; 


static const InterfaceID IID_TBCaloCoordinate("TBCaloCoordinate", 1, 0);

const InterfaceID& TBCaloCoordinate::interfaceID( ) 
{ return IID_TBCaloCoordinate; }

TBCaloCoordinate::TBCaloCoordinate(const std::string& type, 
				   const std::string& name, 
				   const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_table_axis_data(0.), m_table_axis_MC(0.),
  m_table_proj_data(0.), m_table_proj_MC(0.),
  m_table_eta(0.), 
  m_calo_phi_shift(0.), m_calo_theta_shift(0.),
  m_calo_psi_shift(0.),  m_calo_x_shift(0.), 
  m_calo_y_shift(0.),  m_calo_z_shift(0.), 
  m_table_theta(0.),  m_table_z(0.),  m_table_delta(0.), 
  m_table_calc_theta(0.),  m_table_calc_x(0.), 
  m_DBRead(-1), m_PoolRead(-1), m_runNumber(0), m_firstevt(0) 
{
  declareInterface<TBCaloCoordinate>( this );

  // by default, assume Atlas :  m_DBRead=-1, m_PoolRead=-1
  // other clients need to include the jobOpt fragment and set
  // what they want via the properties :

  declareProperty("DBRead",m_DBRead);
  declareProperty("PoolRead",m_PoolRead);

  declareProperty("calo_phi_shift",m_calo_phi_shift);
  declareProperty("calo_theta_shift",m_calo_theta_shift);
  declareProperty("calo_psi_shift",m_calo_psi_shift);
  declareProperty("calo_x_shift",m_calo_x_shift);
  declareProperty("calo_y_shift",m_calo_y_shift);
  declareProperty("calo_z_shift",m_calo_z_shift);

  declareProperty("table_eta",m_table_eta);

  // default values for MC, will be overwritten by Pool numbers
  m_table_axis_MC = 6062.; 
  m_table_proj_MC = 1276. - 6062.;

  // number taken from the note ATC-TT-IN-0001, Feb 12, 2005:
  m_table_axis_data = 6208.;

  // number extracted from the Tiles table rotation program, S. Solodkov, Nov 2004
  m_table_proj_data = -3910. ;

  // this will be calculated
  m_table_calc_theta=0.;
  m_table_calc_x=0.;

  m_transform_calo_to_ctb = new Transform3D(); 
  m_transform_ctb_to_calo = new Transform3D();
  //m_table_rotate = new Rotate3D();
  m_table_rotate = new RotationMatrix3D(); // uninitialized, but always assigned before use
  m_table_shift = new Translation3D(0.,0.,0.);
  m_postool=0;
  m_MCmgr=0;
  m_range = new CaloPhiRange;
}

StatusCode
TBCaloCoordinate::initialize()
{
  float eta = -1.*std::log(tan( (float)
		       ((m_calo_theta_shift+m_range->twopi()/4.)/2.)));
    
  ATH_MSG_DEBUG 
    ( " Numbers used for Calo geometry : "  
      << "phi=" << m_calo_phi_shift << " theta="
      << m_calo_theta_shift << " (=> deta=" << eta << ") psi="
      << m_calo_psi_shift << " x,y,z= "
      << m_calo_x_shift << " "
      << m_calo_y_shift << " "
      << m_calo_z_shift );
    
  // For global Calo position build HepTransforms according to 
  // jobOpt parameters :
  
  m_rotz_extra_calo_to_ctb = Amg::AngleAxis3D(m_calo_phi_shift, Amg::Vector3D(0.,0.,1.));
  m_roty_extra_calo_to_ctb = Amg::AngleAxis3D(m_calo_theta_shift, Amg::Vector3D(0.,1.,0.));
  m_rotx_extra_calo_to_ctb = Amg::AngleAxis3D(m_calo_psi_shift, Amg::Vector3D(0.,0.,1.));
  m_translxyz_extra_calo_to_ctb = Translation3D( m_calo_x_shift, m_calo_y_shift, m_calo_z_shift);
  
  StatusCode sc = StatusCode::SUCCESS;
  return sc;
}

TBCaloCoordinate::~TBCaloCoordinate()
{
  delete m_transform_calo_to_ctb;
  delete m_transform_ctb_to_calo;
  delete m_table_rotate;
  delete m_table_shift;
  delete m_range;
}

StatusCode
TBCaloCoordinate::finalize()
{
    StatusCode sc = StatusCode::SUCCESS;
    return sc;
}

void   
TBCaloCoordinate::ctb_to_local( Vector3D& pt_ctb,
                                Vector3D& pt_local)
{
  read_table_position();
  pt_local = (*m_transform_ctb_to_calo)*pt_ctb;
}

void   
TBCaloCoordinate::local_to_ctb( Vector3D& pt_local,
                                Vector3D& pt_ctb)
{
  read_table_position();
  pt_ctb = (*m_transform_calo_to_ctb)*pt_local;
}

void   
TBCaloCoordinate::ctb_to_local( double& x_ctb, double& y_ctb, double& z_ctb, 
				double& x_local, double& y_local, double& z_local)
{
  Vector3D pt_ctb(x_ctb,y_ctb,z_ctb);
  Vector3D pt_local(0.,0.,0.);

  ctb_to_local(pt_ctb,pt_local);

  x_local = pt_local.x();
  y_local = pt_local.y();
  z_local = pt_local.z();

  //log << MSG::DEBUG << "ctb : " << x_ctb << " " << y_ctb << " " 
  //	    << z_ctb << " " << endmsg;
  //log << MSG::DEBUG << " ==> local : " << x_local << " " << y_local << " " 
  //	    << z_local << " " << endmsg;
}

void   
TBCaloCoordinate::local_to_ctb(double& x_local,double& y_local,double& z_local, 
			       double& x_ctb, double& y_ctb, double& z_ctb)
{
  Vector3D pt_local(x_local,y_local,z_local);
  Vector3D pt_ctb(0.,0.,0.);

  local_to_ctb(pt_local,pt_ctb);

  x_ctb = pt_ctb.x();
  y_ctb = pt_ctb.y();
  z_ctb = pt_ctb.z();

  //log << MSG::DEBUG << " local : " << x_local << " " << y_local << " " 
  //	    << z_local << " " << endmsg;
  //log << MSG::DEBUG << " ==> ctb : " << x_ctb << " " << y_ctb << " " 
  //	    << z_ctb << " " << endmsg;
}

double    
TBCaloCoordinate::beam_local_eta()
{
  read_table_position();
  return m_table_eta;
}

double    
TBCaloCoordinate::beam_local_phi()
{ 
  read_table_position();
  return m_table_delta;
}

// This is where the work is done :

void    
TBCaloCoordinate::read_table_position()
{   
  if ( m_firstevt == 0 ) {
    const EventInfo* eventInfo;
    StatusCode sc = StoreGate::pointer()->retrieve(eventInfo);
    if ( !sc.isFailure()) {
      const EventID* myEventID=eventInfo->event_ID();
      m_runNumber=myEventID->run_number();

      m_firstevt = 1;
    }
  }

  if ( m_DBRead >= 0 ) {
    bool result = read_data_position();
    if (!result) read_user_position();
  }
  else if ( m_PoolRead >= 0 ) {
    bool result = read_MC_position();
    if (!result) read_user_position();
  }
  else
    read_neutral_position();

}
void    
TBCaloCoordinate::read_fake_table_position()
{   
  if ( m_firstevt == 0 ) m_firstevt = 1;

  if ( m_DBRead >= 0 || m_PoolRead >= 0) {
    read_user_position();
  }
  else
    read_neutral_position();
}

Transform3D*  
TBCaloCoordinate::transform_calo_to_ctb()
{
  return m_transform_calo_to_ctb;
}

Transform3D*  
TBCaloCoordinate::transform_ctb_to_calo()
{
  return m_transform_ctb_to_calo;
}

void
TBCaloCoordinate::print_transform(Transform3D& htrans)
{
  RotationMatrix3D junkrot = htrans.rotation();
  double alpha = Eigen::AngleAxisd(junkrot).angle();
  Eigen::Vector3d junkaxis = Eigen::AngleAxisd(junkrot).axis();

  ATH_MSG_INFO ("");

  ATH_MSG_INFO ( " -> Rotation : axis x,y,z = " << junkaxis[Amg::x]
                 << " " << junkaxis[Amg::y] << " " << junkaxis[Amg::z]);
  ATH_MSG_INFO ( "               angle = " << alpha );

  Amg::Vector3D junktransl = htrans.translation();

  ATH_MSG_INFO ( " -> Translation : x,y,z = " << 
                 junktransl[Amg::x] << " " << junktransl[Amg::y] << " " << junktransl[Amg::z] );

  ATH_MSG_INFO ("");

}

bool
TBCaloCoordinate:: read_data_position()
{
  // --------  Access to DB tool : will try once, if fails will use the default (user)
  
  if ( m_DBRead == 0 ) {

    // Horrible hack : DCS update were missing for the end of the data-taking
    if ( m_runNumber >= 2102455 ) {
      m_DBRead = -1;
      return false;      
    }
    // Horrible hack : DCS indicated infinite eta value for some runs
    if ( m_runNumber > 2102164 && m_runNumber <= 2102208 ) {
      m_DBRead = -1;
      return false;      
    }
    
    IToolSvc* p_toolSvc = 0;
    StatusCode sc = service("ToolSvc", p_toolSvc);
    if (sc.isFailure()) {
      ATH_MSG_ERROR ( "Cannot find ToolSvc ??? " );
      m_DBRead = -1;
      return false;
    }
    else {
      sc = p_toolSvc->retrieveTool("TBCaloPosTool", m_postool);
      if(sc.isFailure()) {
	ATH_MSG_ERROR 
          ( "Cannot get Calo table position from DB : keep default" );
	m_DBRead = -1;
	return false;
      }
      else {
	ATH_MSG_DEBUG ( "Did get Calo table position from DB !" );
	m_DBRead = 1;
      } 
    }   
  }

  if ( m_DBRead > 0 ) {
    m_table_eta = m_postool->eta(); 
    m_table_theta = m_postool->theta(); 
    m_table_z = m_postool->z(); 
    m_table_delta = m_postool->delta(); 
    
    // Protection : if result is crasy, switch to handcoded default
    if ( m_table_eta < 0. || m_table_eta > 1.5 ) {
      ATH_MSG_INFO ( "    ==> Calorimeter table position read from DB makes no sense, DB updates will be overwitten " );
      ATH_MSG_INFO ( "  " );
      m_DBRead = 1;
      return false;
    }
    
    if (m_firstevt == 1 ) {
      ATH_MSG_INFO ( " --------------------------------------------------------------------- "  );
      ATH_MSG_INFO ( "  " );
      ATH_MSG_INFO ( " Calorimeter table position is read from DB : for run " << m_runNumber
                     << " eta is = " << m_table_eta );
      ATH_MSG_INFO ( "  " );


      ATH_MSG_INFO ( "         If it does not match the LAr logbook, inform LAr people        " );
      ATH_MSG_INFO ( "  " );
      ATH_MSG_INFO ( "  " );
      ATH_MSG_INFO ( " Other (unused) table numbers are : theta=" << m_table_theta
                     << " z=" << m_table_z << " delta=" << m_table_delta );
      ATH_MSG_INFO ( " --------------------------------------------------------------------- "  );
      
      m_firstevt = 2;
    }
  }
  
  // ------- OK, it worked : now do the work

  // in Atlas, theta is the angle with the z axis, and it is
  // given by the formula atan(exp(-m_table_eta))*2.
  // but here, we want the angle against the y axis :
  m_table_calc_theta= m_range->twopi()/4. - atan(exp(-m_table_eta))*2.;
  *m_table_rotate = Amg::AngleAxis3D(m_table_calc_theta, Amg::Vector3D(0.,1.,0.));

  m_table_calc_x= m_table_axis_data + m_table_proj_data/cosh(m_table_eta);
  //*m_table_shift = TranslateX3D(m_table_calc_x);
  *m_table_shift = Translation3D(m_table_calc_x*Amg::Vector3D(1.,0.,0.));

  *m_transform_calo_to_ctb = m_translxyz_extra_calo_to_ctb*((*m_table_shift)*(m_rotx_extra_calo_to_ctb*((*m_table_rotate)*(m_roty_extra_calo_to_ctb*(m_rotz_extra_calo_to_ctb)))));

  //print_transform( *m_transform_calo_to_ctb );

  *m_transform_ctb_to_calo = m_transform_calo_to_ctb->inverse();

  ATH_MSG_DEBUG ( "Calculated table position is : " );
  ATH_MSG_DEBUG ( "         angle against y axis = " 
                  << m_table_calc_theta );
  ATH_MSG_DEBUG ( "         x shift = " << m_table_calc_x );

  ATH_MSG_DEBUG ( " Final corresponding  Moovement : " );
  //print_transform( *m_transform_calo_to_ctb );

  return true;
}

bool
TBCaloCoordinate:: read_MC_position()
{
  if (!m_MCmgr) {
    ATH_MSG_INFO ( "Retreiving TBDetDescrManager" );

    // get the manager used for simulation :

    ATH_CHECK( detStore()->retrieve( m_MCmgr ), false );
  }

  ATH_MSG_DEBUG ( " found TBDetDescrManager " );
	
  TBElement  LArTileMother = m_MCmgr->getElement(TBElementID::CALO);
  if (LArTileMother.id() == TBElementID::Unknown ) return false;
    
  ATH_MSG_DEBUG ( " found CALO envelope " );

  // ------- OK, now do the work :

  Vector3D pos = LArTileMother.position();
  *m_table_shift = Translation3D(pos.x(),pos.y(),pos.z());
  
  // FIXME : rotation has the wrong sign !
  *m_table_rotate = (LArTileMother.rotation()).inverse();
  
  *m_transform_calo_to_ctb = (*m_table_shift)*(*m_table_rotate);
  
  // Extract m_table_eta
  // Not completely satisfactory : it's wrong if the calorimeter is rotated 
  //                               around z and/or x in addition to y (it's not supposed
  //                               to be the case in MC but still ...)
  //float angle = m_transform_calo_to_ctb->getRotation().delta();
  float angle = Eigen::AngleAxisd(m_transform_calo_to_ctb->rotation()).angle();
  m_table_eta = -1.*std::log(tan((float) (m_range->twopi()/4. - angle)/2.));

  if(m_table_eta<0. || m_table_eta > 1.5) {
    ATH_MSG_INFO ( " m_table_eta is not in [0,1.5] -> will set it to 0. " );
    m_table_eta = 0.;
  }

  //log << MSG::DEBUG << " Enveloppe position is read from Pool, and is : " 
  //    << endmsg;
  //print_transform( *m_transform_calo_to_ctb );
  
  *m_transform_ctb_to_calo = m_transform_calo_to_ctb->inverse();

  return true;
}

void
TBCaloCoordinate:: read_user_position()
{
  // This is a fall back solution if numbers are not found anywhere else
  // Note that the hypothesis is that it is DATA

  // do the work once :
  //if (m_firstevt > 1 ) return;

  // default :
  m_table_eta = 0.45;

  // Horrible hack : DCS indicated infinite eta for some periods
  if ( m_runNumber >= 2102181 && m_runNumber < 2102208 ) m_table_eta = 0.55;
  if ( m_runNumber == 2102208 ) m_table_eta = 0.42;

  // Horrible hack : DCS update were missing for the end of the data-taking
  if ( m_runNumber >= 2102455 && m_runNumber < 2102470 ) m_table_eta = 0.1;
  if ( m_runNumber >= 2102470 && m_runNumber < 2102478 ) m_table_eta = 0.2;
  if ( m_runNumber >= 2102478 && m_runNumber < 2102488 ) m_table_eta = 0.3;
  if ( m_runNumber >= 2102488 && m_runNumber < 2102498 ) m_table_eta = 0.4;
  if ( m_runNumber >= 2102498 && m_runNumber < 2102512 ) m_table_eta = 0.5;
  if ( m_runNumber >= 2102512 && m_runNumber < 2102530 ) m_table_eta = 0.7;
  if ( m_runNumber >= 2102530 && m_runNumber < 2102758 ) m_table_eta = 0.6;

  if ( m_runNumber >= 2102758 && m_runNumber < 2102903 ) m_table_eta = 0.55;
  if ( m_runNumber >= 2102903 && m_runNumber < 2102936 ) m_table_eta = 0.5;
  if ( m_runNumber >= 2102936 && m_runNumber < 2102953 ) m_table_eta = 0.3;
  if ( m_runNumber >= 2102953 && m_runNumber < 2102980 ) m_table_eta = 0.4;
  if ( m_runNumber >= 2102980 ) m_table_eta = 0.3;

  ATH_MSG_DEBUG ( " --------------------------------------------------------------------- "  );
  ATH_MSG_DEBUG ( "  " );
  ATH_MSG_DEBUG ( " Calorimeter table DCS either OFF of not used, position is hardcoded :  for run " 
                  << m_runNumber << " eta is = " << m_table_eta );
  ATH_MSG_DEBUG ( "  " );
  ATH_MSG_DEBUG ( "      If it does not match the LAr logbook, inform LAr people        " );
  ATH_MSG_DEBUG ( "  " );
  ATH_MSG_DEBUG ( " --------------------------------------------------------------------- "  );

  m_table_calc_theta= m_range->twopi()/4. - atan(exp(-m_table_eta))*2.;

  if(m_DBRead >= 0)
    m_table_calc_x= m_table_axis_data + m_table_proj_data/cosh(m_table_eta);
  else
    m_table_calc_x= m_table_axis_MC + m_table_proj_MC/cosh(m_table_eta);

  *m_table_rotate = Amg::AngleAxis3D(m_table_calc_theta, Amg::Vector3D(0.,1.,0.));
  //*m_table_shift = TranslateX3D(m_table_calc_x);
  *m_table_shift = Translation3D(m_table_calc_x*Amg::Vector3D(1.,0.,0.));

  *m_transform_calo_to_ctb = m_translxyz_extra_calo_to_ctb*((*m_table_shift)*(m_rotx_extra_calo_to_ctb*((*m_table_rotate)*(m_roty_extra_calo_to_ctb*(m_rotz_extra_calo_to_ctb)))));

  //print_transform( *m_transform_calo_to_ctb );

  *m_transform_ctb_to_calo = m_transform_calo_to_ctb->inverse();

  ATH_MSG_DEBUG ( " Moovement defined by hardcoded numbers : eta was set to " 
                  << m_table_eta);

  ATH_MSG_DEBUG  ( "Calculated table position is : " );
  ATH_MSG_DEBUG  ( "         angle against y axis = " 
                   << m_table_calc_theta );

  m_firstevt = 2;

  //log << MSG::DEBUG  << "         x shift = " << m_table_calc_x << endmsg;
  //print_transform( *m_transform_calo_to_ctb );

}

void
TBCaloCoordinate:: read_neutral_position()
{
  // do the work once :
  if (m_firstevt > 1 ) return;

  m_table_calc_theta= 0.;
  m_table_calc_x= 0;

  *m_table_rotate = Amg::AngleAxis3D(m_table_calc_theta, Amg::Vector3D(0.,1.,0.));
  //*m_table_shift = TranslateX3D(m_table_calc_x);
  *m_table_shift = Translation3D(m_table_calc_x*Amg::Vector3D(1.,0.,0.));
  *m_transform_calo_to_ctb = (*m_table_shift)*(*m_table_rotate);
  *m_transform_ctb_to_calo = m_transform_calo_to_ctb->inverse();

  m_firstevt = 2;

  ATH_MSG_DEBUG ( " Neutral Moovement : " );
  //print_transform( *m_transform_calo_to_ctb );

}
