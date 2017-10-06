/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ***************************************************************************
// Liquid Argon H8 Combined TestBeam Geometry 
// -----------------------------------------
//
// 30.9.2004 Creation of the class by claire.bourdarios@cern.ch
//
// ***************************************************************************

#ifndef TBCALOGEOMETRY_TBCALOCOORDINATE_H
#define TBCALOGEOMETRY_TBCALOCOORDINATE_H

#include "CaloDetDescr/ICaloCoordinateTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

class CaloPhiRange;
class ITBCaloPosTool;
class IMessageSvc;
class TBDetDescrManager;

//done in the interface : #include "CLHEP/Geometry/Transform3D.h"

//<<<<<< INCLUDES >>>>>>

/** This class implements the CaloDetDescr/ICaloCoordinateTool and
    handles the translation between :

    - the H8 CTB official coordinate system, defined in :

       "Atlas Barrel Combined Run in 2004, TestBeam Layout"
       ATC-TT-IN-0001, Beniamino Di Girolamo

       All variables in this coordinate system contain the
       letters _ctb

    - the LAr+Tile local coordinate system, which differs for 2
      reasons : 

       the origin is shifted by x = 2298 mm, and 
       the two calos sit on a rotating table to allow eta scans.

       In the local coordinate system, calorimeters have a pointing 
       geometry.

       All variables in this coordinate system contain the
       letters _local.
 
     - All intermetiate variables contain the letters
       _table. They are not meant to be used or known by the
       public interface.

Most clients are only interested in the methods TBCaloCoordinate::ctb_to_local
and TBCaloCoordinate::local_to_ctb, which convert a given point from one coordinate
system into another. The read_table_position() in called internally, is is transparent
to the user.

But Atlantis clients need access to the HepTransforms used internally. Some public
methods are thus provided. Note that, for these methods, the user has to call
read_table_position() before hand. 


 **/

class TBCaloCoordinate : public AthAlgTool, virtual public ICaloCoordinateTool
{
public:
  
  // Constructors:

  TBCaloCoordinate(const std::string& type, 
		  const std::string& name, 
		  const IInterface* parent); 

  ~TBCaloCoordinate();
  static const InterfaceID& interfaceID( ) ; 
  StatusCode initialize();
  StatusCode finalize();

  // Methods for general use: 

  // 1) basic translation between the 2 cartesian coordinate systems:
  
  void ctb_to_local( Amg::Vector3D& pt_ctb,
                     Amg::Vector3D& pt_local);
  void local_to_ctb( Amg::Vector3D& pt_local,
                     Amg::Vector3D& pt_ctb);
  
  void ctb_to_local( double& x_ctb, double& y_ctb, double& z_ctb, 
		     double& x_local, double& y_local, double& z_local);
  void local_to_ctb( double& x_local, double& y_local, double& z_local, 
		     double& x_ctb, double& y_ctb, double& z_ctb);

  // 2) the most common use-case : 
  //    the H8 beam goes along the x axis -> that are the local
  //    eta and phi seen by the calorimeter ?

  double beam_local_eta();
  double beam_local_phi();


  // Methods of less general interest : ( see ICaloCoordinate.h for comments )

  // real table reading :
  virtual void read_table_position();

  // hand-coded value :
  virtual void read_fake_table_position();

  Amg::Transform3D* transform_calo_to_ctb();
  Amg::Transform3D* transform_ctb_to_calo();

  void print_transform(Amg::Transform3D& htrans);

private:
  // Avoid coverity warnings
  TBCaloCoordinate (const TBCaloCoordinate&);
  TBCaloCoordinate& operator= (const TBCaloCoordinate&);

  // these methods are used by read_table_position to get numbers :

  bool read_data_position();   // for H8 data
  bool read_MC_position();     // for H8 MC where numbers are in POOL
  void read_user_position();   // for H8 MC where numbers are not avail
  void read_neutral_position(); // for Atlas

  // these are the famous key numbers :

  // 1) position of the table axis (oral tradition)
  double m_table_axis_data;
  double m_table_axis_MC;
  // 2) distance between the table axis and projective point (oral tradition)
  double m_table_proj_data;
  double m_table_proj_MC;
  // 3) angle of rotation : default is set in jobOpt (property)
  //                        + will be overwritten if DB/Pool is used
  //                        + will be ignored "if atlas" 
  double m_table_eta;

  // Another layer on top, "in case" the user wants to add an
  // extra HepTransform on top of these known moovements :
  double m_calo_phi_shift;
  double m_calo_theta_shift;
  double m_calo_psi_shift;
  double m_calo_x_shift;
  double m_calo_y_shift;
  double m_calo_z_shift;

  // these will be read in the DB :
  double m_table_theta;
  double m_table_z;
  double m_table_delta;
  // this is calculated from the DB info :
  double m_table_calc_theta;
  double m_table_calc_x;

  // these properties define what we are reading (see jobOpt)
  int m_DBRead;
  int m_PoolRead;

  // keep the 2 steps separated : table rotation, then translation 
  //HepGeom::Rotate3D* m_table_rotate;
  Amg::RotationMatrix3D* m_table_rotate;
  //HepGeom::Translate3D* m_table_shift;
  Amg::Translation3D* m_table_shift;

  // the user defined one on top :

  Amg::Transform3D m_rotz_extra_calo_to_ctb;
  Amg::Transform3D m_roty_extra_calo_to_ctb;
  Amg::Transform3D m_rotx_extra_calo_to_ctb;
  Amg::Transform3D m_translxyz_extra_calo_to_ctb;

  // the final transform from local to ctb :
  Amg::Transform3D* m_transform_calo_to_ctb;
  Amg::Transform3D* m_transform_ctb_to_calo;

  // Run number
  long m_runNumber;

  // to limit the printout
  int m_firstevt;

  ITBCaloPosTool* m_postool;
  const TBDetDescrManager* m_MCmgr;
  CaloPhiRange* m_range;
};

 
#endif // TBCALOGEOMETRY_TBCALOCOORDINATE_H
