/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REWEIGHTUTILS_PARTICLESCALEFACTORTOOL_H
#define REWEIGHTUTILS_PARTICLESCALEFACTORTOOL_H 1

#include "AsgTools/AsgTool.h"

#include "AsgAnalysisInterfaces/IObservableTool.h"
#include "PATInterfaces/ISystematicsTool.h"

#include "TH1.h"
#include "xAODBase/IParticle.h"

#include <functional>

class ParticleScaleFactorTool: public asg::AsgTool, public virtual IObservableTool , public virtual CP::ISystematicsTool { 
 public: 
   //constructor for athena can be created using special macro
   //Note: if you add a second interface to your tool, you must use: ASG_TOOL_CLASS2( ToolName, Interface1, Interface2) 
   ASG_TOOL_CLASS3( ParticleScaleFactorTool , IObservableTool , CP::ISystematicsTool, CP::IReentrantSystematicsTool )
   //add another constructor for non-athena use cases
   ParticleScaleFactorTool( const std::string& name );

   ~ParticleScaleFactorTool();

   /// Initialize is required by AsgTool base class
   virtual StatusCode  initialize() override;

   virtual double evaluate(const xAOD::IParticle* particle) const override;

   static double part_pt(const xAOD::IParticle& p) {return p.pt();}
   static double part_pt_gev(const xAOD::IParticle& p) {return p.pt()*1e-3;}
   static double part_eta(const xAOD::IParticle& p) {return p.eta();}
   static double part_decor(const xAOD::IParticle& p,const std::string& varname) {
      SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
      const SG::auxid_t auxid = reg.findAuxID( varname );
      if( auxid == SG::null_auxid ) { 
        throw std::runtime_error("unknown aux id for decoration: " + varname); 
      }
      const std::type_info* ti = reg.getType( auxid );
      if( *ti == typeid( double ) ) {
          return p.auxdataConst< double >( varname );
      } else if( *ti == typeid( float ) ) {
         return p.auxdataConst< float >( varname );
      } else if( *ti == typeid( int ) ) {
          return p.auxdataConst< int >( varname );
      } else if( *ti == typeid( unsigned int ) ) {
          return p.auxdataConst< unsigned int>( varname );
      } 
      throw std::runtime_error("Unrecognised type for decoration: " + varname); 
   }


   /// The ISystematicsTool methods 
  bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const override { return (m_affectingSysts.find(systematic)!=m_affectingSysts.end()); }
  CP::SystematicSet affectingSystematics() const override { return m_affectingSysts; }
  CP::SystematicSet recommendedSystematics() const override { return affectingSystematics(); }
  StatusCode applySystematicVariation( const CP::SystematicSet& systConfig ) override;

 private: 
   std::string m_configFile;
   
   CP::SystematicVariation m_currentSyst;
   
   struct Hists {
    std::map<CP::SystematicVariation,TH1*> hists;
    const std::pair<CP::SystematicVariation,TH1*> getHist(const CP::SystematicVariation& set) const;
    std::vector<std::function<double(const xAOD::IParticle&)>> axisFuncs; 
   };
   
   std::map<xAOD::Type::ObjectType,Hists> m_hists;
   
   CP::SystematicSet m_affectingSysts;
   bool m_isNominal=true;
   
}; 

#endif //> !REWEIGHTUTILS_PARTICLESCALEFACTORTOOL_H
