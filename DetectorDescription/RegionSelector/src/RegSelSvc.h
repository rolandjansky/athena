// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef REGSELSVC_H
#define REGSELSVC_H

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/StatusCode.h"
#include <string>
#include <iostream>
#include <vector>
#include <stdint.h>
#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/RegSelEnums.h"
#include "RegionSelector/RegSelectorMap.h"
#include "RegionSelector/RegSelectorHashMap.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "RegionSelector/IRegionLUT_Creator.h"
#include "RegSelLUT/IRegionIDLUT_Creator.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"

#include "IRegionSelector/IRoiDescriptor.h"


class StoreGateSvc;

/** @class RegSelSvc RegSelSvc.h
 
     This is a Region Selector service for Athena.
     It is not a singleton, to allow flexibility at config time.
     It is possible to configure one instance per thread of one shared 
     between each thread using the Gaudi MT extensions.

     See http://proj-gaudi.web.cern.ch/proj-gaudi/GDG/v2/GDG.pdf pp106-108
     - out of date. See for example
     GaudiKernel/.../GaudiKernel/IToolSvc.h
     GaudiSvc/.../src/ToolSvc/ToolSvc.cpp
     GaudiSvc/.../src/ToolSvc/ToolSvc.h

     It still needs to be made thread-safe.

     @author Simon George
*/

class RegSelSvc: public extends2<AthService, IRegSelSvc, IIncidentListener>
{
 public:
  /** @c Standard constructor for Gaudi services.
   */
  RegSelSvc(const std::string& name, ISvcLocator* sl);
  //! Destructor.
  virtual ~RegSelSvc();
  /** @c queryInterface needed for Gaudi services
   */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

  //! @method reinitialize standard Service interface
  /*!
      Read list of pixel and sct robids to be initialised from OKS
      and anable the corresponding modules
  */
  StatusCode reinitialize();

  bool reinitialiseInternal();
  bool reinitialiseInternalFromOKS();
  bool GetEnabledROBsFromOKS(); 

  //! @method initialize standard Service interface
  /*!
    Load detector description tables for all detectors loaded in Constructor. 
    Internal maps for retrieve %Identifier %Hash and ROBID information are loaded. 
    Calorimeters are loaded in hash tables.
    \sa readFromSG, initCalo and initMuon
  */
  StatusCode initialize();

  /** beginRun is when all the tables are initialized */
  // hmmm, how should this be done? IncidentSvc, or 
  // DetectorStore regFcn
  virtual void handle(const Incident& incident);

  // needed for backwards cal compatability (!)
  StatusCode handle(IOVSVC_CALLBACK_ARGS);

  void testSuper(bool robs=false);

  //! Initialise Inner Detector
  bool handleID(); 

  //! Initialise Calo
  bool handleCalo(); 

  // StatusCode handleCalo(IOVSVC_CALLBACK_ARGS);

  //! @method finalize  standard Service interface, deletes all maps from memory
  StatusCode finalize();
  
  // Interface inherited from IRegSelSvc service


  void DetHashIDList(DETID detectorID,
		     const IRoiDescriptor& roi,
		     std::vector<IdentifierHash>& IDList);

  void DetHashIDList(DETID detectorID, long layer,
		     const IRoiDescriptor& roi,
		     std::vector<IdentifierHash>& IDList);

  // full scan
  void DetHashIDList(DETID detectorID, 
		     std::vector<IdentifierHash>& IDList );

  // full scan for a specific layer
  void DetHashIDList(DETID detectorID, long layer,
		     std::vector<IdentifierHash>& IDList );



  // Methods to obtain the rob id list


  void DetROBIDListUint(DETID detectorID,
			const IRoiDescriptor& roi,
			std::vector<uint32_t>& outputROBIDList);


  void DetROBIDListUint(DETID detectorID, long layer, 
			const IRoiDescriptor& roi, 
			std::vector<uint32_t>& outputROBIDList);

  // inner detector full scan
  void DetROBIDListUint(DETID detectorID, 
			std::vector<uint32_t>&    outputROBIDList );

