/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HI_HIEVENTSHAPESUMMARYTOOL_H
#define HI_HIEVENTSHAPESUMMARYTOOL_H

#include "AsgTools/AsgTool.h"
#include "HIEventUtils/IHIEventShapeSummaryTool.h"
#include <functional>
#include <string>
#include <vector>
#include <map>


class HIEventShapeSummaryTool : virtual public asg::AsgTool, virtual public IHIEventShapeSummaryTool
{
  ASG_TOOL_CLASS(HIEventShapeSummaryTool,IHIEventShapeSummaryTool)
public:
  HIEventShapeSummaryTool(const std::string& n);


  StatusCode summarize(const xAOD::HIEventShapeContainer* in, xAOD::HIEventShapeContainer* out) const;
  StatusCode initialize();
  std::string dumpList() const;

private:
  typedef std::function<bool (const xAOD::HIEventShape*)> function_t;
  struct summary_info_t
  {
    std::string name;
    float eta_min;
    float eta_max;
    int layer;
    function_t func;

    summary_info_t(std::string n, float emin, float emax, int ll, function_t ff) : name(n), eta_min(emin), eta_max(emax), layer(ll), func(ff) {};
  };

    
  std::vector<std::string> m_samp_names;
  std::vector<std::string> m_subcalo_names;
  bool m_do_sides;

  //using map instead of unordered version, really do want entries sorted by key
  std::map<std::string,summary_info_t> m_summary_list;

  int getSubCaloLayer(const std::vector<int>& samps) const;
  float getSubCaloEtaMin(const std::vector<int>& samps) const;
  float getSubCaloEtaMax(const std::vector<int>& samps) const;
  float roundToTenth(float v) const;

  static const std::vector<std::string> SubCaloNames;
  static const std::vector<std::initializer_list<int> > SubCaloLists;

};


#endif
