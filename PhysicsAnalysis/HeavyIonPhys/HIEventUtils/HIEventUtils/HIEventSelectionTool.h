/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HIEVENTSELECTIONTOOL_H
#define HIEVENTUTILS_HIEVENTSELECTIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "HIEventUtils/IHIEventSelectionTool.h"
#include "HIEventUtils/IHIVertexSelectionTool.h"
#include "HIEventUtils/IHIPileupTool.h"

namespace HI
{

  class HIEventSelectionTool :
  public virtual HI::IHIEventSelectionTool, public asg::AsgTool
  {

      ASG_TOOL_CLASS( HIEventSelectionTool, IHIEventSelectionTool )

    public:

      HIEventSelectionTool(const std::string& name = "HIEventSelection");
      virtual ~HIEventSelectionTool();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual const asg::AcceptInfo& getAcceptInfo() const override;

      // MBTS timing cut member functions
      void checkMBTSTime( asg::AcceptData& acceptData ) const;
      bool isGoodMBTSTime() const;

      // Vertex member functions
      void checkVertex( asg::AcceptData& acceptData ) const;
      unsigned int getNPrimaryVertices() const;

      std::string dumpSelection() const;

    private:

      static const std::string m_MBTS_CNAME;
      static const std::string m_FWD_CNAME;
      static const std::string m_VERTEX_CNAME;
      static const std::string m_SHAPE_CNAME;
      static const std::string m_ZDC_CNAME;

      const std::string m_name;
      asg::AcceptInfo m_accept;

      // MBTS external parameters
      bool m_checkTime = false;
      float m_timeCut = -1;

      // Vertex external parameters
      ToolHandle<HI::IHIVertexSelectionTool> m_vertexSelectionTool;
      ToolHandle<HI::IHIPileupTool> m_hiPileupTool;
      float m_vtx_min_sumPt = -1;
      int m_vtx_min_nTrk = -1;
      bool m_exclusive_mode = false;
      bool m_reject_pileup = true;
      std::string m_track_cutlevel = "NoCut";
  };

}
#endif