  // inner detector full scan by layer
  void DetROBIDListUint(DETID detectorID, long layer, 
			std::vector<uint32_t>&    outputROBIDList );



  // Utilities

  void GetEtaPhi(DETID detectorID, 
		 IdentifierHash hashId, 
		 double *etaMin, double *etaMax,
		 double *phiMin, double *phiMax );
    
  void getRoIData(DETID detectorID, 
		  const IRoiDescriptor& roi, 
		  std::vector<const RegSelModule*>& modulelist );

  //! Boundary values returns \f$ \eta \f$ and \f$ \phi \f$ boundary values of the detector specified by DETID.
  /*!
    This function can be used, for example, to verify if a specific detector has a \f$ ( 0 < \phi < 2\pi
    )\f$ or \f$ ( -\pi < \phi < \pi )\f$, and also to verify \f$ \eta \f$ boudaries of the loaded table.
  */
  void boundaryValues(DETID detectorID,
		      double &etaminValue, double &etamaxValue,
		      double &phiminValue, double &phimaxValue);
	

  //! disable modules belonging to the robs in the roblist 
  void DisableSiRobList(const std::vector<uint32_t>& RobIdList);

  //! disable pixel modules with the identifiers from list 
  //void DisablePixelHashList(const std::vector<IdentifierHash>& HashList);
  void DisablePixelHashList(const std::vector<unsigned int>& HashList);

  //! disable sct modules with the identifiers from list 
  //  void DisableSCTHashList(const std::vector<IdentifierHash>& HashList);
  void DisableSCTHashList(const std::vector<unsigned int>& HashList);

  //! disable sct modules with the identifiers from list 
  void DisableTRTHashList(const std::vector<unsigned int>& HashList);

  //! reinstate all modules in the lookup tables
  void RestoreSiRobList();


  //  void disablePixelFromConditions();
  //  void disableSCTFromConditions();
  //  void disableTRTFromConditions();

  void disableIDFromConditions(RegSelSiLUT* detector, const std::string& serviceName);


  //! get information on modules by hashid
  const RegSelModule* pixelModule(unsigned int hashid) const { return m_newpixel->Module(hashid); }  
  const RegSelModule* sctModule(unsigned int hashid)   const { return m_newsct->Module(hashid); }  

  const RegSelModule* mdtModule(unsigned int hashid) const { return m_newmdt->Module(hashid); }  
  const RegSelModule* cscModule(unsigned int hashid) const { return m_newcsc->Module(hashid); }  
  const RegSelModule* rpcModule(unsigned int hashid) const { return m_newrpc->Module(hashid); }  
  const RegSelModule* tgcModule(unsigned int hashid) const { return m_newtgc->Module(hashid); }  

  double getDeltaZ() const { return m_DeltaZ; }

private:
  //! %A private variable to read data from StoreGate.
  ServiceHandle<StoreGateSvc> m_detStore; 

  //! Initialise lookup tables using tools provided for each detector
  /*!
     \param ToolHandle<IRegionLUT_Creator> \c \b p_lutCreatorTool, tool (from properties) to be used to initialise the LUT
     \param const std::string \c \b lutName, parameter to be passed to lutCreatorTool.getLUT() if needed, optional
     \return const RegionSelectorLUT* \c \b detRSlut, the detector data LUT
  */
  StatusCode readFromSG( ToolHandle<IRegionLUT_Creator> p_lutCreatorTool, const RegionSelectorLUT*& detRSlut, const std::string lutName=""); 

  //! Initialise lookup tables using tools provided for the inner detector tables
  /*!
     \param ToolHandle<IRegionIDLUT_Creator> \c \b p_lutCreatorTool, tool (from properties) to be used to initialise the LUT
     \param const std::string \c \b lutName, parameter to be passed to lutCreatorTool.getLUT() if needed, optional
     \return const RegSelSiLUT* \c \b detRSlut, the detector data LUT
  */
  StatusCode readFromSG( ToolHandle<IRegionIDLUT_Creator>  p_lutCreatorTool, RegSelSiLUT*& detRSlut); 

