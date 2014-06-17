/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAGFIELDATHENASVC
#define MAGFIELDATHENASVC

#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/StoreGateSvc.h"

/////////////////////////////////////////////////////////////////////////////

#include "BFieldCore/MagFieldParam.h" 
#include "BFieldAth/MagFieldAthena.h"
#include "BFieldAth/IMagFieldAthenaSvc.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

template <class TYPE> class SvcFactory;
class IGeoModelSvc;
class IRDBAccessSvc;
class CondAttrListCollection;

class MagFieldAthenaSvc : public Service , virtual public IMagFieldAthenaSvc {

public:
///////////////////////////////////

  static const InterfaceID& interfaceID() { return IID_IMagFieldAthenaSvc; }

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  MagFieldAthena* GetMagFieldAthena();
  // retrieve through COOL callback - to be used in client callback method
  MagFieldAthena* GetUpdatedMagFieldAthena();
  // check if field was really updated in callback, to be used in clients
  bool WasFieldUpdated();

  // Vakho: This method does mag field initialization. It gets called in two
  //        different ways:
  //        1. When GeoModelSvc is configured from job options, then initGeoModel
  //           is called at initialization phase
  //        2. When GeoModelSvc is configured automaticaly from TagInfo, then
  //           initGeoModelSvc is registered as callback and is called after GeoModelSvc
  //           initialization has been done
  StatusCode initGeoModel(IOVSVC_CALLBACK_ARGS);
  StatusCode initGeoModel2(IOVSVC_CALLBACK_ARGS);
  StatusCode updateField(IOVSVC_CALLBACK_ARGS);

  /// templated function for registering a callback
  template <typename T1>
  StatusCode regFcn( StatusCode (T1::*fcn)(IOVSVC_CALLBACK_ARGS),
		      const T1* obj);

  friend class SvcFactory<MagFieldAthenaSvc>;

///////////////////////////////////
protected:
  // Standard Constructor
  MagFieldAthenaSvc(const std::string& name, ISvcLocator* svc);

  // Standard Destructor
  virtual ~MagFieldAthenaSvc();

///////////////////////////////////
private:

// private functions

//  get stuff to run
    void getTagOracle (StatusCode &Sc);
    void getTagOracle2 (StatusCode &Sc);
    void getBfieldFileName (const IGeoModelSvc * geoModel, IRDBAccessSvc * accessSvc);
    void getBfieldFileName2(const IGeoModelSvc * geoModel, IRDBAccessSvc * accessSvc);
    void getBfieldDisplacements (const IGeoModelSvc * geoModel, IRDBAccessSvc * accessSvc);
    void getCurrent (const IGeoModelSvc * geoModel, IRDBAccessSvc * accessSvc);
    bool isThatFileExists( const std::string & );
    // Vakho: This method performs actuall initialization of the Magnetic field based
    //        on m_param values
    const IGeoModelSvc* geoModel;
    StatusCode initMagField();

    bool checkTol(const double xo, const double xc);
    std::string decodePath(const std::string& path);
    StatusCode chooseParamFromCOOL(const float reqSolCur,const float reqTorCur,
				   MagFieldParam& param);

    // job option parameters
    MagFieldParam par_param; // managed by PropertySvc
    std::string m_alreadyReadFile;

    // currents set by HLT framework for online running
    double par_onlineSolCur;
    double par_onlineTorCur;
    bool par_setupcool;  // flag to setup COOL callback infrastructure
    bool par_usedcs;     // include DCS currents when using COOL
    float par_scaletolerance; // minimum fractional deviation to scale field
    float par_zerotolerance; // current below which considered zero (amps)
    float par_currenttolerance; // absolute change in currents which 
                                // triggers field update (amps)
    float par_maptolerance; // maximum acceptable deviation (fractional, S+T)
                            // between map and real currents to accept map
    bool par_crashmap; // crash (return FATAL) if acceptable deviation exceeded
                       // otherwise just print WARNING
    // fixed parameters
    const std::string m_coolBFieldMap;
    const std::string m_coolCurrents;
    const float m_refSolCur; // refsolenoid and toriod currents for norml
    const float m_refTorCur;

    // internal variables
    StoreGateSvc* p_detStore;
    MagFieldAthena* p_MagFieldAthena; // pointer to BField itself
    // Datahandles for the conditions data objects
    const DataHandle<CondAttrListCollection> h_coolBFieldMap;
    const DataHandle<CondAttrListCollection> h_coolCurrents;

    MagFieldParam m_param; // parameters in use for actual field
    bool alreadyReadFile; 
    int semiAnSol;

    bool m_gotcooldata;      // COOL-based field retrieve has been done
    bool m_coolupdated;      // Last callback actually updated the field

    // counters to count number of retrievals of MagFieldAthena through
    // old and new interfaces, at initialize and in event loop
    int m_getbad; // counter number of bad retrievals at initialise, old int
    int m_getgood; // counter number of good retrieves, old interface
    int m_getcoolbad; // counter number of bad retrieves with COOL
    int m_getcoolgood; // counter number of good retrives with COOL

    bool m_useFieldService;
    ServiceHandle<MagField::IMagFieldSvc> m_fieldService;
};

template <typename T>
StatusCode MagFieldAthenaSvc::regFcn(
     StatusCode (T::*fcn)(IOVSVC_CALLBACK_ARGS),const T* obj) {
  return p_detStore->regFcn(&MagFieldAthenaSvc::updateField,this,fcn,obj);
}


#endif

