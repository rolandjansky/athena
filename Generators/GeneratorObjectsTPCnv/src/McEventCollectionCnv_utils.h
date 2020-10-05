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
#ifdef HEPMC3
std::vector<std::string>   name_index_map_to_names(const std::map<std::string, unsigned long int> &input)
{
std::vector<std::string> result;
std::vector<std::pair<std::string, unsigned long int> > sorted; 
for (auto el: input) sorted.push_back(el);
std::sort(sorted.begin(), sorted.end(),[](std::pair<std::string, unsigned long int> a, std::pair<std::string, unsigned long int> b) {return a.second > b.second; }); 
for (auto a: sorted) result.push_back(a.first);
return result;
}

std::map<std::string, unsigned long int> names_to_name_index_map(const  std::vector<std::string>  &input )
{
std::map<std::string, unsigned long int> result;
unsigned long int i=0;
for (auto a: input) {result[a]=i; i++;}
return result;
}

std::vector<std::pair<int,int> > vector_to_vector_int_int(const  std::vector<int>  &input )
{
std::vector<std::pair<int,int> > result;
unsigned long int i=0;
for (auto a: input) {result.push_back(std::pair<int,int>(a,i)); i++;}
return result;
}
#endif
}
