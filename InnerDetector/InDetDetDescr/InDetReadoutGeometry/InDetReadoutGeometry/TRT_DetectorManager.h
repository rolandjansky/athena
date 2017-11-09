/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_DetectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRT_DetectorManager_h
#define TRT_DetectorManager_h 1
#include "InDetReadoutGeometry/InDetDetectorManager.h"
#include "InDetReadoutGeometry/TRT_BarrelElement.h"
#include "InDetReadoutGeometry/TRT_EndcapElement.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/Version.h"
#include "InDetReadoutGeometry/TRT_DetElementCollection.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "InDetIdentifier/TRT_ID.h"
#include "GeoModelKernel/GeoXF.h"
#include "AthenaKernel/IIOVSvc.h"

#include "CLHEP/Geometry/Transform3D.h"

#include "InDetReadoutGeometry/InDetDD_Defs.h"

#include <vector>
#include <string>
#include <map>

class Identifier;     
class GeoAlignableTransform;
class StoreGateSvc;

namespace InDetDD {

  class TRT_Numerology;
  class ExtendedAlignableTransform;



  /** class TRT_DetectorManager
  
    The Detector Manager for all TRT Detector elements,
    it acts as the interface to the detector elements which can be retrieved
    from the TRT_DetectorManager either via numerology or Identifier access. 
  
    In addition, the alignable transforms are set via Identifer and forwarded
    to the GeoModel class; hence, we are sitting at the interface between
    CLHEP and AMG.
  
    @author Grant Gorfine
     modified & maintined: Nick Styles, Andreas Salzburger
     
  */
  
  class TRT_DetectorManager : public InDetDetectorManager  {
    
  public:
    
    // Constructor
    TRT_DetectorManager(StoreGateSvc * detStore = 0);
    
    // Destructor
    ~TRT_DetectorManager();
    
    /** Access Raw Geometry:-------------------------------------------------------*/
    virtual unsigned int getNumTreeTops()           const;                          //
    virtual PVConstLink  getTreeTop(unsigned int i) const;                         //
    //-----------------------------------------------------------------------------//
    
    /** Get the ID helper: --------------------------------------------------------*/
    void setIdHelper(const TRT_ID * idHelper, bool owns=true);                     //
    //-----------------------------------------------------------------------------//


    //** Get and set information about gasType. -----------------------------------*/
    enum ActiveGasType { unknown, oldgas, newgas };                                //
    ActiveGasType gasType() const;                                                 //
    void setGasType(const ActiveGasType &);                                        //
    //-----------------------------------------------------------------------------//
  
    /** Get and set information about digitization version ------------------------*/
    unsigned int digitizationVersion() const;                                      //
    std::string digitizationVersionName() const;                                   //
    void setDigitizationVersion(const unsigned int &, const std::string& name );   //
    //-----------------------------------------------------------------------------//
  
    /** Access Numerological information:------------------------------------------*/
    TRT_Numerology *getNumerology();                                               //
    const TRT_Numerology * getNumerology() const;                                  //
    //-----------------------------------------------------------------------------//

   
    /** Access Elements Generically------------------------------------------------*/
    const TRT_BaseElement *getElement(Identifier id) const;       // Slow          //
    const TRT_BaseElement *getElement(IdentifierHash id) const;   // Fast          //
    //-----------------------------------------------------------------------------//


    /** Access to Whole Collection of Elements -------------------------------------*/
    const TRT_DetElementCollection * getDetectorElementCollection() const;
    TRT_DetElementCollection::const_iterator getDetectorElementBegin() const;
    TRT_DetElementCollection::const_iterator getDetectorElementEnd() const;

    //-----------------------------------------------------------------------------//

    
    /** Access Barrel Elements:------------------(Fast)----------------------------*/
    const TRT_BarrelElement *getBarrelElement(unsigned int positive,               //
					                          unsigned int moduleIndex,            //
					                          unsigned int phiIndex,               //
					                          unsigned int strawLayerIndex) const; //
    TRT_BarrelElement *getBarrelElement(unsigned int positive,                     //
					                    unsigned int moduleIndex,                  //
					                    unsigned int phiIndex,                     //
					                    unsigned int strawLayerIndex);             //
    //                                                                             //
    //-----------------------------------------------------------------------------//
  
    
    /** Access Endcap Elements:------------------(Fast)-----------------------------*/
    const TRT_EndcapElement *getEndcapElement(unsigned int positive,               //
					                          unsigned int wheelIndex,             //
					                          unsigned int strawLayerIndex,        //
					                          unsigned int phiIndex) const;        //
    TRT_EndcapElement *getEndcapElement(unsigned int positive,                     //
					                    unsigned int wheelIndex,                   //
					                    unsigned int strawLayerIndex,              //
					                    unsigned int phiIndex);                    //
    //                                                                             //
    //-----------------------------------------------------------------------------//

