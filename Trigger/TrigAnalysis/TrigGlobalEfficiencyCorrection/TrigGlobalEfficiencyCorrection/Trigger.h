/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#ifndef TRIGGLOBALEFFICIENCYCORRECTION_TRIGGER_H
#define TRIGGLOBALEFFICIENCYCORRECTION_TRIGGER_H 1

#include "TrigGlobalEfficiencyCorrection/ImportData.h"
#include "xAODBase/ObjectType.h"

#include <array>
#include <algorithm>
#include <type_traits>
#include <boost/container/flat_set.hpp>
template<typename Key> using flat_set = boost::container::flat_set<Key>;

namespace TrigGlobEffCorr
{

class UnusedArg { public: static constexpr xAOD::Type::ObjectType object() { return xAOD::Type::Other; } };

class TriggerProperties
{
public:
	explicit constexpr TriggerProperties(TriggerType tt) : m_type(tt), m_legs{} {}
	explicit TriggerProperties(const ImportData::TrigDef& def) : m_type(def.type) { loadLegs(def, m_legs); }
	constexpr TriggerType type() const { return m_type; }
	constexpr bool valid() const
	{
		return !((m_type&TT_MASK_FLAVOUR)&~(TT_ELECTRON_FLAG|TT_MUON_FLAG|TT_PHOTON_FLAG)); /// validity of mixed() function
	}
	constexpr bool mixed() const
	{
		auto x = m_type&TT_MASK_FLAVOUR;
		return (x!=TT_ELECTRON_FLAG) && (x!=TT_MUON_FLAG) && (x!=TT_PHOTON_FLAG);
	}
	constexpr unsigned nDistinctLegs() const
	{
		auto x = m_type&TT_MASK_SYMMETRY;
		if(m_type&TT_SINGLELEPTON_FLAG) return 1;
		else if(m_type&TT_DILEPTON_FLAG) return 2 - 1*(x==TT_SYM);
		else if(m_type&TT_TRILEPTON_FLAG) return (x==TT_ASYM)? 3 : 1 + 1*(mixed()||(x!=TT_SYM));
		return 0;
	}
	constexpr unsigned nDistinctLegs(xAOD::Type::ObjectType obj) const
	{
		bool firstPos = true;
		switch(obj)
		{
			case xAOD::Type::Electron:
				if(!(m_type&TT_ELECTRON_FLAG)) return 0;
				break;
			case xAOD::Type::Muon:
				if(!(m_type&TT_MUON_FLAG)) return 0;
				firstPos = (m_type&TT_PHOTON_FLAG);
				break;
			case xAOD::Type::Photon:
				if(!(m_type&TT_PHOTON_FLAG)) return 0;
				firstPos = false;
				break;
			default: return 0;
		}
		if(!mixed()) return nDistinctLegs();
		return (firstPos == (m_type&TT_X2Y_FLAG))? 1 : nDistinctLegs()-1;
	}
	template<typename Array>
	void loadLegs(const ImportData::TrigDef& src, Array& dest)
	{
		if(src.type != m_type) throw; /// can't be thrown due to bad user action -- only in case of a bug in the Calculator class
		std::fill(dest.begin(), dest.end(), 0);
		if(m_type==TT_2E_MU_SYM || m_type==TT_2E_G_SYM || m_type==TT_2MU_G_SYM) /// special case needed to skip the duplicated leg for 2X_Y triggers
		{
			dest[0] = src.leg[0];
			dest[1] = src.leg[2];
		}
		else /// Works as well for non-mixed trilepton triggers since the asymmetric leg is always stored first
		{
			std::copy_n(src.leg.cbegin(), nDistinctLegs(), dest.begin());
		}
	}
	
	constexpr int cbegin_offset(xAOD::Type::ObjectType obj) const
	{
		return (obj!=xAOD::Type::Electron) *(nDistinctLegs(xAOD::Type::Electron)  
			+ (obj!=xAOD::Type::Muon)*nDistinctLegs(xAOD::Type::Muon));
	}
	
	auto cbegin(xAOD::Type::ObjectType obj) const
	{
		return m_legs.cbegin() + cbegin_offset(obj);
	}
	
