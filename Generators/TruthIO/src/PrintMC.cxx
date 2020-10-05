/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// PrintMC - dump details of MC events
// Updated 29/10/2009 by Andy Buckley <andy.buckley@cern.ch>
// Updated 18.06.2020 by <andrii.verbytskyi@mpp.mpg.de>
#include "TruthIO/PrintMC.h"
#include "GeneratorObjects/McEventCollection.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"
#include "AtlasHepMC/GenEvent.h"


inline void drawLine(std::ostream& os) {
  os << std::string(80, '_') << "\n" << std::endl;
}


PrintMC::PrintMC(const std::string& name, ISvcLocator* pSvcLocator)
  : GenBase(name, pSvcLocator)
{
  // Declare JO interface
  declareProperty("VerboseOutput", m_VerboseOutput=true, "Print out detailed information or not");
  declareProperty("PrintStyle", m_printsty="Barcode", "Type of info to print out: 'Barcode' or 'Vertex'");
  declareProperty("VertexInfo", m_vertexinfo=false, "Print out vertex information");
  declareProperty("TrustHepMC",  m_trustHepMC=false, "Use the event numbers in the HepMC record to choose the events to print");
  declareProperty("FirstEvent", m_firstEvt=1, "Lowest event number to print");
  declareProperty("LastEvent", m_lastEvt=100000000, "Highest event number to print");
}


StatusCode PrintMC::initialize() {
  CHECK(GenBase::initialize());
  // Check settings
  if (m_printsty != "Barcode" && m_printsty != "Vertex") {
    ATH_MSG_ERROR("Unknown PrintStyle: " << m_printsty << "! Choose Barcode/Vertex");
    return StatusCode::FAILURE;
  }

  // Check that last > first
  if (m_lastEvt < m_firstEvt) m_lastEvt = m_firstEvt;

  return StatusCode::SUCCESS;
}



