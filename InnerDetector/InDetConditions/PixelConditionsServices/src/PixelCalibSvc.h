/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PIXELCALIBSVC_PIXELCALIBSVC_H
#define PIXELCALIBSVC_PIXELCALIBSVC_H

//#include "GaudiKernel/Service.h"
#include "AthenaBaseComps/AthService.h"
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "PixelConditionsData/PixelCalibDataColl.h"
#include "PixelGeoModel/IBLParameterSvc.h"


class AtlasDetectorID;
class Identifier;
class StoreGateSvc; 
class PixelID; 
//class IPixelCablingSvc;

class IPixelCalibDbTool;

  /** @class PixelCalibSvc 

   The PixelCalibSvc provides a set of services to access pixel calibration data objects in the detector store. 
   The creation of pixel calibration data is handled by PixelCalibDbTool via a callback function from the 
   cool database.
  
  @author  Weiming Yao <WMYAO@LBL.GOV>
  */  

class PixelCalibSvc : public AthService, virtual public IPixelCalibSvc
{
    public:
      PixelCalibSvc(const std::string& name, ISvcLocator* sl); 

       /** default destructor */
      virtual ~PixelCalibSvc ();

      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF); 

       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();

      /** access to db constants with wafer_id: irow, icol per module index */ 
      virtual  bool hasCalibData(const Identifier& wafer_id) const; //<! true for wafer_id contains the calibration data 
      virtual int getNFE(const Identifier& wafer_id) const; // number of FE Chips per module 
      virtual bool isFEI4(const Identifier& wafer_id) const; // is FEI4 module      

      /** Note: irow icol is for each chip, not phi_index, eta_index per module */ 
      virtual int getThreshold(const Identifier& wafer_id, int irow, int icol, int circ) const; //<! get threshold 
      virtual int getThresholdSigma(const Identifier& wafer_id, int irow, int icol,int circ) const; //<<! get threshold sigma
      virtual int getNoise(const Identifier& wafer_id, int irow, int icol, int circ) const; //<! get Noise 
      virtual int getTimeWalk(const Identifier& wafer_id, int irow, int icol, int circ) const; //<! get  time walk 
      virtual float getQ2TotA(const Identifier& wafer_id, int irow, int icol, int circ) const; //<! get Q tot conversion constant A
      virtual float getQ2TotE(const Identifier& wafer_id, int irow, int icol, int circ) const;//<! get tot conversion constant E
      virtual float getQ2TotC(const Identifier& wafer_id, int irow, int icol, int circ) const; //<! get tot conversion constant C
      virtual float getTotP1(const Identifier& wafer_id, int circ) const; //<! get tot resolution parameter P1
      virtual float getTotP2(const Identifier& wafer_id, int circ) const; //<! get tot resolution parameter P2
      virtual float getTotRes(const Identifier& wafer_id, int circ, float Q) const; //<! get tot resolution

      /**  access to db with pix_id: */ 
      virtual int getThreshold(const Identifier& pix_id) const;
      virtual int getThresholdSigma(const Identifier& pix_id) const; 
      virtual int getNoise(const Identifier& pix_id) const; 
      virtual int getTimeWalk(const Identifier& pix_id) const; 
      virtual float getQ2TotA(const Identifier& pix_id) const; 
      virtual float getQ2TotE(const Identifier& pix_id) const;
      virtual float getQ2TotC(const Identifier& pix_id) const; 
      virtual float getTotP1(const Identifier& pix_id) const;
      virtual float getTotP2(const Identifier& pix_id) const;
      virtual float getTotRes(const Identifier& pix_id, float Q) const;
      virtual float getTotMean(const Identifier& pix_id, float Q) const;
      virtual void disableDb() {m_disableDb = true;}
    private:

      int PixelType(const Identifier& wafer_id, int irow, int icol) const; // type of pixel
      int PixelType(const Identifier& pix_id, const Identifier& wafer_id, int & circ) const; // type of pixel
      int PixelCirc(const Identifier& pix_id, const Identifier& wafer_id) const; // the chip number circ 
      int gangedType(int row, int col, const unsigned int& columnsPerFE, const unsigned int& rowsFGanged, const unsigned int& rowsLGanged) const; // ganged pixel type 


      /** class member version of retrieving MsgStream */
      ServiceHandle<StoreGateSvc>      m_sgSvc;
      ServiceHandle<StoreGateSvc>      m_detStore;
      ToolHandle<IPixelCalibDbTool>    m_dbTool;
      const PixelID*                   m_pixid;
      //      ServiceHandle<IPixelCablingSvc>  m_pixidmap; 
      mutable PixelCalib::PixelCalibData*  m_pat; //<! save the calibration pointer after updating 
      mutable Identifier                   m_wafer_id; //<! wafer_id  
        
      // int/double/bool  m_propertyName;
      double                    m_totparA;
      double                    m_totparE;
      double                    m_totparC;
      double                    m_totparP1; /**< Correction for dispersion */
      double                    m_totparP2; 
      int                       m_discrThresh;          /**< discriminator threshold value */
      int                       m_discrThreshIBL;          /**< discriminator threshold value */
      int                       m_discrThreshSigma;     /**< discriminator threshold sigma value */
      int                       m_intimeThresh;         /**< intime threshold */
      int                       m_intimeThreshIBL;         /**< intime threshold */
      int                       m_noiseThresh;          /**< threshold noise*/
      bool m_IBLabsent;
      bool m_disableDb;
      ServiceHandle<IBLParameterSvc> m_IBLParameterSvc; 
      
    }; 



#endif 