	constexpr int cend_offset(xAOD::Type::ObjectType obj) const
	{
		return -((obj!=xAOD::Type::Photon) *(nDistinctLegs(xAOD::Type::Photon)  
			+ (obj!=xAOD::Type::Muon)*nDistinctLegs(xAOD::Type::Muon)));
	}
	
	auto cend(xAOD::Type::ObjectType obj) const
	{
		return m_legs.cbegin() + nDistinctLegs() + cend_offset(obj);
	}
	
protected:
	TriggerType m_type;
	std::array<std::size_t, 3> m_legs;
};

template<TriggerType tt, typename CastType1 = UnusedArg, typename CastType2 = UnusedArg>
class Trigger
{
	static_assert(TriggerProperties(tt).valid(), "trigger type not supported");
private:
	static constexpr bool extraCheck(xAOD::Type::ObjectType obj) { return (object()==obj || obj==xAOD::Type::Other); }
	template<typename T> struct Optional {};
public:
	
	static constexpr TriggerType type() { return tt; }
	
	static constexpr bool mixed()
	{
		return TriggerProperties(tt).mixed();
	}
	
	static constexpr unsigned nDistinctLegs() { return TriggerProperties(tt).nDistinctLegs(); }
	
	static constexpr unsigned nDistinctLegs(xAOD::Type::ObjectType obj) { return TriggerProperties(tt).nDistinctLegs(obj); }

	static constexpr xAOD::Type::ObjectType object1()
	{
		if(!is3Lmix())
		{
			if(tt&TT_ELECTRON_FLAG) return xAOD::Type::Electron;
			if(tt&TT_MUON_FLAG) return xAOD::Type::Muon;
			if(tt&TT_PHOTON_FLAG) return xAOD::Type::Photon;
			return xAOD::Type::Other;
		}
		if((tt&TT_ELECTRON_FLAG) && !(tt&TT_X2Y_FLAG)) return xAOD::Type::Electron;
		if((tt&TT_PHOTON_FLAG) && (tt&TT_X2Y_FLAG)) return xAOD::Type::Photon;
		return (tt&TT_MUON_FLAG)? xAOD::Type::Muon : xAOD::Type::Other;
	}
	
	static constexpr xAOD::Type::ObjectType object2()
	{
		if(is2Lmix())
		{
			if((tt&TT_ELECTRON_FLAG) && (tt&TT_MUON_FLAG)) return xAOD::Type::Muon;
			if((tt&(TT_ELECTRON_FLAG|TT_MUON_FLAG)) && (tt&TT_PHOTON_FLAG)) return xAOD::Type::Photon;
			return xAOD::Type::Other;
		}
		else if(!is3Lmix()) return xAOD::Type::Other;	
		if((tt&TT_ELECTRON_FLAG) && (tt&TT_X2Y_FLAG)) return xAOD::Type::Electron;
		if((tt&TT_PHOTON_FLAG) && !(tt&TT_X2Y_FLAG)) return xAOD::Type::Photon;
		return (tt&TT_MUON_FLAG)? xAOD::Type::Muon : xAOD::Type::Other;
	}
	
	static constexpr xAOD::Type::ObjectType object()
	{
		if(mixed()) return xAOD::Type::Other;
		return object1();
	}
	
	static bool relevantFor(const Lepton& lepton) { return lepton.type()==object(); }
	static bool irrelevantFor(const Lepton& lepton) { return !relevantFor(lepton); }
	
	static constexpr bool is1L() { return (tt&TT_SINGLELEPTON_FLAG); }
	static constexpr bool is2Lnomix() { return (tt&TT_DILEPTON_FLAG) && !mixed(); }
	static constexpr bool is2Lasym() { return ((tt&TT_MASK_TYPE)==TT_DILEPTON_ASYM); }
	static constexpr bool is2Lsym() { return ((tt&TT_MASK_TYPE)==TT_DILEPTON_SYM); }
	static constexpr bool is3Lsym() { return ((tt&TT_MASK_TYPE)==TT_TRILEPTON_SYM) && !mixed(); }
	static constexpr bool is3Lhalfsym() { return ((tt&TT_MASK_TYPE)==TT_TRILEPTON_HALFSYM) && !mixed(); }
	static constexpr bool is2Lmix() { return (tt&TT_DILEPTON_FLAG) && mixed(); }
	static constexpr bool is3Lmix() { return (tt&TT_TRILEPTON_FLAG) && mixed(); }

	
	std::array<std::size_t, nDistinctLegs()> legs;
	
