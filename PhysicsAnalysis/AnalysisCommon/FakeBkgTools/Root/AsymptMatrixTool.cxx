/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FakeBkgTools/AsymptMatrixTool.h"
#include "FakeBkgTools/FakeBkgInternals.h"

#include <algorithm>
#include <numeric>

using namespace FakeBkgTools;
using namespace CP;

Client AsymptMatrixTool::clientForDB()
{
    return Client::MATRIX_METHOD;
}

AsymptMatrixTool::AsymptMatrixTool(const std::string& name) : BaseLinearFakeBkgTool(name)
{
}

AsymptMatrixTool::~AsymptMatrixTool()
{
}

StatusCode AsymptMatrixTool::initialize()
{
    return BaseLinearFakeBkgTool::initialize();
}

StatusCode AsymptMatrixTool::addEventCustom()
{
    m_cachedWeights.clear();
   
    m_components.clear();
    m_derivatives.clear();
    for(auto& p : m_particles)
    {
        double e = p.real_efficiency.nominal, z = p.fake_efficiency.nominal;
        double den = 1. / (e - z);
        double delta = p.tight? 1. : 0.;
        m_components.emplace_back(std::array<double,2>{den*(delta-z), den*(e-delta)});
        m_derivatives.emplace_back(std::array<double,2>{den*den*(z-delta), den*den*(delta-z)});
        m_derivatives.emplace_back(std::array<double,2>{den*den*(delta-e), den*den*(e-delta)});
    }

    return incrementTotalYield();
}

StatusCode AsymptMatrixTool::getEventWeightCustom(FakeBkgTools::Weight& weight, const FinalState& fs)
{
    const uint64_t n = m_particles.size();
    const uint64_t nc = (1 << n);
    
    std::array<double, maxCombinations()> w;
    std::array<std::array<double, maxCombinations()>, 2*maxParticles()> dproj_dt;
    
    /// Fill projector
    std::fill_n(w.begin(), nc, 0.);
    for(uint64_t k=0;k<2*n;++k) std::fill_n(dproj_dt[k].begin(), nc, 0.);

    auto charges = fs.retrieveCharges(m_particles);
    std::array<double, maxParticles()+1> effprod;
    for(uint64_t i=0;i<nc;++i) // loop on tight/loose
    {
        FSBitset tights(i);
        if(!fs.accept_selection(tights, charges)) continue;
        for(uint64_t j=0;j<nc;++j) // loop on real/fake
        {
            FSBitset reals(j);
            if(!fs.accept_process(n, reals, tights)) continue;
            std::fill(effprod.begin(), effprod.begin()+n+1, 1.);
            for(uint64_t k=0;k<n;++k)
            {
                double eff = reals[k]? m_particles[k].real_efficiency.nominal : m_particles[k].fake_efficiency.nominal;
                double x =  tights[k]? eff : 1.-eff;
                effprod[0] *= x;
                for(uint64_t l=1;l<n+1;++l)
                {
                    if(l != k+1) effprod[l] *= x;
                }
            }
            auto rev_j = nc-j-1;
            w[rev_j] += effprod[0]; /// storage convention is w[0] = RR...R, w[1] = FR...R, etc
            for(uint64_t k=0;k<n;++k) dproj_dt[2*k+!reals[k]][rev_j] += effprod[k+1];
        }
    }
    
    
    std::array<std::array<double, maxCombinations()>, 2*maxParticles()> dlambdaprod_dt;
    for(uint64_t k=0;k<2*n;++k) std::copy(w.begin(), w.begin()+nc, dlambdaprod_dt[k].begin());

    for(unsigned k=0;k<n;++k)
    {
        for(uint64_t j=0;j<nc;++j)
        {
            char index = (j>>k)&1;
            double x = m_components[k][index];
            w[j] *= x;
            for(uint64_t u=0;u<n;++u)
            {
                dproj_dt[2*u][j] *= x;
                dproj_dt[2*u+1][j] *= x;
                if(u != k)
                {
                    dlambdaprod_dt[2*u][j] *= x;
                    dlambdaprod_dt[2*u+1][j] *= x;
                }
                else
                {
                    dlambdaprod_dt[2*u][j] *= m_derivatives[2*u][index];
                    dlambdaprod_dt[2*u+1][j] *= m_derivatives[2*u+1][index];
                }
            }
        }
    }
    weight.nominal = std::accumulate(w.begin(), w.begin()+nc, 0.);
    auto wu = dlambdaprod_dt.begin(), wv = dproj_dt.begin();
    for(auto& p : m_particles)
    {
        double theta = std::accumulate(wu->begin(), wu->begin()+nc, 0.) + std::accumulate(wv->begin(), wv->begin()+nc, 0.);
        ++wu;
        ++wv;
        for(auto& kv : p.real_efficiency.uncertainties)
        {
            auto& uncertainties =  weight.uncertainties[kv.first];
            uncertainties.up += theta * kv.second.up;
            uncertainties.down += theta * kv.second.down;
        }
        theta = std::accumulate(wu->begin(), wu->begin()+nc, 0.) + std::accumulate(wv->begin(), wv->begin()+nc, 0.);
        ++wu;
        ++wv;
        for(auto& kv : p.fake_efficiency.uncertainties)
        {
            auto& uncertainties =  weight.uncertainties[kv.first];
            uncertainties.up += theta * kv.second.up;
            uncertainties.down += theta * kv.second.down;
        }
    }
    return StatusCode::SUCCESS;
}
