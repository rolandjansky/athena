#ifndef ATLASHEPMC_GENEVENT_H
#define ATLASHEPMC_GENEVENT_H
#include "HepMC/GenEvent.h"
namespace HepMC {
inline int signal_process_id(const GenEvent e) {
    return e.signal_process_id();
}
inline int signal_process_id(const GenEvent* e) {
    return e->signal_process_id();
}
inline void set_signal_process_id(const GenEvent* e, const int i) {
    e->set_signal_process_id(i);
}
template <class T> void set_random_states(const GenEvent* e, std::vector<T> a) {
    e->set_random_states(a);
}
template <class T> void set_signal_process_vertex(const GenEvent* e, T v) {
    e->set_signal_process_vertex(v);
}
}
#include "HepMCI/SimpleVector.h"
#endif