  //! Uses m_detStore and RegionSelectorLUT detector key, specified by DETID type, to read from %StoreGate the necessary detector data. 
  /*!
     \param DETID \c \b type, an enum to define subdetectors
     \return const RegionSelectorLUT* \c \b detRSlut, the detector data LUT
  */
  StatusCode readFromSG( DETID type, const RegionSelectorLUT*& detRSlut ); 
  
  //! Mount Region Selector maps
  /*!
    This function works only with Inner Detectors and Calorimeters, not Muon Detectors.
    \param DETID \c \b type, an enum to define subdetectors
    \param const RegionSelectorLUT* \c \b detRSlut, the detector data LUT
    \return m_detStore loaded with relevant data to mount detectors internal tables. 
   */
  StatusCode addLUT(DETID type, const RegionSelectorLUT* detRSlut);

  //! An enum to std::string converter. The std::string corresponds to the subdetector store key in %StoreGate.
  /*!
     \param DETID \c \b type, an enum to define subdetectors
     \return std::string that is the %StoreGate key to that subdetector. 
  */
  std::string returnKeyString( DETID type );
  
  //! Calorimeters initialization. All data are loaded to private variables. 
  /*!
     \return StatusCode returns the status codes from appropriate routines, that can be SUCCESS or FAILURE. 
     initCalo reads from Files all Calo information
     initTileCalo reads from Files only Tile Calo information

  */
  StatusCode initCalo(void);
  StatusCode initTileCalo(void);
  
  //! Muons initialization. All data are loaded to private variables. 
  /*!
     \return StatusCode returns the status codes from appropriate routines, that can be SUCCESS or FAILURE. 
  */
  StatusCode initMuon(void);
  
  //! Verify the status of subdetectors initialization. If SUCCESS load OUTTYPE enum into Region Selector internal maps. 
  /*!
     \param StatusCode returns the status codes from appropriate routines, that can be SUCCESS or FAILURE.
     \param DETID \c \b type, an enum to define subdetectors
     \return RegSelectorMap& \c \b subDetData, data structure for maps of %Identifier %Hash and ROBIDs. 
  */
  void openDataStatus(StatusCode &sc, DETID type, 
		      RegSelectorMap &subDetData);

  //! Verify the status of subdetectors initialization. If SUCCESS load OUTTYPE enum into Region Selector internal maps. 
  /*!
     \param StatusCode returns the status codes from appropriate routines, that can be SUCCESS or FAILURE.
     \param DETID \c \b type, an enum to define subdetectors
     \return RegSelectorHashMap& \c \b subDetData, data structure for hash maps of %Identifier %Hash and ROBIDs. 
  */
  void openDataStatus(StatusCode &sc, DETID type,
		      RegSelectorHashMap &subDetHashData);
		
  //! Verify inputs before calling selection function. If \f$ \eta \f$ or \f$ \phi \f$ are outside boundary values they are adjusted to be inside \f$ \eta \f$ and \f$ \phi \f$ ranges.
  /*!
    All \f$ \eta \f$ or \f$ \phi \f$ inputs can have their values changed by this function. 
    If \f$ \eta \f$ is out of range 
     \param DETID \c \b Type, an enum to define subdetectors
     \param double& \c \b etaminIn, lower boundary of \f$ \eta \f$ range. 
     \param double& \c \b etamaxIn, higher boundary of \f$ \eta \f$ range.
     \param double& \c \b phiminIn, lower boundary of \f$ \phi \f$ range.
     \param double& \c \b phimaxIn, higher boundary of \f$ \phi \f$ range.     
  */
  void verifyInputs(DETID Type, double &etaminIn, double &etamaxIn,
		    double &phiminIn, double &phimaxIn);
		    
  //! Dump in a data file of all loaded tables. Each loaded subdetector is loaded in a separate file.
  /*!
    This function is controlled by jobOptions flag PrintTable = TRUE
     \param std::vector<std::string>& \c \b detTypeStr, is vector holding the subdetector names (Inner, Calorimeter, Muon) to be dumped out 
  */
  void dumpLoadedTables(std::vector<std::string>& enabledDetectors);

