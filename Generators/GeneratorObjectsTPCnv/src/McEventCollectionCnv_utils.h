namespace {
  // helper method to compute the number of particles and vertices in a
  // whole McEventCollection
  std::pair<unsigned int,unsigned int>
  nbrParticlesAndVertices( const McEventCollection* mcEvents ) {
    unsigned int nParts = 0;
    unsigned int nVerts = 0;
    const McEventCollection::const_iterator itrEnd = mcEvents->end();
    for ( McEventCollection::const_iterator itr = mcEvents->begin();
          itr != itrEnd;
          ++itr ) {
#ifdef HEPMC3
      nParts += (*itr)->particles().size();
      nVerts += (*itr)->vertices().size();
#else
      nParts += (*itr)->particles_size();
      nVerts += (*itr)->vertices_size();
#endif
    }

    return std::make_pair( nParts, nVerts );
  }
}