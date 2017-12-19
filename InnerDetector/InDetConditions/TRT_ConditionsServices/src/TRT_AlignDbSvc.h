/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_ALIGNDBSVC_H
#define TRT_ALIGNDBSVC_H 

/** @file TRT_AlignDbSvc.h
 * @brief a Service to manage TRT alignment conditions
 * @author John Alison, Peter Hansen <phansen@nbi.dk>, Wouter Hulsbergen
 * 
 * TRT alignment transforms are built up like this:
 *
 *    aligned = level-2 * level-1 * nominal
 *
 * The 'level-2' correction is the global transform. There is a separate
 * transform for each 'bec'. However, note that the barrel is bec=-1:
 * the +1 is ignored in the geometry.
 * 
 * The 'level-1' transform is a transform per module.
 */

#include <vector>
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"
#include "TRT_ConditionsServices/ITRT_AlignDbSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawAlignDbSvc.h"
#include "DetDescrConditions/AlignableTransformContainer.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

namespace InDetDD {
  class TRT_DetectorManager ;
}
class IAthenaOutputStreamTool;
class TRT_ID ;

/** @class TRT_AlignDbSvc
    Service to manage TRT alignment data
*/
class TRT_AlignDbSvc: public AthService, virtual public ITRT_AlignDbSvc
{
 public:
  
  /// constructor
  TRT_AlignDbSvc( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// destructor
  virtual ~TRT_AlignDbSvc();
  
  /// tool initialize
  virtual StatusCode initialize();

  /// tool finalize
  virtual StatusCode finalize();

  // TOOL METHODS
  /// access to interfaceID
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ) ;

  /** Call back function for alignment folders */
  StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS);

  /** write AlignableTransforms to flat text file */
  StatusCode writeAlignTextFile(std::string file) const;
  StatusCode writeStrawAlignTextFile(std::string file) const;
  StatusCode writeGlobalFolderFile(std::string file) const;  

  /** read AlignableTransforms from text file into TDS */
  StatusCode readAlignTextFile(std::string file);

  /** write the alignment objects to output */
  StatusCode streamOutAlignObjects() const;
  
  /** register alignment objects with the IoV service */
  StatusCode registerAlignObjects(std::string tag, int run1, int event1, int run2, int event2) const;
    
  /** set AlignableTransform for an identifier */
  StatusCode setAlignTransform(Identifier ident, Amg::Transform3D trans, unsigned int level);

  /** tweak AlignableTransform for an identifier */
  StatusCode tweakAlignTransform(Identifier ident, Amg::Transform3D trans, unsigned int level);

  /** get AlignableTransform for an identifier */
  const Amg::Transform3D getAlignmentTransform(const Identifier& ident, unsigned int level) const;
  const Amg::Transform3D* getAlignmentTransformPtr(const Identifier& ident, unsigned int level) const;
  
  /** tweak L1 DB for global folders for an identifier */
  StatusCode tweakGlobalFolder(Identifier ident, Amg::Transform3D trans ); 

 private:  

  /** set Level 1 AlignableTransform for an identifier */
  StatusCode setAlignTransformL1(Identifier ident, Amg::Transform3D trans);
  
  /** set Level 2 AlignableTransform for an identifier */
  StatusCode setAlignTransformL2(Identifier ident, Amg::Transform3D trans);

  /** set Level 3 AlignableTransform for an identifier */
  StatusCode setAlignTransformL3(Identifier ident, Amg::Transform3D trans);
  
  /** get Level 1 AlignableTransform for an identifier */
  const Amg::Transform3D getAlignmentTransformL1(Identifier const& ident) const ;
  const Amg::Transform3D* getAlignmentTransformL1Ptr(Identifier const& ident) const ;

  /** get Level 2 AlignableTransform for an identifier */
  const Amg::Transform3D getAlignmentTransformL2(Identifier const& ident) const ;
  const Amg::Transform3D* getAlignmentTransformL2Ptr(Identifier const& ident) const ;

  /** tweak Level 1 AlignableTransform for an identifier */
  StatusCode tweakAlignTransformL1(Identifier ident, Amg::Transform3D trans);

  /** tweak Level 2 AlignableTransform for an identifier */
  StatusCode tweakAlignTransformL2(Identifier ident, Amg::Transform3D trans);

  /** tweak Level 3 AlignableTransform for an identifier */
  StatusCode tweakAlignTransformL3(Identifier ident, Amg::Transform3D trans);

  /** get AlignableTransform pointer for an object key */
  AlignableTransform* getTransPtr(const std::string key) const;

  /** get const AlignableTransform pointer for an object key */
  const AlignableTransform* cgetTransPtr(const std::string key) const;

  /** Returns the ring for a given strawLayer */
  int getRingForStrawLayer(int strawlayer) const;
  
  /** Returns the true if the input key is from the old endcap scheme*/
  bool isOldKey(std::string input) const;

  /** Return the prefix tag for a given calibration folder */
  std::string prefixtag(std::string key) const;

  /** Convert from an int to a string */
  std::string intToString(int input) const;

  /** Output the transform to the cout, for debugging */
  void printTransform(std::string thisName,  const Amg::Transform3D& transform) const;

  /** Output the conditions objects currently in memory */
  void printCondObjects() const ;

  /** Create an empty set of AlignableTransforms for the GeoModel setup */
  StatusCode createAlignObjects() const;
  
  /** Create an empty set of AlignableTransforms for the transforms 
      which are not created by XXXXXXX from the conddb  */
  StatusCode createAlignObjectsWhichDoNotAlreadyExist();

  /** Return the object key for a given identifier and data type */
  std::string findkey(const Identifier& ident, std::string type) const;

  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<ITRT_StrawAlignDbSvc> m_trtStrawAlignDbSvc;
  const TRT_ID* m_trtid;                 //!< trt id helper
  const InDetDD::TRT_DetectorManager* m_trtman; //!< detector manager
  std::string m_alignroot;               //!< root alignment folder
  std::string m_alignString;               //!< alignment string searched for
  std::string m_par_alitextfile;         //!< input text file
  std::vector<std::string> m_alignobjs;  //!< folder names
  std::vector<int> m_alignchans;         //!< channels
  const DataHandle<AlignableTransformContainer> m_aligncontainerhandle;
  ToolHandle<IAthenaOutputStreamTool> m_streamer; //!< OutputStreamTool
  mutable std::vector<Amg::Transform3D*> m_amgTransformCache;

  std::string m_alignDBprefix;  // Not very elegant hack to make more modular for DB modifications

  bool m_dynamicDB;
  bool m_forceUserDBConfig;     // DB folder scheme is auto-config now; This allows user override
};

inline StatusCode TRT_AlignDbSvc::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == ITRT_AlignDbSvc::interfaceID() )  {
    *ppvIf = dynamic_cast<ITRT_AlignDbSvc*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  return AthService::queryInterface( riid, ppvIf );
}

#endif 
