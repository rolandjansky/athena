/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file ICaloCoordinateTool.h
 *
 * @brief ICaloCoordinateTool interface declaration
 *  30.9.2004 Creation of the class TBCalocoordinate by claire.bourdarios@cern.ch
 *  30.7.2005 Creation of the interface CaloDetDescr/ICaloCoordinateTool : the 
              (dirty )implementation stays in the TestBeam package.
 */

#ifndef CALODETDESCR_ICALOCOORDINATETOOL_H
#define CALODETDESCR_ICALOCOORDINATETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"


/** @class ICaloCoordinateTool

    @brief This (clean) interface is implemented in the (rather dirty) ICaloCoordinateTool
    class, which depends on data-bases and H8 CTB specificities.

    The goal of these classes is to handles the translation between :

    - the H8 CTB official coordinate system, defined in :

       "Atlas Barrel Combined Run in 2004, TestBeam Layout"
       ATC-TT-IN-0001, Beniamino Di Girolamo

       All variables in this coordinate system contain the
       letters _ctb

    - the LAr+Tile local coordinate system, which differs for 2
      reasons : 

       the origin is shifted by x = 2298 CLHEP::mm, and 
       the two calos sit on a rotating table to allow eta scans.

       In the local coordinate system, calorimeters have a pointing 
       geometry.

       All variables in this coordinate system contain the
       letters _local.
 
     - All intermetiate variables contain the letters
       _table. They are not meant to be used or known by the
       public interface.

Most clients are only interested in the methods ICaloCoordinateTool::ctb_to_local
and ICaloCoordinateTool::local_to_ctb, which convert a given point from one coordinate
system into another. The read_table_position() in called internally, is is transparent
to the user.

But Atlantis clients need access to the AmgTransforms used internally. Some public
methods are thus provided. Note that, for these methods, the user has to call
read_table_position() before hand. 


 **/

class ICaloCoordinateTool : virtual public IAlgTool
{
public:

  // Methods for general use: 

  /** @brief General use method:  basic translation between the 2 cartesian coordinate systems:
   */ 
  virtual void ctb_to_local( Amg::Vector3D& pt_ctb, Amg::Vector3D& pt_local) =0 ;
  /** @brief General use method:  basic translation between the 2 cartesian coordinate systems:
   */ 
  virtual void local_to_ctb( Amg::Vector3D& pt_local, Amg::Vector3D& pt_ctb) =0 ;
  
  /** @brief General use method:  basic translation between the 2 cartesian coordinate systems:
   */ 
  virtual void ctb_to_local( double& x_ctb, double& y_ctb, double& z_ctb, 
			     double& x_local, double& y_local, double& z_local) =0 ;
  /** @brief General use method:  basic translation between the 2 cartesian coordinate systems:
   */ 
  virtual void local_to_ctb( double& x_local, double& y_local, double& z_local, 
			     double& x_ctb, double& y_ctb, double& z_ctb) =0 ;

  /** @brief the most common use-case : 
      the H8 beam goes along the x axis -> that are the local
      calorimeters eta and phi seen by the beam ?
   */
  virtual double beam_local_eta() =0 ;
  /** @brief the most common use-case : 
      the H8 beam goes along the x axis -> that are the local
      calorimeters eta and phi seen by the beam ?
   */
  virtual double beam_local_phi() =0 ;

  // Methods of less general interest, reserved to clients who want
  // direct access to the HepTransform(s) :
  // Warning : they must first read the table position !

  /** @brief clients who want the real position must call this
   */
  virtual void read_table_position() =0 ;

  /** @brief clients who want to avoid the  DB-decoding, and 
   force a fixed eta must call this :
   */
  virtual void read_fake_table_position() =0 ;

  /** @brief method accessing the transformations
   */ 
  virtual Amg::Transform3D* transform_calo_to_ctb() =0 ;
  /** @brief method accessing the transformations
   */ 
  virtual Amg::Transform3D* transform_ctb_to_calo() =0 ;

  /** @brief printout method
   */
  virtual void print_transform(Amg::Transform3D& htrans) =0 ;

};

 
#endif // CALODETDESCR_ICALOCOORDINATETOOL