	explicit Trigger()
	{
		std::fill(legs.begin(), legs.end(), 0);
	}
	
	void setDefinition(const ImportData::TrigDef& def)
	{
		TriggerProperties(tt).loadLegs(def, legs);
	}
	
	template<bool=true> std::size_t operator()(void) const
	{
		static_assert(nDistinctLegs()==1, "this function is not meaningful for this type of trigger, hence should not be used.");
		return legs[0];
	}
	
	std::size_t operator()(unsigned index) const
	{
		return legs[index];
	}
	
	template<bool=true> std::size_t operator<(const Trigger& rhs) const
	{
		static_assert(is1L(), "this function is not meaningful for this type of trigger, hence should not be used.");
		return legs[0] < rhs.legs[0];
	}
	
	explicit operator bool() const
	{
		return std::all_of(legs.cbegin(), legs.cend(), [](std::size_t x)->bool{ return x; });
	}
	
	bool operator==(const Trigger& rhs) const
	{
		return legs == rhs.legs;
	}
	
	template<xAOD::Type::ObjectType obj = object()> auto cbegin() const
	{
		return legs.cbegin() + TriggerProperties(tt).cbegin_offset(obj);
	}
	
	template<xAOD::Type::ObjectType obj = object()> auto cend() const
	{
		return legs.cend() + TriggerProperties(tt).cend_offset(obj);
	}

	template<typename Trig1L> auto hiddenBy(const Trig1L trig) const
		-> typename std::enable_if<Trig1L::is1L(), bool>::type
	{
		static_assert(Trig1L::is1L(), "this function is not meaningful for this type of trigger, hence should not be used.");
		constexpr auto obj = Trig1L::object();
		return std::find(cbegin<obj>(), cend<obj>(), trig()) != cend<obj>();
	}
	
	template<typename Trig1L> auto hiddenBy(const flat_set<Trig1L>& trigs) const
		-> typename std::enable_if<Trig1L::is1L(), bool>::type
	{
		static_assert(Trig1L::is1L(), "this function is not meaningful for this type of trigger, hence should not be used.");
		return std::any_of(trigs.cbegin(), trigs.cend(), [&](Trig1L t)->bool{ return hiddenBy(t); });
	}
	
	/// Returns a pseudo trigger built only from the legs of flavour 'obj'
	/// If anti==true, uses instead only legs with flavours other than 'obj'
	template<xAOD::Type::ObjectType obj, bool anti=false> auto side() const
		-> std::conditional_t<anti ^ (CastType1::object()==obj), CastType1, CastType2>
	{
		static_assert(mixed(), "this function is not meaningful for this type of trigger, hence should not be used.");
		static_assert(obj != xAOD::Type::Other, "implementation incomplete");
		using CastType = decltype(side<obj, anti>());
		CastType trig;
		std::copy(this->cbegin<CastType::object()>(), this->cend<CastType::object()>(), trig.legs.begin());
		return trig;
	}
	/// Returns a pseudo trigger built only from the legs of the same flavour as the trigger 'TrigX'
	template<typename TrigX> auto side() const -> decltype(side<TrigX::object()>()) { return side<TrigX::object()>(); }
	/// Complement to the previous function
	template<typename TrigX> auto antiside() const -> decltype(side<TrigX::object(),true>()) { return side<TrigX::object(),true>(); }
	/// Returns a pseudo trigger of type CastType1/2
	CastType1 side1() const { return side<CastType1::object()>(); }
	CastType2 side2() const { return side<CastType2::object()>(); }

	template<typename Trig1L> auto addTo(const flat_set<Trig1L>& trigs1L) const
		-> std::enable_if_t<Trig1L::is1L() && nDistinctLegs(Trig1L::object())==1, flat_set<Trig1L>> 
	{
		static_assert(mixed(), "this function is not meaningful for this type of trigger, hence should not be used.");
		flat_set<Trig1L> trigs(trigs1L);
		trigs.insert(side<Trig1L>());
		return trigs;
	}
	
	template<typename Trig1L> static auto anonymize(const flat_set<Trig1L>& triggers)
		-> std::enable_if_t<is1L() && tt==Trig1L::type(), const flat_set<std::size_t>&>
	{
		static_assert(sizeof(Trig1L)==sizeof(std::size_t), "invalid cast if the key sizes differ");
		return reinterpret_cast<const flat_set<std::size_t>&>(triggers);
	}
	
