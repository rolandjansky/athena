/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_MUONALIGNDBTOOL_H
#define MUONALIGNGENTOOLS_MUONALIGNDBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DataModel/DataVector.h"

#include "MuonCondInterface/IMultiLevelAlignmentDbTool.h"
#include "MuonGMdbObjects/DblQ00Aszt.h"

#include "TrkAlignInterfaces/ITrkAlignDBTool.h"
#include "TrkAlignEvent/AlignParList.h"
#include "TrkAlignEvent/AlignModule.h"

/**
   @file MuonAlignDBTool.h
   @class MuonAlignDBTool

   @brief Base class for tools for reading and writing alignment constants.  
   Can read constants from oracle DB or an ASCII file.  Writes constants to 
   an output ASCII file.

   Modified from MdtAlignDBTool

   @author Robert Harrington <roberth@bu.edu>, Steffen Kaiser <Steffen.Kaiser@cern.ch>
   @date 7/7/2010
*/

class IRDBAccessSvc;
class TFile;

namespace MuonCalib {
  class IIdToFixedIdTool;
}

namespace MuonGM {
  class MuonDetectorManager;
  class MuonReadoutElement;
}

namespace Trk {
  class IAlignModuleTool;
  class TrkDetElementBase;
}

class IMultiLevelAlignmentDbTool;

namespace Muon {

  class MuonAlignDBTool : virtual public Trk::ITrkAlignDBTool, public AthAlgTool {
    
  public:
    MuonAlignDBTool(const std::string& type, const std::string& name,
			    const IInterface* parent);
    virtual ~MuonAlignDBTool();
    
    StatusCode initialize();
    
    StatusCode finalize();

    /** Writes alignment parameter (to ASCII file for now) */
    void writeAlignPar();

    /** sets ntuple */
    void setNtuple(TFile* ntuple) { m_ntuple=ntuple; }

    /** called before final solving to get initial A-lines */
    StatusCode preSolve();

  protected:
    /** used to decompose a HepGeom::Transform3D into translations and rotations **/
    void decomposeTransform(const Amg::Transform3D& transform, double* values) const; 
    const MuonGM::MuonReadoutElement* getMuonRE(const Trk::TrkDetElementBase* detElement) const;


  private:

    void                initL1FromDatabase() const;
    MuonGM::DblQ00Aszt* initAlinesEqZero() const;
    MuonGM::DblQ00Aszt* initSigmasFromAsciiFile() const;

    void writeALinesToAsciiFile(const ALineMapContainer* alines, const std::string& file) const;

    void writeASZTToAscii(MuonGM::DblQ00Aszt::ASZT* aszt, 
			  int nObj, const std::string& filename) const;

    //bool selectModuleASZT(const AlignModuleList* alignModList,
    //		  const MuonGM::DblQ00Aszt::ASZT& aszt, 
    //		  const AlignModule* alignMod);

    //void fillSigmasToFirstAlignParMap();
    
    /** returns TrkDetElementBase if it is in an AlignModule only, returns 0 otherwise */
    const Trk::TrkDetElementBase* getDetElement(Identifier id) const;

    void createL1LookupTable();
    void createL23LookupTable();
    void createL3LookupTable();    
    void setDetElemLookup() const;

    ALineMapContainer* asciiToALines(const std::string& inputfile);

    StatusCode getInitialALines();

    void globalToL1AlignFrame(const Trk::AlignModule* mod, 
			      double* globalPars, double* alignPars) const;

    void level1AlignToGlobalFrame(const Trk::AlignModule* mod, 
				  double* alignPars, double* globalPars) const;

    ToolHandle<IMultiLevelAlignmentDbTool>  m_multilevelDbTool; //!< Pointer to MultiLevelAlignmentDbTool
    ToolHandle<MuonCalib::IIdToFixedIdTool> m_idTool;           //!< Pointer to IdToFixedIdTool
    ToolHandle<Trk::IAlignModuleTool>       m_alignModuleTool;  //!< Pointer to AlignModuleTool

    const MuonGM::MuonDetectorManager* p_muonMgr;
    
    std::string m_asztInputFile;
    std::string m_asztOutputFile;
    std::string m_sigmaInputFile;
    std::string m_sigmaOutputFile;

    TFile* m_ntuple;

    // for RDBAccess
    IRDBAccessSvc* m_rdbAccessSvc;
    std::string m_detectorKey;
    std::string m_detectorNode;

    const MuonGM::MuonDetectorManager* m_muonDetMgr;
    //ALineMapContainer* m_alineData;

    const ALineMapContainer* m_alines;
    
    //Trk::AlignParList* m_detElementAlignPar;

    //bool m_storeL3Params;

    // lookup tables to help go from ASZT to AlignPar and AlignModule
    mutable std::vector<Identifier>*                    m_detElemLookup_identifier;
    mutable std::vector<const Trk::AlignModule*>*       m_detElemLookup_alignMod;
    mutable std::vector<const Trk::TrkDetElementBase*>* m_detElemLookup_detElem;
    mutable std::vector<int>*                           m_detElemLookup_alineIndex;
    mutable std::vector<std::string>*                   m_detElemLookup_stName;
    mutable std::vector<int>*                           m_detElemLookup_stPhi;
    mutable std::vector<int>*                           m_detElemLookup_stEta;

    std::vector<DataVector<Trk::AlignPar>* >             m_ALineContainerToAlignPar;
    std::vector<const Trk::AlignModule*>                 m_ALineContainerToMod;

    bool m_useMultiLevelDbTool;


  }; // end class
  
} // end namespace

#endif // MUONALIGNGENTOOLS_MUONALIGNDBTOOL_H
