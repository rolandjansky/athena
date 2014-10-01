#ifndef MODEL_H
#define MODEL_H

#include <fastjet/PseudoJet.hh>
#include <vector>

#include "Helper.h"

#include "Message.h"

#include "Parameters.h"

#include "StoredJet.h"

#include "StoredCalculations.h"

namespace Deconstruction {

  class Model {
    public:

      //virtual double weight(const std::vector<fastjet::PseudoJet> &jets, fastjet::PseudoJet &sumJet) = 0;
      virtual double weight(const StoredJet &jets, fastjet::PseudoJet &sumJet) = 0;
      virtual double hamiltonian(double pTsum = 0) = 0;
      virtual double M2(std::vector<fastjet::PseudoJet> &transformed);

      void setQsquare(double q);

    protected:
      Model(Parameters &p);
      virtual ~Model();

      //double make_splitting(const std::vector<fastjet::PseudoJet> &input, const std::vector<fastjet::PseudoJet> &leftcolpartner, const std::vector<fastjet::PseudoJet> &rightcolpartner, const std::vector<fastjet::PseudoJet> &grandmother, const Flavour::id flavor, const Flavour::id granflavor, const Flavour::id leftcolflav, const Flavour::id rightcolflav, const Shower::id shower);
      double make_splitting(const StoredJet &input, const StoredJet &leftcolpartner, const StoredJet &rightcolpartner, const StoredJet &grandmother, const Flavour::id flavor, const Flavour::id granflavor, const Flavour::id leftcolflav, const Flavour::id rightcolflav, const Shower::id shower);

      //double start_splitting(const std::vector<fastjet::PseudoJet> &input, const Flavour::id flavour, const Shower::id shower);
      double start_splitting(const StoredJet &input, const Flavour::id flavour, const Shower::id shower);

      Parameters &param() const;

      double m_Qsquare;
      double topmass;

      //static std::vector<fastjet::PseudoJet> empty;

      // these are constants within a run ... maybe they should be elsewhere?
      double nrbreitw;
      double wwidth;
      double delta_wmass;
      double topwidth;
      double delta_topmass;
      double dR2_outside;

      double lambda_mu;

  public:
  static int level;
  static int btopshower_level;
  static int tshower_level;
  static int Wshower_level;

  static int initlevel;
  static int binitlevel;
  static int tinitlevel;
  static int Winitlevel;

  static StoredCalculations m_calc;

  protected:

    bool m_useBtag;
    double m_tagprob;
    double m_fakeprob;


    private:
      Parameters &m_param;

  // Here all quark -> quark g splittings are considered. 
  // q -> q g with and without massive color con. partner
  // b -> b g right after top decay ....
  double HQuark(const fastjet::PseudoJet &tmptot,
                const fastjet::PseudoJet & tmpleftcol,
                const fastjet::PseudoJet & tmprightcol,
                const fastjet::PseudoJet & tmpL,
                const fastjet::PseudoJet & tmpR,
                const fastjet::PseudoJet & tmpgrandm,
                const Flavour::id flavor,
                const Flavour::id granflavor,
                const Flavour::id tmpleftcolflav,
                const Flavour::id tmprightcolflav,
                const Shower::id shower);

  double HGluonQQ(const fastjet::PseudoJet & tmptot,
                  const fastjet::PseudoJet & tmpmleftcol,
                  const fastjet::PseudoJet & tmpmrightcol,
                  const fastjet::PseudoJet & tmpL,
                  const fastjet::PseudoJet & tmpR,
                  const fastjet::PseudoJet & tmpgrandm,
                  const Flavour::id flavour,
                  const Flavour::id granflavour,
                  const Flavour::id tmpleftcolflav,
                  const Flavour::id tmprightcolflav,
                  const Shower::id shower);

double HGluonBB(const fastjet::PseudoJet & tmptot, 
				 const fastjet::PseudoJet & tmpmleftcol, 
				 const fastjet::PseudoJet & tmpmrightcol,
				 const fastjet::PseudoJet & tmpL,
				 const fastjet::PseudoJet & tmpR, 
				 const fastjet::PseudoJet & tmpgrandm,
				 const Flavour::id flavor,
				 const Flavour::id granflavor,
				 const Flavour::id tmpleftcolflav,
				 const Flavour::id tmprightcolflav,
				 const Shower::id shower);

double HGluonGG(const fastjet::PseudoJet & tmptot, 
				    const fastjet::PseudoJet & tmpmleftcol, 
				    const fastjet::PseudoJet & tmpmrightcol,
				    const fastjet::PseudoJet & tmpL,
				    const fastjet::PseudoJet & tmpR, 
				    const fastjet::PseudoJet & tmpgrandm,
				    const Flavour::id flavor,
				    const Flavour::id granflavor,
				    const Flavour::id tmpleftcolflav,
				    const Flavour::id tmprightcolflav,
				    const Shower::id shower);


double sudakovGluon(const fastjet::PseudoJet & tmptot, 
					const fastjet::PseudoJet & tmpmleftcol, 
					const fastjet::PseudoJet & tmpmrightcol,
					const fastjet::PseudoJet & tmpgrandm,
					const Flavour::id flavor,
					const Flavour::id granflavor,
					const Flavour::id tmpleftcolflav,
					const Flavour::id tmprightcolflav,
					const Shower::id shower);

double sudakovQuark(const fastjet::PseudoJet & tmptot, 
					const fastjet::PseudoJet & tmpmleftcol, 
					const fastjet::PseudoJet & tmpmrightcol,
					const fastjet::PseudoJet & tmpgrandm,
					const Flavour::id flavor,
					const Flavour::id granflavor,
					const Flavour::id tmpleftcolflav,
					const Flavour::id tmprightcolflav,
					const Shower::id shower);

double sudakovTopEND(const fastjet::PseudoJet & tmptot, 
				      const fastjet::PseudoJet & tmpmleftcol, 
				      const fastjet::PseudoJet & tmpmrightcol,
				      const fastjet::PseudoJet & tmpgrandm,
				      const Flavour::id flavor,
				      const Flavour::id granflavor,
				      const Flavour::id tmpleftcolflav,
				      const Flavour::id tmprightcolflav,
				      const Shower::id shower);

double sudakov(const fastjet::PseudoJet & tmptot, 
				   const fastjet::PseudoJet & tmpmleftcol, 
				   const fastjet::PseudoJet & tmpmrightcol,
				   const fastjet::PseudoJet & tmpgrandm,
				   const Flavour::id flavor,
				   const Flavour::id granflavor,
				   const Flavour::id tmpleftcolflav,
				   const Flavour::id tmprightcolflav,
				   const Shower::id shower);



  };

}

#endif

