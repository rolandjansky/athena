/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNDBTOOLS_TRTTRKALIGNDBTOOL_H
#define INDETALIGNDBTOOLS_TRTTRKALIGNDBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkAlignInterfaces/ITrkAlignDBTool.h"


/**
   @file TRTTrkAlignDBTool.h
   @class TRTTrkAlignDBTool

   @brief Tool for writing TRT alignment constants into oracle DB or an ASCII file.

   @author Daniel Kollar <daniel.kollar@cern.ch>
   @date 25/02/2009
*/

class TFile;

class TRT_ID;
class PixelID;
class ITRT_AlignDbSvc;

class Identifier;

namespace Trk
{
   class AlignModule;
   class IAlignModuleTool;
   class IGeometryManagerTool;
}

namespace InDet
{

   class TRTTrkAlignDBTool : virtual public Trk::ITrkAlignDBTool, public AthAlgTool
   {

      public:
         TRTTrkAlignDBTool(const std::string & type, const std::string & name, const IInterface * parent);
         virtual ~TRTTrkAlignDBTool();

         StatusCode initialize();
         StatusCode finalize();

         /** Writes alignment parameters */
         void writeAlignPar();

         /** sets ntuple to be used to write output */
         void setNtuple(TFile * file)
            { m_ntuple=file; }

      private:

         /** print translation to stdout */
         void printTransform(const Amg::Transform3D & tr) const;

         /** updates alignment constants in the DB */
         void updateDB();

         /** checks whether the AlignModule contains TRT detector elements*/
         bool containsTRT(Trk::AlignModule * module) const;

         /** updates level 0 transform as level 1 constants in the DB */
         void updateL0asL1(Identifier idL0, const Amg::Transform3D & transform);

         bool checkAlignLevel();

         const TRT_ID  * m_trtHelper;
         const PixelID * m_pixHelper;

         std::string m_outputAlignFile;
         std::string m_outputStrawAlignFile;
         std::string m_oldAlignFile;
         std::string m_oldStrawAlignFile;
         bool m_writeTextFile;
         bool m_writeOldConstants;
         bool m_updateConstants;


         ServiceHandle<ITRT_AlignDbSvc>         m_trtAlignDbSvc;
         ToolHandle<Trk::IAlignModuleTool>      m_alignModuleTool;
         ToolHandle<Trk::IGeometryManagerTool>  m_trtGeoManager;
	 
         int m_trtAlignLevel;
         int m_trtAlignLevelBarrel;
         int m_trtAlignLevelEndcaps;

         bool m_WriteTRTAsL2;

         TFile * m_ntuple;

   }; // end class

} // end namespace

#endif // INDETALIGNDBTOOLS_TRTTRKALIGNDBTOOL_H