    /** Conditions interface (mostly for internal use):----------------------------*/
    void setConditions(TRT_Conditions * conditions);                               //
    const TRT_Conditions * conditions() const;                                     //
    //-----------------------------------------------------------------------------//    
    
    
    // This is for Detector Descriptors -------------------------------------------//
    void addTreeTop(PVLink);                                                       //
    //                                                                             //
    // Manage the barrel elements:                                                 //
    void manageBarrelElement(TRT_BarrelElement *barrel);                           //
    //                                                                             //
    // Manage the endcap elements:                                                 //
    void manageEndcapElement(TRT_EndcapElement *endcap);                           //
    //                                                                             //
    //-----------------------------------------------------------------------------//
    
    //-----------------------------------------------------------------------------//
    //                                                                             //
    //  Transform of straw relative to module.  Stored as a tranformation field    //
    //  to make this compact.  Note , GeoXF::Function * is a pointer to a so-      //
    //  called "TRANSFUNCTION".  Functions are delete when the manager is deleted. //
    //                                                                             //
    void setBarrelTransformField(size_t i, const GeoXF::Function *field);          //
    const GeoXF::Function *barrelTransformField(size_t i) const;                   //
    void setEndcapTransformField(size_t i, const GeoXF::Function *field);          //
    const GeoXF::Function *endcapTransformField(size_t i) const;                   //
    //                                                                             //
    //-----------------------------------------------------------------------------//

    // Alignment stuff
    // DEPRECATED - kept for compatibilty with Lisbon CondDB
    void addKey(const std::string & key, int level);
    // DEPRECATED use addChannel
    void addKey(const std::string & key, int level, FrameType frame);



    /** Add alignable transforms: GeoModel/CLHEP based */
    void addAlignableTransform (int level,
				const Identifier &id, 
				GeoAlignableTransform *transform,
				const GeoVFullPhysVol * child = 0,
				const GeoVFullPhysVol * frameVol = 0);
				
    /** Add alignable transforms: GeoModel/CLHEP based */
    void addAlignableTransform (int level,
				const Identifier &id, 
				GeoAlignableTransform *transform,
				const GeoVPhysVol * child = 0,
				const GeoVPhysVol * frameVol = 0);

    /** Set alignable transforms: Amg based */
    virtual bool setAlignableTransformDelta(int level, 
				                            const Identifier & id, 
				                            const Amg::Transform3D & delta,
				                            FrameType frame) const;

    /** Set alignable transforms: Amg based */
    bool setAlignableTransformAnyFrameDelta(ExtendedAlignableTransform * extXF, 
					                        const Amg::Transform3D & delta,
					                        FrameType frame) const;


    /** Invalidate cache for all detector elements */
    virtual void invalidateAll() const;
   
    /** Update all caches. */
    virtual void updateAll() const;


    /** Check identifier is for this detector */
    virtual bool identifierBelongs(const Identifier & id) const;
    
    /** Call back for alignment updates, DEPRECATED. Now registered in tool. */
    StatusCode alignmentCallback( IOVSVC_CALLBACK_ARGS );

    /** Process new global DB folders for L1 and L2 **/
    bool processGlobalAlignment(const std::string &, int level, FrameType frame) const;
 
 private:  
  

    // Illegal operations:---------------------------------------------------------//
    //                                                                             //
    const TRT_DetectorManager & operator=(const TRT_DetectorManager &right);       //
    TRT_DetectorManager(const TRT_DetectorManager &right);                         //
    //                                                                             //
    //-----------------------------------------------------------------------------//
    
    
    // Private member data:--------------------------------------------------------//
    std::vector<PVLink> m_volume;                                                  //
    //                                                                             //
    enum {NMODMAX=3};                                                              //
    enum {NWHEELMAX=18};                                                           //
    enum {NPHIMAX=32};                                                             //
    enum {NSTRAWLAYMAXBR=30};                                                      //
    enum {NSTRAWLAYMAXEC=16};                                                      //
    //                                                                             //
    TRT_BarrelElement *m_barrelArray[2][NMODMAX][NPHIMAX][NSTRAWLAYMAXBR];         //
    TRT_EndcapElement *m_endcapArray[2][NWHEELMAX][NSTRAWLAYMAXEC][NPHIMAX];       //
    TRT_DetElementCollection m_elements;                                           //     
    //                                                                             //
    TRT_Numerology  *m_numerology;                                                 //
    const TRT_ID    *m_idHelper;                                                   //
    bool             m_ownsIdHelper;                                               //
    const GeoXF::Function *m_barrelXF[3];                                          //
    const GeoXF::Function *m_endcapXF[3];                                          //
    //                                                                             //
    ActiveGasType m_gasType;                                                       //
    unsigned int m_digvers;                                                        //
    std::string m_digversname;                                                     //

    TRT_Conditions * m_conditions;

    // Alignment stuff
    typedef std::map<Identifier, ExtendedAlignableTransform *> AlignableTransformMap;
    std::vector< AlignableTransformMap > m_alignableTransforms;

    // here temporarily
    virtual const TRT_ID *getIdHelper() const;                                             //
    virtual bool processSpecialAlignment(const std::string & key, InDetDD::AlignFolderType dummy) const;

    //-----------------------------------------------------------------------------//
  };
}
#ifndef GAUDI_NEUTRAL
#include "CLIDSvc/CLASS_DEF.h" 
CLASS_DEF(InDetDD::TRT_DetectorManager, 10907524, 1)
#endif
#endif
  
  
