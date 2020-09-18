/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNMENT_CREATEMISALIGNALG_H
#define INDETALIGNMENT_CREATEMISALIGNALG_H

// CreateMisalignAlg.h
//

// #include <map>
#include <vector>

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// CLHEP includes
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Geometry/Transform3D.h"

// Write output into ROOT Trees
#include "TTree.h"

//Forward declaration
class IdentifierHash;
#include "Identifier/Identifier.h"
class TTree;

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;

#include "InDetAlignGenTools/IInDetAlignDBTool.h"
#include "TRT_ConditionsServices/ITRT_AlignDbSvc.h"

namespace InDetDD {
class SCT_DetectorManager;
class PixelDetectorManager;
class TRT_DetectorManager;
}

namespace InDetAlignment {
class CreateMisalignAlg : public AthAlgorithm {

public:

    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CreateMisalignAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~CreateMisalignAlg();

    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

private:
    // Data Members m_*
    const AtlasDetectorID                 *m_idHelper;
    const PixelID                         *m_pixelIdHelper;
    const SCT_ID                          *m_sctIdHelper;
    const TRT_ID                          *m_trtIdHelper;

    const InDetDD::PixelDetectorManager   *m_pixelManager;
    const InDetDD::SCT_DetectorManager    *m_SCT_Manager;
    const InDetDD::TRT_DetectorManager    *m_TRT_Manager;

    ToolHandle< IInDetAlignDBTool >        m_IDAlignDBTool;
    ServiceHandle<ITRT_AlignDbSvc>          m_trtaligndbservice;

    // algorithm parameters, possible to declare at runtime
    std::string                            m_asciiFileNameBase;  ///< filename basis for ASCII files with alignment constants
    std::string                            m_SQLiteTag;          ///< tag name for the ConditionsDB

    bool                                   m_firstEvent;         //!< Flag for Setup of AlignModuleList (1st event)
    bool                                   m_createFreshDB;      ///< Flag to call the createDB method of DBTool
                                                                 ///< (to be switched off when adding misalignments to a given geometry)
    int                                    m_MisalignmentMode;   //!< Flag which Misalignment mode is to be generated
    long int                               m_nEvents;
    double                                 m_Misalign_x;         //!< x Misalignment for the "22" chosen modules
    double                                 m_Misalign_y;         //!< y Misalignment for the "22" chosen modules
    double                                 m_Misalign_z;         //!< z Misalignment for the "22" chosen modules
    double                                 m_Misalign_alpha;     //!< alpha Misalignment for the "22" chosen modules
    double                                 m_Misalign_beta;      //!< beta Misalignment for the "22" chosen modules
    double                                 m_Misalign_gamma;     //!< gamma Misalignment for the "22" chosen modules
    double                                 m_Misalign_maxShift;  //!< Maximum shift for global modes
    double                                 m_Misalign_maxShift_Inner;  //!< Maximum shift of the Pixel B-layer in curl (d0 bias!)
    double                                 m_ScalePixelIBL;
    double                                 m_ScalePixelDBM;
    double                                 m_IBLBowingTshift;    //!< The relative temp shift of set point that intriduces bowing (sign is important)
    double                                 m_ScalePixelBarrel;
    double                                 m_ScalePixelEndcap;
    double                                 m_ScaleSCTBarrel;
    double                                 m_ScaleSCTEndcap;
    double                                 m_ScaleTRTBarrel;
    double                                 m_ScaleTRTEndcap;

    std::map<Identifier, HepGeom::Point3D<double> >  m_ModuleList;        //!< map of all SiIdentifiers to be misaligned and their centerpoints in global coordinates

    // Generated Initial Alignment NTuple
    TTree * m_VisualizationLookupTree;

    int         m_AthenaHashedID;
    int         m_HumanReadableID;

    bool                                   m_doTRT;
    bool                                   m_doPixel;
    bool                                   m_doSCT;

    NTuple::Item<double> m_AlignResults_x;                    //!< Alignment parameter sensitive coordinate
    NTuple::Item<double> m_AlignResults_y;                    //!< AP not-so-sensitive coordinate
    NTuple::Item<double> m_AlignResults_z;                    //!< AP normal to module plane
    NTuple::Item<double> m_AlignResults_alpha;                //!< AP rotation around x-axis
    NTuple::Item<double> m_AlignResults_beta;                 //!< AP rotation aorund y-axis
    NTuple::Item<double> m_AlignResults_gamma;                //!< AP rotation around z-axis
    NTuple::Item<long>   m_AlignResults_Identifier_ID;        //!< ID information for this module
    NTuple::Item<long>   m_AlignResults_Identifier_PixelSCT;  //!< ID information for this module
    NTuple::Item<long>   m_AlignResults_Identifier_BarrelEC;  //!< ID information for this module
    NTuple::Item<long>   m_AlignResults_Identifier_LayerDisc; //!< ID information for this module
    NTuple::Item<long>   m_AlignResults_Identifier_Phi;       //!< ID information for this module
    NTuple::Item<long>   m_AlignResults_Identifier_Eta;       //!< ID information for this module

    NTuple::Item<double> m_AlignResults_center_x;             //!< Initial global center of module
    NTuple::Item<double> m_AlignResults_center_y;             //!< Initial global center of module
    NTuple::Item<double> m_AlignResults_center_z;             //!< Initial global center of module

    // Member Functions

    void setupSCT_AlignModule(int&);
    void setupPixel_AlignModule(int&);
    void setupTRT_AlignModule(int&);

    /// builds a HepGeom::Transform3D from the 6 Alignment Parameters
    const HepGeom::Transform3D BuildAlignTransform(const CLHEP::HepVector &);

    const Identifier reduceTRTID(Identifier id);
    const Identifier reduceTRTID(IdentifierHash &hash);

    // IBL bowing functions
    double getBowingMagParam(double temp_shift);
    double getBowingTx(double p1, double z);
  
    /// the main function which calculates and applies a transformation to each detector element
    StatusCode GenerateMisaligment();
    


};

} // end of namespace bracket
#endif // INDETALIGNMENT_CREATEMISALIGNALG_H
