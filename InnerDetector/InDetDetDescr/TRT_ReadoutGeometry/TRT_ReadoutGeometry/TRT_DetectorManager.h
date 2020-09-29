/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_DetectorManager.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRT_DetectorManager_h
#define TRT_DetectorManager_h 1
#include "InDetReadoutGeometry/InDetDetectorManager.h"
#include "TRT_ReadoutGeometry/TRT_Conditions.h"
#include "TRT_ReadoutGeometry/TRT_BarrelElement.h"
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"
#include "InDetReadoutGeometry/Version.h"
#include "TRT_ReadoutGeometry/TRT_DetElementCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "InDetIdentifier/TRT_ID.h"
#include "GeoModelKernel/GeoXF.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "CLHEP/Geometry/Transform3D.h"

#include "InDetReadoutGeometry/InDetDD_Defs.h"

#include "CxxUtils/checker_macros.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

class Identifier;     
class GeoAlignableTransform;
class GeoVAlignmentStore;
class StoreGateSvc;
class CondAttrListCollection;

namespace InDetDD {

  class ExtendedAlignableTransform;
  class TRT_BarrelDescriptor;
  class TRT_EndcapDescriptor;
  class TRT_Numerology;

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
    virtual unsigned int getNumTreeTops()           const override;                 //
    virtual PVConstLink  getTreeTop(unsigned int i) const override;                 //
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
    void addKey ATLAS_NOT_THREAD_SAFE (const std::string & key, int level);
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
                                            FrameType frame,
                                            GeoVAlignmentStore* alignStore) const override;

    /** Set alignable transforms: Amg based */
    bool setAlignableTransformAnyFrameDelta(ExtendedAlignableTransform * extXF, 
                                            const Amg::Transform3D & delta,
                                            FrameType frame,
                                            GeoVAlignmentStore* alignStore) const;


    /** Invalidate cache for all detector elements */
    virtual void invalidateAll() const override;
   
    /** Update all caches. */
    virtual void updateAll() const override;


    /** Check identifier is for this detector */
    virtual bool identifierBelongs(const Identifier & id) const override;
    
    /** Call back for alignment updates, DEPRECATED. Now registered in tool. */
    StatusCode alignmentCallback( IOVSVC_CALLBACK_ARGS );

    /** Process new global DB folders for L1 and L2 **/
    virtual
    bool processGlobalAlignment(const std::string &, int level, FrameType frame,
                                const CondAttrListCollection* obj,
                                GeoVAlignmentStore* alignStore) const override;

    bool processSpecialAlignment(const std::string & key, InDetDD::AlignFolderType dummy) const override;

    bool processSpecialAlignment(const std::string& key,
                                 const CondAttrListCollection* obj=nullptr,
                                 GeoVAlignmentStore* alignStore=nullptr) const override;

    /** Set TRT_Barrel/EndcapDescriptor pointer to the internal sets to delete them in the destructor */
    void setBarrelDescriptor(const TRT_BarrelDescriptor* barrelDescriptor);
    void setEndcapDescriptor(const TRT_EndcapDescriptor* endcapDescriptor);

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

    std::unique_ptr<TRT_Conditions> m_conditions;

    // Alignment stuff
    typedef std::map<Identifier, ExtendedAlignableTransform *> AlignableTransformMap;
    std::vector< AlignableTransformMap > m_alignableTransforms;

    // Descriptors are owned by TRT_DetectorManager.
    std::set<const TRT_BarrelDescriptor*> m_barrelDescriptors;
    std::set<const TRT_EndcapDescriptor*> m_endcapDescriptors;

    // here temporarily
    virtual const TRT_ID *getIdHelper() const override;                                    //

    //-----------------------------------------------------------------------------//
  };
}
#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h" 
CLASS_DEF(InDetDD::TRT_DetectorManager, 10907524, 1)
#endif
#endif
  
  
