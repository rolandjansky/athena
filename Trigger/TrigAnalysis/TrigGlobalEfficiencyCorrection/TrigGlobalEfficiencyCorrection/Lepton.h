/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#ifndef TRIGGLOBALEFFICIENCYCORRECTION_LEPTON_H
#define TRIGGLOBALEFFICIENCYCORRECTION_LEPTON_H 1

#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

namespace TrigGlobEffCorr
{

class Lepton
{
public:
	Lepton(const xAOD::IParticle* ptr, std::size_t tag=0) : m_obj(ptr), m_tag(tag) {}
	Lepton(const xAOD::Electron* ptr, std::size_t tag=0) : m_obj(ptr), m_tag(tag) {}
	Lepton(const xAOD::Photon* ptr, std::size_t tag=0) : m_obj(ptr), m_tag(tag) {}
	Lepton(const Lepton&) = default;
	Lepton& operator=(const Lepton&) = default;
	Lepton(Lepton&&) = default;
	Lepton& operator=(Lepton&&) = default;
	float pt() const { return m_obj->pt(); }
	xAOD::Type::ObjectType type() const { return m_obj->type(); }
	std::size_t tag() const { return m_tag; }
	const xAOD::Electron* electron() const { return static_cast<const xAOD::Electron*>(m_obj); }
	const xAOD::Muon* muon() const { return static_cast<const xAOD::Muon*>(m_obj); }
	const xAOD::Photon* photon() const { return static_cast<const xAOD::Photon*>(m_obj); }
	const xAOD::IParticle* particle() const { return m_obj; }
	bool operator<(const Lepton& rhs) const { return m_obj<rhs.m_obj; }
protected:
	const xAOD::IParticle* m_obj = nullptr;
	mutable std::size_t m_tag = 0; // 0 = no tag
};

}


#endif
