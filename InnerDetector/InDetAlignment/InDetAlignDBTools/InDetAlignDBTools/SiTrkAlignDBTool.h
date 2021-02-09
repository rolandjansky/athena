/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNDBTOOLS_SITRKALIGNDBTOOL_H
#define INDETALIGNDBTOOLS_SITRKALIGNDBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkAlignInterfaces/ITrkAlignDBTool.h"

#include "GeoPrimitives/GeoPrimitives.h"

/**
  @file SITrkAlignDBTool.h 
  @class SITrkAlignDBTool 

  @brief Tool for  writing Silicon alignment constants into oracle DB or an ASCII file.

  @author Roland Haertel <haertel@mppmu.mpg.de>
  @author Daniel Kollar <daniel.kollar@cern.ch>
  @date 27/08/2009
*/

class TFile;

class PixelID;
class SCT_ID;
class AtlasDetectorID;

class Identifier;

class IInDetAlignDBTool;

namespace Trk
{
  class AlignModule;
  class IAlignModuleTool;
  class IGeometryManagerTool;
}

namespace InDet
{

  class SiTrkAlignDBTool : virtual public Trk::ITrkAlignDBTool, public AthAlgTool {

  public:

    /** constructor */
    SiTrkAlignDBTool(const std::string & type, const std::string & name, const IInterface * parent);

    /** destructor */
    virtual ~SiTrkAlignDBTool();

    /** initialize method */
    virtual StatusCode initialize();

    /** finalize method */
    virtual StatusCode finalize();

    /** Writes alignment parameters */
    void writeAlignPar();

    /** sets ntuple to be used to write output */
    void setNtuple(TFile * file)
      { m_ntuple=file; }
    

  private:

    /** updates alignment constants in the DB */
    void updateDB();

    bool checkPixelLevel();
    bool checkSCTLevel();
    bool checkSiLevel();
    
    void updateSiL0asL1(Identifier idL0, const Amg::Transform3D & transform);
    
    void updateAsL3(const Trk::AlignModule * module, const Amg::Transform3D & transform, double);
    
    void printTransform(const Amg::Transform3D & tr) const;
    
    void updateAsL2(const Trk::AlignModule * module, const Amg::Transform3D & transform, const int);

    void updateAsL16(const Trk::AlignModule * module, double);
    
    virtual Identifier GetL1L2fromL3Identifier( const Identifier& ident , const int& level) const ;

    const PixelID          * m_pixHelper; //!< pointer to PIX detector manager
    const SCT_ID           * m_sctHelper; //!< pointer to SCT detector manager
    const AtlasDetectorID  * m_idHelper;

    bool        m_writeSQLFile;           //!< flag wether to write out .db file or not (file is not needed in parallel jobs)
    std::string m_SQLiteTag;              //!< tag name for the ConditionsDB
    std::string m_outputAlignFile;        //!< filename for the textfile with final alignment constants
    std::string m_outIBLDistFile;         //!< filename for the textfile with final IBLDist params 
    std::string m_outGlobalFolderFile;    //!< filename for the textfile with final GlobalFolder constants
    bool        m_writeTextFile;          //!< flag whether to write out the final constants into text file
    bool        m_writeOldConstants;      //!< flag whether to write out the initial constants into text file
    std::string m_oldAlignFile;           //!< filename for the textfile with initial alignment constants
    std::string m_oldIBLDistFile;         //!< filename for the textfile with initial IBLDist params           
    std::string m_oldGlobalFolderFile;    //!< filename for the textfile with initial GlobalFolder constants
    bool        m_updateConstants;        //!< flag whether to update the constants

    ToolHandle<IInDetAlignDBTool>        m_IDAlignDBTool;
    ToolHandle<Trk::IAlignModuleTool>    m_alignModuleTool;

    ToolHandle<Trk::IGeometryManagerTool> m_pixelGeoManager;
    ToolHandle<Trk::IGeometryManagerTool> m_sctGeoManager;
    ToolHandle<Trk::IGeometryManagerTool> m_siGeoManager;

    TFile * m_ntuple;

    int m_pixelAlignLevel;
    int m_pixelAlignLevelBarrel;
    int m_pixelAlignLevelEndcaps;
    bool m_pixelaAlignDBM;

    int m_sctAlignLevel;
    int m_sctAlignLevelBarrel;
    int m_sctAlignLevelEndcaps;

    int m_siAlignLevel;

    bool m_doSi;
    bool m_doPixel;
    bool m_doSCT;

    bool m_writeAsL3;
    bool m_writeAsL2;
    bool m_writeAsL16;

  }; // end class

} // end namespace

#endif // INDETALIGNDBTOOLS_SITRKALIGNDBTOOL_H
