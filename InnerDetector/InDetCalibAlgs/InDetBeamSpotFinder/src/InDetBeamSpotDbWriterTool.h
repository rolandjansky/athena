/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_IINDETBEAMPSPOTDBWRITERTOOL_H
#define INDET_IINDETBEAMPSPOTDBWRITERTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetBeamSpotFinder/IInDetBeamSpotWriterTool.h"

//#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/Service.h"
//#include "GaudiKernel/ServiceHandle.h" 


//#include "StoreGate/DataHandle.h"
//#include "AthenaKernel/DefaultKey.h"
//#include "AthenaPoolUtilities/AthenaAttributeList.h"

class TTree;
class ITHistSvc;
class IBeamCondSvc;
class IIOVRegistrationSvc;

namespace coral{
  class AttributeListSpecification;
}
namespace InDet{
  /** 
      Concrete implementation of @c IInDetBeamSpotWriterTool.
      Designed to store beamspot information into a standard ATLAS conditions database-style format.
      JobOptions can define the specific type of DB and settings.
  */
  class InDetBeamSpotDbWriterTool 
    : public AthAlgTool, virtual public IInDetBeamSpotWriterTool {
    public:
    // Constructor
    InDetBeamSpotDbWriterTool( const std::string& type, 
			       const std::string& name, 
			       const IInterface* parent);
    // Standard Destructor
    virtual ~InDetBeamSpotDbWriterTool() { }
    
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    virtual StatusCode write(std::map<BeamSpot::ID, BeamSpot::BeamSpotDB> &);

    void setLBfromAcceptedEvents(bool set) {m_setLBwithAcceptedEvents = set;}
    void setLBfromViewedEvents(bool set) {m_setLBwithViewedEvents   = set;}

    private:
    
    std::string m_beamposFolder; //beamposFolder name
    std::string m_tagID;  //tag name for folder
    IIOVRegistrationSvc * m_regsvc;
    //ServiceHandle<IIOVRegistrationSvc> m_regsvc;
    ServiceHandle<IToolSvc> m_toolSvc;
    
    coral::AttributeListSpecification* m_aspec;

    StatusCode registerBeamspots();
    bool addBeamspot(const BeamSpot::ID &,  const BeamSpot::BeamSpotDB&);

    /** Write output also to a ROOT file */
    bool m_doRoot; 
    ITHistSvc * m_thistSvc;
    std::string m_root_filename;
    std::string m_root_treename;
    std::string m_root_dir;
    TTree * m_root_bs;

    double m_beamPos[9], m_beamPosErr[9];
    int    m_beamStatus[5];
    int    m_beamID[10];
    StatusCode fillRoot(const BeamSpot::ID *,const BeamSpot::BeamSpotDB*); /** Fill the root tree with BS info */

    bool m_doCOOL;
    bool m_doTimeStampWrite; /** write to database via the timestamp */ 
    bool m_doEventWrite; /** Write using the run and event info */
    bool m_writeFailed; /** Are failed beamspots written to the Db */

    bool m_setLBwithViewedEvents;
    bool m_setLBwithAcceptedEvents;

    bool m_storeErrors; /** Set whether to store parameter errors in COOL*/

    };

}
#endif