  //! Dump in a data file of all loaded tables. Each loaded subdetector is loaded in a separate file.
  /*!
    This function is set by jobOptions flag PrintTable = TRUE
     \param std::string& \c \b detTypeStr, is a std::string holding the subdetector name (Inner, Calorimeter, Muon or All) 
  */
  void dumpLoadedTables(const std::string& detTypeStr);
  
  //! Retrieve subdetector filename from jobOptions file, adding subdetector detTypeStr in file name.
  /*!
    In jobOptions the user can set OutputFile = "filename.txt". This function changes this name
    according to detTypeStr. For example, if detTypeStr = "Inner", then the names of dumped files
    will be: filenamePIXEL.txt, filenameSCT.txt and filenameTRT.txt. 
     \param std::string& \c \b detTypeStr, is a std::string holding the subdetector name (Inner, Calorimeter, Muon or All) 
     \return std::vector<std::string>& \c \b filename, vector containing all file names
     \return std::vector<std::string>& \c \b detname, vector containing all subdetector names
  */
  void getFilenames(const std::string& detTypeStr, std::vector<std::string>& filename, std::vector<std::string>& detName);
  
  //! Retrieve subdetector name from detTypeStr std::string.
  /*!
    This function returns all subdetector names listed in a vector. For example, if detTypeStr is
    Inner, detName vector has "PIXEL", "SCT" and "TRT" in output vector. 
     \param std::string& \c \b detTypeStr, is a std::string holding the subdetector name (Inner, Calorimeter, Muon or All) 
     \return std::vector<std::string>& \c \b detname, vector containing all subdetector names
  */
  void getDetname(const std::string& detTypeStr, std::vector<std::string>& detName);
  
  //! Retrieve subdetector name from detTypeStr std::string.
  /*!
     Dump in files all data loaded for subdetectors. 
     \param std::ofstream& \c \b fileout, name of the output file
     \param std::string& \c \b detName, subdetector name.
     \param std::string& \c \b detTypeStr, subdetector type, i.e., Inner, Calorimeter, Muon or All.
  */
  void writeFile(std::ofstream& fileout, const std::string& detName, const std::string& detTypeStr);
  
  //! Retrieve from internal data structure (private member) all information from detName subdetector. This function is used only for Inner Detectors and Muons
  /*!
     Dump in files all data loaded for subdetectors. 
     \param std::string& \c \b detName, subdetector name.
     \return RegSelectorMap& \c \b rsData, internal data structure for detName subdetector.
  */
  void switchDetector(const std::string& detName, RegSelectorMap& rsData);
  
  //! Retrieve from internal data structure (private member) all information from detName subdetector. This function is used only for Calorimeters.
  /*!
     Dump in files all data loaded for subdetectors. 
     \param std::string& \c \b detName, subdetector name.
     \return RegSelectorHashMap& \c \b rsDataCalo, internal data structure for detName subdetector.
  */
  void switchDetector(const std::string& detName, RegSelectorHashMap& rsDataCalo);

  //! Helper method to print tables retrieved from SG-DetStore at debug level
  /*! \param RegionSelectorLUT* \c \b lut, look up table to print
   */
  void printTable(const RegionSelectorLUT * lut);



private: 

  void setDuplicateRemoval( bool b=true ) { m_duplicateRemoval=b; }

private:

  //! Specifies the type of selection: wedge, cone or helical.
  /*!
    \warning At the moment this std::string is not being used, because only wedge selection is implemented. 
  */
  StringProperty  m_regionType;  
  
  //! Specifies the detector description version. 
  /*! \note This is only used to select muon maps. Other detectors just use the detector description in memory to calculate the maps.
    \note By default it will take the version from GeoModel during initialisation. Can be overridden by GeometryLayout property. For example:
    \code Service("RegSelSvc").GeometryLayout="ATLAS-CSC-01-02-00" \endcode
  */
  StringProperty m_geometryVersionProp;
  std::string m_geometryVersion;

  //! Flag to set Inner Detectors initialization.
  BooleanProperty  m_initOnlyID;

  //! Flag to set individual Inner Detectors subdetector initializations.
  BooleanProperty  m_initSCT;
  BooleanProperty  m_initPixel;
  BooleanProperty  m_initTRT;