/// @todo Avoid use of unprotected std::cout: stringstream + MsgStream would be better
StatusCode PrintMC::execute() {
  // If output already turned off by passing last dumped event, just return
  /// @todo I get the feeling VerboseOutput is being abused here...
  if (!m_VerboseOutput) return StatusCode::SUCCESS;

  // Loop over all events in McEventCollection

  for (McEventCollection::const_iterator itr = events_const()->begin(); itr != events_const()->end(); ++itr) {

    // Get event number from HepMC
    const HepMC::GenEvent* evt = *itr;
    int evtnum = evt->event_number();
    // Override with evtnum from Athena if enabled and functional
    if (!m_trustHepMC) {
      const EventInfo* evtinfo;
      const StatusCode sc =  evtStore()->retrieve(evtinfo);
      if (sc.isSuccess()) {
        evtnum = evtinfo->event_ID()->event_number();
      }
    }

    // Return immediately if event number is outside the print range
    if (evtnum < m_firstEvt) {
      return StatusCode::SUCCESS;
    }
    if (evtnum > m_lastEvt) {
      /// @todo Looks like misuse of the verbose output flag
      m_VerboseOutput = false;
      return StatusCode::SUCCESS;
    }

    // Printout header
    ATH_MSG_INFO(">>> PrintMC from execute");

    // VERTEX format
    /// @todo Isn't this if (m_VerboseOutput... redundant?
    if (m_VerboseOutput && m_printsty == "Vertex") {
     HepMC::Print::line(std::cout, *evt); // standard HepMC dump of vertex list
    }
    // BARCODE format
    /// @todo Isn't this if (m_VerboseOutput... redundant?
    else if (m_VerboseOutput && m_printsty == "Barcode") {
      drawLine(std::cout);
#ifdef HEPMC3
      std::cout << "GenEvent: #" << evt->event_number()
                << " ID=" << HepMC::signal_process_id(evt)
                << " SignalProcessGenVertex Barcode: "
                << ( HepMC::signal_process_vertex(evt) ? HepMC::barcode(HepMC::signal_process_vertex(evt)) : 0 ) << "\n";
      std::cout << " Entries this event: " << evt->vertices().size() << " vertices, "
                << evt->particles().size() << " particles.\n";

      if (evt->heavy_ion()) {
        std::cout << " HeavyIon: jatt=" << evt->heavy_ion()->Ncoll_hard
                  << " np=" << evt->heavy_ion()->Npart_proj
                  << " nt=" << evt->heavy_ion()->Npart_targ
                  << " ncoll=" << evt->heavy_ion()->Ncoll
                  << " specn=" << evt->heavy_ion()->spectator_neutrons
                  << " specp=" << evt->heavy_ion()->spectator_protons
                  << " n01=" << evt->heavy_ion()->N_Nwounded_collisions
                  << " n10=" << evt->heavy_ion()->Nwounded_N_collisions
                  << " n11=" << evt->heavy_ion()->Nwounded_Nwounded_collisions
                  << " impact=" << evt->heavy_ion()->impact_parameter
                  << " evplane=" << evt->heavy_ion()->event_plane_angle
                  << " ecc=" << evt->heavy_ion()->eccentricity
                  << " sigmaNNinel=" << evt->heavy_ion()->sigma_inel_NN
		  << std::endl;
      }
      else {
        std::cout << "HeavyIon: EMPTY"
		  << std::endl;
      }


      // Weights
      std::cout << " Weights(" << evt->weights().size() << ")=";
      for ( auto wgt = evt->weights().begin();
            wgt != evt->weights().end(); wgt++ ) { std::cout << *wgt << " "; }
      std::cout << "\n";
      std::cout << " EventScale " << (evt->attribute<HepMC3::DoubleAttribute>("event_scale")? evt->attribute<HepMC3::DoubleAttribute>("event_scale")->value():0.0)
                << " [energy] \t alphaQCD=" << (evt->attribute<HepMC3::DoubleAttribute>("alphaQCD")? evt->attribute<HepMC3::DoubleAttribute>("alphaQCD")->value():0.0)
                << "\t alphaQED=" << (evt->attribute<HepMC3::DoubleAttribute>("alphaQED")? evt->attribute<HepMC3::DoubleAttribute>("alphaQED")->value():0.0) << std::endl;

      if (evt->pdf_info()) {
        std::cout << "PdfInfo: id1=" << evt->pdf_info()->parton_id[0]
                  << " id2=" << evt->pdf_info()->parton_id[1]
                  << " x1=" << evt->pdf_info()->x[0]
                  << " x2=" << evt->pdf_info()->x[1]
                  << " q=" << evt->pdf_info()->scale
                  << " xpdf1=" << evt->pdf_info()->pdf_id[0]
                  << " xpdf2=" << evt->pdf_info()->pdf_id[1]
		  << std::endl;
      }
      else {
        std::cout << "PdfInfo: EMPTY"
		  << std::endl;
      }
#else
      std::cout << "GenEvent: #" << evt->event_number()
                << " ID=" << HepMC::signal_process_id(evt)
                << " SignalProcessGenVertex Barcode: "
                << ( HepMC::signal_process_vertex(evt) ? HepMC::barcode(HepMC::signal_process_vertex(evt)) : 0 ) << "\n";
      std::cout << " Entries this event: " << evt->vertices_size() << " vertices, "
                << evt->particles_size() << " particles.\n";

      if (evt->heavy_ion()) {
        std::cout << " HeavyIon: jatt=" << evt->heavy_ion()->Ncoll_hard()
                  << " np=" << evt->heavy_ion()->Npart_proj()
                  << " nt=" << evt->heavy_ion()->Npart_targ()
                  << " ncoll=" << evt->heavy_ion()->Ncoll()
                  << " specn=" << evt->heavy_ion()->spectator_neutrons()
                  << " specp=" << evt->heavy_ion()->spectator_protons()
                  << " n01=" << evt->heavy_ion()->N_Nwounded_collisions()
                  << " n10=" << evt->heavy_ion()->Nwounded_N_collisions()
                  << " n11=" << evt->heavy_ion()->Nwounded_Nwounded_collisions()
                  << " impact=" << evt->heavy_ion()->impact_parameter()
                  << " evplane=" << evt->heavy_ion()->event_plane_angle()
                  << " ecc=" << evt->heavy_ion()->eccentricity()
                  << " sigmaNNinel=" << evt->heavy_ion()->sigma_inel_NN()
		  << std::endl;
      }
      else {
        std::cout << "HeavyIon: EMPTY"
		  << std::endl;
      }


      // Weights
      std::cout << " Weights(" << evt->weights().size() << ")=";
      for ( HepMC::WeightContainer::const_iterator wgt = evt->weights().begin();
            wgt != evt->weights().end(); wgt++ ) { std::cout << *wgt << " "; }
      std::cout << "\n";
      std::cout << " EventScale " << evt->event_scale()
                << " [energy] \t alphaQCD=" << evt->alphaQCD()
                << "\t alphaQED=" << evt->alphaQED() << std::endl;

      if (evt->pdf_info()) {
        std::cout << "PdfInfo: id1=" << evt->pdf_info()->id1()
                  << " id2=" << evt->pdf_info()->id2()
                  << " x1=" << evt->pdf_info()->x1()
                  << " x2=" << evt->pdf_info()->x2()
                  << " q=" << evt->pdf_info()->scalePDF()
                  << " xpdf1=" << evt->pdf_info()->pdf1()
                  << " xpdf2=" << evt->pdf_info()->pdf2()
		  << std::endl;
      }
      else {
        std::cout << "PdfInfo: EMPTY"
		  << std::endl;
      }
#endif

      // Print a legend to describe the particle info
      char particle_legend[120];
      sprintf( particle_legend,"  %9s %8s %-15s %4s %8s %8s   (%9s,%9s,%9s,%9s,%9s)",
               "Barcode","PDG ID","Name","Stat","ProdVtx","DecayVtx","Px","Py","Pz","E ","m");
      std::cout << std::endl;
      std::cout << "                                    GenParticle Legend\n" << particle_legend << "\n";
      if (m_vertexinfo) {
        sprintf( particle_legend," %60s (%9s,%9s,%9s,%9s)"," ","Vx","Vy","Vz","Vct ");
        std::cout << particle_legend << std::endl;
      }
      drawLine(std::cout);

      // Print all particles
      for (auto  p: *evt) {
        int p_bcode = HepMC::barcode(p);
        int p_pdg_id = p->pdg_id();
        HepMC::FourVector mom=p->momentum();
        int p_stat = p->status();
        int p_prodvtx = p->production_vertex()?HepMC::barcode(p->production_vertex()):0;
        int p_endvtx = p->end_vertex()?HepMC::barcode(p->end_vertex()):0;
        HepMC::FourVector prodvtx=p->production_vertex()?p->production_vertex()->position():HepMC::FourVector(0.0,0.0,0.0,0.0);
        // Access the PDG table to get the particle name (and mass?)
        std::string sname;
        double p_mass = p->generated_mass();
        const HepPDT::ParticleData* ap = particleData(std::abs(p_pdg_id));
        if (!ap) {
          ATH_MSG_DEBUG("PID " << std::abs(p_pdg_id) << " is not in particle data table");
        } else {
          const double p_charge = ap->charge() * (p_pdg_id < 0 ? -1 : 1); // assuming that charged leptons are in the PDT...
          // Build particle name string
          sname = ap->name();
          if (p_charge < 0) {
            const size_t plusidx = sname.rfind("+");
            if (plusidx != std::string::npos) {
              sname.replace(plusidx, 1, "-");
            }
          }
        }
        // PDG table missing or exceptions
        // quarks & gluons
        if      (p_pdg_id == 21) sname="g";
        else if (p_pdg_id == 1)  sname="d";
        else if (p_pdg_id == -1) sname="d~";
        else if (p_pdg_id == 2)  sname="u";
        else if (p_pdg_id == -2) sname="u~";
        else if (p_pdg_id == 3)  sname="s";
        else if (p_pdg_id == -3) sname="s~";
        else if (p_pdg_id == 4)  sname="c";
        else if (p_pdg_id == -4) sname="c~";
        else if (p_pdg_id == 5)  sname="b";
        else if (p_pdg_id == -5) sname="b~";
        else if (p_pdg_id == 6)  sname="t";
        else if (p_pdg_id == -6) sname="t~";
        // shower-specific
        else if (p_pdg_id == 91) sname="cluster";
        else if (p_pdg_id == 92) sname="string";
        else if (p_pdg_id == 9922212) sname="remn";
        else if (p_pdg_id == 2101) sname="ud";
        else if (p_pdg_id == 2203) sname="uu";

        // Calc mass if missing
        /// @todo Is there no better way to detect unspecified masses than == 0? Final state photons *should* be 0
        if (p_mass == 0 && (p_stat == 2 || (p_stat != 1 && p_pdg_id != 22))) {
          p_mass = mom.m();
        }

        const char* p_name = sname.c_str() ;
        char particle_entries[120];
        sprintf(particle_entries, "  %9i %8i %-15s %4i %8i %8i   (%+9.3g,%+9.3g,%+9.3g,%+9.3g,%9.3g)",
                p_bcode, p_pdg_id, p_name, p_stat, p_prodvtx, p_endvtx, mom.px(), mom.py(),mom.pz(), mom.e(), p_mass);
        std::cout << particle_entries << "\n";
        if (m_vertexinfo) {
          sprintf(particle_entries," %60s (%+9.3g,%+9.3g,%+9.3g,%+9.3g)"," ",prodvtx.x(), prodvtx.y(),prodvtx.z(), prodvtx.t());
          std::cout << particle_entries << "\n";
        }
      }

    }

  }
  // End of execution for each event
  return StatusCode::SUCCESS;
}