	template<bool=true> bool symmetric() const
	{
		static_assert(!std::is_same<CastType1, UnusedArg>::value, "this function is not meaningful for this type of trigger, hence should not be used.");
		return std::all_of(legs.cbegin()+1, legs.cend(), [&](std::size_t l)->bool{ return l==legs[0]; });
	}

	template<bool=true> CastType1 to_symmetric() const
	{
		static_assert(!std::is_same<CastType1, UnusedArg>::value, "this function is not meaningful for this type of trigger, hence should not be used.");
		CastType1 trig;
		trig.legs[0] = this->legs[0];
		return trig;
	}
	
	template<bool=true> std::size_t asymLeg() const
	{
		static_assert((tt&TT_MASK_SYMMETRY)==TT_HALFSYM, "this function is not meaningful for this type of trigger, hence should not be used.");
		return legs[0];
	}
	
	template<bool=true> std::size_t symLeg() const
	{
		static_assert((tt&TT_MASK_SYMMETRY)==TT_HALFSYM, "this function is not meaningful for this type of trigger, hence should not be used.");
		return legs[1];
	}
};

template<TriggerType, TriggerType=TT_UNKNOWN> struct TriggerClass;

template<TriggerType object_flag>
struct TriggerClass<object_flag, TT_UNKNOWN>
{
	static constexpr auto addObjFlag(int tt) { return static_cast<TriggerType>(tt|object_flag); }
	
	/// single-lepton trigger (e24_lhmedium_L1EM20VH, mu24_imedium_OR_mu40...)
	struct T_1 : public Trigger<addObjFlag(TT_SINGLELEPTON_FLAG)> {};
	/// symmetric dilepton trigger (2e17_lhvloose_nod0, 2mu14...)
	struct T_2sym : public Trigger<addObjFlag(TT_DILEPTON_SYM)> {};
	/// asymmetric dilepton trigger (mu18_mu8noL1, g35_loose_g25_loose, ...):
	struct T_2asym : public Trigger<addObjFlag(TT_DILEPTON_ASYM), T_2sym> {};
	/// symmetric trilepton trigger (3mu6, ...):
	struct T_3sym : public Trigger<addObjFlag(TT_TRILEPTON_SYM)> {};
	/// half-symmetric trilepton trigger (e17_lhloose_2e9_lhloose, mu6_2mu4, ...):
	struct T_3halfsym : public Trigger<addObjFlag(TT_TRILEPTON_HALFSYM), T_3sym> {};
};

template<TriggerType object1_flag, TriggerType object2_flag>
struct TriggerClass
{
	using A = TriggerClass<object1_flag>;
	using B = TriggerClass<object2_flag>;
	static constexpr auto addObjFlags(int tt) { return static_cast<TriggerType>(tt|object1_flag|object2_flag); }
	
	/// mixed-flavour dilepton trigger (e17_lhloose_nod0_mu14, e7_lhmedium_mu24, ...):
	struct T_1_1 : public Trigger<addObjFlags(TT_DILEPTON_FLAG), typename A::T_1, typename B::T_1> {};
	/// mixed-flavour trilepton trigger type 2x_y (2e12_lhloose_mu10, ...)
	struct T_2sym_1 : public Trigger<addObjFlags(TT_TRILEPTON_SYM), typename A::T_2sym, typename B::T_1> {};
	/// mixed-flavour trilepton trigger type x_x_y
	struct T_2asym_1: public Trigger<addObjFlags(TT_TRILEPTON_ASYM), typename A::T_2asym, typename B::T_1> {};
	/// mixed-flavour trilepton trigger type x_2y (e12_lhloose_2mu10, ...)
	struct T_1_2sym: public Trigger<addObjFlags(TT_TRILEPTON_SYM|TT_X2Y_FLAG), typename A::T_1, typename B::T_2sym> {};
	/// mixed-flavour trilepton trigger type x_y_y
	struct T_1_2asym: public Trigger<addObjFlags(TT_TRILEPTON_SYM|TT_X2Y_FLAG), typename A::T_1, typename B::T_2asym> {};
};

}


#endif