  //! Flag to set Calorimeters initialization.
  BooleanProperty  m_initOnlyCalo;

  //! Flag to set Muons initialization.
  BooleanProperty  m_initOnlyMuon;

  //! Flag to dump loaded tables in data files.
  BooleanProperty  m_dumpTable;

  //! Flag to dump loaded tables in data files.
  BooleanProperty  m_disableFromConditions;

  BooleanProperty  m_disablePixelFromConditions;
  BooleanProperty  m_disableSCTFromConditions;
  BooleanProperty  m_disableTRTFromConditions;

  //! Dumped data file name, by default this is set to "RoITable.txt".
  StringProperty m_roiFileName;

  //! LArConditions which should delay the RegionSelectorSvc
  StringProperty m_LArFebRodMapKey;

  //! Data structure to hold Pixel data.
  RegSelSiLUT* m_newpixel;

  //! Data structure to hold SCT data.
  RegSelSiLUT* m_newsct;

  //! Data structure to hold TRT data.
  RegSelSiLUT* m_newtrt;

  //! Data structure to hold Tile data.
  RegSelectorMap m_tileData; 

  //! Data structure to hold LAR data.
  RegSelectorHashMap m_larData;

  //! Data structure to hold TTEM.
  RegSelectorHashMap m_ttemData;

  //! Data structure to hold TTHEC data.
  RegSelectorHashMap m_tthecData;

  //! Data structure to hold FCALEM data.
  RegSelectorHashMap m_ttfcalemData;

  //! Data structure to hold FCALHAD data.
  RegSelectorHashMap m_ttfcalhadData;

  std::vector<uint32_t>        m_deleteRobList;
  //  std::vector<IdentifierHash>  m_deletePixelHashList;
  //  std::vector<IdentifierHash>  m_deleteSCTHashList;
  std::vector<unsigned int>  m_deletePixelHashList;
  std::vector<unsigned int>  m_deleteSCTHashList;
  std::vector<unsigned int>  m_deleteTRTHashList;

private:
  typedef SimpleProperty< std::vector<uint32_t> > Uint32ArrayProperty;
  
  //! Tools to create the lookup tables in StoreGate
  ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolPixel;
  ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolSCT;
  ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolTRT;
  ToolHandle<IRegionLUT_Creator> m_lutCreatorToolLAR;
  ToolHandle<IRegionLUT_Creator> m_lutCreatorToolTile;
  ServiceHandle< IGeoModelSvc > m_geoModelSvc;
  std::vector<std::string> m_enabledDetectors;
  bool m_errorFlag;
  
  
  //! Flag to read the rob list from OKS
  BooleanProperty     m_readSiROBListFromOKS;
  
  //! List of all ROBs to be enabled if setting from OKS    
  Uint32ArrayProperty m_enabledROBs;
  
  double m_DeltaZ;

public:

  //   const RegSelSiLUT* getpixel() const { return m_newpixel; } 
  //   const RegSelSiLUT* getsct()   const { return m_newsct; } 
  //   const RegSelSiLUT* gettrt()   const { return m_newtrt; } 

  bool handleMuon();

  void DisableMissingROBs(const std::vector<uint32_t>& vec);

private:
  
  BooleanProperty  m_initRPC;
  BooleanProperty  m_initMDT;
  BooleanProperty  m_initTGC;
  BooleanProperty  m_initCSC;

  ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolRPC;
  ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolMDT;
  ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolTGC;
  ToolHandle<IRegionIDLUT_Creator> m_lutCreatorToolCSC;


  RegSelSiLUT* m_newrpc;
  RegSelSiLUT* m_newmdt;
  RegSelSiLUT* m_newtgc;
  RegSelSiLUT* m_newcsc;
  
  StringProperty m_PixConditionsSvc;
  StringProperty m_SCTConditionsSvc;
  StringProperty m_TRTConditionsSvc;

  BooleanProperty                    m_initFTK;
  ToolHandle<IRegionIDLUT_Creator>   m_lutCreatorToolFTK;
  RegSelSiLUT*                       m_ftklut;

  bool m_duplicateRemoval;

};

#endif

